package vi

import (
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
	"github.com/DabDavis/eqoa-esf-tools/pkg/mips"
)

func TestParseHSpriteAnim_MatchesPS2(t *testing.T) {
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

	// Find HSpriteAnim objects in tree
	var nodes []*esf.ObjInfo
	var findNodes func(n *esf.ObjInfo)
	findNodes = func(n *esf.ObjInfo) {
		if n.Type == 0x2600 {
			nodes = append(nodes, n)
		}
		for _, c := range n.Children {
			findNodes(c)
		}
	}
	findNodes(root)

	if len(nodes) == 0 {
		t.Fatal("No HSpriteAnim objects found")
	}

	// Read raw ESF data
	rawData, err := os.ReadFile(esfPath)
	if err != nil {
		t.Fatalf("ReadFile: %v", err)
	}

	tested := 0
	for i, node := range nodes {
		if tested >= 10 {
			break
		}

		// Run PS2 MIPS interpreter
		result, ps2Reads := mips.RunParserTree(eeDump, 0x00436560, file, rawData, node)
		if result < 0 {
			continue
		}

		// Count PS2 data reads (exclude ReadBegin/ReadEnd)
		ps2DataReads := 0
		for _, r := range ps2Reads {
			if r.Type != "ReadBegin" && r.Type != "ReadEnd" {
				ps2DataReads++
			}
		}

		// Build raw object data for vi parser.
		// Go ObjInfo: Offset is past the 12-byte header (type+ver+size+numSubObjects).
		// Size includes numSubObjects(4) + body.
		// VI parsers expect: [type(2)+ver(2)+size(4)] + body, pos=8 skips to body.
		// Construct: 8-byte header from rawData[Offset-12..Offset-4] + body from rawData[Offset..Offset+Size-4]
		hdrStart := node.Offset - 12
		bodyEnd := node.Offset + int(node.Size) - 4 // Size includes 4 bytes of numSubObjects
		if hdrStart < 0 || bodyEnd > len(rawData) {
			continue
		}
		// Concatenate header (8 bytes) + body
		objData := make([]byte, 8+(bodyEnd-node.Offset))
		copy(objData[0:8], rawData[hdrStart:hdrStart+8])
		copy(objData[8:], rawData[node.Offset:bodyEnd])

		// Parse with vi
		viAnim, err := ParseHSpriteAnim(objData)
		if err != nil {
			t.Errorf("[%d] vi parse failed: %v", i, err)
			continue
		}

		// Count vi reads: header + numNodes × (1 refID + numFrames × readsPerFrame)
		var readsPerFrame int
		switch viAnim.Format {
		case 0:
			readsPerFrame = 8 // 8×float32
		case 1, 2:
			readsPerFrame = 8 // 8×int16
		}

		// Header reads depend on version
		headerReads := 3 // format + numNodes + numFrames (unconditional)
		if node.Version > 1 {
			headerReads++ // dictID
		}
		if node.Version >= 3 {
			headerReads++ // numKeyframes
		}
		headerReads++ // fps
		if node.Version >= 1 {
			headerReads += 2 // playSpeed + playbackType
		}

		viDataReads := headerReads
		viTotalFrames := 0
		for n := int32(0); n < viAnim.NumNodes; n++ {
			viDataReads++ // refID
			viDataReads += int(viAnim.NumFrames) * readsPerFrame
			viTotalFrames += int(viAnim.NumFrames)
		}

		if viDataReads != ps2DataReads {
			t.Errorf("[%d] read count mismatch: vi=%d ps2=%d (nodes=%d frames=%d format=%d ver=%d)",
				i, viDataReads, ps2DataReads, viAnim.NumNodes, viAnim.NumFrames, viAnim.Format, node.Version)
			continue
		}

		t.Logf("[%d] ver=%d format=%d nodes=%d frames=%d reads=%d → MATCH",
			tested, node.Version, viAnim.Format, viAnim.NumNodes, viAnim.NumFrames, ps2DataReads)
		tested++
	}

	if tested == 0 {
		t.Fatal("No HSpriteAnim objects tested")
	}
}
