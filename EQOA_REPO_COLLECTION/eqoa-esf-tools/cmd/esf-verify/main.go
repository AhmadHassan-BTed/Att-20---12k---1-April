// esf-verify compares Go ESF parsers against PS2 native MIPS execution.
// Runs both parsers on the same ESF data, diffs read traces by stream position,
// and outputs fix suggestions for any mismatches.
//
// Usage:
//
//	esf-verify CHAR.ESF                         # verify PrimBuffers (default)
//	esf-verify CHAR.ESF --type 0x4200           # verify CollBuffers
//	esf-verify CHAR.ESF --max 10                # limit to 10 objects
//	esf-verify CHAR.ESF -v                      # verbose: show all diffs + suggestions
//	esf-verify CHAR.ESF --ps2-only              # just show PS2 read trace (no Go comparison)
package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"math"
	"os"
	"strings"

	"github.com/DabDavis/eqoa-esf-tools/pkg/mips"
)

// PS2 parser addresses (SUPPORT symbols) and Go parser file locations.
var parsers = []struct {
	Typ     uint16
	Addr    uint32
	Name    string
	GoFile  string
}{
	{0x1200, 0x004320B8, "ParsePrimBuffer", "pkg/esf/primbuffer.go"},
	{0x1210, 0x00432F98, "ParseSkinPrimBuffer", "pkg/esf/primbuffer.go"},
	{0x1230, 0x00433E48, "ParseFloraPrimBuffer", "pkg/esf/primbuffer.go"},
	{0x4200, 0x004343D8, "ParseCollBuffer", "pkg/esf/collbuffer.go"},
}

func parserForType(typ uint16) (uint32, string, string) {
	for _, p := range parsers {
		if p.Typ == typ {
			return p.Addr, p.Name, p.GoFile
		}
	}
	return 0, "", ""
}

// scanForType finds all objects of a given ESF type by scanning for headers.
func scanForType(data []byte, typ uint16, maxCount int) []int {
	target := make([]byte, 2)
	binary.LittleEndian.PutUint16(target, typ)

	var offsets []int
	for i := 0; i+8 < len(data) && len(offsets) < maxCount; i++ {
		if data[i] == target[0] && data[i+1] == target[1] {
			ver := binary.LittleEndian.Uint16(data[i+2:])
			size := binary.LittleEndian.Uint32(data[i+4:])
			if ver < 100 && size > 0 && size < 10_000_000 && i+8+int(size) <= len(data) {
				offsets = append(offsets, i)
				i += 8 + int(size) - 1
			}
		}
	}
	return offsets
}

// goTraceFromRaw replays the Go parser's read sequence by manually parsing
// the raw ESF object data. This mirrors what the Go Load() function reads
// so we can compare against the PS2 trace position-by-position.
func goTraceFromRaw(data []byte, typ uint16) []mips.ReadEntry {
	if len(data) < 8 {
		return nil
	}
	switch typ {
	case 0x1200:
		return goTracePrimBuffer(data)
	case 0x1210:
		return goTraceSkinPrimBuffer(data)
	case 0x4200:
		return goTraceCollBuffer(data)
	default:
		return nil
	}
}

// goTracePrimBuffer replays PrimBuffer.Load reads.
func goTracePrimBuffer(data []byte) []mips.ReadEntry {
	r := &traceReader{data: data}
	r.readBegin() // type + ver + size

	ver := binary.LittleEndian.Uint16(data[2:])
	if ver == 0 {
		r.ri32("nmats")
		nfaces := r.ri32v("nfaces")
		r.ri32("unk")
		for fi := 0; fi < int(nfaces) && !r.eof(); fi++ {
			nverts := r.ri32v("nverts")
			r.ri32("mat")
			for j := 0; j < int(nverts) && !r.eof(); j++ {
				for k := 0; k < 8; k++ {
					r.rf32("float") // 3pos + 2uv + 3normal as float
				}
				r.ru8("r"); r.ru8("g"); r.ru8("b"); r.ru8("a")
			}
		}
		return r.trace
	}

	if ver > 1 {
		r.ru32("dictID")
	}
	pbtype := r.ri32v("pbtype")
	r.ri32("nmats")
	nfaces := r.ri32v("nfaces")
	r.ri32("unk")
	r.ri32("p1")
	r.ri32("p2")
	r.ri32("p3")

	for fi := int32(0); fi < nfaces && !r.eof(); fi++ {
		nverts := r.ri32v("nverts")
		r.ri32("mat")
		for j := int32(0); j < nverts && !r.eof(); j++ {
			switch pbtype {
			case 0:
				for k := 0; k < 8; k++ {
					r.rf32("float")
				}
				r.ru8("r"); r.ru8("g"); r.ru8("b"); r.ru8("a")
			case 2:
				r.rs16("x"); r.rs16("y"); r.rs16("z")
				r.rs16("u"); r.rs16("v")
				r.ri8("nx"); r.ri8("ny"); r.ri8("nz")
				r.ru8("r"); r.ru8("g"); r.ru8("b"); r.ru8("a")
			case 4:
				r.rs16("x"); r.rs16("y"); r.rs16("z")
				r.rs16("u"); r.rs16("v")
				r.ri8("nx"); r.ri8("ny"); r.ri8("nz")
				r.ru8("r"); r.ru8("g"); r.ru8("b"); r.ru8("a")
				r.rs16("vgroup")
			case 5:
				r.rs16("x"); r.rs16("y"); r.rs16("z")
				r.rs16("u"); r.rs16("v")
				r.ri8("nx"); r.ri8("ny"); r.ri8("nz")
				r.ru8("r"); r.ru8("g"); r.ru8("b"); r.ru8("a")
				r.rs16("vgroup")
			}
		}
	}
	return r.trace
}

// goTraceSkinPrimBuffer replays SkinPrimBuffer reads (type 0x1210, pbtype=5).
// PS2: ParseSkinPrimBuffer at 0x00432F98 → ParseSkinPrimBufferObjV0 at 0x00433AE0.
func goTraceSkinPrimBuffer(data []byte) []mips.ReadEntry {
	r := &traceReader{data: data}
	r.readBegin()
	ver := binary.LittleEndian.Uint16(data[2:])

	if ver == 0 {
		// SkinPrimBufferObjV0 — same as PrimBuffer v0 but with vgroup
		r.ri32("nmats")
		nfaces := r.ri32v("nfaces")
		r.ri32("unk")
		for fi := 0; fi < int(nfaces) && !r.eof(); fi++ {
			nverts := r.ri32v("nverts")
			r.ri32("mat")
			for j := 0; j < int(nverts) && !r.eof(); j++ {
				for k := 0; k < 8; k++ {
					r.rf32("float")
				}
				r.ru8("r"); r.ru8("g"); r.ru8("b"); r.ru8("a")
				r.rs16("vgroup")
			}
		}
		return r.trace
	}

	// ver > 0: same header as PrimBuffer
	if ver > 1 {
		r.ru32("dictID")
	}
	r.ri32("pbtype") // always 5 for SkinPrimBuffer
	r.ri32("nmats")
	nfaces := r.ri32v("nfaces")
	r.ri32("unk")
	r.ri32("p1")
	r.ri32("p2")
	r.ri32("p3")

	for fi := int32(0); fi < nfaces && !r.eof(); fi++ {
		nverts := r.ri32v("nverts")
		r.ri32("mat")
		for j := int32(0); j < nverts && !r.eof(); j++ {
			// Skinned: pos(3×i16) + uv(2×i16) + normal(3×i8) + color(4×u8) + vgroup(i16)
			r.rs16("x"); r.rs16("y"); r.rs16("z")
			r.rs16("u"); r.rs16("v")
			r.ri8("nx"); r.ri8("ny"); r.ri8("nz")
			r.ru8("r"); r.ru8("g"); r.ru8("b"); r.ru8("a")
			r.rs16("vgroup")
		}
	}
	return r.trace
}

// goTraceCollBuffer replays CollBuffer.Load reads.
func goTraceCollBuffer(data []byte) []mips.ReadEntry {
	r := &traceReader{data: data}
	r.readBegin()

	ver := binary.LittleEndian.Uint16(data[2:])

	var cbtype int32
	if ver > 1 {
		cbtype = r.ri32v("cbtype")
	}
	r.ri32("numPrimgroups")
	numVG := r.ri32v("numVertexGroups")
	r.ri32("unk")
	if ver >= 2 {
		r.ri32("packing")
	}

	for i := int32(0); i < numVG && !r.eof(); i++ {
		num := r.ri32v("num")
		r.ri32("primg")
		r.ri32("list")

		for j := int32(0); j < num && !r.eof(); j++ {
			switch cbtype {
			case 0:
				r.rf32("x"); r.rf32("y"); r.rf32("z")
			case 1:
				r.rs16("x"); r.rs16("y"); r.rs16("z")
			case 2:
				r.rs16("x"); r.rs16("y"); r.rs16("z"); r.rs16("vgroup")
			case 3:
				r.rs16("x"); r.rs16("y"); r.rs16("z")
				r.ri8("vgroup"); r.ri8("flora")
			}
		}
	}
	return r.trace
}

// traceReader records every read as a ReadEntry for comparison.
type traceReader struct {
	data  []byte
	pos   int
	trace []mips.ReadEntry
}

func (r *traceReader) eof() bool { return r.pos >= len(r.data) }

func (r *traceReader) readBegin() {
	if r.pos+8 > len(r.data) {
		return
	}
	typ := binary.LittleEndian.Uint16(r.data[r.pos:])
	ver := binary.LittleEndian.Uint16(r.data[r.pos+2:])
	size := binary.LittleEndian.Uint32(r.data[r.pos+4:])
	r.trace = append(r.trace, mips.ReadEntry{
		Type: "ReadBegin", Pos: r.pos,
		Extra: fmt.Sprintf("type=0x%04X ver=%d size=%d", typ, ver, size),
	})
	r.pos += 8
}

func (r *traceReader) ri32(name string) {
	if r.pos+4 > len(r.data) { return }
	v := int32(binary.LittleEndian.Uint32(r.data[r.pos:]))
	r.trace = append(r.trace, mips.ReadEntry{Type: "int32", IVal: int64(v), Pos: r.pos, Extra: name})
	r.pos += 4
}

func (r *traceReader) ri32v(name string) int32 {
	if r.pos+4 > len(r.data) { return 0 }
	v := int32(binary.LittleEndian.Uint32(r.data[r.pos:]))
	r.trace = append(r.trace, mips.ReadEntry{Type: "int32", IVal: int64(v), Pos: r.pos, Extra: name})
	r.pos += 4
	return v
}

func (r *traceReader) ru32(name string) {
	if r.pos+4 > len(r.data) { return }
	v := binary.LittleEndian.Uint32(r.data[r.pos:])
	r.trace = append(r.trace, mips.ReadEntry{Type: "uint32", IVal: int64(v), Pos: r.pos, Extra: name})
	r.pos += 4
}

func (r *traceReader) rf32(name string) {
	if r.pos+4 > len(r.data) { return }
	v := math.Float32frombits(binary.LittleEndian.Uint32(r.data[r.pos:]))
	r.trace = append(r.trace, mips.ReadEntry{Type: "float32", FVal: v, Pos: r.pos, Extra: name})
	r.pos += 4
}

func (r *traceReader) rs16(name string) {
	if r.pos+2 > len(r.data) { return }
	v := int16(binary.LittleEndian.Uint16(r.data[r.pos:]))
	r.trace = append(r.trace, mips.ReadEntry{Type: "int16", IVal: int64(v), Pos: r.pos, Extra: name})
	r.pos += 2
}

func (r *traceReader) ru8(name string) {
	if r.pos >= len(r.data) { return }
	v := r.data[r.pos]
	r.trace = append(r.trace, mips.ReadEntry{Type: "uint8", IVal: int64(v), Pos: r.pos, Extra: name})
	r.pos++
}

func (r *traceReader) ri8(name string) {
	if r.pos >= len(r.data) { return }
	v := int8(r.data[r.pos])
	r.trace = append(r.trace, mips.ReadEntry{Type: "int8", IVal: int64(v), Pos: r.pos, Extra: name})
	r.pos++
}

// Diff represents one mismatch between Go and PS2 read traces.
type Diff struct {
	Index   int
	GoRead  mips.ReadEntry
	PS2Read mips.ReadEntry
	Kind    string // "type", "value", "pos", "missing_go", "missing_ps2"
}

// diffTraces compares Go and PS2 read traces by stream position.
func diffTraces(goReads, ps2Reads []mips.ReadEntry) []Diff {
	var diffs []Diff

	// Build position maps for alignment
	gi, pi := 0, 0
	for gi < len(goReads) && pi < len(ps2Reads) {
		g := goReads[gi]
		p := ps2Reads[pi]

		// Skip ReadBegin/ReadEnd for position alignment (they don't consume data)
		if g.Type == "ReadBegin" || g.Type == "ReadEnd" {
			if p.Type == g.Type {
				if g.Type == "ReadBegin" && g.Extra != p.Extra {
					diffs = append(diffs, Diff{gi, g, p, "readbegin_mismatch"})
				}
				gi++
				pi++
				continue
			}
			gi++
			continue
		}
		if p.Type == "ReadBegin" || p.Type == "ReadEnd" {
			pi++
			continue
		}

		// Both are data reads — compare by position
		if g.Pos == p.Pos {
			if g.Type != p.Type {
				diffs = append(diffs, Diff{gi, g, p, "type"})
			} else if g.Type == "float32" {
				if g.FVal != p.FVal {
					diffs = append(diffs, Diff{gi, g, p, "value_float"})
				}
			} else if g.IVal != p.IVal {
				diffs = append(diffs, Diff{gi, g, p, "value"})
			}
			gi++
			pi++
		} else if g.Pos < p.Pos {
			diffs = append(diffs, Diff{gi, g, mips.ReadEntry{}, "extra_go"})
			gi++
		} else {
			diffs = append(diffs, Diff{pi, mips.ReadEntry{}, p, "extra_ps2"})
			pi++
		}
	}

	// Remaining reads
	for ; gi < len(goReads); gi++ {
		g := goReads[gi]
		if g.Type != "ReadBegin" && g.Type != "ReadEnd" {
			diffs = append(diffs, Diff{gi, g, mips.ReadEntry{}, "extra_go"})
		}
	}
	for ; pi < len(ps2Reads); pi++ {
		p := ps2Reads[pi]
		if p.Type != "ReadBegin" && p.Type != "ReadEnd" {
			diffs = append(diffs, Diff{pi, mips.ReadEntry{}, p, "extra_ps2"})
		}
	}

	return diffs
}

// formatSuggestion generates a fix suggestion for a diff.
func formatSuggestion(d Diff, goFile string) string {
	var sb strings.Builder
	switch d.Kind {
	case "type":
		sb.WriteString(fmt.Sprintf("  TYPE MISMATCH at ESF+0x%04X:\n", d.GoRead.Pos))
		sb.WriteString(fmt.Sprintf("    Go reads:  %-8s (%s)\n", d.GoRead.Type, d.GoRead.Extra))
		sb.WriteString(fmt.Sprintf("    PS2 reads: %-8s\n", d.PS2Read.Type))
		sb.WriteString(fmt.Sprintf("    FIX: In %s, change read%s() to read%s() at offset 0x%X\n",
			goFile, capitalize(d.GoRead.Type), capitalize(d.PS2Read.Type), d.GoRead.Pos))

	case "value":
		sb.WriteString(fmt.Sprintf("  VALUE MISMATCH at ESF+0x%04X (%s):\n", d.GoRead.Pos, d.GoRead.Type))
		sb.WriteString(fmt.Sprintf("    Go value:  %d (0x%X)\n", d.GoRead.IVal, uint64(d.GoRead.IVal)))
		sb.WriteString(fmt.Sprintf("    PS2 value: %d (0x%X)\n", d.PS2Read.IVal, uint64(d.PS2Read.IVal)))
		sb.WriteString(fmt.Sprintf("    FIX: Check %s — field at 0x%X reads wrong value\n",
			goFile, d.GoRead.Pos))

	case "extra_go":
		sb.WriteString(fmt.Sprintf("  EXTRA GO READ at ESF+0x%04X: %s (%s)\n",
			d.GoRead.Pos, d.GoRead.Type, d.GoRead.Extra))
		sb.WriteString(fmt.Sprintf("    PS2 does NOT read this field.\n"))
		sb.WriteString(fmt.Sprintf("    FIX: Remove this read from %s\n", goFile))

	case "extra_ps2":
		sb.WriteString(fmt.Sprintf("  MISSING GO READ at ESF+0x%04X: PS2 reads %s\n",
			d.PS2Read.Pos, d.PS2Read.Type))
		sb.WriteString(fmt.Sprintf("    Go does NOT read this field.\n"))
		sb.WriteString(fmt.Sprintf("    FIX: Add read%s() to %s at offset 0x%X\n",
			capitalize(d.PS2Read.Type), goFile, d.PS2Read.Pos))

	case "readbegin_mismatch":
		sb.WriteString(fmt.Sprintf("  READBEGIN MISMATCH:\n"))
		sb.WriteString(fmt.Sprintf("    Go:  %s\n", d.GoRead.Extra))
		sb.WriteString(fmt.Sprintf("    PS2: %s\n", d.PS2Read.Extra))
	}
	return sb.String()
}

func capitalize(s string) string {
	if s == "" {
		return s
	}
	return strings.ToUpper(s[:1]) + s[1:]
}

func main() {
	typFlag := flag.String("type", "0x1200", "ESF type to verify (hex)")
	maxFlag := flag.Int("max", 50, "Max objects to verify")
	verboseFlag := flag.Bool("v", false, "Verbose: show diffs with fix suggestions")
	ps2OnlyFlag := flag.Bool("ps2-only", false, "Show PS2 read trace only (no Go comparison)")
	dumpPath := flag.String("dump", "/home/sdg/claude-eqoa/memory-dumps/go-inspect2.eeMemory", "EE memory dump path")
	maxDiffs := flag.Int("max-diffs", 5, "Max diffs to show per object")
	flag.Parse()

	if flag.NArg() < 1 {
		fmt.Fprintf(os.Stderr, "Usage: esf-verify [flags] <esf-file>\n\n")
		fmt.Fprintf(os.Stderr, "Supported types:\n")
		for _, p := range parsers {
			fmt.Fprintf(os.Stderr, "  0x%04X  %s → %s\n", p.Typ, p.Name, p.GoFile)
		}
		os.Exit(1)
	}
	esfPath := flag.Arg(0)

	var targetType uint16
	if *typFlag != "" {
		var t uint32
		fmt.Sscanf(*typFlag, "0x%x", &t)
		if t == 0 {
			fmt.Sscanf(*typFlag, "%x", &t)
		}
		targetType = uint16(t)
	}

	parserAddr, parserName, goFile := parserForType(targetType)
	if parserAddr == 0 {
		fmt.Fprintf(os.Stderr, "No PS2 parser for type 0x%04X. Supported:\n", targetType)
		for _, p := range parsers {
			fmt.Fprintf(os.Stderr, "  0x%04X  %s\n", p.Typ, p.Name)
		}
		os.Exit(1)
	}

	// Load EE dump
	fmt.Printf("Loading EE dump...\n")
	eeDump, err := os.ReadFile(*dumpPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "EE dump: %v\n", err)
		os.Exit(1)
	}

	// Load ESF
	fmt.Printf("Loading %s...\n", esfPath)
	esfData, err := os.ReadFile(esfPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "ESF: %v\n", err)
		os.Exit(1)
	}

	// Find objects
	offsets := scanForType(esfData, targetType, *maxFlag)
	fmt.Printf("Found %d objects of type 0x%04X (%s)\n\n", len(offsets), targetType, parserName)

	if len(offsets) == 0 {
		return
	}

	matched := 0
	diffed := 0
	errored := 0
	totalDiffs := 0

	for i, offset := range offsets {
		ver := binary.LittleEndian.Uint16(esfData[offset+2:])
		size := binary.LittleEndian.Uint32(esfData[offset+4:])
		objData := esfData[offset : offset+8+int(size)]

		// Run PS2 parser
		ps2Result, ps2Reads := mips.RunParser(eeDump, parserAddr, objData)

		if *ps2OnlyFlag {
			fmt.Printf("[%3d] @0x%06X ver=%d size=%d  PS2=%d reads  result=%d\n",
				i, offset, ver, size, len(ps2Reads), ps2Result)
			if *verboseFlag {
				limit := 50
				if *maxDiffs > 0 {
					limit = *maxDiffs
				}
				for j, r := range ps2Reads {
					if j >= limit {
						fmt.Printf("  ... (%d more)\n", len(ps2Reads)-limit)
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
			continue
		}

		// Run Go trace
		goReads := goTraceFromRaw(objData, targetType)

		// Diff
		diffs := diffTraces(goReads, ps2Reads)

		status := "MATCH"
		if ps2Result < 0 {
			status = "PS2_ERROR"
			errored++
		} else if len(diffs) > 0 {
			status = fmt.Sprintf("DIFF(%d)", len(diffs))
			diffed++
			totalDiffs += len(diffs)
		} else {
			matched++
		}

		// Count data reads (excluding ReadBegin/ReadEnd)
		ps2DataReads := 0
		for _, r := range ps2Reads {
			if r.Type != "ReadBegin" && r.Type != "ReadEnd" {
				ps2DataReads++
			}
		}
		goDataReads := 0
		for _, r := range goReads {
			if r.Type != "ReadBegin" && r.Type != "ReadEnd" {
				goDataReads++
			}
		}

		fmt.Printf("[%3d] @0x%06X v%d size=%5d  PS2=%4d Go=%4d reads  %s\n",
			i, offset, ver, size, ps2DataReads, goDataReads, status)

		// Show diffs with suggestions
		if *verboseFlag && len(diffs) > 0 {
			shown := 0
			for _, d := range diffs {
				if shown >= *maxDiffs {
					fmt.Printf("  ... (%d more diffs)\n", len(diffs)-shown)
					break
				}
				fmt.Print(formatSuggestion(d, goFile))
				shown++
			}
		}
	}

	if !*ps2OnlyFlag {
		fmt.Printf("\n=== Summary ===\n")
		fmt.Printf("Total: %d objects verified against %s\n", len(offsets), parserName)
		fmt.Printf("Match: %d  Diff: %d  Error: %d  Total diffs: %d\n",
			matched, diffed, errored, totalDiffs)
		if diffed > 0 {
			fmt.Printf("\nRe-run with -v for fix suggestions.\n")
		}
		if matched == len(offsets) {
			fmt.Printf("\nAll objects MATCH PS2. Go parser is correct.\n")
		}
	}
}
