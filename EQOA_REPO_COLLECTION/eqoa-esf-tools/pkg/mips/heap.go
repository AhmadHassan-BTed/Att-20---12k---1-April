package mips

// PS2 heap allocator for the MIPS interpreter.
// Provides memory for PS2 new/malloc calls so runtime data structures
// (VIDictionary, VIMap, VIArray, etc.) can be initialized and used natively.
//
// The heap occupies 0x01E00000-0x01EFFFFF (1MB) in the interpreter's
// write overlay. This is below the stack (0x01FF0000) and above the
// fake pointer region (0x01F00000-0x01FDFFFF).

const (
	heapBase = uint32(0x01E00000)
	heapSize = uint32(0x00400000) // 4MB
)

// heapState tracks the simple bump allocator.
type heapState struct {
	nextAddr uint32
}

// HeapAllocExported is the exported version of heapAlloc.
func (m *Interp) HeapAllocExported(size uint32) uint32 { return m.heapAlloc(size) }

// Load32At reads a 32-bit value from memory (exported).
func (m *Interp) Load32At(addr uint32) uint32 { return m.load32(addr) }

// Regs returns the register file for inspection.
func (m *Interp) Regs() [32]int64 { return m.regs }

// GetPC returns the current program counter.
func (m *Interp) GetPC() uint32 { return m.pc }

// heapAlloc allocates n bytes from the PS2 heap, zeroed.
// Returns the address of the allocated block.
func (m *Interp) heapAlloc(size uint32) uint32 {
	if m.heap.nextAddr == 0 {
		m.heap.nextAddr = heapBase
	}

	// Align to 16 bytes (PS2 quadword alignment)
	size = (size + 15) & ^uint32(15)

	addr := m.heap.nextAddr
	if addr+size > heapBase+heapSize {
		// Out of heap — return 0 (allocation failure)
		return 0
	}

	// Zero the allocated memory
	for i := uint32(0); i < size; i++ {
		m.store8(addr+i, 0)
	}

	m.heap.nextAddr = addr + size
	return addr
}
