// VIHSprite — PS2-accurate HSprite struct for zone/object models.
// Generated from PS2 ParseHSpriteObj traces on TUNARIA.ESF.
// Verified: 153 objects, ver=2.
package vi

// VIHSprite is a hierarchical sprite (multi-part model with skeleton).
// Used for zone objects (buildings, trees, rocks) in TUNARIA.
type VIHSprite struct {
	// Header (from child 0x2210)
	DictID uint32
	BBox   VIBox

	// Skeleton hierarchy (from child 0x2400)
	Hierarchy VIHierarchy

	// Reference map (from child 0x5000)
	RefMaps []VIRefMap

	// Triggers (from child 0x2450)
	Triggers []int32

	// Attachment points (from child 0x2500)
	Attachments []VIAttachment

	// Animation (from child 0x2600 — parsed by ParseHSpriteAnim)
	Animation *VIHSpriteAnim
}

// VIAttachment is an attachment point on an HSprite.
// Parsed from child 0x2500 (ParseHSpriteAttachments).
// PS2 read sequence per attachment:
//   int32(nodeIndex) + int32(type) + uint32(dictID) + int32(flags)
type VIAttachment struct {
	NodeIndex int32
	Type      int32
	DictID    uint32
	Flags     int32
}

// HSprite type codes
const (
	TypeHSprite       = 0x2200
	TypeHSpriteHeader = 0x2210
)
