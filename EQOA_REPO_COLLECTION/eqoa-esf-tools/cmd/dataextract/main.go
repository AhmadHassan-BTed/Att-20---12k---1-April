package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"
	"unicode/utf16"
)

func main() {
	var (
		outputDir string
		pakFile   string
		textDir   string
	)

	flag.StringVar(&outputDir, "o", "extracted-data", "Output directory")
	flag.StringVar(&pakFile, "pak", "", "Extract STATION.PAK archive")
	flag.StringVar(&textDir, "text", "", "Process text files from directory (convert UTF-16LE to UTF-8, extract PAK)")

	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "dataextract - EQOA text/XML data extractor\n\n")
		fmt.Fprintf(os.Stderr, "Usage:\n")
		fmt.Fprintf(os.Stderr, "  dataextract [flags]\n\n")
		fmt.Fprintf(os.Stderr, "Examples:\n")
		fmt.Fprintf(os.Stderr, "  dataextract -pak STATION.PAK -o output/            # Extract PAK archive\n")
		fmt.Fprintf(os.Stderr, "  dataextract -text /path/to/textfiles/ -o output/   # Convert all text files\n\n")
		fmt.Fprintf(os.Stderr, "Flags:\n")
		flag.PrintDefaults()
	}

	flag.Parse()

	if pakFile == "" && textDir == "" {
		flag.Usage()
		os.Exit(1)
	}

	if err := os.MkdirAll(outputDir, 0o755); err != nil {
		log.Fatal(err)
	}

	if pakFile != "" {
		pakOutDir := filepath.Join(outputDir, "pak")
		if err := extractPAK(pakFile, pakOutDir); err != nil {
			log.Fatal(err)
		}
	}

	if textDir != "" {
		if err := processTextFiles(textDir, outputDir); err != nil {
			log.Fatal(err)
		}
	}
}

// extractPAK extracts all entries from a STATION.PAK archive.
//
// PAK format (repeated entries until EOF):
//
//	[4 bytes BE: filename length N]
//	[N bytes: filename]
//	[\0: null terminator]
//	[content: XML or text until next entry]
func extractPAK(pakPath, outputDir string) error {
	data, err := os.ReadFile(pakPath)
	if err != nil {
		return err
	}

	log.Printf("Parsing PAK: %s (%d bytes)", pakPath, len(data))

	if err := os.MkdirAll(outputDir, 0o755); err != nil {
		return err
	}

	pos := 0
	count := 0

	for pos < len(data)-4 {
		// Read filename length (4 bytes big-endian)
		nameLen := int(binary.BigEndian.Uint32(data[pos : pos+4]))
		pos += 4

		if nameLen < 1 || nameLen > 256 {
			break
		}

		// Read filename
		if pos+nameLen > len(data) {
			break
		}
		name := string(data[pos : pos+nameLen])
		pos += nameLen

		// Skip null terminator
		if pos < len(data) && data[pos] == 0 {
			pos++
		}

		// Find end of content: look for </PS2GUIPage> tag
		var content []byte
		endTag := []byte("</PS2GUIPage>")
		endIdx := indexOf(data, endTag, pos)
		if endIdx >= 0 {
			endIdx += len(endTag)
			// Skip trailing whitespace
			for endIdx < len(data) && isWhitespace(data[endIdx]) {
				endIdx++
			}
			content = data[pos:endIdx]
			pos = endIdx
		} else {
			// Non-XML entry - scan for next entry header
			nextEntry := findNextEntry(data, pos)
			if nextEntry > 0 {
				content = trimRight(data[pos:nextEntry])
				pos = nextEntry
			} else {
				content = trimRight(data[pos:])
				pos = len(data)
			}
		}

		// Write the file, preserving directory structure
		outPath := filepath.Join(outputDir, name)
		if err := os.MkdirAll(filepath.Dir(outPath), 0o755); err != nil {
			return err
		}
		if err := os.WriteFile(outPath, content, 0o644); err != nil {
			return err
		}

		count++
		log.Printf("  [%2d] %s (%d bytes)", count, name, len(content))
	}

	log.Printf("Extracted %d entries from PAK", count)
	return nil
}

// findNextEntry scans for the next PAK entry after pos.
func findNextEntry(data []byte, pos int) int {
	for i := pos; i < len(data)-8; i++ {
		nameLen := int(binary.BigEndian.Uint32(data[i : i+4]))
		if nameLen < 8 || nameLen > 64 {
			continue
		}
		if i+4+nameLen > len(data) {
			continue
		}
		candidate := string(data[i+4 : i+4+nameLen])
		if strings.HasPrefix(candidate, "data/") || strings.HasPrefix(candidate, "station") {
			return i
		}
	}
	return -1
}

func indexOf(data, pattern []byte, start int) int {
	for i := start; i <= len(data)-len(pattern); i++ {
		match := true
		for j := range pattern {
			if data[i+j] != pattern[j] {
				match = false
				break
			}
		}
		if match {
			return i
		}
	}
	return -1
}

func isWhitespace(b byte) bool {
	return b == ' ' || b == '\t' || b == '\r' || b == '\n'
}

func trimRight(b []byte) []byte {
	end := len(b)
	for end > 0 && (b[end-1] == 0 || isWhitespace(b[end-1])) {
		end--
	}
	return b[:end]
}

// processTextFiles reads text files from a directory, converts UTF-16LE to UTF-8,
// strips UTF-8 BOMs, extracts PAK archives, and writes clean output.
func processTextFiles(inputDir, outputDir string) error {
	entries, err := os.ReadDir(inputDir)
	if err != nil {
		return err
	}

	if err := os.MkdirAll(outputDir, 0o755); err != nil {
		return err
	}

	log.Printf("Processing text files from: %s", inputDir)

	for _, e := range entries {
		if e.IsDir() {
			continue
		}
		name := e.Name()
		inPath := filepath.Join(inputDir, name)

		data, err := os.ReadFile(inPath)
		if err != nil {
			log.Printf("Warning: reading %s: %v", name, err)
			continue
		}

		// Handle PAK archives
		if strings.EqualFold(filepath.Ext(name), ".pak") {
			pakOutDir := filepath.Join(outputDir, "pak")
			if err := extractPAK(inPath, pakOutDir); err != nil {
				log.Printf("Warning: extracting PAK %s: %v", name, err)
			}
			continue
		}

		encoding := detectEncoding(data)
		outPath := filepath.Join(outputDir, name)

		switch encoding {
		case "utf-16le":
			utf8Data := decodeUTF16LE(data)
			if err := os.WriteFile(outPath, []byte(utf8Data), 0o644); err != nil {
				log.Printf("Warning: writing %s: %v", name, err)
				continue
			}
			log.Printf("  %s (UTF-16LE → UTF-8, %d → %d bytes)", name, len(data), len(utf8Data))

		case "utf-8-bom":
			if err := os.WriteFile(outPath, data[3:], 0o644); err != nil {
				log.Printf("Warning: writing %s: %v", name, err)
				continue
			}
			log.Printf("  %s (UTF-8, BOM stripped, %d bytes)", name, len(data)-3)

		default:
			if err := os.WriteFile(outPath, data, 0o644); err != nil {
				log.Printf("Warning: writing %s: %v", name, err)
				continue
			}
			log.Printf("  %s (ASCII/UTF-8, %d bytes)", name, len(data))
		}
	}

	return nil
}

// detectEncoding checks for BOM markers to determine text encoding.
func detectEncoding(data []byte) string {
	if len(data) >= 2 && data[0] == 0xFF && data[1] == 0xFE {
		return "utf-16le"
	}
	if len(data) >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF {
		return "utf-8-bom"
	}
	return "ascii"
}

// decodeUTF16LE decodes UTF-16LE bytes (with optional BOM) to a UTF-8 string.
func decodeUTF16LE(data []byte) string {
	start := 0
	if len(data) >= 2 && data[0] == 0xFF && data[1] == 0xFE {
		start = 2
	}
	remaining := data[start:]
	u16 := make([]uint16, len(remaining)/2)
	for i := range u16 {
		u16[i] = binary.LittleEndian.Uint16(remaining[i*2:])
	}
	return string(utf16.Decode(u16))
}
