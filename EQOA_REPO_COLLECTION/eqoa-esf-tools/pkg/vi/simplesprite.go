// VISimpleSprite — basic single-mesh model.
// 9,381 in TUNARIA. Most zone objects (rocks, fences, decorations).
//
// Structure: header(0x2001, dictID+bbox+lod) + MaterialPal(0x1110) + PrimBuffer.
// PS2 ParseSimpleSpriteObj at 0x004358C0.
package vi

// VISimpleSprite is a single-mesh model with one material.
type VISimpleSprite struct {
	DictID      uint32
	BBox        VIBox
	LodDistance float32
}

const TypeSimpleSprite = 0x2000

// ParseSimpleSprite reads a SimpleSprite header from ESF child data.
func ParseSimpleSprite(data []byte, children map[uint16][]byte) (*VISimpleSprite, error) {
	ss := &VISimpleSprite{}
	if hdr, ok := children[0x2001]; ok {
		pos := 0
		ss.DictID = ru32(hdr, &pos)
		ss.BBox.Min.X = rf32(hdr, &pos)
		ss.BBox.Min.Y = rf32(hdr, &pos)
		ss.BBox.Min.Z = rf32(hdr, &pos)
		ss.BBox.Max.X = rf32(hdr, &pos)
		ss.BBox.Max.Y = rf32(hdr, &pos)
		ss.BBox.Max.Z = rf32(hdr, &pos)
		if pos < len(hdr) {
			ss.LodDistance = rf32(hdr, &pos)
		}
	}
	return ss, nil
}
