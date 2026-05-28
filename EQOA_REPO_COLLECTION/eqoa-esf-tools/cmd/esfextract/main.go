package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func main() {
	var (
		zoneIdx    int
		zoneRange  string
		charMode   bool
		listZones  bool
		decompCSF  string
		outputFile string
		sizeCutoff float64
		lowDetail  bool
		exportColl bool
		debug      bool
		treeMode   bool
		actorMode  bool
		spriteMode bool
		jsonOutput bool
		maxDepth   int
		extractESF bool
		modifyY    float64
	)

	flag.IntVar(&zoneIdx, "zone", -1, "Export a single zone by index (e.g. 84 for Freeport)")
	flag.StringVar(&zoneRange, "zones", "", "Export zone range (e.g. '74-84')")
	flag.BoolVar(&charMode, "chars", false, "Export character/NPC models from CHAR.ESF")
	flag.BoolVar(&listZones, "list", false, "List all zones with bounding boxes")
	flag.StringVar(&decompCSF, "decompress", "", "Decompress a CSF file to ESF")
	flag.StringVar(&outputFile, "o", "output.obj", "Output OBJ filename")
	flag.Float64Var(&sizeCutoff, "cutoff", 0, "Size cutoff for small objects (0=include all)")
	flag.BoolVar(&lowDetail, "low", false, "Use low detail LOD models")
	flag.BoolVar(&exportColl, "coll", false, "Export collision meshes instead of visual")
	flag.BoolVar(&debug, "debug", false, "Enable debug logging")
	flag.BoolVar(&treeMode, "tree", false, "Dump object tree hierarchy")
	flag.BoolVar(&actorMode, "actors", false, "List zone actors with sprite types and positions")
	flag.BoolVar(&spriteMode, "sprites", false, "List all sprites in dictionary")
	flag.BoolVar(&jsonOutput, "json", false, "Output as JSON (use with -tree, -actors, or -sprites)")
	flag.IntVar(&maxDepth, "depth", 0, "Max tree depth, 0=unlimited (use with -tree)")
	flag.BoolVar(&extractESF, "esf", false, "Extract zone as standalone ESF file (use with -zone and -o)")
	flag.Float64Var(&modifyY, "yscale", 0, "Scale Y coordinates of zone terrain (use with -esf)")

	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "esfextract - EQOA ESF/CSF asset extractor (Go port of joukop/ESF-file-format)\n\n")
		fmt.Fprintf(os.Stderr, "Usage:\n")
		fmt.Fprintf(os.Stderr, "  esfextract [flags] <input.esf>\n\n")
		fmt.Fprintf(os.Stderr, "Examples:\n")
		fmt.Fprintf(os.Stderr, "  esfextract -list TUNARIA.ESF              # List all zones\n")
		fmt.Fprintf(os.Stderr, "  esfextract -zone 84 -o freeport.obj TUNARIA.ESF   # Export Freeport\n")
		fmt.Fprintf(os.Stderr, "  esfextract -zones 74-84 -cutoff 20 TUNARIA.ESF    # Export zones 74-84\n")
		fmt.Fprintf(os.Stderr, "  esfextract -chars -o models.obj CHAR.ESF  # Export all character models\n")
		fmt.Fprintf(os.Stderr, "  esfextract -decompress UI.CSF             # Decompress CSF to ESF\n")
		fmt.Fprintf(os.Stderr, "  esfextract -tree TUNARIA.ESF              # Dump object tree\n")
		fmt.Fprintf(os.Stderr, "  esfextract -tree -depth 3 TUNARIA.ESF     # Tree with max depth\n")
		fmt.Fprintf(os.Stderr, "  esfextract -actors -zone 84 TUNARIA.ESF   # List actors in Freeport\n")
		fmt.Fprintf(os.Stderr, "  esfextract -actors TUNARIA.ESF | grep CSprite  # Find NPC placements\n")
		fmt.Fprintf(os.Stderr, "  esfextract -actors -json TUNARIA.ESF      # JSON actor output\n")
		fmt.Fprintf(os.Stderr, "  esfextract -sprites CHAR.ESF              # List sprite dictionary\n\n")
		fmt.Fprintf(os.Stderr, "Flags:\n")
		flag.PrintDefaults()
	}

	flag.Parse()

	// Handle CSF decompression
	if decompCSF != "" {
		outPath := strings.TrimSuffix(decompCSF, filepath.Ext(decompCSF)) + ".ESF"
		log.Printf("Decompressing %s → %s", decompCSF, outPath)
		if err := esf.DecompressCSFToFile(decompCSF, outPath); err != nil {
			log.Fatal(err)
		}
		log.Printf("Done")
		return
	}

	if flag.NArg() < 1 {
		flag.Usage()
		os.Exit(1)
	}

	inputFile := flag.Arg(0)
	esf.LODLowLevel = lowDetail

	log.Printf("Opening %s...", inputFile)
	file, err := esf.Open(inputFile)
	if err != nil {
		log.Fatal(err)
	}
	file.Debug = debug

	// Character model export
	if charMode {
		exportChars(file, outputFile)
		return
	}

	root, err := file.Root()
	if err != nil {
		log.Fatal(err)
	}

	// Tree dump mode
	if treeMode {
		dumpTree(root, jsonOutput, maxDepth)
		return
	}

	// Sprite dictionary mode
	if spriteMode {
		listSprites(file, jsonOutput)
		return
	}

	// List zones
	if listZones {
		listAllZones(file, root)
		return
	}

	// Actor listing mode
	if actorMode {
		listActors(file, root, zoneIdx, zoneRange, jsonOutput)
		return
	}

	// Extract zone as standalone ESF
	if extractESF {
		if zoneIdx < 0 {
			log.Fatal("-esf requires -zone")
		}
		log.Printf("Extracting zone %d as standalone ESF...", zoneIdx)
		data, err := esf.ExtractZoneESF(file, zoneIdx)
		if err != nil {
			log.Fatal(err)
		}
		// Apply Y-scale modification if requested
		if modifyY != 0 {
			log.Printf("Scaling Y coordinates by %.2f...", modifyY)
			if err := esf.PatchPrimBufferY(data, float32(modifyY)); err != nil {
				log.Fatalf("Patching Y: %v", err)
			}
		}

		outPath := outputFile
		if strings.HasSuffix(strings.ToLower(outPath), ".obj") {
			outPath = strings.TrimSuffix(outPath, filepath.Ext(outPath)) + ".esf"
		}
		if err := os.WriteFile(outPath, data, 0o644); err != nil {
			log.Fatal(err)
		}
		log.Printf("Wrote %s (%d bytes)", outPath, len(data))

		// Verify: re-read the file and count objects
		verify, err := esf.OpenBytes(data)
		if err != nil {
			log.Fatalf("Verification FAILED: %v", err)
		}
		vroot, err := verify.Root()
		if err != nil {
			log.Fatalf("Verification FAILED: %v", err)
		}
		log.Printf("Verification OK: root type=%s, %d children",
			esf.TypeName(vroot.Type), len(vroot.Children))
		return
	}

	// Export zone(s)
	worldInfo := root.Child(esf.TypeWorld)
	if worldInfo == nil {
		log.Fatal("No World object found in file")
	}
	zones := worldInfo.ChildrenOfType(esf.TypeZone)
	log.Printf("Found %d zones", len(zones))

	if zoneIdx >= 0 {
		exportSingleZone(file, zones, zoneIdx, outputFile, float32(sizeCutoff), exportColl)
	} else if zoneRange != "" {
		var start, end int
		if _, err := fmt.Sscanf(zoneRange, "%d-%d", &start, &end); err != nil {
			log.Fatalf("Invalid zone range %q (use e.g. '74-84')", zoneRange)
		}
		exportZoneRange(file, zones, start, end, outputFile, float32(sizeCutoff), exportColl)
	} else {
		flag.Usage()
		os.Exit(1)
	}
}

// --- Tree dump ---

type treeNode struct {
	Type     string `json:"type"`
	TypeCode uint16 `json:"type_code"`
	Version  int16  `json:"version"`
	Size     int32  `json:"size"`
	Subs     int32  `json:"sub_objects"`
	DictID   int32  `json:"dict_id,omitempty"`
	Children []treeNode `json:"children,omitempty"`
}

func dumpTree(root *esf.ObjInfo, asJSON bool, maxDepth int) {
	if asJSON {
		node := buildTreeNode(root, 0, maxDepth)
		enc := json.NewEncoder(os.Stdout)
		enc.SetIndent("", "  ")
		if err := enc.Encode(node); err != nil {
			log.Fatal(err)
		}
		return
	}
	printTreeNode(root, 0, maxDepth)
}

func buildTreeNode(info *esf.ObjInfo, depth, maxDepth int) treeNode {
	node := treeNode{
		Type:     esf.TypeName(info.Type),
		TypeCode: info.Type,
		Version:  info.Version,
		Size:     info.Size,
		Subs:     info.NumSubObjects,
		DictID:   info.DictID,
	}
	if maxDepth > 0 && depth >= maxDepth {
		return node
	}
	for _, child := range info.Children {
		node.Children = append(node.Children, buildTreeNode(child, depth+1, maxDepth))
	}
	return node
}

func printTreeNode(info *esf.ObjInfo, depth, maxDepth int) {
	indent := strings.Repeat("  ", depth)
	dictStr := ""
	if info.DictID != 0 {
		dictStr = fmt.Sprintf(" dict=0x%08x", info.DictID)
	}
	fmt.Printf("%s%s(0x%04x) v%d size=0x%x subs=%d%s\n",
		indent, esf.TypeName(info.Type), info.Type, info.Version, info.Size, info.NumSubObjects, dictStr)
	if maxDepth > 0 && depth >= maxDepth {
		return
	}
	for _, child := range info.Children {
		printTreeNode(child, depth+1, maxDepth)
	}
}

// --- Actor listing ---

type jsonZoneActors struct {
	ZoneIndex int         `json:"zone_index"`
	ZoneName  string      `json:"zone_name"`
	Actors    []jsonActor `json:"actors"`
}

type jsonActor struct {
	SpriteID   int32      `json:"sprite_id"`
	SpriteType string     `json:"sprite_type"`
	Pos        [3]float32 `json:"pos"`
	Rot        [3]float32 `json:"rot"`
	Scale      float32    `json:"scale"`
}

func listActors(file *esf.ObjFile, root *esf.ObjInfo, zoneIdx int, zoneRange string, asJSON bool) {
	worldInfo := root.Child(esf.TypeWorld)
	if worldInfo == nil {
		log.Fatal("No World object found in file")
	}

	// Load zone names from proxies
	zoneNames := loadZoneNames(file, root)

	zones := worldInfo.ChildrenOfType(esf.TypeZone)

	// Build dictionary for sprite type resolution
	if err := file.BuildDictionary(); err != nil {
		log.Fatal(err)
	}

	// Determine which zones to process
	startIdx, endIdx := 0, len(zones)-1
	if zoneIdx >= 0 {
		startIdx, endIdx = zoneIdx, zoneIdx
	} else if zoneRange != "" {
		if _, err := fmt.Sscanf(zoneRange, "%d-%d", &startIdx, &endIdx); err != nil {
			log.Fatalf("Invalid zone range %q", zoneRange)
		}
	}
	if endIdx >= len(zones) {
		endIdx = len(zones) - 1
	}

	var allResults []jsonZoneActors

	for i := startIdx; i <= endIdx; i++ {
		zoneObj, err := file.GetObject(zones[i])
		if err != nil {
			log.Printf("Error loading zone %d: %v", i, err)
			continue
		}
		if zoneObj == nil {
			continue
		}
		zone := zoneObj.(*esf.Zone)
		actors, err := zone.GetZoneActors(file)
		if err != nil {
			log.Printf("Error loading actors for zone %d: %v", i, err)
			continue
		}
		if actors == nil || len(actors.GetActors()) == 0 {
			continue
		}

		zoneName := "(unnamed)"
		if i < len(zoneNames) && zoneNames[i] != "" {
			zoneName = zoneNames[i]
		}

		if asJSON {
			result := jsonZoneActors{
				ZoneIndex: i,
				ZoneName:  zoneName,
			}
			for _, actor := range actors.GetActors() {
				p := actor.Placement
				spriteType := resolveSpriteType(file, p.SpriteID)
				result.Actors = append(result.Actors, jsonActor{
					SpriteID:   p.SpriteID,
					SpriteType: spriteType,
					Pos:        [3]float32{p.Pos.X, p.Pos.Y, p.Pos.Z},
					Rot:        [3]float32{p.Rot.X, p.Rot.Y, p.Rot.Z},
					Scale:      p.GetScale(),
				})
			}
			allResults = append(allResults, result)
		} else {
			fmt.Printf("Zone %d: %s\n", i, zoneName)
			for _, actor := range actors.GetActors() {
				p := actor.Placement
				spriteType := resolveSpriteType(file, p.SpriteID)
				fmt.Printf("  Actor spriteID=0x%08x type=%-18s pos=(%.1f, %.1f, %.1f) rot=(%.2f, %.2f, %.2f) scale=%.1f\n",
					uint32(p.SpriteID), spriteType,
					p.Pos.X, p.Pos.Y, p.Pos.Z,
					p.Rot.X, p.Rot.Y, p.Rot.Z,
					p.GetScale())
			}
		}
	}

	if asJSON {
		enc := json.NewEncoder(os.Stdout)
		enc.SetIndent("", "  ")
		if err := enc.Encode(allResults); err != nil {
			log.Fatal(err)
		}
	}
}

func resolveSpriteType(file *esf.ObjFile, spriteID int32) string {
	if spriteID == 0 {
		return "(none)"
	}
	obj, err := file.FindObject(spriteID)
	if err != nil || obj == nil {
		return "(unresolved)"
	}
	return esf.TypeName(obj.ObjInfo().Type)
}

func loadZoneNames(file *esf.ObjFile, root *esf.ObjInfo) []string {
	worldBaseInfo := root.Child(esf.TypeWorldBase)
	if worldBaseInfo == nil {
		return nil
	}
	proxiesInfo := worldBaseInfo.Child(esf.TypeWorldZoneProxies)
	if proxiesInfo == nil {
		return nil
	}
	obj, err := file.GetObject(proxiesInfo)
	if err != nil {
		return nil
	}
	proxies := obj.(*esf.WorldZoneProxies)
	names := make([]string, len(proxies.Zones))
	for i, z := range proxies.Zones {
		names[i] = z.Name
	}
	return names
}

// --- Sprite dictionary ---

type jsonSprite struct {
	DictID   int32      `json:"dict_id"`
	Type     string     `json:"type"`
	BBox     *[3]float32 `json:"bbox_size,omitempty"`
}

func listSprites(file *esf.ObjFile, asJSON bool) {
	if err := file.BuildDictionary(); err != nil {
		log.Fatal(err)
	}

	// Collect all objects with DictIDs
	var results []jsonSprite
	seen := make(map[int32]bool)

	for _, info := range file.AllObjects() {
		if info.DictID == 0 || seen[info.DictID] {
			continue
		}
		seen[info.DictID] = true

		obj, err := file.FindObject(info.DictID)
		typeName := esf.TypeName(info.Type)
		if err == nil && obj != nil {
			typeName = esf.TypeName(obj.ObjInfo().Type)
		}

		entry := jsonSprite{
			DictID: info.DictID,
			Type:   typeName,
		}

		// Try to get BBox for sprite types
		if obj != nil {
			if sp, ok := obj.(*esf.SimpleSprite); ok && !sp.BBox.IsEmpty() {
				dim := sp.BBox.Dimensions()
				entry.BBox = &[3]float32{dim.X, dim.Y, dim.Z}
			} else if sp, ok := obj.(*esf.SimpleSubSprite); ok && !sp.BBox.IsEmpty() {
				dim := sp.BBox.Dimensions()
				entry.BBox = &[3]float32{dim.X, dim.Y, dim.Z}
			} else if sp, ok := obj.(*esf.GroupSprite); ok && !sp.BBox.IsEmpty() {
				dim := sp.BBox.Dimensions()
				entry.BBox = &[3]float32{dim.X, dim.Y, dim.Z}
			} else if sp, ok := obj.(*esf.LODSprite); ok && !sp.BBox.IsEmpty() {
				dim := sp.BBox.Dimensions()
				entry.BBox = &[3]float32{dim.X, dim.Y, dim.Z}
			} else if sp, ok := obj.(*esf.CSprite); ok && !sp.BBox.IsEmpty() {
				dim := sp.BBox.Dimensions()
				entry.BBox = &[3]float32{dim.X, dim.Y, dim.Z}
			} else if sp, ok := obj.(*esf.HSprite); ok && !sp.BBox.IsEmpty() {
				dim := sp.BBox.Dimensions()
				entry.BBox = &[3]float32{dim.X, dim.Y, dim.Z}
			}
		}

		results = append(results, entry)
	}

	if asJSON {
		enc := json.NewEncoder(os.Stdout)
		enc.SetIndent("", "  ")
		if err := enc.Encode(results); err != nil {
			log.Fatal(err)
		}
		return
	}

	for _, r := range results {
		if r.BBox != nil {
			fmt.Printf("DictID=0x%08x type=%-18s bbox=%.1fx%.1fx%.1f\n",
				uint32(r.DictID), r.Type, r.BBox[0], r.BBox[1], r.BBox[2])
		} else {
			fmt.Printf("DictID=0x%08x type=%-18s (no bbox)\n",
				uint32(r.DictID), r.Type)
		}
	}
}

// --- Existing functions ---

func listAllZones(file *esf.ObjFile, root *esf.ObjInfo) {
	worldBaseInfo := root.Child(esf.TypeWorldBase)
	if worldBaseInfo == nil {
		log.Fatal("No WorldBase found")
	}
	proxiesInfo := worldBaseInfo.Child(esf.TypeWorldZoneProxies)
	if proxiesInfo == nil {
		log.Fatal("No WorldZoneProxies found")
	}
	obj, err := file.GetObject(proxiesInfo)
	if err != nil {
		log.Fatal(err)
	}
	proxies := obj.(*esf.WorldZoneProxies)
	for i, z := range proxies.Zones {
		name := z.Name
		if name == "" {
			name = "(unnamed)"
		}
		dim := z.BBox.Dimensions()
		fmt.Printf("Zone %3d: %s  center=(%.0f, %.0f, %.0f)  size=%.0fx%.0f  bbox=(%.0f,%.0f,%.0f)-(%.0f,%.0f,%.0f)\n",
			i, name, z.Center.X, z.Center.Y, z.Center.Z, dim.X, dim.Z,
			z.BBox.MinX, z.BBox.MinY, z.BBox.MinZ, z.BBox.MaxX, z.BBox.MaxY, z.BBox.MaxZ)
	}
}

func exportSingleZone(file *esf.ObjFile, zones []*esf.ObjInfo, idx int, output string, cutoff float32, coll bool) {
	if idx >= len(zones) {
		log.Fatalf("Zone index %d out of range (max %d)", idx, len(zones)-1)
	}
	log.Printf("Loading zone %d...", idx)
	zoneObj, err := file.GetObject(zones[idx])
	if err != nil {
		log.Fatal(err)
	}
	zone := zoneObj.(*esf.Zone)

	log.Printf("Getting sprite placements...")
	placements, err := zone.GetSpritePlacements(file)
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("Got %d placements", len(placements))

	exp := esf.NewExporter()
	exp.SizeCutoff = cutoff
	exp.ExportColl = coll

	if err := exp.AddAll(placements, file); err != nil {
		log.Fatal(err)
	}
	exp.Center()

	log.Printf("Writing %s...", output)
	if err := exp.Write(output); err != nil {
		log.Fatal(err)
	}
	log.Printf("Done! Vertices: %d", exp.VertexCount())
}

func exportZoneRange(file *esf.ObjFile, zones []*esf.ObjInfo, start, end int, output string, cutoff float32, coll bool) {
	if end >= len(zones) {
		end = len(zones) - 1
	}
	base := strings.TrimSuffix(output, filepath.Ext(output))

	for i := start; i <= end; i++ {
		log.Printf("Loading zone %d...", i)
		zoneObj, err := file.GetObject(zones[i])
		if err != nil {
			log.Printf("Error loading zone %d: %v", i, err)
			continue
		}
		zone := zoneObj.(*esf.Zone)

		placements, err := zone.GetSpritePlacements(file)
		if err != nil {
			log.Printf("Error getting placements for zone %d: %v", i, err)
			continue
		}

		exp := esf.NewExporter()
		exp.SizeCutoff = cutoff
		exp.ExportColl = coll

		if err := exp.AddAll(placements, file); err != nil {
			log.Printf("Error adding placements for zone %d: %v", i, err)
			continue
		}
		exp.Center()

		fn := fmt.Sprintf("%s-zone%d.obj", base, i)
		log.Printf("Writing %s (%d placements)...", fn, len(placements))
		if err := exp.Write(fn); err != nil {
			log.Printf("Error writing zone %d: %v", i, err)
			continue
		}
	}
	log.Printf("Done!")
}

func exportChars(file *esf.ObjFile, output string) {
	root, err := file.Root()
	if err != nil {
		log.Fatal(err)
	}

	// CHAR.ESF has ResourceDir2 containing CSprites
	resDir := root.Child(esf.TypeResourceDir2)
	if resDir == nil {
		log.Fatal("No ResourceDir2 found (is this CHAR.ESF?)")
	}

	exp := esf.NewExporter()
	var offX, offZ float32
	boxCol := esf.NewBox()
	count := 0

	for _, child := range resDir.ChildrenOfType(0) {
		obj, err := file.GetObject(child)
		if err != nil {
			continue
		}

		var placements []*esf.SpritePlacement
		switch s := obj.(type) {
		case *esf.CSprite:
			placements = s.GetSprites()
		case *esf.SimpleSprite:
			placements = []*esf.SpritePlacement{{Sprite: s}}
		default:
			continue
		}

		for _, sp := range placements {
			sp.Pos.X = offX
			sp.Pos.Z = offZ
			if err := exp.Add(sp, file); err != nil {
				continue
			}
			sprite, _ := sp.GetSprite(file)
			if sprite != nil && !sprite.BBox.IsEmpty() {
				boxCol.AddBox(sprite.BBox)
				offX += sprite.BBox.Dimensions().X * 1.5
				if offX > 30 {
					offX = 0
					offZ += boxCol.Dimensions().Z * 1.5
					boxCol = esf.NewBox()
				}
			}
		}
		count++
	}

	exp.Center()
	log.Printf("Writing %s (%d models)...", output, count)
	if err := exp.Write(output); err != nil {
		log.Fatal(err)
	}
	log.Printf("Done!")
}
