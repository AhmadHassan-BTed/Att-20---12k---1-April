package esf

// PointSprite is a billboard marker rendered at a world position.
// PS2: ParsePointSpriteObj (0x0043BFB0), type 0x2D00.
// Used for zone indicators, quest markers, effect anchors.
// Renders via VIScene::GetPointUISprite (a shared UI sprite billboard).
//
// PS2 ParsePointSpriteObj reads:
//   1. DictID (uint32) — for dictionary registration
//   2. PointType (int32) — VIPointType enum, passed to VIPointSprite::Init
//
// VIPointSprite::Init stores PointType at this+0x40, sets scale to 0.1,
// and computes bounding box from position.
type PointSprite struct {
	info      *ObjInfo
	PointType int32 // VIPointType enum
}

func (ps *PointSprite) ObjInfo() *ObjInfo { return ps.info }

func (ps *PointSprite) Load(file *ObjFile) error {
	// The object data starts at info.Offset with DictID then PointType
	file.Seek(ps.info.Offset)
	_ = file.readInt32()           // DictID (already tracked by ObjInfo)
	ps.PointType = file.readInt32() // VIPointType enum
	return nil
}
