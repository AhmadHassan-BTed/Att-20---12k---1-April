// dumpskeleton dumps full bone hierarchy data from CHAR.ESF CSprites.
// Shows parent chains, FixScale values, and NodeIDList for helm attachment.
package main

import (
	"fmt"
	"log"
	"math"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

const esfPath = "/home/sdg/claude-eqoa/extracted-assets/CHAR.ESF"

// All 10 race model DictIDs (from CLAUDE.md).
func i32(v uint32) int32 { return int32(v) }

var raceModels = []struct {
	DictID int32
	Label  string
}{
	// ESF DictIDs are LE-swapped from the server's BE model IDs.
	{1893243078, "HumanM"},      // 0x70D898C6 (server: 0xC698D870)
	{223572789, "DarkElfM"},     // 0x0D537335 (server: 0x3573530D)
	{1640644319, "GnomeM"},      // 0x61CA3EDF (server: 0xDF3ECA61)
	{-1001728746, "DwarfM"},     // 0xC44AD516 (server: 0x16D54AC4)
	{-1396700337, "TrollM"},     // 0xACC00B4F (server: 0x4F0BC0AC)
	{-2071956336, "BarbarianM"}, // 0x84807490 (server: 0x90748084)
	{-657100808, "HalflingM"},   // 0xD8D56FF8 (server: 0xF86FD5D8)
	{-1545912350, "EruditeM"},   // 0xA3DB3FE2 (server: 0xE23FDBA3)
	{1786918188, "OgreM"},       // 0x6A82352C (server: 0x2C35826A)
	{-1449366763, "ElfM"},       // 0xA99C6B15 (server: 0x156B9CA9)
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

	for _, rm := range raceModels {
		dumpFull(file, rm.DictID, rm.Label)
	}
}

func dumpFull(file *esf.ObjFile, dictID int32, label string) {
	obj, err := file.FindObject(dictID)
	if err != nil || obj == nil {
		fmt.Printf("=== %s (0x%08X): NOT FOUND ===\n\n", label, uint32(dictID))
		return
	}

	fmt.Printf("=== %s (Model 0x%08X) ===\n", label, uint32(dictID))

	// Get CSprite-specific data.
	cs, isCS := obj.(*esf.CSprite)

	// Get mesh bounding box.
	var placements []*esf.SpritePlacement
	switch s := obj.(type) {
	case *esf.CSprite:
		placements = s.Placements
	case *esf.HSprite:
		placements, _ = s.LoadSprites(file)
	}

	bb := newBBox()
	for _, sp := range placements {
		sprite := sp.Sprite
		if sprite == nil {
			continue
		}
		pb, err := sprite.GetPrimBuffer(file)
		if err != nil || pb == nil {
			continue
		}
		for _, vl := range pb.VertexLists {
			for _, v := range vl.Vertices {
				p := esf.Point{X: v.X, Y: v.Y, Z: v.Z}
				sp.Transform(&p)
				bb.add(p.X, p.Y, p.Z)
			}
		}
	}

	if !bb.empty {
		cx := 0.5 * (bb.minX + bb.maxX)
		cz := 0.5 * (bb.minZ + bb.maxZ)
		fmt.Printf("  Mesh bbox: X=[%.3f, %.3f] Y=[%.3f, %.3f] Z=[%.3f, %.3f]\n",
			bb.minX, bb.maxX, bb.minY, bb.maxY, bb.minZ, bb.maxZ)
		fmt.Printf("  Mesh center: (%.3f, %.3f) yBottom=%.3f height=%.3f\n",
			cx, cz, bb.minY, bb.maxY-bb.minY)
	}

	// Dump NodeIDList (named node → bone mapping).
	if isCS && len(cs.NodeIDList) > 0 {
		fmt.Printf("\n  NodeIDList (named node → bone index):\n")
		for _, entry := range cs.NodeIDList {
			name := nodeIDName(entry.NodeIndex)
			fmt.Printf("    nodeID=%d (%s) → bone %d\n", entry.NodeIndex, name, entry.BoneIndex)
		}
	}

	// Dump ASlotList (attachment slot → bone mapping).
	if isCS && len(cs.ASlotList) > 0 {
		fmt.Printf("  ASlotList (attach slot → bone index):\n")
		for _, entry := range cs.ASlotList {
			name := aslotName(entry.SlotID)
			fmt.Printf("    slot=%d (%s) → bone %d\n", entry.SlotID, name, entry.BoneIndex)
		}
	}

	// Get hierarchy.
	var hier *esf.HSpriteHierarchy
	switch s := obj.(type) {
	case *esf.CSprite:
		hier = s.Hierarchy
	case *esf.HSprite:
		hier = s.Hierarchy
	}

	if hier == nil || len(hier.Nodes) == 0 {
		fmt.Printf("  No hierarchy!\n\n")
		return
	}

	// Print bones with FixScale.
	fmt.Printf("\n  === %d BONES ===\n", len(hier.Nodes))
	fmt.Printf("  %4s %6s %10s %10s %10s   %5s   %s\n",
		"idx", "parent", "X", "Y", "Z", "scale", "FixScale")
	for i, n := range hier.Nodes {
		fs := ""
		if n.FixScale[0] != 1.0 || n.FixScale[1] != 1.0 || n.FixScale[2] != 1.0 {
			fs = fmt.Sprintf("(%.4f, %.4f, %.4f)", n.FixScale[0], n.FixScale[1], n.FixScale[2])
		}
		if n.FixScale[0] == 0 && n.FixScale[1] == 0 && n.FixScale[2] == 0 {
			fs = "(0, 0, 0) [DEFAULT]"
		}
		fmt.Printf("  [%2d] %6d   %8.3f %8.3f %8.3f   %.3f   %s\n",
			i, n.ParentID, n.Pos.X, n.Pos.Y, n.Pos.Z, n.Scale, fs)
	}

	// Find head bone via NodeIDList[8] if available.
	headIdx := -1
	if isCS {
		headIdx = int(cs.NodeBone(8))
	}
	// Fallback: highest Y near centerline.
	if headIdx < 0 {
		var headY float32
		for i, n := range hier.Nodes {
			nearCenter := n.Pos.X > -0.1 && n.Pos.X < 0.1
			if nearCenter && (headIdx == -1 || n.Pos.Y > headY) {
				headY = n.Pos.Y
				headIdx = i
			}
		}
	}

	if headIdx >= 0 {
		n := hier.Nodes[headIdx]
		fmt.Printf("\n  Head bone[%d]: pos=(%.3f, %.3f, %.3f) scale=%.3f fixScale=(%.4f, %.4f, %.4f)\n",
			headIdx, n.Pos.X, n.Pos.Y, n.Pos.Z, n.Scale, n.FixScale[0], n.FixScale[1], n.FixScale[2])
	}

	fmt.Println()
}

func nodeIDName(id int32) string {
	names := map[int32]string{
		0: "RightHand", 1: "LeftHand", 2: "TwoHand",
		3: "Chest", 4: "Back", 5: "LeftShoulder", 6: "RightShoulder",
		7: "Waist", 8: "Head/Helm",
	}
	if n, ok := names[id]; ok {
		return n
	}
	return fmt.Sprintf("unknown(%d)", id)
}

func aslotName(id int32) string {
	names := map[int32]string{
		0: "RightHand", 1: "LeftHand", 2: "TwoHand",
	}
	if n, ok := names[id]; ok {
		return n
	}
	return fmt.Sprintf("slot%d", id)
}

type bbox struct {
	minX, minY, minZ float32
	maxX, maxY, maxZ float32
	empty            bool
}

func newBBox() *bbox {
	return &bbox{
		minX: float32(math.MaxFloat32), minY: float32(math.MaxFloat32), minZ: float32(math.MaxFloat32),
		maxX: -float32(math.MaxFloat32), maxY: -float32(math.MaxFloat32), maxZ: -float32(math.MaxFloat32),
		empty: true,
	}
}

func (b *bbox) add(x, y, z float32) {
	b.empty = false
	if x < b.minX {
		b.minX = x
	}
	if y < b.minY {
		b.minY = y
	}
	if z < b.minZ {
		b.minZ = z
	}
	if x > b.maxX {
		b.maxX = x
	}
	if y > b.maxY {
		b.maxY = y
	}
	if z > b.maxZ {
		b.maxZ = z
	}
}
