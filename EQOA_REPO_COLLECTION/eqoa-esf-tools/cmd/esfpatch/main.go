// esfpatch: Patch a specific zone in TUNARIA.ESF/ISO and export as a zone overlay.
//
// Instead of modifying the full TUNARIA.ESF, this extracts just the zone's byte
// range, applies the patch, and saves it as a small overlay file. The serve
// script reads overlays for patched zones and falls back to original for the rest.
//
// Usage:
//
//	esfpatch -zone 84 -red -o patches/ TUNARIA.ESF
//	esfpatch -zone 84 -red -o patches/ game.iso        (ISO support)
//	esfpatch -zone 84 -yscale 1.5 -o patches/ TUNARIA.ESF
//	esfpatch -zone 84 -swap -x 25245 -z 15699 -newid 0x3950ce16 -o patches/ game.iso
//	esfpatch -zone 84 -raise 50 -x 25247 -z 15695 -radius 200 -o patches/ game.iso
//	esfpatch -list TUNARIA.ESF   (show zone byte ranges)
package main

import (
	"encoding/binary"
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"path/filepath"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

var mode string
var yScaleVal float32
var colorR, colorG, colorB uint8
var raiseAmount float32
var raiseX, raiseZ float64
var raiseRadius float64

func main() {
	zoneIdx := flag.Int("zone", -1, "Zone index to patch")
	yScale := flag.Float64("yscale", 0, "Y scale factor")
	red := flag.Bool("red", false, "Tint vertex colors red")
	blue := flag.Bool("blue", false, "Tint vertex colors blue")
	green := flag.Bool("green", false, "Tint vertex colors green")
	swap := flag.Bool("swap", false, "Swap nearest actor's DictID (use with -x -z -newid)")
	raise := flag.Float64("raise", 0, "Raise terrain height by N units at -x -z within -radius")
	posX := flag.Float64("x", 0, "X position (use with -swap or -raise)")
	posZ := flag.Float64("z", 0, "Z position (use with -swap or -raise)")
	radius := flag.Float64("radius", 200, "Radius for -raise (world units)")
	newDictID := flag.String("newid", "", "New DictID hex (e.g. 0x3950ce16, use with -swap)")
	oldDictID := flag.String("oldid", "", "Filter swap to only match this DictID (hex)")
	outDir := flag.String("o", ".", "Output directory for patch files")
	listZones := flag.Bool("list", false, "List all zone byte ranges and exit")
	flag.Parse()

	if flag.NArg() < 1 {
		fmt.Fprintf(os.Stderr, "Usage: esfpatch -zone N [-yscale F] [-red|-blue|-green] -o DIR input\n")
		fmt.Fprintf(os.Stderr, "       esfpatch -zone N -swap -x X -z Z -newid 0xID -o DIR input\n")
		fmt.Fprintf(os.Stderr, "       esfpatch -zone N -raise H -x X -z Z [-radius R] -o DIR input\n")
		fmt.Fprintf(os.Stderr, "       esfpatch -list input\n")
		fmt.Fprintf(os.Stderr, "\nInput can be TUNARIA.ESF or an EQOA .iso file.\n")
		os.Exit(1)
	}

	path := flag.Arg(0)
	log.Printf("Opening %s...", path)

	f, err := esf.Open(path)
	if err != nil {
		log.Fatal(err)
	}

	root, err := f.Root()
	if err != nil {
		log.Fatal(err)
	}

	worldInfo := root.Child(esf.TypeWorld)
	if worldInfo == nil {
		log.Fatal("no World object found")
	}

	zones := worldInfo.ChildrenOfType(esf.TypeZone)

	if *listZones {
		for i, z := range zones {
			headerOff := z.Offset - 12
			totalSize := z.Size + 12
			if f.ISOBase > 0 {
				isoOff := f.ISOBase + int64(headerOff)
				fmt.Printf("Zone %3d: esf_offset=0x%09x  iso_offset=0x%09x  size=%9d (%5.1f MB)\n",
					i, headerOff, isoOff, totalSize, float64(totalSize)/(1024*1024))
			} else {
				fmt.Printf("Zone %3d: file_offset=0x%09x  size=%9d (%5.1f MB)\n",
					i, headerOff, totalSize, float64(totalSize)/(1024*1024))
			}
		}
		fmt.Printf("\nTotal: %d zones\n", len(zones))
		return
	}

	if *zoneIdx < 0 {
		fmt.Fprintf(os.Stderr, "Specify -zone N or -list\n")
		os.Exit(1)
	}

	if *zoneIdx >= len(zones) {
		log.Fatalf("zone %d out of range (max %d)", *zoneIdx, len(zones)-1)
	}

	// Determine patch mode
	if *swap {
		mode = "swap"
	} else if *raise != 0 {
		mode = "raise"
		raiseAmount = float32(*raise)
		raiseX = *posX
		raiseZ = *posZ
		raiseRadius = *radius
	} else if *yScale != 0 {
		mode = "yscale"
		yScaleVal = float32(*yScale)
	} else if *red {
		mode = "color"
		colorR, colorG, colorB = 255, 40, 40
	} else if *blue {
		mode = "color"
		colorR, colorG, colorB = 40, 40, 255
	} else if *green {
		mode = "color"
		colorR, colorG, colorB = 40, 255, 40
	}
	if mode == "" {
		fmt.Fprintf(os.Stderr, "Specify -yscale, -red, -blue, -green, -swap, or -raise\n")
		os.Exit(1)
	}

	zoneInfo := zones[*zoneIdx]
	zoneFileOffset := int64(zoneInfo.Offset) - 12
	zoneTotalSize := int(zoneInfo.Size) + 12

	log.Printf("Zone %d: esf_offset=0x%x total_size=%d (%.1f MB)",
		*zoneIdx, zoneFileOffset, zoneTotalSize, float64(zoneTotalSize)/(1024*1024))

	// Extract zone data from the parsed file's raw bytes
	rawData := f.Data()
	zoneEnd := int(zoneFileOffset) + zoneTotalSize
	if zoneEnd > len(rawData) {
		log.Fatalf("zone extends beyond data: end=0x%x, data_size=0x%x", zoneEnd, len(rawData))
	}
	zoneData := make([]byte, zoneTotalSize)
	copy(zoneData, rawData[zoneFileOffset:zoneEnd])

	if mode == "raise" {
		// Island mode: create a sloping hill from existing terrain.
		// 1. Hide water (alpha=0) in expanded radius
		// 2. Set terrain to absolute hill height (dome profile from shore level)
		// 3. Set collision to match
		zoneObj, err := f.GetObject(zoneInfo)
		if err != nil {
			log.Fatal(err)
		}
		zone := zoneObj.(*esf.Zone)
		zoneBase, err := zone.GetZoneBase(f)
		if err != nil {
			log.Fatal(err)
		}
		var preTrans []esf.Point
		if zoneBase != nil {
			preTrans = zoneBase.GetPreTranslations()
		}
		log.Printf("Loaded %d pretranslations", len(preTrans))
		for i, pt := range preTrans {
			log.Printf("  preTrans[%d] = (%.1f, %.1f, %.1f)", i, pt.X, pt.Y, pt.Z)
		}

		radiusSq := raiseRadius * raiseRadius

		// Collect all pbtype 4 PrimBuffers, split into terrain vs water.
		var allPBs []*esf.ObjInfo
		collectType(zoneInfo, esf.TypePrimBuffer, &allPBs)
		var terrainPBs, waterPBs []*esf.ObjInfo
		for _, pb := range allPBs {
			if getPBType(zoneData, pb, zoneFileOffset) != 4 {
				continue
			}
			if isTranslucentPB(zoneData, pb, zoneFileOffset) {
				waterPBs = append(waterPBs, pb)
			} else {
				terrainPBs = append(terrainPBs, pb)
			}
		}
		log.Printf("Found %d terrain, %d water PrimBuffers (of %d total)",
			len(terrainPBs), len(waterPBs), len(allPBs))

		// Raise terrain only — don't touch water for now.
		// Raise terrain with additive dome from existing height.
		// Uses smoothstep: full raise at center, 0 at edge.
		raisedVerts := 0
		raisedPBs := 0
		for _, pb := range terrainPBs {
			localOffset := int(int64(pb.Offset) - zoneFileOffset)
			if localOffset < 0 || localOffset+int(pb.Size) > len(zoneData) {
				continue
			}
			pbData := zoneData[localOffset : localOffset+int(pb.Size)]
			n := raisePBVertices(pbData, pb, preTrans, raiseX, raiseZ, radiusSq, raiseAmount)
			if n > 0 {
				raisedVerts += n
				raisedPBs++
			}
		}
		log.Printf("Raised %d terrain vertices (additive dome, peak=%.0f) in %d PBs",
			raisedVerts, raiseAmount, raisedPBs)

		// Step 3: Raise collision with same additive dome.
		var allCBs []*esf.ObjInfo
		collectType(zoneInfo, esf.TypeCollBuffer, &allCBs)
		var terrainCBs []*esf.ObjInfo
		for _, cb := range allCBs {
			cbt := getCBType(zoneData, cb, zoneFileOffset)
			if cbt == 2 || cbt == 3 {
				terrainCBs = append(terrainCBs, cb)
			}
		}
		log.Printf("Found %d/%d terrain CollBuffers", len(terrainCBs), len(allCBs))

		collVerts := 0
		collCBs := 0
		for _, cb := range terrainCBs {
			localOffset := int(int64(cb.Offset) - zoneFileOffset)
			if localOffset < 0 || localOffset+int(cb.Size) > len(zoneData) {
				continue
			}
			cbData := zoneData[localOffset : localOffset+int(cb.Size)]
			n := raiseCBVertices(cbData, cb, preTrans, raiseX, raiseZ, radiusSq, raiseAmount)
			if n > 0 {
				collVerts += n
				collCBs++
			}
		}
		log.Printf("Raised %d collision vertices in %d CBs", collVerts, collCBs)
		log.Printf("Total: radius=%.0f around (%.0f, %.0f), peak=%.0f",
			raiseRadius, raiseX, raiseZ, raiseAmount)

		// Debug scan: report ALL PrimBuffers (including skipped ones) with vertices in area
		log.Printf("--- DEBUG: Scanning ALL %d PrimBuffers for vertices in radius ---", len(allPBs))
		for _, pb := range allPBs {
			localOffset := int(int64(pb.Offset) - zoneFileOffset)
			if localOffset < 0 || localOffset+int(pb.Size) > len(zoneData) {
				continue
			}
			pbData := zoneData[localOffset : localOffset+int(pb.Size)]
			pbt := getPBType(zoneData, pb, zoneFileOffset)
			translucent := isTranslucentPB(zoneData, pb, zoneFileOffset)

			// Count vertices in area (read-only scan)
			count := countVertsInArea(pbData, pb, preTrans, raiseX, raiseZ, radiusSq)
			if count > 0 {
				alpha := getFirstAlpha(zoneData, pb, zoneFileOffset)
				mats := getMaterialIndices(zoneData, pb, zoneFileOffset)
				log.Printf("  PB offset=0x%x pbtype=%d alpha=%d translucent=%v verts=%d materials=%v",
					pb.Offset, pbt, alpha, translucent, count, mats)
			}
		}
	} else if mode == "swap" {
		// Actor swap mode
		if *newDictID == "" {
			log.Fatal("-swap requires -newid")
		}
		var newID uint32
		if _, err := fmt.Sscanf(*newDictID, "0x%x", &newID); err != nil {
			if _, err := fmt.Sscanf(*newDictID, "%x", &newID); err != nil {
				log.Fatalf("Invalid DictID %q: %v", *newDictID, err)
			}
		}

		// Load actors
		zoneObj, err := f.GetObject(zoneInfo)
		if err != nil {
			log.Fatal(err)
		}
		zone := zoneObj.(*esf.Zone)
		actors, err := zone.GetZoneActors(f)
		if err != nil {
			log.Fatal(err)
		}
		if actors == nil || len(actors.GetActors()) == 0 {
			log.Fatal("No actors in zone")
		}

		// Parse optional oldid filter
		var filterID uint32
		hasFilter := *oldDictID != ""
		if hasFilter {
			if _, err := fmt.Sscanf(*oldDictID, "0x%x", &filterID); err != nil {
				if _, err := fmt.Sscanf(*oldDictID, "%x", &filterID); err != nil {
					log.Fatalf("Invalid oldid %q: %v", *oldDictID, err)
				}
			}
		}

		// Find nearest actor (optionally filtered by DictID)
		var nearest *esf.ZoneActor
		bestDist := math.MaxFloat64
		for _, actor := range actors.GetActors() {
			p := actor.Placement
			if hasFilter && uint32(p.SpriteID) != filterID {
				continue
			}
			dx := float64(p.Pos.X) - *posX
			dz := float64(p.Pos.Z) - *posZ
			dist := math.Sqrt(dx*dx + dz*dz)
			if dist < bestDist {
				bestDist = dist
				nearest = actor
			}
		}
		if nearest == nil {
			log.Fatal("No actors found")
		}

		p := nearest.Placement
		log.Printf("Nearest actor: DictID=0x%08x pos=(%.1f, %.1f, %.1f) dist=%.1f",
			uint32(p.SpriteID), p.Pos.X, p.Pos.Y, p.Pos.Z, bestDist)

		// Patch DictID in zone data
		actorInfo := nearest.ObjInfo()
		patchOffset := actorInfo.Offset - int(zoneFileOffset)

		oldVal := binary.LittleEndian.Uint32(zoneData[patchOffset : patchOffset+4])
		if oldVal != uint32(p.SpriteID) {
			log.Fatalf("DictID mismatch at offset 0x%x: got 0x%08x, expected 0x%08x",
				patchOffset, oldVal, uint32(p.SpriteID))
		}

		binary.LittleEndian.PutUint32(zoneData[patchOffset:patchOffset+4], newID)
		log.Printf("Swapped DictID 0x%08x → 0x%08x at zone offset 0x%x",
			uint32(p.SpriteID), newID, patchOffset)
	} else {
		// PrimBuffer patch mode (yscale / color)
		var primBuffers []*esf.ObjInfo
		collectType(zoneInfo, esf.TypePrimBuffer, &primBuffers)
		collectType(zoneInfo, esf.TypeSkinPrimBuffer, &primBuffers)
		log.Printf("Found %d PrimBuffers", len(primBuffers))

		patched := 0
		for _, pb := range primBuffers {
			localOffset := int(int64(pb.Offset) - zoneFileOffset)
			if localOffset < 0 || localOffset+int(pb.Size) > len(zoneData) {
				log.Printf("WARNING: PrimBuffer at 0x%x out of zone range", pb.Offset)
				continue
			}
			pbCopy := make([]byte, int(pb.Size))
			copy(pbCopy, zoneData[localOffset:localOffset+int(pb.Size)])
			if err := patchPB(pbCopy, 0, pb); err != nil {
				log.Printf("WARNING: skip PrimBuffer at 0x%x: %v", pb.Offset, err)
				continue
			}
			copy(zoneData[localOffset:localOffset+int(pb.Size)], pbCopy)
			patched++
		}
		log.Printf("Patched %d/%d PrimBuffers (mode=%s)", patched, len(primBuffers), mode)
	}

	// Sector-align the patch so every disc read falls fully within.
	// The zone may not start/end on a 2048-byte sector boundary.
	const sectorSize = 2048
	isoByteOffset := zoneFileOffset
	if f.ISOBase > 0 {
		isoByteOffset = f.ISOBase + zoneFileOffset
	}

	alignedStart := (isoByteOffset / sectorSize) * sectorSize
	alignedEnd := ((isoByteOffset + int64(zoneTotalSize)) + sectorSize - 1) / sectorSize * sectorSize
	startPad := int(isoByteOffset - alignedStart)
	endPad := int(alignedEnd - (isoByteOffset + int64(zoneTotalSize)))

	if startPad > 0 || endPad > 0 {
		log.Printf("Sector-aligning: pad start=%d end=%d bytes", startPad, endPad)
		alignedData := make([]byte, int(alignedEnd-alignedStart))
		// Fill padding from original file data
		if startPad > 0 {
			srcOff := int(zoneFileOffset) - startPad
			if srcOff >= 0 && srcOff+startPad <= len(rawData) {
				copy(alignedData[:startPad], rawData[srcOff:srcOff+startPad])
			}
		}
		copy(alignedData[startPad:startPad+len(zoneData)], zoneData)
		if endPad > 0 {
			srcOff := int(zoneFileOffset) + zoneTotalSize
			if srcOff+endPad <= len(rawData) {
				copy(alignedData[startPad+len(zoneData):], rawData[srcOff:srcOff+endPad])
			}
		}
		zoneData = alignedData
		isoByteOffset = alignedStart
	}
	tunByteOffset := isoByteOffset - f.ISOBase

	// Write patch files
	os.MkdirAll(*outDir, 0755)

	patchPath := filepath.Join(*outDir, fmt.Sprintf("zone_%d.bin", *zoneIdx))
	if err := os.WriteFile(patchPath, zoneData, 0644); err != nil {
		log.Fatal(err)
	}
	log.Printf("Wrote %s (%d bytes, sector-aligned)", patchPath, len(zoneData))

	meta := map[string]interface{}{
		"zone":            *zoneIdx,
		"iso_byte_offset": isoByteOffset,
		"tun_byte_offset": tunByteOffset,
		"size":            len(zoneData),
	}
	metaPath := filepath.Join(*outDir, fmt.Sprintf("zone_%d.json", *zoneIdx))
	metaJSON, _ := json.MarshalIndent(meta, "", "  ")
	if err := os.WriteFile(metaPath, metaJSON, 0644); err != nil {
		log.Fatal(err)
	}
	log.Printf("Wrote %s", metaPath)
}

// countVertsInArea counts how many vertices in a PrimBuffer fall within radius (read-only).
func countVertsInArea(pbData []byte, pb *esf.ObjInfo, preTrans []esf.Point, targetX, targetZ, radiusSq float64) int {
	ver := pb.Version
	if ver == 0 {
		// v0: float32 coords
		pos := 0
		if pos+12 > len(pbData) {
			return 0
		}
		nfaces := int32(binary.LittleEndian.Uint32(pbData[pos+8:]))
		pos += 12
		count := 0
		for fi := int32(0); fi < nfaces; fi++ {
			if pos+8 > len(pbData) {
				return count
			}
			nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
			pos += 8
			for vi := int32(0); vi < nverts; vi++ {
				if pos+36 > len(pbData) {
					return count
				}
				worldX := float64(math.Float32frombits(binary.LittleEndian.Uint32(pbData[pos:])))
				worldZ := float64(math.Float32frombits(binary.LittleEndian.Uint32(pbData[pos+8:])))
				dx := worldX - targetX
				dz := worldZ - targetZ
				if dx*dx+dz*dz <= radiusSq {
					count++
				}
				pos += 36
			}
		}
		return count
	}

	pos := 0
	if ver > 1 {
		pos += 4
	}
	if pos+28 > len(pbData) {
		return 0
	}
	pbtype := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // unknown
	p1 := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 12 // p1+p2+p3

	packing1 := float32(1.0 / math.Pow(2, float64(p1)))

	count := 0
	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return count
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 8

		var stride int
		switch pbtype {
		case 2:
			stride = 17
		case 4:
			stride = 19
		case 5:
			stride = 21
		default:
			return count
		}

		for vi := int32(0); vi < nverts; vi++ {
			if pos+stride > len(pbData) {
				return count
			}
			x := int16(binary.LittleEndian.Uint16(pbData[pos:]))
			z := int16(binary.LittleEndian.Uint16(pbData[pos+4:]))
			worldX := float64(float32(x) * packing1)
			worldZ := float64(float32(z) * packing1)
			if pbtype == 4 && len(preTrans) > 0 {
				vgroup := int16(binary.LittleEndian.Uint16(pbData[pos+17:]))
				if int(vgroup) < len(preTrans) {
					worldX += float64(preTrans[vgroup].X)
					worldZ += float64(preTrans[vgroup].Z)
				}
			}
			dx := worldX - targetX
			dz := worldZ - targetZ
			if dx*dx+dz*dz <= radiusSq {
				count++
			}
			pos += stride
		}
	}
	return count
}

// getFirstAlpha returns the alpha byte of the first vertex in a PrimBuffer, or -1.
func getFirstAlpha(zoneData []byte, pb *esf.ObjInfo, zoneFileOffset int64) int {
	if pb.Version == 0 {
		localOff := int(int64(pb.Offset) - zoneFileOffset)
		if localOff < 0 || localOff+48 > len(zoneData) {
			return -1
		}
		// v0: 12 byte header, then first vertex at offset 12, alpha at vertex+35
		return int(zoneData[localOff+12+35])
	}
	localOff := int(int64(pb.Offset) - zoneFileOffset)
	if localOff < 0 || localOff+int(pb.Size) > len(zoneData) {
		return -1
	}
	pos := localOff
	if pb.Version > 1 {
		pos += 4
	}
	if pos+36 > localOff+int(pb.Size) {
		return -1
	}
	pos += 28 // skip header
	// First face: nverts + mat
	if pos+8 > localOff+int(pb.Size) {
		return -1
	}
	pos += 8
	// First vertex alpha at offset 16
	if pos+17 > localOff+int(pb.Size) {
		return -1
	}
	return int(zoneData[pos+16])
}

// parentChainStr builds a readable parent type chain for an ObjInfo.
func parentChainStr(info *esf.ObjInfo) string {
	var parts []string
	for p := info.Parent; p != nil; p = p.Parent {
		parts = append(parts, fmt.Sprintf("0x%04X", p.Type))
	}
	if len(parts) == 0 {
		return "none"
	}
	return fmt.Sprintf("%s", parts)
}

// getMaterialIndices returns unique material indices used by a PrimBuffer's faces.
func getMaterialIndices(zoneData []byte, pb *esf.ObjInfo, zoneFileOffset int64) []int {
	localOff := int(int64(pb.Offset) - zoneFileOffset)
	if localOff < 0 || localOff+int(pb.Size) > len(zoneData) {
		return nil
	}
	if pb.Version == 0 {
		return []int{0} // v0 doesn't have per-face materials the same way
	}
	pos := localOff
	if pb.Version > 1 {
		pos += 4
	}
	if pos+28 > localOff+int(pb.Size) {
		return nil
	}
	pbtype := int32(binary.LittleEndian.Uint32(zoneData[pos:]))
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(zoneData[pos:]))
	pos += 4
	pos += 16 // unknown+p1+p2+p3

	var stride int
	switch pbtype {
	case 2:
		stride = 17
	case 4:
		stride = 19
	case 5:
		stride = 21
	default:
		return nil
	}

	seen := map[int]bool{}
	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > localOff+int(pb.Size) {
			break
		}
		nverts := int32(binary.LittleEndian.Uint32(zoneData[pos:]))
		mat := int(binary.LittleEndian.Uint32(zoneData[pos+4:]))
		pos += 8
		seen[mat] = true
		pos += int(nverts) * stride
	}
	var mats []int
	for m := range seen {
		mats = append(mats, m)
	}
	return mats
}

// isTranslucentPB checks if a PrimBuffer's first vertex has alpha < 255.
// Water meshes use translucent alpha; terrain is opaque.
func isTranslucentPB(zoneData []byte, pb *esf.ObjInfo, zoneFileOffset int64) bool {
	if pb.Version == 0 {
		return false // v0 uses different format, check separately
	}
	localOffset := int(int64(pb.Offset) - zoneFileOffset)
	if localOffset < 0 || localOffset+int(pb.Size) > len(zoneData) {
		return false
	}
	pos := localOffset
	if pb.Version > 1 {
		pos += 4 // dictID
	}
	if pos+28 > localOffset+int(pb.Size) {
		return false
	}
	pbtype := int32(binary.LittleEndian.Uint32(zoneData[pos:]))
	pos += 28 // skip header (pbtype+nmats+nfaces+unknown+p1+p2+p3)
	// Read first face: nverts + mat
	if pos+8 > localOffset+int(pb.Size) {
		return false
	}
	nverts := int32(binary.LittleEndian.Uint32(zoneData[pos:]))
	pos += 8 // nverts + mat
	if nverts == 0 {
		return false
	}
	// First vertex alpha is at offset 16 within vertex (color byte 3)
	// Vertex: x(2)+y(2)+z(2)+u(2)+v(2)+normal(3)+color(4) [+vgroup(2)]
	// Alpha = pos + 16
	var stride int
	switch pbtype {
	case 2:
		stride = 17
	case 4:
		stride = 19
	default:
		return false
	}
	if pos+stride > localOffset+int(pb.Size) {
		return false
	}
	alpha := zoneData[pos+16]
	return alpha < 255
}

// getPBType reads the pbtype field from a PrimBuffer's raw data.
// Returns -1 if unable to read.
func getPBType(zoneData []byte, pb *esf.ObjInfo, zoneFileOffset int64) int32 {
	if pb.Version == 0 {
		return 0 // v0 is a special format
	}
	localOffset := int(int64(pb.Offset) - zoneFileOffset)
	if localOffset < 0 || localOffset+int(pb.Size) > len(zoneData) {
		return -1
	}
	pos := localOffset
	if pb.Version > 1 {
		pos += 4 // skip dictID
	}
	if pos+4 > localOffset+int(pb.Size) {
		return -1
	}
	return int32(binary.LittleEndian.Uint32(zoneData[pos:]))
}

// getCBType reads the cbtype field from a CollBuffer's raw data.
// Returns -1 if unable to read.
func getCBType(zoneData []byte, cb *esf.ObjInfo, zoneFileOffset int64) int32 {
	if cb.Version <= 1 {
		return 0 // ver <= 1 has no cbtype field
	}
	localOffset := int(int64(cb.Offset) - zoneFileOffset)
	if localOffset < 0 || localOffset+4 > len(zoneData) {
		return -1
	}
	return int32(binary.LittleEndian.Uint32(zoneData[localOffset:]))
}

func collectType(info *esf.ObjInfo, typ uint16, out *[]*esf.ObjInfo) {
	if info.Type == typ {
		*out = append(*out, info)
	}
	for _, c := range info.Children {
		collectType(c, typ, out)
	}
}

func patchPB(data []byte, localOffset int, pb *esf.ObjInfo) error {
	// Work on the slice within data at localOffset
	pbData := data[localOffset : localOffset+int(pb.Size)]

	pos := 0
	ver := pb.Version

	if ver == 0 {
		return patchV0(pbData)
	}

	if ver > 1 {
		pos += 4
	}

	if pos+28 > len(pbData) {
		return fmt.Errorf("truncated header")
	}

	pbtype := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // unknown
	pos += 4 // p1
	pos += 4 // p2
	pos += 4 // p3

	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return fmt.Errorf("truncated face header")
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 4
		pos += 4 // material

		for vi := int32(0); vi < nverts; vi++ {
			var stride, colorOff int
			switch pbtype {
			case 2:
				stride = 17
				colorOff = 13
			case 4, 5:
				stride = 19
				colorOff = 13
			default:
				return fmt.Errorf("unknown pbtype %d", pbtype)
			}

			if pos+stride > len(pbData) {
				return fmt.Errorf("truncated vertex")
			}

			switch mode {
			case "yscale":
				y := int16(binary.LittleEndian.Uint16(pbData[pos+2:]))
				yf := float32(y) * yScaleVal
				if yf > 32767 {
					yf = 32767
				} else if yf < -32768 {
					yf = -32768
				}
				binary.LittleEndian.PutUint16(pbData[pos+2:], uint16(int16(yf)))
			case "color":
				pbData[pos+colorOff+0] = colorR
				pbData[pos+colorOff+1] = colorG
				pbData[pos+colorOff+2] = colorB
			}

			pos += stride
		}
	}
	return nil
}

// sinkWaterVertices sets water vertex Y to packed -32768 (minimum) within radius,
// pushing water far underground so it doesn't render on top of raised terrain.
func sinkWaterVertices(pbData []byte, pb *esf.ObjInfo, preTrans []esf.Point, targetX, targetZ, radiusSq float64) int {
	ver := pb.Version
	if ver == 0 {
		return 0
	}
	pos := 0
	if ver > 1 {
		pos += 4
	}
	if pos+28 > len(pbData) {
		return 0
	}
	pbtype := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	if pbtype != 4 {
		return 0
	}
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // unknown
	p1 := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 12

	packing1 := float32(1.0 / math.Pow(2, float64(p1)))
	count := 0
	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return count
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 8
		for vi := int32(0); vi < nverts; vi++ {
			if pos+19 > len(pbData) {
				return count
			}
			x := int16(binary.LittleEndian.Uint16(pbData[pos:]))
			z := int16(binary.LittleEndian.Uint16(pbData[pos+4:]))
			worldX := float64(float32(x) * packing1)
			worldZ := float64(float32(z) * packing1)
			vgroup := int16(binary.LittleEndian.Uint16(pbData[pos+17:]))
			if len(preTrans) > 0 && int(vgroup) < len(preTrans) {
				worldX += float64(preTrans[vgroup].X)
				worldZ += float64(preTrans[vgroup].Z)
			}
			dx := worldX - targetX
			dz := worldZ - targetZ
			if dx*dx+dz*dz <= radiusSq {
				// Set Y to packed minimum — pushes water far underground
				binary.LittleEndian.PutUint16(pbData[pos+2:], 0x8000) // int16 min = -32768
				count++
			}
			pos += 19
		}
	}
	return count
}

// convertWaterToTerrain makes water vertices transparent (alpha=0).
func convertWaterToTerrain(pbData []byte, pb *esf.ObjInfo, preTrans []esf.Point, targetX, targetZ, radiusSq float64, amount float32) int {
	ver := pb.Version
	if ver == 0 {
		return 0
	}
	pos := 0
	if ver > 1 {
		pos += 4
	}
	if pos+28 > len(pbData) {
		return 0
	}
	pbtype := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // unknown
	p1 := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 12 // p1+p2+p3

	packing1 := float32(1.0 / math.Pow(2, float64(p1)))

	var stride int
	switch pbtype {
	case 2:
		stride = 17
	case 4:
		stride = 19
	default:
		return 0
	}

	converted := 0
	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return converted
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 8

		for vi := int32(0); vi < nverts; vi++ {
			if pos+stride > len(pbData) {
				return converted
			}
			x := int16(binary.LittleEndian.Uint16(pbData[pos:]))
			z := int16(binary.LittleEndian.Uint16(pbData[pos+4:]))
			worldX := float64(float32(x) * packing1)
			worldZ := float64(float32(z) * packing1)
			if pbtype == 4 && len(preTrans) > 0 {
				vgroup := int16(binary.LittleEndian.Uint16(pbData[pos+17:]))
				if int(vgroup) < len(preTrans) {
					worldX += float64(preTrans[vgroup].X)
					worldZ += float64(preTrans[vgroup].Z)
				}
			}
			dx := worldX - targetX
			dz := worldZ - targetZ
			distSq := dx*dx + dz*dz
			if distSq <= radiusSq {
				// Make fully transparent (invisible) — don't move Y
				pbData[pos+16] = 0
				converted++
			}
			pos += stride
		}
	}
	return converted
}

// sinkAndHideWater pushes water vertices down AND sets their alpha to 0.
func sinkAndHideWater(pbData []byte, pb *esf.ObjInfo, preTrans []esf.Point, targetX, targetZ, radiusSq float64, sinkAmount float32) int {
	ver := pb.Version
	if ver == 0 {
		return 0
	}
	pos := 0
	if ver > 1 {
		pos += 4
	}
	if pos+28 > len(pbData) {
		return 0
	}
	pbtype := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // unknown
	p1 := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 12 // p1+p2+p3

	packing1 := float32(1.0 / math.Pow(2, float64(p1)))
	invPacking := float32(math.Pow(2, float64(p1)))

	var stride int
	switch pbtype {
	case 2:
		stride = 17
	case 4:
		stride = 19
	default:
		return 0
	}

	cleared := 0
	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return cleared
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 8

		for vi := int32(0); vi < nverts; vi++ {
			if pos+stride > len(pbData) {
				return cleared
			}
			x := int16(binary.LittleEndian.Uint16(pbData[pos:]))
			y := int16(binary.LittleEndian.Uint16(pbData[pos+2:]))
			z := int16(binary.LittleEndian.Uint16(pbData[pos+4:]))
			_ = z
			worldX := float64(float32(x) * packing1)
			worldZ := float64(float32(z) * packing1)
			if pbtype == 4 && len(preTrans) > 0 {
				vgroup := int16(binary.LittleEndian.Uint16(pbData[pos+17:]))
				if int(vgroup) < len(preTrans) {
					worldX += float64(preTrans[vgroup].X)
					worldZ += float64(preTrans[vgroup].Z)
				}
			}
			dx := worldX - targetX
			dz := worldZ - targetZ
			if dx*dx+dz*dz <= radiusSq {
				// Sink Y way down
				newY := float32(y) - sinkAmount*invPacking
				if newY < -32768 {
					newY = -32768
				}
				binary.LittleEndian.PutUint16(pbData[pos+2:], uint16(int16(newY)))
				// Set alpha to 0 (invisible) — alpha is at color offset + 3
				// color starts at offset 13 for both pbtype 2 and 4
				pbData[pos+16] = 0 // alpha byte
				cleared++
			}
			pos += stride
		}
	}
	return cleared
}

// flatRaisePBVertices raises/lowers Y for vertices within radius with NO gradient.
// Used for sinking water where we want uniform displacement.
func flatRaisePBVertices(pbData []byte, pb *esf.ObjInfo, preTrans []esf.Point, targetX, targetZ, radiusSq float64, amount float32) int {
	ver := pb.Version
	if ver == 0 {
		return 0 // skip v0 for water
	}
	pos := 0
	if ver > 1 {
		pos += 4
	}
	if pos+28 > len(pbData) {
		return 0
	}
	pbtype := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // unknown
	p1 := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 12 // p1+p2+p3

	invPacking := float32(math.Pow(2, float64(p1)))
	packing1 := float32(1.0 / math.Pow(2, float64(p1)))

	raised := 0
	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return raised
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 8

		var stride int
		switch pbtype {
		case 2:
			stride = 17
		case 4:
			stride = 19
		default:
			return raised
		}

		for vi := int32(0); vi < nverts; vi++ {
			if pos+stride > len(pbData) {
				return raised
			}
			x := int16(binary.LittleEndian.Uint16(pbData[pos:]))
			y := int16(binary.LittleEndian.Uint16(pbData[pos+2:]))
			z := int16(binary.LittleEndian.Uint16(pbData[pos+4:]))
			_ = z
			worldX := float64(float32(x) * packing1)
			worldZ := float64(float32(z) * packing1)
			if pbtype == 4 && len(preTrans) > 0 {
				vgroup := int16(binary.LittleEndian.Uint16(pbData[pos+17:]))
				if int(vgroup) < len(preTrans) {
					worldX += float64(preTrans[vgroup].X)
					worldZ += float64(preTrans[vgroup].Z)
				}
			}
			dx := worldX - targetX
			dz := worldZ - targetZ
			if dx*dx+dz*dz <= radiusSq {
				newY := float32(y) + amount*invPacking
				if newY > 32767 {
					newY = 32767
				} else if newY < -32768 {
					newY = -32768
				}
				binary.LittleEndian.PutUint16(pbData[pos+2:], uint16(int16(newY)))
				raised++
			}
			pos += stride
		}
	}
	return raised
}

// raisePBVertices raises Y for vertices within radius of (targetX, targetZ).
// Uses smooth gradient: full raise at center, tapering to 0 at edge.
// Returns the number of vertices raised.
func raisePBVertices(pbData []byte, pb *esf.ObjInfo, preTrans []esf.Point, targetX, targetZ, radiusSq float64, amount float32) int {
	ver := pb.Version
	if ver == 0 {
		return raiseV0(pbData, targetX, targetZ, radiusSq, amount)
	}

	pos := 0
	if ver > 1 {
		pos += 4 // skip dictID
	}

	if pos+28 > len(pbData) {
		return 0
	}

	pbtype := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // unknown
	p1 := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // p2
	pos += 4 // p3

	packing1 := float32(1.0 / math.Pow(2, float64(p1)))
	invPacking := float32(math.Pow(2, float64(p1)))

	raised := 0
	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return raised
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 4
		pos += 4 // material

		for vi := int32(0); vi < nverts; vi++ {
			var stride int
			switch pbtype {
			case 2:
				stride = 17
			case 4:
				stride = 19
			case 5:
				stride = 21
			default:
				return raised
			}

			if pos+stride > len(pbData) {
				return raised
			}

			// Read vertex position
			x := int16(binary.LittleEndian.Uint16(pbData[pos:]))
			y := int16(binary.LittleEndian.Uint16(pbData[pos+2:]))
			z := int16(binary.LittleEndian.Uint16(pbData[pos+4:]))

			// Compute world-space X/Z
			worldX := float64(float32(x) * packing1)
			worldZ := float64(float32(z) * packing1)

			// Add pretranslation if pbtype 4 (has vgroup)
			if pbtype == 4 && len(preTrans) > 0 {
				vgroup := int16(binary.LittleEndian.Uint16(pbData[pos+17:]))
				if int(vgroup) < len(preTrans) {
					worldX += float64(preTrans[vgroup].X)
					worldZ += float64(preTrans[vgroup].Z)
				}
			}

			// Check if within radius, apply gradient falloff
			dx := worldX - targetX
			dz := worldZ - targetZ
			distSq := dx*dx + dz*dz
			if distSq <= radiusSq {
				// Smooth gradient: cosine falloff (1.0 at center, 0.0 at edge)
				dist := math.Sqrt(distSq)
				radius := math.Sqrt(radiusSq)
				t := dist / radius
				// Smoothstep: 3t²-2t³ inverted
				factor := float32(1.0 - (3*t*t - 2*t*t*t))
				raiseAmt := amount * factor * invPacking
				newY := float32(y) + raiseAmt
				if newY > 32767 {
					newY = 32767
				} else if newY < -32768 {
					newY = -32768
				}
				binary.LittleEndian.PutUint16(pbData[pos+2:], uint16(int16(newY)))
				raised++
			}

			pos += stride
		}
	}
	return raised
}

// raiseV0 raises Y for version 0 PrimBuffers (float32 coords, no pretrans).
func raiseV0(pbData []byte, targetX, targetZ, radiusSq float64, amount float32) int {
	pos := 0
	if pos+12 > len(pbData) {
		return 0
	}

	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos+8:]))
	pos += 12

	raised := 0
	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return raised
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 4
		pos += 4

		for vi := int32(0); vi < nverts; vi++ {
			if pos+36 > len(pbData) {
				return raised
			}

			worldX := float64(math.Float32frombits(binary.LittleEndian.Uint32(pbData[pos:])))
			worldZ := float64(math.Float32frombits(binary.LittleEndian.Uint32(pbData[pos+8:])))

			dx := worldX - targetX
			dz := worldZ - targetZ
			distSq := dx*dx + dz*dz
			if distSq <= radiusSq {
				dist := math.Sqrt(distSq)
				radius := math.Sqrt(radiusSq)
				t := dist / radius
				factor := float32(1.0 - (3*t*t - 2*t*t*t))
				yBits := binary.LittleEndian.Uint32(pbData[pos+4:])
				y := math.Float32frombits(yBits)
				y += amount * factor
				binary.LittleEndian.PutUint32(pbData[pos+4:], math.Float32bits(y))
				raised++
			}
			pos += 36
		}
	}
	return raised
}

// raiseCBVertices raises Y for CollBuffer vertices within radius with gradient falloff.
func raiseCBVertices(cbData []byte, cb *esf.ObjInfo, preTrans []esf.Point, targetX, targetZ, radiusSq float64, amount float32) int {
	ver := cb.Version
	pos := 0

	var cbtype int32
	if ver > 1 {
		if pos+4 > len(cbData) {
			return 0
		}
		cbtype = int32(binary.LittleEndian.Uint32(cbData[pos:]))
		pos += 4
	}

	if pos+12 > len(cbData) {
		return 0
	}
	pos += 4 // numPrimgroups
	numVertexGroups := int32(binary.LittleEndian.Uint32(cbData[pos:]))
	pos += 4
	pos += 4 // numSomething

	var packing int32
	if ver >= 2 {
		if pos+4 > len(cbData) {
			return 0
		}
		packing = int32(binary.LittleEndian.Uint32(cbData[pos:]))
		pos += 4
	}

	p := float32(1.0 / math.Pow(2, float64(packing)))
	invP := float32(math.Pow(2, float64(packing)))
	radius := math.Sqrt(radiusSq)

	raised := 0
	for i := int32(0); i < numVertexGroups; i++ {
		if pos+12 > len(cbData) {
			return raised
		}
		num := int32(binary.LittleEndian.Uint32(cbData[pos:]))
		pos += 4
		pos += 4 // primg
		pos += 4 // list

		switch cbtype {
		case 0:
			for j := int32(0); j < num; j++ {
				if pos+12 > len(cbData) {
					return raised
				}
				worldX := float64(math.Float32frombits(binary.LittleEndian.Uint32(cbData[pos:])))
				worldZ := float64(math.Float32frombits(binary.LittleEndian.Uint32(cbData[pos+8:])))
				dx := worldX - targetX
				dz := worldZ - targetZ
				distSq := dx*dx + dz*dz
				if distSq <= radiusSq {
					t := math.Sqrt(distSq) / radius
					factor := float32(1.0 - (3*t*t - 2*t*t*t))
					y := math.Float32frombits(binary.LittleEndian.Uint32(cbData[pos+4:]))
					y += amount * factor
					binary.LittleEndian.PutUint32(cbData[pos+4:], math.Float32bits(y))
					raised++
				}
				pos += 12
			}
		case 1:
			for j := int32(0); j < num; j++ {
				if pos+6 > len(cbData) {
					return raised
				}
				x := int16(binary.LittleEndian.Uint16(cbData[pos:]))
				y := int16(binary.LittleEndian.Uint16(cbData[pos+2:]))
				z := int16(binary.LittleEndian.Uint16(cbData[pos+4:]))
				worldX := float64(float32(x) * p)
				worldZ := float64(float32(z) * p)
				dx := worldX - targetX
				dz := worldZ - targetZ
				distSq := dx*dx + dz*dz
				if distSq <= radiusSq {
					t := math.Sqrt(distSq) / radius
					factor := float32(1.0 - (3*t*t - 2*t*t*t))
					newY := float32(y) + amount*factor*invP
					if newY > 32767 {
						newY = 32767
					} else if newY < -32768 {
						newY = -32768
					}
					binary.LittleEndian.PutUint16(cbData[pos+2:], uint16(int16(newY)))
					raised++
				}
				pos += 6
			}
		case 2:
			for j := int32(0); j < num; j++ {
				if pos+8 > len(cbData) {
					return raised
				}
				x := int16(binary.LittleEndian.Uint16(cbData[pos:]))
				y := int16(binary.LittleEndian.Uint16(cbData[pos+2:]))
				z := int16(binary.LittleEndian.Uint16(cbData[pos+4:]))
				vgroup := int(int8(int16(binary.LittleEndian.Uint16(cbData[pos+6:]))))
				worldX := float64(float32(x) * p)
				worldZ := float64(float32(z) * p)
				if len(preTrans) > 0 && vgroup >= 0 && vgroup < len(preTrans) {
					worldX += float64(preTrans[vgroup].X)
					worldZ += float64(preTrans[vgroup].Z)
				}
				dx := worldX - targetX
				dz := worldZ - targetZ
				distSq := dx*dx + dz*dz
				if distSq <= radiusSq {
					t := math.Sqrt(distSq) / radius
					factor := float32(1.0 - (3*t*t - 2*t*t*t))
					newY := float32(y) + amount*factor*invP
					if newY > 32767 {
						newY = 32767
					} else if newY < -32768 {
						newY = -32768
					}
					binary.LittleEndian.PutUint16(cbData[pos+2:], uint16(int16(newY)))
					raised++
				}
				pos += 8
			}
		case 3:
			for j := int32(0); j < num; j++ {
				if pos+8 > len(cbData) {
					return raised
				}
				x := int16(binary.LittleEndian.Uint16(cbData[pos:]))
				y := int16(binary.LittleEndian.Uint16(cbData[pos+2:]))
				z := int16(binary.LittleEndian.Uint16(cbData[pos+4:]))
				vgroup := int(cbData[pos+6])
				worldX := float64(float32(x) * p)
				worldZ := float64(float32(z) * p)
				if len(preTrans) > 0 && vgroup < len(preTrans) {
					worldX += float64(preTrans[vgroup].X)
					worldZ += float64(preTrans[vgroup].Z)
				}
				dx := worldX - targetX
				dz := worldZ - targetZ
				distSq := dx*dx + dz*dz
				if distSq <= radiusSq {
					t := math.Sqrt(distSq) / radius
					factor := float32(1.0 - (3*t*t - 2*t*t*t))
					newY := float32(y) + amount*factor*invP
					if newY > 32767 {
						newY = 32767
					} else if newY < -32768 {
						newY = -32768
					}
					binary.LittleEndian.PutUint16(cbData[pos+2:], uint16(int16(newY)))
					raised++
				}
				pos += 8
			}
		}
	}
	return raised
}

func patchV0(pbData []byte) error {
	pos := 0
	if pos+12 > len(pbData) {
		return fmt.Errorf("truncated v0 header")
	}

	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos+8:]))
	pos += 12

	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return fmt.Errorf("truncated v0 face")
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 4
		pos += 4

		for vi := int32(0); vi < nverts; vi++ {
			if pos+36 > len(pbData) {
				return fmt.Errorf("truncated v0 vertex")
			}
			switch mode {
			case "yscale":
				yBits := binary.LittleEndian.Uint32(pbData[pos+4:])
				y := math.Float32frombits(yBits)
				y *= yScaleVal
				binary.LittleEndian.PutUint32(pbData[pos+4:], math.Float32bits(y))
			case "color":
				pbData[pos+32] = colorR
				pbData[pos+33] = colorG
				pbData[pos+34] = colorB
			}
			pos += 36
		}
	}
	return nil
}

// setAlphaInRadius sets the alpha byte for all pbtype 4 vertices within radius.
func setAlphaInRadius(pbData []byte, pb *esf.ObjInfo, preTrans []esf.Point, targetX, targetZ, radiusSq float64, alpha byte) int {
	ver := pb.Version
	if ver == 0 {
		return 0
	}
	pos := 0
	if ver > 1 {
		pos += 4
	}
	if pos+28 > len(pbData) {
		return 0
	}
	pbtype := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	if pbtype != 4 {
		return 0
	}
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // unknown
	p1 := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 12

	packing1 := float32(1.0 / math.Pow(2, float64(p1)))
	count := 0
	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return count
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 8
		for vi := int32(0); vi < nverts; vi++ {
			if pos+19 > len(pbData) {
				return count
			}
			x := int16(binary.LittleEndian.Uint16(pbData[pos:]))
			z := int16(binary.LittleEndian.Uint16(pbData[pos+4:]))
			worldX := float64(float32(x) * packing1)
			worldZ := float64(float32(z) * packing1)
			vgroup := int16(binary.LittleEndian.Uint16(pbData[pos+17:]))
			if len(preTrans) > 0 && int(vgroup) < len(preTrans) {
				worldX += float64(preTrans[vgroup].X)
				worldZ += float64(preTrans[vgroup].Z)
			}
			dx := worldX - targetX
			dz := worldZ - targetZ
			if dx*dx+dz*dz <= radiusSq {
				pbData[pos+16] = alpha
				count++
			}
			pos += 19
		}
	}
	return count
}

// setHillProfile sets terrain PB vertices to an absolute dome height profile.
// Target world Y = shoreY + peak * smoothstep(1 - dist/radius).
// Vertices outside radius are untouched.
func setHillProfile(pbData []byte, pb *esf.ObjInfo, preTrans []esf.Point, targetX, targetZ, radiusSq float64, shoreY, peak float32) int {
	ver := pb.Version
	if ver == 0 {
		return 0
	}
	pos := 0
	if ver > 1 {
		pos += 4
	}
	if pos+28 > len(pbData) {
		return 0
	}
	pbtype := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	if pbtype != 4 {
		return 0
	}
	pos += 4
	pos += 4 // nmats
	nfaces := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 4
	pos += 4 // unknown
	p1 := int32(binary.LittleEndian.Uint32(pbData[pos:]))
	pos += 12

	packing1 := float32(1.0 / math.Pow(2, float64(p1)))
	invPacking := float32(math.Pow(2, float64(p1)))
	radius := math.Sqrt(radiusSq)
	count := 0

	for fi := int32(0); fi < nfaces; fi++ {
		if pos+8 > len(pbData) {
			return count
		}
		nverts := int32(binary.LittleEndian.Uint32(pbData[pos:]))
		pos += 8
		for vi := int32(0); vi < nverts; vi++ {
			if pos+19 > len(pbData) {
				return count
			}
			x := int16(binary.LittleEndian.Uint16(pbData[pos:]))
			z := int16(binary.LittleEndian.Uint16(pbData[pos+4:]))
			vgroup := int16(binary.LittleEndian.Uint16(pbData[pos+17:]))

			worldX := float64(float32(x) * packing1)
			worldZ := float64(float32(z) * packing1)
			preTransY := float32(0)
			if len(preTrans) > 0 && int(vgroup) < len(preTrans) {
				worldX += float64(preTrans[vgroup].X)
				worldZ += float64(preTrans[vgroup].Z)
				preTransY = preTrans[vgroup].Y
			}

			dx := worldX - targetX
			dz := worldZ - targetZ
			distSq := dx*dx + dz*dz
			if distSq <= radiusSq {
				// Smoothstep dome: full peak at center, 0 at edge
				t := math.Sqrt(distSq) / radius
				factor := float32(1.0 - (3*t*t - 2*t*t*t))
				// Absolute target world Y
				targetWorldY := shoreY + peak*factor
				// Convert world Y back to packed int16: packedY = (worldY - preTrans.Y) * invPacking
				newPackedY := (targetWorldY - preTransY) * invPacking
				if newPackedY > 32767 {
					newPackedY = 32767
				} else if newPackedY < -32768 {
					newPackedY = -32768
				}
				binary.LittleEndian.PutUint16(pbData[pos+2:], uint16(int16(newPackedY)))
				count++
			}
			pos += 19
		}
	}
	return count
}

// setHillCB sets collision buffer vertices to the same hill profile.
func setHillCB(cbData []byte, cb *esf.ObjInfo, preTrans []esf.Point, targetX, targetZ, radiusSq float64, shoreY, peak float32) int {
	ver := cb.Version
	pos := 0

	var cbtype int32
	if ver > 1 {
		if pos+4 > len(cbData) {
			return 0
		}
		cbtype = int32(binary.LittleEndian.Uint32(cbData[pos:]))
		pos += 4
	}

	if pos+12 > len(cbData) {
		return 0
	}
	pos += 4 // numPrimgroups
	numVertexGroups := int32(binary.LittleEndian.Uint32(cbData[pos:]))
	pos += 4
	pos += 4 // numSomething

	var packing int32
	if ver >= 2 {
		if pos+4 > len(cbData) {
			return 0
		}
		packing = int32(binary.LittleEndian.Uint32(cbData[pos:]))
		pos += 4
	}

	p := float32(1.0 / math.Pow(2, float64(packing)))
	invP := float32(math.Pow(2, float64(packing)))
	radius := math.Sqrt(radiusSq)

	count := 0
	for i := int32(0); i < numVertexGroups; i++ {
		if pos+12 > len(cbData) {
			return count
		}
		num := int32(binary.LittleEndian.Uint32(cbData[pos:]))
		pos += 4
		pos += 4 // primg
		pos += 4 // list

		switch cbtype {
		case 2:
			for j := int32(0); j < num; j++ {
				if pos+8 > len(cbData) {
					return count
				}
				x := int16(binary.LittleEndian.Uint16(cbData[pos:]))
				z := int16(binary.LittleEndian.Uint16(cbData[pos+4:]))
				vgroup := int(int8(int16(binary.LittleEndian.Uint16(cbData[pos+6:]))))
				worldX := float64(float32(x) * p)
				worldZ := float64(float32(z) * p)
				preTransY := float32(0)
				if len(preTrans) > 0 && vgroup >= 0 && vgroup < len(preTrans) {
					worldX += float64(preTrans[vgroup].X)
					worldZ += float64(preTrans[vgroup].Z)
					preTransY = preTrans[vgroup].Y
				}
				dx := worldX - targetX
				dz := worldZ - targetZ
				distSq := dx*dx + dz*dz
				if distSq <= radiusSq {
					t := math.Sqrt(distSq) / radius
					factor := float32(1.0 - (3*t*t - 2*t*t*t))
					targetWorldY := shoreY + peak*factor
					newPackedY := (targetWorldY - preTransY) * invP
					if newPackedY > 32767 {
						newPackedY = 32767
					} else if newPackedY < -32768 {
						newPackedY = -32768
					}
					binary.LittleEndian.PutUint16(cbData[pos+2:], uint16(int16(newPackedY)))
					count++
				}
				pos += 8
			}
		case 3:
			for j := int32(0); j < num; j++ {
				if pos+8 > len(cbData) {
					return count
				}
				x := int16(binary.LittleEndian.Uint16(cbData[pos:]))
				z := int16(binary.LittleEndian.Uint16(cbData[pos+4:]))
				vgroup := int(cbData[pos+6])
				worldX := float64(float32(x) * p)
				worldZ := float64(float32(z) * p)
				preTransY := float32(0)
				if len(preTrans) > 0 && vgroup < len(preTrans) {
					worldX += float64(preTrans[vgroup].X)
					worldZ += float64(preTrans[vgroup].Z)
					preTransY = preTrans[vgroup].Y
				}
				dx := worldX - targetX
				dz := worldZ - targetZ
				distSq := dx*dx + dz*dz
				if distSq <= radiusSq {
					t := math.Sqrt(distSq) / radius
					factor := float32(1.0 - (3*t*t - 2*t*t*t))
					targetWorldY := shoreY + peak*factor
					newPackedY := (targetWorldY - preTransY) * invP
					if newPackedY > 32767 {
						newPackedY = 32767
					} else if newPackedY < -32768 {
						newPackedY = -32768
					}
					binary.LittleEndian.PutUint16(cbData[pos+2:], uint16(int16(newPackedY)))
					count++
				}
				pos += 8
			}
		default:
			// cbtype 0/1 don't have vgroup — skip (water collision)
			var stride int
			if cbtype == 0 {
				stride = 12
			} else {
				stride = 6
			}
			pos += int(num) * stride
		}
	}
	return count
}
