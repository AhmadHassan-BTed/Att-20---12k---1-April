package mips

// DRDP protocol bridge: runs PS2 DRDP packet processing natively
// and captures the parsed segment/message structure.
//
// Architecture:
//   Raw UDP bytes → buffer_init_read → drdp_process_packet → native MIPS
//   Buffer I/O intercepted: buffer_read_int8/16/32, buffer_read_packed_uns32, etc.
//   Socket I/O intercepted: sendto → capture outgoing packets
//
// This verifies our Go DRDP implementation byte-for-byte against the PS2 binary.

// DRDPRead records one buffer read during DRDP processing.
type DRDPRead struct {
	Type string // "int8", "uns8", "int16", "uns16", "int32", "uns32", "packed_uns32", "string"
	Pos  int
	Size int
	IVal int64
	SVal string
}

// DRDPResult holds the result of processing a DRDP packet.
type DRDPResult struct {
	Result int32
	Reads  []DRDPRead
	Steps  int
}

// DRDP buffer_t read function addresses (from SUPPORT symbol map)
var drdpBufferReadFuncs = map[uint32]string{
	0x004BD478: "read_int8",
	0x004BD500: "read_uns8",
	0x004BD588: "read_int16",
	0x004BD618: "read_uns16",
	0x004BD698: "read_int32",
	0x004BD728: "read_uns32",
	0x004BD7B8: "read_int64",
	0x004BD848: "read_uns64",
	0x004BDCA8: "read_packed_uns32",
	0x004BE088: "read_string",
	0x004B1098: "read_size", // buffer_read_size (uint16)
}

// DRDP buffer_t write function addresses (for outgoing packet capture)
var drdpBufferWriteFuncs = map[uint32]string{
	0x004BD4B8: "append_int8",
	0x004BD540: "append_uns8",
	0x004BD5C8: "append_int16",
	0x004BD658: "append_uns16",
	0x004BD6D8: "append_int32",
	0x004BD768: "append_uns32",
	0x004BD7F8: "append_int64",
	0x004BD888: "append_uns64",
	0x004BDC88: "append_packed_uns32",
	0x004BE018: "append_string",
	0x004B1038: "append_size", // buffer_append_size (uint16)
}

// handleDRDPRead intercepts buffer read functions during DRDP processing.
func (m *Interp) handleDRDPRead(target uint32, ds *DRDPStream) bool {
	name, ok := drdpBufferReadFuncs[target]
	if !ok {
		return false
	}

	// buffer_read_* functions: $a0 = buffer_t*, $a1 = output pointer
	dest := uint32(m.rReg(5)) // $a1

	switch name {
	case "read_int8":
		if ds.pos < len(ds.data) {
			v := int8(ds.data[ds.pos])
			ds.Reads = append(ds.Reads, DRDPRead{Type: "int8", Pos: ds.pos, Size: 1, IVal: int64(v)})
			if dest != 0 { m.store8(dest, byte(v)) }
			ds.pos++
		}
		m.wReg32(2, 0)

	case "read_uns8":
		if ds.pos < len(ds.data) {
			v := ds.data[ds.pos]
			ds.Reads = append(ds.Reads, DRDPRead{Type: "uns8", Pos: ds.pos, Size: 1, IVal: int64(v)})
			if dest != 0 { m.store8(dest, v) }
			ds.pos++
		}
		m.wReg32(2, 0)

	case "read_int16", "read_size":
		if ds.pos+2 <= len(ds.data) {
			v := int16(ds.data[ds.pos]) | int16(ds.data[ds.pos+1])<<8
			ds.Reads = append(ds.Reads, DRDPRead{Type: "int16", Pos: ds.pos, Size: 2, IVal: int64(v)})
			if dest != 0 { m.store16(dest, uint16(v)) }
			ds.pos += 2
		}
		m.wReg32(2, 0)

	case "read_uns16":
		if ds.pos+2 <= len(ds.data) {
			v := uint16(ds.data[ds.pos]) | uint16(ds.data[ds.pos+1])<<8
			ds.Reads = append(ds.Reads, DRDPRead{Type: "uns16", Pos: ds.pos, Size: 2, IVal: int64(v)})
			if dest != 0 { m.store16(dest, v) }
			ds.pos += 2
		}
		m.wReg32(2, 0)

	case "read_int32":
		if ds.pos+4 <= len(ds.data) {
			v := int32(ds.data[ds.pos]) | int32(ds.data[ds.pos+1])<<8 |
				int32(ds.data[ds.pos+2])<<16 | int32(ds.data[ds.pos+3])<<24
			ds.Reads = append(ds.Reads, DRDPRead{Type: "int32", Pos: ds.pos, Size: 4, IVal: int64(v)})
			if dest != 0 { m.store32(dest, uint32(v)) }
			ds.pos += 4
		}
		m.wReg32(2, 0)

	case "read_uns32":
		if ds.pos+4 <= len(ds.data) {
			v := uint32(ds.data[ds.pos]) | uint32(ds.data[ds.pos+1])<<8 |
				uint32(ds.data[ds.pos+2])<<16 | uint32(ds.data[ds.pos+3])<<24
			ds.Reads = append(ds.Reads, DRDPRead{Type: "uns32", Pos: ds.pos, Size: 4, IVal: int64(v)})
			if dest != 0 { m.store32(dest, v) }
			ds.pos += 4
		}
		m.wReg32(2, 0)

	case "read_packed_uns32":
		// Varint: 7-bit encoding with continuation bit
		val, n := readVarint(ds.data[ds.pos:])
		ds.Reads = append(ds.Reads, DRDPRead{Type: "packed_uns32", Pos: ds.pos, Size: n, IVal: int64(val)})
		if dest != 0 { m.store32(dest, val) }
		ds.pos += n
		m.wReg32(2, 0)

	case "read_int64", "read_uns64":
		if ds.pos+8 <= len(ds.data) {
			var v uint64
			for i := 0; i < 8; i++ {
				v |= uint64(ds.data[ds.pos+i]) << (i * 8)
			}
			ds.Reads = append(ds.Reads, DRDPRead{Type: name, Pos: ds.pos, Size: 8, IVal: int64(v)})
			if dest != 0 {
				m.store32(dest, uint32(v))
				m.store32(dest+4, uint32(v>>32))
			}
			ds.pos += 8
		}
		m.wReg32(2, 0)

	case "read_string":
		// Read null-terminated string
		maxLen := int(m.rReg(6)) // $a2
		start := ds.pos
		var str []byte
		for ds.pos < len(ds.data) && len(str) < maxLen {
			b := ds.data[ds.pos]
			ds.pos++
			if b == 0 { break }
			str = append(str, b)
		}
		ds.Reads = append(ds.Reads, DRDPRead{Type: "string", Pos: start, Size: ds.pos - start, SVal: string(str)})
		if dest != 0 {
			for i, b := range str {
				m.store8(dest+uint32(i), b)
			}
			m.store8(dest+uint32(len(str)), 0)
		}
		m.wReg32(2, 0)
	}

	m.Intercepted++
	return true
}

// DRDPStream provides raw UDP packet data to the DRDP processor.
type DRDPStream struct {
	data  []byte
	pos   int
	Reads []DRDPRead
}

// NewDRDPStream creates a stream from raw UDP packet bytes.
func NewDRDPStream(data []byte) *DRDPStream {
	return &DRDPStream{data: data}
}

// SegmentHeaderResult holds the PS2-parsed segment header fields.
type SegmentHeaderResult struct {
	Result int32
	Steps  int
	// Raw segment_header_t struct dump (first 128 bytes)
	RawBytes [128]byte
	// Extracted fields (populated after struct layout discovery)
	FlagsSize uint32 // combined flags|size varint value
	Flags     uint32
	Size      uint32
	Instance  uint32
	SrcAddr   uint64
}

// RunDRDPSegmentHeader runs PS2 buffer_read_segment_header natively on segment data.
// segmentData should be the body bytes AFTER the 4-byte endpoint header, BEFORE CRC.
// All buffer reads run natively (no stream intercept) for full PS2 accuracy.
func RunDRDPSegmentHeader(eeDump []byte, segmentData []byte, localEndpoint, remoteEndpoint uint16) SegmentHeaderResult {
	interp := New(eeDump)
	// No drdpStream — buffer reads run natively against real memory

	// Put segment data into interpreter memory
	dataBuf := interp.heapAlloc(uint32(len(segmentData)) + 16)
	for i, b := range segmentData {
		interp.store8(dataBuf+uint32(i), b)
	}

	// Set up buffer_t via buffer_init_read(buf, data, size, 1) @ 0x004BD130
	bufAddr := interp.heapAlloc(256)
	interp.RunCall(0x004BD130, bufAddr, dataBuf, uint32(len(segmentData)), 1)

	// Allocate segment_header_t output struct (zeroed by heapAlloc)
	headerAddr := interp.heapAlloc(256)

	// Call buffer_read_segment_header(buf, header, local_ep, remote_ep) @ 0x004B5998
	result := interp.RunCall(0x004B5998, bufAddr, headerAddr, uint32(localEndpoint), uint32(remoteEndpoint))

	// Read back the raw header struct
	var raw [128]byte
	for i := 0; i < 128; i++ {
		raw[i] = interp.load8(headerAddr + uint32(i))
	}

	return SegmentHeaderResult{
		Result:   result,
		Steps:    interp.Steps,
		RawBytes: raw,
	}
}

// RunDRDPProcess runs the PS2 DRDP packet processor on raw UDP bytes.
// Returns the parsed field trace.
func RunDRDPProcess(eeDump []byte, packetData []byte) DRDPResult {
	interp := New(eeDump)
	ds := NewDRDPStream(packetData)
	interp.drdpStream = ds

	// Allocate drdp_t context
	drdpCtx := interp.heapAlloc(0x10000) // large enough for drdp state

	// Set up a buffer_t on the stack with the packet data
	// buffer_init_read(buf, data, size, 1)
	bufAddr := interp.heapAlloc(256) // buffer_t struct
	packetBuf := interp.heapAlloc(uint32(len(packetData)) + 16)
	for i, b := range packetData {
		interp.store8(packetBuf+uint32(i), b)
	}

	// Call drdp_process_packet(drdp, data, size)
	result := interp.Run(0x004B64E0, nil, drdpCtx, packetBuf, uint32(len(packetData)))

	_ = bufAddr

	return DRDPResult{
		Result: result,
		Reads:  ds.Reads,
		Steps:  interp.Steps,
	}
}
