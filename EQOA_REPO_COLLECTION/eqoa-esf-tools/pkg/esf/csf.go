package esf

import (
	"bytes"
	"compress/zlib"
	"encoding/binary"
	"fmt"
	"io"
	"os"
)

// DecompressCSF reads a CSF file and returns the decompressed ESF data.
// CSF is a compressed ESF format with a 40-byte header followed by
// zlib-compressed blocks. This is a port of CSFFile.java.
func DecompressCSF(csfPath string) ([]byte, error) {
	data, err := os.ReadFile(csfPath)
	if err != nil {
		return nil, fmt.Errorf("reading CSF file: %w", err)
	}

	if len(data) < 40 {
		return nil, fmt.Errorf("CSF file too small for header: %d bytes", len(data))
	}

	// Parse 40-byte header (all little-endian).
	// Bytes 4-7:   num_blocks (int32 LE)
	// Bytes 24-31: offset to block data (int64 LE)
	// Bytes 32-35: output size (int32 LE)
	numBlocks := int(binary.LittleEndian.Uint32(data[4:8]))
	offset := int(binary.LittleEndian.Uint64(data[24:32]))
	outSize := int(binary.LittleEndian.Uint32(data[32:36]))

	if offset < 0 || offset > len(data) {
		return nil, fmt.Errorf("CSF block offset %d out of range (file size %d)", offset, len(data))
	}

	out := make([]byte, 0, outSize)
	pos := offset

	for b := 0; b < numBlocks; b++ {
		// Each block: 8 bytes block size (int64 LE), then block_size bytes of zlib data.
		if pos+8 > len(data) {
			return nil, fmt.Errorf("CSF block %d: cannot read size at offset %d", b, pos)
		}
		blockSize := int(binary.LittleEndian.Uint64(data[pos:pos+8]) & 0xFFFFFFFF)
		pos += 8

		if pos+blockSize > len(data) {
			return nil, fmt.Errorf("CSF block %d: block size %d exceeds file at offset %d", b, blockSize, pos)
		}

		blockData := data[pos : pos+blockSize]
		pos += blockSize

		// Decompress with zlib (Java InflaterInputStream = zlib).
		r, err := zlib.NewReader(bytes.NewReader(blockData))
		if err != nil {
			return nil, fmt.Errorf("CSF block %d: zlib init: %w", b, err)
		}
		decompressed, err := io.ReadAll(r)
		r.Close()
		if err != nil {
			return nil, fmt.Errorf("CSF block %d: zlib decompress: %w", b, err)
		}

		out = append(out, decompressed...)
	}

	return out, nil
}

// DecompressCSFBytes decompresses CSF data already in memory (e.g., read from ISO).
func DecompressCSFBytes(data []byte) ([]byte, error) {
	if len(data) < 40 {
		return nil, fmt.Errorf("CSF data too small for header: %d bytes", len(data))
	}

	numBlocks := int(binary.LittleEndian.Uint32(data[4:8]))
	offset := int(binary.LittleEndian.Uint64(data[24:32]))
	outSize := int(binary.LittleEndian.Uint32(data[32:36]))

	if offset < 0 || offset > len(data) {
		return nil, fmt.Errorf("CSF block offset %d out of range (data size %d)", offset, len(data))
	}

	out := make([]byte, 0, outSize)
	pos := offset

	for b := 0; b < numBlocks; b++ {
		if pos+8 > len(data) {
			return nil, fmt.Errorf("CSF block %d: cannot read size at offset %d", b, pos)
		}
		blockSize := int(binary.LittleEndian.Uint64(data[pos:pos+8]) & 0xFFFFFFFF)
		pos += 8

		if pos+blockSize > len(data) {
			return nil, fmt.Errorf("CSF block %d: block size %d exceeds data at offset %d", b, blockSize, pos)
		}

		blockData := data[pos : pos+blockSize]
		pos += blockSize

		r, err := zlib.NewReader(bytes.NewReader(blockData))
		if err != nil {
			return nil, fmt.Errorf("CSF block %d: zlib init: %w", b, err)
		}
		decompressed, err := io.ReadAll(r)
		r.Close()
		if err != nil {
			return nil, fmt.Errorf("CSF block %d: zlib decompress: %w", b, err)
		}

		out = append(out, decompressed...)
	}

	return out, nil
}

// DecompressCSFToFile decompresses a CSF file and writes the resulting ESF
// data to the specified output path.
func DecompressCSFToFile(csfPath, esfPath string) error {
	data, err := DecompressCSF(csfPath)
	if err != nil {
		return err
	}
	return os.WriteFile(esfPath, data, 0o644)
}
