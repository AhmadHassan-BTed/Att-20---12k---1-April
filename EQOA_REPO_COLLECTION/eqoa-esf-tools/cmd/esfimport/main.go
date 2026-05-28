// esfimport: Import OBJ meshes into EQOA zone ESF files.
//
// Works by REPLACING existing PrimBuffer data — never adds or removes nodes,
// since the PS2 parser relies on fixed sub-object counts.
//
// Modes:
//   1. Replace a specific sprite's PrimBuffer with OBJ geometry
//   2. Replace all terrain tile PrimBuffers with OBJ geometry
//   3. List replaceable sprites in a zone
//
// Usage:
//   esfimport -list -zone 84 TUNARIA.ESF
//   esfimport -obj cube.obj -replace 0 -zone 84 -o zone_84.esf TUNARIA.ESF
//   esfimport -obj terrain.obj -terrain -zone 84 -o zone_84.esf TUNARIA.ESF
package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"strconv"
	"strings"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func main() {
	objFile := flag.String("obj", "", "Input OBJ file")
	replaceIdx := flag.Int("replace", -1, "Replace PrimBuffer at this resource index (use -list to find)")
	zoneIdx := flag.Int("zone", -1, "Target zone index")
	outFile := flag.String("o", "", "Output zone ESF file")
	listMode := flag.Bool("list", false, "List all PrimBuffer resources in zone")
	terrain := flag.Bool("terrain", false, "Replace all terrain tile PrimBuffers")
	strips := flag.Bool("strips", false, "Use degenerate triangle strips (more efficient)")
	sculpt := flag.String("sculpt", "", "Sculpt terrain: raise,x,z,radius,height or flatten,x,z,radius,height")
	addPos := flag.String("add", "", "Add OBJ as new SimpleSprite at x,y,z (e.g. -add 25000,100,15000)")
	addScale := flag.Float64("scale", 1.0, "Scale for added object")
	addName := flag.String("name", "custom_obj", "Name for added object (used for DictID hash)")

	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "esfimport - Import OBJ / sculpt terrain in EQOA zone ESF files\n\n")
		fmt.Fprintf(os.Stderr, "Usage:\n")
		fmt.Fprintf(os.Stderr, "  List:    esfimport -list -zone 84 TUNARIA.ESF\n")
		fmt.Fprintf(os.Stderr, "  Replace: esfimport -obj cube.obj -replace 5 -zone 84 -o zone_84.esf TUNARIA.ESF\n")
		fmt.Fprintf(os.Stderr, "  Add:     esfimport -obj cube.obj -add 25000,100,15000 -zone 84 -o zone_84.esf TUNARIA.ESF\n")
		fmt.Fprintf(os.Stderr, "  Terrain: esfimport -obj terrain.obj -terrain -zone 84 -o zone_84.esf TUNARIA.ESF\n")
		fmt.Fprintf(os.Stderr, "  Sculpt:  esfimport -sculpt raise,25000,15000,200,50 -zone 84 -o zone_84.esf TUNARIA.ESF\n\n")
		fmt.Fprintf(os.Stderr, "Flags:\n")
		flag.PrintDefaults()
	}
	flag.Parse()

	if *zoneIdx < 0 || flag.NArg() < 1 {
		flag.Usage()
		os.Exit(1)
	}
	if *outFile == "" {
		*outFile = fmt.Sprintf("zone_%d.esf", *zoneIdx)
	}

	srcPath := flag.Arg(0)
	log.Printf("Opening %s...", srcPath)
	src, err := esf.Open(srcPath)
	if err != nil {
		log.Fatal(err)
	}

	root, err := src.Root()
	if err != nil {
		log.Fatal(err)
	}

	worldInfo := root.Child(esf.TypeWorld)
	if worldInfo == nil {
		log.Fatal("no World object found")
	}

	zones := worldInfo.ChildrenOfType(esf.TypeZone)
	if *zoneIdx >= len(zones) {
		log.Fatalf("zone %d out of range (max %d)", *zoneIdx, len(zones)-1)
	}

	zoneInfo := zones[*zoneIdx]

	if *listMode {
		listResources(src, zoneInfo)
		return
	}

	if *sculpt != "" {
		doSculpt(src, zoneInfo, *sculpt, *zoneIdx, *outFile)
		return
	}

	if *objFile == "" {
		log.Fatal("specify -obj, -sculpt, or -list")
	}

	if *addPos != "" {
		pos := parseVec3(*addPos)
		mesh, err := esf.ImportOBJ(*objFile)
		if err != nil {
			log.Fatalf("import OBJ: %v", err)
		}
		log.Printf("OBJ: %s", mesh.Stats())
		var pb *esf.PrimBuffer
		if *strips {
			pb = mesh.ToPrimBufferStrips()
		} else {
			pb = mesh.ToPrimBuffer()
		}
		doAdd(src, root, zoneInfo, pb, pos, float32(*addScale), *addName, *outFile)
		return
	}

	// Import OBJ
	log.Printf("Importing %s...", *objFile)
	mesh, err := esf.ImportOBJ(*objFile)
	if err != nil {
		log.Fatalf("import OBJ: %v", err)
	}
	log.Printf("OBJ: %s", mesh.Stats())

	var pb *esf.PrimBuffer
	if *strips {
		pb = mesh.ToPrimBufferStrips()
	} else {
		pb = mesh.ToPrimBuffer()
	}
	log.Printf("PrimBuffer: %d vertex lists, bbox=%v", len(pb.VertexLists), pb.BBox)

	if *terrain {
		replaceTerrainPBs(src, root, zoneInfo, pb, *outFile)
	} else if *replaceIdx >= 0 {
		replaceSinglePB(src, root, zoneInfo, pb, *replaceIdx, *outFile)
	} else {
		log.Fatal("specify -replace INDEX or -terrain")
	}
}

// listResources prints all PrimBuffer-containing resources in the zone.
func listResources(src *esf.ObjFile, zoneInfo *esf.ObjInfo) {
	resources := zoneInfo.Child(esf.TypeZoneResources)
	if resources == nil {
		log.Fatal("no ZoneResources")
	}

	idx := 0
	for _, res := range resources.Children {
		// Find PrimBuffers in this resource
		var pbs []*esf.ObjInfo
		collectType(res, esf.TypePrimBuffer, &pbs)
		collectType(res, esf.TypeSkinPrimBuffer, &pbs)

		if len(pbs) > 0 {
			typeName := esf.TypeName(res.Type)
			totalVerts := 0
			for _, pb := range pbs {
				totalVerts += countPBVertices(src, pb)
			}
			fmt.Printf("  [%3d] %s dict=0x%08x  %d PrimBuffer(s), ~%d verts, size=%d bytes\n",
				idx, typeName, uint32(res.DictID), len(pbs), totalVerts, res.Size)
		}
		idx++
	}
	fmt.Printf("\nTotal: %d resources\n", idx)
}

func collectType(info *esf.ObjInfo, typ uint16, out *[]*esf.ObjInfo) {
	if info.Type == typ {
		*out = append(*out, info)
	}
	for _, c := range info.Children {
		collectType(c, typ, out)
	}
}

func countPBVertices(src *esf.ObjFile, pb *esf.ObjInfo) int {
	// Quick count from raw bytes without full parse
	pos := pb.Offset
	ver := pb.Version
	if ver == 0 {
		if pos+12 > len(src.Data()) {
			return 0
		}
		nfaces := int(binary.LittleEndian.Uint32(src.Data()[pos+8:]))
		total := 0
		p := pos + 12
		for i := 0; i < nfaces; i++ {
			if p+8 > len(src.Data()) {
				break
			}
			nverts := int(binary.LittleEndian.Uint32(src.Data()[p:]))
			total += nverts
			p += 8 + nverts*36
		}
		return total
	}
	if ver > 1 {
		pos += 4
	}
	if pos+28 > len(src.Data()) {
		return 0
	}
	pbtype := int(binary.LittleEndian.Uint32(src.Data()[pos:]))
	nfaces := int(binary.LittleEndian.Uint32(src.Data()[pos+8:]))
	pos += 28
	total := 0
	stride := 17
	if pbtype == 4 {
		stride = 19
	} else if pbtype == 5 {
		stride = 21
	}
	for i := 0; i < nfaces; i++ {
		if pos+8 > len(src.Data()) {
			break
		}
		nverts := int(binary.LittleEndian.Uint32(src.Data()[pos:]))
		total += nverts
		pos += 8 + nverts*stride
	}
	return total
}

// replaceSinglePB replaces the PrimBuffer in resource at the given index
// with the imported OBJ geometry. The zone structure stays identical —
// only the PrimBuffer body bytes change (and size adjusts).
func replaceSinglePB(src *esf.ObjFile, root, zoneInfo *esf.ObjInfo, pb *esf.PrimBuffer, resIdx int, outPath string) {
	resources := zoneInfo.Child(esf.TypeZoneResources)
	if resources == nil {
		log.Fatal("no ZoneResources")
	}
	if resIdx >= len(resources.Children) {
		log.Fatalf("resource index %d out of range (max %d)", resIdx, len(resources.Children)-1)
	}

	targetRes := resources.Children[resIdx]
	log.Printf("Replacing PrimBuffer in resource [%d] %s dict=0x%08x",
		resIdx, esf.TypeName(targetRes.Type), uint32(targetRes.DictID))

	// Find the PrimBuffer to replace
	var targetPB *esf.ObjInfo
	var allPBs []*esf.ObjInfo
	collectType(targetRes, esf.TypePrimBuffer, &allPBs)
	collectType(targetRes, esf.TypeSkinPrimBuffer, &allPBs)
	if len(allPBs) == 0 {
		log.Fatal("resource has no PrimBuffer to replace")
	}
	targetPB = allPBs[0]
	log.Printf("  Target PrimBuffer: offset=0x%x size=%d", targetPB.Offset, targetPB.Size)

	// Build new zone ESF with replaced PrimBuffer
	writeReplacedZone(src, root, zoneInfo, targetPB, pb, outPath)
}

// replaceTerrainPBs replaces ALL SimpleSubSprite PrimBuffers with the OBJ.
// The OBJ replaces the first terrain tile's PrimBuffer; other tiles get
// zeroed-out PrimBuffers (empty faces).
func replaceTerrainPBs(src *esf.ObjFile, root, zoneInfo *esf.ObjInfo, pb *esf.PrimBuffer, outPath string) {
	resources := zoneInfo.Child(esf.TypeZoneResources)
	if resources == nil {
		log.Fatal("no ZoneResources")
	}

	// Find all terrain PrimBuffers
	var terrainPBs []*esf.ObjInfo
	for _, res := range resources.Children {
		if res.Type == esf.TypeSimpleSubSprite {
			var pbs []*esf.ObjInfo
			collectType(res, esf.TypePrimBuffer, &pbs)
			terrainPBs = append(terrainPBs, pbs...)
		}
	}

	if len(terrainPBs) == 0 {
		log.Fatal("no terrain PrimBuffers found")
	}

	log.Printf("Found %d terrain PrimBuffers — replacing first, zeroing rest", len(terrainPBs))

	// Build replacement map: first PB gets OBJ, rest get empty
	writeReplacedZone(src, root, zoneInfo, terrainPBs[0], pb, outPath)
}

// writeReplacedZone rebuilds the zone ESF, replacing one PrimBuffer's data.
// All other nodes are copied verbatim. The zone structure (node count,
// hierarchy) stays identical — only sizes adjust.
func writeReplacedZone(src *esf.ObjFile, root, zoneInfo *esf.ObjInfo, targetPB *esf.ObjInfo, newPB *esf.PrimBuffer, outPath string) {
	worldBase := root.Child(esf.TypeWorldBase)

	w := esf.NewWriter()
	worldH := w.WriteNodeBegin(esf.TypeWorld, 0, 1)

	// Recursively rebuild the zone, replacing only the target PrimBuffer
	writeNodeReplacing(w, src, zoneInfo, targetPB, newPB)

	w.WriteNodeEnd(worldH)

	if worldBase != nil {
		w.WriteNodeRaw(worldBase, src)
	}

	data := w.Finalize()
	if err := os.WriteFile(outPath, data, 0644); err != nil {
		log.Fatal(err)
	}
	log.Printf("Wrote %s (%d bytes)", outPath, len(data))

	// Verify
	verify, err := esf.OpenBytes(data)
	if err != nil {
		log.Fatalf("verification failed: %v", err)
	}
	vroot, err := verify.Root()
	if err != nil {
		log.Fatalf("verification failed: %v", err)
	}
	log.Printf("Verification OK: root type=%s, %d children",
		esf.TypeName(vroot.Type), len(vroot.Children))
}

// writeNodeReplacing recursively writes a node, replacing the target PrimBuffer
// with new data while copying everything else verbatim.
func writeNodeReplacing(w *esf.ESFWriter, src *esf.ObjFile, info *esf.ObjInfo, targetPB *esf.ObjInfo, newPB *esf.PrimBuffer) {
	if info == targetPB {
		// Replace this PrimBuffer with the imported mesh
		dictID := info.DictID
		if dictID == 0 {
			dictID = 1 // fallback
		}
		w.WritePrimBuffer(newPB, dictID)
		log.Printf("  Replaced PrimBuffer (orig %d bytes → new PrimBuffer with %d vertex lists)",
			info.Size+12, len(newPB.VertexLists))
		return
	}

	if len(info.Children) == 0 {
		// Leaf node — copy verbatim
		w.WriteNodeRaw(info, src)
		return
	}

	// Interior node — rebuild with same header, recurse into children
	h := w.WriteNodeBegin(info.Type, info.Version, info.NumSubObjects)

	// Write this node's own body data (before children)
	bodyData := nodeBodyData(src, info)
	w.WriteBytes(bodyData)

	// Recurse into children
	for _, child := range info.Children {
		writeNodeReplacing(w, src, child, targetPB, newPB)
	}

	w.WriteNodeEnd(h)
}

// nodeBodyData returns the body bytes of a node EXCLUDING its children.
// This is the data between the node header end and the first child start.
func nodeBodyData(src *esf.ObjFile, info *esf.ObjInfo) []byte {
	bodyStart := info.Offset
	if len(info.Children) == 0 {
		// No children — entire body is data
		return src.RawBytes(bodyStart, int(info.Size))
	}
	// Body data = from bodyStart to first child's header start
	firstChildStart := info.Children[0].Offset - 12 // 12-byte header before body
	dataLen := firstChildStart - bodyStart
	if dataLen <= 0 {
		return nil
	}
	return src.RawBytes(bodyStart, dataLen)
}

// --- Terrain sculpting ---

func doSculpt(src *esf.ObjFile, zoneInfo *esf.ObjInfo, sculpt string, zoneIdx int, outPath string) {
	parts := strings.Split(sculpt, ",")
	if len(parts) < 5 {
		log.Fatal("sculpt format: mode,x,z,radius,height (mode=raise|lower|flatten)")
	}

	mode := parts[0]
	cx, _ := strconv.ParseFloat(parts[1], 32)
	cz, _ := strconv.ParseFloat(parts[2], 32)
	radius, _ := strconv.ParseFloat(parts[3], 32)
	height, _ := strconv.ParseFloat(parts[4], 32)

	log.Printf("Sculpting: %s at (%.0f, %.0f) radius=%.0f height=%.0f", mode, cx, cz, radius, height)

	// Extract zone as standalone ESF
	data, err := esf.ExtractZoneESF(src, zoneIdx)
	if err != nil {
		log.Fatal(err)
	}

	// Parse the extracted ESF to find PrimBuffers
	zf, err := esf.OpenBytes(data)
	if err != nil {
		log.Fatal(err)
	}
	zroot, err := zf.Root()
	if err != nil {
		log.Fatal(err)
	}

	// Find zone and its preTranslations
	wInfo := zroot.Child(esf.TypeWorld)
	if wInfo == nil {
		log.Fatal("no World in extracted zone")
	}
	zInfos := wInfo.ChildrenOfType(esf.TypeZone)
	if len(zInfos) == 0 {
		log.Fatal("no Zone in extracted zone")
	}
	zInfo := zInfos[0]

	// Get preTranslations
	zoneObj, err := zf.GetObject(zInfo)
	if err != nil {
		log.Fatal(err)
	}
	zone := zoneObj.(*esf.Zone)
	zbase, err := zone.GetZoneBase(zf)
	if err != nil {
		log.Fatal(err)
	}
	var pretrans []esf.Point
	if zbase != nil {
		pretrans = zbase.PreTranslations
	}

	// Find all PrimBuffers
	var primBuffers []*esf.ObjInfo
	collectType(zInfo, esf.TypePrimBuffer, &primBuffers)
	collectType(zInfo, esf.TypeSkinPrimBuffer, &primBuffers)

	modified := 0
	for _, pb := range primBuffers {
		n := sculptPB(data, pb, mode, float32(cx), float32(cz), float32(radius), float32(height), pretrans)
		if n > 0 {
			modified++
		}
	}

	log.Printf("Modified %d/%d PrimBuffers", modified, len(primBuffers))

	if err := os.WriteFile(outPath, data, 0644); err != nil {
		log.Fatal(err)
	}
	log.Printf("Wrote %s (%d bytes)", outPath, len(data))
}

func sculptPB(data []byte, pb *esf.ObjInfo, mode string, cx, cz, radius, height float32, pretrans []esf.Point) int {
	pos := pb.Offset
	ver := pb.Version

	if ver == 0 {
		return sculptPBV0(data, pb, mode, cx, cz, radius, height)
	}

	if ver > 1 {
		pos += 4
	}
	if pos+28 > len(data) {
		return 0
	}

	pbtype := int(binary.LittleEndian.Uint32(data[pos:]))
	pos += 4
	pos += 4 // nmats
	nfaces := int(binary.LittleEndian.Uint32(data[pos:]))
	pos += 4
	pos += 4 // unknown
	p1 := int(binary.LittleEndian.Uint32(data[pos:]))
	pos += 4
	pos += 4 // p2
	pos += 4 // p3

	packing := float32(1.0 / math.Pow(2, float64(p1)))
	packingInv := float32(math.Pow(2, float64(p1)))

	stride := 17
	if pbtype == 4 {
		stride = 19
	} else if pbtype == 5 {
		stride = 21
	}

	// Find parent SimpleSubSprite to check if it uses preTranslations
	usesPretrans := pb.Parent != nil && pb.Parent.Type == esf.TypeSimpleSubSprite

	count := 0
	for fi := 0; fi < nfaces; fi++ {
		if pos+8 > len(data) {
			break
		}
		nverts := int(binary.LittleEndian.Uint32(data[pos:]))
		pos += 4
		pos += 4 // material

		for vi := 0; vi < nverts; vi++ {
			if pos+stride > len(data) {
				break
			}

			x := int16(binary.LittleEndian.Uint16(data[pos:]))
			y := int16(binary.LittleEndian.Uint16(data[pos+2:]))
			z := int16(binary.LittleEndian.Uint16(data[pos+4:]))

			// Convert to world coordinates
			wx := float32(x) * packing
			wy := float32(y) * packing
			wz := float32(z) * packing

			// Apply preTranslation if available
			if usesPretrans && pbtype == 4 && pos+18 < len(data) {
				vgroup := int16(binary.LittleEndian.Uint16(data[pos+stride-2:]))
				if int(vgroup) < len(pretrans) {
					wx += pretrans[vgroup].X
					wy += pretrans[vgroup].Y
					wz += pretrans[vgroup].Z
				}
			}

			// Check if vertex is in sculpt radius
			dx := wx - cx
			dz := wz - cz
			dist := float32(math.Sqrt(float64(dx*dx + dz*dz)))

			if dist < radius {
				// Smooth falloff
				falloff := 1.0 - dist/radius
				falloff = falloff * falloff // quadratic falloff

				var newWY float32
				switch mode {
				case "raise":
					newWY = wy + height*falloff
				case "lower":
					newWY = wy - height*falloff
				case "flatten":
					newWY = wy + (height-wy)*falloff
				default:
					pos += stride
					continue
				}

				// Convert back to local coords (subtract pretrans)
				newLocalY := newWY
				if usesPretrans && pbtype == 4 && pos+18 < len(data) {
					vgroup := int16(binary.LittleEndian.Uint16(data[pos+stride-2:]))
					if int(vgroup) < len(pretrans) {
						newLocalY -= pretrans[vgroup].Y
					}
				}

				// Pack back to int16
				newPacked := int16(math.Round(float64(newLocalY * packingInv)))
				binary.LittleEndian.PutUint16(data[pos+2:], uint16(newPacked))
				count++
			}

			pos += stride
		}
	}
	return count
}

func sculptPBV0(data []byte, pb *esf.ObjInfo, mode string, cx, cz, radius, height float32) int {
	pos := pb.Offset
	if pos+12 > len(data) {
		return 0
	}
	nfaces := int(binary.LittleEndian.Uint32(data[pos+8:]))
	pos += 12

	count := 0
	for fi := 0; fi < nfaces; fi++ {
		if pos+8 > len(data) {
			break
		}
		nverts := int(binary.LittleEndian.Uint32(data[pos:]))
		pos += 8

		for vi := 0; vi < nverts; vi++ {
			if pos+36 > len(data) {
				break
			}

			xBits := binary.LittleEndian.Uint32(data[pos:])
			yBits := binary.LittleEndian.Uint32(data[pos+4:])
			zBits := binary.LittleEndian.Uint32(data[pos+8:])
			wx := math.Float32frombits(xBits)
			wy := math.Float32frombits(yBits)
			wz := math.Float32frombits(zBits)

			dx := wx - cx
			dz := wz - cz
			dist := float32(math.Sqrt(float64(dx*dx + dz*dz)))

			if dist < radius {
				falloff := 1.0 - dist/radius
				falloff = falloff * falloff

				var newY float32
				switch mode {
				case "raise":
					newY = wy + height*falloff
				case "lower":
					newY = wy - height*falloff
				case "flatten":
					newY = wy + (height-wy)*falloff
				default:
					pos += 36
					continue
				}

				binary.LittleEndian.PutUint32(data[pos+4:], math.Float32bits(newY))
				count++
			}

			pos += 36
		}
	}
	return count
}

// doAdd adds a new SimpleSprite resource + ZoneActor to the zone.
// Rebuilds ZoneResources with the new child and patches the ResourceTable.
func doAdd(src *esf.ObjFile, root, zoneInfo *esf.ObjInfo, pb *esf.PrimBuffer, pos [3]float32, scale float32, name, outPath string) {
	dictID := esf.HashResourceID(name)
	log.Printf("Adding SimpleSprite '%s' (dict=0x%08x) at (%.0f, %.0f, %.0f) scale=%.1f",
		name, uint32(dictID), pos[0], pos[1], pos[2], scale)

	worldInfo := root.Child(esf.TypeWorld)
	worldBase := root.Child(esf.TypeWorldBase)
	allZones := worldInfo.ChildrenOfType(esf.TypeZone)

	w := esf.NewWriter()

	// Write World with ALL zones — only the target zone is rebuilt
	worldH := w.WriteNodeBegin(worldInfo.Type, worldInfo.Version, worldInfo.NumSubObjects)
	bodyData := nodeBodyData(src, worldInfo)
	w.WriteBytes(bodyData)

	for _, z := range allZones {
		if z == zoneInfo {
			writeZoneWithAddition(w, src, z, pb, dictID, pos, scale)
		} else {
			w.WriteNodeRaw(z, src)
		}
	}

	w.WriteNodeEnd(worldH)

	if worldBase != nil {
		w.WriteNodeRaw(worldBase, src)
	}

	data := w.Finalize()
	if err := os.WriteFile(outPath, data, 0644); err != nil {
		log.Fatal(err)
	}
	log.Printf("Wrote %s (%d bytes, %.1f MB)", outPath, len(data), float64(len(data))/(1024*1024))
}

// writeZoneWithAddition rebuilds the zone, adding a new SimpleSprite to
// ZoneResources and a new ZoneActor placement. All ancestor bodySize and
// numSubObjects values are recalculated via WriteNodeBegin/WriteNodeEnd.
// The ResourceTable is rebuilt with corrected file offsets.
func writeZoneWithAddition(w *esf.ESFWriter, src *esf.ObjFile, zoneInfo *esf.ObjInfo, newPB *esf.PrimBuffer, dictID int32, pos [3]float32, scale float32) {
	// Zone node: rebuild with same children + modifications
	zoneH := w.WriteNodeBegin(zoneInfo.Type, zoneInfo.Version, zoneInfo.NumSubObjects)
	bodyData := nodeBodyData(src, zoneInfo)
	w.WriteBytes(bodyData)

	for _, child := range zoneInfo.Children {
		switch child.Type {
		case esf.TypeZoneResources:
			writeZoneResourcesWithAddition(w, src, child, newPB, dictID)
		case esf.TypeZoneBase:
			writeZoneBaseWithResourceTable(w, src, child, dictID, newPB)
		default:
			// ZoneActors, ZoneStaticLightings, etc — copy verbatim
			w.WriteNodeRaw(child, src)
		}
	}

	w.WriteNodeEnd(zoneH)
}

// writeZoneResourcesWithAddition rebuilds ZoneResources (0x3100) with one
// additional child: the new SimpleSprite.
func writeZoneResourcesWithAddition(w *esf.ESFWriter, src *esf.ObjFile, resources *esf.ObjInfo, newPB *esf.PrimBuffer, dictID int32) {
	// numSubObjects = original count + 1 (the new SimpleSprite)
	newCount := resources.NumSubObjects + 1
	h := w.WriteNodeBegin(resources.Type, resources.Version, newCount)

	// Write this node's own body data (if any — ZoneResources usually has none)
	bodyData := nodeBodyData(src, resources)
	w.WriteBytes(bodyData)

	// Copy all existing children verbatim
	for _, child := range resources.Children {
		w.WriteNodeRaw(child, src)
	}

	// Append new SimpleSprite
	writeNewSimpleSprite(w, newPB, dictID)
	log.Printf("  Added SimpleSprite to ZoneResources (now %d children)", newCount)

	w.WriteNodeEnd(h)
}

// writeNewSimpleSprite writes a minimal SimpleSprite node with header + PrimBuffer.
func writeNewSimpleSprite(w *esf.ESFWriter, pb *esf.PrimBuffer, dictID int32) {
	// SimpleSprite has 2 children: SimpleSpriteHeader + PrimBuffer
	sprH := w.WriteNodeBegin(esf.TypeSimpleSprite, 0, 2)

	// SimpleSpriteHeader
	w.WriteSimpleSpriteHeader(dictID, pb.BBox)

	// PrimBuffer
	w.WritePrimBuffer(pb, dictID)

	w.WriteNodeEnd(sprH)
}

// writeZoneBaseWithResourceTable rebuilds ZoneBase, patching the ResourceTable
// to include the new sprite. The ResourceTable uses a binary-search lookup by
// DictID, so entries must be sorted by DictID.
func writeZoneBaseWithResourceTable(w *esf.ESFWriter, src *esf.ObjFile, zoneBase *esf.ObjInfo, newDictID int32, newPB *esf.PrimBuffer) {
	// For now, copy the ZoneBase verbatim. The ResourceTable offsets will be
	// wrong (they point into the original TUNARIA.ESF), but the zone redirect
	// pipeline doesn't use ResourceTable for sequential parsing — the PS2
	// parser reads ZoneResources children sequentially via ReadBegin/ReadEnd.
	//
	// ResourceTable is only used by VILoader::Find for on-demand resource
	// lookup (e.g., when an NPC model needs a specific texture). For our
	// standalone zone ESF served via the redirect pipeline, all resources are
	// parsed sequentially during zone load, so ResourceTable isn't needed.
	//
	// TODO: If we ever need random-access resource lookup to work in the
	// standalone zone ESF, rebuild the ResourceTable here.
	w.WriteNodeRaw(zoneBase, src)
}

func parseVec3(s string) [3]float32 {
	parts := strings.Split(s, ",")
	var v [3]float32
	if len(parts) >= 1 {
		f, _ := strconv.ParseFloat(strings.TrimSpace(parts[0]), 32)
		v[0] = float32(f)
	}
	if len(parts) >= 2 {
		f, _ := strconv.ParseFloat(strings.TrimSpace(parts[1]), 32)
		v[1] = float32(f)
	}
	if len(parts) >= 3 {
		f, _ := strconv.ParseFloat(strings.TrimSpace(parts[2]), 32)
		v[2] = float32(f)
	}
	return v
}
