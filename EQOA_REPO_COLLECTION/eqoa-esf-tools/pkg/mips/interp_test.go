package mips

import (
	"encoding/binary"
	"fmt"
	"os"
	"testing"
)

const eeDumpPath = "/home/sdg/claude-eqoa/memory-dumps/go-inspect2.eeMemory"

func loadEE(t *testing.T) []byte {
	t.Helper()
	data, err := os.ReadFile(eeDumpPath)
	if err != nil {
		t.Skipf("EE dump not found: %v", err)
	}
	return data
}

// findPrimBuffers scans raw ESF data for PrimBuffer headers with valid pbtypes.
func findPrimBuffers(data []byte, maxCount int) []struct{ offset, size int } {
	target := []byte{0x00, 0x12} // type 0x1200 LE
	var results []struct{ offset, size int }
	for i := 0; i+16 < len(data) && len(results) < maxCount; i++ {
		if data[i] != target[0] || data[i+1] != target[1] {
			continue
		}
		ver := binary.LittleEndian.Uint16(data[i+2:])
		size := binary.LittleEndian.Uint32(data[i+4:])
		if ver > 20 || size < 8 || size > 500000 || i+8+int(size) > len(data) {
			continue
		}
		// Check pbtype
		doff := i + 8
		if ver > 1 {
			doff += 4
		}
		if ver == 0 {
			results = append(results, struct{ offset, size int }{i, int(size)})
			i += 8 + int(size) - 1
			continue
		}
		if doff+4 > len(data) {
			continue
		}
		pb := int32(binary.LittleEndian.Uint32(data[doff:]))
		if pb == 0 || pb == 2 || pb == 4 {
			results = append(results, struct{ offset, size int }{i, int(size)})
			i += 8 + int(size) - 1
		}
	}
	return results
}

func TestParsePrimBuffer_TUNARIA(t *testing.T) {
	eeDump := loadEE(t)

	// Read from TUNARIA in ISO
	isoPath := "/home/sdg/claude-eqoa/EverQuest - Online Adventures - Frontiers (USA).iso"
	f, err := os.Open(isoPath)
	if err != nil {
		t.Skipf("ISO not found: %v", err)
	}
	defer f.Close()

	tunariaOffset := int64(520000 * 2048)
	chunk := make([]byte, 50*1024*1024) // 50MB
	f.ReadAt(chunk, tunariaOffset)

	pbs := findPrimBuffers(chunk, 20)
	if len(pbs) == 0 {
		t.Fatal("No valid PrimBuffers found in TUNARIA")
	}

	t.Logf("Found %d valid PrimBuffers in TUNARIA", len(pbs))

	for i, pb := range pbs {
		if i >= 5 {
			break // test first 5
		}
		objData := chunk[pb.offset : pb.offset+8+pb.size]
		ver := binary.LittleEndian.Uint16(objData[2:])

		result, reads := RunParser(eeDump, 0x004320B8, objData)

		// Count data reads (not ReadBegin/ReadEnd)
		dataReads := 0
		for _, r := range reads {
			if r.Type != "ReadBegin" && r.Type != "ReadEnd" {
				dataReads++
			}
		}

		status := "OK"
		if result < 0 {
			status = "ERROR"
		}

		t.Logf("[%d] @0x%06X ver=%d size=%d → %s, %d data reads",
			i, pb.offset, ver, pb.size, status, dataReads)

		if result < 0 {
			t.Errorf("[%d] ParsePrimBuffer returned error: %d", i, result)
			for j, r := range reads {
				if j >= 15 {
					break
				}
				t.Logf("  [%d] %s val=%d pos=0x%X", j, r.Type, r.IVal, r.Pos)
			}
			continue
		}

		// Verify we read vertex data (not just header)
		if dataReads < 20 {
			t.Errorf("[%d] Too few data reads: %d (expected vertex data)", i, dataReads)
		}

		// Show a few vertex reads for inspection
		if i == 0 {
			t.Log("First PrimBuffer trace (first 30 reads):")
			for j, r := range reads {
				if j >= 30 {
					break
				}
				switch r.Type {
				case "ReadBegin", "ReadEnd":
					fmt.Printf("  [%3d] %s %s\n", j, r.Type, r.Extra)
				case "float32":
					fmt.Printf("  [%3d] %-8s = %12.6f  @ 0x%04X\n", j, r.Type, r.FVal, r.Pos)
				default:
					fmt.Printf("  [%3d] %-8s = %12d (0x%08X)  @ 0x%04X\n",
						j, r.Type, r.IVal, uint32(r.IVal), r.Pos)
				}
			}
		}
	}
}

func TestParseCollBuffer_TUNARIA(t *testing.T) {
	eeDump := loadEE(t)

	isoPath := "/home/sdg/claude-eqoa/EverQuest - Online Adventures - Frontiers (USA).iso"
	f, err := os.Open(isoPath)
	if err != nil {
		t.Skipf("ISO not found: %v", err)
	}
	defer f.Close()

	chunk := make([]byte, 50*1024*1024)
	f.ReadAt(chunk, int64(520000*2048))

	target := []byte{0x00, 0x42} // 0x4200 LE
	var offsets []struct{ offset, size int }
	for i := 0; i+8 < len(chunk) && len(offsets) < 5; i++ {
		if chunk[i] != target[0] || chunk[i+1] != target[1] {
			continue
		}
		ver := binary.LittleEndian.Uint16(chunk[i+2:])
		size := binary.LittleEndian.Uint32(chunk[i+4:])
		if ver <= 2 && size > 8 && size < 200000 && i+8+int(size) <= len(chunk) {
			offsets = append(offsets, struct{ offset, size int }{i, int(size)})
			i += 8 + int(size) - 1
		}
	}
	if len(offsets) == 0 {
		t.Fatal("No CollBuffers found")
	}
	t.Logf("Found %d CollBuffers", len(offsets))

	for i, cb := range offsets {
		objData := chunk[cb.offset : cb.offset+8+cb.size]
		result, reads := RunParser(eeDump, 0x004343D8, objData)
		dataReads := 0
		for _, r := range reads {
			if r.Type != "ReadBegin" && r.Type != "ReadEnd" {
				dataReads++
			}
		}
		ver := binary.LittleEndian.Uint16(objData[2:])
		t.Logf("[%d] @0x%06X ver=%d size=%d → result=%d, %d data reads",
			i, cb.offset, ver, cb.size, result, dataReads)
		if result < 0 {
			t.Errorf("[%d] ParseCollBuffer returned error: %d", i, result)
		}
	}
}

func TestParseSkinPrimBuffer_CHAR(t *testing.T) {
	eeDump := loadEE(t)

	charData, err := os.ReadFile("/home/sdg/claude-eqoa/extracted-assets/CHAR.ESF")
	if err != nil {
		t.Skipf("CHAR.ESF not found: %v", err)
	}

	target := []byte{0x10, 0x12} // 0x1210 LE
	var offsets []struct{ offset, size int }
	for i := 0; i+8 < len(charData) && len(offsets) < 5; i++ {
		if charData[i] != target[0] || charData[i+1] != target[1] {
			continue
		}
		ver := binary.LittleEndian.Uint16(charData[i+2:])
		size := binary.LittleEndian.Uint32(charData[i+4:])
		if ver <= 5 && size > 100 && size < 100000 && i+8+int(size) <= len(charData) {
			offsets = append(offsets, struct{ offset, size int }{i, int(size)})
			i += 8 + int(size) - 1
		}
	}
	if len(offsets) == 0 {
		t.Fatal("No SkinPrimBuffers found")
	}
	t.Logf("Found %d SkinPrimBuffers", len(offsets))

	for i, spb := range offsets {
		objData := charData[spb.offset : spb.offset+8+spb.size]
		result, reads := RunParser(eeDump, 0x00432F98, objData)
		dataReads := 0
		for _, r := range reads {
			if r.Type != "ReadBegin" && r.Type != "ReadEnd" {
				dataReads++
			}
		}
		ver := binary.LittleEndian.Uint16(objData[2:])
		t.Logf("[%d] @0x%06X ver=%d size=%d → result=%d, %d data reads",
			i, spb.offset, ver, spb.size, result, dataReads)
		if result < 0 {
			t.Errorf("[%d] ParseSkinPrimBuffer returned error: %d", i, result)
			for j, r := range reads {
				if j >= 15 { break }
				t.Logf("  [%d] %s val=%d pos=0x%X", j, r.Type, r.IVal, r.Pos)
			}
		}
	}
}
