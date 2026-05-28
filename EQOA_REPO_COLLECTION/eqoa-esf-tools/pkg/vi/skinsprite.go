// VISkinSprite — skinned model for equipment (armor, weapons, robes).
// 1,273 in TUNARIA. Attached to character skeletons via SkinList.
//
// Structure: header(0x2A10, dictID+bbox) + SpriteArray(0x2A20) + LODInfo(0x2A30)
// PS2 ParseSkinLODSpriteObj at 0x0043B0C8.
package vi

// VISkinSprite is an equipment mesh bound to a skeleton.
type VISkinSprite struct {
	DictID uint32
	BBox   VIBox
	// LOD info (from child 0x2A30)
	LODNear  float32 // distance for high-detail model
	LODFar   float32 // distance for low-detail model
}

const TypeSkinSprite = 0x2A00

// ParseSkinSprite reads a SkinSprite header from ESF child data.
func ParseSkinSprite(children map[uint16][]byte) (*VISkinSprite, error) {
	ss := &VISkinSprite{}
	// Header child 0x2A10
	if hdr, ok := children[0x2A10]; ok {
		pos := 0
		ss.DictID = ru32(hdr, &pos)
		ss.BBox.Min.X = rf32(hdr, &pos)
		ss.BBox.Min.Y = rf32(hdr, &pos)
		ss.BBox.Min.Z = rf32(hdr, &pos)
		ss.BBox.Max.X = rf32(hdr, &pos)
		ss.BBox.Max.Y = rf32(hdr, &pos)
		ss.BBox.Max.Z = rf32(hdr, &pos)
	}
	// LOD info child 0x2A30
	if lod, ok := children[0x2A30]; ok && len(lod) >= 8 {
		pos := 0
		ss.LODNear = rf32(lod, &pos)
		ss.LODFar = rf32(lod, &pos)
	}
	return ss, nil
}
