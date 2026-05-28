package main

import (
	"fmt"
	"image/png"
	"os"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func main() {
	// Human male model ID from character-data-formats.md
	// Race 0 (Human), Sex 0 (Male) = model ID from the game
	charPath := "/home/sdg/claude-eqoa/extracted-assets/CHAR.ESF"
	f, err := esf.Open(charPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "open: %v\n", err)
		os.Exit(1)
	}
	if err := f.BuildDictionary(); err != nil {
		fmt.Fprintf(os.Stderr, "dict: %v\n", err)
		os.Exit(1)
	}

	// Find all CSprites with TSlotList containing slot 7 (helm)
	allInfos := f.AllObjects()
	os.MkdirAll("/tmp/helm_diag", 0755)

	helmCount := 0
	for _, info := range allInfos {
		if info.Type != esf.TypeCSprite {
			continue
		}
		obj, err := f.GetObject(info)
		if err != nil || obj == nil {
			continue
		}
		cs, ok := obj.(*esf.CSprite)
		if !ok {
			continue
		}

		// Check TSlotList for slot 7
		hasSlot7 := false
		for _, entry := range cs.TSlotList {
			if entry.SlotID == 7 {
				hasSlot7 = true
				break
			}
		}
		if !hasSlot7 {
			continue
		}

		dictID := cs.ObjInfo().DictID
		fmt.Printf("\n=== CSprite DictID=%d (0x%08X) ===\n", dictID, uint32(dictID))

		// Print all TSlotList entries
		fmt.Printf("  TSlotList (%d entries):\n", len(cs.TSlotList))
		for _, entry := range cs.TSlotList {
			slotName := fmt.Sprintf("%d", entry.SlotID)
			switch entry.SlotID {
			case 0:
				slotName = "0(chest)"
			case 1:
				slotName = "1(bracer)"
			case 2:
				slotName = "2(gloves)"
			case 3:
				slotName = "3(legs)"
			case 4:
				slotName = "4(boots)"
			case 7:
				slotName = "7(HELM)"
			case 8:
				slotName = "8(robe)"
			}
			fmt.Printf("    matIdx=%d → slot=%s flag=%d\n", entry.MeshIndex, slotName, entry.Flag)
		}

		// For each placement, dump material textures for slot 7 material indices
		slot7MatIndices := make(map[int]bool)
		for _, entry := range cs.TSlotList {
			if entry.SlotID == 7 {
				slot7MatIndices[int(entry.MeshIndex)] = true
			}
		}

		for pi, sp := range cs.Placements {
			if sp.Sprite == nil {
				continue
			}
			pb, err := sp.Sprite.GetPrimBuffer(f)
			if err != nil || pb == nil {
				continue
			}
			mp := pb.MatPal
			if mp == nil {
				mp, _ = sp.Sprite.GetMatPal(f)
			}
			if mp == nil {
				continue
			}

			for matIdx := range slot7MatIndices {
				if matIdx >= len(mp.Materials) {
					continue
				}
				mat := mp.Materials[matIdx]
				if len(mat.Layers) == 0 {
					continue
				}
				surf := mat.Layers[0].Surface
				if surf == nil || surf.Image == nil {
					continue
				}
				surfDictID := surf.ObjInfo().DictID
				w := surf.Image.Bounds().Dx()
				h := surf.Image.Bounds().Dy()
				fname := fmt.Sprintf("/tmp/helm_diag/csprite%d_place%d_mat%d_dict%d_%dx%d.png",
					dictID, pi, matIdx, surfDictID, w, h)
				out, err := os.Create(fname)
				if err != nil {
					continue
				}
				png.Encode(out, surf.Image)
				out.Close()
				fmt.Printf("  → Dumped slot7 base texture: matIdx=%d surfDictID=%d size=%dx%d → %s\n",
					matIdx, surfDictID, w, h, fname)
				helmCount++
			}

			// Also dump ALL material textures for this placement to see what's what
			for mi, mat := range mp.Materials {
				if len(mat.Layers) == 0 || mat.Layers[0].Surface == nil || mat.Layers[0].Surface.Image == nil {
					continue
				}
				surf := mat.Layers[0].Surface
				surfDictID := surf.ObjInfo().DictID
				slotLabel := ""
				for _, entry := range cs.TSlotList {
					if int(entry.MeshIndex) == mi {
						slotLabel = fmt.Sprintf("_slot%d", entry.SlotID)
						break
					}
				}
				fname := fmt.Sprintf("/tmp/helm_diag/csprite%d_place%d_allmat%d%s_dict%d.png",
					dictID, pi, mi, slotLabel, surfDictID)
				out, err := os.Create(fname)
				if err != nil {
					continue
				}
				png.Encode(out, surf.Image)
				out.Close()
			}
		}
	}
	fmt.Printf("\nTotal slot 7 base textures dumped: %d\n", helmCount)
}
