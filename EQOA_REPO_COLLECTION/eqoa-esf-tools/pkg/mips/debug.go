package mips

import "fmt"

// Debug features ported from PCSX2 DebugTools:
// - Breakpoints (address-triggered stop)
// - MemCheck (memory read/write watching)
// - Call trace (function call logging)

// Breakpoint stops execution at a specific PC address.
type Breakpoint struct {
	Addr    uint32
	Enabled bool
	HitFunc func(m *Interp) // called when hit (can inspect state)
}

// MemCheck watches a memory range for reads/writes.
// Ported from PCSX2 DebugTools/Breakpoints.h MemCheck struct.
type MemCheck struct {
	Start uint32
	End   uint32 // exclusive
	Cond  MemCheckCond
	Log   bool // log reads/writes
	Break bool // stop on access

	// Captured writes for output extraction
	Writes []MemWrite
}

// MemCheckCond matches PCSX2 MemCheckCondition flags.
type MemCheckCond int

const (
	MemCheckRead      MemCheckCond = 0x01
	MemCheckWrite     MemCheckCond = 0x02
	MemCheckReadWrite MemCheckCond = 0x03
)

// MemWrite records one memory write captured by a MemCheck.
type MemWrite struct {
	Addr uint32
	Size int    // 1, 2, or 4 bytes
	Val  uint32 // value written
	PC   uint32 // instruction that wrote it
}

// CallEntry records a function call for the call trace.
type CallEntry struct {
	Target uint32 // call target address
	Caller uint32 // PC of the JAL/JALR instruction
	Name   string // symbol name (if known)
}

// AddBreakpoint adds a breakpoint. The interpreter will call bp.HitFunc
// when PC matches bp.Addr (before executing the instruction).
func (m *Interp) AddBreakpoint(bp Breakpoint) {
	m.breakpoints = append(m.breakpoints, bp)
}

// AddMemCheck adds a memory watch. Writes to [start, end) are captured.
func (m *Interp) AddMemCheck(mc *MemCheck) {
	m.memChecks = append(m.memChecks, mc)
}

// EnableCallTrace enables logging of all JAL/JALR calls.
func (m *Interp) EnableCallTrace() {
	m.callTraceEnabled = true
}

// CallTrace returns the recorded function calls.
func (m *Interp) CallTrace() []CallEntry {
	return m.callTrace
}

// checkBreakpoints checks if any breakpoint matches the current PC.
func (m *Interp) checkBreakpoints() bool {
	for i := range m.breakpoints {
		if m.breakpoints[i].Enabled && m.breakpoints[i].Addr == m.pc {
			if m.breakpoints[i].HitFunc != nil {
				m.breakpoints[i].HitFunc(m)
			}
			return true
		}
	}
	return false
}

// checkMemWrite checks if a memory write hits any MemCheck.
// Called from store8/store16/store32.
func (m *Interp) checkMemWrite(addr uint32, size int, val uint32) {
	for _, mc := range m.memChecks {
		if mc.Cond&MemCheckWrite == 0 {
			continue
		}
		if addr >= mc.Start && addr < mc.End {
			w := MemWrite{Addr: addr, Size: size, Val: val, PC: m.pc}
			mc.Writes = append(mc.Writes, w)
			if mc.Log {
				fmt.Printf("  MEMWRITE @0x%08X size=%d val=0x%08X (PC=0x%08X)\n",
					addr, size, val, m.pc)
			}
		}
	}
}

// Reg returns the value of a GPR register (for breakpoint inspection).
func (m *Interp) Reg(r int) int64 { return m.rReg(r) }

// FReg returns the value of a FPR register.
func (m *Interp) FReg(r int) float32 { return m.fregs[r] }

// PC returns the current program counter.
func (m *Interp) PC() uint32 { return m.pc }

// SP returns the current stack pointer ($sp = $29).
func (m *Interp) SP() uint32 { return uint32(m.rReg(29)) }

// ReadMem32 reads a 32-bit value from interpreted memory (for inspection).
func (m *Interp) ReadMem32(addr uint32) uint32 { return m.load32(addr) }
