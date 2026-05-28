package esf

import (
	"encoding/binary"
	"math"
)

// XmModule holds parsed XM module data ready for rendering.
type XmModule struct {
	SongLength     int
	RestartPos     int
	NumChannels    int
	NumPatterns    int
	NumInstruments int
	Flags          int
	Tempo          int // ticks per row
	BPM            int
	PatternOrder   [256]byte
	Patterns       []XmPattern
	Instruments    []XmInstrument
}

// XmPattern holds one pattern's note data (64 rows, up to 16 channels).
type XmPattern struct {
	NumRows int
	Rows    [][]XmNote // [row][channel]
}

// XmNote is a single note slot.
type XmNote struct {
	Note       byte // 1-96 (0=none, 97=note-off)
	Instrument byte
	Volume     byte // 0x10-0x50 (0=none)
	EffectType byte
	EffectParam byte
}

// XmEnvPoint is a single envelope point (tick, value).
type XmEnvPoint struct {
	Tick  int
	Value int // 0-64
}

// XmInstrument holds sample references for one instrument.
type XmInstrument struct {
	NumSamples     int
	NoteToSample   [96]byte // maps note 0-95 to sample index
	PanEnvPoints   []XmEnvPoint
	PanEnvType     byte // bit 0=on, bit 1=sustain, bit 2=loop
	PanSustainPt   int
	PanLoopStart   int
	PanLoopEnd     int
	Samples        []XmSample
}

// XmSample holds a decoded PCM sample with loop info.
type XmSample struct {
	PCM       []int16
	LoopStart int // -1 = no loop
	LoopEnd   int // -1 = no loop
}

// ParseXmModule parses the raw pattern+sample data from an Xm object.
// Layout verified against SNDDRV.IRX Ghidra decompilation (FUN_0000888c).
func ParseXmModule(xm *Xm) *XmModule {
	if xm == nil || len(xm.PatternData) < 0x891C {
		return nil
	}

	d := xm.PatternData
	m := &XmModule{}

	// Header at offset 0
	m.SongLength = int(binary.LittleEndian.Uint16(d[4:]))
	m.RestartPos = int(binary.LittleEndian.Uint16(d[6:]))
	m.NumChannels = int(binary.LittleEndian.Uint16(d[8:]))
	m.NumPatterns = int(binary.LittleEndian.Uint16(d[10:]))
	m.NumInstruments = int(binary.LittleEndian.Uint16(d[12:]))
	m.Flags = int(binary.LittleEndian.Uint16(d[14:]))
	m.Tempo = int(binary.LittleEndian.Uint16(d[16:]))
	m.BPM = int(binary.LittleEndian.Uint16(d[18:]))

	if m.Tempo <= 0 {
		m.Tempo = 6
	}
	if m.BPM <= 0 {
		m.BPM = 125
	}
	if m.NumChannels <= 0 || m.NumChannels > 16 {
		return nil
	}
	if m.NumPatterns <= 0 || m.NumPatterns > 256 {
		return nil
	}

	// Pattern order table at offset 28
	copy(m.PatternOrder[:], d[28:28+256])

	// Pattern descriptor table at +0x11C (12 bytes per pattern)
	// Entry+8 = relative offset to packed row data from +0x891C
	m.Patterns = make([]XmPattern, m.NumPatterns)
	for pi := 0; pi < m.NumPatterns; pi++ {
		descOff := 0x11C + pi*12
		if descOff+12 > len(d) {
			break
		}
		rowDataRel := int(binary.LittleEndian.Uint32(d[descOff+8:]))
		rowDataAbs := 0x891C + rowDataRel
		if rowDataAbs >= len(d) {
			continue
		}

		// Find data extent (next pattern's offset, or end of data)
		var dataEnd int
		if pi+1 < m.NumPatterns {
			nextDescOff := 0x11C + (pi+1)*12
			nextRel := int(binary.LittleEndian.Uint32(d[nextDescOff+8:]))
			dataEnd = 0x891C + nextRel
		} else {
			dataEnd = len(d)
		}
		if dataEnd > len(d) {
			dataEnd = len(d)
		}

		// Decode standard XM packed rows (64 rows per pattern)
		numRows := 64
		pat := XmPattern{NumRows: numRows}
		pat.Rows = make([][]XmNote, numRows)
		pos := rowDataAbs

		for row := 0; row < numRows && pos < dataEnd; row++ {
			pat.Rows[row] = make([]XmNote, m.NumChannels)
			for ch := 0; ch < m.NumChannels && pos < dataEnd; ch++ {
				b := d[pos]
				var n XmNote
				if b&0x80 != 0 {
					// Packed: bits indicate which fields follow
					pos++
					if b&0x01 != 0 && pos < dataEnd {
						n.Note = d[pos]; pos++
					}
					if b&0x02 != 0 && pos < dataEnd {
						n.Instrument = d[pos]; pos++
					}
					if b&0x04 != 0 && pos < dataEnd {
						n.Volume = d[pos]; pos++
					}
					if b&0x08 != 0 && pos < dataEnd {
						n.EffectType = d[pos]; pos++
					}
					if b&0x10 != 0 && pos < dataEnd {
						n.EffectParam = d[pos]; pos++
					}
				} else {
					// Full 5 bytes: note, instrument, volume, effect, param
					n.Note = b
					if pos+4 < dataEnd {
						n.Instrument = d[pos+1]
						n.Volume = d[pos+2]
						n.EffectType = d[pos+3]
						n.EffectParam = d[pos+4]
					}
					pos += 5
				}
				pat.Rows[row][ch] = n
			}
		}
		m.Patterns[pi] = pat
	}

	// Parse instruments at +0xD1C (0xE0 = 224 bytes each)
	// and sample references at +0x7D1C (0x18 = 24 bytes each)
	m.Instruments = make([]XmInstrument, m.NumInstruments)
	for i := 0; i < m.NumInstruments; i++ {
		instOff := 0xD1C + i*0xE0
		if instOff >= len(d) {
			break
		}
		numSamp := int(d[instOff])
		sampTableIdx := int(int16(binary.LittleEndian.Uint16(d[instOff+0xDA:])))

		inst := XmInstrument{NumSamples: numSamp}
		// Note-to-sample map: 96 bytes at instrument +4
		if instOff+4+96 <= len(d) {
			copy(inst.NoteToSample[:], d[instOff+4:instOff+4+96])
		}
		// Panning envelope: points at +148 (12 points × 4 bytes), params at +197-205
		if instOff+206 <= len(d) {
			inst.PanEnvType = d[instOff+205]
			numPanPts := int(d[instOff+197])
			inst.PanSustainPt = int(d[instOff+201])
			inst.PanLoopStart = int(d[instOff+202])
			inst.PanLoopEnd = int(d[instOff+203])
			if inst.PanEnvType&1 != 0 && numPanPts > 0 && numPanPts <= 12 {
				inst.PanEnvPoints = make([]XmEnvPoint, numPanPts)
				for p := 0; p < numPanPts; p++ {
					poff := instOff + 148 + p*4
					inst.PanEnvPoints[p] = XmEnvPoint{
						Tick:  int(binary.LittleEndian.Uint16(d[poff:])),
						Value: int(binary.LittleEndian.Uint16(d[poff+2:])),
					}
				}
			}
		}
		inst.Samples = make([]XmSample, numSamp)

		for si := 0; si < numSamp; si++ {
			refIdx := sampTableIdx + si
			if sampTableIdx < 0 {
				continue
			}
			refOff := 0x7D1C + refIdx*0x18
			if refOff+0x18 > len(d) {
				continue
			}
			length := int(binary.LittleEndian.Uint32(d[refOff:]))
			loopStart := int(int32(binary.LittleEndian.Uint32(d[refOff+4:])))
			loopEnd := int(int32(binary.LittleEndian.Uint32(d[refOff+8:])))
			spuAddr := int(binary.LittleEndian.Uint32(d[refOff+20:]))

			// Decode VAG from SampleData at the SPU address offset
			if length > 0 && len(xm.SampleData) > spuAddr {
				// Convert block count to byte size: (length-1) * 0x1C for no-loop,
				// (length-2) * 0x1C if loop (from IOP init)
				vagBytes := length * 16 // raw VAG blocks
				end := spuAddr + vagBytes
				if end > len(xm.SampleData) {
					end = len(xm.SampleData)
				}
				pcm := decodeVAGBlock(xm.SampleData[spuAddr:end])
				inst.Samples[si] = XmSample{
					PCM:       pcm,
					LoopStart: loopStart,
					LoopEnd:   loopEnd,
				}
			}
		}
		m.Instruments[i] = inst
	}

	return m
}

// decodeVAGBlock decodes VAG ADPCM to signed 16-bit PCM.
func decodeVAGBlock(data []byte) []int16 {
	filters := [5][2]float64{
		{0, 0},
		{60.0 / 64.0, 0},
		{115.0 / 64.0, -52.0 / 64.0},
		{98.0 / 64.0, -55.0 / 64.0},
		{122.0 / 64.0, -60.0 / 64.0},
	}

	var samples []int16
	var s1, s2 float64

	for i := 0; i < len(data); i += 16 {
		if i+16 > len(data) {
			break
		}
		predict := int((data[i] >> 4) & 0x0F)
		shift := int(data[i] & 0x0F)
		flags := data[i+1]

		if predict > 4 {
			predict = 0
		}
		if shift > 12 {
			shift = 9
		}
		f0, f1 := filters[predict][0], filters[predict][1]

		for j := 2; j < 16; j++ {
			for k := 0; k < 2; k++ {
				var nibble int
				if k == 0 {
					nibble = int(data[i+j] & 0x0F)
				} else {
					nibble = int((data[i+j] >> 4) & 0x0F)
				}
				if nibble > 7 {
					nibble -= 16
				}
				sample := float64(nibble) * float64(int(1)<<max(0, 12-shift))
				sample += s1*f0 + s2*f1
				s2 = s1
				s1 = sample
				samples = append(samples, int16(math.Max(-32768, math.Min(32767, sample))))
			}
		}

		if flags == 1 || flags == 7 {
			break
		}
	}
	return samples
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

// RenderXmToWAV renders an XM module to stereo 16-bit PCM.
func RenderXmToWAV(m *XmModule, sampleRate int) []int16 {
	if m == nil || len(m.Patterns) == 0 || len(m.Instruments) == 0 {
		return nil
	}

	tickDur := 2500.0 / float64(m.BPM)
	rowDur := tickDur * float64(m.Tempo)
	samplesPerRow := int(rowDur * float64(sampleRate) / 1000.0)

	// Calculate total output length
	totalRows := 0
	for oi := 0; oi < m.SongLength; oi++ {
		pi := int(m.PatternOrder[oi])
		if pi < len(m.Patterns) {
			totalRows += m.Patterns[pi].NumRows
		}
	}
	totalSamples := totalRows * samplesPerRow
	if totalSamples <= 0 {
		return nil
	}

	out := make([]int16, totalSamples*2)

	type chanState struct {
		instIdx    int
		sampleIdx  int
		posF       float64 // fractional sample position for pitch shifting
		volume     float64 // 0-1
		pan        float64 // 0-1 (0=left, 0.5=center, 1=right)
		rate       float64 // playback rate relative to native (1.0 = normal)
		fadeVol    float64 // fadeout volume (1.0 = full, decreases after note-off)
		volSlide   float64 // per-tick volume slide delta
		panEnvTick int     // current tick in panning envelope
		playing    bool
		releasing  bool // true = note-off triggered, fading out
	}
	channels := make([]chanState, m.NumChannels)
	for i := range channels {
		channels[i].instIdx = -1
		channels[i].sampleIdx = -1
		channels[i].rate = 1.0
		channels[i].fadeVol = 1.0
		channels[i].pan = 0.5 // center
		channels[i].volume = 1.0
	}

	outPos := 0
	for orderIdx := 0; orderIdx < m.SongLength; orderIdx++ {
		patIdx := int(m.PatternOrder[orderIdx])
		if patIdx >= len(m.Patterns) {
			continue
		}
		pat := &m.Patterns[patIdx]

		for row := 0; row < pat.NumRows; row++ {
			if row >= len(pat.Rows) {
				outPos += samplesPerRow
				continue
			}

			// Process note events
			for ch := 0; ch < m.NumChannels && ch < len(pat.Rows[row]); ch++ {
				n := pat.Rows[row][ch]

				if n.Note == 97 { // note off — start fadeout
					channels[ch].releasing = true
					continue
				}

				if n.Instrument > 0 {
					instIdx := int(n.Instrument) - 1 // 1-based
					if instIdx < len(m.Instruments) {
						channels[ch].instIdx = instIdx
					}
				}

				if n.Volume >= 0x10 && n.Volume <= 0x50 {
					channels[ch].volume = float64(n.Volume-0x10) / 64.0
				} else if n.Volume >= 0x60 && n.Volume <= 0x6F {
					// Volume slide down: rate = low nibble
					channels[ch].volSlide = -float64(n.Volume&0x0F) / 64.0
				} else if n.Volume >= 0x70 && n.Volume <= 0x7F {
					// Volume slide up: rate = low nibble
					channels[ch].volSlide = float64(n.Volume&0x0F) / 64.0
				} else if n.Volume >= 0xC0 && n.Volume <= 0xCF {
					// Set panning: 0xC0=left, 0xCF=right
					channels[ch].pan = float64(n.Volume&0x0F) / 15.0
				}

				// Effect column
				if n.EffectType == 0x08 {
					// Set panning: 0x00=left, 0x80=center, 0xFF=right
					channels[ch].pan = float64(n.EffectParam) / 255.0
				} else if n.EffectType == 0x0A {
					// Volume slide: hi nibble = up, lo nibble = down
					hi := float64(n.EffectParam >> 4)
					lo := float64(n.EffectParam & 0x0F)
					if hi > 0 {
						channels[ch].volSlide = hi / 64.0
					} else {
						channels[ch].volSlide = -lo / 64.0
					}
				}

				if n.Note >= 1 && n.Note <= 96 {
					// Trigger note — use note-to-sample map and compute pitch
					instIdx := channels[ch].instIdx
					if instIdx >= 0 && instIdx < len(m.Instruments) {
						inst := &m.Instruments[instIdx]
						// Note-to-sample map (96 entries, note is 1-based)
						si := int(inst.NoteToSample[n.Note-1])
						if si >= inst.NumSamples {
							si = 0
						}
						if si < len(inst.Samples) && len(inst.Samples[si].PCM) > 0 {
							channels[ch].sampleIdx = instIdx*100 + si
							channels[ch].posF = 0
							channels[ch].playing = true
							channels[ch].releasing = false
							channels[ch].fadeVol = 1.0
							channels[ch].panEnvTick = 0
							// XM linear frequency: pitch relative to C-4 (note 49)
							// Each semitone = 2^(1/12) ratio
							// Note 49 = C-4 = native sample rate
							semitones := float64(n.Note) - 49.0
							channels[ch].rate = math.Pow(2.0, semitones/12.0)
						}
					}
				}
			}

			// Mix with per-channel panning and volume slide
			for s := 0; s < samplesPerRow; s++ {
				var mixL, mixR float64
				for ch := 0; ch < m.NumChannels; ch++ {
					cs := &channels[ch]
					if !cs.playing || cs.instIdx < 0 {
						continue
					}
					inst := &m.Instruments[cs.instIdx]
					si := cs.sampleIdx % 100
					if si >= len(inst.Samples) {
						cs.playing = false
						continue
					}
					samp := &inst.Samples[si]
					if len(samp.PCM) == 0 {
						cs.playing = false
						continue
					}

					pos := int(cs.posF)
					if pos >= len(samp.PCM) {
						if samp.LoopStart >= 0 && samp.LoopEnd > samp.LoopStart {
							cs.posF = float64(samp.LoopStart)
							pos = samp.LoopStart
						} else {
							cs.playing = false
							continue
						}
					}
					// Linear interpolation
					frac := cs.posF - float64(pos)
					s0 := float64(samp.PCM[pos])
					s1 := s0
					if pos+1 < len(samp.PCM) {
						s1 = float64(samp.PCM[pos+1])
					}
					val := (s0 + (s1-s0)*frac) * cs.volume * cs.fadeVol
					// Panning envelope modulation
					pan := cs.pan
					if cs.instIdx >= 0 && cs.instIdx < len(m.Instruments) {
						envPan := evalPanEnvelope(&m.Instruments[cs.instIdx], cs.panEnvTick, cs.releasing)
						if envPan >= 0 {
							// XM panning envelope: 0=left, 32=center, 64=right
							// Modulates the channel pan toward envelope value
							pan = float64(envPan) / 64.0
						}
					}
					// Stereo panning (PS2: pan 0=left, 0.5=center, 1=right)
					mixL += val * (1.0 - pan)
					mixR += val * pan
					cs.posF += cs.rate
					cs.panEnvTick++

					// Volume slide (per sample, not per tick — approximate)
					if cs.volSlide != 0 {
						cs.volume += cs.volSlide / float64(samplesPerRow)
						if cs.volume < 0 {
							cs.volume = 0
						} else if cs.volume > 1 {
							cs.volume = 1
						}
					}
					// Fadeout after note-off
					if cs.releasing {
						cs.fadeVol -= 4.0 / float64(sampleRate)
						if cs.fadeVol <= 0 {
							cs.fadeVol = 0
							cs.playing = false
						}
					}
				}

				idx := (outPos + s) * 2
				if idx+1 < len(out) {
					out[idx] = int16(math.Max(-32768, math.Min(32767, mixL)))
					out[idx+1] = int16(math.Max(-32768, math.Min(32767, mixR)))
				}
			}
			outPos += samplesPerRow
		}
	}

	return out
}

// evalPanEnvelope evaluates the panning envelope at the given tick.
// Returns 0-64 (XM panning value) or -1 if envelope is disabled.
func evalPanEnvelope(inst *XmInstrument, tick int, releasing bool) int {
	if inst.PanEnvType&1 == 0 || len(inst.PanEnvPoints) == 0 {
		return -1
	}

	pts := inst.PanEnvPoints

	// Sustain: hold at sustain point if not releasing
	if inst.PanEnvType&2 != 0 && !releasing {
		sp := inst.PanSustainPt
		if sp < len(pts) && tick >= pts[sp].Tick {
			return pts[sp].Value
		}
	}

	// Find the two points surrounding the current tick
	if tick <= pts[0].Tick {
		return pts[0].Value
	}
	if tick >= pts[len(pts)-1].Tick {
		return pts[len(pts)-1].Value
	}

	for i := 0; i < len(pts)-1; i++ {
		if tick >= pts[i].Tick && tick < pts[i+1].Tick {
			// Linear interpolation between points
			dt := pts[i+1].Tick - pts[i].Tick
			if dt <= 0 {
				return pts[i].Value
			}
			t := float64(tick-pts[i].Tick) / float64(dt)
			return pts[i].Value + int(t*float64(pts[i+1].Value-pts[i].Value))
		}
	}
	return pts[len(pts)-1].Value
}
