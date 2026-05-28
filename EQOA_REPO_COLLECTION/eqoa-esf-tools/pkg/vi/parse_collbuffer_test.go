package vi

import (
	"encoding/binary"
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/mips"
)

func TestParseCollBuffer_MatchesPS2(t *testing.T) {
	eeDump, err := os.ReadFile("/home/sdg/claude-eqoa/memory-dumps/go-inspect2.eeMemory")
	if err != nil {
		t.Skipf("EE dump not found: %v", err)
	}

	f, err := os.Open("/home/sdg/claude-eqoa/EverQuest - Online Adventures - Frontiers (USA).iso")
	if err != nil {
		t.Skipf("ISO not found: %v", err)
	}
	defer f.Close()
	chunk := make([]byte, 50*1024*1024)
	f.ReadAt(chunk, 520000*2048)

	// Find valid CollBuffers (type=0x4200)
	target := []byte{0x00, 0x42}
	tested := 0
	for i := 0; i+8 < len(chunk) && tested < 10; i++ {
		if chunk[i] != target[0] || chunk[i+1] != target[1] {
			continue
		}
		ver := binary.LittleEndian.Uint16(chunk[i+2:])
		size := binary.LittleEndian.Uint32(chunk[i+4:])
		if ver > 20 || size < 10 || size > 50000 || i+8+int(size) > len(chunk) {
			continue
		}

		objData := chunk[i : i+8+int(size)]

		// Parse with PS2 MIPS interpreter
		ps2Result, ps2Reads := mips.RunParser(eeDump, 0x004343D8, objData)
		if ps2Result < 0 {
			i += 8 + int(size)
			continue
		}

		// Parse with our vi package
		viCB, err := ParseCollBuffer(objData)
		if err != nil {
			t.Errorf("[%d] vi parse failed: %v", tested, err)
			continue
		}

		// Count PS2 data reads
		ps2DataReads := 0
		for _, r := range ps2Reads {
			if r.Type != "ReadBegin" && r.Type != "ReadEnd" {
				ps2DataReads++
			}
		}

		// Count vi vertices
		viVerts := 0
		for _, face := range viCB.Faces {
			viVerts += len(face.Vertices)
		}

		// Verify: same number of faces
		if int32(len(viCB.Faces)) != viCB.NumVertexGroups {
			t.Errorf("[%d] face count mismatch: vi=%d expected=%d",
				tested, len(viCB.Faces), viCB.NumVertexGroups)
		}

		// Verify vertex count matches PS2 read count
		// PS2 CollBuffer reads:
		//   header: cbtype(1) + numPG(1) + numVG(1) + unk(1) + packing(1) = 5 (for ver>=2)
		//   per group: nverts(1) + primg(1) + list(1) = 3
		//   per vertex: depends on cbtype (3 for float, 3 for packed, 4 for vg, 5 for vgf)
		headerReads := 5
		if ver == 0 {
			headerReads = 3 // no cbtype, no packing
		} else if ver == 1 {
			headerReads = 4 // no packing
		}
		groupHeaders := int(viCB.NumVertexGroups) * 3
		vertexReads := ps2DataReads - headerReads - groupHeaders

		var readsPerVertex int
		switch viCB.Type {
		case CollTypeFloat:
			readsPerVertex = 3
		case CollTypePacked:
			readsPerVertex = 3
		case CollTypePackedVG:
			readsPerVertex = 4
		case CollTypePackedVGF:
			readsPerVertex = 5
		default:
			readsPerVertex = 3
		}

		expectedVerts := 0
		if readsPerVertex > 0 {
			expectedVerts = vertexReads / readsPerVertex
		}

		if viVerts != expectedVerts {
			t.Errorf("[%d] vertex count mismatch: vi=%d ps2_expected=%d (ps2_reads=%d, header=%d, groups=%d)",
				tested, viVerts, expectedVerts, ps2DataReads, headerReads, groupHeaders)
		}

		t.Logf("[%d] @0x%06X: ver=%d type=%d faces=%d verts=%d ps2_reads=%d → MATCH",
			tested, i, ver, viCB.Type, len(viCB.Faces), viVerts, ps2DataReads)
		tested++
		i += 8 + int(size) - 1
	}

	if tested == 0 {
		t.Fatal("No CollBuffers tested")
	}
}
