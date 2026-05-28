package esf

import (
	"encoding/binary"
	"fmt"
	"math"
)

// SpellEvent is a 44-byte event in a SpellEffect event list.
// Layout: EventType(4) + Fields[4](4×4=16) + Floats[6](6×4=24) = 44 bytes.
type SpellEvent struct {
	EventType uint32
	Fields    [4]int32
	Floats    [6]float32
}

// SpellEffect represents a parsed SpellEffect (0xC200) object.
// It contains three event lists: Caster, Target, Area.
type SpellEffect struct {
	info       *ObjInfo
	DictID     int32
	Version    int16
	EventLists [3][]SpellEvent // [0]=Caster, [1]=Target, [2]=Area
}

func (se *SpellEffect) ObjInfo() *ObjInfo { return se.info }

func (se *SpellEffect) Load(file *ObjFile) error {
	// Find header child (0xC210) for DictID
	header := se.info.Child(TypeSpellEffectHeader)
	if header != nil {
		se.DictID = header.DictID
	}
	se.Version = se.info.Version

	// Find data child (0xC220) for event lists
	dataChild := se.info.Child(TypeSpellEffectData)
	if dataChild == nil {
		return nil // no data — empty spell effect
	}

	chunk := file.RawBytes(dataChild.Offset, int(dataChild.Size))
	if len(chunk) < 4 {
		return nil
	}

	// Parse interleaved 3-list format:
	// count0(4B), events0[count0×44B], count1(4B), events1[...], count2(4B), events2[...]
	pos := 0
	for i := 0; i < 3; i++ {
		if pos+4 > len(chunk) {
			break
		}
		count := int(binary.LittleEndian.Uint32(chunk[pos:]))
		pos += 4

		events := make([]SpellEvent, 0, count)
		for j := 0; j < count; j++ {
			if pos+44 > len(chunk) {
				break
			}
			var ev SpellEvent
			ev.EventType = binary.LittleEndian.Uint32(chunk[pos:])
			for k := 0; k < 4; k++ {
				ev.Fields[k] = int32(binary.LittleEndian.Uint32(chunk[pos+4+k*4:]))
			}
			for k := 0; k < 6; k++ {
				ev.Floats[k] = math.Float32frombits(binary.LittleEndian.Uint32(chunk[pos+20+k*4:]))
			}
			events = append(events, ev)
			pos += 44
		}
		se.EventLists[i] = events
	}
	return nil
}

// TotalEvents returns the total event count across all three lists.
func (se *SpellEffect) TotalEvents() int {
	return len(se.EventLists[0]) + len(se.EventLists[1]) + len(se.EventLists[2])
}

// Color32F is an RGBA color with float32 components (0.0-1.0).
type Color32F struct {
	R, G, B, A float32
}

// ParticleMotif is a named variant within a ParticleDefinition.
// Each motif has its own set of particle attributes.
type ParticleMotif struct {
	Name       string
	Attributes ParticleAttributes
}

// ParticleAttributes holds all per-particle simulation parameters.
// Matches the VIParticleAttributes struct layout from PS2 SUPPORT module.
type ParticleAttributes struct {
	Friction        float32
	Birthrate       float32 // particles/sec (1.0-1000.0)
	BirthrateVar    float32
	Lifespan        float32 // seconds (0.06-5.0)
	LifespanVar     float32
	Velocity        float32
	VelocityVar     float32
	StartSize       float32 // units (diameter, halved to radius at birth)
	StartSizeVar    float32
	EndSize         float32
	EndSizeVar      float32
	InheritVelocity float32
	DeltaSpawn      float32
	StartColorVar   Color32F
	EndColorVar     Color32F
	Gradient        [32]Color32F // birth→death color ramp
	GradientRepeat  float32
	InnerOffset     [3]float32 // XYZ
	InnerHprVar     [3]float32
	OuterOffset     [3]float32
	OuterHprVar     [3]float32
	NozzleAxis      [3]float32
	NozzleHprVar    [3]float32
	GravityOn       bool // when true, gravity = -5.0
}

// ParticleDefinition is a fully-parsed ParticleDefinition (0xC000) object.
// Contains texture reference, blend mode, and per-motif particle attributes.
type ParticleDefinition struct {
	info      *ObjInfo
	DictID    int32
	TexDictID int32
	BlendMode int32 // 0=Normal, 1=Add
	ZWrite    bool
	ZTest     bool
	TexConfig int32
	Version   int16
	Base      ParticleAttributes // default motif
	Motifs    []ParticleMotif    // extra motifs (SpellMotif1, etc.)
}

func (pd *ParticleDefinition) ObjInfo() *ObjInfo { return pd.info }

// EmitShape constants matching PS2 VIParticleDefinition::GetEmitType().
const (
	EmitPoint  = 1
	EmitLine   = 2
	EmitRing   = 3
	EmitSphere = 4
)

// EmitShape returns the emit shape type for this ParticleDefinition.
// PS2 stores emitType at VIParticleDefinitionEx+0x2F8 (int32), defaulting to
// 1 (Point) in the constructor. Decompilation of the entire SUPPORT module
// confirms no code ever calls SetEmitType — the field stays at 1 for all PDs.
// The Line/Ring/Sphere code paths in VIParticleEmitter::Update (which use a
// pre-computed position table at emitter+0x37C and a colorShapeIndex) are
// dead code in EQOA Frontiers.
// See claude-notes/particle-emit-shape.md for full decompilation.
func (pd *ParticleDefinition) EmitShape() int {
	return EmitPoint
}

func (pd *ParticleDefinition) Load(file *ObjFile) error {
	// Find header child (0xC010) for DictID
	header := pd.info.Child(TypeParticleDefHeader)
	if header != nil {
		pd.DictID = header.DictID
	}
	pd.Version = pd.info.Version

	// Find data child (0xC020)
	dataChild := pd.info.Child(TypeParticleDefData)
	if dataChild == nil || dataChild.Size < 24 {
		return nil
	}

	chunk := file.RawBytes(dataChild.Offset, int(dataChild.Size))
	if len(chunk) < 24 {
		return nil
	}

	// Header (24 bytes)
	pd.TexDictID = int32(binary.LittleEndian.Uint32(chunk[0:]))
	pd.BlendMode = int32(binary.LittleEndian.Uint32(chunk[4:]))
	pd.ZWrite = binary.LittleEndian.Uint32(chunk[8:]) != 0
	pd.ZTest = binary.LittleEndian.Uint32(chunk[12:]) != 0
	pd.TexConfig = int32(binary.LittleEndian.Uint32(chunk[16:]))
	numMotifs := int(binary.LittleEndian.Uint32(chunk[20:]))

	pos := 24
	// Parse base motif attributes.
	// PS2 HasMoreData returns the 0xC020 child version, NOT the 0xC000 parent version.
	// GravityOn is only read when 0xC020 version > 0 (499 PDs are v0, 1195 are v1).
	hasGravity := dataChild.Version > 0
	pos = parseParticleAttributes(chunk, pos, hasGravity, &pd.Base)

	// Parse extra motifs
	for i := 0; i < numMotifs && pos < len(chunk); i++ {
		var m ParticleMotif
		// Motif name: 32 bytes null-terminated ASCII
		if pos+32 > len(chunk) {
			break
		}
		nameBytes := chunk[pos : pos+32]
		for j, b := range nameBytes {
			if b == 0 {
				m.Name = string(nameBytes[:j])
				break
			}
		}
		pos += 32
		pos = parseParticleAttributes(chunk, pos, hasGravity, &m.Attributes)
		pd.Motifs = append(pd.Motifs, m)
	}

	return nil
}

// parseParticleAttributes reads one motif's worth of attribute data from chunk at pos.
// PS2 ParseParticleDefinition (0x0043CA08) always reads Friction (13 floats unconditionally).
// GravityOn (int32) is only read when the 0xC020 data child version > 0.
// v0 format: 672 bytes/motif (no GravityOn), v1 format: 676 bytes/motif (with GravityOn).
func parseParticleAttributes(chunk []byte, pos int, hasGravity bool, a *ParticleAttributes) int {
	rf := func() float32 {
		if pos+4 > len(chunk) {
			return 0
		}
		v := math.Float32frombits(binary.LittleEndian.Uint32(chunk[pos:]))
		pos += 4
		return v
	}
	ri := func() int32 {
		if pos+4 > len(chunk) {
			return 0
		}
		v := int32(binary.LittleEndian.Uint32(chunk[pos:]))
		pos += 4
		return v
	}

	// PS2 always reads Friction first (unconditional, not version-dependent).
	a.Friction = rf()
	a.Birthrate = rf()
	a.BirthrateVar = rf()
	a.Lifespan = rf()
	a.LifespanVar = rf()
	a.Velocity = rf()
	a.VelocityVar = rf()
	a.StartSize = rf()
	a.StartSizeVar = rf()
	a.EndSize = rf()
	a.EndSizeVar = rf()
	a.InheritVelocity = rf()
	a.DeltaSpawn = rf()
	// StartColorVar (RGBA)
	a.StartColorVar = Color32F{rf(), rf(), rf(), rf()}
	// EndColorVar (RGBA)
	a.EndColorVar = Color32F{rf(), rf(), rf(), rf()}
	// Gradient (32 × RGBA)
	for i := 0; i < 32; i++ {
		a.Gradient[i] = Color32F{rf(), rf(), rf(), rf()}
	}
	a.GradientRepeat = rf()
	// Vec3 fields
	for i := 0; i < 3; i++ {
		a.InnerOffset[i] = rf()
	}
	for i := 0; i < 3; i++ {
		a.InnerHprVar[i] = rf()
	}
	for i := 0; i < 3; i++ {
		a.OuterOffset[i] = rf()
	}
	for i := 0; i < 3; i++ {
		a.OuterHprVar[i] = rf()
	}
	for i := 0; i < 3; i++ {
		a.NozzleAxis[i] = rf()
	}
	for i := 0; i < 3; i++ {
		a.NozzleHprVar[i] = rf()
	}
	// PS2: GravityOn only read when 0xC020 version > 0. Defaults to false (0) for v0.
	if hasGravity {
		a.GravityOn = ri() != 0
	}
	return pos
}

// SpellEventName returns the human-readable name for a spell event type.
// Names derived from MIPS jump table at 0x004EBA70 in ParseSpellEffectObj.
func SpellEventName(t uint32) string {
	switch t {
	case 1:
		return "Wait"
	case 2:
		return "SetAction"
	case 3:
		return "CreateEmitter"
	case 4:
		return "DestroyEmitter"
	case 5:
		return "SetEmitterAttractor"
	case 6:
		return "SetEmitterMotif"
	case 7:
		return "PlayTargetInstant"
	case 8:
		return "PlayTargetProjectile"
	case 9:
		return "CreateSprite"
	case 10:
		return "DestroySprite"
	case 11:
		return "CreateSound"
	case 12:
		return "DestroySound"
	case 13:
		return "PlayTargetLightning"
	case 14:
		return "FadeSprite"
	case 15:
		return "PlayAttack"
	case 16:
		return "VibrateController"
	case 17:
		return "ShakeCamera"
	default:
		return fmt.Sprintf("Unknown(%d)", t)
	}
}

// SpellEventListName returns the name for an event list index.
func SpellEventListName(i int) string {
	switch i {
	case 0:
		return "Caster"
	case 1:
		return "Target"
	case 2:
		return "Area"
	default:
		return "Unknown"
	}
}

// ParticleSprite is a zone-placed particle effect (campfire smoke, waterfall mist, etc.).
// PS2: ParseParticleSpriteObj (0x0043BCF0), type 0xC100, sprite type 10, dict type 19.
// Contains a header (0xC101: DictID, +extra int32 if ver>0) and an embedded
// ParticleDefinition (0xC000) child.
type ParticleSprite struct {
	info              *ObjInfo
	DictID            int32
	ParticleDefDictID int32              // external ParticleDefinition DictID reference (header ver>0)
	ParticleDef       *ParticleDefinition // embedded ParticleDefinition child
	BBox              Box
}

func (ps *ParticleSprite) ObjInfo() *ObjInfo { return ps.info }

func (ps *ParticleSprite) Load(file *ObjFile) error {
	// Read header child (0xC101)
	header := ps.info.Child(TypeParticleSpriteHeader)
	if header != nil {
		file.Seek(header.Offset)
		ps.DictID = file.readInt32()
		// PS2: if header version != 0, read extra int32 (ParticleDefinition DictID ref)
		if header.Version != 0 {
			ps.ParticleDefDictID = file.readInt32()
		}
	}

	// Default BBox: center(0,0,0) ± radius(1.0)
	// PS2 initializes sprite+44=1.0f then subtracts/adds to get BBox
	ps.BBox = Box{
		MinX: -1, MinY: -1, MinZ: -1,
		MaxX: 1, MaxY: 1, MaxZ: 1,
	}

	// Load embedded ParticleDefinition child (0xC000)
	pdInfo := ps.info.Child(TypeParticleDefinition)
	if pdInfo != nil {
		obj, err := file.GetObject(pdInfo)
		if err == nil && obj != nil {
			if pd, ok := obj.(*ParticleDefinition); ok {
				ps.ParticleDef = pd
			}
		}
	}

	return nil
}
