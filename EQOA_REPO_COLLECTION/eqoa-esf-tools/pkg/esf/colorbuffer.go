package esf

// Color32 holds an RGBA color with 8 bits per channel.
type Color32 struct {
	R, G, B, A byte
}

// ColorBuffer stores per-vertex RGBA color data.
// PS2: ParseColorBuffer (0x00434AB8), type 0x1220.
// Reads DictID, count (int32), then count × {R, G, B, A} bytes.
type ColorBuffer struct {
	info   *ObjInfo
	DictID int32
	Colors []Color32
}

func (cb *ColorBuffer) ObjInfo() *ObjInfo { return cb.info }

func (cb *ColorBuffer) Load(file *ObjFile) error {
	file.Seek(cb.info.Offset)
	cb.DictID = file.readInt32()
	count := file.readInt32()
	if count <= 0 || count > 65536 {
		return nil
	}
	cb.Colors = make([]Color32, count)
	for i := int32(0); i < count; i++ {
		cb.Colors[i] = Color32{
			R: file.readByte(),
			G: file.readByte(),
			B: file.readByte(),
			A: file.readByte(),
		}
	}
	return nil
}
