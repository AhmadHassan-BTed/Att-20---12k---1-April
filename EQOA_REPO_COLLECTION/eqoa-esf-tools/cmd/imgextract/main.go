package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"log"
	"os"
	"path/filepath"
	"strings"
)

func main() {
	var (
		outputDir string
		inputDir  string
	)

	flag.StringVar(&outputDir, "o", ".", "Output directory for PNG files")
	flag.StringVar(&inputDir, "dir", "", "Convert all .16 and .RGB files in directory")

	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "imgextract - EQOA PS2 raw image to PNG converter\n\n")
		fmt.Fprintf(os.Stderr, "Supported formats:\n")
		fmt.Fprintf(os.Stderr, "  .16  - 640x448 raw 16bpp ABGR1555 (loading/error screens)\n")
		fmt.Fprintf(os.Stderr, "  .RGB - Raw RGB24 or RGBA32 framebuffer (network GUI backgrounds)\n\n")
		fmt.Fprintf(os.Stderr, "Usage:\n")
		fmt.Fprintf(os.Stderr, "  imgextract [flags] [file ...]\n\n")
		fmt.Fprintf(os.Stderr, "Examples:\n")
		fmt.Fprintf(os.Stderr, "  imgextract -o png/ LOADING0.16 BG.RGB\n")
		fmt.Fprintf(os.Stderr, "  imgextract -dir /path/to/raw/ -o png/\n\n")
		fmt.Fprintf(os.Stderr, "Flags:\n")
		flag.PrintDefaults()
	}

	flag.Parse()

	files := flag.Args()

	if inputDir != "" {
		entries, err := os.ReadDir(inputDir)
		if err != nil {
			log.Fatal(err)
		}
		for _, e := range entries {
			if e.IsDir() {
				continue
			}
			ext := strings.ToLower(filepath.Ext(e.Name()))
			if ext == ".16" || ext == ".rgb" {
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
		outPath := filepath.Join(outputDir, base+".png")

		ext := strings.ToLower(filepath.Ext(f))
		var err error
		switch ext {
		case ".16":
			err = convert16(f, outPath)
		case ".rgb":
			err = convertRGB(f, outPath)
		default:
			log.Printf("Skipping unknown format: %s", filepath.Base(f))
			continue
		}

		if err != nil {
			log.Printf("Error converting %s: %v", filepath.Base(f), err)
			continue
		}
		converted++
	}

	log.Printf("Done! Converted %d/%d files", converted, len(files))
}

// convert16 converts a raw 16bpp ABGR1555 framebuffer image to PNG.
// PS2 GS format: 640x448 pixels, 2 bytes per pixel, little-endian uint16.
// Bit layout: [A:1][B:5][G:5][R:5] (MSB to LSB).
func convert16(inPath, outPath string) error {
	data, err := os.ReadFile(inPath)
	if err != nil {
		return err
	}

	// Determine dimensions from file size.
	// Standard PS2 framebuffer: 640x448 = 573440 bytes at 2 bpp.
	width, height := detectDimensions16(len(data))
	if width == 0 {
		return fmt.Errorf("unexpected .16 file size %d (expected 640x448x2=573440)", len(data))
	}

	expected := width * height * 2
	if len(data) < expected {
		return fmt.Errorf("file too small: %d bytes, need %d for %dx%d", len(data), expected, width, height)
	}

	img := image.NewNRGBA(image.Rect(0, 0, width, height))

	for y := 0; y < height; y++ {
		for x := 0; x < width; x++ {
			off := (y*width + x) * 2
			pix := uint16(data[off]) | uint16(data[off+1])<<8 // little-endian

			// ABGR1555: bit 15=A, bits 14-10=B, bits 9-5=G, bits 4-0=R
			r := uint8((pix & 0x1F) << 3)
			g := uint8(((pix >> 5) & 0x1F) << 3)
			b := uint8(((pix >> 10) & 0x1F) << 3)
			a := uint8(255)
			if pix>>15 == 0 && pix != 0 {
				// Alpha bit 0 with non-zero color = semi-transparent on PS2,
				// but for loading screens treat as opaque.
				a = 255
			}
			if pix == 0 {
				a = 0 // fully transparent black
			}

			img.SetNRGBA(x, y, color.NRGBA{R: r, G: g, B: b, A: a})
		}
	}

	return writePNG(outPath, img, filepath.Base(inPath), width, height)
}

// detectDimensions16 returns width, height for a .16 file based on its size.
func detectDimensions16(size int) (int, int) {
	// Standard PS2 framebuffer sizes
	candidates := [][2]int{
		{640, 448}, // NTSC interlaced
		{640, 224}, // NTSC non-interlaced
		{640, 512}, // PAL interlaced
		{640, 256}, // PAL non-interlaced
		{512, 448},
		{512, 512},
	}
	for _, c := range candidates {
		if c[0]*c[1]*2 == size {
			return c[0], c[1]
		}
	}
	return 0, 0
}

// convertRGB converts a raw RGB24 or RGBA32 framebuffer image to PNG.
// Auto-detects pixel format from file size.
func convertRGB(inPath, outPath string) error {
	data, err := os.ReadFile(inPath)
	if err != nil {
		return err
	}

	width, height, bpp := detectDimensionsRGB(len(data))
	if width == 0 {
		return fmt.Errorf("cannot determine dimensions for .RGB file (%d bytes)", len(data))
	}

	var img image.Image

	switch bpp {
	case 3: // RGB24
		rgba := image.NewNRGBA(image.Rect(0, 0, width, height))
		for y := 0; y < height; y++ {
			for x := 0; x < width; x++ {
				off := (y*width + x) * 3
				rgba.SetNRGBA(x, y, color.NRGBA{
					R: data[off],
					G: data[off+1],
					B: data[off+2],
					A: 255,
				})
			}
		}
		img = rgba

	case 4: // RGBA32
		rgba := image.NewNRGBA(image.Rect(0, 0, width, height))
		for y := 0; y < height; y++ {
			for x := 0; x < width; x++ {
				off := (y*width + x) * 4
				rgba.SetNRGBA(x, y, color.NRGBA{
					R: data[off],
					G: data[off+1],
					B: data[off+2],
					A: 255, // Alpha channel in these files is always 0x00, treat as opaque
				})
			}
		}
		img = rgba

	default:
		return fmt.Errorf("unsupported bpp %d", bpp)
	}

	return writePNG(outPath, img, filepath.Base(inPath), width, height)
}

// detectDimensionsRGB tries common PS2 framebuffer dimensions for RGB files.
func detectDimensionsRGB(size int) (width, height, bpp int) {
	type candidate struct {
		w, h, bpp int
	}
	candidates := []candidate{
		// Try RGBA32 first (more specific match)
		{640, 448, 4},
		{640, 512, 4},
		{512, 448, 4},
		{512, 512, 4},
		{640, 256, 4},
		// Then RGB24
		{640, 448, 3},
		{640, 512, 3},
		{512, 448, 3},
		{512, 512, 3},
		{640, 256, 3},
		{320, 448, 3},
		{320, 224, 3},
	}
	for _, c := range candidates {
		if c.w*c.h*c.bpp == size {
			return c.w, c.h, c.bpp
		}
	}
	return 0, 0, 0
}

func writePNG(outPath string, img image.Image, srcName string, w, h int) error {
	f, err := os.Create(outPath)
	if err != nil {
		return err
	}
	defer f.Close()

	if err := png.Encode(f, img); err != nil {
		return err
	}

	fi, _ := f.Stat()
	log.Printf("  %-20s → %-20s  %dx%d  %d bytes",
		srcName, filepath.Base(outPath), w, h, fi.Size())
	return nil
}
