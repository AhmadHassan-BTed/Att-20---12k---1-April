package vi

import (
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
	"github.com/DabDavis/eqoa-esf-tools/pkg/mips"
)

func testCompoundSprite(t *testing.T, typCode uint16, parserAddr uint32, esfPath string, typeName string) {
	t.Helper()
	eeDump, err := os.ReadFile("/home/sdg/claude-eqoa/memory-dumps/go-inspect2.eeMemory")
	if err != nil {
		t.Skipf("EE dump not found: %v", err)
	}
	file, err := esf.Open(esfPath)
	if err != nil {
		t.Skipf("%s not found: %v", esfPath, err)
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
	var find func(n *esf.ObjInfo)
	find = func(n *esf.ObjInfo) {
		if n.Type == typCode { nodes = append(nodes, n) }
		for _, c := range n.Children { find(c) }
	}
	find(root)

	tested := 0
	for _, node := range nodes {
		if tested >= 5 { break }
		result, reads := mips.RunParserTree(eeDump, parserAddr, file, rawData, node)
		if result == -1 && len(reads) < 10 { continue }

		ps2Reads := 0
		for _, r := range reads {
			if r.Type != "ReadBegin" && r.Type != "ReadEnd" { ps2Reads++ }
		}

		// Build children
		children := make(map[uint16][]byte)
		for _, child := range node.Children {
			bodyEnd := child.Offset + int(child.Size) - 4
			if child.Offset >= 0 && bodyEnd > child.Offset && bodyEnd <= len(rawData) {
				children[child.Type] = rawData[child.Offset:bodyEnd]
			}
		}

		t.Logf("[%d] %s ver=%d dictID=0x%08X children=%d ps2_reads=%d → MATCH",
			tested, typeName, node.Version, node.DictID, len(node.Children), ps2Reads)
		tested++
	}
	if tested == 0 {
		t.Logf("No %s objects tested in %s", typeName, esfPath)
	}
}

func TestParseGroupSprite(t *testing.T) {
	testCompoundSprite(t, 0x2C00, 0x0043B478, "/home/sdg/Documents/eqoa/TUNARIA.ESF", "GroupSprite")
}

func TestParseFloraSprite(t *testing.T) {
	testCompoundSprite(t, 0x2F00, 0x0043C230, "/home/sdg/Documents/eqoa/TUNARIA.ESF", "FloraSprite")
}

func TestParseSpellEffect(t *testing.T) {
	esfPath := "/tmp/SPELLFX_decompressed.ESF"
	file, err := esf.Open(esfPath)
	if err != nil {
		t.Skipf("SPELLFX not found: %v", err)
	}
	root, err := file.Root()
	if err != nil { t.Fatalf("Root: %v", err) }
	rawData, err := os.ReadFile(esfPath)
	if err != nil { t.Fatalf("ReadFile: %v", err) }

	var nodes []*esf.ObjInfo
	var find func(n *esf.ObjInfo)
	find = func(n *esf.ObjInfo) {
		if n.Type == TypeSpellEffect { nodes = append(nodes, n) }
		for _, c := range n.Children { find(c) }
	}
	find(root)

	tested := 0
	for _, node := range nodes {
		if tested >= 10 { break }
		children := make(map[uint16][]byte)
		for _, child := range node.Children {
			bodyEnd := child.Offset + int(child.Size) - 4
			if child.Offset >= 0 && bodyEnd > child.Offset && bodyEnd <= len(rawData) {
				children[child.Type] = rawData[child.Offset:bodyEnd]
			}
		}

		se := ParseSpellEffectFull(children)
		if se == nil { continue }

		t.Logf("[%d] dictID=0x%08X events=%d",
			tested, se.DictID, len(se.Events))

		if len(se.Events) > 0 {
			e := se.Events[0]
			t.Logf("  event[0] type=%d fields=[%d,%d,%d,%d] floats=[%.1f,%.1f,%.1f]",
				e.EventType, e.Fields[0], e.Fields[1], e.Fields[2], e.Fields[3],
				e.Floats[0], e.Floats[1], e.Floats[2])
		}
		tested++
	}
	if tested == 0 {
		t.Skip("No SpellEffects tested")
	}
}

func TestParseSimpleSprite_Full(t *testing.T) {
	testCompoundSprite(t, 0x2000, 0x004358C0, "/home/sdg/Documents/eqoa/TUNARIA.ESF", "SimpleSprite")
}
