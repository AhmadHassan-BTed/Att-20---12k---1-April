// VILODSprite — level-of-detail sprite variant.
// 200 in TUNARIA. References child sprites at different quality levels.
//
// LODSprite is a container that holds multiple quality variants of the same
// model. The PS2 switches between them based on camera distance.
// PS2 ParseLODSpriteObj at 0x0043AC70.
package vi

// VILODSprite defines LOD distance thresholds.
type VILODSprite struct {
	DictID    uint32
	NumLevels int32
	// Level distances are encoded in the children
}

const TypeLODSprite = 0x2E00
