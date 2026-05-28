// VISpellEffect — spell visual effect definition.
// 238 in SPELLFX.CSF. Defines particle/sound/animation events for spells.
//
// PS2 ParseSpellEffectObj at 0x0043CE80.
// Structure: header(0xC210, dictID) + event array(0xC220, bulk Read_PUci).
// Transpiler trace: 6 reads per object (dictID + count + event data as bulk bytes).
package vi

// VISpellEffect defines a spell's visual/audio events.
type VISpellEffect struct {
	DictID    uint32
	NumEvents int32
	Events    []VISpellEvent
}

// VISpellEvent is one event in a spell effect sequence.
// 44 bytes on PS2 (from claude-notes/spell-effect-system.md):
//
//	+0x00  uint32 eventType
//	+0x04  int32  Fields[0] (emitter/sprite/sound ID)
//	+0x08  int32  Fields[1] (DictID reference, duration, animation ID)
//	+0x0C  int32  Fields[2] (duration ms, actor override bits)
//	+0x10  int32  Fields[3] (lifetime ms, flight mode bits)
//	+0x14  float32 Floats[0] (X offset, volume, opacity)
//	+0x18  float32 Floats[1] (Y offset, color G)
//	+0x1C  float32 Floats[2] (Z offset, color B)
//	+0x20  float32 Floats[3] (scale, spread)
//	+0x24  float32 Floats[4] (parameter)
//	+0x28  float32 Floats[5] (unused)
type VISpellEvent struct {
	EventType uint32
	Fields    [4]int32
	Floats    [6]float32
}

const TypeSpellEffect = 0xC200

// ParseSpellEffect reads a SpellEffect from ESF child data.
// Child 0xC210 has the dictID. Child 0xC220 has event count + bulk event data.
func ParseSpellEffectFull(children map[uint16][]byte) *VISpellEffect {
	se := &VISpellEffect{}

	if hdr, ok := children[0xC210]; ok && len(hdr) >= 4 {
		pos := 0
		se.DictID = ru32(hdr, &pos)
	}

	if evData, ok := children[0xC220]; ok && len(evData) >= 4 {
		pos := 0
		se.NumEvents = ri32(evData, &pos)
		if se.NumEvents < 0 || se.NumEvents > 1000 {
			se.NumEvents = 0
		}
		se.Events = make([]VISpellEvent, se.NumEvents)
		for i := int32(0); i < se.NumEvents && pos+44 <= len(evData); i++ {
			se.Events[i].EventType = ru32(evData, &pos)
			for j := 0; j < 4; j++ {
				se.Events[i].Fields[j] = ri32(evData, &pos)
			}
			for j := 0; j < 6; j++ {
				se.Events[i].Floats[j] = rf32(evData, &pos)
			}
		}
	}

	return se
}
