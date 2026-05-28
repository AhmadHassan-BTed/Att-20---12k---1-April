package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func main() {
	var files []string
	filepath.Walk("/home/sdg/claude-eqoa/extracted-assets", func(path string, info os.FileInfo, err error) error {
		if err != nil { return nil }
		lower := strings.ToLower(path)
		if strings.HasSuffix(lower, ".esf") || strings.HasSuffix(lower, ".csf") {
			files = append(files, path)
		}
		return nil
	})

	versionCounts := map[int16]int{}
	matVersionCounts := map[int16]int{}
	var totalPalettes, totalMats int
	var withSurfaceChild int

	for _, path := range files {
		var f *esf.ObjFile
		var err error
		lower := strings.ToLower(path)
		if strings.HasSuffix(lower, ".csf") {
			decompressed, derr := esf.DecompressCSF(path)
			if derr != nil { continue }
			f, err = esf.OpenBytes(decompressed)
		} else {
			f, err = esf.Open(path)
		}
		if err != nil { continue }
		_, err = f.Root()
		if err != nil { continue }

		for _, obj := range f.AllObjects() {
			if obj.Type == 0x1110 { // MaterialPalette
				totalPalettes++
				versionCounts[obj.Version]++
				// Check for Surface child (before MaterialArray)
				for _, c := range obj.Children {
					if c.Type == 0x1000 { // Surface
						withSurfaceChild++
						break
					}
				}
			}
			if obj.Type == 0x1100 { // Material
				totalMats++
				matVersionCounts[obj.Version]++
			}
		}
	}

	fmt.Printf("MaterialPalettes: %d\n", totalPalettes)
	fmt.Println("  By version:")
	for v, c := range versionCounts {
		fmt.Printf("    version %d: %d\n", v, c)
	}
	fmt.Printf("  With Surface child: %d\n", withSurfaceChild)
	fmt.Printf("\nMaterials (0x1100): %d\n", totalMats)
	fmt.Println("  By version:")
	for v, c := range matVersionCounts {
		fmt.Printf("    version %d: %d\n", v, c)
	}
}
