package esf

// Xm represents a PS2 IOP XM tracker module.
// PS2: ParseXmObj (0x00437040), type 0xB030.
//
// Children:
//   0xB040 (XmHeader v1): DictID(u32) + [v>0: Volume(f32) + Pan(f32)] + pattern data
//   0xB060 (XmSampleData): raw IOP sample data
//
// The pattern and sample data are in PS2 IOP proprietary format,
// not standard FastTracker 2 XM. Playback requires an IOP XM decoder.
type Xm struct {
	info        *ObjInfo
	DictID      int32
	Volume      float32
	Pan         float32
	PatternData []byte // raw 0xB040 pattern data (after DictID/volume/pan header)
	SampleData  []byte // raw 0xB060 IOP sample data
}

const (
	TypeXmHeader     uint16 = 0xB040
	TypeXmSampleData uint16 = 0xB060
)

func (x *Xm) ObjInfo() *ObjInfo { return x.info }

func (x *Xm) Load(file *ObjFile) error {
	x.Volume = 1.0

	// Header (0xB040)
	hdr := x.info.Child(TypeXmHeader)
	if hdr != nil {
		file.Seek(hdr.Offset)
		x.DictID = file.readInt32()
		if hdr.Version > 0 && hdr.Size >= 12 {
			x.Volume = file.readFloat32()
			x.Pan = file.readFloat32()
		}
		// Remaining data is XM pattern data (PS2 IOP format)
		headerBytes := 4
		if hdr.Version > 0 {
			headerBytes = 12
		}
		remaining := int(hdr.Size) - headerBytes
		if remaining > 0 {
			x.PatternData = file.RawBytes(int(hdr.Offset)+headerBytes, remaining)
		}
	}

	// Sample data (0xB060)
	smp := x.info.Child(TypeXmSampleData)
	if smp != nil && smp.Size > 0 {
		x.SampleData = file.RawBytes(smp.Offset, int(smp.Size))
	}

	return nil
}
