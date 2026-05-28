package esf

import (
	"encoding/binary"
	"fmt"
	"io"
	"math"
)

// ESFWriter builds an ESF binary file.
type ESFWriter struct {
	buf         []byte
	objectCount int32
}

// NewWriter creates a new ESFWriter with a placeholder file header.
func NewWriter() *ESFWriter {
	w := &ESFWriter{
		buf: make([]byte, 0, 1024*1024), // 1MB initial cap
	}
	// Reserve 32 bytes for the file header (patched in Finalize).
	w.buf = append(w.buf, make([]byte, 32)...)
	return w
}

// WriteNodeRaw copies an entire object (header + body + all children) verbatim
// from a source ESF file. This is the fast path for unmodified objects.
func (w *ESFWriter) WriteNodeRaw(info *ObjInfo, src *ObjFile) {
	start := info.Offset - 12 // include the 12-byte header
	end := info.Offset + int(info.Size)
	raw := src.RawBytes(start, end-start)
	w.buf = append(w.buf, raw...)
	w.objectCount += countObjects(info)
}

// WriteNodeBegin writes an object header and returns a handle for patching
// the body size after children are written.
func (w *ESFWriter) WriteNodeBegin(typ uint16, version int16, numSubObjects int32) NodeHandle {
	h := NodeHandle{
		headerOffset: len(w.buf),
	}
	// Write 12-byte header with placeholder size.
	w.WriteUint16(typ)
	w.WriteInt16(version)
	w.WriteInt32(0) // size placeholder
	w.WriteInt32(numSubObjects)
	h.bodyOffset = len(w.buf)
	w.objectCount++
	return h
}

// WriteNodeEnd patches the body size in the header written by WriteNodeBegin.
func (w *ESFWriter) WriteNodeEnd(h NodeHandle) {
	size := int32(len(w.buf) - h.bodyOffset)
	binary.LittleEndian.PutUint32(w.buf[h.headerOffset+4:], uint32(size))
}

// NodeHandle tracks position for deferred size patching.
type NodeHandle struct {
	headerOffset int
	bodyOffset   int
}

// Finalize patches the file header and returns the complete ESF file bytes.
func (w *ESFWriter) Finalize() []byte {
	// Patch the 32-byte file header.
	// Magic "OBJF" stored reversed.
	w.buf[0] = 'F'
	w.buf[1] = 'J'
	w.buf[2] = 'B'
	w.buf[3] = 'O'
	binary.LittleEndian.PutUint32(w.buf[4:], uint32(w.objectCount))
	binary.LittleEndian.PutUint32(w.buf[8:], 0)  // fileType
	binary.LittleEndian.PutUint32(w.buf[12:], 0)  // unknown
	binary.LittleEndian.PutUint64(w.buf[16:], 32) // dataOffset = right after header
	binary.LittleEndian.PutUint64(w.buf[24:], 0)  // unknown2
	return w.buf
}

// WriteTo writes the finalized ESF to an io.Writer.
func (w *ESFWriter) WriteTo(out io.Writer) (int64, error) {
	data := w.Finalize()
	n, err := out.Write(data)
	return int64(n), err
}

// Len returns current buffer size.
func (w *ESFWriter) Len() int {
	return len(w.buf)
}

// --- Typed write helpers ---

func (w *ESFWriter) WriteBytes(b []byte)   { w.buf = append(w.buf, b...) }
func (w *ESFWriter) WriteByte(b byte)      { w.buf = append(w.buf, b) }
func (w *ESFWriter) WriteUint16(v uint16)  { w.buf = binary.LittleEndian.AppendUint16(w.buf, v) }
func (w *ESFWriter) WriteInt16(v int16)    { w.buf = binary.LittleEndian.AppendUint16(w.buf, uint16(v)) }
func (w *ESFWriter) WriteInt32(v int32)    { w.buf = binary.LittleEndian.AppendUint32(w.buf, uint32(v)) }
func (w *ESFWriter) WriteUint32(v uint32)  { w.buf = binary.LittleEndian.AppendUint32(w.buf, v) }
func (w *ESFWriter) WriteUint64(v uint64)  { w.buf = binary.LittleEndian.AppendUint64(w.buf, v) }

// PatchUint32At overwrites 4 bytes at the given buffer offset.
func (w *ESFWriter) PatchUint32At(offset int, v uint32) {
	binary.LittleEndian.PutUint32(w.buf[offset:], v)
}

// PatchUint64At overwrites 8 bytes at the given buffer offset.
func (w *ESFWriter) PatchUint64At(offset int, v uint64) {
	binary.LittleEndian.PutUint64(w.buf[offset:], v)
}

func (w *ESFWriter) WriteFloat32(v float32) {
	w.buf = binary.LittleEndian.AppendUint32(w.buf, math.Float32bits(v))
}

func (w *ESFWriter) WritePoint(p Point) {
	w.WriteFloat32(p.X)
	w.WriteFloat32(p.Y)
	w.WriteFloat32(p.Z)
}

func (w *ESFWriter) WriteBox(b Box) {
	w.WriteFloat32(b.MinX)
	w.WriteFloat32(b.MinY)
	w.WriteFloat32(b.MinZ)
	w.WriteFloat32(b.MaxX)
	w.WriteFloat32(b.MaxY)
	w.WriteFloat32(b.MaxZ)
}

func (w *ESFWriter) WriteColor(c [4]byte) {
	w.buf = append(w.buf, c[0], c[1], c[2], c[3])
}

// --- PrimBuffer serialization ---

// WritePrimBuffer writes a PrimBuffer (type 0x1200) node including header.
// Packing exponents are chosen automatically to maximize precision.
func (w *ESFWriter) WritePrimBuffer(pb *PrimBuffer, dictID int32) {
	// Compute bounding box and optimal packing exponents.
	posMax := float32(0)
	uvMax := float32(0)
	for _, vl := range pb.VertexLists {
		for _, v := range vl.Vertices {
			posMax = max32(posMax, abs32(v.X), abs32(v.Y), abs32(v.Z))
			uvMax = max32(uvMax, abs32(v.U), abs32(v.V))
		}
	}

	p1 := packingExponent(posMax)
	p2 := packingExponent(uvMax)

	// Count total faces (vertex lists) and determine pbtype.
	pbtype := int32(2) // basic vertex format
	nfaces := int32(len(pb.VertexLists))

	nmats := int32(0)
	if pb.MatPal != nil {
		nmats = int32(len(pb.MatPal.Materials))
	}

	h := w.WriteNodeBegin(TypePrimBuffer, 2, 0)

	w.WriteInt32(dictID)         // DictID (version > 1)
	w.WriteInt32(pbtype)         // vertex format
	w.WriteInt32(nmats)          // nmats
	w.WriteInt32(nfaces)         // nfaces
	w.WriteInt32(0)              // unknown
	w.WriteInt32(p1)             // position packing
	w.WriteInt32(p2)             // UV packing
	w.WriteInt32(0)              // unused packing

	posFactor := float64(math.Pow(2, float64(p1)))
	uvFactor := float64(math.Pow(2, float64(p2)))

	for _, vl := range pb.VertexLists {
		w.WriteInt32(int32(len(vl.Vertices)))
		w.WriteInt32(int32(vl.Material))

		for _, v := range vl.Vertices {
			// Packed int16 position
			w.WriteInt16(int16(math.Round(float64(v.X) * posFactor)))
			w.WriteInt16(int16(math.Round(float64(v.Y) * posFactor)))
			w.WriteInt16(int16(math.Round(float64(v.Z) * posFactor)))
			// Packed int16 UV
			w.WriteInt16(int16(math.Round(float64(v.U) * uvFactor)))
			w.WriteInt16(int16(math.Round(float64(v.V) * uvFactor)))
			// Normal as signed bytes
			w.WriteByte(byte(int8(v.NX * 127)))
			w.WriteByte(byte(int8(v.NY * 127)))
			w.WriteByte(byte(int8(v.NZ * 127)))
			// Color RGBA
			w.WriteByte(byte(v.R * 255))
			w.WriteByte(byte(v.G * 255))
			w.WriteByte(byte(v.B * 255))
			w.WriteByte(byte(v.A * 255))
		}
	}

	w.WriteNodeEnd(h)
}

// WriteSurface writes a Surface node by copying raw bytes from the source file.
// Texture data is complex (palettes, mipmaps) so raw copy is the safest approach.
func (w *ESFWriter) WriteSurfaceRaw(info *ObjInfo, src *ObjFile) {
	w.WriteNodeRaw(info, src)
}

// --- Zone building helpers ---

// WriteZoneActor writes a ZoneActor (type 0x6000) node.
func (w *ESFWriter) WriteZoneActor(spriteID int32, pos, rot Point, scale float32, color [4]byte) {
	h := w.WriteNodeBegin(TypeZoneActor, 0, 0)
	w.WriteInt32(spriteID)
	w.WritePoint(pos)
	w.WritePoint(rot)
	w.WriteFloat32(scale)
	w.WriteColor(color)
	w.WriteNodeEnd(h)
}

// WriteSimpleSpriteHeader writes a SimpleSprite header child (0x2001).
func (w *ESFWriter) WriteSimpleSpriteHeader(dictID int32, bbox Box) {
	h := w.WriteNodeBegin(TypeSimpleSpriteHeader, 0, 0)
	w.WriteInt32(dictID)
	w.WriteBox(bbox)
	w.WriteNodeEnd(h)
}

// WriteSimpleSubSpriteHeader writes a SimpleSubSprite header child (0x2311).
func (w *ESFWriter) WriteSimpleSubSpriteHeader(dictID, matPalID int32, bbox Box) {
	h := w.WriteNodeBegin(TypeSimpleSubSpriteHeader, 0, 0)
	w.WriteInt32(dictID)
	w.WriteInt32(matPalID)
	w.WriteBox(bbox)
	w.WriteNodeEnd(h)
}

// --- Utility ---

// countObjects counts an ObjInfo node plus all descendants.
func countObjects(info *ObjInfo) int32 {
	n := int32(1)
	for _, c := range info.Children {
		n += countObjects(c)
	}
	return n
}

// packingExponent returns the best packing exponent for a given max absolute value.
// The exponent is chosen so that maxVal * 2^exp fits in int16 (-32768..32767).
func packingExponent(maxVal float32) int32 {
	if maxVal <= 0 {
		return 0
	}
	// We need maxVal * 2^exp <= 32767
	// exp = floor(log2(32767 / maxVal))
	exp := int32(math.Floor(math.Log2(32767 / float64(maxVal))))
	if exp < 0 {
		exp = 0
	}
	if exp > 15 {
		exp = 15
	}
	return exp
}

func abs32(v float32) float32 {
	if v < 0 {
		return -v
	}
	return v
}

func max32(vals ...float32) float32 {
	m := vals[0]
	for _, v := range vals[1:] {
		if v > m {
			m = v
		}
	}
	return m
}

// PatchPrimBufferY scales the Y coordinate of all PrimBuffer vertices in the
// given ESF data by the specified factor. This is an in-place patch on the raw
// packed int16 values, so no tree reconstruction is needed.
func PatchPrimBufferY(data []byte, yScale float32) error {
	// Parse the tree to find PrimBuffer nodes.
	f, err := OpenBytes(append([]byte(nil), data...)) // don't modify during parse
	if err != nil {
		return err
	}
	root, err := f.Root()
	if err != nil {
		return err
	}

	var primBuffers []*ObjInfo
	collectType(root, TypePrimBuffer, &primBuffers)
	collectType(root, TypeSkinPrimBuffer, &primBuffers)

	for _, pb := range primBuffers {
		if err := patchPBYInPlace(data, pb, yScale); err != nil {
			return fmt.Errorf("patching PrimBuffer at offset 0x%x: %w", pb.Offset, err)
		}
	}
	return nil
}

func collectType(info *ObjInfo, typ uint16, out *[]*ObjInfo) {
	if info.Type == typ {
		*out = append(*out, info)
	}
	for _, c := range info.Children {
		collectType(c, typ, out)
	}
}

func patchPBYInPlace(data []byte, pb *ObjInfo, yScale float32) error {
	pos := pb.Offset
	ver := pb.Version

	if ver == 0 {
		return patchPBV0YInPlace(data, pb, yScale)
	}

	// Skip DictID for version > 1
	if ver > 1 {
		pos += 4
	}

	if pos+28 > len(data) {
		return fmt.Errorf("truncated header")
	}

	pbtype := int32(binary.LittleEndian.Uint32(data[pos:]))
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(data[pos:]))
	pos += 4
	pos += 4 // unknown
	pos += 4 // p1 (don't need to read — we scale the packed value directly)
	pos += 4 // p2
	pos += 4 // p3

	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(data) {
			return fmt.Errorf("truncated face header")
		}
		nverts := int32(binary.LittleEndian.Uint32(data[pos:]))
		pos += 4
		pos += 4 // material

		for vi := int32(0); vi < nverts; vi++ {
			switch pbtype {
			case 2:
				// x(2) y(2) z(2) u(2) v(2) normal(3) color(4) = 17 bytes
				if pos+17 > len(data) {
					return fmt.Errorf("truncated vertex")
				}
				// Patch Y at pos+2
				y := int16(binary.LittleEndian.Uint16(data[pos+2:]))
				y = int16(float32(y) * yScale)
				binary.LittleEndian.PutUint16(data[pos+2:], uint16(y))
				pos += 17

			case 4:
				// same as 2 but + vgroup(2) = 19 bytes
				if pos+19 > len(data) {
					return fmt.Errorf("truncated vertex")
				}
				y := int16(binary.LittleEndian.Uint16(data[pos+2:]))
				y = int16(float32(y) * yScale)
				binary.LittleEndian.PutUint16(data[pos+2:], uint16(y))
				pos += 19

			case 5:
				// x(2) y(2) z(2) u(2) v(2) normal(3) bones(4) weights(4) = 21 bytes
				if pos+21 > len(data) {
					return fmt.Errorf("truncated vertex")
				}
				y := int16(binary.LittleEndian.Uint16(data[pos+2:]))
				y = int16(float32(y) * yScale)
				binary.LittleEndian.PutUint16(data[pos+2:], uint16(y))
				pos += 21

			default:
				return fmt.Errorf("unknown pbtype %d", pbtype)
			}
		}
	}
	return nil
}

func patchPBV0YInPlace(data []byte, pb *ObjInfo, yScale float32) error {
	pos := pb.Offset
	pos += 4 // numMaterials
	nfaces := int32(binary.LittleEndian.Uint32(data[pos:]))
	pos += 4
	pos += 4 // numSomething

	for fi := int32(0); fi < nfaces; fi++ {
		nverts := int32(binary.LittleEndian.Uint32(data[pos:]))
		pos += 4
		pos += 4 // material
		for vi := int32(0); vi < nverts; vi++ {
			// x(4) y(4) z(4) u(4) v(4) nx(4) ny(4) nz(4) color(4) = 36 bytes
			if pos+36 > len(data) {
				return fmt.Errorf("truncated v0 vertex")
			}
			// Y is at pos+4 as float32
			yBits := binary.LittleEndian.Uint32(data[pos+4:])
			y := math.Float32frombits(yBits)
			y *= yScale
			binary.LittleEndian.PutUint32(data[pos+4:], math.Float32bits(y))
			pos += 36
		}
	}
	return nil
}

// ExtractZoneESF extracts a single zone from a TUNARIA.ESF file into a
// standalone ESF file containing just the zone's inline geometry (terrain
// tiles in ZoneResources) and the WorldBase metadata.
func ExtractZoneESF(src *ObjFile, zoneIdx int) ([]byte, error) {
	root, err := src.Root()
	if err != nil {
		return nil, err
	}

	worldInfo := root.Child(TypeWorld)
	if worldInfo == nil {
		return nil, fmt.Errorf("no World object found")
	}

	zones := worldInfo.ChildrenOfType(TypeZone)
	if zoneIdx >= len(zones) {
		return nil, fmt.Errorf("zone %d out of range (max %d)", zoneIdx, len(zones)-1)
	}

	zoneInfo := zones[zoneIdx]

	// Build output matching TUNARIA.ESF structure:
	// Root(0x8000) → World(0x8100, zones) + WorldBase(0x8200)
	w := NewWriter()

	worldBase := root.Child(TypeWorldBase)

	// World node containing just this zone
	worldH := w.WriteNodeBegin(TypeWorld, 0, 1)
	w.WriteNodeRaw(zoneInfo, src)
	w.WriteNodeEnd(worldH)

	// WorldBase as sibling (the parser wraps siblings under synthetic Root)
	if worldBase != nil {
		w.WriteNodeRaw(worldBase, src)
	}

	return w.Finalize(), nil
}
