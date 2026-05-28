// ParseCSprite — hand-written from PS2 trace analysis.
// Reads the CSprite header + all sub-parser children.
package vi

import (
	"encoding/binary"
	"fmt"
)

// ChildInfo holds a child's body data and version for version-dependent parsing.
type ChildInfo struct {
	Data    []byte
	Version int16
}

// ParseCSpriteFull reads a CSprite from ESF tree data.
// children = map of child type → body data + version info.
func ParseCSpriteFull(rootData []byte, children map[uint16][]byte, childVersions ...map[uint16]int16) (*VICSprite, error) {
	// Get version for a child type
	getVer := func(typ uint16) int16 {
		if len(childVersions) > 0 {
			if v, ok := childVersions[0][typ]; ok { return v }
		}
		return 0
	}
	_ = getVer
	cs := &VICSprite{}

	// Header child 0x2710
	if hdr, ok := children[TypeCSpriteHeader]; ok {
		pos := 0
		cs.DictID = ru32(hdr, &pos)
		cs.BBox.Min.X = rf32(hdr, &pos)
		cs.BBox.Min.Y = rf32(hdr, &pos)
		cs.BBox.Min.Z = rf32(hdr, &pos)
		cs.BBox.Max.X = rf32(hdr, &pos)
		cs.BBox.Max.Y = rf32(hdr, &pos)
		cs.BBox.Max.Z = rf32(hdr, &pos)
		cs.SkelType = ri32(hdr, &pos)
		cs.DefaultScale = rf32(hdr, &pos)
		cs.Race = ri32(hdr, &pos)
		cs.Sex = ri32(hdr, &pos)
		if pos < len(hdr) {
			cs.ExtraFlag = ri32(hdr, &pos)
		}
	}

	// Hierarchy child 0x2400
	if data, ok := children[0x2400]; ok {
		cs.Hierarchy = parseHierarchy(data)
	}

	// Triggers child 0x2450
	if data, ok := children[0x2450]; ok {
		cs.Triggers = parseTriggers(data)
	}

	// SkinList child 0x2900
	if data, ok := children[0x2900]; ok {
		cs.SkinList = parseSkinList(data)
	}

	// PlayList child 0x2910 (version-dependent fields)
	if data, ok := children[0x2910]; ok {
		cs.PlayList = parsePlayListV(data, getVer(0x2910))
	}

	// NodeIDList child 0x2915
	if data, ok := children[0x2915]; ok {
		cs.NodeIDs = parseNodeIDList(data)
	}

	// ASlotList child 0x2920
	if data, ok := children[0x2920]; ok {
		cs.ASlots = parseASlotList(data)
	}

	// TSlotList child 0x2930
	if data, ok := children[0x2930]; ok {
		cs.TSlots = parseTSlotList(data)
	}

	// ContSound child 0x2940
	if data, ok := children[0x2940]; ok {
		pos := 0
		cs.ContSound.DictID = ru32(data, &pos)
		cs.ContSound.Volume = rf32(data, &pos)
	}

	return cs, nil
}

func parseHierarchy(data []byte) VIHierarchy {
	pos := 0
	numNodes := ri32(data, &pos)
	if numNodes < 0 || numNodes > 10000 {
		return VIHierarchy{}
	}
	h := VIHierarchy{Nodes: make([]VIBoneNode, numNodes)}
	for i := int32(0); i < numNodes && pos < len(data); i++ {
		n := &h.Nodes[i]
		n.Parent = ri32(data, &pos)
		n.PosX = rf32(data, &pos)
		n.PosY = rf32(data, &pos)
		n.PosZ = rf32(data, &pos)
		n.QuatX = rf32(data, &pos)
		n.QuatY = rf32(data, &pos)
		n.QuatZ = rf32(data, &pos)
		n.QuatW = rf32(data, &pos)
		n.ScaleX = rf32(data, &pos)
		n.ScaleY = rf32(data, &pos)
		n.ScaleZ = rf32(data, &pos)
	}
	return h
}

func parseTriggers(data []byte) []int32 {
	pos := 0
	count := ri32(data, &pos)
	if count < 0 || count > 10000 { return nil }
	triggers := make([]int32, count)
	for i := int32(0); i < count && pos < len(data); i++ {
		triggers[i] = ri32(data, &pos)
	}
	return triggers
}

func parseSkinList(data []byte) []VISkinEntry {
	pos := 0
	count := ri32(data, &pos)
	if count < 0 || count > 10000 { return nil }
	skins := make([]VISkinEntry, count)
	for i := int32(0); i < count && pos < len(data); i++ {
		skins[i].DictID = ru32(data, &pos)
		skins[i].SkinIndex = ri32(data, &pos)
	}
	return skins
}

// parsePlayListV parses with version-dependent fields.
// ver comes from the 0x2910 child's ObjInfo.Version.
func parsePlayListV(data []byte, ver int16) []VIPlayEntry {
	pos := 0
	count := ri32(data, &pos)
	if count < 0 || count > 10000 { return nil }
	plays := make([]VIPlayEntry, 0, count)
	for i := int32(0); i < count && pos < len(data); i++ {
		var p VIPlayEntry
		p.DictID = ru32(data, &pos)
		if ver >= 2 {
			p.AnimDictID = ru32(data, &pos)
		}
		p.Priority = ri32(data, &pos)
		p.PlaySpeed = rf32(data, &pos)
		p.PlaybackType = ri32(data, &pos)
		if ver >= 2 {
			p.RefMapDictID = ru32(data, &pos)
			p.BlendIn = rf32(data, &pos)
		}
		if ver >= 2 {
			p.BlendOut = rf32(data, &pos)
		}
		if ver >= 3 {
			p.AnimDictID2 = ru32(data, &pos)
			p.StartPhase = rf32(data, &pos)
			p.EndPhase = rf32(data, &pos)
		}
		plays = append(plays, p)
	}
	return plays
}

func parseNodeIDList(data []byte) []VINodeIDEntry {
	pos := 0
	count := ri32(data, &pos)
	if count < 0 || count > 10000 { return nil }
	ids := make([]VINodeIDEntry, count)
	for i := int32(0); i < count && pos < len(data); i++ {
		ids[i].NodeIndex = ri32(data, &pos)
		ids[i].NameID = ru32(data, &pos)
	}
	return ids
}

func parseASlotList(data []byte) []VIASlotEntry {
	pos := 0
	count := ri32(data, &pos)
	if count < 0 || count > 10000 { return nil }
	slots := make([]VIASlotEntry, count)
	for i := int32(0); i < count && pos < len(data); i++ {
		slots[i].SlotType = ri32(data, &pos)
		slots[i].DictID = ru32(data, &pos)
	}
	return slots
}

func parseTSlotList(data []byte) []int32 {
	pos := 0
	count := ri32(data, &pos)
	if count < 0 || count > 10000 { return nil }
	slots := make([]int32, count)
	for i := int32(0); i < count && pos < len(data); i++ {
		slots[i] = ri32(data, &pos)
	}
	return slots
}

// Verify type codes at compile time
var _ = fmt.Errorf
var _ = binary.LittleEndian
