// variantdump walks all CSprites in CHAR.ESF and dumps detailed info about
// variant vs non-variant children. This helps reverse-engineer how armor
// variants map to equipment slots.
package main

import (
	"encoding/binary"
	"fmt"
	"log"
	"math"
	"os"
	"sort"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "Usage: variantdump <CHAR.ESF>\n")
		os.Exit(1)
	}
	path := os.Args[1]

	file, err := esf.Open(path)
	if err != nil {
		log.Fatalf("open: %v", err)
	}
	if err := file.BuildDictionary(); err != nil {
		log.Fatalf("dict: %v", err)
	}

	root, err := file.Root()
	if err != nil {
		log.Fatalf("root: %v", err)
	}

	// Collect all CSprite nodes.
	var csprites []*esf.ObjInfo
	var walk func(info *esf.ObjInfo)
	walk = func(info *esf.ObjInfo) {
		if info.Type == esf.TypeCSprite {
			csprites = append(csprites, info)
		}
		for _, c := range info.Children {
			walk(c)
		}
	}
	walk(root)

	sort.Slice(csprites, func(i, j int) bool {
		return csprites[i].DictID < csprites[j].DictID
	})

	fmt.Printf("Found %d CSprites in %s\n\n", len(csprites), path)

	// Stats
	withVariants := 0
	withoutVariants := 0

	for _, cs := range csprites {
		dictID := cs.DictID
		headerInfo := cs.Child(esf.TypeCSpriteHeader)
		if headerInfo != nil && headerInfo.DictID != 0 {
			dictID = headerInfo.DictID
		}

		arrayInfo := cs.Child(esf.TypeCSpriteArray)
		if arrayInfo == nil {
			continue
		}

		allChildren := arrayInfo.ChildrenOfType(0)
		var directChildren []*esf.ObjInfo
		var variantChildren []*esf.ObjInfo
		for _, child := range allChildren {
			if child.Type == esf.TypeCSpriteVariant {
				variantChildren = append(variantChildren, child)
			} else {
				directChildren = append(directChildren, child)
			}
		}

		if len(variantChildren) == 0 {
			withoutVariants++
			continue
		}
		withVariants++

		fmt.Printf("=== CSprite DictID=0x%08X (%d) ===\n", uint32(dictID), dictID)
		fmt.Printf("  Direct children: %d  Variant children: %d\n", len(directChildren), len(variantChildren))

		// List direct children types
		for i, dc := range directChildren {
			fmt.Printf("  direct[%d]: %s (0x%04X)\n", i, esf.TypeName(dc.Type), dc.Type)
		}

		// List variant details
		for vi, v := range variantChildren {
			// Header
			hdr := v.Child(esf.TypeCSpriteVariantHeader)
			// Footer
			ftr := v.Child(esf.TypeCSpriteVariantFooter)
			// Meshes
			meshes := v.Child(esf.TypeCSpriteVariantMeshes)
			meshCount := 0
			var meshTypes []string
			if meshes != nil {
				mc := meshes.ChildrenOfType(0)
				meshCount = len(mc)
				for _, m := range mc {
					meshTypes = append(meshTypes, fmt.Sprintf("%s(0x%04X)", esf.TypeName(m.Type), m.Type))
				}
			}

			fmt.Printf("  variant[%d]: meshes=%d types=%v\n", vi, meshCount, meshTypes)

			// Parse header as floats
			if hdr != nil {
				raw := file.RawBytes(hdr.Offset, int(hdr.Size))
				if len(raw) >= 28 {
					// First 4 bytes: ID
					id := binary.LittleEndian.Uint32(raw[0:4])
					// Next 6 float32s
					f := make([]float32, 6)
					for i := 0; i < 6; i++ {
						f[i] = math.Float32frombits(binary.LittleEndian.Uint32(raw[4+i*4 : 8+i*4]))
					}
					fmt.Printf("    header: id=0x%08X floats=[%.3f, %.3f, %.3f, %.3f, %.3f, %.3f]\n",
						id, f[0], f[1], f[2], f[3], f[4], f[5])
				}
			}

			// Parse footer
			if ftr != nil {
				raw := file.RawBytes(ftr.Offset, int(ftr.Size))
				if len(raw) >= 20 {
					v0 := binary.LittleEndian.Uint32(raw[0:4])
					v1 := binary.LittleEndian.Uint32(raw[4:8])
					f2 := math.Float32frombits(binary.LittleEndian.Uint32(raw[8:12]))
					v3 := binary.LittleEndian.Uint32(raw[12:16])
					f4 := math.Float32frombits(binary.LittleEndian.Uint32(raw[16:20]))
					fmt.Printf("    footer: int=%d id=0x%08X float=%.1f id=0x%08X float=%.1f\n",
						v0, v1, f2, v3, f4)
				}
			}
		}
		fmt.Println()
	}

	fmt.Printf("\nSummary: %d with variants, %d without variants\n", withVariants, withoutVariants)
}
