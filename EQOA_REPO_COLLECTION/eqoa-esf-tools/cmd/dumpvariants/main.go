// dumpvariants dumps all variant placement tags and texture DictIDs from a CSprite.
// Shows which armor sets have variant geometry and what textures they use.
package main

import (
	"fmt"
	"log"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

const esfPath = "/home/sdg/claude-eqoa/extracted-assets/CHAR.ESF"

func main() {
	log.SetFlags(0)

	file, err := esf.Open(esfPath)
	if err != nil {
		log.Fatalf("Failed to open ESF: %v", err)
	}
	if err := file.BuildDictionary(); err != nil {
		log.Fatalf("Failed to build dictionary: %v", err)
	}

	// Human male model.
	dumpVariants(file, 1893243078, "Human Male")
	// Ogre model.
	dumpVariants(file, -2071956336, "Ogre")
}

func dumpVariants(file *esf.ObjFile, dictID int32, label string) {
	obj, err := file.FindObject(dictID)
	if err != nil || obj == nil {
		fmt.Printf("=== %s (%d): NOT FOUND ===\n", label, dictID)
		return
	}

	fmt.Printf("=== %s (Model %d) ===\n", label, dictID)

	cs, ok := obj.(*esf.CSprite)
	if !ok {
		fmt.Printf("  Not a CSprite\n\n")
		return
	}

	// Dump TSlotList.
	if len(cs.TSlotList) > 0 {
		fmt.Printf("  TSlotList (%d entries):\n", len(cs.TSlotList))
		for _, entry := range cs.TSlotList {
			fmt.Printf("    matIndex=%d slotID=%d flag=%d\n", entry.MeshIndex, entry.SlotID, entry.Flag)
		}
	}

	// Dump all placements with their variant tags and texture info.
	fmt.Printf("\n  Placements: %d\n", len(cs.Placements))
	for i, sp := range cs.Placements {
		varStr := "BASE"
		if sp.Variant != nil {
			varStr = fmt.Sprintf("Slot=%d Set=%d", sp.Variant.Slot, sp.Variant.Set)
		}

		sprite := sp.Sprite
		spriteStr := "nil"
		if sprite != nil {
			spriteStr = fmt.Sprintf("DictID=%d", sprite.ObjInfo().DictID)
		} else if sp.SpriteID != 0 {
			spriteStr = fmt.Sprintf("SpriteID=%d (unresolved)", sp.SpriteID)
		}

		fmt.Printf("  [%d] %s sprite=%s pos=(%.3f,%.3f,%.3f)\n",
			i, varStr, spriteStr, sp.Pos.X, sp.Pos.Y, sp.Pos.Z)

		// Get PrimBuffer and dump material info.
		if sprite == nil {
			continue
		}
		pb, err := sprite.GetPrimBuffer(file)
		if err != nil || pb == nil {
			continue
		}
		mp := pb.MatPal
		if mp == nil {
			mp, _ = sprite.GetMatPal(file)
		}
		for _, vl := range pb.VertexLists {
			matStr := fmt.Sprintf("mat=%d", vl.Material)
			if mp != nil && vl.Material < len(mp.Materials) {
				m := mp.Materials[vl.Material]
				if len(m.Layers) > 0 && m.Layers[0].Surface != nil {
					surf := m.Layers[0].Surface
					matStr += fmt.Sprintf(" surfDictID=%d (%dx%d)", surf.ObjInfo().DictID,
						surf.W, surf.H)
				}
			}
			fmt.Printf("      verts=%d %s\n", len(vl.Vertices), matStr)
		}
	}

	fmt.Println()
}
