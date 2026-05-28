package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"path/filepath"
	"strings"
)

// PS2 VAG/SPU2 ADPCM filter coefficients (5 standard filters).
var vagF = [5][2]float64{
	{0.0, 0.0},
	{60.0 / 64.0, 0.0},
	{115.0 / 64.0, -52.0 / 64.0},
	{98.0 / 64.0, -55.0 / 64.0},
	{122.0 / 64.0, -60.0 / 64.0},
}

func main() {
	var (
		outputDir  string
		sampleRate int
		inputDir   string
	)

	flag.StringVar(&outputDir, "o", ".", "Output directory for WAV files")
	flag.IntVar(&sampleRate, "rate", 48000, "Sample rate (48000 or 44100)")
	flag.StringVar(&inputDir, "dir", "", "Convert all .BGM files in directory")

	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "bgmextract - EQOA PS2 BGM (SPU2-ADPCM) to WAV converter\n\n")
		fmt.Fprintf(os.Stderr, "Usage:\n")
		fmt.Fprintf(os.Stderr, "  bgmextract [flags] [file.BGM ...]\n\n")
		fmt.Fprintf(os.Stderr, "Examples:\n")
		fmt.Fprintf(os.Stderr, "  bgmextract -o music/ THEME.BGM FREEPORT.BGM\n")
		fmt.Fprintf(os.Stderr, "  bgmextract -dir /path/to/bgm/ -o wav/\n")
		fmt.Fprintf(os.Stderr, "  bgmextract -rate 44100 THEME.BGM\n\n")
		fmt.Fprintf(os.Stderr, "Format:\n")
		fmt.Fprintf(os.Stderr, "  BGM files are raw PS2 SPU2-ADPCM stereo audio.\n")
		fmt.Fprintf(os.Stderr, "  Music tracks use 0x10000 byte interleave.\n")
		fmt.Fprintf(os.Stderr, "  Voice-over clips use 0x4000 byte interleave.\n")
		fmt.Fprintf(os.Stderr, "  Interleave is auto-detected from end markers.\n\n")
		fmt.Fprintf(os.Stderr, "Flags:\n")
		flag.PrintDefaults()
	}

	flag.Parse()

	files := flag.Args()

	// Collect files from -dir flag
	if inputDir != "" {
		entries, err := os.ReadDir(inputDir)
		if err != nil {
			log.Fatal(err)
		}
		for _, e := range entries {
			if !e.IsDir() && strings.EqualFold(filepath.Ext(e.Name()), ".bgm") {
				files = append(files, filepath.Join(inputDir, e.Name()))
			}
		}
	}

	if len(files) == 0 {
		flag.Usage()
		os.Exit(1)
	}

	if err := os.MkdirAll(outputDir, 0o755); err != nil {
		log.Fatal(err)
	}

	converted := 0
	for _, f := range files {
		base := strings.TrimSuffix(filepath.Base(f), filepath.Ext(filepath.Base(f)))
		outPath := filepath.Join(outputDir, base+".wav")

		if err := convertBGM(f, outPath, sampleRate); err != nil {
			log.Printf("Error converting %s: %v", filepath.Base(f), err)
			continue
		}
		converted++
	}

	log.Printf("Done! Converted %d/%d files", converted, len(files))
}

// convertBGM decodes a PS2 BGM file (SPU2-ADPCM stereo) to WAV.
func convertBGM(inPath, outPath string, sampleRate int) error {
	data, err := os.ReadFile(inPath)
	if err != nil {
		return err
	}

	if len(data) < 32 {
		return fmt.Errorf("file too small")
	}

	// Find end markers to auto-detect interleave.
	// Each stereo BGM has exactly 2 end markers (one per channel)
	// at the same offset within consecutive interleave blocks.
	interleave, err := detectInterleave(data)
	if err != nil {
		return err
	}

	totalBlocks := len(data) / interleave

	// Decode both channels.
	left := decodeChannel(data, interleave, 0, totalBlocks)
	right := decodeChannel(data, interleave, 1, totalBlocks)

	// Trim to same length.
	minLen := len(left)
	if len(right) < minLen {
		minLen = len(right)
	}
	left = left[:minLen]
	right = right[:minLen]

	if minLen == 0 {
		return fmt.Errorf("no audio data decoded")
	}

	// Write WAV.
	if err := writeWAV(outPath, left, right, sampleRate); err != nil {
		return err
	}

	dur := float64(minLen) / float64(sampleRate)
	log.Printf("  %-25s → %-25s  interleave=0x%x  %d samples  %.1fs",
		filepath.Base(inPath), filepath.Base(outPath), interleave, minLen, dur)

	return nil
}

// detectInterleave finds the interleave size by locating the two ADPCM end
// markers and computing their spacing.
func detectInterleave(data []byte) (int, error) {
	var markers []int
	numBlocks := len(data) / 16
	for i := 0; i < numBlocks; i++ {
		flags := data[i*16+1]
		if flags&1 != 0 {
			markers = append(markers, i*16)
			if len(markers) == 2 {
				break
			}
		}
	}

	if len(markers) < 2 {
		return 0, fmt.Errorf("need 2 end markers for stereo, found %d", len(markers))
	}

	interleave := markers[1] - markers[0]
	if interleave <= 0 || interleave > len(data)/2 {
		return 0, fmt.Errorf("invalid interleave %d from markers at 0x%x and 0x%x",
			interleave, markers[0], markers[1])
	}

	return interleave, nil
}

// decodeChannel decodes one stereo channel from interleaved SPU2-ADPCM data.
// channel=0 for left (even blocks), channel=1 for right (odd blocks).
func decodeChannel(data []byte, interleave, channel, totalBlocks int) []int16 {
	var samples []int16
	var hist1, hist2 float64

	for blk := channel; blk < totalBlocks; blk += 2 {
		blkStart := blk * interleave
		blkEnd := blkStart + interleave
		if blkEnd > len(data) {
			blkEnd = len(data)
		}

		for off := blkStart; off+16 <= blkEnd; off += 16 {
			decoded := decodeVAGBlock(data[off:off+16], &hist1, &hist2)
			samples = append(samples, decoded...)

			// Stop at end marker.
			if data[off+1]&1 != 0 {
				return samples
			}
		}
	}

	return samples
}

// decodeVAGBlock decodes one 16-byte VAG/SPU2 ADPCM block into 28 PCM samples.
func decodeVAGBlock(block []byte, hist1, hist2 *float64) []int16 {
	shift := int(block[0] & 0x0F)
	filt := int((block[0] >> 4) & 0x0F)
	if filt > 4 {
		filt = 4
	}

	f0 := vagF[filt][0]
	f1 := vagF[filt][1]

	samples := make([]int16, 0, 28)

	for i := 2; i < 16; i++ {
		b := block[i]
		for ni := 0; ni < 2; ni++ {
			var nibble int
			if ni == 0 {
				nibble = int(b & 0x0F)
			} else {
				nibble = int((b >> 4) & 0x0F)
			}

			// Sign-extend 4-bit nibble.
			if nibble >= 8 {
				nibble -= 16
			}

			// Apply shift and prediction filter.
			sample := float64(nibble) * math.Exp2(float64(12-shift))
			sample += *hist1*f0 + *hist2*f1

			// Clamp to int16 range.
			if sample > 32767.0 {
				sample = 32767.0
			} else if sample < -32768.0 {
				sample = -32768.0
			}

			*hist2 = *hist1
			*hist1 = sample
			samples = append(samples, int16(sample))
		}
	}

	return samples
}

// writeWAV writes stereo 16-bit PCM WAV file.
func writeWAV(path string, left, right []int16, sampleRate int) error {
	f, err := os.Create(path)
	if err != nil {
		return err
	}
	defer f.Close()

	numSamples := len(left)
	numChannels := 2
	bitsPerSample := 16
	byteRate := sampleRate * numChannels * bitsPerSample / 8
	blockAlign := numChannels * bitsPerSample / 8
	dataSize := numSamples * blockAlign

	// RIFF header
	f.Write([]byte("RIFF"))
	binary.Write(f, binary.LittleEndian, uint32(36+dataSize))
	f.Write([]byte("WAVE"))

	// fmt chunk
	f.Write([]byte("fmt "))
	binary.Write(f, binary.LittleEndian, uint32(16))          // chunk size
	binary.Write(f, binary.LittleEndian, uint16(1))            // PCM format
	binary.Write(f, binary.LittleEndian, uint16(numChannels))  // channels
	binary.Write(f, binary.LittleEndian, uint32(sampleRate))   // sample rate
	binary.Write(f, binary.LittleEndian, uint32(byteRate))     // byte rate
	binary.Write(f, binary.LittleEndian, uint16(blockAlign))   // block align
	binary.Write(f, binary.LittleEndian, uint16(bitsPerSample))// bits per sample

	// data chunk
	f.Write([]byte("data"))
	binary.Write(f, binary.LittleEndian, uint32(dataSize))

	// Write interleaved samples (LRLRLR...)
	buf := make([]byte, 4)
	for i := 0; i < numSamples; i++ {
		binary.LittleEndian.PutUint16(buf[0:2], uint16(left[i]))
		binary.LittleEndian.PutUint16(buf[2:4], uint16(right[i]))
		f.Write(buf)
	}

	return nil
}
