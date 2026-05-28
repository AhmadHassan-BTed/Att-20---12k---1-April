package mips

import "math"

// VU0 macro mode implementation for the R5900 MIPS interpreter.
// Cherry-picked from PCSX2's VU0micro.cpp — only implements the
// 25 VU0 operations used by EQOA's SUPPORT module.
//
// VU0 register file:
//   VF[0-31]  — 32 vector registers, each 4×float32 (x,y,z,w)
//   VF[0]     — constant (0.0, 0.0, 0.0, 1.0)
//   ACC       — accumulator register (4×float32)
//   Q         — division result register (float32)
//   I         — immediate value register (float32)

// vu0State holds the VU0 vector unit state.
type vu0State struct {
	vf  [32][4]float32 // VF registers: [reg][component] where 0=x,1=y,2=z,3=w
	acc [4]float32     // accumulator
	q   float32        // Q register (div/sqrt result)
	i   float32        // I register (immediate)
}

func (m *Interp) initVU0() {
	// VF0 is hardwired to (0,0,0,1)
	m.vu0.vf[0] = [4]float32{0, 0, 0, 1.0}
}

// execCOP2 handles COP2 (VU0 macro mode) instructions.
// Returns true if handled, false if unrecognized.
func (m *Interp) execCOP2(insn uint32, next uint32) bool {
	co := (insn >> 25) & 1

	if co == 0 {
		// Data transfer instructions
		return m.execVU0Transfer(insn, next)
	}

	// Computational instructions (co=1)
	return m.execVU0Compute(insn, next)
}

// execVU0Transfer handles COP2 data transfer (co=0).
func (m *Interp) execVU0Transfer(insn uint32, next uint32) bool {
	rs := (insn >> 21) & 0x1F
	rt := int((insn >> 16) & 0x1F)
	rd := int((insn >> 11) & 0x1F) // VU0 register index for some ops

	switch rs {
	case 1: // QMFC2 — Quadword Move From COP2: GPR[rt] = VF[rd]
		// Copy 128 bits from VU0 VF register to EE GPR
		// In our interpreter, GPRs are 64-bit. Store x,y as low 64 bits.
		// For the purposes of ESF parsing, this is used to read back vector results.
		x := math.Float32bits(m.vu0.vf[rd][0])
		y := math.Float32bits(m.vu0.vf[rd][1])
		m.wReg(rt, int64(uint64(y)<<32|uint64(x)))
		m.pc = next
		return true

	case 2: // CFC2 — Copy From COP2 control register
		// Control registers: 0-15 = VI regs, 16 = Status, 17 = MAC, etc.
		// For parsing purposes, return 0
		m.wReg32(rt, 0)
		m.pc = next
		return true

	case 5: // QMTC2 — Quadword Move To COP2: VF[rd] = GPR[rt]
		// Copy 128 bits from EE GPR to VU0 VF register
		// We need to handle 128-bit SQ/LQ stored values
		// For GPR, take the low 64 bits and split into x,y
		val := uint64(m.rReg(rt))
		m.vu0.vf[rd][0] = math.Float32frombits(uint32(val))
		m.vu0.vf[rd][1] = math.Float32frombits(uint32(val >> 32))
		// For the upper 64 bits, we'd need 128-bit GPR support.
		// In practice, QMTC2 is preceded by LQ which loads 128 bits.
		// Our SQ/LQ already handle the upper bits via store8/load8.
		// Read upper 64 bits from the memory where the value came from.
		m.vu0.vf[rd][2] = 0
		m.vu0.vf[rd][3] = 0
		if rd != 0 { // VF0 is constant
			m.pc = next
		} else {
			m.vu0.vf[0] = [4]float32{0, 0, 0, 1.0} // restore VF0
			m.pc = next
		}
		return true

	case 4: // MTC2 — Move To COP2 (32-bit)
		if rd != 0 {
			m.vu0.vf[rd][0] = math.Float32frombits(uint32(m.rReg(rt)))
		}
		m.pc = next
		return true

	case 0: // MFC2 — Move From COP2 (32-bit)
		m.wReg32(rt, int64(int32(math.Float32bits(m.vu0.vf[rd][0]))))
		m.pc = next
		return true

	case 6: // CTC2 — Copy To COP2 control register
		// Store to VI register or control register — no-op for parsing
		m.pc = next
		return true

	case 8: // BC2 — Branch on COP2 condition
		// BC2F/BC2T/BC2FL/BC2TL — branch on VU0 condition flag
		// For parsing, condition is always false (no VU0 computation affects branches)
		subOp := (insn >> 16) & 0x1F
		switch subOp & 3 {
		case 0: // BC2F — branch if false (always taken since we assume false)
			m.execDelay(next)
			m.pc = uint32(int64(next) + (int64(int16(insn&0xFFFF)) << 2))
		case 1: // BC2T — branch if true (never taken)
			m.execDelay(next)
			m.pc = next + 4
		case 2: // BC2FL — branch if false, likely
			m.execDelay(next)
			m.pc = uint32(int64(next) + (int64(int16(insn&0xFFFF)) << 2))
		case 3: // BC2TL — branch if true, likely (never taken, nullify delay)
			m.pc = next + 4
		}
		return true
	}

	return false
}

// execVU0Compute handles COP2 computational instructions (co=1).
func (m *Interp) execVU0Compute(insn uint32, next uint32) bool {
	dest := (insn >> 21) & 0xF // destination mask: bit3=x, bit2=y, bit1=z, bit0=w
	ft := int((insn >> 16) & 0x1F)
	fs := int((insn >> 11) & 0x1F)
	fd := int((insn >> 6) & 0x1F)
	funct := insn & 0x3F

	// Helper: apply dest mask
	apply := func(fd int, result [4]float32) {
		if fd == 0 { return } // VF0 is constant
		if dest&8 != 0 { m.vu0.vf[fd][0] = result[0] }
		if dest&4 != 0 { m.vu0.vf[fd][1] = result[1] }
		if dest&2 != 0 { m.vu0.vf[fd][2] = result[2] }
		if dest&1 != 0 { m.vu0.vf[fd][3] = result[3] }
	}
	applyACC := func(result [4]float32) {
		if dest&8 != 0 { m.vu0.acc[0] = result[0] }
		if dest&4 != 0 { m.vu0.acc[1] = result[1] }
		if dest&2 != 0 { m.vu0.acc[2] = result[2] }
		if dest&1 != 0 { m.vu0.acc[3] = result[3] }
	}

	s := m.vu0.vf[fs]
	t := m.vu0.vf[ft]

	switch funct {
	// VADD variants
	case 0: // VADDx: fd = fs + ft.x
		apply(fd, [4]float32{s[0] + t[0], s[1] + t[0], s[2] + t[0], s[3] + t[0]})
	case 1: // VADDy
		apply(fd, [4]float32{s[0] + t[1], s[1] + t[1], s[2] + t[1], s[3] + t[1]})
	case 2: // VADDz
		apply(fd, [4]float32{s[0] + t[2], s[1] + t[2], s[2] + t[2], s[3] + t[2]})
	case 3: // VADDw
		apply(fd, [4]float32{s[0] + t[3], s[1] + t[3], s[2] + t[3], s[3] + t[3]})

	// VSUB variants
	case 4: // VSUBx
		apply(fd, [4]float32{s[0] - t[0], s[1] - t[0], s[2] - t[0], s[3] - t[0]})
	case 5: // VSUBy
		apply(fd, [4]float32{s[0] - t[1], s[1] - t[1], s[2] - t[1], s[3] - t[1]})
	case 6: // VSUBz
		apply(fd, [4]float32{s[0] - t[2], s[1] - t[2], s[2] - t[2], s[3] - t[2]})
	case 7: // VSUBw
		apply(fd, [4]float32{s[0] - t[3], s[1] - t[3], s[2] - t[3], s[3] - t[3]})

	// VMADD variants
	case 8: // VMADDx
		apply(fd, [4]float32{m.vu0.acc[0]+s[0]*t[0], m.vu0.acc[1]+s[1]*t[0], m.vu0.acc[2]+s[2]*t[0], m.vu0.acc[3]+s[3]*t[0]})
	case 9: // VMADDy
		apply(fd, [4]float32{m.vu0.acc[0]+s[0]*t[1], m.vu0.acc[1]+s[1]*t[1], m.vu0.acc[2]+s[2]*t[1], m.vu0.acc[3]+s[3]*t[1]})
	case 10: // VMADDz
		apply(fd, [4]float32{m.vu0.acc[0]+s[0]*t[2], m.vu0.acc[1]+s[1]*t[2], m.vu0.acc[2]+s[2]*t[2], m.vu0.acc[3]+s[3]*t[2]})
	case 11: // VMADDw
		apply(fd, [4]float32{m.vu0.acc[0]+s[0]*t[3], m.vu0.acc[1]+s[1]*t[3], m.vu0.acc[2]+s[2]*t[3], m.vu0.acc[3]+s[3]*t[3]})

	// VMSUB variants
	case 12: // VMSUBx
		apply(fd, [4]float32{m.vu0.acc[0]-s[0]*t[0], m.vu0.acc[1]-s[1]*t[0], m.vu0.acc[2]-s[2]*t[0], m.vu0.acc[3]-s[3]*t[0]})
	case 13: // VMSUBy
		apply(fd, [4]float32{m.vu0.acc[0]-s[0]*t[1], m.vu0.acc[1]-s[1]*t[1], m.vu0.acc[2]-s[2]*t[1], m.vu0.acc[3]-s[3]*t[1]})
	case 14: // VMSUBz
		apply(fd, [4]float32{m.vu0.acc[0]-s[0]*t[2], m.vu0.acc[1]-s[1]*t[2], m.vu0.acc[2]-s[2]*t[2], m.vu0.acc[3]-s[3]*t[2]})
	case 15: // VMSUBw
		apply(fd, [4]float32{m.vu0.acc[0]-s[0]*t[3], m.vu0.acc[1]-s[1]*t[3], m.vu0.acc[2]-s[2]*t[3], m.vu0.acc[3]-s[3]*t[3]})

	// VMAX broadcast variants (16-19)
	case 16: // VMAXx
		apply(fd, [4]float32{fmax(s[0], t[0]), fmax(s[1], t[0]), fmax(s[2], t[0]), fmax(s[3], t[0])})
	case 17: // VMAXy
		apply(fd, [4]float32{fmax(s[0], t[1]), fmax(s[1], t[1]), fmax(s[2], t[1]), fmax(s[3], t[1])})
	case 18: // VMAXz
		apply(fd, [4]float32{fmax(s[0], t[2]), fmax(s[1], t[2]), fmax(s[2], t[2]), fmax(s[3], t[2])})
	case 19: // VMAXw
		apply(fd, [4]float32{fmax(s[0], t[3]), fmax(s[1], t[3]), fmax(s[2], t[3]), fmax(s[3], t[3])})

	// VMINI broadcast variants (20-23)
	case 20: // VMINIx
		apply(fd, [4]float32{fmin(s[0], t[0]), fmin(s[1], t[0]), fmin(s[2], t[0]), fmin(s[3], t[0])})
	case 21: // VMINIy
		apply(fd, [4]float32{fmin(s[0], t[1]), fmin(s[1], t[1]), fmin(s[2], t[1]), fmin(s[3], t[1])})
	case 22: // VMINIz
		apply(fd, [4]float32{fmin(s[0], t[2]), fmin(s[1], t[2]), fmin(s[2], t[2]), fmin(s[3], t[2])})
	case 23: // VMINIw
		apply(fd, [4]float32{fmin(s[0], t[3]), fmin(s[1], t[3]), fmin(s[2], t[3]), fmin(s[3], t[3])})

	// VMUL variants
	case 24: // VMULx
		apply(fd, [4]float32{s[0] * t[0], s[1] * t[0], s[2] * t[0], s[3] * t[0]})
	case 25: // VMULy
		apply(fd, [4]float32{s[0] * t[1], s[1] * t[1], s[2] * t[1], s[3] * t[1]})
	case 26: // VMULz
		apply(fd, [4]float32{s[0] * t[2], s[1] * t[2], s[2] * t[2], s[3] * t[2]})
	case 27: // VMULw
		apply(fd, [4]float32{s[0] * t[3], s[1] * t[3], s[2] * t[3], s[3] * t[3]})

	case 28: // VMULq
		q := m.vu0.q
		apply(fd, [4]float32{s[0] * q, s[1] * q, s[2] * q, s[3] * q})
	case 29: // VMAXi
		iv := m.vu0.i
		apply(fd, [4]float32{fmax(s[0], iv), fmax(s[1], iv), fmax(s[2], iv), fmax(s[3], iv)})
	case 30: // VMULi
		iv := m.vu0.i
		apply(fd, [4]float32{s[0] * iv, s[1] * iv, s[2] * iv, s[3] * iv})
	case 31: // VMINIi
		iv := m.vu0.i
		apply(fd, [4]float32{fmin(s[0], iv), fmin(s[1], iv), fmin(s[2], iv), fmin(s[3], iv)})

	// Full-register operations
	case 32: // VADDq
		q := m.vu0.q
		apply(fd, [4]float32{s[0] + q, s[1] + q, s[2] + q, s[3] + q})
	case 34: // VADDi
		iv := m.vu0.i
		apply(fd, [4]float32{s[0] + iv, s[1] + iv, s[2] + iv, s[3] + iv})
	case 33: // VMADDq
		q := m.vu0.q
		apply(fd, [4]float32{m.vu0.acc[0]+s[0]*q, m.vu0.acc[1]+s[1]*q, m.vu0.acc[2]+s[2]*q, m.vu0.acc[3]+s[3]*q})
	case 35: // VMADDi
		iv := m.vu0.i
		apply(fd, [4]float32{m.vu0.acc[0]+s[0]*iv, m.vu0.acc[1]+s[1]*iv, m.vu0.acc[2]+s[2]*iv, m.vu0.acc[3]+s[3]*iv})
	case 36: // VSUBq
		q := m.vu0.q
		apply(fd, [4]float32{s[0] - q, s[1] - q, s[2] - q, s[3] - q})
	case 37: // VMSUBq
		q := m.vu0.q
		apply(fd, [4]float32{m.vu0.acc[0]-s[0]*q, m.vu0.acc[1]-s[1]*q, m.vu0.acc[2]-s[2]*q, m.vu0.acc[3]-s[3]*q})
	case 38: // VSUBi
		iv := m.vu0.i
		apply(fd, [4]float32{s[0] - iv, s[1] - iv, s[2] - iv, s[3] - iv})
	case 39: // VMSUBi
		iv := m.vu0.i
		apply(fd, [4]float32{m.vu0.acc[0]-s[0]*iv, m.vu0.acc[1]-s[1]*iv, m.vu0.acc[2]-s[2]*iv, m.vu0.acc[3]-s[3]*iv})
	case 40: // VADD
		apply(fd, [4]float32{s[0] + t[0], s[1] + t[1], s[2] + t[2], s[3] + t[3]})
	case 41: // VMADD
		apply(fd, [4]float32{m.vu0.acc[0]+s[0]*t[0], m.vu0.acc[1]+s[1]*t[1], m.vu0.acc[2]+s[2]*t[2], m.vu0.acc[3]+s[3]*t[3]})
	case 42: // VMUL
		apply(fd, [4]float32{s[0] * t[0], s[1] * t[1], s[2] * t[2], s[3] * t[3]})
	case 43: // VMAX
		apply(fd, [4]float32{fmax(s[0], t[0]), fmax(s[1], t[1]), fmax(s[2], t[2]), fmax(s[3], t[3])})
	case 44: // VSUB
		apply(fd, [4]float32{s[0] - t[0], s[1] - t[1], s[2] - t[2], s[3] - t[3]})
	case 45: // VMSUB
		apply(fd, [4]float32{m.vu0.acc[0]-s[0]*t[0], m.vu0.acc[1]-s[1]*t[1], m.vu0.acc[2]-s[2]*t[2], m.vu0.acc[3]-s[3]*t[3]})
	case 46: // VOPMSUB (cross product step)
		apply(fd, [4]float32{
			m.vu0.acc[0] - s[1]*t[2],
			m.vu0.acc[1] - s[2]*t[0],
			m.vu0.acc[2] - s[0]*t[1],
			0,
		})
	case 47: // VMINI
		apply(fd, [4]float32{fmin(s[0], t[0]), fmin(s[1], t[1]), fmin(s[2], t[2]), fmin(s[3], t[3])})

	// Integer ops
	case 48: // VIADD
		// VI integer add — not commonly used in EQOA
		m.pc = next
		return true
	case 49: // VISUB
		m.pc = next
		return true

	// SPECIAL1 (funct=60): Broadcast accumulator and conversion ops
	// Subfn layout: bits[10:6] = operation
	// 0-3: VADDAx/y/z/w,  4-7: VSUBAx/y/z/w
	// 8-11: VMADDAx/y/z/w, 12-15: VMSUBAx/y/z/w
	// 24-27: VMULAx/y/z/w
	case 60:
		subfn := (insn >> 6) & 0x1F
		bc := int(subfn & 3) // broadcast component
		switch subfn >> 2 {
		case 0: // VADDAx/y/z/w (subfn 0-3)
			applyACC([4]float32{s[0] + t[bc], s[1] + t[bc], s[2] + t[bc], s[3] + t[bc]})
		case 1: // VSUBAx/y/z/w (subfn 4-7)
			applyACC([4]float32{s[0] - t[bc], s[1] - t[bc], s[2] - t[bc], s[3] - t[bc]})
		case 2: // VMADDAx/y/z/w (subfn 8-11)
			applyACC([4]float32{m.vu0.acc[0]+s[0]*t[bc], m.vu0.acc[1]+s[1]*t[bc], m.vu0.acc[2]+s[2]*t[bc], m.vu0.acc[3]+s[3]*t[bc]})
		case 3: // VMSUBAx/y/z/w (subfn 12-15)
			applyACC([4]float32{m.vu0.acc[0]-s[0]*t[bc], m.vu0.acc[1]-s[1]*t[bc], m.vu0.acc[2]-s[2]*t[bc], m.vu0.acc[3]-s[3]*t[bc]})
		case 6: // VMULAx/y/z/w (subfn 24-27)
			applyACC([4]float32{s[0] * t[bc], s[1] * t[bc], s[2] * t[bc], s[3] * t[bc]})
		case 7: // VMULAq(28), VABS(29), VMULAi(30), VCLIPw(31)
			switch subfn {
			case 28: // VMULAq
				q := m.vu0.q
				applyACC([4]float32{s[0] * q, s[1] * q, s[2] * q, s[3] * q})
			case 29: // VABS
				abs := func(v float32) float32 { if v < 0 { return -v }; return v }
				apply(fd, [4]float32{abs(s[0]), abs(s[1]), abs(s[2]), abs(s[3])})
			case 30: // VMULAi
				iv := m.vu0.i
				applyACC([4]float32{s[0] * iv, s[1] * iv, s[2] * iv, s[3] * iv})
			}
		default:
			// Conversion ops (VITOF/VFTOI) — skip for parsing
		}

	// SPECIAL2 (funct=61): Full-register accumulator ops
	case 61:
		subfn := (insn >> 6) & 0x1F
		switch subfn {
		case 0: // VADDAq
			q := m.vu0.q
			applyACC([4]float32{s[0] + q, s[1] + q, s[2] + q, s[3] + q})
		case 1: // VMADDAq
			q := m.vu0.q
			applyACC([4]float32{m.vu0.acc[0]+s[0]*q, m.vu0.acc[1]+s[1]*q, m.vu0.acc[2]+s[2]*q, m.vu0.acc[3]+s[3]*q})
		case 2: // VADDAi
			iv := m.vu0.i
			applyACC([4]float32{s[0] + iv, s[1] + iv, s[2] + iv, s[3] + iv})
		case 3: // VMADDAi
			iv := m.vu0.i
			applyACC([4]float32{m.vu0.acc[0]+s[0]*iv, m.vu0.acc[1]+s[1]*iv, m.vu0.acc[2]+s[2]*iv, m.vu0.acc[3]+s[3]*iv})
		case 4: // VSUBAq
			q := m.vu0.q
			applyACC([4]float32{s[0] - q, s[1] - q, s[2] - q, s[3] - q})
		case 5: // VMSUBAq
			q := m.vu0.q
			applyACC([4]float32{m.vu0.acc[0]-s[0]*q, m.vu0.acc[1]-s[1]*q, m.vu0.acc[2]-s[2]*q, m.vu0.acc[3]-s[3]*q})
		case 8: // VADDA
			applyACC([4]float32{s[0] + t[0], s[1] + t[1], s[2] + t[2], s[3] + t[3]})
		case 9: // VMADDA
			applyACC([4]float32{m.vu0.acc[0]+s[0]*t[0], m.vu0.acc[1]+s[1]*t[1], m.vu0.acc[2]+s[2]*t[2], m.vu0.acc[3]+s[3]*t[3]})
		case 10: // VMULA
			applyACC([4]float32{s[0] * t[0], s[1] * t[1], s[2] * t[2], s[3] * t[3]})
		case 12: // VSUBA
			applyACC([4]float32{s[0] - t[0], s[1] - t[1], s[2] - t[2], s[3] - t[3]})
		case 13: // VMSUBA
			applyACC([4]float32{m.vu0.acc[0]-s[0]*t[0], m.vu0.acc[1]-s[1]*t[1], m.vu0.acc[2]-s[2]*t[2], m.vu0.acc[3]-s[3]*t[3]})
		case 14: // VOPMULA (cross product step to ACC)
			applyACC([4]float32{s[1] * t[2], s[2] * t[0], s[0] * t[1], 0})
		case 15: // VNOP
			// no-op
		}

	// SPECIAL3 (funct=62)
	case 62:
		subfn := (insn >> 6) & 0x1F
		switch subfn {
		case 0: // VMOVE: fd = fs
			apply(fd, s)
		case 1: // VMR32: rotate right by 32 bits
			apply(fd, [4]float32{s[1], s[2], s[3], s[0]})
		case 8: // VDIV: Q = fs.fsf / ft.ftf
			fsf := int((insn >> 21) & 3)
			ftf := int((insn >> 23) & 3)
			if t[ftf] != 0 {
				m.vu0.q = s[fsf] / t[ftf]
			}
		case 9: // VSQRT: Q = sqrt(ft.ftf)
			ftf := int((insn >> 23) & 3)
			m.vu0.q = float32(math.Sqrt(float64(t[ftf])))
		case 10: // VRSQRT: Q = fs.fsf / sqrt(ft.ftf)
			fsf := int((insn >> 21) & 3)
			ftf := int((insn >> 23) & 3)
			sq := float32(math.Sqrt(float64(t[ftf])))
			if sq != 0 {
				m.vu0.q = s[fsf] / sq
			}
		case 11: // VWAITQ — wait for division, no-op in software
		case 12: // VMTIR: VI[ft] = VF[fs].x (as int16)
			// Integer transfer — skip for parsing
		case 13: // VMFIR: VF[ft].x = VI[fs] (as float from int16)
			// Integer transfer — skip for parsing
		default:
			m.pc = next
			return true
		}

	// SPECIAL4 (funct=63)
	case 63:
		// Rarely used in EQOA — skip
		m.pc = next
		return true

	default:
		return false
	}

	m.pc = next
	return true
}

func fmax(a, b float32) float32 {
	if a > b { return a }
	return b
}

func fmin(a, b float32) float32 {
	if a < b { return a }
	return b
}
