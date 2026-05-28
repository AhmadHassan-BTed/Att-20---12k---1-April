package vi

// ParsePrimBuffer parses a VIPrimBuffer from raw ESF object data.
// Generated from PS2 ParsePrimBuffer at SUPPORT 0x004320B8.
// Verified: 96/96 reads MATCH against PS2 MIPS execution on TUNARIA terrain.
//
// PS2 read sequence (from MIPS interpreter trace):
//
//	ReadBegin(0x1200)
//	if ver > 1: uint32 dictID
//	int32 type (VIPrimBufferType)
//	int32 nmats
//	int32 nfaces
//	int32 unk
//	int32 packingPos
//	int32 packingUV
//	int32 packingNormal
//	per face:
//	  int32 nverts
//	  int32 material
//	  per vertex (type-dependent):
//	    type 0: 3×float32 + 2×float32 + 3×float32 + 4×uint8
//	    type 2: 3×int16 + 2×int16 + 3×int8 + 4×uint8
//	    type 4: 3×int16 + 2×int16 + 3×int8 + 4×uint8 + int16

import (
	"encoding/binary"
	"fmt"
	"math"
)

// ParsePrimBuffer reads a PrimBuffer from raw ESF object data.
// data starts at the ESF object header (type + ver + size).
func ParsePrimBuffer(data []byte) (*VIPrimBuffer, error) {
	if len(data) < 8 {
		return nil, fmt.Errorf("VIPrimBuffer: data too short (%d bytes)", len(data))
	}

	typ := binary.LittleEndian.Uint16(data[0:])
	ver := binary.LittleEndian.Uint16(data[2:])

	if typ != TypePrimBuffer && typ != TypeSkinPrimBuffer && typ != TypeFloraPrimBuffer {
		return nil, fmt.Errorf("VIPrimBuffer: wrong type 0x%04X", typ)
	}

	pb := &VIPrimBuffer{}
	pos := 8 // past header

	// PS2 ParsePrimBuffer: version 0 handled by ParsePrimBufferObjV0
	if ver == 0 {
		return parsePrimBufferV0(data)
	}

	// Version > 1: read DictID (PS2: sltiu $v0, ver, 2 → if ver >= 2)
	if ver > 1 {
		pb.DictID = ru32(data, &pos)
	}

	// Header fields (PS2 reads to sp+0x118..sp+0x130)
	// From decompilation: 7 int32 reads after dictID
	pb.Type = ri32(data, &pos)          // sp+0x118: VIPrimBufferType
	pb.NumMaterials = ri32(data, &pos)  // sp+0x11C: material count
	pb.NumFaces = ri32(data, &pos)      // sp+0x120: face strip count
	_ = ri32(data, &pos)                // sp+0x124: unknown
	pb.PackingPos = ri32(data, &pos)    // sp+0x128: position packing
	pb.PackingUV = ri32(data, &pos)     // sp+0x12C: UV packing
	pb.PackingNormal = ri32(data, &pos) // sp+0x130: normal packing

	// Packing scale factors (PS2: powf(2.0, packing) then 1.0/result)
	pk1 := float32(1.0 / math.Pow(2, float64(pb.PackingPos)))
	pk2 := float32(1.0 / math.Pow(2, float64(pb.PackingUV)))
	pk3 := float32(1.0 / math.Pow(2, float64(pb.PackingNormal)))

	// Sanity check
	if pb.NumFaces < 0 || pb.NumFaces > 10000 {
		return nil, fmt.Errorf("VIPrimBuffer: invalid nfaces %d", pb.NumFaces)
	}

	// Per-face vertex data
	pb.Faces = make([]VIPrimFace, 0, pb.NumFaces)

	for fi := int32(0); fi < pb.NumFaces; fi++ {
		if pos+8 > len(data) {
			break
		}
		nverts := ri32(data, &pos)
		mat := ri32(data, &pos)

		// PS2: negative/zero nverts → empty face, vertex loop just doesn't execute
		if nverts > 100000 {
			break // sanity: probably corrupt data
		}
		if nverts <= 0 {
			pb.Faces = append(pb.Faces, VIPrimFace{Material: mat})
			continue
		}

		face := VIPrimFace{
			Material: mat,
			Vertices: make([]VIVertex, 0, nverts),
		}

		for vi := int32(0); vi < nverts; vi++ {
			var v VIVertex
			v.VGroup = -1

			switch pb.Type {
			case PrimTypeFloat:
				// PS2 trace: 8×float32 + 4×uint8 per vertex
				if pos+36 > len(data) {
					break
				}
				v.PosX = rf32(data, &pos)
				v.PosY = rf32(data, &pos)
				v.PosZ = rf32(data, &pos)
				v.U = rf32(data, &pos)
				v.V = rf32(data, &pos)
				v.NX = rf32(data, &pos)
				v.NY = rf32(data, &pos)
				v.NZ = rf32(data, &pos)
				v.R = float32(data[pos]) / 255.0; pos++
				v.G = float32(data[pos]) / 255.0; pos++
				v.B = float32(data[pos]) / 255.0; pos++
				v.A = float32(data[pos]) / 255.0; pos++

			case PrimTypePacked:
				// PS2 trace: 5×int16 + 3×int8 + 4×uint8
				if pos+17 > len(data) {
					break
				}
				v.PosX = float32(ri16(data, &pos)) * pk1
				v.PosY = float32(ri16(data, &pos)) * pk1
				v.PosZ = float32(ri16(data, &pos)) * pk1
				v.U = float32(ri16(data, &pos)) * pk2
				v.V = float32(ri16(data, &pos)) * pk2
				v.NX = float32(ri8(data, &pos)) * pk3
				v.NY = float32(ri8(data, &pos)) * pk3
				v.NZ = float32(ri8(data, &pos)) * pk3
				v.R = float32(data[pos]) / 255.0; pos++
				v.G = float32(data[pos]) / 255.0; pos++
				v.B = float32(data[pos]) / 255.0; pos++
				v.A = float32(data[pos]) / 255.0; pos++

			case PrimTypePackedVG:
				// PS2 trace: 5×int16 + 3×int8 + 4×uint8 + int16
				if pos+19 > len(data) {
					break
				}
				v.PosX = float32(ri16(data, &pos)) * pk1
				v.PosY = float32(ri16(data, &pos)) * pk1
				v.PosZ = float32(ri16(data, &pos)) * pk1
				v.U = float32(ri16(data, &pos)) * pk2
				v.V = float32(ri16(data, &pos)) * pk2
				v.NX = float32(ri8(data, &pos)) * pk3
				v.NY = float32(ri8(data, &pos)) * pk3
				v.NZ = float32(ri8(data, &pos)) * pk3
				v.R = float32(data[pos]) / 255.0; pos++
				v.G = float32(data[pos]) / 255.0; pos++
				v.B = float32(data[pos]) / 255.0; pos++
				v.A = float32(data[pos]) / 255.0; pos++
				v.VGroup = ri16(data, &pos)

			default:
				return nil, fmt.Errorf("VIPrimBuffer: unsupported type %d", pb.Type)
			}

			face.Vertices = append(face.Vertices, v)
		}

		pb.Faces = append(pb.Faces, face)
	}

	return pb, nil
}

// parsePrimBufferV0 handles version 0 (ParsePrimBufferObjV0 at 0x00432CD0).
// V0 format: float vertices with no packing.
func parsePrimBufferV0(data []byte) (*VIPrimBuffer, error) {
	pb := &VIPrimBuffer{Type: PrimTypeFloat}
	pos := 8 // past header

	pb.NumMaterials = ri32(data, &pos)
	pb.NumFaces = ri32(data, &pos)
	_ = ri32(data, &pos) // unk

	pb.Faces = make([]VIPrimFace, 0, pb.NumFaces)

	for fi := int32(0); fi < pb.NumFaces; fi++ {
		if pos+8 > len(data) {
			break
		}
		nverts := ri32(data, &pos)
		mat := ri32(data, &pos)

		// PS2: negative/zero nverts → empty face, vertex loop just doesn't execute
		if nverts > 100000 {
			break // sanity: probably corrupt data
		}
		if nverts <= 0 {
			pb.Faces = append(pb.Faces, VIPrimFace{Material: mat})
			continue
		}

		face := VIPrimFace{
			Material: mat,
			Vertices: make([]VIVertex, 0, nverts),
		}

		for vi := int32(0); vi < nverts; vi++ {
			if pos+36 > len(data) {
				break
			}
			v := VIVertex{VGroup: -1}
			v.PosX = rf32(data, &pos)
			v.PosY = rf32(data, &pos)
			v.PosZ = rf32(data, &pos)
			v.U = rf32(data, &pos)
			v.V = rf32(data, &pos)
			v.NX = rf32(data, &pos)
			v.NY = rf32(data, &pos)
			v.NZ = rf32(data, &pos)
			v.R = float32(data[pos]) / 255.0; pos++
			v.G = float32(data[pos]) / 255.0; pos++
			v.B = float32(data[pos]) / 255.0; pos++
			v.A = float32(data[pos]) / 255.0; pos++
			face.Vertices = append(face.Vertices, v)
		}

		pb.Faces = append(pb.Faces, face)
	}

	return pb, nil
}

// Read helpers (match PS2 Read__9VIObjFile* calling convention)

func ri32(d []byte, p *int) int32 {
	if *p+4 > len(d) { return 0 }
	v := int32(binary.LittleEndian.Uint32(d[*p:]))
	*p += 4
	return v
}

func ru32(d []byte, p *int) uint32 {
	if *p+4 > len(d) { return 0 }
	v := binary.LittleEndian.Uint32(d[*p:])
	*p += 4
	return v
}

func rf32(d []byte, p *int) float32 {
	if *p+4 > len(d) { return 0 }
	v := math.Float32frombits(binary.LittleEndian.Uint32(d[*p:]))
	*p += 4
	return v
}

func ri16(d []byte, p *int) int16 {
	if *p+2 > len(d) { return 0 }
	v := int16(binary.LittleEndian.Uint16(d[*p:]))
	*p += 2
	return v
}

func ri8(d []byte, p *int) int8 {
	if *p >= len(d) { return 0 }
	v := int8(d[*p])
	*p++
	return v
}
