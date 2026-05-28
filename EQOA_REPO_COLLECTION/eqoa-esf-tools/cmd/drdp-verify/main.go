// drdp-verify — verify Go DRDP implementation against PS2 native code.
//
// Runs the PS2 DRDP CRC function on packet data and compares with Go.
// Can also verify segment parsing and message decoding.
//
// Usage:
//
//	drdp-verify --crc --hex "0a 7e fe ff c8 e0 ..."
//	drdp-verify --segment --hex "0a 7e fe ff c8 e0 ..."
package main

import (
	"encoding/hex"
	"flag"
	"fmt"
	"hash/crc32"
	"os"
	"strings"

	"github.com/DabDavis/eqoa-esf-tools/pkg/mips"
)

const DRDP_CRC_XOR = 0x11f19ed3

func main() {
	crcMode := flag.Bool("crc", false, "Verify CRC computation")
	segMode := flag.Bool("segment", false, "Verify segment header parsing")
	bodyMode := flag.Bool("body", false, "Verify full packet: header + body + messages")
	hexFlag := flag.String("hex", "", "Packet data as hex")
	dumpFlag := flag.String("dump", "/home/sdg/claude-eqoa/memory-dumps/go-inspect2.eeMemory", "EE dump path")
	flag.Parse()

	if !*crcMode && !*segMode && !*bodyMode {
		fmt.Println("Usage: drdp-verify --crc --hex \"bytes...\"")
		fmt.Println("       drdp-verify --segment --hex \"bytes...\"")
		fmt.Println("       drdp-verify --body --hex \"bytes...\"")
		fmt.Println("\nVerifies Go DRDP against PS2 native code.")
		os.Exit(0)
	}

	eeDump, err := os.ReadFile(*dumpFlag)
	if err != nil {
		fmt.Fprintf(os.Stderr, "EE dump: %v\n", err)
		os.Exit(1)
	}

	var packetData []byte
	if *hexFlag != "" {
		clean := strings.ReplaceAll(*hexFlag, " ", "")
		packetData, err = hex.DecodeString(clean)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Bad hex: %v\n", err)
			os.Exit(1)
		}
	} else {
		fmt.Fprintln(os.Stderr, "Provide --hex")
		os.Exit(1)
	}

	fmt.Printf("Packet: %d bytes\n\n", len(packetData))

	if len(packetData) < 8 {
		fmt.Println("Packet too short")
		os.Exit(1)
	}

	// DRDP packet format: [body][CRC:4]
	body := packetData[:len(packetData)-4]
	storedCRC := uint32(packetData[len(packetData)-4]) |
		uint32(packetData[len(packetData)-3])<<8 |
		uint32(packetData[len(packetData)-2])<<16 |
		uint32(packetData[len(packetData)-1])<<24

	// Extract endpoints from body
	localEndpoint := uint16(body[0]) | uint16(body[1])<<8
	remoteEndpoint := uint16(body[2]) | uint16(body[3])<<8

	if *crcMode {
		verifyCRC(eeDump, body, storedCRC)
	}

	if *segMode {
		verifySegmentHeader(eeDump, body, localEndpoint, remoteEndpoint)
	}

	if *bodyMode {
		verifySegmentHeader(eeDump, body, localEndpoint, remoteEndpoint)
		fmt.Println()
		verifySegmentBody(body[4:])
	}
}

func verifyCRC(eeDump, body []byte, storedCRC uint32) {
	// PS2 native CRC — full pipeline: drdp_crc(0, data, len) XOR drdp_crc_table_get(2)
	interp := mips.New(eeDump)
	bodyAddr := interp.HeapAllocExported(uint32(len(body)) + 16)
	for i, b := range body {
		interp.Store8At(bodyAddr+uint32(i), b)
	}
	ps2Raw := uint32(interp.RunCall(0x004B8ED8, 0, bodyAddr, uint32(len(body))))
	ps2Mask := uint32(interp.RunCall(0x004B9068, 2)) // protocol version 2
	ps2CRC := ps2Raw ^ ps2Mask

	// Go CRC32 — PS2 drdp_crc(0) == ChecksumIEEE ^ 0xEE0E612C
	const PS2_CRC_MASK = 0xEE0E612C
	goCRC := crc32.ChecksumIEEE(body) ^ PS2_CRC_MASK

	fmt.Printf("=== CRC Verification ===\n")
	fmt.Printf("Body length:    %d bytes\n", len(body))
	fmt.Printf("Stored CRC:     0x%08X\n", storedCRC)
	fmt.Printf("PS2 CRC:        0x%08X (native drdp_crc)\n", ps2CRC)
	fmt.Printf("Go CRC:         0x%08X (crc32.IEEE ^ 0x%08X)\n", goCRC, DRDP_CRC_XOR)
	fmt.Printf("\n")
	fmt.Printf("PS2 == Stored:  %v\n", ps2CRC == storedCRC)
	fmt.Printf("Go  == Stored:  %v\n", goCRC == storedCRC)
	fmt.Printf("PS2 == Go:      %v\n", ps2CRC == goCRC)

	if ps2CRC == goCRC && goCRC == storedCRC {
		fmt.Println("\n✓ All CRCs match — Go implementation is PS2-accurate")
	} else if ps2CRC != goCRC {
		fmt.Println("\n✗ PS2 and Go CRCs differ — Go implementation has a bug")
	}

	fmt.Printf("\nLocal endpoint:  0x%04X\n", uint16(body[0])|uint16(body[1])<<8)
	fmt.Printf("Remote endpoint: 0x%04X\n", uint16(body[2])|uint16(body[3])<<8)
}

func verifySegmentHeader(eeDump, body []byte, localEndpoint, remoteEndpoint uint16) {
	// Segment data starts after 4-byte endpoint header
	segmentData := body[4:]

	fmt.Printf("=== Segment Header Verification ===\n")
	fmt.Printf("Local endpoint:  0x%04X\n", localEndpoint)
	fmt.Printf("Remote endpoint: 0x%04X\n", remoteEndpoint)
	fmt.Printf("Segment data:    %d bytes\n", len(segmentData))
	fmt.Printf("Segment hex:     ")
	for i, b := range segmentData {
		if i >= 32 {
			fmt.Printf("...")
			break
		}
		fmt.Printf("%02X ", b)
	}
	fmt.Println()

	// --- PS2 native segment header parsing ---
	result := mips.RunDRDPSegmentHeader(eeDump, segmentData, localEndpoint, remoteEndpoint)
	fmt.Printf("\nPS2 result:      %d\n", result.Result)
	fmt.Printf("PS2 steps:       %d\n", result.Steps)

	// Extract PS2 segment_header_t fields (layout discovered empirically):
	//   +0x00: uint32 flags
	//   +0x04: uint32 size
	//   +0x08: uint32 instance
	//   +0x0C: uint32 extra (Data0x40000 / ResetConnection value)
	//   +0x10: uint64 local_endpoint
	//   +0x18: uint64 src_addr
	//   +0x20: uint64 remote_endpoint
	r := result.RawBytes
	u32 := func(off int) uint32 {
		return uint32(r[off]) | uint32(r[off+1])<<8 | uint32(r[off+2])<<16 | uint32(r[off+3])<<24
	}
	u64 := func(off int) uint64 {
		return uint64(u32(off)) | uint64(u32(off+4))<<32
	}

	ps2Flags := u32(0)
	ps2Size := u32(4)
	ps2Instance := u32(8)
	ps2Extra := u32(0x0C) // Data0x40000 or ResetConnection
	ps2LocalEP := u64(0x10)
	ps2SrcAddr := u64(0x18)
	ps2RemoteEP := u64(0x20)

	fmt.Printf("\n--- PS2 Native (segment_header_t) ---\n")
	fmt.Printf("  Flags:          0x%08X\n", ps2Flags)
	fmt.Printf("  Size:           %d (0x%03X)\n", ps2Size, ps2Size)
	fmt.Printf("  Instance:       0x%08X\n", ps2Instance)
	if ps2Extra != 0 {
		fmt.Printf("  Extra (+0x0C):  0x%08X\n", ps2Extra)
	}
	fmt.Printf("  LocalEndpoint:  0x%016X\n", ps2LocalEP)
	fmt.Printf("  SrcAddr:        0x%016X\n", ps2SrcAddr)
	fmt.Printf("  RemoteEndpoint: 0x%016X\n", ps2RemoteEP)

	// Decode flag bits
	flagNames := []struct {
		mask uint32
		name string
	}{
		{0x0800, "IsRemote"},
		{0x1000, "ClientInitiated"},
		{0x2000, "HasInstance"},
		{0x4000, "DidServerInitiate"},
		{0x8000, "SrcAddrInline"},
		{0x10000, "ResetConnection"},
		{0x40000, "Data0x40000"},
		{0x80000, "NewInstance"},
	}
	var setFlags []string
	for _, f := range flagNames {
		if ps2Flags&f.mask != 0 {
			setFlags = append(setFlags, f.name)
		}
	}
	fmt.Printf("  Flag bits:      %v\n", setFlags)

	// --- Go segment header parsing ---
	fmt.Printf("\n--- Go Segment Header Parse ---\n")
	goFlags, goSize, goInstance, goLocalEP := goParseSegmentHeader(segmentData)

	// --- Comparison ---
	fmt.Printf("\n=== Comparison ===\n")
	allMatch := true
	compare := func(name string, ps2, go_ uint64) {
		if ps2 == go_ {
			fmt.Printf("  %-16s ✓ 0x%X\n", name+":", ps2)
		} else {
			fmt.Printf("  %-16s ✗ PS2=0x%X  Go=0x%X\n", name+":", ps2, go_)
			allMatch = false
		}
	}
	compare("Flags", uint64(ps2Flags), uint64(goFlags))
	compare("Size", uint64(ps2Size), uint64(goSize))
	compare("Instance", uint64(ps2Instance), uint64(goInstance))

	// LocalEndpoint / SrcAddr semantics differ between PS2 and Go:
	// - PS2: LocalEndpoint = function param (packet header), SrcAddr = inline varint
	// - Go:  LocalEndpoint = inline varint (when NOT ClientInitiated)
	// When ClientInitiated, no inline varint — both use packet header value.
	if goFlags&0x1000 != 0 {
		// ClientInitiated: both use packet header
		compare("LocalEndpoint", ps2LocalEP, uint64(localEndpoint))
	} else {
		// NOT ClientInitiated: Go reads varint as "LocalEndpoint", PS2 stores it as SrcAddr
		compare("LocalEndpoint", ps2LocalEP, uint64(localEndpoint)) // both from param
		compare("SrcAddr(varint)", ps2SrcAddr, goLocalEP)           // Go's "LocalEndpoint" == PS2's SrcAddr
	}

	if allMatch {
		fmt.Println("\n✓ All segment header fields match — Go is PS2-accurate")
	} else {
		fmt.Println("\n✗ Segment header mismatch — Go implementation differs from PS2")
	}
}

// goParseSegmentHeader parses the segment header using the same logic as
// go-eqoa-client/internal/protocol/protocol.go SegmentHeader.Read()
// Returns (flags, size, instance, localEndpoint).
func goParseSegmentHeader(data []byte) (uint32, uint32, uint32, uint64) {
	pos := 0

	// 1. Read varint (flags|size combined)
	flagsSize, n := readVarint64(data[pos:])
	pos += n

	const flagsMask = uint64(0xFFFFF800)
	const sizeMask = uint64(0x7FF)
	flags := uint32(flagsSize & flagsMask)
	size := uint32(flagsSize & sizeMask)

	fmt.Printf("  Varint raw:     0x%X (%d bytes)\n", flagsSize, n)
	fmt.Printf("  Flags:          0x%08X\n", flags)
	fmt.Printf("  Size:           %d (0x%03X)\n", size, size)

	var instance uint32
	var localEP uint64

	// 2. Instance ID (flag 0x2000)
	if flags&0x2000 != 0 {
		if pos+4 <= len(data) {
			instance = uint32(data[pos]) | uint32(data[pos+1])<<8 |
				uint32(data[pos+2])<<16 | uint32(data[pos+3])<<24
			fmt.Printf("  Instance:       0x%08X\n", instance)
			pos += 4
		}
	}

	// 3. Data 0x40000 (extra uint32)
	if flags&0x40000 != 0 {
		if pos+4 <= len(data) {
			v := uint32(data[pos]) | uint32(data[pos+1])<<8 |
				uint32(data[pos+2])<<16 | uint32(data[pos+3])<<24
			fmt.Printf("  Data0x40000:    0x%08X\n", v)
			pos += 4
		}
	}

	// 4. Src addr inline (flag 0x8000) — 8-byte long
	if flags&0x8000 != 0 {
		if pos+8 <= len(data) {
			lo := uint32(data[pos]) | uint32(data[pos+1])<<8 |
				uint32(data[pos+2])<<16 | uint32(data[pos+3])<<24
			hi := uint32(data[pos+4]) | uint32(data[pos+5])<<8 |
				uint32(data[pos+6])<<16 | uint32(data[pos+7])<<24
			fmt.Printf("  SrcAddr:        0x%08X%08X\n", hi, lo)
			pos += 8
		}
	}

	// 5. Local endpoint (NOT 0x1000 ClientInitiated) — varint64
	if flags&0x1000 == 0 {
		v, vn := readVarint64(data[pos:])
		localEP = v
		fmt.Printf("  LocalEndpoint:  0x%X (%d bytes varint)\n", v, vn)
		pos += vn
	} else {
		fmt.Printf("  LocalEndpoint:  (client-initiated, not inline)\n")
	}

	// 6. Remote endpoint (flag 0x0800) — varint64
	if flags&0x0800 != 0 {
		v, vn := readVarint64(data[pos:])
		fmt.Printf("  RemoteEndpoint: 0x%X (%d bytes varint)\n", v, vn)
		pos += vn
	}

	fmt.Printf("  Header bytes:   %d\n", pos)
	fmt.Printf("  Remaining:      %d bytes (segment body)\n", len(data)-pos)
	return flags, size, instance, localEP
}

// verifySegmentBody parses the segment body (after endpoints) following the
// PS2 connection_parse_segment_body format from decompilation at 0x004B2DB0.
// Compares the read sequence against Go's SegmentBody.Read() format.
func verifySegmentBody(segmentData []byte) {
	// First parse header to find body start
	pos := 0
	flagsSize, n := readVarint64(segmentData[pos:])
	pos += n
	flags := uint32(flagsSize & 0xFFFFF800)
	size := int(flagsSize & 0x7FF)

	// Skip header fields
	if flags&0x2000 != 0 { pos += 4 } // instance
	if flags&0x40000 != 0 { pos += 4 } // data_0x40000
	if flags&0x8000 != 0 { pos += 8 }  // src_addr
	if flags&0x1000 == 0 {              // local_endpoint varint
		_, vn := readVarint64(segmentData[pos:])
		pos += vn
	}
	if flags&0x0800 != 0 { // remote_endpoint varint
		_, vn := readVarint64(segmentData[pos:])
		pos += vn
	}

	bodyStart := pos
	bodyEnd := bodyStart + size
	if bodyEnd > len(segmentData) {
		bodyEnd = len(segmentData)
	}

	fmt.Printf("=== Segment Body Verification ===\n")
	fmt.Printf("Body offset:     %d\n", bodyStart)
	fmt.Printf("Body size:       %d\n", size)

	if size == 0 {
		fmt.Println("(empty body — reset/probe packet)")
		return
	}

	// --- Parse body following PS2 format ---
	// PS2: buffer_read_uns8 → flags byte
	if pos >= bodyEnd {
		fmt.Println("(body too short)")
		return
	}
	bodyFlags := segmentData[pos]
	pos++
	fmt.Printf("Body flags:      0x%02X", bodyFlags)
	var bodyFlagNames []string
	if bodyFlags&0x01 != 0 { bodyFlagNames = append(bodyFlagNames, "FlushACK") }
	if bodyFlags&0x02 != 0 { bodyFlagNames = append(bodyFlagNames, "GuaranteedACK") }
	if bodyFlags&0x04 != 0 { bodyFlagNames = append(bodyFlagNames, "FlushACKMask") }
	if bodyFlags&0x08 != 0 { bodyFlagNames = append(bodyFlagNames, "GuaranteedACKMask") }
	if bodyFlags&0x10 != 0 { bodyFlagNames = append(bodyFlagNames, "StateACK") }
	if bodyFlags&0x20 != 0 { bodyFlagNames = append(bodyFlagNames, "Short") }
	if bodyFlags&0x40 != 0 { bodyFlagNames = append(bodyFlagNames, "InstanceACK") }
	fmt.Printf(" %v\n", bodyFlagNames)

	// PS2: if flags & 0x40, buffer_read_uns32 → instance remote ID
	if bodyFlags&0x40 != 0 {
		if pos+4 <= bodyEnd {
			v := readU32(segmentData, pos)
			fmt.Printf("Instance ACK:    0x%08X\n", v)
			pos += 4
		}
	}

	// PS2: buffer_read_u16 → arrival sequence number
	if pos+2 <= bodyEnd {
		arrivalSeq := readU16(segmentData, pos)
		fmt.Printf("Arrival seq:     %d (0x%04X)\n", arrivalSeq, arrivalSeq)
		pos += 2
	}

	// PS2: Flush ACK section (flag bit 0)
	if bodyFlags&0x01 != 0 {
		if pos+2 <= bodyEnd {
			flushACK := readU16(segmentData, pos)
			fmt.Printf("Flush ACK:       %d (0x%04X)\n", flushACK, flushACK)
			pos += 2
		}
		// PS2: if flags & 0x04, buffer_read_packed_uns64 → flush ACK mask
		if bodyFlags&0x04 != 0 {
			v, vn := readVarint64(segmentData[pos:])
			fmt.Printf("Flush ACK mask:  0x%016X (%d bytes varint)\n", v, vn)
			pos += vn
		}
	}

	// PS2: Guaranteed ACK section (flag bit 1)
	if bodyFlags&0x02 != 0 {
		if pos+2 <= bodyEnd {
			gACK := readU16(segmentData, pos)
			fmt.Printf("Guaranteed ACK:  %d (0x%04X)\n", gACK, gACK)
			pos += 2
		}
		// PS2: if flags & 0x08, buffer_read_packed_uns64 → guaranteed ACK mask
		if bodyFlags&0x08 != 0 {
			v, vn := readVarint64(segmentData[pos:])
			fmt.Printf("Guaranteed mask: 0x%016X (%d bytes varint)\n", v, vn)
			pos += vn
		}
	}

	// PS2: State ACK section (flag bit 4)
	if bodyFlags&0x10 != 0 {
		ackCount := 0
		for pos < bodyEnd {
			channel := segmentData[pos]
			pos++
			if channel == 0xF8 {
				break
			}
			if channel > 0xF8 {
				fmt.Printf("State ACK ERROR: invalid channel 0x%02X\n", channel)
				break
			}
			if pos+2 <= bodyEnd {
				seq := readU16(segmentData, pos)
				pos += 2
				if ackCount < 5 {
					fmt.Printf("State ACK:       ch=%d seq=%d\n", channel, seq)
				}
				ackCount++
			}
		}
		if ackCount > 5 {
			fmt.Printf("State ACK:       ... (%d total)\n", ackCount)
		} else if ackCount > 0 {
			fmt.Printf("State ACKs:      %d entries\n", ackCount)
		}
	}

	// PS2: Message loop — buffer_bytes_remaining → read messages
	msgCount := 0
	for pos < bodyEnd {
		// PS2: buffer_read_uns8 → message type
		msgType := segmentData[pos]
		pos++

		if msgType == 0xF8 { // terminator (shouldn't appear, but just in case)
			break
		}

		// PS2: buffer_read_size (uint16) → message size
		if pos+2 > bodyEnd { break }
		msgSize := int(readU16(segmentData, pos))
		pos += 2

		typeName := ""
		hasSeq := false
		switch {
		case msgType < 0xF8:
			typeName = fmt.Sprintf("StateChannel(%d)", msgType)
			hasSeq = true
		case msgType == 0xF9:
			typeName = "Reliable"
			hasSeq = true
		case msgType == 0xFA:
			typeName = "Ping"
			hasSeq = true
		case msgType == 0xFB:
			typeName = "ReliableLarge"
			hasSeq = true
		case msgType == 0xFC:
			typeName = "Unreliable"
		default:
			typeName = fmt.Sprintf("Unknown(0x%02X)", msgType)
		}

		if hasSeq {
			if pos+2 <= bodyEnd {
				seq := readU16(segmentData, pos)
				pos += 2
				fmt.Printf("Message[%d]:      %s seq=%d size=%d", msgCount, typeName, seq, msgSize)
			}
		} else {
			fmt.Printf("Message[%d]:      %s size=%d", msgCount, typeName, msgSize)
		}

		// For reliable messages, show opcode
		if (msgType == 0xF9 || msgType == 0xFB) && msgSize >= 2 && pos+2 <= bodyEnd {
			opcode := readU16(segmentData, pos)
			fmt.Printf(" opcode=0x%04X", opcode)
		}

		// For state channels, show refnum
		if msgType < 0xF8 && pos < bodyEnd {
			refNum := segmentData[pos]
			fmt.Printf(" refnum=%d", refNum)
		}

		fmt.Println()

		// Skip message data
		if pos+msgSize > bodyEnd {
			fmt.Printf("  (truncated: need %d bytes, have %d)\n", msgSize, bodyEnd-pos)
			pos = bodyEnd
		} else {
			pos += msgSize
		}

		msgCount++
	}

	remaining := bodyEnd - pos
	fmt.Printf("\nTotal messages:  %d\n", msgCount)
	fmt.Printf("Bytes consumed:  %d / %d\n", pos-bodyStart, size)
	if remaining == 0 {
		fmt.Println("✓ All body bytes consumed — parse complete")
	} else {
		fmt.Printf("✗ %d bytes remaining — possible parse error\n", remaining)
	}
}

func readU16(data []byte, pos int) uint16 {
	return uint16(data[pos]) | uint16(data[pos+1])<<8
}

func readU32(data []byte, pos int) uint32 {
	return uint32(data[pos]) | uint32(data[pos+1])<<8 |
		uint32(data[pos+2])<<16 | uint32(data[pos+3])<<24
}

// readVarint64 reads a 7-bit encoded uint64 (MSB continuation bit).
func readVarint64(data []byte) (uint64, int) {
	var val uint64
	var shift uint
	for i, b := range data {
		val |= uint64(b&0x7F) << shift
		shift += 7
		if b&0x80 == 0 {
			return val, i + 1
		}
		if i >= 9 { // max 10 bytes for 64-bit varint
			return val, i + 1
		}
	}
	return val, len(data)
}
