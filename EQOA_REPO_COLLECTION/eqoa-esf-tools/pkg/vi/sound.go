// VISound — audio resource (ADPCM or XM tracker).
// 2,913 in CHAR.ESF, 8 in SPELLFX.CSF.
//
// PS2 ParseSound dispatches to ParseAdpcmObj (0x00436C68) or ParseXmObj (0x00437040).
// ADPCM: uint32(dictID) + int32(sampleRate) + int32(numChannels) + int32(numSamples)
//        + float32(volume) + float32(minDist) + int32(flags)
//        + raw audio data (Read_PUci)
// XM: uint32(dictID) + float32(volume) + float32(minDist)
//     + raw XM data (Read_PUci)
package vi

// VISound represents an audio resource.
type VISound struct {
	DictID      uint32
	Type        VISoundType // ADPCM or XM
	SampleRate  int32       // ADPCM only
	NumChannels int32       // ADPCM only
	NumSamples  int32       // ADPCM only
	Volume      float32
	MinDistance  float32
	Flags       int32 // ADPCM only
}

// VISoundType identifies the audio format.
type VISoundType int32

const (
	SoundTypeADPCM VISoundType = 0 // PS2 VAG ADPCM
	SoundTypeXM    VISoundType = 1 // Extended Module tracker
)

const (
	TypeSound     = 0xB000
	TypeSoundADPCM = 0xB010
	TypeSoundXM    = 0xB020
)

// ParseSound reads a Sound from ESF child data.
// Sound container (0xB000) has children: 0xB010 (header) + 0xB020 (audio data).
//
// PS2 ParseAdpcmObj read sequence (from decompilation at 0x00436C68):
//   uint32(dictID) → int32(numChannels) → int32(numSamples) →
//   int32(sampleRate) → float32(volume) → float32(minDistance) → int32(flags)
//
// Body is typically 24 bytes (6 fields), flags may be absent.
func ParseSound(children map[uint16][]byte) *VISound {
	s := &VISound{}

	if hdr, ok := children[TypeSoundADPCM]; ok && len(hdr) >= 4 {
		pos := 0
		s.Type = SoundTypeADPCM
		s.DictID = ru32(hdr, &pos)
		if pos+4 <= len(hdr) { s.NumChannels = ri32(hdr, &pos) }
		if pos+4 <= len(hdr) { s.NumSamples = ri32(hdr, &pos) }
		if pos+4 <= len(hdr) { s.SampleRate = ri32(hdr, &pos) }
		if pos+4 <= len(hdr) { s.Volume = rf32(hdr, &pos) }
		if pos+4 <= len(hdr) { s.MinDistance = rf32(hdr, &pos) }
		if pos+4 <= len(hdr) { s.Flags = ri32(hdr, &pos) }
	}

	return s
}
