package esf

import (
	"image"
)

// RadialFloraDist describes how one flora model is distributed within a set.
// Parsed from ZoneFloraDistArray (0x32F4), 20 bytes per entry.
type RadialFloraDist struct {
	SetIndex    int32   // which FloraSet this belongs to
	SpriteIndex int32   // index into the FloraSprite array
	Density     float32 // spawn density (0.01 - 0.06 typical)
	MaxScale    float32 // maximum scale factor (0.4 - 1.0)
	MinScale    float32 // minimum scale factor (typically 0.2)
}

// RadialFloraSet defines a range of distribution entries.
// Parsed from ZoneFloraSetArray (0x32F8), 8 bytes per entry.
type RadialFloraSet struct {
	StartIndex int32 // first dist entry index
	Count      int32 // number of dist entries
}

// RadialFloraModel holds the texture and bounding box for one flora sprite.
type RadialFloraModel struct {
	Texture *image.NRGBA
	BBox    Box
}

// ZoneRadialFlora holds all radial flora data for a zone.
// Parsed from ZoneFlora (0x32D0) which lives under ZoneBase (0x3200).
type ZoneRadialFlora struct {
	Models []RadialFloraModel
	Dists  []RadialFloraDist
	Sets   []RadialFloraSet
}

// GetRadialFlora loads radial flora data from the zone.
// Returns nil if the zone has no flora data.
func (z *Zone) GetRadialFlora(file *ObjFile) (*ZoneRadialFlora, error) {
	baseInfo := z.info.Child(TypeZoneBase)
	if baseInfo == nil {
		return nil, nil
	}
	floraInfo := baseInfo.Child(TypeZoneFlora)
	if floraInfo == nil {
		return nil, nil
	}

	rf := &ZoneRadialFlora{}

	// Parse flora sprites (0x32E0 → children of type 0x2F00)
	spriteArrayInfo := floraInfo.Child(TypeZoneFloraSpriteArray)
	if spriteArrayInfo != nil {
		floraInfos := spriteArrayInfo.ChildrenOfType(TypeFloraSprite)
		for _, fi := range floraInfos {
			obj, err := file.GetObject(fi)
			if err != nil {
				continue
			}
			fs := obj.(*FloraSprite)

			var tex *image.NRGBA
			mp, err := fs.GetMatPal(file)
			if err == nil && mp != nil && len(mp.Materials) > 0 {
				mat := mp.Materials[0]
				if len(mat.Layers) > 0 && mat.Layers[0].Surface != nil {
					tex = mat.Layers[0].Surface.Image
				}
			}

			rf.Models = append(rf.Models, RadialFloraModel{
				Texture: tex,
				BBox:    fs.BBox,
			})
		}
	}

	// Parse flora distribution (0x32F0 → 0x32F4)
	setsInfo := floraInfo.Child(TypeZoneFloraSets)
	if setsInfo != nil {
		distInfo := setsInfo.Child(TypeZoneFloraDistArray)
		if distInfo != nil {
			file.Seek(distInfo.Offset)
			count := file.readInt32()
			rf.Dists = make([]RadialFloraDist, count)
			for i := int32(0); i < count; i++ {
				rf.Dists[i] = RadialFloraDist{
					SetIndex:    file.readInt32(),
					SpriteIndex: file.readInt32(),
					Density:     file.readFloat32(),
					MaxScale:    file.readFloat32(),
					MinScale:    file.readFloat32(),
				}
			}
		}

		setInfo := setsInfo.Child(TypeZoneFloraSetArray)
		if setInfo != nil {
			file.Seek(setInfo.Offset)
			count := file.readInt32()
			rf.Sets = make([]RadialFloraSet, count)
			for i := int32(0); i < count; i++ {
				rf.Sets[i] = RadialFloraSet{
					StartIndex: file.readInt32(),
					Count:      file.readInt32(),
				}
			}
		}
	}

	if len(rf.Models) == 0 {
		return nil, nil
	}

	return rf, nil
}
