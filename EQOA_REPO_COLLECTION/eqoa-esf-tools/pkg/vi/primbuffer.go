package vi

// VIPrimBuffer matches the PS2 VIPrimBuffer struct layout.
// Parsed by ParsePrimBuffer at SUPPORT 0x004320B8.
//
// PS2 struct (from decompilation + MIPS trace):
//
//	+0x00  int32   type           (VIPrimBufferType: 0=float, 2=packed, 4=packedVGroup)
//	+0x04  int32   numMaterials   (material count)
//	+0x08  int32   numFaces       (face strip count)
//	+0x0C  int32   numVertices    (total vertex count across all faces)
//	+0x10  int32   packingPos     (position packing exponent: scale = 1/2^p)
//	+0x14  int32   packingUV      (UV packing exponent)
//	+0x18  int32   packingNormal  (normal packing exponent)
//
// Per face strip:
//
//	int32  numVerts   (vertices in this strip)
//	int32  material   (material index)
//	N × vertex data   (format depends on type)
//
// Vertex formats (from PS2 MIPS trace, verified 96/96 reads MATCH):
//
//	Type 0 (float):     pos(3×f32) + uv(2×f32) + normal(3×f32) + color(4×u8) = 36 bytes
//	Type 2 (packed):    pos(3×i16) + uv(2×i16) + normal(3×i8) + color(4×u8)  = 17 bytes
//	Type 4 (vgroup):    pos(3×i16) + uv(2×i16) + normal(3×i8) + color(4×u8) + vgroup(i16) = 19 bytes
type VIPrimBuffer struct {
	Type          int32 // VIPrimBufferType enum
	NumMaterials  int32
	NumFaces      int32
	NumVertices   int32
	PackingPos    int32 // position scale = 1 / pow(2, PackingPos)
	PackingUV     int32 // UV scale = 1 / pow(2, PackingUV)
	PackingNormal int32 // normal scale = 1 / pow(2, PackingNormal)
	DictID        uint32

	Faces []VIPrimFace
}

// VIPrimFace is one triangle strip in the PrimBuffer.
type VIPrimFace struct {
	Material int32
	Vertices []VIVertex
}

// VIVertex is a single vertex in PS2 format.
// For packed types (2/4), values are stored raw — apply packing scale at render time.
type VIVertex struct {
	// Position (float for type 0, packed int16 for type 2/4)
	PosX, PosY, PosZ float32

	// Texture coordinates
	U, V float32

	// Normal vector
	NX, NY, NZ float32

	// Color (0.0-1.0)
	R, G, B, A float32

	// Vertex group (type 4 only, -1 for type 0/2)
	VGroup int16
}

// ESF type codes
const (
	TypePrimBuffer     = 0x1200
	TypeSkinPrimBuffer = 0x1210
	TypeFloraPrimBuffer = 0x1230
)

// VIPrimBufferType enum matching PS2
const (
	PrimTypeFloat      = 0 // 3×float pos + 2×float uv + 3×float normal + 4×uint8 color
	PrimTypePacked     = 2 // 3×int16 pos + 2×int16 uv + 3×int8 normal + 4×uint8 color
	PrimTypePackedVG   = 4 // same as packed + int16 vgroup
)
