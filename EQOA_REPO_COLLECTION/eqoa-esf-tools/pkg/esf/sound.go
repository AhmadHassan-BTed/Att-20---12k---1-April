package esf

// SoundSprite represents a positional sound effect in a zone.
// PS2: ParseSoundSpriteObj (0x0043BAB0), type 0xB100.
//
// Layout (0xB101 header):
//   DictID(u32) + [v>0: InnerRadius(f32) + OuterRadius(f32)]
//
// Children: one Adpcm (0xB000) containing the sound data.
type SoundSprite struct {
	info        *ObjInfo
	DictID      int32
	InnerRadius float32
	OuterRadius float32
	Sound       *Adpcm // child Adpcm data (nil if not present)
}

func (s *SoundSprite) ObjInfo() *ObjInfo { return s.info }

func (s *SoundSprite) Load(file *ObjFile) error {
	// Header child (0xB101) contains the sprite parameters.
	hdr := s.info.Child(TypeSoundSpriteHeader)
	if hdr != nil {
		file.Seek(hdr.Offset)
		s.DictID = file.readInt32()
		if hdr.Version > 0 && hdr.Size >= 12 {
			s.InnerRadius = file.readFloat32()
			s.OuterRadius = file.readFloat32()
		}
	}

	// Child Adpcm (0xB000) holds the actual sound data.
	adpcmInfo := s.info.Child(TypeAdpcm)
	if adpcmInfo != nil {
		obj, err := file.GetObject(adpcmInfo)
		if err == nil {
			if adpcm, ok := obj.(*Adpcm); ok {
				s.Sound = adpcm
			}
		}
	}

	return nil
}

// Adpcm represents a PS2 SPU2-ADPCM sound clip.
// PS2: ParseAdpcmObj (0x00436C68), type 0xB000.
//
// Children:
//   0xB010 (AdpcmHeader): DictID + channels + numBlocks + sampleRate + volume + pan + loopStart
//   0xB020 (AdpcmData): raw VAG ADPCM blocks
type Adpcm struct {
	info       *ObjInfo
	DictID     int32
	Channels   int32
	NumBlocks  int32
	SampleRate int32
	Volume     float32
	Pan        float32
	LoopStart  int32
	RawVAG     []byte // raw SPU2-ADPCM data from 0xB020
}

func (a *Adpcm) ObjInfo() *ObjInfo { return a.info }

func (a *Adpcm) Load(file *ObjFile) error {
	a.SampleRate = 11025 // default
	a.Volume = 1.0

	// Header (0xB010)
	hdr := a.info.Child(TypeAdpcmHeader)
	if hdr != nil && hdr.Size >= 16 {
		file.Seek(hdr.Offset)
		a.DictID = file.readInt32()
		a.Channels = file.readInt32()
		a.NumBlocks = file.readInt32()
		a.SampleRate = file.readInt32()
		if hdr.Size >= 28 {
			a.Volume = file.readFloat32()
			a.Pan = file.readFloat32()
			a.LoopStart = file.readInt32()
		}
	}

	// Raw VAG data (0xB020)
	data := a.info.Child(TypeAdpcmData)
	if data != nil && data.Size > 0 {
		a.RawVAG = file.RawBytes(data.Offset, int(data.Size))
	}

	return nil
}
