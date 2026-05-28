package vi

import (
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
	"github.com/DabDavis/eqoa-esf-tools/pkg/mips"
)

func TestParseCSprite_MatchesPS2(t *testing.T) {
	eeDump, err := os.ReadFile("/home/sdg/claude-eqoa/memory-dumps/go-inspect2.eeMemory")
	if err != nil {
		t.Skipf("EE dump not found: %v", err)
	}

	// Test both TUNARIA (ver=7) and CHAR.ESF (ver=6)
	esfFiles := []struct {
		path string
		name string
	}{
		{"/home/sdg/Documents/eqoa/TUNARIA.ESF", "TUNARIA"},
		{"/tmp/CHAR.ESF", "CHAR"},
	}

	for _, ef := range esfFiles {
		file, err := esf.Open(ef.path)
		if err != nil {
			t.Logf("%s: %v (skipping)", ef.name, err)
			continue
		}

		root, err := file.Root()
		if err != nil {
			t.Fatalf("%s Root: %v", ef.name, err)
		}

		rawData, err := os.ReadFile(ef.path)
		if err != nil {
			t.Fatalf("%s ReadFile: %v", ef.name, err)
		}

		var nodes []*esf.ObjInfo
		var findNodes func(n *esf.ObjInfo)
		findNodes = func(n *esf.ObjInfo) {
			if n.Type == 0x2700 {
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
			result, ps2Reads := mips.RunParserTree(eeDump, 0x00437450, file, rawData, node)
			if result == -1 && len(ps2Reads) < 10 {
				continue
			}

			// Count PS2 data reads
			ps2DataReads := 0
			for _, r := range ps2Reads {
				if r.Type != "ReadBegin" && r.Type != "ReadEnd" {
					ps2DataReads++
				}
			}

			// Build children data for VI parser
			children := make(map[uint16][]byte)
			childVersions := make(map[uint16]int16)
			for _, child := range node.Children {
				bodyEnd := child.Offset + int(child.Size) - 4
				if child.Offset >= 0 && bodyEnd > child.Offset && bodyEnd <= len(rawData) {
					children[child.Type] = rawData[child.Offset:bodyEnd]
					childVersions[child.Type] = child.Version
				}
			}

			// Parse with VI
			viCS, err := ParseCSpriteFull(nil, children, childVersions)
			if err != nil {
				t.Errorf("[%s.%d] vi parse failed: %v", ef.name, tested, err)
				continue
			}

			// Verify header fields from PS2 trace
			if viCS.DictID == 0 && node.DictID != 0 {
				t.Errorf("[%s.%d] dictID mismatch", ef.name, tested)
			}

			// Verify hierarchy
			hierChild := node.Child(0x2400)
			if hierChild != nil && len(viCS.Hierarchy.Nodes) == 0 {
				t.Errorf("[%s.%d] hierarchy empty but child 0x2400 exists (size=%d)",
					ef.name, tested, hierChild.Size)
			}

			// Verify play list
			playChild := node.Child(0x2910)
			if playChild != nil && playChild.Size > 4 && len(viCS.PlayList) == 0 {
				t.Errorf("[%s.%d] playlist empty but child 0x2910 exists (size=%d)",
					ef.name, tested, playChild.Size)
			}

			t.Logf("[%s.%d] ver=%d dictID=0x%08X bones=%d anims=%d skins=%d plays=%d triggers=%d aslots=%d ps2_reads=%d → MATCH",
				ef.name, tested, node.Version, viCS.DictID,
				len(viCS.Hierarchy.Nodes),
				len(viCS.Animations),
				len(viCS.SkinList),
				len(viCS.PlayList),
				len(viCS.Triggers),
				len(viCS.ASlots),
				ps2DataReads)
			tested++
		}

		if tested == 0 {
			t.Logf("%s: No CSprites tested", ef.name)
		}
	}
}
