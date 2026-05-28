// dumpanim dumps HSpriteAnim (0x2600) binary data from CHAR.ESF CSprites
// to reverse-engineer the animation format.
//
// Discovered format (format=1, packed):
//
//   Header (32 bytes):
//     int32 dictID       — animation name hash
//     int32 format       — 1 = packed (VIHSpritePackFrame)
//     int32 numNodes     — number of animation tracks
//     int32 numFrames    — keyframes per track
//     int32 numKeyframes — extra keyframe entries (usually 0)
//     float32 fps        — playback rate (typically 10.0)
//     float32 playSpeed  — speed multiplier (typically 1.0)
//     int32 playbackType — 0=loop, 1=once, etc.
//
//   Extra keyframe data: numKeyframes × 8 bytes
//
//   Per node (numNodes times):
//     int32 refID        — maps this anim track to a hierarchy bone index
//     numFrames × VIHSpritePackFrame (16 bytes each):
//       8 × int16: quaternion(4) + scale(1) + position(3)
//       Quaternion normalized to magnitude ~32767
package main

import (
	"encoding/binary"
	"fmt"
	"log"
	"math"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

const esfPath = "/home/sdg/claude-eqoa/extracted-assets/CHAR.ESF"

var models = []struct {
	DictID int32
	Name   string
}{
	{1893243078, "Human Male"},
	{-2071956336, "Ogre"},
	{-2145145487, "Spider/Large"},
	{335215986, "Quadruped"},
}

func main() {
	log.SetFlags(0)

	file, err := esf.Open(esfPath)
	if err != nil {
		log.Fatalf("Failed to open ESF: %v", err)
	}
	if err := file.BuildDictionary(); err != nil {
		log.Fatalf("Failed to build dictionary: %v", err)
	}

	// Investigate refID → bone mapping
	for _, m := range models {
		investigateRefIDs(file, m.DictID, m.Name)
	}
}

func investigateRefIDs(file *esf.ObjFile, dictID int32, label string) {
	obj, err := file.FindObject(dictID)
	if err != nil || obj == nil {
		fmt.Printf("\n=== %s (%d): NOT FOUND ===\n", label, dictID)
		return
	}
	fmt.Printf("\n=== %s (DictID %d) ===\n", label, dictID)
	info := obj.ObjInfo()

	// Dump ALL child types of the CSprite ObjInfo tree
	fmt.Println("\n  --- ObjInfo tree (immediate children) ---")
	for _, c := range info.Children {
		fmt.Printf("    type=0x%04x (%s) ver=%d size=%d dict=0x%08x subs=%d\n",
			c.Type, esf.TypeName(c.Type), c.Version, c.Size, uint32(c.DictID), c.NumSubObjects)
	}

	// Get hierarchy ObjInfo and read the "pre-ints" raw data
	hierInfo := info.Child(esf.TypeHSpriteHierarchy)
	if hierInfo == nil {
		fmt.Println("  No hierarchy found!")
		return
	}

	fmt.Printf("\n  --- Hierarchy ObjInfo: ver=%d size=%d offset=0x%x ---\n",
		hierInfo.Version, hierInfo.Size, hierInfo.Offset)

	// Read hierarchy raw data to extract the "pre-ints"
	hierRaw := file.RawBytes(hierInfo.Offset, int(hierInfo.Size))
	off := 0

	var nodeIDs []int32
	if hierInfo.Version != 0 {
		num := readI32(hierRaw, off)
		off += 4
		fmt.Printf("  Hierarchy pre-int count: %d\n", num)
		nodeIDs = make([]int32, num)
		for i := int32(0); i < num; i++ {
			nodeIDs[i] = readI32(hierRaw, off)
			off += 4
		}
		fmt.Printf("  Hierarchy pre-ints (potential NodeID list):\n")
		for i, id := range nodeIDs {
			fmt.Printf("    nodeID[%2d] = %d (0x%08x)\n", i, id, uint32(id))
		}
	} else {
		fmt.Println("  Hierarchy version=0, no pre-ints")
	}

	// Read bone data
	numBones := readI32(hierRaw, off)
	off += 4
	fmt.Printf("\n  Bone count: %d\n", numBones)

	// Now collect ALL unique refIDs from this model's animations
	anims := findAllOfType(info, esf.TypeHSpriteAnim)
	fmt.Printf("  Animation count: %d\n", len(anims))

	// Collect refIDs from first animation
	if len(anims) > 0 {
		raw := file.RawBytes(anims[0].Offset, int(anims[0].Size))
		if len(raw) >= 32 {
			numNodes := readI32(raw, 8)
			numFrames := readI32(raw, 12)
			numKF := readI32(raw, 16)
			animOff := 32 + int(numKF)*8

			fmt.Printf("\n  --- Animation 0 refIDs (numNodes=%d) ---\n", numNodes)
			animRefIDs := make([]int32, numNodes)
			for n := int32(0); n < numNodes; n++ {
				animRefIDs[n] = readI32(raw, animOff)
				animOff += 4 + int(numFrames)*16
			}

			// Cross-reference: for each animation refID, find matching nodeID index
			if len(nodeIDs) > 0 {
				nodeIDMap := map[int32]int{}
				for i, id := range nodeIDs {
					nodeIDMap[id] = i
				}
				fmt.Printf("\n  --- Cross-reference: anim refID → hierarchy nodeID index ---\n")
				matchCount := 0
				for n, ref := range animRefIDs {
					if boneIdx, ok := nodeIDMap[ref]; ok {
						fmt.Printf("    animNode[%2d] refID=%d → nodeID[%2d] (bone %d) MATCH\n",
							n, ref, boneIdx, boneIdx)
						matchCount++
					} else {
						fmt.Printf("    animNode[%2d] refID=%d → NO MATCH in nodeID list\n", n, ref)
					}
				}
				fmt.Printf("  Matched: %d/%d\n", matchCount, len(animRefIDs))
			} else {
				// No nodeIDs, just print the refIDs
				for n, ref := range animRefIDs {
					fmt.Printf("    animNode[%2d] refID=%d\n", n, ref)
				}
			}

			// Also check: are refIDs in the ESF dictionary?
			fmt.Printf("\n  --- Dictionary lookup for refIDs ---\n")
			for n, ref := range animRefIDs {
				dObj, _ := file.FindObject(ref)
				if dObj != nil {
					dInfo := dObj.ObjInfo()
					fmt.Printf("    animNode[%2d] refID=%d → FOUND in dict: type=0x%04x (%s)\n",
						n, ref, dInfo.Type, esf.TypeName(dInfo.Type))
				}
			}
		}
	}

	// Read RefMap data — the second RefMap (near hierarchy) should contain bone ref mapping
	refMaps := findAllOfType(info, esf.TypeRefMap)
	fmt.Printf("\n  --- RefMap objects: %d ---\n", len(refMaps))
	refIDToBone := map[int32]int32{}
	for ri, rm := range refMaps {
		rmRaw := file.RawBytes(rm.Offset, int(rm.Size))
		rmDictID := readI32(rmRaw, 0)
		fmt.Printf("    RefMap[%d] dictID=0x%08x size=%d\n", ri, uint32(rmDictID), rm.Size)

		// Try to interpret as: dictID(4) + count(4) + count × (int32, int32)
		if rm.Size >= 8 {
			count := readI32(rmRaw, 4)
			expectedRMSize := int32(8) + count*8
			fmt.Printf("      Potential count=%d, expected size=%d, actual=%d %s\n",
				count, expectedRMSize, rm.Size,
				func() string {
					if expectedRMSize == rm.Size {
						return "MATCH!"
					}
					return "no match"
				}())

			if expectedRMSize == rm.Size && count > 0 && count < 200 {
				fmt.Printf("      Pairs:\n")
				for p := int32(0); p < count; p++ {
					pOff := 8 + int(p)*8
					a := readI32(rmRaw, pOff)
					b := readI32(rmRaw, pOff+4)
					fmt.Printf("        [%2d] %d (0x%08x) → %d\n", p, a, uint32(a), b)
					refIDToBone[a] = b
				}
			}
		}
	}

	// Cross-reference refIDs from animation with RefMap entries
	if len(refIDToBone) > 0 && len(anims) > 0 {
		raw := file.RawBytes(anims[0].Offset, int(anims[0].Size))
		if len(raw) >= 32 {
			numNodes := readI32(raw, 8)
			numFrames := readI32(raw, 12)
			numKF := readI32(raw, 16)
			animOff := 32 + int(numKF)*8

			fmt.Printf("\n  --- RefMap Cross-reference for Anim[0] ---\n")
			matchCount := 0
			for n := int32(0); n < numNodes; n++ {
				ref := readI32(raw, animOff)
				if boneIdx, ok := refIDToBone[ref]; ok {
					fmt.Printf("    animNode[%2d] refID=0x%08x → bone %d MATCH\n",
						n, uint32(ref), boneIdx)
					matchCount++
				} else {
					fmt.Printf("    animNode[%2d] refID=0x%08x → NOT IN REFMAP\n",
						n, uint32(ref))
				}
				animOff += 4 + int(numFrames)*16
			}
			fmt.Printf("    Matched: %d/%d\n", matchCount, numNodes)
		}
	}

	// Verify RefMap cross-reference across ALL animations
	if len(refIDToBone) > 0 {
		fmt.Printf("\n  --- RefMap match rate across ALL %d animations ---\n", len(anims))
		fullMatch := 0
		noMatch := 0
		for a := 0; a < len(anims); a++ {
			raw := file.RawBytes(anims[a].Offset, int(anims[a].Size))
			if len(raw) < 32 {
				continue
			}
			aDictID := readI32(raw, 0)
			numNodes := readI32(raw, 8)
			numFrames := readI32(raw, 12)
			numKF := readI32(raw, 16)
			fps := readF32(raw, 20)
			playback := readI32(raw, 28)
			animOff := 32 + int(numKF)*8
			matched := 0
			for n := int32(0); n < numNodes; n++ {
				ref := readI32(raw, animOff)
				if _, ok := refIDToBone[ref]; ok {
					matched++
				}
				animOff += 4 + int(numFrames)*16
			}
			if matched == int(numNodes) {
				fullMatch++
				fmt.Printf("    MATCH Anim[%2d] dict=0x%08x nodes=%d frames=%d fps=%.0f play=%d\n",
					a, uint32(aDictID), numNodes, numFrames, fps, playback)
			} else {
				noMatch++
			}
		}
		fmt.Printf("    Full match: %d, No match: %d (total %d)\n", fullMatch, noMatch, len(anims))

		// For the FIRST non-matching animation, dump node[0] raw bytes in detail
		for a := 0; a < len(anims); a++ {
			raw := file.RawBytes(anims[a].Offset, int(anims[a].Size))
			if len(raw) < 32 {
				continue
			}
			numNodes := readI32(raw, 8)
			numFrames := readI32(raw, 12)
			numKF := readI32(raw, 16)
			animOff := 32 + int(numKF)*8
			ref := readI32(raw, animOff)
			if _, ok := refIDToBone[ref]; ok {
				continue // skip matching
			}
			// This is a non-matching animation - dump raw bytes of first 2 nodes
			fmt.Printf("\n  --- Non-matching Anim[%d] raw bytes (nodes=%d, frames=%d) ---\n", a, numNodes, numFrames)
			fmt.Printf("    Node[0] starts at offset %d:\n", animOff)
			dumpLen := 4 + int(numFrames)*16
			if dumpLen > 68 {
				dumpLen = 68 // 4 bytes "refID" + 4 frames of 16 bytes
			}
			hexdump(raw[animOff:animOff+dumpLen], "      ")

			// Also show as int16 pairs
			fmt.Printf("    Node[0] as int16 pairs:\n")
			for b := 0; b < dumpLen && b+1 < dumpLen; b += 2 {
				s := readI16(raw, animOff+b)
				if b%16 == 0 {
					fmt.Printf("      off=%3d: ", b)
				}
				fmt.Printf("%6d ", s)
				if b%16 == 14 {
					fmt.Println()
				}
			}
			fmt.Println()

			// Verify quat magnitude for BOTH matching and non-matching
			fmt.Printf("    Non-matching quat magnitudes (frames at offset 4 from refID):\n")
			for f := 0; f < int(numFrames) && f < 4; f++ {
				foff := animOff + 4 + f*16
				s0 := readI16(raw, foff)
				s1 := readI16(raw, foff+2)
				s2 := readI16(raw, foff+4)
				s3 := readI16(raw, foff+6)
				qmag := math.Sqrt(float64(s0)*float64(s0) + float64(s1)*float64(s1) +
					float64(s2)*float64(s2) + float64(s3)*float64(s3))
				fmt.Printf("      f[%d]: |q|=%.0f  (%d,%d,%d,%d, %d,%d,%d,%d)\n", f, qmag,
					s0, s1, s2, s3, readI16(raw, foff+8), readI16(raw, foff+10), readI16(raw, foff+12), readI16(raw, foff+14))
			}
			break
		}
	}

	// Dump the 0x2910 object to look for alternative refID mapping
	obj2910 := info.Child(0x2910)
	if obj2910 != nil {
		raw2910 := file.RawBytes(obj2910.Offset, int(obj2910.Size))
		fmt.Printf("\n  --- Type 0x2910 (ver=%d, size=%d) first 128 bytes ---\n", obj2910.Version, obj2910.Size)
		dLen := len(raw2910)
		if dLen > 128 {
			dLen = 128
		}
		// Interpret as int32 array
		fmt.Printf("    As int32 values:\n")
		for i := 0; i < dLen; i += 4 {
			v := readI32(raw2910, i)
			fmt.Printf("      [%3d] %12d (0x%08x)\n", i/4, v, uint32(v))
		}
	}

	fmt.Println()
}

// verifyFormatHypothesis tests our header + per-node format theory against all 0x2600 objects.
func verifyFormatHypothesis(file *esf.ObjFile, anims []*esf.ObjInfo) {
	fmt.Println("=== Format Verification ===")

	matchCount := 0
	mismatchCount := 0
	formatDist := map[int32]int{}
	playbackDist := map[int32]int{}
	fpsDist := map[float32]int{}
	versionDist := map[int16]int{}
	keyframeDist := map[int32]int{}

	for _, info := range anims {
		raw := file.RawBytes(info.Offset, int(info.Size))
		if len(raw) < 32 {
			mismatchCount++
			continue
		}

		versionDist[info.Version]++

		dictID := readI32(raw, 0)
		format := readI32(raw, 4)
		numNodes := readI32(raw, 8)
		numFrames := readI32(raw, 12)
		numKF := readI32(raw, 16)
		fps := readF32(raw, 20)
		playSpeed := readF32(raw, 24)
		playback := readI32(raw, 28)

		_ = dictID
		formatDist[format]++
		playbackDist[playback]++
		fpsDist[fps]++
		keyframeDist[numKF]++

		// Check if our formula works:
		// totalSize = 32 + numKF*8 + numNodes*(4 + numFrames*16)
		expectedSize := int32(32) + numKF*8 + numNodes*(4+numFrames*16)
		if expectedSize == info.Size {
			matchCount++
		} else {
			mismatchCount++
			if mismatchCount <= 10 {
				fmt.Printf("  MISMATCH: size=%d expected=%d (format=%d nodes=%d frames=%d kf=%d fps=%.1f speed=%.1f play=%d version=%d)\n",
					info.Size, expectedSize, format, numNodes, numFrames, numKF, fps, playSpeed, playback, info.Version)
			}
		}
	}

	fmt.Printf("\n  Results: %d/%d match (%.1f%%)\n", matchCount, len(anims), 100*float64(matchCount)/float64(len(anims)))
	fmt.Printf("  Mismatches: %d\n", mismatchCount)

	fmt.Printf("\n  Format distribution:   ")
	for f, c := range formatDist {
		fmt.Printf("format=%d: %d  ", f, c)
	}
	fmt.Printf("\n  Version distribution:  ")
	for v, c := range versionDist {
		fmt.Printf("v%d: %d  ", v, c)
	}
	fmt.Printf("\n  Playback distribution: ")
	for p, c := range playbackDist {
		fmt.Printf("type=%d: %d  ", p, c)
	}
	fmt.Printf("\n  FPS distribution:      ")
	for f, c := range fpsDist {
		fmt.Printf("%.1f: %d  ", f, c)
	}
	fmt.Printf("\n  Keyframe distribution: ")
	for k, c := range keyframeDist {
		fmt.Printf("kf=%d: %d  ", k, c)
	}
	fmt.Println()
}

func dumpModelAnims(file *esf.ObjFile, dictID int32, label string) {
	obj, err := file.FindObject(dictID)
	if err != nil || obj == nil {
		fmt.Printf("\n=== %s (%d): NOT FOUND ===\n", label, dictID)
		return
	}

	fmt.Printf("\n=== %s (DictID %d) ===\n", label, dictID)
	info := obj.ObjInfo()

	// Get hierarchy
	var hier *esf.HSpriteHierarchy
	switch s := obj.(type) {
	case *esf.CSprite:
		hier = s.Hierarchy
	case *esf.HSprite:
		hier = s.Hierarchy
	}

	boneCount := 0
	if hier != nil {
		boneCount = len(hier.Nodes)
		fmt.Printf("  Hierarchy: %d bones\n", boneCount)
		// Show bone bind poses for cross-reference
		for i, b := range hier.Nodes {
			fmt.Printf("    bone[%2d] parent=%2d pos=(%.3f,%.3f,%.3f) q=(%.4f,%.4f,%.4f,%.4f) s=%.3f\n",
				i, b.ParentID, b.Pos.X, b.Pos.Y, b.Pos.Z,
				b.Quat[0], b.Quat[1], b.Quat[2], b.Quat[3], b.Scale)
		}
	}

	// Find all 0x2600 children
	anims := findAllOfType(info, esf.TypeHSpriteAnim)
	fmt.Printf("  Animations: %d\n\n", len(anims))

	// Dump first 5 animations in detail
	maxAnims := len(anims)
	if maxAnims > 5 {
		maxAnims = 5
	}

	for i := 0; i < maxAnims; i++ {
		dumpAnimation(file, anims[i], i, hier)
	}

	// Summary for remaining animations
	if len(anims) > 5 {
		fmt.Printf("  --- Remaining %d animations (summary) ---\n", len(anims)-5)
		for i := 5; i < len(anims); i++ {
			raw := file.RawBytes(anims[i].Offset, int(anims[i].Size))
			if len(raw) < 32 {
				continue
			}
			aDict := readI32(raw, 0)
			format := readI32(raw, 4)
			numNodes := readI32(raw, 8)
			numFrames := readI32(raw, 12)
			numKF := readI32(raw, 16)
			fps := readF32(raw, 20)
			playback := readI32(raw, 28)
			dur := float32(numFrames) / fps
			fmt.Printf("    [%2d] dict=0x%08x fmt=%d nodes=%d frames=%d kf=%d fps=%.0f play=%d dur=%.2fs size=%d\n",
				i, uint32(aDict), format, numNodes, numFrames, numKF, fps, playback, dur, anims[i].Size)
		}
	}
}

func dumpAnimation(file *esf.ObjFile, animInfo *esf.ObjInfo, idx int, hier *esf.HSpriteHierarchy) {
	raw := file.RawBytes(animInfo.Offset, int(animInfo.Size))
	if len(raw) < 32 {
		fmt.Printf("  --- Animation %d: TOO SMALL (%d bytes) ---\n", idx, len(raw))
		return
	}

	dictID := readI32(raw, 0)
	format := readI32(raw, 4)
	numNodes := readI32(raw, 8)
	numFrames := readI32(raw, 12)
	numKF := readI32(raw, 16)
	fps := readF32(raw, 20)
	playSpeed := readF32(raw, 24)
	playback := readI32(raw, 28)

	duration := float32(numFrames) / fps
	expectedSize := int32(32) + numKF*8 + numNodes*(4+numFrames*16)

	fmt.Printf("  --- Animation %d ---\n", idx)
	fmt.Printf("  Version=%d Size=%d ExpectedSize=%d %s\n",
		animInfo.Version, animInfo.Size, expectedSize,
		func() string {
			if expectedSize == animInfo.Size {
				return "MATCH"
			}
			return "MISMATCH"
		}())
	fmt.Printf("  DictID=0x%08x Format=%d NumNodes=%d NumFrames=%d\n",
		uint32(dictID), format, numNodes, numFrames)
	fmt.Printf("  NumKeyframes=%d FPS=%.1f PlaySpeed=%.2f PlaybackType=%d Duration=%.2fs\n",
		numKF, fps, playSpeed, playback, duration)

	if expectedSize != animInfo.Size || format != 1 || numNodes <= 0 || numFrames <= 0 {
		// Hexdump first 128 bytes for debugging
		dumpLen := len(raw)
		if dumpLen > 128 {
			dumpLen = 128
		}
		fmt.Printf("  Raw hex (first %d bytes):\n", dumpLen)
		hexdump(raw[:dumpLen], "    ")
		fmt.Println()
		return
	}

	// Read extra keyframe data
	off := 32
	if numKF > 0 {
		fmt.Printf("  Keyframe extra data (%d × 8 bytes):\n", numKF)
		for k := int32(0); k < numKF; k++ {
			a := readI32(raw, off)
			b := readI32(raw, off+4)
			bF := readF32(raw, off+4)
			fmt.Printf("    kf[%d]: int32=%d int32=%d (or float=%.4f)\n", k, a, b, bF)
			off += 8
		}
	}

	// Read per-node data
	maxNodes := int(numNodes)
	showNodes := maxNodes
	if showNodes > 10 {
		showNodes = 10
	}

	for n := 0; n < maxNodes; n++ {
		refID := readI32(raw, off)
		off += 4

		if n < showNodes {
			boneName := ""
			if hier != nil && refID >= 0 && int(refID) < len(hier.Nodes) {
				b := hier.Nodes[refID]
				boneName = fmt.Sprintf(" → bone[%d] parent=%d pos=(%.3f,%.3f,%.3f)",
					refID, b.ParentID, b.Pos.X, b.Pos.Y, b.Pos.Z)
			}
			fmt.Printf("  Node[%2d] refID=%d%s\n", n, refID, boneName)

			// Show first 3 frames and last frame
			showFrames := []int{0, 1, 2}
			if numFrames > 3 {
				showFrames = append(showFrames, int(numFrames)-1)
			}

			for _, f := range showFrames {
				if f >= int(numFrames) {
					break
				}
				foff := off + f*16
				if foff+16 > len(raw) {
					break
				}
				s0 := readI16(raw, foff+0)
				s1 := readI16(raw, foff+2)
				s2 := readI16(raw, foff+4)
				s3 := readI16(raw, foff+6)
				s4 := readI16(raw, foff+8)
				s5 := readI16(raw, foff+10)
				s6 := readI16(raw, foff+12)
				s7 := readI16(raw, foff+14)

				// Try to figure out which fields are quat/scale/pos by looking at magnitude
				qmag := math.Sqrt(float64(s0)*float64(s0) + float64(s1)*float64(s1) +
					float64(s2)*float64(s2) + float64(s3)*float64(s3))

				fmt.Printf("    f[%2d]: (%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d) |q0123|=%.0f\n",
					f, s0, s1, s2, s3, s4, s5, s6, s7, qmag)
			}
		}

		off += int(numFrames) * 16
	}

	fmt.Println()
}

func findAllOfType(info *esf.ObjInfo, typ uint16) []*esf.ObjInfo {
	var result []*esf.ObjInfo
	if info.Type == typ {
		result = append(result, info)
	}
	for _, c := range info.Children {
		result = append(result, findAllOfType(c, typ)...)
	}
	return result
}

func readI32(data []byte, offset int) int32 {
	return int32(binary.LittleEndian.Uint32(data[offset:]))
}

func readF32(data []byte, offset int) float32 {
	return math.Float32frombits(binary.LittleEndian.Uint32(data[offset:]))
}

func readI16(data []byte, offset int) int16 {
	return int16(binary.LittleEndian.Uint16(data[offset:]))
}

func hexdump(data []byte, prefix string) {
	for i := 0; i < len(data); i += 16 {
		end := i + 16
		if end > len(data) {
			end = len(data)
		}
		fmt.Printf("%s%04x: ", prefix, i)
		for j := i; j < end; j++ {
			fmt.Printf("%02x ", data[j])
		}
		for j := end; j < i+16; j++ {
			fmt.Print("   ")
		}
		fmt.Print(" |")
		for j := i; j < end; j++ {
			c := data[j]
			if c >= 32 && c < 127 {
				fmt.Printf("%c", c)
			} else {
				fmt.Print(".")
			}
		}
		fmt.Println("|")
	}
}
