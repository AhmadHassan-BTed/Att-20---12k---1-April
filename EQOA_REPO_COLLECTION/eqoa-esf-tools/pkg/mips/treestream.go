package mips

import (
	"encoding/binary"
	"fmt"
	"math"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

// ESFTreeStream navigates the ESF object tree using Go's ObjFile parser.
// Replaces the flat ESFStream for parsers that call ReadBegin/ReadEnd
// to navigate to child objects by type (CSprite, HSprite, HSpriteAnim, etc.).
//
// The Go ObjFile pre-parses the full ESF tree. When the PS2 parser calls
// ReadBegin, we find the next child in the tree and seek to it. ReadEnd
// returns to the parent. Data reads (Read_Ri, Read_Rf, etc.) read from
// the current object's raw data at the current position.
type ESFTreeStream struct {
	file     *esf.ObjFile
	data     []byte      // raw ESF file data
	current  *esf.ObjInfo // current object node
	pos      int          // absolute read position in ESF data
	stack    []treeFrame  // parent stack for ReadBegin/ReadEnd nesting
	childIdx       map[*esf.ObjInfo]int          // next child index per parent
	visitedChildren map[*esf.ObjInfo]struct{}   // children already opened
	rootOpened     bool                         // true after first ReadBegin

	Reads []ReadEntry // captured trace
}

type treeFrame struct {
	node *esf.ObjInfo
	pos  int // saved position to restore on ReadEnd
}

// NewESFTreeStream creates a tree-navigating stream from a Go ObjFile.
// startNode is the ObjInfo of the object to parse (e.g., a CSprite node).
func NewESFTreeStream(file *esf.ObjFile, data []byte, startNode *esf.ObjInfo) *ESFTreeStream {
	return &ESFTreeStream{
		file:     file,
		data:     data,
		current:  startNode,
		pos:      int(startNode.Offset),
		childIdx: make(map[*esf.ObjInfo]int),
	}
}

// ReadBegin opens the next child object of the current node.
// PS2 parsers call this to descend into typed children.
// If we're at the root (startNode), the first ReadBegin opens the node itself.
func (s *ESFTreeStream) ReadBegin() (typ uint16, ver uint16, size uint32) {
	// First call: open the start node itself
	if !s.rootOpened {
		s.rootOpened = true
		typ = s.current.Type
		ver = uint16(s.current.Version)
		size = uint32(s.current.Size)
		s.pos = int(s.current.Offset)
		s.Reads = append(s.Reads, ReadEntry{
			Type:  "ReadBegin",
			Pos:   int(s.current.Offset),
			Extra: fmt.Sprintf("type=0x%04X ver=%d size=%d", typ, ver, size),
		})
		return
	}

	// Subsequent calls: find the next UNVISITED child of current node.
	// PS2 sub-parsers call ReadBegin expecting a specific child type.
	// We find children sequentially by advancing childIdx, but some
	// children may be skipped (stubbed sub-parsers don't call ReadBegin).
	// The PS2 ReadBegin reads the type code from the file position.
	// We match by advancing through children until we find one that
	// hasn't been visited yet.
	// Find the next unvisited child. PS2 sub-parsers call ReadBegin expecting
	// to get their specific child type. We search forward from childIdx to find
	// the next child — this handles gaps where stubbed sub-parsers skipped
	// their children.
	children := s.current.Children
	idx := s.childIdx[s.current]

	// Advance past any already-visited children
	for idx < len(children) {
		// Check if this child was already visited by a previous ReadBegin
		if _, visited := s.visitedChildren[children[idx]]; !visited {
			break
		}
		idx++
	}

	if idx >= len(children) {
		// No more children — read from stream as raw data (fallback)
		if s.pos+8 <= len(s.data) {
			typ = binary.LittleEndian.Uint16(s.data[s.pos:])
			ver = binary.LittleEndian.Uint16(s.data[s.pos+2:])
			size = binary.LittleEndian.Uint32(s.data[s.pos+4:])
			s.pos += 8
		}
		s.Reads = append(s.Reads, ReadEntry{
			Type:  "ReadBegin",
			Pos:   s.pos - 8,
			Extra: fmt.Sprintf("type=0x%04X ver=%d size=%d (fallback)", typ, ver, size),
		})
		return
	}

	child := children[idx]
	s.childIdx[s.current] = idx + 1
	if s.visitedChildren == nil {
		s.visitedChildren = make(map[*esf.ObjInfo]struct{})
	}
	s.visitedChildren[child] = struct{}{}

	// Push current state, descend into child
	s.stack = append(s.stack, treeFrame{node: s.current, pos: s.pos})
	s.current = child
	s.pos = int(child.Offset) // Go ObjInfo.Offset is already past the header

	typ = child.Type
	ver = uint16(child.Version)
	size = uint32(child.Size)

	s.Reads = append(s.Reads, ReadEntry{
		Type:  "ReadBegin",
		Pos:   int(child.Offset),
		Extra: fmt.Sprintf("type=0x%04X ver=%d size=%d", typ, ver, size),
	})
	return
}

// ReadEnd closes the current object and returns to the parent.
func (s *ESFTreeStream) ReadEnd() {
	s.Reads = append(s.Reads, ReadEntry{Type: "ReadEnd", Pos: s.pos})
	if len(s.stack) > 0 {
		frame := s.stack[len(s.stack)-1]
		s.stack = s.stack[:len(s.stack)-1]
		s.current = frame.node
		s.pos = frame.pos
	}
}

// ObjectVersion returns the version of the current object.
func (s *ESFTreeStream) ObjectVersion() uint16 {
	return uint16(s.current.Version)
}

// --- Data reads (same interface as ESFStream) ---

func (s *ESFTreeStream) ReadInt32() int32 {
	if s.pos+4 > len(s.data) { return 0 }
	v := int32(binary.LittleEndian.Uint32(s.data[s.pos:]))
	s.Reads = append(s.Reads, ReadEntry{Type: "int32", IVal: int64(v), Pos: s.pos})
	s.pos += 4
	return v
}

func (s *ESFTreeStream) ReadUint32() uint32 {
	if s.pos+4 > len(s.data) { return 0 }
	v := binary.LittleEndian.Uint32(s.data[s.pos:])
	s.Reads = append(s.Reads, ReadEntry{Type: "uint32", IVal: int64(v), Pos: s.pos})
	s.pos += 4
	return v
}

func (s *ESFTreeStream) ReadFloat32() float32 {
	if s.pos+4 > len(s.data) { return 0 }
	v := math.Float32frombits(binary.LittleEndian.Uint32(s.data[s.pos:]))
	s.Reads = append(s.Reads, ReadEntry{Type: "float32", FVal: v, Pos: s.pos})
	s.pos += 4
	return v
}

func (s *ESFTreeStream) ReadInt16() int16 {
	if s.pos+2 > len(s.data) { return 0 }
	v := int16(binary.LittleEndian.Uint16(s.data[s.pos:]))
	s.Reads = append(s.Reads, ReadEntry{Type: "int16", IVal: int64(v), Pos: s.pos})
	s.pos += 2
	return v
}

func (s *ESFTreeStream) ReadUint8() byte {
	if s.pos >= len(s.data) { return 0 }
	v := s.data[s.pos]
	s.Reads = append(s.Reads, ReadEntry{Type: "uint8", IVal: int64(v), Pos: s.pos})
	s.pos++
	return v
}

func (s *ESFTreeStream) ReadInt8() int8 {
	if s.pos >= len(s.data) { return 0 }
	v := int8(s.data[s.pos])
	s.Reads = append(s.Reads, ReadEntry{Type: "int8", IVal: int64(v), Pos: s.pos})
	s.pos++
	return v
}

func (s *ESFTreeStream) ReadBytes(n int) []byte {
	if s.pos+n > len(s.data) { return make([]byte, n) }
	v := make([]byte, n)
	copy(v, s.data[s.pos:s.pos+n])
	s.Reads = append(s.Reads, ReadEntry{Type: "bytes", IVal: int64(n), Pos: s.pos})
	s.pos += n
	return v
}

// RunParserTree runs a PS2 parser using ESF tree navigation.
// Finds the object by DictID in the ObjFile tree, then traces it.
func RunParserTree(eeDump []byte, parserAddr uint32, file *esf.ObjFile, data []byte, node *esf.ObjInfo) (int32, []ReadEntry) {
	interp := New(eeDump)
	stream := NewESFTreeStream(file, data, node)

	// The ESF dispatch table calls ReadBegin for the outer object,
	// then dispatches to the type-specific parser. We pre-call ReadBegin.
	stream.ReadBegin()

	thisAddr := setupVIESFParse(interp)
	interp.Reader = stream
	result := interp.Run(parserAddr, nil, thisAddr)
	return result, stream.Reads
}

// prePopulateDict walks the ESF tree and calls Dictionary::Add for each node
// with a non-zero DictID. This ensures Find returns correct results for
// duplicate DictIDs without relying on native VIMap Insert correctness.
func prePopulateDict(interp *Interp, dictAddr uint32, root *esf.ObjInfo) {
	// ESF type → VIDictionary resource type mapping (from PS2 ParseObject dispatch)
	typeMap := map[uint16]uint32{
		0x1000: 1,  // Surface
		0x1110: 2,  // MaterialPalette
		0x1200: 3,  // PrimBuffer
		0x2000: 4,  // SimpleSprite
		0x2200: 5,  // HSprite
		0x2600: 6,  // HSpriteAnim
		0x2700: 9,  // CSprite
		0x4200: 10, // CollBuffer
		0x5000: 11, // RefMap
	}

	idx := int32(0)
	var walk func(n *esf.ObjInfo)
	walk = func(n *esf.ObjInfo) {
		if n.DictID != 0 {
			resType, ok := typeMap[n.Type]
			if !ok {
				resType = 0
			}
			// Create a fake resource object on heap for the Add call
			fakeRes := interp.heapAlloc(64)
			// Call Dictionary::Add(dict, resource, dictID, resourceType, index)
			interp.RunCall(0x003E42D8, dictAddr, fakeRes, uint32(n.DictID), resType, uint32(idx))
			idx++
		}
		for _, c := range n.Children {
			walk(c)
		}
	}
	walk(root)
}
