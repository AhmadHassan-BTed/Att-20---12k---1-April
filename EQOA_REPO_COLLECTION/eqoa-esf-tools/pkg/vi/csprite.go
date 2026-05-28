// VICSprite — PS2-accurate CSprite struct for character models.
// Generated from PS2 ParseCSpriteObj traces across CHAR.ESF + TUNARIA.ESF.
// Verified: 568 CHAR.ESF + 207 TUNARIA objects, ver=6 and ver=7.
package vi

// VICSprite is a character model (VICSprite on PS2).
// Contains skeleton, animations, equipment bindings, and sound.
type VICSprite struct {
	// Header (from child 0x2710)
	DictID       uint32
	BBox         VIBox
	SkelType     int32   // VICSpriteSkelType: skeleton topology
	DefaultScale float32 // model scale factor
	Race         int32   // VICSpriteRace enum
	Sex          int32   // VICSpriteSex: 0=male, 1=female
	ExtraFlag    int32   // ver >= 2 only

	// Skeleton hierarchy (from child 0x2400)
	Hierarchy VIHierarchy

	// Animations (from child 0x2610, contains 0x2600 children)
	Animations []VIHSpriteAnim

	// Reference maps (from children 0x5000)
	RefMaps []VIRefMap

	// Triggers (from child 0x2450)
	Triggers []int32

	// Skin bindings (from child 0x2900)
	SkinList []VISkinEntry

	// Animation play list (from child 0x2910)
	PlayList []VIPlayEntry

	// Node ID list (from child 0x2915)
	NodeIDs []VINodeIDEntry

	// Attachment slots (from child 0x2920)
	ASlots []VIASlotEntry

	// Texture slots (from child 0x2930)
	TSlots []int32

	// Continuous sound (from child 0x2940)
	ContSound VIContSound
}

// VIHierarchy is the bone hierarchy for a skeletal model.
// Parsed from child 0x2400 (HSpriteHierarchy).
// PS2 read sequence: int32(numNodes) + per node:
//   int32(parent) + 7×float32(bindPose: pos.xyz + quat.xyzw) + 3×float32(fixScale.xyz)
type VIHierarchy struct {
	Nodes []VIBoneNode
}

// VIBoneNode is one bone in the skeleton hierarchy.
type VIBoneNode struct {
	Parent int32
	// Bind pose
	PosX, PosY, PosZ       float32
	QuatX, QuatY, QuatZ, QuatW float32
	// Fix scale (per-bone scale correction)
	ScaleX, ScaleY, ScaleZ float32
}

// VIRefMap is a key-value reference map.
// Parsed from child 0x5000.
// PS2 read: uint32(dictID) + int32(count) + count × (int32 key + int32 value)
type VIRefMap struct {
	DictID  uint32
	Entries []VIRefMapEntry
}

// VIRefMapEntry is one key-value pair in a RefMap.
type VIRefMapEntry struct {
	Key   int32
	Value int32
}

// VISkinEntry binds a mesh to a skin index.
// Parsed from child 0x2900 (CSpriteSkinList).
// PS2 read: int32(count) + count × (uint32 dictID + int32 skinIndex)
type VISkinEntry struct {
	DictID    uint32
	SkinIndex int32
}

// VIPlayEntry defines an animation play for the CSprite.
// Parsed from child 0x2910 (CSpritePlayList).
// PS2 struct layout (52 bytes at sprite+0x1A8 + index*52):
//
//	+0x00  uint32 dictID (animation play reference)
//	+0x04  uint32 animDictID (ver >= 2 only; 0 for ver < 2)
//	+0x08  float32 playSpeed
//	+0x0C  int32 playbackType (0=loop, 1=once, etc.)
//	+0x18  int32 refMapIndex (from FindTyped on sp+0x30 dictID)
//	+0x1C  int32 animIndex (from FindTyped on sp+0x34 dictID)
//	+0x20  float32 blendIn (ver >= 2)
//	+0x24  float32 blendOut (ver >= 2)
//	+0x28  float32 startPhase (ver >= 3)
//	+0x2C  float32 endPhase (ver >= 3)
//
// PS2 read sequence (version-dependent):
//   ReadUint32(dictID)
//   if ver >= 2: ReadUint32(animDictID)
//   ReadInt32(priority/nodeCount) → $s2
//   ReadFloat32(playSpeed)
//   ReadInt32(playbackType) → $s5
//   if ver >= 2: ReadUint32(refMapDictID), ReadFloat32(blendIn)
//   if ver >= 2: ReadFloat32(blendOut) [via additional check]
//   if ver >= 3: ReadUint32(animDictID2), ReadFloat32(startPhase), ReadFloat32(endPhase)
type VIPlayEntry struct {
	DictID       uint32
	AnimDictID   uint32  // ver >= 2
	Priority     int32
	PlaySpeed    float32
	PlaybackType int32
	RefMapDictID uint32  // ver >= 2
	BlendIn      float32 // ver >= 2
	BlendOut     float32 // ver >= 2
	AnimDictID2  uint32  // ver >= 3
	StartPhase   float32 // ver >= 3
	EndPhase     float32 // ver >= 3
}

// VINodeIDEntry maps a node index to a name/ID.
// Parsed from child 0x2915 (CSpriteNodeIDList).
// PS2 read: int32(count) + count × (int32 nodeIndex + uint32 nameID)
type VINodeIDEntry struct {
	NodeIndex int32
	NameID    uint32
}

// VIASlotEntry defines an attachment slot (weapon/shield/helm mount point).
// Parsed from child 0x2920 (CSpriteASlotList).
// PS2 read: int32(count) + count × (int32 slotType + uint32 dictID)
type VIASlotEntry struct {
	SlotType int32
	DictID   uint32
}

// VIContSound defines continuous sound for the model.
// Parsed from child 0x2940 (CSpriteContSound).
// PS2 read: uint32(dictID) + float32(volume)
type VIContSound struct {
	DictID uint32
	Volume float32
}

// CSprite type codes
const (
	TypeCSprite       = 0x2700
	TypeCSpriteHeader = 0x2710
)
