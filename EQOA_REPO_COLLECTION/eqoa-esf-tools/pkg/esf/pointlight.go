package esf

// PointLight represents an ESF PointLight (type 0x2B00).
// PS2: ParsePointLightObj (0x0043B8D0), dict type 8.
//
// Binary layout (24 bytes, no version-dependent fields):
//
//	DictID(int32) + Radius(float32) + Color(4 × float32: R,G,B,A)
//
// PS2 computes BBox as a cube centered at origin with half-extent = Radius.
// Optional Surface child (type 0x1000) is present in some files but skipped by PS2.
type PointLight struct {
	info   *ObjInfo
	DictID int32
	Radius float32
	Color  Color32F
}

func (pl *PointLight) ObjInfo() *ObjInfo { return pl.info }

func (pl *PointLight) Load(file *ObjFile) error {
	if pl.info.Size < 24 {
		return nil
	}
	file.Seek(pl.info.Offset)
	pl.DictID = file.readInt32()
	pl.Radius = file.readFloat32()
	pl.Color.R = file.readFloat32()
	pl.Color.G = file.readFloat32()
	pl.Color.B = file.readFloat32()
	pl.Color.A = file.readFloat32()
	return nil
}
