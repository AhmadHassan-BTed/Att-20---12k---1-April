package main

import (
	"fmt"
	"os"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

// HelmTextures DictIDs extracted from PS2 EE memory dump VIRaster pool.
var HelmTextures = [9]int32{
	0,            // helm 0 = none
	-1256796368,  // helm 1
	2057020990,   // helm 2
	2036326070,   // helm 3
	638503769,    // helm 4
	-610493769,   // helm 5
	-818129029,   // helm 6
	-846861641,   // helm 7
	-1663337271,  // helm 8
}

func main() {
	assetsDir := "/home/sdg/claude-eqoa/extracted-assets"

	// 1. Open ITEM.CSF (decompress first, then parse as ESF)
	fmt.Println("=== Loading ITEM.CSF ===")
	itemCSFPath := assetsDir + "/ITEM.CSF"
	itemData, err := esf.DecompressCSF(itemCSFPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "DecompressCSF ITEM.CSF: %v\n", err)
		os.Exit(1)
	}
	fmt.Printf("ITEM.CSF decompressed: %d bytes\n", len(itemData))

	itemFile, err := esf.OpenBytes(itemData)
	if err != nil {
		fmt.Fprintf(os.Stderr, "OpenBytes ITEM.CSF: %v\n", err)
		os.Exit(1)
	}
	if err := itemFile.BuildDictionary(); err != nil {
		fmt.Fprintf(os.Stderr, "BuildDictionary ITEM.CSF: %v\n", err)
		os.Exit(1)
	}
	fmt.Println("ITEM.CSF dictionary built")

	// 2. Open CHARCUST.CSF (decompress first, then parse as ESF)
	fmt.Println("\n=== Loading CHARCUST.CSF ===")
	charcustCSFPath := assetsDir + "/CHARCUST.CSF"
	charcustData, err := esf.DecompressCSF(charcustCSFPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "DecompressCSF CHARCUST.CSF: %v\n", err)
		os.Exit(1)
	}
	fmt.Printf("CHARCUST.CSF decompressed: %d bytes\n", len(charcustData))

	charcustFile, err := esf.OpenBytes(charcustData)
	if err != nil {
		fmt.Fprintf(os.Stderr, "OpenBytes CHARCUST.CSF: %v\n", err)
		os.Exit(1)
	}
	if err := charcustFile.BuildDictionary(); err != nil {
		fmt.Fprintf(os.Stderr, "BuildDictionary CHARCUST.CSF: %v\n", err)
		os.Exit(1)
	}
	fmt.Println("CHARCUST.CSF dictionary built")

	// 3. Open CHAR.ESF
	fmt.Println("\n=== Loading CHAR.ESF ===")
	charESFPath := assetsDir + "/CHAR.ESF"
	charFile, err := esf.Open(charESFPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Open CHAR.ESF: %v\n", err)
		os.Exit(1)
	}
	if err := charFile.BuildDictionary(); err != nil {
		fmt.Fprintf(os.Stderr, "BuildDictionary CHAR.ESF: %v\n", err)
		os.Exit(1)
	}
	fmt.Println("CHAR.ESF dictionary built")

	// 4. Also try ITEM.ESF (uncompressed) if it exists
	var itemESFFile *esf.ObjFile
	itemESFPath := assetsDir + "/ITEM.ESF"
	if _, statErr := os.Stat(itemESFPath); statErr == nil {
		fmt.Println("\n=== Loading ITEM.ESF ===")
		itemESFFile, err = esf.Open(itemESFPath)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Open ITEM.ESF: %v (skipping)\n", err)
		} else {
			if err := itemESFFile.BuildDictionary(); err != nil {
				fmt.Fprintf(os.Stderr, "BuildDictionary ITEM.ESF: %v (skipping)\n", err)
				itemESFFile = nil
			} else {
				fmt.Println("ITEM.ESF dictionary built")
			}
		}
	}

	// 5. Also try other CSF files that might contain helm textures
	type namedFile struct {
		name string
		file *esf.ObjFile
	}
	extraCSFs := []string{"ADDART.CSF", "FX.CSF", "SPELLFX.CSF", "CHARFACE.CSF"}
	var extras []namedFile
	for _, name := range extraCSFs {
		path := assetsDir + "/" + name
		if _, statErr := os.Stat(path); statErr != nil {
			continue
		}
		data, err := esf.DecompressCSF(path)
		if err != nil {
			fmt.Fprintf(os.Stderr, "DecompressCSF %s: %v (skipping)\n", name, err)
			continue
		}
		f, err := esf.OpenBytes(data)
		if err != nil {
			fmt.Fprintf(os.Stderr, "OpenBytes %s: %v (skipping)\n", name, err)
			continue
		}
		if err := f.BuildDictionary(); err != nil {
			fmt.Fprintf(os.Stderr, "BuildDictionary %s: %v (skipping)\n", name, err)
			continue
		}
		extras = append(extras, namedFile{name: name, file: f})
		fmt.Printf("%s loaded and dictionary built\n", name)
	}

	// 6. Search for each helm DictID across all loaded files
	fmt.Println("\n=== Helm DictID Search Results ===")
	fmt.Println()

	for i := 1; i < len(HelmTextures); i++ {
		dictID := HelmTextures[i]
		fmt.Printf("Helm %d: DictID = %d (0x%08X)\n", i, dictID, uint32(dictID))

		found := false

		// Check ITEM.CSF
		obj, _ := itemFile.FindObject(dictID)
		if obj != nil {
			fmt.Printf("  FOUND in ITEM.CSF\n")
			found = true
		}

		// Check CHARCUST.CSF
		obj, _ = charcustFile.FindObject(dictID)
		if obj != nil {
			fmt.Printf("  FOUND in CHARCUST.CSF\n")
			found = true
		}

		// Check CHAR.ESF
		obj, _ = charFile.FindObject(dictID)
		if obj != nil {
			fmt.Printf("  FOUND in CHAR.ESF\n")
			found = true
		}

		// Check ITEM.ESF
		if itemESFFile != nil {
			obj, _ = itemESFFile.FindObject(dictID)
			if obj != nil {
				fmt.Printf("  FOUND in ITEM.ESF\n")
				found = true
			}
		}

		// Check extra CSF files
		for _, extra := range extras {
			obj, _ = extra.file.FindObject(dictID)
			if obj != nil {
				fmt.Printf("  FOUND in %s\n", extra.name)
				found = true
			}
		}

		if !found {
			fmt.Printf("  NOT FOUND in any file\n")
		}
		fmt.Println()
	}

	// 7. Summary
	fmt.Println("=== Summary ===")
	foundCount := 0
	for i := 1; i < len(HelmTextures); i++ {
		dictID := HelmTextures[i]
		anyFound := false

		// Re-check all files for summary
		type source struct {
			name string
			file *esf.ObjFile
		}
		sources := []source{
			{"ITEM.CSF", itemFile},
			{"CHARCUST.CSF", charcustFile},
			{"CHAR.ESF", charFile},
		}
		if itemESFFile != nil {
			sources = append(sources, source{"ITEM.ESF", itemESFFile})
		}
		for _, extra := range extras {
			sources = append(sources, source{extra.name, extra.file})
		}

		var foundIn []string
		for _, src := range sources {
			obj, _ := src.file.FindObject(dictID)
			if obj != nil {
				foundIn = append(foundIn, src.name)
				anyFound = true
			}
		}

		status := "NOT FOUND"
		if anyFound {
			foundCount++
			status = fmt.Sprintf("found in: %v", foundIn)
		}
		fmt.Printf("  Helm %d (DictID %12d): %s\n", i, dictID, status)
	}
	fmt.Printf("\nTotal: %d/%d helm textures found\n", foundCount, len(HelmTextures)-1)
}
