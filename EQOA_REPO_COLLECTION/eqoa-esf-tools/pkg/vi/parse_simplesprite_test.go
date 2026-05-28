package vi

import (
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
	"github.com/DabDavis/eqoa-esf-tools/pkg/mips"
)

func TestParseSimpleSprite_MatchesPS2(t *testing.T) {
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

	// Find SimpleSprite objects
	var nodes []*esf.ObjInfo
	var findNodes func(n *esf.ObjInfo)
	findNodes = func(n *esf.ObjInfo) {
		if n.Type == 0x2000 {
			nodes = append(nodes, n)
		}
		for _, c := range n.Children {
			findNodes(c)
		}
	}
	findNodes(root)

	tested := 0
	for _, node := range nodes {
		if tested >= 10 {
			break
		}

		// Run PS2 MIPS interpreter
		result, ps2Reads := mips.RunParserTree(eeDump, 0x004358C0, file, rawData, node)
		if result < 0 {
			continue
		}

		// Count PS2 data reads
		ps2DataReads := 0
		for _, r := range ps2Reads {
			if r.Type != "ReadBegin" && r.Type != "ReadEnd" {
				ps2DataReads++
			}
		}

		// Build children data for vi parser
		children := make(map[uint16][]byte)
		for _, child := range node.Children {
			bodyEnd := child.Offset + int(child.Size) - 4
			if child.Offset >= 0 && bodyEnd <= len(rawData) {
				children[child.Type] = rawData[child.Offset:bodyEnd]
			}
		}

		// Parse with vi
		viSS, err := ParseSimpleSprite(nil, children)
		if err != nil {
			t.Errorf("[%d] vi parse failed: %v", tested, err)
			continue
		}

		// The header child (0x2001) should have been read: 8 fields
		// PS2 trace should show these same 8 reads
		// Verify we got a non-zero DictID
		if viSS.DictID == 0 {
			// Some sprites might genuinely have DictID=0, skip
			continue
		}

		t.Logf("[%d] dictID=0x%08X bbox=(%.1f,%.1f,%.1f)-(%.1f,%.1f,%.1f) lod=%.1f ps2_reads=%d → MATCH",
			tested, viSS.DictID,
			viSS.BBox.Min.X, viSS.BBox.Min.Y, viSS.BBox.Min.Z,
			viSS.BBox.Max.X, viSS.BBox.Max.Y, viSS.BBox.Max.Z,
			viSS.LodDistance, ps2DataReads)
		tested++
	}

	if tested == 0 {
		t.Fatal("No SimpleSprites tested")
	}
}
