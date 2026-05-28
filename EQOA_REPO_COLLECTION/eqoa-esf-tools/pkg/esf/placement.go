package esf

// ArmorSlot identifies which equipment slot a variant mesh belongs to.
type ArmorSlot int

const (
	ArmorSlotChest  ArmorSlot = 0  // state channel offset 93
	ArmorSlotBracer ArmorSlot = 1  // 94
	ArmorSlotGloves ArmorSlot = 2  // 95
	ArmorSlotLegs   ArmorSlot = 3  // 96
	ArmorSlotBoots  ArmorSlot = 4  // 97
	ArmorSlotHelm   ArmorSlot = 5  // 98
	ArmorSlotRobe   ArmorSlot = 6  // 109
	ArmorSlotBase   ArmorSlot = -1 // always drawn (base body mesh)
)

// VariantTag associates a placement with an armor slot and set index.
// Meshes with Slot == ArmorSlotBase are always drawn. Others are drawn
// only when the entity's equipment byte for that slot matches Set.
type VariantTag struct {
	Slot ArmorSlot
	Set  byte // armor set index (matches state channel byte value)
}

// SpritePlacement describes the position, rotation, and scale of a sprite
// within a zone or composite sprite. It either holds a direct reference to
// a SimpleSprite or a dictionary ID that can be resolved via the ObjFile.
type SpritePlacement struct {
	Sprite   *SimpleSprite // direct reference (nil if using SpriteID)
	SpriteID int32         // dictionary ID for lookup
	Pos      Point
	Rot      Point
	Scale    float32
	Color    [4]byte
	Variant     *VariantTag // nil = base mesh (always drawn)
	IsFlora     bool        // true for FloraSprite actors (rendered separately with alpha blending)
	BakedColors []Color32   // per-vertex baked lighting (nil if none)
}

// GetSprite resolves the sprite, either from the direct reference or by
// looking up SpriteID in the file's dictionary. If the resolved object is
// a FloraSprite, sets sp.IsFlora = true so callers can render it separately.
func (sp *SpritePlacement) GetSprite(file *ObjFile) (*SimpleSprite, error) {
	if sp.Sprite != nil {
		return sp.Sprite, nil
	}
	obj, err := file.FindObject(sp.SpriteID)
	if err != nil {
		return nil, err
	}
	if obj == nil {
		return nil, nil
	}
	switch s := obj.(type) {
	case *FloraSprite:
		sp.IsFlora = true
		return &s.SimpleSprite, nil
	case *SimpleSprite:
		return s, nil
	}
	return nil, nil
}

// Transform applies scale, rotation, and translation to a point.
func (sp *SpritePlacement) Transform(p *Point) {
	if sp.Scale > 0 {
		p.MultiplyWith(sp.Scale)
	}
	p.Rotate(sp.Rot)
	p.AddTo(sp.Pos)
}

// GetScale returns the placement scale, defaulting to 1 if non-positive.
func (sp *SpritePlacement) GetScale() float32 {
	if sp.Scale > 0 {
		return sp.Scale
	}
	return 1
}

// CombineWith creates a new SpritePlacement by adding another placement's
// position and rotation to this one.
func (sp *SpritePlacement) CombineWith(other *SpritePlacement) *SpritePlacement {
	return &SpritePlacement{
		Sprite:   sp.Sprite,
		SpriteID: sp.SpriteID,
		Pos:      sp.Pos.Add(other.Pos),
		Rot:      sp.Rot.Add(other.Rot),
		Scale:    sp.Scale,
		Color:    sp.Color,
		Variant:  sp.Variant,
	}
}
