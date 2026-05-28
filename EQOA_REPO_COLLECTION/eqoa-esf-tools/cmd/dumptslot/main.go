package main

import (
	"fmt"
	"os"
	"strconv"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func main() {
	if len(os.Args) < 3 {
		fmt.Fprintf(os.Stderr, "usage: dumptslot <CHAR.ESF> <modelID> [modelID...]\n")
		os.Exit(1)
	}
	path := os.Args[1]

	f, err := esf.Open(path)
	if err != nil {
		fmt.Fprintf(os.Stderr, "open: %v\n", err)
		os.Exit(1)
	}
	if err := f.BuildDictionary(); err != nil {
		fmt.Fprintf(os.Stderr, "build dict: %v\n", err)
		os.Exit(1)
	}

	if os.Args[2] == "--verify-armor" {
		verifyArmorTextures(f)
		return
	}
	if os.Args[2] == "--verify-robe" {
		verifyRobeTextures(f)
		return
	}

	for _, arg := range os.Args[2:] {
		id, err := strconv.ParseInt(arg, 10, 32)
		if err != nil {
			fmt.Fprintf(os.Stderr, "bad modelID %q: %v\n", arg, err)
			continue
		}
		modelID := int32(id)
		dumpModel(f, modelID)
		fmt.Println()
	}
}

func dumpModel(f *esf.ObjFile, modelID int32) {
	obj, err := f.FindObject(modelID)
	if err != nil {
		fmt.Printf("model %d: FindObject error: %v\n", modelID, err)
		return
	}
	if obj == nil {
		fmt.Printf("model %d: not found\n", modelID)
		return
	}

	cs, ok := obj.(*esf.CSprite)
	if !ok {
		fmt.Printf("model %d: not a CSprite (type %T)\n", modelID, obj)
		return
	}

	fmt.Printf("=== Model %d ===\n", modelID)

	// Print TSlotList entries as-is
	if len(cs.TSlotList) == 0 {
		fmt.Println("  TSlotList: empty")
	} else {
		fmt.Printf("  TSlotList: %d entries\n", len(cs.TSlotList))
		for i, e := range cs.TSlotList {
			slotName := slotNames[int(e.SlotID)]
			fmt.Printf("    [%2d] field1=%2d  field2=%2d  field3=%d  (interpreted as: matIndex=%d → slot=%d/%s)\n",
				i, e.MeshIndex, e.SlotID, e.Flag, e.MeshIndex, e.SlotID, slotName)
		}
		// Also show swapped interpretation
		fmt.Println("  --- Swapped interpretation (field1=SlotID, field2=MeshIndex) ---")
		for i, e := range cs.TSlotList {
			slotName := slotNames[int(e.MeshIndex)]
			fmt.Printf("    [%2d] slot=%2d/%-8s  matIndex=%2d  flag=%d\n",
				i, e.MeshIndex, slotName, e.SlotID, e.Flag)
		}
	}

	// Count vertices per material index from placements
	placements := cs.GetSprites()

	type matInfo struct {
		vertCount int
		triCount  int
		surfID    int32
	}
	matCounts := make(map[int]*matInfo)

	for _, sp := range placements {
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

		for _, vl := range pb.VertexLists {
			mi := vl.Material
			info := matCounts[mi]
			if info == nil {
				info = &matInfo{}
				matCounts[mi] = info
				// Resolve surface for this material
				if mp != nil && mi >= 0 && mi < len(mp.Materials) {
					mat := mp.Materials[mi]
					if mat != nil && len(mat.Layers) > 0 && mat.Layers[0].Surface != nil {
						info.surfID = mat.Layers[0].Surface.ObjInfo().DictID
					}
				}
			}
			info.vertCount += len(vl.Vertices)
			// Triangle strip: N vertices → N-2 triangles
			if len(vl.Vertices) >= 3 {
				info.triCount += len(vl.Vertices) - 2
			}
		}
	}

	fmt.Printf("  Materials: %d unique\n", len(matCounts))
	for mi := 0; mi < 30; mi++ {
		info, ok := matCounts[mi]
		if !ok {
			continue
		}
		// Check TSlotList assignment for this mat index (current interpretation)
		tslotStr := "none"
		for _, e := range cs.TSlotList {
			if int(e.MeshIndex) == mi {
				tslotStr = fmt.Sprintf("slot=%d/%s (flag=%d)", e.SlotID, slotNames[int(e.SlotID)], e.Flag)
			}
		}
		swapStr := "none"
		for _, e := range cs.TSlotList {
			if int(e.SlotID) == mi {
				swapStr = fmt.Sprintf("slot=%d/%s (flag=%d)", e.MeshIndex, slotNames[int(e.MeshIndex)], e.Flag)
			}
		}
		fmt.Printf("    mat[%2d]: %5d verts, %4d tris, surfDictID=%12d, tslot=%s, swapped=%s\n",
			mi, info.vertCount, info.triCount, info.surfID, tslotStr, swapStr)
	}
}

func verifyRobeTextures(f *esf.ObjFile) {
	allInfos := f.AllObjects()
	var surfInfos []*esf.ObjInfo
	for _, info := range allInfos {
		if info.Type == esf.TypeSurface {
			surfInfos = append(surfInfos, info)
		}
	}

	robeIndices := [4][2]int32{{199, 200}, {201, 202}, {203, 204}, {205, 206}}
	for robe := 0; robe < 4; robe++ {
		for slot := 0; slot < 2; slot++ {
			idx := robeIndices[robe][slot]
			slotName := "chest"
			if slot == 1 {
				slotName = "legs"
			}
			if int(idx) >= len(surfInfos) {
				fmt.Printf("  robe %d %-5s idx=%d: OUT OF RANGE\n", robe, slotName, idx)
				continue
			}
			si := surfInfos[idx]
			obj, err := f.GetObject(si)
			if err != nil || obj == nil {
				fmt.Printf("  robe %d %-5s idx=%d dictID=%d: LOAD ERROR\n", robe, slotName, idx, si.DictID)
				continue
			}
			surf := obj.(*esf.Surface)
			if err := surf.Load(f); err != nil {
				fmt.Printf("  robe %d %-5s idx=%d dictID=%d: LOAD ERROR %v\n", robe, slotName, idx, si.DictID, err)
				continue
			}
			if surf.Image == nil {
				fmt.Printf("  robe %d %-5s idx=%d dictID=%d: NIL IMAGE\n", robe, slotName, idx, si.DictID)
				continue
			}
			img := surf.Image
			b := img.Bounds()
			w, h := b.Dx(), b.Dy()
			alphaZero, alphaLow, total := 0, 0, 0
			for y := 0; y < h; y++ {
				for x := 0; x < w; x++ {
					total++
					a := img.NRGBAAt(x, y).A
					if a == 0 {
						alphaZero++
					}
					if a < 64 {
						alphaLow++
					}
				}
			}
			fmt.Printf("  robe %d %-5s idx=%d dictID=%12d: %dx%d alpha0=%.1f%% alphaLow=%.1f%% hasAlpha=%v\n",
				robe, slotName, idx, si.DictID, w, h,
				float64(alphaZero)/float64(total)*100,
				float64(alphaLow)/float64(total)*100,
				surf.HasAlpha)
		}
	}
}

func verifyArmorTextures(f *esf.ObjFile) {
	armorTextures := [9][5]int32{
		{-1, -1, -1, -1, -1},
		{-2061243141, 969963525, -2002922121, 506406120, -605227631},
		{1334710637, -171824836, -2002922121, 969963525, -905154740},
		{1772850571, 1013378323, 1722970967, 506406120, -2061243141},
		{-605227631, 152262920, 1432086292, -1136804155, 299312765},
		{-1158474190, 404991724, -1260369019, -724082273, -568193900},
		{305809905, -830509086, 265192385, -2047685244, -1903828527},
		{265192385, -1158474190, 1640089331, -568193900, 1772850571},
		{1640089331, 1432086292, 265192385, -1819198730, 305809905},
	}
	slots := []string{"chest", "bracer", "gloves", "legs", "boots"}

	for set := 1; set <= 8; set++ {
		for slot := 0; slot < 5; slot++ {
			dictID := armorTextures[set][slot]
			obj, err := f.FindObject(dictID)
			if err != nil {
				fmt.Printf("  set %d %-6s dictID=%12d: ERROR %v\n", set, slots[slot], dictID, err)
				continue
			}
			if obj == nil {
				fmt.Printf("  set %d %-6s dictID=%12d: NOT FOUND\n", set, slots[slot], dictID)
				continue
			}
			surf, ok := obj.(*esf.Surface)
			if !ok {
				fmt.Printf("  set %d %-6s dictID=%12d: NOT A SURFACE (%T)\n", set, slots[slot], dictID, obj)
				continue
			}
			// Surface needs to be loaded first
			if err := surf.Load(f); err != nil {
				fmt.Printf("  set %d %-6s dictID=%12d: LOAD ERROR %v\n", set, slots[slot], dictID, err)
				continue
			}
			if surf.Image == nil {
				fmt.Printf("  set %d %-6s dictID=%12d: NIL IMAGE (%dx%d)\n", set, slots[slot], dictID, surf.W, surf.H)
				continue
			}
			img := surf.Image
			b := img.Bounds()
			w, h := b.Dx(), b.Dy()
			// Sample alpha channel
			alphaZero, total := 0, 0
			for y := 0; y < h; y += 2 {
				for x := 0; x < w; x += 2 {
					total++
					if img.NRGBAAt(x, y).A == 0 {
						alphaZero++
					}
				}
			}
			pct := 0.0
			if total > 0 {
				pct = float64(alphaZero) / float64(total) * 100
			}
			fmt.Printf("  set %d %-6s dictID=%12d: %dx%d alpha0=%.1f%% hasAlpha=%v\n", set, slots[slot], dictID, w, h, pct, surf.HasAlpha)
		}
	}
}

var slotNames = map[int]string{
	-1: "unknown",
	0:  "chest",
	1:  "bracer",
	2:  "gloves",
	3:  "legs",
	4:  "boots",
	5:  "hair",
	6:  "face",
	7:  "helm",
	8:  "robe",
	9:  "slot9",
	10: "slot10",
	11: "slot11",
	12: "slot12",
	13: "slot13",
}
