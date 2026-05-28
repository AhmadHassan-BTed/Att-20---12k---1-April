package esf

// StreamAudioSprite represents a streaming audio region in a zone.
// PS2: ParseStreamAudioSpriteObj (0x0043C0A8), type 0x2E00.
//
// Layout:
//   DictID(u32) + BBox(6×f32) + Path(string16) + Loop(i32) + Priority(i32)
type StreamAudioSprite struct {
	info     *ObjInfo
	DictID   int32
	MinX     float32
	MinY     float32
	MinZ     float32
	MaxX     float32
	MaxY     float32
	MaxZ     float32
	Path     string // relative BGM file path (e.g. "data\freeport.bgm")
	Loop     bool
	Priority int32
}

func (s *StreamAudioSprite) ObjInfo() *ObjInfo { return s.info }

func (s *StreamAudioSprite) Load(file *ObjFile) error {
	file.Seek(s.info.Offset)
	s.DictID = file.readInt32()
	s.MinX = file.readFloat32()
	s.MinY = file.readFloat32()
	s.MinZ = file.readFloat32()
	s.MaxX = file.readFloat32()
	s.MaxY = file.readFloat32()
	s.MaxZ = file.readFloat32()
	path, _ := file.readString()
	s.Path = path
	s.Loop = file.readInt32() != 0
	s.Priority = file.readInt32()
	return nil
}
