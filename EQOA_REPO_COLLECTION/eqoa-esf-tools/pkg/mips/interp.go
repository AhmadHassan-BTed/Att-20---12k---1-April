// Package mips implements a lightweight R5900 MIPS interpreter for executing
// PS2 ESF parser functions natively. Only implements the instruction subset
// used by SUPPORT module Parse*Obj functions (~30 ops).
package mips

import (
	"encoding/binary"
	"fmt"
	"math"
)

// Interp is a lightweight MIPS R5900 interpreter.
type Interp struct {
	code     []byte    // EE memory dump (all code in-place)
	regs     [32]int64 // GPR ($zero..$ra)
	fregs    [32]float32
	pc       uint32
	hi, lo   int64
	fpCC     bool                    // FPU condition code (PCSX2: fpuRegs.fprc[31] bit 23)
	writes   map[uint32]byte // sparse writable memory overlay
	MaxSteps int
	Verbose  bool

	// ESF reader — flat (ESFStream) or tree (ESFTreeStream)
	ESF    *ESFStream    // kept for backward compat with RunParser
	Reader ESFReader     // active reader (either ESF or tree stream)

	// Stats
	Steps       int
	Intercepted int

	// PS2 runtime state (heap + fake objects)
	heap    heapState
	runtime runtimeState
	vu0          vu0State     // VU0 vector unit (128-bit SIMD)
	packetStream *PacketStream // CLIENT opcode packet data (nil for ESF mode)
	drdpStream   *DRDPStream   // DRDP protocol packet data (nil for ESF/CLIENT mode)

	// Debug (ported from PCSX2 DebugTools)
	breakpoints      []Breakpoint
	memChecks        []*MemCheck
	callTraceEnabled bool
	callTrace        []CallEntry
}

// New creates an interpreter loaded with EE memory.
func New(eeDump []byte) *Interp {
	return &Interp{
		code:     eeDump,
		writes:   make(map[uint32]byte, 4096),
		MaxSteps: 10_000_000,
	}
}

// Reset clears all state for a new run.
func (m *Interp) Reset() {
	m.regs = [32]int64{}
	m.fregs = [32]float32{}
	m.pc = 0
	m.hi = 0
	m.lo = 0
	// Preserve writes if pre-set (e.g., zeroFakePointers called before Run)
	if m.writes == nil {
		m.writes = make(map[uint32]byte, 4096)
	}
	m.Steps = 0
	m.Intercepted = 0
}

// Run executes from entryAddr with the given ESF data.
// args are placed in $a0, $a1, $a2, $a3, $t0, $t1.
// Returns $v0.
func (m *Interp) Run(entryAddr uint32, esf *ESFStream, args ...uint32) int32 {
	m.Reset()
	m.ESF = esf
	if esf != nil {
		m.Reader = esf
	}
	m.pc = entryAddr

	// Stack
	sp := uint32(0x01FF0000)
	m.wReg(29, int64(sp))
	m.wReg(31, 0) // $ra = 0 sentinel

	// Args
	argRegs := []int{4, 5, 6, 7, 8, 9}
	for i, v := range args {
		if i < len(argRegs) {
			m.wReg(argRegs[i], int64(v))
		}
	}

	for m.Steps < m.MaxSteps {
		if m.pc == 0 {
			if m.Verbose {
				fmt.Printf("PC=0 sentinel at step %d, $ra=0x%08X, $v0=%d, prev_pc was set by jr instruction\n", m.Steps, uint32(m.regs[31]), int32(m.regs[2]))
			}
			break
		}
		// PCSX2-style breakpoint check (before instruction execution)
		if len(m.breakpoints) > 0 && m.checkBreakpoints() {
			break
		}
		insn := m.load32(m.pc)
		if !m.exec(insn) {
			if m.Verbose {
				fmt.Printf("UNHANDLED at 0x%08X: 0x%08X\n", m.pc, insn)
			}
			break
		}
		m.Steps++
	}

	return int32(m.regs[2])
}

// RunCall executes a function without resetting interpreter state.
// Used for calling Init functions during setup.
// Steps used by RunCall do NOT count toward the main Run's step limit.
func (m *Interp) RunCall(funcAddr uint32, args ...uint32) int32 {
	oldPC := m.pc
	oldRA := m.regs[31]
	oldSteps := m.Steps

	m.pc = funcAddr
	m.wReg(31, 0) // sentinel
	m.wReg(29, 0x01FE0000-0x1000) // separate stack for RunCall (below thisAddr)

	argRegs := []int{4, 5, 6, 7, 8, 9}
	for i, v := range args {
		if i < len(argRegs) {
			m.wReg(argRegs[i], int64(v))
		}
	}

	for i := 0; i < 1000000; i++ {
		if m.pc == 0 {
			break
		}
		insn := m.load32(m.pc)
		if !m.exec(insn) {
			if m.Verbose {
				fmt.Printf("RunCall: UNHANDLED at 0x%08X: 0x%08X (step %d)\n", m.pc, insn, i)
			}
			break
		}
	}

	result := int32(m.regs[2])

	m.pc = oldPC
	m.regs[31] = oldRA
	m.Steps = oldSteps // don't count Init steps against main Run limit

	return result
}

// --- Register access ---

func (m *Interp) rReg(r int) int64 {
	if r == 0 {
		return 0
	}
	return m.regs[r]
}

func (m *Interp) wReg(r int, v int64) {
	if r != 0 {
		m.regs[r] = v
	}
}

func (m *Interp) rReg32(r int) int64 {
	v := m.rReg(r) & 0xFFFFFFFF
	if v&0x80000000 != 0 {
		v |= ^int64(0xFFFFFFFF)
	}
	return v
}

func (m *Interp) wReg32(r int, v int64) {
	v32 := v & 0xFFFFFFFF
	if v32&0x80000000 != 0 {
		v32 |= ^int64(0xFFFFFFFF)
	}
	m.wReg(r, v32)
}

// --- Memory access ---

func (m *Interp) load8(addr uint32) byte {
	if v, ok := m.writes[addr]; ok {
		return v
	}
	if int(addr) < len(m.code) {
		return m.code[addr]
	}
	return 0
}

func (m *Interp) load16(addr uint32) uint16 {
	return uint16(m.load8(addr)) | uint16(m.load8(addr+1))<<8
}

func (m *Interp) load32(addr uint32) uint32 {
	return uint32(m.load8(addr)) | uint32(m.load8(addr+1))<<8 |
		uint32(m.load8(addr+2))<<16 | uint32(m.load8(addr+3))<<24
}

func (m *Interp) store8(addr uint32, v byte) {
	m.writes[addr] = v
	if len(m.memChecks) > 0 {
		m.checkMemWrite(addr, 1, uint32(v))
	}
}

func (m *Interp) store16(addr uint32, v uint16) {
	m.writes[addr] = byte(v)
	m.writes[addr+1] = byte(v >> 8)
	if len(m.memChecks) > 0 {
		m.checkMemWrite(addr, 2, uint32(v))
	}
}

func (m *Interp) store32(addr uint32, v uint32) {
	m.writes[addr] = byte(v)
	m.writes[addr+1] = byte(v >> 8)
	m.writes[addr+2] = byte(v >> 16)
	m.writes[addr+3] = byte(v >> 24)
	if len(m.memChecks) > 0 {
		m.checkMemWrite(addr, 4, v)
	}
}

func (m *Interp) loadFloat(addr uint32) float32 {
	return math.Float32frombits(m.load32(addr))
}

func (m *Interp) storeFloat(addr uint32, v float32) {
	m.store32(addr, math.Float32bits(v))
}

// --- Instruction execution ---

func (m *Interp) exec(insn uint32) bool {
	if insn == 0 { // NOP
		m.pc += 4
		return true
	}

	op := (insn >> 26) & 0x3F
	rs := int((insn >> 21) & 0x1F)
	rt := int((insn >> 16) & 0x1F)
	rd := int((insn >> 11) & 0x1F)
	sa := int((insn >> 6) & 0x1F)
	funct := insn & 0x3F
	imm := insn & 0xFFFF
	simm := int64(int16(imm))
	target := insn & 0x3FFFFFF

	next := m.pc + 4

	switch op {
	case 0: // SPECIAL
		return m.execSpecial(rs, rt, rd, sa, funct, next)
	case 1: // REGIMM
		return m.execRegimm(rs, rt, simm, next)
	case 2: // J
		dst := (m.pc & 0xF0000000) | (target << 2)
		m.execDelay(next)
		m.pc = dst
		return true
	case 3: // JAL
		callTarget := (m.pc & 0xF0000000) | (target << 2)
		m.wReg(31, int64(next+4))
		// PCSX2 doBranch: delay slot executes BEFORE the target function.
		// Critical for cases like: jal Read_Ri / lw $fp, 280($sp)
		// where the delay slot must read the OLD value at sp+280.
		delayInsn := m.load32(next)
		m.exec(delayInsn) // delay slot first (matches PCSX2 _doBranch_shared)
		if m.handleJAL(callTarget) {
			m.pc = next + 4
		} else {
			m.pc = callTarget
		}
		return true

	// Branches
	case 4: // BEQ
		if m.rReg(rs) == m.rReg(rt) {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (simm << 2))
		} else {
			m.execDelay(next)
			m.pc = next + 4
		}
		return true
	case 5: // BNE
		if m.rReg(rs) != m.rReg(rt) {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (simm << 2))
		} else {
			m.execDelay(next)
			m.pc = next + 4
		}
		return true
	case 6: // BLEZ
		if m.rReg32(rs) <= 0 {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (simm << 2))
		} else {
			m.execDelay(next)
			m.pc = next + 4
		}
		return true
	case 7: // BGTZ
		if m.rReg32(rs) > 0 {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (simm << 2))
		} else {
			m.execDelay(next)
			m.pc = next + 4
		}
		return true

	// Branch-likely
	case 20: // BEQL
		if m.rReg(rs) == m.rReg(rt) {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (simm << 2))
		} else {
			m.pc = next + 4
		}
		return true
	case 21: // BNEL
		if m.rReg(rs) != m.rReg(rt) {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (simm << 2))
		} else {
			m.pc = next + 4
		}
		return true

	// Immediate arithmetic
	case 8, 9: // ADDI, ADDIU
		m.wReg32(rt, (m.rReg32(rs)+simm)&0xFFFFFFFF)
		m.pc = next
		return true
	case 10: // SLTI
		if m.rReg32(rs) < simm {
			m.wReg32(rt, 1)
		} else {
			m.wReg32(rt, 0)
		}
		m.pc = next
		return true
	case 11: // SLTIU
		if uint64(m.rReg(rs))&0xFFFFFFFF < uint64(imm) {
			m.wReg32(rt, 1)
		} else {
			m.wReg32(rt, 0)
		}
		m.pc = next
		return true
	case 12: // ANDI
		m.wReg(rt, m.rReg(rs)&int64(imm))
		m.pc = next
		return true
	case 13: // ORI
		m.wReg(rt, m.rReg(rs)|int64(imm))
		m.pc = next
		return true
	case 14: // XORI
		m.wReg(rt, m.rReg(rs)^int64(imm))
		m.pc = next
		return true
	case 15: // LUI
		m.wReg32(rt, int64(imm)<<16)
		m.pc = next
		return true

	// COP1 (FPU)
	case 17:
		return m.execCOP1(insn, rs, rt, rd, sa, funct, next)

	// COP2 (VU0 macro mode)
	case 18:
		return m.execCOP2(insn, next)

	// Loads
	case 32: // LB
		addr := uint32(m.rReg32(rs) + simm)
		v := m.load8(addr)
		if v&0x80 != 0 {
			m.wReg32(rt, int64(v)|^0xFF)
		} else {
			m.wReg32(rt, int64(v))
		}
		m.pc = next
		return true
	case 33: // LH
		addr := uint32(m.rReg32(rs) + simm)
		v := m.load16(addr)
		if v&0x8000 != 0 {
			m.wReg32(rt, int64(v)|^0xFFFF)
		} else {
			m.wReg32(rt, int64(v))
		}
		m.pc = next
		return true
	case 35: // LW
		addr := uint32(m.rReg32(rs) + simm)
		m.wReg32(rt, int64(int32(m.load32(addr))))
		m.pc = next
		return true
	case 36: // LBU
		addr := uint32(m.rReg32(rs) + simm)
		m.wReg32(rt, int64(m.load8(addr)))
		m.pc = next
		return true
	case 37: // LHU
		addr := uint32(m.rReg32(rs) + simm)
		m.wReg32(rt, int64(m.load16(addr)))
		m.pc = next
		return true

	// Stores
	case 40: // SB
		m.store8(uint32(m.rReg32(rs)+simm), byte(m.rReg(rt)))
		m.pc = next
		return true
	case 41: // SH
		m.store16(uint32(m.rReg32(rs)+simm), uint16(m.rReg(rt)))
		m.pc = next
		return true
	case 43: // SW
		m.store32(uint32(m.rReg32(rs)+simm), uint32(m.rReg(rt)))
		m.pc = next
		return true

	// FPU loads/stores
	case 49: // LWC1
		addr := uint32(m.rReg32(rs) + simm)
		m.fregs[rt] = m.loadFloat(addr)
		m.pc = next
		return true
	case 57: // SWC1
		addr := uint32(m.rReg32(rs) + simm)
		m.storeFloat(addr, m.fregs[rt])
		m.pc = next
		return true

	// R5900 SQ/LQ (128-bit → treat as 32-bit for register save/restore)
	case 0x1F: // SQ (128-bit store, PCSX2: memWrite128, addr aligned to 16)
		// PS2 R5900: stores full 128-bit register. We store low 64 bits (sufficient
		// for C code register save/restore which only uses the low 32-64 bits).
		addr := uint32(m.rReg32(rs)+simm) & ^uint32(0xF) // PCSX2: addr & ~0xf
		v := m.rReg(rt)
		m.store32(addr, uint32(v))
		m.store32(addr+4, uint32(v>>32))
		// Upper 64 bits (quadword) zeroed — R5900 stores all 128 but parsers
		// only use low 64. PCSX2 stores cpuRegs.GPR.r[rt].UQ (full 128).
		m.store32(addr+8, 0)
		m.store32(addr+12, 0)
		m.pc = next
		return true
	case 0x1E: // LQ (128-bit load, PCSX2: memRead128, addr aligned to 16)
		addr := uint32(m.rReg32(rs)+simm) & ^uint32(0xF) // PCSX2: addr & ~0xf
		lo := int64(m.load32(addr))
		hi := int64(m.load32(addr + 4))
		m.wReg(rt, lo|(hi<<32))
		m.pc = next
		return true

	// 64-bit LD/SD
	case 55: // LD
		addr := uint32(m.rReg32(rs) + simm)
		lo := int64(m.load32(addr))
		hi := int64(m.load32(addr + 4))
		m.wReg(rt, lo|(hi<<32))
		m.pc = next
		return true
	case 63: // SD
		addr := uint32(m.rReg32(rs) + simm)
		v := m.rReg(rt)
		m.store32(addr, uint32(v))
		m.store32(addr+4, uint32(v>>32))
		m.pc = next
		return true

	// VU0 128-bit memory operations (top-level opcodes, NOT inside COP2)
	case 54: // LQC2 — Load Quadword to COP2: VF[ft] = mem128[GPR[rs]+imm]
		addr := uint32(m.rReg32(rs)+simm) & ^uint32(0xF) // 16-byte aligned
		m.vu0.vf[rt][0] = math.Float32frombits(m.load32(addr))
		m.vu0.vf[rt][1] = math.Float32frombits(m.load32(addr + 4))
		m.vu0.vf[rt][2] = math.Float32frombits(m.load32(addr + 8))
		m.vu0.vf[rt][3] = math.Float32frombits(m.load32(addr + 12))
		if rt == 0 { m.vu0.vf[0] = [4]float32{0, 0, 0, 1.0} } // VF0 constant
		m.pc = next
		return true
	case 62: // SQC2 — Store Quadword from COP2: mem128[GPR[rs]+imm] = VF[ft]
		addr := uint32(m.rReg32(rs)+simm) & ^uint32(0xF) // 16-byte aligned
		m.store32(addr, math.Float32bits(m.vu0.vf[rt][0]))
		m.store32(addr+4, math.Float32bits(m.vu0.vf[rt][1]))
		m.store32(addr+8, math.Float32bits(m.vu0.vf[rt][2]))
		m.store32(addr+12, math.Float32bits(m.vu0.vf[rt][3]))
		m.pc = next
		return true

	// LWL/LWR (unaligned load — from PCSX2 R5900OpcodeImpl.cpp)
	case 34: // LWL
		addr := uint32(m.rReg32(rs) + simm)
		shift := addr & 3
		mem := m.load32(addr & ^uint32(3))
		// PCSX2: LWL_MASK = {0xffffff, 0xffff, 0xff, 0}, LWL_SHIFT = {24, 16, 8, 0}
		lwlMask := [4]uint32{0x00FFFFFF, 0x0000FFFF, 0x000000FF, 0x00000000}
		lwlShift := [4]uint32{24, 16, 8, 0}
		result := (uint32(m.rReg(rt)) & lwlMask[shift]) | (mem << lwlShift[shift])
		m.wReg32(rt, int64(int32(result)))
		m.pc = next
		return true
	case 38: // LWR
		addr := uint32(m.rReg32(rs) + simm)
		shift := addr & 3
		mem := m.load32(addr & ^uint32(3))
		// PCSX2: LWR_MASK = {0, 0xff000000, 0xffff0000, 0xffffff00}, LWR_SHIFT = {0, 8, 16, 24}
		lwrMask := [4]uint32{0x00000000, 0xFF000000, 0xFFFF0000, 0xFFFFFF00}
		lwrShift := [4]uint32{0, 8, 16, 24}
		result := (uint32(m.rReg(rt)) & lwrMask[shift]) | (mem >> lwrShift[shift])
		m.wReg32(rt, int64(int32(result)))
		m.pc = next
		return true

	// SWL/SWR (unaligned store — from PCSX2)
	case 42: // SWL
		addr := uint32(m.rReg32(rs) + simm)
		shift := addr & 3
		mem := m.load32(addr & ^uint32(3))
		swlMask := [4]uint32{0xFFFFFF00, 0xFFFF0000, 0xFF000000, 0x00000000}
		swlShift := [4]uint32{24, 16, 8, 0}
		result := (mem & swlMask[shift]) | (uint32(m.rReg(rt)) >> swlShift[shift])
		m.store32(addr&^uint32(3), result)
		m.pc = next
		return true
	case 46: // SWR
		addr := uint32(m.rReg32(rs) + simm)
		shift := addr & 3
		mem := m.load32(addr & ^uint32(3))
		swrMask := [4]uint32{0x00000000, 0x000000FF, 0x0000FFFF, 0x00FFFFFF}
		swrShift := [4]uint32{0, 8, 16, 24}
		result := (mem & swrMask[shift]) | (uint32(m.rReg(rt)) << swrShift[shift])
		m.store32(addr&^uint32(3), result)
		m.pc = next
		return true

	// 64-bit unaligned load/store (LDL/LDR/SDL/SDR)
	// Used by VIMap for copying dictionary entries (8-byte key+value pairs).
	// Similar to LWL/LWR but for doublewords.
	// 64-bit unaligned load/store — from PCSX2 R5900OpcodeImpl.cpp (little-endian)
	case 26: // LDL: rt = (rt & LDL_MASK[shift]) | (mem << LDL_SHIFT[shift])
		addr := uint32(m.rReg32(rs) + simm)
		shift := addr & 7
		aligned := addr & ^uint32(7)
		lo := uint64(m.load32(aligned))
		hi := uint64(m.load32(aligned + 4))
		mem := (hi << 32) | lo
		reg := uint64(m.rReg(rt))
		ldlShift := [8]uint{56, 48, 40, 32, 24, 16, 8, 0}
		ldlMask := [8]uint64{
			0x00FFFFFFFFFFFFFF, 0x0000FFFFFFFFFFFF, 0x000000FFFFFFFFFF, 0x00000000FFFFFFFF,
			0x0000000000FFFFFF, 0x000000000000FFFF, 0x00000000000000FF, 0x0000000000000000,
		}
		m.wReg(rt, int64((reg&ldlMask[shift])|(mem<<ldlShift[shift])))
		m.pc = next
		return true
	case 27: // LDR: rt = (rt & LDR_MASK[shift]) | (mem >> LDR_SHIFT[shift])
		addr := uint32(m.rReg32(rs) + simm)
		shift := addr & 7
		aligned := addr & ^uint32(7)
		lo := uint64(m.load32(aligned))
		hi := uint64(m.load32(aligned + 4))
		mem := (hi << 32) | lo
		reg := uint64(m.rReg(rt))
		ldrShift := [8]uint{0, 8, 16, 24, 32, 40, 48, 56}
		ldrMask := [8]uint64{
			0x0000000000000000, 0xFF00000000000000, 0xFFFF000000000000, 0xFFFFFF0000000000,
			0xFFFFFFFF00000000, 0xFFFFFFFFFF000000, 0xFFFFFFFFFFFF0000, 0xFFFFFFFFFFFFFF00,
		}
		m.wReg(rt, int64((reg&ldrMask[shift])|(mem>>ldrShift[shift])))
		m.pc = next
		return true
	case 44: // SDL — from PCSX2: mem = (reg >> SDL_SHIFT) | (mem & SDL_MASK)
		addr := uint32(m.rReg32(rs) + simm)
		shift := addr & 7
		aligned := addr & ^uint32(7)
		lo := uint64(m.load32(aligned))
		hi := uint64(m.load32(aligned + 4))
		mem := (hi << 32) | lo
		reg := uint64(m.rReg(rt))
		sdlShift := [8]uint{56, 48, 40, 32, 24, 16, 8, 0}
		sdlMask := [8]uint64{
			0xFFFFFFFFFFFFFF00, 0xFFFFFFFFFFFF0000, 0xFFFFFFFFFF000000, 0xFFFFFFFF00000000,
			0xFFFFFF0000000000, 0xFFFF000000000000, 0xFF00000000000000, 0x0000000000000000,
		}
		result64 := (reg >> sdlShift[shift]) | (mem & sdlMask[shift])
		m.store32(aligned, uint32(result64))
		m.store32(aligned+4, uint32(result64>>32))
		m.pc = next
		return true
	case 45: // SDR — from PCSX2: mem = (reg << SDR_SHIFT) | (mem & SDR_MASK)
		addr := uint32(m.rReg32(rs) + simm)
		shift := addr & 7
		aligned := addr & ^uint32(7)
		lo := uint64(m.load32(aligned))
		hi := uint64(m.load32(aligned + 4))
		mem := (hi << 32) | lo
		reg := uint64(m.rReg(rt))
		sdrShift := [8]uint{0, 8, 16, 24, 32, 40, 48, 56}
		sdrMask := [8]uint64{
			0x0000000000000000, 0x00000000000000FF, 0x000000000000FFFF, 0x0000000000FFFFFF,
			0x00000000FFFFFFFF, 0x000000FFFFFFFFFF, 0x0000FFFFFFFFFFFF, 0x00FFFFFFFFFFFFFF,
		}
		result64 := (reg << sdrShift[shift]) | (mem & sdrMask[shift])
		m.store32(aligned, uint32(result64))
		m.store32(aligned+4, uint32(result64>>32))
		m.pc = next
		return true
	}

	return false
}

func (m *Interp) execSpecial(rs, rt, rd, sa int, funct uint32, next uint32) bool {
	switch funct {
	case 0: // SLL
		m.wReg32(rd, (m.rReg(rt)<<sa)&0xFFFFFFFF)
	case 2: // SRL
		m.wReg32(rd, int64(uint32(m.rReg(rt))>>sa))
	case 3: // SRA
		m.wReg32(rd, m.rReg32(rt)>>sa)
	case 4: // SLLV
		m.wReg32(rd, (m.rReg(rt)<<(m.rReg(rs)&0x1F))&0xFFFFFFFF)
	case 6: // SRLV
		m.wReg32(rd, int64(uint32(m.rReg(rt))>>(m.rReg(rs)&0x1F)))
	case 8: // JR
		target := uint32(m.rReg(rs))
		m.execDelay(next)
		if target == 0 {
			m.pc = 0
		} else {
			m.pc = target
		}
		return true
	case 9: // JALR
		target := uint32(m.rReg(rs))
		m.wReg(rd, int64(next+4))
		delayInsn := m.load32(next)
		m.exec(delayInsn) // delay slot first (PCSX2 convention)
		if m.handleJAL(target) {
			m.pc = next + 4
		} else {
			m.pc = target
		}
		return true
	case 10: // MOVZ
		if m.rReg(rt) == 0 {
			m.wReg(rd, m.rReg(rs))
		}
	case 11: // MOVN
		if m.rReg(rt) != 0 {
			m.wReg(rd, m.rReg(rs))
		}
	case 16: // MFHI
		m.wReg(rd, m.hi)
	case 18: // MFLO
		m.wReg(rd, m.lo)
	case 24: // MULT — R5900: 3-operand form writes low result to rd
		a := int64(int32(m.rReg(rs)))
		b := int64(int32(m.rReg(rt)))
		r := a * b
		m.lo = r & 0xFFFFFFFF
		m.hi = (r >> 32) & 0xFFFFFFFF
		if rd != 0 {
			m.wReg32(rd, m.lo) // R5900 extension: rd = LO
		}
	case 25: // MULTU — R5900: 3-operand form writes low result to rd
		a := uint64(uint32(m.rReg(rs)))
		b := uint64(uint32(m.rReg(rt)))
		r := a * b
		m.lo = int64(r & 0xFFFFFFFF)
		m.hi = int64((r >> 32) & 0xFFFFFFFF)
		if rd != 0 {
			m.wReg32(rd, m.lo) // R5900 extension: rd = LO
		}
	case 26: // DIV
		a := int32(m.rReg(rs))
		b := int32(m.rReg(rt))
		if b != 0 {
			m.lo = int64(a / b)
			m.hi = int64(a % b)
		}
	case 32, 33: // ADD, ADDU
		m.wReg32(rd, (m.rReg32(rs)+m.rReg32(rt))&0xFFFFFFFF)
	case 34, 35: // SUB, SUBU
		m.wReg32(rd, (m.rReg32(rs)-m.rReg32(rt))&0xFFFFFFFF)
	case 36: // AND
		m.wReg(rd, m.rReg(rs)&m.rReg(rt))
	case 37: // OR
		m.wReg(rd, m.rReg(rs)|m.rReg(rt))
	case 38: // XOR
		m.wReg(rd, m.rReg(rs)^m.rReg(rt))
	case 39: // NOR
		m.wReg(rd, ^(m.rReg(rs) | m.rReg(rt)))
	case 42: // SLT
		if m.rReg32(rs) < m.rReg32(rt) {
			m.wReg32(rd, 1)
		} else {
			m.wReg32(rd, 0)
		}
	case 43: // SLTU
		if uint32(m.rReg(rs)) < uint32(m.rReg(rt)) {
			m.wReg32(rd, 1)
		} else {
			m.wReg32(rd, 0)
		}
	case 20: // DSLLV
		m.wReg(rd, m.rReg(rt)<<(m.rReg(rs)&63))
	case 22: // DSRLV
		m.wReg(rd, int64(uint64(m.rReg(rt))>>(m.rReg(rs)&63)))
	case 23: // DSRAV
		m.wReg(rd, m.rReg(rt)>>(m.rReg(rs)&63))
	case 44: // DADD (R5900)
		m.wReg(rd, m.rReg(rs)+m.rReg(rt))
	case 45: // DADDU (R5900)
		m.wReg(rd, m.rReg(rs)+m.rReg(rt))
	case 46: // DSUB (R5900)
		m.wReg(rd, m.rReg(rs)-m.rReg(rt))
	case 47: // DSUBU (R5900)
		m.wReg(rd, m.rReg(rs)-m.rReg(rt))
	case 56: // DSLL
		m.wReg(rd, m.rReg(rt)<<sa)
	case 58: // DSRL
		m.wReg(rd, int64(uint64(m.rReg(rt))>>sa))
	case 59: // DSRA
		m.wReg(rd, m.rReg(rt)>>sa)
	case 60: // DSLL32
		m.wReg(rd, m.rReg(rt)<<(sa+32))
	case 62: // DSRL32
		m.wReg(rd, int64(uint64(m.rReg(rt))>>(sa+32)))
	case 63: // DSRA32
		m.wReg(rd, m.rReg(rt)>>(sa+32))
	default:
		return false
	}
	m.pc = next
	return true
}

func (m *Interp) execRegimm(rs, rt int, simm int64, next uint32) bool {
	val := m.rReg32(rs)
	switch rt {
	case 0: // BLTZ
		if val < 0 {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (simm << 2))
		} else {
			m.execDelay(next)
			m.pc = next + 4
		}
	case 1: // BGEZ
		if val >= 0 {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (simm << 2))
		} else {
			m.execDelay(next)
			m.pc = next + 4
		}
	case 2: // BLTZL (likely)
		if val < 0 {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (simm << 2))
		} else {
			// Likely: delay slot nullified (NOT executed)
			m.pc = next + 4
		}
	case 3: // BGEZL (likely)
		if val >= 0 {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (simm << 2))
		} else {
			// Likely: delay slot nullified (NOT executed)
			m.pc = next + 4
		}
	default:
		return false
	}
	return true
}

func (m *Interp) execCOP1(insn uint32, rs, rt, rd, sa int, funct uint32, next uint32) bool {
	fmt_ := rs

	switch fmt_ {
	case 0: // MFC1
		m.wReg32(rt, int64(int32(math.Float32bits(m.fregs[rd]))))
		m.pc = next
		return true
	case 4: // MTC1
		m.fregs[rd] = math.Float32frombits(uint32(m.rReg(rt)))
		m.pc = next
		return true
	case 8: // BC1F/BC1T (PCSX2: checks fpuRegs.fprc[31] bit 23)
		cc := rt & 1 // 0=BC1F (branch if !CC), 1=BC1T (branch if CC)
		taken := false
		if cc == 0 {
			taken = !m.fpCC // BC1F: branch if condition false
		} else {
			taken = m.fpCC // BC1T: branch if condition true
		}
		if taken {
			m.execDelay(next)
			m.pc = uint32(int64(next) + (int64(int16(insn&0xFFFF)) << 2))
		} else {
			m.execDelay(next)
			m.pc = next + 4
		}
		return true
	case 16: // FPU single ops
		fd := sa
		fs := rd
		ft := rt
		switch funct {
		case 0: // ADD.S
			m.fregs[fd] = m.fregs[fs] + m.fregs[ft]
		case 1: // SUB.S
			m.fregs[fd] = m.fregs[fs] - m.fregs[ft]
		case 2: // MUL.S
			m.fregs[fd] = m.fregs[fs] * m.fregs[ft]
		case 3: // DIV.S
			if m.fregs[ft] != 0 {
				m.fregs[fd] = m.fregs[fs] / m.fregs[ft]
			}
		case 6: // MOV.S
			m.fregs[fd] = m.fregs[fs]
		case 7: // NEG.S
			m.fregs[fd] = -m.fregs[fs]
		case 32: // CVT.S.W (int bits → float)
			raw := math.Float32bits(m.fregs[fs])
			m.fregs[fd] = float32(int32(raw))
		case 36: // CVT.W.S (float → int bits)
			ival := int32(m.fregs[fs])
			m.fregs[fd] = math.Float32frombits(uint32(ival))
		case 48: // C.F.S (always false)
			m.fpCC = false
		case 50: // C.EQ.S
			m.fpCC = m.fregs[fs] == m.fregs[ft]
		case 52: // C.LT.S
			m.fpCC = m.fregs[fs] < m.fregs[ft]
		case 54: // C.LE.S
			m.fpCC = m.fregs[fs] <= m.fregs[ft]
		default:
			// Unknown FPU op — skip
		}
		m.pc = next
		return true
	case 20: // Word format — CVT.S.W
		fd := sa
		fs := rd
		if funct == 32 {
			raw := math.Float32bits(m.fregs[fs])
			m.fregs[fd] = float32(int32(raw))
		}
		m.pc = next
		return true
	}
	return false
}

func (m *Interp) execDelay(slotPC uint32) {
	insn := m.load32(slotPC)
	if insn != 0 {
		m.exec(insn)
	}
}

// Store32At stores a uint32 at a given address (for context setup).
func (m *Interp) Store32At(addr, val uint32) {
	m.store32(addr, val)
}

// Store8At stores a byte at a given address.
func (m *Interp) Store8At(addr uint32, val byte) {
	m.store8(addr, val)
}

// Read trace entry types
const (
	_ = iota
)

// ReadEntry records one Read* call intercepted from the PS2 parser.
type ReadEntry struct {
	Type  string  // "int32", "uint32", "float32", "int16", "uint8", "int8", "ReadBegin", "ReadEnd"
	IVal  int64   // integer value (for int types)
	FVal  float32 // float value (for float32)
	Pos   int     // ESF stream position at time of read
	Extra string  // additional info (ReadBegin type/ver/size)
}

// ESFStream simulates VIObjFile for reading ESF data sequentially.
type ESFStream struct {
	Data     []byte
	Pos      int
	objStack []esfObj
	Reads    []ReadEntry
}

type esfObj struct {
	start int
	typ   uint16
	ver   uint16
	size  uint32
}

// NewESFStream creates a stream from raw ESF object data.
func NewESFStream(data []byte) *ESFStream {
	return &ESFStream{Data: data}
}

func (s *ESFStream) ReadBegin() (typ uint16, ver uint16, size uint32) {
	if s.Pos+8 > len(s.Data) {
		return 0, 0, 0
	}
	typ = binary.LittleEndian.Uint16(s.Data[s.Pos:])
	ver = binary.LittleEndian.Uint16(s.Data[s.Pos+2:])
	size = binary.LittleEndian.Uint32(s.Data[s.Pos+4:])
	s.objStack = append(s.objStack, esfObj{s.Pos, typ, ver, size})
	s.Reads = append(s.Reads, ReadEntry{
		Type:  "ReadBegin",
		Pos:   s.Pos,
		Extra: fmt.Sprintf("type=0x%04X ver=%d size=%d", typ, ver, size),
	})
	s.Pos += 8
	return
}

func (s *ESFStream) ReadEnd() {
	if len(s.objStack) > 0 {
		top := s.objStack[len(s.objStack)-1]
		s.objStack = s.objStack[:len(s.objStack)-1]
		s.Pos = top.start + 8 + int(top.size)
		s.Reads = append(s.Reads, ReadEntry{Type: "ReadEnd", Pos: s.Pos})
	}
}

func (s *ESFStream) ObjectVersion() uint16 {
	if len(s.objStack) > 0 {
		return s.objStack[len(s.objStack)-1].ver
	}
	return 0
}

func (s *ESFStream) ReadInt32() int32 {
	if s.Pos+4 > len(s.Data) {
		return 0
	}
	v := int32(binary.LittleEndian.Uint32(s.Data[s.Pos:]))
	s.Reads = append(s.Reads, ReadEntry{Type: "int32", IVal: int64(v), Pos: s.Pos})
	s.Pos += 4
	return v
}

func (s *ESFStream) ReadUint32() uint32 {
	if s.Pos+4 > len(s.Data) {
		return 0
	}
	v := binary.LittleEndian.Uint32(s.Data[s.Pos:])
	s.Reads = append(s.Reads, ReadEntry{Type: "uint32", IVal: int64(v), Pos: s.Pos})
	s.Pos += 4
	return v
}

func (s *ESFStream) ReadFloat32() float32 {
	if s.Pos+4 > len(s.Data) {
		return 0
	}
	v := math.Float32frombits(binary.LittleEndian.Uint32(s.Data[s.Pos:]))
	s.Reads = append(s.Reads, ReadEntry{Type: "float32", FVal: v, Pos: s.Pos})
	s.Pos += 4
	return v
}

func (s *ESFStream) ReadInt16() int16 {
	if s.Pos+2 > len(s.Data) {
		return 0
	}
	v := int16(binary.LittleEndian.Uint16(s.Data[s.Pos:]))
	s.Reads = append(s.Reads, ReadEntry{Type: "int16", IVal: int64(v), Pos: s.Pos})
	s.Pos += 2
	return v
}

func (s *ESFStream) ReadUint8() byte {
	if s.Pos >= len(s.Data) {
		return 0
	}
	v := s.Data[s.Pos]
	s.Reads = append(s.Reads, ReadEntry{Type: "uint8", IVal: int64(v), Pos: s.Pos})
	s.Pos++
	return v
}

func (s *ESFStream) ReadInt8() int8 {
	if s.Pos >= len(s.Data) {
		return 0
	}
	v := int8(s.Data[s.Pos])
	s.Reads = append(s.Reads, ReadEntry{Type: "int8", IVal: int64(v), Pos: s.Pos})
	s.Pos++
	return v
}

func (s *ESFStream) ReadBytes(n int) []byte {
	if s.Pos+n > len(s.Data) {
		return make([]byte, n)
	}
	v := make([]byte, n)
	copy(v, s.Data[s.Pos:s.Pos+n])
	s.Reads = append(s.Reads, ReadEntry{Type: "bytes", IVal: int64(n), Pos: s.Pos})
	s.Pos += n
	return v
}
