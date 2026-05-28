// extract-armor-tex extracts armor set 5 (padded) textures from CHARCUST.CSF
// and saves them as PNG files to /tmp/armor_textures/.
package main

import (
	"fmt"
	"image/png"
	"log"
	"os"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func main() {
	csfPath := "/home/sdg/claude-eqoa/extracted-assets/CHARCUST.CSF"
	outDir := "/tmp/armor_textures"

	// Armor set 5 (padded) DictIDs from equipment.go ArmorTextures[5]
	textures := []struct {
		Name   string
		DictID int32
	}{
		{"chest", 968685663},
		{"bracer", -435228783},
		{"gloves", -12292918},
		{"legs", 1978836175},
		{"boots", -1005239304},
	}

	// Create output directory
	if err := os.MkdirAll(outDir, 0o755); err != nil {
		log.Fatalf("Failed to create output directory: %v", err)
	}

	// Decompress CSF to ESF
	fmt.Println("Decompressing CHARCUST.CSF...")
	esfData, err := esf.DecompressCSF(csfPath)
	if err != nil {
		log.Fatalf("Failed to decompress CSF: %v", err)
	}
	fmt.Printf("Decompressed %d bytes of ESF data\n", len(esfData))

	// Parse ESF
	fmt.Println("Parsing ESF data...")
	file, err := esf.OpenBytes(esfData)
	if err != nil {
		log.Fatalf("Failed to parse ESF: %v", err)
	}

	// Build dictionary for FindObject lookups
	if err := file.BuildDictionary(); err != nil {
		log.Fatalf("Failed to build dictionary: %v", err)
	}
	fmt.Printf("Dictionary has %d entries\n", file.DictLen())

	// Extract each texture
	for _, tex := range textures {
		fmt.Printf("\nLooking up %s (DictID=%d)...\n", tex.Name, tex.DictID)

		obj, err := file.FindObject(tex.DictID)
		if err != nil {
			log.Printf("ERROR: FindObject for %s: %v", tex.Name, err)
			continue
		}
		if obj == nil {
			log.Printf("ERROR: %s (DictID=%d) not found in CHARCUST.CSF", tex.Name, tex.DictID)
			continue
		}

		surf, ok := obj.(*esf.Surface)
		if !ok {
			log.Printf("ERROR: %s is not a Surface, got %T", tex.Name, obj)
			continue
		}

		fmt.Printf("  Found surface: %dx%d, depth=%d, hasAlpha=%v\n",
			surf.W, surf.H, surf.Depth, surf.HasAlpha)

		if surf.Image == nil {
			log.Printf("ERROR: %s has nil Image", tex.Name)
			continue
		}

		// Save as PNG
		filename := fmt.Sprintf("padded_%s.png", tex.Name)
		outPath := fmt.Sprintf("%s/%s", outDir, filename)

		f, err := os.Create(outPath)
		if err != nil {
			log.Printf("ERROR: creating file %s: %v", outPath, err)
			continue
		}

		if err := png.Encode(f, surf.Image); err != nil {
			f.Close()
			log.Printf("ERROR: encoding PNG for %s: %v", tex.Name, err)
			continue
		}
		f.Close()

		fmt.Printf("  Saved: %s\n", outPath)
	}

	fmt.Printf("\nDone! Textures saved to %s\n", outDir)
}
