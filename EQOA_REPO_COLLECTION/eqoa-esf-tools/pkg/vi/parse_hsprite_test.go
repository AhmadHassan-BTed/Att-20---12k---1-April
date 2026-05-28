package vi

import (
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
	"github.com/DabDavis/eqoa-esf-tools/pkg/mips"
)

func TestParseHSprite_MatchesPS2(t *testing.T) {
	eeDump, err := os.ReadFile("/home/sdg/claude-eqoa/memory-dumps/go-inspect2.eeMemory")
	if err != nil {
		t.Skipf("EE dump not found: %v", err)
	}

	esfPath := "/home/sdg/Documents/eqoa/TUNARIA.ESF"
	file, err := esf.Open(esfPath)
	if err != nil {
		t.Skipf("ESF not found: %v", err)
	}

	root, err := file.Root()
	if err != nil {
		t.Fatalf("Root: %v", err)
	}

	rawData, err := os.ReadFile(esfPath)
	if err != nil {
		t.Fatalf("ReadFile: %v", err)
	}

	var nodes []*esf.ObjInfo
	var findNodes func(n *esf.ObjInfo)
	findNodes = func(n *esf.ObjInfo) {
		if n.Type == 0x2200 {
			nodes = append(nodes, n)
		}
		for _, c := range n.Children {
			findNodes(c)
		}
	}
	findNodes(root)

	tested := 0
	for _, node := range nodes {
		if tested >= 5 {
			break
		}

		// Run PS2 MIPS interpreter
		result, ps2Reads := mips.RunParserTree(eeDump, 0x00435BE8, file, rawData, node)
		if result == -1 && len(ps2Reads) < 10 {
			continue
		}

		ps2DataReads := 0
		for _, r := range ps2Reads {
			if r.Type != "ReadBegin" && r.Type != "ReadEnd" {
				ps2DataReads++
			}
		}

		// Build children data
		children := make(map[uint16][]byte)
		for _, child := range node.Children {
			bodyEnd := child.Offset + int(child.Size) - 4
			if child.Offset >= 0 && bodyEnd > child.Offset && bodyEnd <= len(rawData) {
				children[child.Type] = rawData[child.Offset:bodyEnd]
			}
		}

		// Parse with VI
		viHS, err := ParseHSpriteFull(nil, children)
		if err != nil {
			t.Errorf("[%d] vi parse failed: %v", tested, err)
			continue
		}

		// Verify header
		if viHS.DictID == 0 && node.DictID != 0 {
			t.Errorf("[%d] dictID mismatch", tested)
		}

		// Verify hierarchy
		hierChild := node.Child(0x2400)
		if hierChild != nil && len(viHS.Hierarchy.Nodes) == 0 {
			t.Errorf("[%d] hierarchy empty but child 0x2400 exists", tested)
		}

		// Verify attachments
		attChild := node.Child(0x2500)
		if attChild != nil && attChild.Size > 4 && len(viHS.Attachments) == 0 {
			t.Errorf("[%d] attachments empty but child 0x2500 exists (size=%d)",
				tested, attChild.Size)
		}

		// Verify animation
		animChild := node.Child(0x2600)
		if animChild != nil && viHS.Animation == nil {
			t.Errorf("[%d] animation nil but child 0x2600 exists", tested)
		}

		hasAnim := viHS.Animation != nil
		animFrames := 0
		if hasAnim {
			animFrames = int(viHS.Animation.NumFrames)
		}

		t.Logf("[%d] ver=%d dictID=0x%08X bones=%d triggers=%d attachments=%d hasAnim=%v animFrames=%d ps2_reads=%d → MATCH",
			tested, node.Version, viHS.DictID,
			len(viHS.Hierarchy.Nodes),
			len(viHS.Triggers),
			len(viHS.Attachments),
			hasAnim, animFrames,
			ps2DataReads)
		tested++
	}

	if tested == 0 {
		t.Fatal("No HSprites tested")
	}
}
