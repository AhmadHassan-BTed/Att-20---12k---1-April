// VIGroupSprite — compound model made of multiple SimpleSprites.
// 688 in TUNARIA. Buildings, complex objects.
//
// Structure: header(0x2C10) + SpriteArray(0x2C20) containing SimpleSprites.
// PS2 ParseGroupSpriteObj at 0x0043B478.
// Transpiler trace: 1029-3929 reads per object.
package vi

// VIGroupSprite is a multi-part model composed of SimpleSprites.
type VIGroupSprite struct {
	DictID uint32
	BBox   VIBox
	// Children: SimpleSprites parsed via SpriteArray (skip-stubbed)
}

const TypeGroupSprite = 0x2C00

// ParseGroupSprite reads a GroupSprite header from ESF tree data.
func ParseGroupSprite(data []byte, children map[uint16][]byte) (*VIGroupSprite, error) {
	gs := &VIGroupSprite{}
	if hdr, ok := children[0x2C10]; ok {
		pos := 0
		gs.DictID = ru32(hdr, &pos)
		gs.BBox.Min.X = rf32(hdr, &pos)
		gs.BBox.Min.Y = rf32(hdr, &pos)
		gs.BBox.Min.Z = rf32(hdr, &pos)
		gs.BBox.Max.X = rf32(hdr, &pos)
		gs.BBox.Max.Y = rf32(hdr, &pos)
		gs.BBox.Max.Z = rf32(hdr, &pos)
	}
	return gs, nil
}
