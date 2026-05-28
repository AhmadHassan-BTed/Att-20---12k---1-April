package esf

import (
	"fmt"
	"image"
	"log"
)

// EffectVolumeType identifies the visual effect produced by a volume.
type EffectVolumeType int32

// PS2 VIEffectVolumeType enum (matches runtime jump table in VIEffectVolume::Process).
const (
	EffectTypeNone          EffectVolumeType = 0
	EffectTypeFallingLeaves EffectVolumeType = 1
	EffectTypeLightShafts   EffectVolumeType = 2
	EffectTypeGroundFog     EffectVolumeType = 3
	EffectTypeDustStorm     EffectVolumeType = 4
	EffectTypeFlock         EffectVolumeType = 5
)

func (t EffectVolumeType) String() string {
	switch t {
	case EffectTypeNone:
		return "None"
	case EffectTypeFallingLeaves:
		return "FallingLeaves"
	case EffectTypeLightShafts:
		return "LightShafts"
	case EffectTypeGroundFog:
		return "GroundFog"
	case EffectTypeDustStorm:
		return "DustStorm"
	case EffectTypeFlock:
		return "Flock"
	default:
		return fmt.Sprintf("Unknown(%d)", int32(t))
	}
}

// EffectVolumeSprite represents an ESF EffectVolumeSprite (type 0xC300).
// PS2: ParseEffectVolumeSpriteObj (0x0043C4A0), sprite type 14, dict type 26.
//
// Binary layout (3 children):
//
//	0xC310 (28 bytes): DictID(int32) + LocalBBox(6 floats: MinX,MinY,MinZ, MaxX,MaxY,MaxZ)
//	0xC320 (~2088 bytes): Particle visual — contains embedded Surface (texture) or CSprite
//	0xC330 (32-36 bytes): Effect params:
//	  EffectType(int32) + SpawnOffsetY(f32) + SpawnOffsetXZ(f32) +
//	  Height(f32) + Density(f32) + Speed(f32) + ParticleDictID(int32) +
//	  CSpriteDictID(int32) + [ver>=2: ExtraDictID(int32)]
type EffectVolumeSprite struct {
	info           *ObjInfo
	DictID         int32
	LocalBBox      Box              // from 0xC310, local coordinates
	EffectType     EffectVolumeType // from 0xC330
	SpawnOffsetY   float32          // vertical spawn offset
	SpawnOffsetXZ  float32          // horizontal spawn offset
	Height         float32          // volume height (usually 100)
	Density        float32          // particle density
	Speed          float32          // movement speed / animation rate
	ParticleDictID int32            // references embedded texture in 0xC320 (PS2 dict type 1)
	CSpriteDictID  int32            // references CSprite for Flock type (PS2 dict type 9)
	ExtraDictID    int32            // ver >= 2: stored at VIEffectVolume+0x3C (flags)
	Texture        *image.NRGBA     // particle texture extracted from 0xC320 Surface child
	MeshVertices   []VertexList     // mesh data from 0xC320 CSprite child (flock birds)
}

func (e *EffectVolumeSprite) Load(file *ObjFile) error {
	// Child 0xC310: DictID + local bounding box.
	hdr := e.info.Child(TypeEffectVolumeSpriteHeader)
	if hdr != nil {
		e.DictID = hdr.DictID
		file.Seek(hdr.Offset)
		if hdr.Size >= 28 {
			_ = file.readInt32() // DictID (already in hdr.DictID)
			e.LocalBBox = file.readBox()
		}
	}

	// Child 0xC330: effect parameters.
	// PS2 reads: EffectType(i32) + 5 floats + ParticleDictID(i32) + CSpriteDictID(i32)
	// + [ver>=2: ExtraDictID via ReadDictID]
	params := e.info.Child(TypeEffectVolumeParams)
	if params != nil {
		file.Seek(params.Offset)
		if params.Size >= 32 {
			e.EffectType = EffectVolumeType(file.readInt32())
			e.SpawnOffsetY = file.readFloat32()
			e.SpawnOffsetXZ = file.readFloat32()
			e.Height = file.readFloat32()
			e.Density = file.readFloat32()
			e.Speed = file.readFloat32()
			e.ParticleDictID = file.readInt32()
			e.CSpriteDictID = file.readInt32()
		}
		if params.Version >= 2 && params.Size >= 36 {
			e.ExtraDictID = file.readInt32()
		}
	}

	// Child 0xC320: particle visual — contains Surface (texture) OR CSprite (3D model).
	particle := e.info.Child(TypeEffectVolumeParticle)
	if particle != nil {
		// Try Surface first (fog, dust, leaves, light shafts).
		surfInfo := particle.Child(TypeSurface)
		if surfInfo != nil {
			obj, err := file.GetObject(surfInfo)
			if err == nil && obj != nil {
				if surf, ok := obj.(*Surface); ok && surf.Image != nil {
					e.Texture = surf.Image
				}
			}
		}
		// Try CSprite (flock birds) — extract mesh geometry.
		csInfo := particle.Child(TypeCSprite)
		if csInfo != nil {
			obj, err := file.GetObject(csInfo)
			if err == nil && obj != nil {
				if cs, ok := obj.(*CSprite); ok {
					// Walk CSprite's sub-sprites to find PrimBuffer mesh data.
					for _, child := range csInfo.Children {
						if child.Type == TypeCSpriteArray {
							for _, sub := range child.Children {
								pbInfo := sub.Child(TypeSkinPrimBuffer)
								if pbInfo == nil {
									pbInfo = sub.Child(TypePrimBuffer)
								}
								if pbInfo != nil {
									pbObj, err := file.GetObject(pbInfo)
									if err == nil && pbObj != nil {
										if pb, ok := pbObj.(*PrimBuffer); ok && len(pb.VertexLists) > 0 {
											e.MeshVertices = pb.VertexLists
											log.Printf("esf: EffectVolume CSprite mesh: %d strips, %d total verts",
												len(pb.VertexLists), pb.NumberOfVertices())
										}
									}
								}
							}
						}
					}
					_ = cs // CSprite loaded successfully
				}
			}
		}
	}

	texSize := ""
	if e.Texture != nil {
		b := e.Texture.Bounds()
		texSize = fmt.Sprintf(" tex=%dx%d", b.Dx(), b.Dy())
	}
	if len(e.MeshVertices) > 0 {
		texSize += " mesh=CSprite"
	}
	log.Printf("esf: EffectVolumeSprite dictID=0x%08x type=%s density=%.2f speed=%.2f%s bbox=(%.1f,%.1f,%.1f)-(%.1f,%.1f,%.1f)",
		uint32(e.DictID), e.EffectType, e.Density, e.Speed, texSize,
		e.LocalBBox.MinX, e.LocalBBox.MinY, e.LocalBBox.MinZ,
		e.LocalBBox.MaxX, e.LocalBBox.MaxY, e.LocalBBox.MaxZ)

	return nil
}

func (e *EffectVolumeSprite) ObjInfo() *ObjInfo { return e.info }
