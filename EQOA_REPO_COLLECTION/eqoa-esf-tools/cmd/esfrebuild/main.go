// esfrebuild: Rebuild TUNARIA.ESF with zone replacements.
//
// Takes the original TUNARIA.ESF and a directory of replacement zone ESF files.
// Zones are replaced by matching filename: zone_N.esf replaces zone N.
// The output is a complete, valid ESF file with correct size headers.
//
// Usage:
//   esfrebuild -o TUNARIA-modified.ESF -zones patches/ TUNARIA.ESF
//
// Zone files can be any size — the rebuilt file adjusts automatically.
// Non-replaced zones are copied verbatim from the original.
package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func main() {
	outPath := flag.String("o", "TUNARIA-rebuilt.ESF", "Output ESF file")
	zonesDir := flag.String("zones", "", "Directory with zone_N.esf replacement files")
	flag.Parse()

	if flag.NArg() < 1 || *zonesDir == "" {
		fmt.Fprintf(os.Stderr, "Usage: esfrebuild -o OUTPUT.ESF -zones DIR TUNARIA.ESF\n")
		fmt.Fprintf(os.Stderr, "\nReplacement zone files: DIR/zone_N.esf (where N = zone index)\n")
		os.Exit(1)
	}

	srcPath := flag.Arg(0)
	log.Printf("Opening %s...", srcPath)

	src, err := esf.Open(srcPath)
	if err != nil {
		log.Fatal(err)
	}

	root, err := src.Root()
	if err != nil {
		log.Fatal(err)
	}

	worldInfo := root.Child(esf.TypeWorld)
	if worldInfo == nil {
		log.Fatal("no World object found")
	}

	worldBase := root.Child(esf.TypeWorldBase)

	zones := worldInfo.ChildrenOfType(esf.TypeZone)
	log.Printf("Original has %d zones", len(zones))

	// Scan for replacement zone files
	replacements := map[int]*esf.ObjFile{}
	replacementZones := map[int]*esf.ObjInfo{}

	entries, err := os.ReadDir(*zonesDir)
	if err != nil {
		log.Fatalf("reading zones dir: %v", err)
	}

	for _, e := range entries {
		name := e.Name()
		if !strings.HasPrefix(name, "zone_") || !strings.HasSuffix(name, ".esf") {
			continue
		}
		numStr := strings.TrimSuffix(strings.TrimPrefix(name, "zone_"), ".esf")
		idx, err := strconv.Atoi(numStr)
		if err != nil {
			continue
		}

		path := filepath.Join(*zonesDir, name)
		zf, err := esf.Open(path)
		if err != nil {
			log.Printf("WARNING: cannot open %s: %v", path, err)
			continue
		}

		zRoot, err := zf.Root()
		if err != nil {
			log.Printf("WARNING: cannot parse %s: %v", path, err)
			continue
		}

		// Find the Zone object — it might be wrapped in a World node
		var zoneObj *esf.ObjInfo
		if w := zRoot.Child(esf.TypeWorld); w != nil {
			zoneChildren := w.ChildrenOfType(esf.TypeZone)
			if len(zoneChildren) > 0 {
				zoneObj = zoneChildren[0]
			}
		}
		if zoneObj == nil {
			zoneObj = zRoot.Child(esf.TypeZone)
		}
		if zoneObj == nil {
			log.Printf("WARNING: %s has no Zone object, skipping", path)
			continue
		}

		replacements[idx] = zf
		replacementZones[idx] = zoneObj
		fi, _ := os.Stat(path)
		log.Printf("  Zone %d: %s (%d bytes)", idx, path, fi.Size())
	}

	log.Printf("Building new TUNARIA.ESF with %d zone replacement(s)...", len(replacements))

	// The original file structure is:
	//   FileHeader(32 bytes)
	//   Root(0x8000)
	//     World(0x8100)
	//       Zone(0x3000) × N
	//     WorldBase(0x8200)
	//
	// We must preserve this exact structure including the Root wrapper.

	// Copy the original file header verbatim (32 bytes) — it has the correct
	// object count, fileType, and unknown fields that the PS2 parser expects.
	origHeader := src.RawBytes(0, 32)

	// Copy the Root object header (12 bytes at offset 32)
	rootInfo := root
	rootHeader := src.RawBytes(rootInfo.Offset-12, 12)

	// Build new body content for Root
	w := esf.NewWriter()

	// World node containing all zones
	worldH := w.WriteNodeBegin(esf.TypeWorld, worldInfo.Version, int32(len(zones)))
	for i, zone := range zones {
		if repFile, ok := replacements[i]; ok {
			w.WriteNodeRaw(replacementZones[i], repFile)
			log.Printf("  Zone %d: REPLACED (orig %d bytes → new %d bytes)",
				i, zone.Size+12, replacementZones[i].Size+12)
		} else {
			w.WriteNodeRaw(zone, src)
		}
	}
	w.WriteNodeEnd(worldH)

	// Copy WorldBase as-is
	if worldBase != nil {
		w.WriteNodeRaw(worldBase, src)
	}

	innerData := w.Finalize()
	// innerData has its own 32-byte header we don't need — just use the body
	innerBody := innerData[32:]

	// Build final file: original header + Root header (with patched size) + body
	data := make([]byte, 0, 32+12+len(innerBody))
	data = append(data, origHeader...)

	// Root header: type(2) + version(2) + size(4) + numChildren(4)
	// Copy original root header but patch the size field
	rh := make([]byte, 12)
	copy(rh, rootHeader)
	binary.LittleEndian.PutUint32(rh[4:], uint32(len(innerBody)))
	data = append(data, rh...)

	data = append(data, innerBody...)
	log.Printf("Output: %d bytes (%.1f MB)", len(data), float64(len(data))/(1024*1024))

	if err := os.WriteFile(*outPath, data, 0644); err != nil {
		log.Fatal(err)
	}
	log.Printf("Wrote %s", *outPath)
}
