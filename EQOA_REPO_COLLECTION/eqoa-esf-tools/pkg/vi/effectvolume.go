// VIEffectVolumeSprite — environmental effects (fog, dust, leaves).
// 1,096 in TUNARIA. Defines volumetric effect regions.
//
// Structure: header(0xC310, dictID+bbox) + EffectData(0xC320).
// PS2 ParseEffectVolumeSpriteObj at 0x0043C4A0.
package vi

// VIEffectVolumeSprite defines a volumetric environmental effect.
type VIEffectVolumeSprite struct {
	DictID uint32
	BBox   VIBox
}

const TypeEffectVolumeSprite = 0xC300

// ParseEffectVolumeSprite reads an EffectVolumeSprite header.
func ParseEffectVolumeSprite(data []byte, children map[uint16][]byte) (*VIEffectVolumeSprite, error) {
	ev := &VIEffectVolumeSprite{}
	if hdr, ok := children[0xC310]; ok {
		pos := 0
		ev.DictID = ru32(hdr, &pos)
		ev.BBox.Min.X = rf32(hdr, &pos)
		ev.BBox.Min.Y = rf32(hdr, &pos)
		ev.BBox.Min.Z = rf32(hdr, &pos)
		ev.BBox.Max.X = rf32(hdr, &pos)
		ev.BBox.Max.Y = rf32(hdr, &pos)
		ev.BBox.Max.Z = rf32(hdr, &pos)
	}
	return ev, nil
}
