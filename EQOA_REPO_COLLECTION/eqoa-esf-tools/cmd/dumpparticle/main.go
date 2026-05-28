package main

import (
	"encoding/binary"
	"fmt"
	"math"
	"os"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func main() {
	path := "/home/sdg/claude-eqoa/extracted-assets/SPELLFX.ESF"
	if len(os.Args) > 1 {
		path = os.Args[1]
	}

	file, err := esf.Open(path)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error opening %s: %v\n", path, err)
		os.Exit(1)
	}

	if err := file.BuildDictionary(); err != nil {
		fmt.Fprintf(os.Stderr, "Error building dictionary: %v\n", err)
		os.Exit(1)
	}

	// Find all ParticleDefinition (0xC000) objects
	allObjects := file.AllObjects()
	fmt.Printf("Total objects: %d\n", len(allObjects))

	var pdCount, pdDataCount int
	for _, info := range allObjects {
		if info.Type == esf.TypeParticleDefinition {
			pdCount++
		}
		if info.Type == esf.TypeParticleDefData {
			pdDataCount++
		}
	}
	fmt.Printf("ParticleDefinition (0xC000) objects: %d\n", pdCount)
	fmt.Printf("ParticleDefData (0xC020) objects: %d\n", pdDataCount)

	// Dump details for first few 0xC020 blocks
	dumped := 0
	for _, info := range allObjects {
		if info.Type != esf.TypeParticleDefData {
			continue
		}

		data := file.RawBytes(info.Offset, int(info.Size))
		if len(data) < 24 {
			fmt.Printf("\n0xC020 at offset=0x%X: too small (%d bytes)\n", info.Offset, len(data))
			continue
		}

		fmt.Printf("\n=== ParticleDefData 0xC020 ===\n")
		fmt.Printf("  Offset: 0x%X, Size: %d bytes, Version: %d, DictID: 0x%08X\n",
			info.Offset, info.Size, info.Version, uint32(info.DictID))

		// Parse the header
		texDictID := binary.LittleEndian.Uint32(data[0:4])
		blendMode := int32(binary.LittleEndian.Uint32(data[4:8]))
		zWrite := int32(binary.LittleEndian.Uint32(data[8:12]))
		zTest := int32(binary.LittleEndian.Uint32(data[12:16]))
		texConfig := int32(binary.LittleEndian.Uint32(data[16:20]))
		numExtra := int32(binary.LittleEndian.Uint32(data[20:24]))

		fmt.Printf("  TextureDictID: 0x%08X\n", texDictID)
		fmt.Printf("  BlendMode: %d\n", blendMode)
		fmt.Printf("  ZWrite: %d\n", zWrite)
		fmt.Printf("  ZTest: %d\n", zTest)
		fmt.Printf("  TextureConfig: %d\n", texConfig)
		fmt.Printf("  NumExtraTextures: %d (= %d motifs total)\n", numExtra, 1+max(0, numExtra))

		// Parse motifs
		pos := 24
		numMotifs := 1
		if numExtra > 0 {
			numMotifs += int(numExtra)
		}
		for m := 0; m < numMotifs; m++ {
			if pos >= len(data) {
				fmt.Printf("  Motif %d: ran out of data at pos=%d\n", m, pos)
				break
			}

			motifName := "(default)"
			if m > 0 && pos+32 <= len(data) {
				nameBytes := data[pos : pos+32]
				for i, b := range nameBytes {
					if b == 0 {
						motifName = string(nameBytes[:i])
						break
					}
				}
				pos += 32
			}

			fmt.Printf("  Motif %d: '%s'\n", m, motifName)

			if pos+13*4 > len(data) {
				fmt.Printf("    (insufficient data for 13 floats at pos=%d)\n", pos)
				break
			}

			// 13 scalar floats
			names := []string{"Friction", "Birthrate", "BirthrateVar", "Lifespan", "LifespanVar",
				"Velocity", "VelocityVar", "StartSize", "StartSizeVar", "EndSize", "EndSizeVar",
				"InheritVelocity", "DeltaSpawn"}
			for _, name := range names {
				val := math.Float32frombits(binary.LittleEndian.Uint32(data[pos : pos+4]))
				if val != 0 {
					fmt.Printf("    %s: %.6f\n", name, val)
				}
				pos += 4
			}

			// StartColorVar (16 bytes)
			if pos+16 <= len(data) {
				r := math.Float32frombits(binary.LittleEndian.Uint32(data[pos:]))
				g := math.Float32frombits(binary.LittleEndian.Uint32(data[pos+4:]))
				b := math.Float32frombits(binary.LittleEndian.Uint32(data[pos+8:]))
				a := math.Float32frombits(binary.LittleEndian.Uint32(data[pos+12:]))
				fmt.Printf("    StartColorVar: [%.3f, %.3f, %.3f, %.3f]\n", r, g, b, a)
				pos += 16
			}

			// EndColorVar (16 bytes)
			if pos+16 <= len(data) {
				r := math.Float32frombits(binary.LittleEndian.Uint32(data[pos:]))
				g := math.Float32frombits(binary.LittleEndian.Uint32(data[pos+4:]))
				b := math.Float32frombits(binary.LittleEndian.Uint32(data[pos+8:]))
				a := math.Float32frombits(binary.LittleEndian.Uint32(data[pos+12:]))
				fmt.Printf("    EndColorVar: [%.3f, %.3f, %.3f, %.3f]\n", r, g, b, a)
				pos += 16
			}

			// GradientColor (32 x 16 bytes = 512 bytes)
			if pos+512 <= len(data) {
				nonZero := 0
				for i := 0; i < 32; i++ {
					gPos := pos + i*16
					for j := 0; j < 4; j++ {
						val := math.Float32frombits(binary.LittleEndian.Uint32(data[gPos+j*4:]))
						if val != 0 {
							nonZero++
							break
						}
					}
				}
				fmt.Printf("    GradientColor: %d/32 non-zero entries\n", nonZero)
				pos += 512
			}

			// GradientRepeat (4 bytes)
			if pos+4 <= len(data) {
				val := math.Float32frombits(binary.LittleEndian.Uint32(data[pos:]))
				fmt.Printf("    GradientRepeat: %.6f\n", val)
				pos += 4
			}

			// 6 Vec3s (each 12 bytes)
			vecNames := []string{"InnerOffset", "InnerHprVar", "OuterOffset", "OuterHprVar", "NozzleAxis", "NozzleHprVar"}
			for _, vn := range vecNames {
				if pos+12 > len(data) {
					break
				}
				x := math.Float32frombits(binary.LittleEndian.Uint32(data[pos:]))
				y := math.Float32frombits(binary.LittleEndian.Uint32(data[pos+4:]))
				z := math.Float32frombits(binary.LittleEndian.Uint32(data[pos+8:]))
				if x != 0 || y != 0 || z != 0 {
					fmt.Printf("    %s: [%.6f, %.6f, %.6f]\n", vn, x, y, z)
				}
				pos += 12
			}

			// GravityOn (only if version > 0)
			if info.Version > 0 && pos+4 <= len(data) {
				val := int32(binary.LittleEndian.Uint32(data[pos:]))
				fmt.Printf("    GravityOn: %d\n", val)
				pos += 4
			}

			fmt.Printf("    (consumed %d bytes so far)\n", pos)
		}

		fmt.Printf("  Total consumed: %d / %d bytes\n", pos, len(data))
		remaining := len(data) - pos
		if remaining > 0 {
			fmt.Printf("  Remaining: %d bytes", remaining)
			if remaining <= 32 {
				fmt.Printf(" = ")
				for i := 0; i < remaining && i < 32; i++ {
					fmt.Printf("%02X ", data[pos+i])
				}
			}
			fmt.Println()
		}

		dumped++
		if dumped >= 332 {
			break
		}
	}

	fmt.Printf("\nDone. Dumped %d ParticleDefData blocks.\n", dumped)
}

func max(a, b int32) int32 {
	if a > b {
		return a
	}
	return b
}
