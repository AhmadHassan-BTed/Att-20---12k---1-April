package vi

import (
	"encoding/binary"
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/mips"
)

func TestParsePrimBuffer_MatchesPS2(t *testing.T) {
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

	// Find valid PrimBuffers (pbtype=0, ver=2)
	target := []byte{0x00, 0x12}
	tested := 0
	for i := 0; i+8 < len(chunk) && tested < 5; i++ {
		if chunk[i] != target[0] || chunk[i+1] != target[1] {
			continue
		}
		ver := binary.LittleEndian.Uint16(chunk[i+2:])
		size := binary.LittleEndian.Uint32(chunk[i+4:])
		if ver != 2 || size < 100 || size > 5000 || i+8+int(size) > len(chunk) {
			continue
		}
		doff := i + 12 // after header + dictID
		pb := int32(binary.LittleEndian.Uint32(chunk[doff:]))
		if pb != 0 {
			i++
			continue
		}

		objData := chunk[i : i+8+int(size)]

		// Parse with PS2 MIPS interpreter
		ps2Result, ps2Reads := mips.RunParser(eeDump, 0x004320B8, objData)
		if ps2Result < 0 {
			i += 8 + int(size)
			continue
		}

		// Parse with our vi package
		viPB, err := ParsePrimBuffer(objData)
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
		for _, face := range viPB.Faces {
			viVerts += len(face.Vertices)
		}

		// Verify: same number of faces
		if int32(len(viPB.Faces)) != viPB.NumFaces {
			t.Errorf("[%d] face count mismatch: vi=%d expected=%d",
				tested, len(viPB.Faces), viPB.NumFaces)
		}

		// Verify: PS2 header reads match vi struct fields
		// PS2 read[1]=dictID, [2]=type, [3]=nmats, [4]=nfaces, [5]=unk, [6]=p1, [7]=p2, [8]=p3
		ps2Type := int32(0)
		ps2Nfaces := int32(0)
		di := 0
		for _, r := range ps2Reads {
			if r.Type == "ReadBegin" || r.Type == "ReadEnd" {
				continue
			}
			di++
			switch di {
			case 2:
				ps2Type = int32(r.IVal)
			case 4:
				ps2Nfaces = int32(r.IVal)
			}
		}

		if viPB.Type != ps2Type {
			t.Errorf("[%d] type mismatch: vi=%d ps2=%d", tested, viPB.Type, ps2Type)
		}
		if viPB.NumFaces != ps2Nfaces {
			t.Errorf("[%d] nfaces mismatch: vi=%d ps2=%d", tested, viPB.NumFaces, ps2Nfaces)
		}

		// Verify vertex count matches PS2 read count
		// PS2: 8 header reads + nfaces×2 (nverts+mat) + total_verts × 12 (per vertex)
		expectedVerts := (ps2DataReads - 8 - int(viPB.NumFaces)*2) / 12
		if viVerts != expectedVerts {
			t.Errorf("[%d] vertex count mismatch: vi=%d ps2_expected=%d (ps2_reads=%d)",
				tested, viVerts, expectedVerts, ps2DataReads)
		}

		t.Logf("[%d] @0x%06X: type=%d faces=%d verts=%d ps2_reads=%d → MATCH",
			tested, i, viPB.Type, len(viPB.Faces), viVerts, ps2DataReads)
		tested++
		i += 8 + int(size) - 1
	}

	if tested == 0 {
		t.Fatal("No PrimBuffers tested")
	}
}
