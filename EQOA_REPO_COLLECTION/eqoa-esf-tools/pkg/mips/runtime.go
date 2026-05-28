package mips

// PS2 runtime object implementations for the MIPS interpreter.
// These replace the per-function stubs with actual data structures
// that PS2 code can operate on natively.

// FakeDictionary implements VIDictionary Find/Add operations.
// PS2 parsers use this to register and look up parsed objects by DictID.
type FakeDictionary struct {
	entries map[uint32]dictEntry
}

type dictEntry struct {
	resourceType uint16
	index        int32
}

// Find looks up a DictID. Returns (found bool, type, index).
func (d *FakeDictionary) Find(dictID uint32) (bool, uint16, int32) {
	if d.entries == nil {
		return false, 0, -1
	}
	if e, ok := d.entries[dictID]; ok {
		return true, e.resourceType, e.index
	}
	return false, 0, -1
}

// Add registers a DictID with a type and index.
func (d *FakeDictionary) Add(dictID uint32, resourceType uint16, index int32) {
	if d.entries == nil {
		d.entries = make(map[uint32]dictEntry)
	}
	d.entries[dictID] = dictEntry{resourceType, index}
}

// FakeScene implements VIScene Create*/Get* operations.
// Allocates incrementing indices for animations, refmaps, etc.
type FakeScene struct {
	nextAnim   int32
	nextRefMap int32
	nextSprite int32
}

func (s *FakeScene) CreateAnimation() int32 { s.nextAnim++; return s.nextAnim - 1 }
func (s *FakeScene) CreateRefMap() int32    { s.nextRefMap++; return s.nextRefMap - 1 }
func (s *FakeScene) CreateSprite() int32    { s.nextSprite++; return s.nextSprite - 1 }

// FakeRaster implements VIRaster Create*/Get* operations.
type FakeRaster struct {
	nextPrimBuf  int32
	nextSurface  int32
	nextMatPal   int32
	nextColorBuf int32
}

func (r *FakeRaster) CreatePrimBuffer() int32 { r.nextPrimBuf++; return r.nextPrimBuf - 1 }
func (r *FakeRaster) CreateSurface() int32    { r.nextSurface++; return r.nextSurface - 1 }
func (r *FakeRaster) CreateMatPal() int32     { r.nextMatPal++; return r.nextMatPal - 1 }
func (r *FakeRaster) CreateColorBuf() int32   { r.nextColorBuf++; return r.nextColorBuf - 1 }

// runtimeState holds all fake PS2 runtime objects.
type runtimeState struct {
	dict   FakeDictionary
	scene  FakeScene
	raster FakeRaster
}

// Known PS2 runtime function addresses and their Go handlers.
// These functions are intercepted by handleJAL and routed to Go.
var runtimeFuncs = map[uint32]string{
	// VIDictionary — Go FakeDictionary for reliable Find/Add.
	// Native VIMap works in isolation but fails in deep parser call chains
	// due to an unresolved register/memory interaction bug.
	0x003E4318: "Dictionary_Find",
	0x003E43A8: "Dictionary_FindTyped",
	0x003E42D8: "Dictionary_Add",
}

// handleRuntime processes a PS2 runtime function call.
// Returns true if the function was handled, false if not recognized.
// With Tier 2 native execution, all runtime objects (VIDictionary, VIScene,
// VIRaster) run as native MIPS code. This function only fires for entries
// in runtimeFuncs, which is now empty.
func (m *Interp) handleRuntime(target uint32) bool {
	name, ok := runtimeFuncs[target]
	if !ok {
		return false
	}

	switch name {
	case "Dictionary_Find":
		// Find(dict, dictID, &resourceType, &index)
		// Smart return: check if the caller's beq-on-zero leads to an error
		// return (SkinList) or a create path (CSprite/HSprite main parser).
		dictID := uint32(m.rReg(5))
		a2 := uint32(m.rReg(6))
		a3 := uint32(m.rReg(7))

		if dictID != 0 {
			if found, resType, idx := m.runtime.dict.Find(dictID); found {
				if a2 != 0 { m.store16(a2, resType) }
				if a3 != 0 { m.store32(a3, uint32(idx)) }
				m.wReg32(2, 1)
				m.Intercepted++
				return true
			}
			// Not in dict. Check caller's branch pattern:
			// beq $v0,$zero → TARGET. If TARGET is error return, return "found".
			// If TARGET is create path, return "not found".
			ra := uint32(m.rReg(31))
			wantFound := false
			if ra > 0 && ra+12 < uint32(len(m.code)) {
				for scan := uint32(0); scan <= 4; scan += 4 {
					insn := m.load32(ra + scan)
					op := (insn >> 26) & 0x3F
					rs := (insn >> 21) & 0x1F
					rt := (insn >> 16) & 0x1F
					if op == 4 && rs == 2 && rt == 0 { // BEQ $v0, $zero
						imm := int16(insn & 0xFFFF)
						target := (ra + scan) + 4 + uint32(imm<<2)
						if target+8 < uint32(len(m.code)) {
							tInsn := m.load32(target)
							tOp := (tInsn >> 26) & 0x3F
							tRt := (tInsn >> 16) & 0x1F
							// LQ $ra = epilogue → error path
							if tOp == 0x1E && tRt == 31 { wantFound = true }
							// b OFFSET; li $v0,-1 = error path
							tInsn2 := m.load32(target + 4)
							if tInsn2 == 0x2402FFFF { wantFound = true }
						}
						break
					}
				}
			}
			if wantFound {
				expectedType := m.peekExpectedType()
				if a2 != 0 && expectedType != 0 { m.store16(a2, expectedType) }
				if a3 != 0 { m.store32(a3, 0) }
				m.wReg32(2, 1) // "found" → avoid error
			} else {
				m.wReg32(2, 0) // "not found" → create path
			}
		} else {
			m.wReg32(2, 0)
		}

	case "Dictionary_FindTyped":
		// Always return "found" for non-zero dictIDs. Skip-stubbed parsers
		// (SpriteArray, MaterialPal) don't Add their objects to the dict,
		// but reference parsers (SkinList, Attachments) need them to exist.
		dictID := uint32(m.rReg(5))
		a3 := uint32(m.rReg(7))
		if dictID != 0 {
			if found, _, idx := m.runtime.dict.Find(dictID); found {
				if a3 != 0 { m.store32(a3, uint32(idx)) }
			} else {
				if a3 != 0 { m.store32(a3, 0) } // fake index
			}
			m.wReg32(2, 1) // always "found"
		} else {
			m.wReg32(2, 0) // dictID=0 → not found
		}

	case "Dictionary_Add":
		dictID := uint32(m.rReg(6))
		resType := uint16(m.rReg(7))
		idx := int32(m.rReg(8))
		m.runtime.dict.Add(dictID, resType, idx)
		m.wReg32(2, 0)

	default:
		return false
	}

	m.Intercepted++
	return true
}

// peekExpectedType reads the type constant from the caller's comparison
// instruction after Find returns. Pattern: beq/bne → li $v0, N → compare.
func (m *Interp) peekExpectedType() uint16 {
	ra := uint32(m.rReg(31))
	if ra == 0 || ra+12 >= uint32(len(m.code)) {
		return 0
	}
	// Scan ra+4 through ra+12 for "li $v0, N" (ADDIU $v0, $zero, N)
	// Pattern after Find: beq(delay) → lw(delay slot) → li $v0, N
	for off := uint32(4); off <= 12; off += 4 {
		insn := m.load32(ra + off)
		op := (insn >> 26) & 0x3F
		rs := (insn >> 21) & 0x1F
		rt := (insn >> 16) & 0x1F
		imm := insn & 0xFFFF
		// ADDIU $v0, $zero, N or LI $v0, N
		if op == 9 && rs == 0 && rt == 2 {
			return uint16(imm)
		}
		// ADDI variant
		if op == 8 && rs == 0 && rt == 2 {
			return uint16(imm)
		}
	}
	return 0
}
