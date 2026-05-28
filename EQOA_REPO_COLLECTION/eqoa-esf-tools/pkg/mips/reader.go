package mips

// ESFReader is the common interface for both flat (ESFStream) and
// tree-navigating (ESFTreeStream) ESF data sources. The MIPS interpreter
// routes all VIObjFile Read* calls through this interface.
type ESFReader interface {
	ReadBegin() (typ uint16, ver uint16, size uint32)
	ReadEnd()
	ObjectVersion() uint16
	NumSubObjects() int32
	ObjectSize() int32
	ReadInt32() int32
	ReadUint32() uint32
	ReadFloat32() float32
	ReadInt16() int16
	ReadUint8() byte
	ReadInt8() int8
	ReadBytes(n int) []byte
	GetReads() []ReadEntry
}

// Verify both types implement ESFReader
var _ ESFReader = (*ESFStream)(nil)
var _ ESFReader = (*ESFTreeStream)(nil)

// NumSubObjects returns the number of sub-objects at the current node.
// For flat streams, returns 0 (no tree structure).
func (s *ESFStream) NumSubObjects() int32 { return 0 }
func (s *ESFStream) ObjectSize() int32 { return 0 }

// NumSubObjects returns children count of the current tree node.
func (s *ESFTreeStream) NumSubObjects() int32 {
	if s.current == nil {
		return 0
	}
	return int32(len(s.current.Children))
}

func (s *ESFTreeStream) ObjectSize() int32 {
	if s.current == nil {
		return 0
	}
	return s.current.Size
}

// GetReads returns the captured read trace from ESFStream.
func (s *ESFStream) GetReads() []ReadEntry { return s.Reads }

// GetReads returns the captured read trace from ESFTreeStream.
func (s *ESFTreeStream) GetReads() []ReadEntry { return s.Reads }
