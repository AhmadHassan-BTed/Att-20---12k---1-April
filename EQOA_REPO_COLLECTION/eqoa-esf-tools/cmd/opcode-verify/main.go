// opcode-verify runs PS2 CLIENT opcode handlers on real packet data
// and dumps the complete read trace + entity struct writes.
//
// Usage:
//
//	opcode-verify --handler 0x00BD1F10 --hex "82 65 cc 27 ..."
//	opcode-verify --batch --pcap capture.pcap
package main

import (
	"encoding/binary"
	"encoding/hex"
	"flag"
	"fmt"
	"os"
	"sort"
	"strings"

	"github.com/DabDavis/eqoa-esf-tools/pkg/mips"
)

func main() {
	handlerFlag := flag.String("handler", "", "Handler address (hex, e.g. 0x00BD1F10)")
	hexFlag := flag.String("hex", "", "Packet data as hex string")
	fileFlag := flag.String("file", "", "Packet data from binary file")
	batchFlag := flag.Bool("batch", false, "Batch verify all reliable messages from pcap")
	synthFlag := flag.Bool("synth", false, "Run synthetic payload tests for all known handlers")
	pcapFlag := flag.String("pcap", "", "PCAP file for batch mode")
	dumpFlag := flag.String("dump", "/home/sdg/claude-eqoa/memory-dumps/go-inspect2.eeMemory", "EE memory dump")
	flag.Parse()

	if *synthFlag {
		eeDump, err := os.ReadFile(*dumpFlag)
		if err != nil {
			fmt.Fprintf(os.Stderr, "EE dump: %v\n", err)
			os.Exit(1)
		}
		batchSyntheticVerify(eeDump)
		return
	}

	if *batchFlag {
		if *pcapFlag == "" {
			fmt.Fprintln(os.Stderr, "Provide --pcap for batch mode")
			os.Exit(1)
		}
		eeDump, err := os.ReadFile(*dumpFlag)
		if err != nil {
			fmt.Fprintf(os.Stderr, "EE dump: %v\n", err)
			os.Exit(1)
		}
		batchVerify(eeDump, *pcapFlag)
		return
	}

	if *handlerFlag == "" {
		fmt.Fprintf(os.Stderr, "Usage: opcode-verify --handler 0xADDR --hex \"bytes...\"\n")
		fmt.Fprintf(os.Stderr, "       opcode-verify --batch --pcap capture.pcap\n")
		fmt.Fprintf(os.Stderr, "\nKnown handlers (%d):\n", len(opcodeHandlers))
		for _, h := range opcodeHandlers {
			fmt.Fprintf(os.Stderr, "  0x%04X  0x%08X  %s\n", h.opcode, h.addr, h.name)
		}
		os.Exit(1)
	}

	var handlerAddr uint32
	fmt.Sscanf(*handlerFlag, "0x%x", &handlerAddr)
	if handlerAddr == 0 {
		fmt.Sscanf(*handlerFlag, "%x", &handlerAddr)
	}

	var packetData []byte
	if *hexFlag != "" {
		clean := strings.ReplaceAll(*hexFlag, " ", "")
		var err error
		packetData, err = hex.DecodeString(clean)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Bad hex: %v\n", err)
			os.Exit(1)
		}
	} else if *fileFlag != "" {
		var err error
		packetData, err = os.ReadFile(*fileFlag)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Read file: %v\n", err)
			os.Exit(1)
		}
	} else {
		fmt.Fprintf(os.Stderr, "Provide --hex or --file\n")
		os.Exit(1)
	}

	eeDump, err := os.ReadFile(*dumpFlag)
	if err != nil {
		fmt.Fprintf(os.Stderr, "EE dump: %v\n", err)
		os.Exit(1)
	}

	handlerName := fmt.Sprintf("0x%08X", handlerAddr)
	for _, h := range opcodeHandlers {
		if h.addr == handlerAddr {
			handlerName = h.name
			break
		}
	}

	runSingleHandler(eeDump, handlerAddr, handlerName, packetData)
}

func runSingleHandler(eeDump []byte, handlerAddr uint32, handlerName string, packetData []byte) {
	fmt.Printf("=== %s (0x%08X) ===\n", handlerName, handlerAddr)
	fmt.Printf("Packet: %d bytes\n", len(packetData))
	fmt.Printf("Hex: %s\n\n", hex.EncodeToString(packetData))

	result := mips.RunOpcodeHandlerFull(eeDump, handlerAddr, packetData)

	fmt.Printf("Result: %d (0x%08X)\n", result.Result, uint32(result.Result))
	fmt.Printf("Steps: %d\n", result.Steps)

	fmt.Printf("\n--- Reads (%d) ---\n", len(result.Reads))
	for i, r := range result.Reads {
		switch r.Type {
		case "zigzag":
			fmt.Printf("  [%d] zigzag  pos=%d size=%d → %d\n", i, r.Pos, r.Size, r.IVal)
		case "string":
			fmt.Printf("  [%d] string  pos=%d size=%d → %q\n", i, r.Pos, r.Size, r.SVal)
		case "raw":
			if r.Size <= 4 {
				fmt.Printf("  [%d] raw%d    pos=%d → %d (0x%X)\n", i, r.Size, r.Pos, r.IVal, uint32(r.IVal))
			} else {
				fmt.Printf("  [%d] raw     pos=%d size=%d\n", i, r.Pos, r.Size)
			}
		default:
			fmt.Printf("  [%d] %s  pos=%d size=%d ival=%d\n", i, r.Type, r.Pos, r.Size, r.IVal)
		}
	}

	if len(result.Writes) > 0 {
		fmt.Printf("\n--- Entity Writes (%d bytes) ---\n", len(result.Writes))
		type writeGroup struct {
			offset uint32
			bytes  map[uint32]byte
		}
		groups := map[uint32]*writeGroup{}
		for _, w := range result.Writes {
			aligned := w.Offset & ^uint32(3)
			g, ok := groups[aligned]
			if !ok {
				g = &writeGroup{offset: aligned, bytes: make(map[uint32]byte)}
				groups[aligned] = g
			}
			g.bytes[w.Offset] = byte(w.Value)
		}
		var offsets []uint32
		for off := range groups {
			offsets = append(offsets, off)
		}
		sort.Slice(offsets, func(i, j int) bool { return offsets[i] < offsets[j] })
		for _, off := range offsets {
			g := groups[off]
			var val uint32
			for byteOff, b := range g.bytes {
				shift := (byteOff - off) * 8
				val |= uint32(b) << shift
			}
			fmt.Printf("  entity+0x%04X = 0x%08X (%d)\n", off, val, int32(val))
		}
	}
}

// batchVerify extracts reliable messages from a pcap and runs each through PS2 handlers.
func batchVerify(eeDump []byte, pcapPath string) {
	messages := extractMessages(pcapPath)
	fmt.Printf("Extracted %d reliable messages from %s\n\n", len(messages), pcapPath)

	pass, fail, skip := 0, 0, 0
	opcodeCounts := map[uint16]int{}
	opcodeResults := map[uint16]string{} // first result per opcode

	// Filter to complete messages only (skip truncated continuations)
	var complete []extractedMessage
	for _, msg := range messages {
		if msg.complete {
			complete = append(complete, msg)
		}
	}
	fmt.Printf("Complete (non-fragmented): %d\n\n", len(complete))

	for _, msg := range complete {
		opcodeCounts[msg.opcode]++

		handler := lookupHandler(msg.opcode)
		if handler == nil {
			skip++
			continue
		}

		// Run PS2 handler on message payload (after 2-byte opcode)
		payload := msg.data[2:] // skip opcode bytes
		result := mips.RunOpcodeHandlerFull(eeDump, handler.addr, payload)

		// Calculate bytes consumed
		totalRead := 0
		for _, r := range result.Reads {
			totalRead += r.Size
		}

		status := "OK"
		if result.Steps == 0 {
			status = "NO_EXEC"
			fail++
		} else if totalRead == 0 && len(payload) > 0 {
			status = "NO_READS"
			fail++
		} else {
			pass++
		}

		// Record first result per opcode
		if _, seen := opcodeResults[msg.opcode]; !seen {
			opcodeResults[msg.opcode] = fmt.Sprintf("%-6s reads=%d/%dB writes=%d steps=%d",
				status, len(result.Reads), totalRead, len(result.Writes), result.Steps)
		}
	}

	// Summary by opcode
	fmt.Printf("=== Per-Opcode Summary ===\n")
	var opcodes []uint16
	for op := range opcodeResults {
		opcodes = append(opcodes, op)
	}
	sort.Slice(opcodes, func(i, j int) bool { return opcodes[i] < opcodes[j] })

	for _, op := range opcodes {
		handler := lookupHandler(op)
		name := "???"
		if handler != nil {
			name = handler.name
		}
		fmt.Printf("  0x%04X %-24s count=%-3d %s\n", op, name, opcodeCounts[op], opcodeResults[op])
	}

	// Unhandled opcodes
	var unhandled []uint16
	for op, count := range opcodeCounts {
		if lookupHandler(op) == nil {
			unhandled = append(unhandled, op)
			_ = count
		}
	}
	if len(unhandled) > 0 {
		sort.Slice(unhandled, func(i, j int) bool { return unhandled[i] < unhandled[j] })
		fmt.Printf("\n  Unmapped opcodes: ")
		for _, op := range unhandled {
			fmt.Printf("0x%04X(%d) ", op, opcodeCounts[op])
		}
		fmt.Println()
	}

	fmt.Printf("\n=== TOTAL: %d pass, %d fail, %d skip (unmapped) ===\n", pass, fail, skip)
}

type extractedMessage struct {
	opcode   uint16
	data     []byte // includes 2-byte opcode prefix
	complete bool   // true if message fits entirely in segment
}

// extractMessages parses a pcap and extracts reliable message payloads.
func extractMessages(pcapPath string) []extractedMessage {
	data, err := os.ReadFile(pcapPath)
	if err != nil {
		fmt.Fprintf(os.Stderr, "pcap: %v\n", err)
		return nil
	}

	if len(data) < 24 {
		return nil
	}
	// Skip global header
	pos := 24

	var messages []extractedMessage

	for pos+16 <= len(data) {
		// Packet header
		inclLen := int(binary.LittleEndian.Uint32(data[pos+8 : pos+12]))
		pos += 16

		if pos+inclLen > len(data) {
			break
		}

		pkt := data[pos : pos+inclLen]
		pos += inclLen

		// Ethernet(14) + IP header
		if len(pkt) < 42 {
			continue
		}
		if pkt[23] != 17 { // not UDP
			continue
		}
		srcPort := int(pkt[34])<<8 | int(pkt[35])
		dstPort := int(pkt[36])<<8 | int(pkt[37])
		if srcPort != 10070 && dstPort != 10070 {
			continue
		}

		udp := pkt[42:]
		if len(udp) < 8 {
			continue
		}

		// Parse DRDP: body = udp[:-4], endpoints at [0:4], segment at [4:]
		body := udp[:len(udp)-4]
		if len(body) < 6 {
			continue
		}

		seg := body[4:]

		// Parse segment header varint
		segPos := 0
		flagsSize, n := readVarint64(seg[segPos:])
		segPos += n
		flags := uint32(flagsSize & 0xFFFFF800)
		size := int(flagsSize & 0x7FF)

		// Skip header fields
		if flags&0x2000 != 0 { segPos += 4 }
		if flags&0x40000 != 0 { segPos += 4 }
		if flags&0x8000 != 0 { segPos += 8 }
		if flags&0x1000 == 0 {
			_, vn := readVarint64(seg[segPos:])
			segPos += vn
		}
		if flags&0x0800 != 0 {
			_, vn := readVarint64(seg[segPos:])
			segPos += vn
		}

		bodyStart := segPos
		bodyEnd := bodyStart + size
		if bodyEnd > len(seg) {
			bodyEnd = len(seg)
		}
		if segPos >= bodyEnd {
			continue
		}

		// Parse segment body
		bodyFlags := seg[segPos]
		segPos++

		if bodyFlags&0x40 != 0 { segPos += 4 } // instance ACK
		segPos += 2                              // arrival seq
		if bodyFlags&0x01 != 0 {
			segPos += 2 // flush ACK
			if bodyFlags&0x04 != 0 {
				_, vn := readVarint64(seg[segPos:])
				segPos += vn
			}
		}
		if bodyFlags&0x02 != 0 {
			segPos += 2 // guaranteed ACK
			if bodyFlags&0x08 != 0 {
				_, vn := readVarint64(seg[segPos:])
				segPos += vn
			}
		}
		if bodyFlags&0x10 != 0 {
			for segPos < bodyEnd {
				ch := seg[segPos]
				segPos++
				if ch == 0xF8 {
					break
				}
				segPos += 2 // seq
			}
		}

		// Parse messages
		for segPos < bodyEnd {
			if segPos >= len(seg) {
				break
			}
			msgType := seg[segPos]
			segPos++
			if msgType == 0xF8 {
				break
			}
			if segPos+2 > bodyEnd {
				break
			}
			msgSize := int(seg[segPos]) | int(seg[segPos+1])<<8
			segPos += 2

			// Reliable messages (0xF9, 0xFB) have seq + opcode + data
			if msgType == 0xF9 || msgType == 0xFB {
				if segPos+2 > bodyEnd {
					break
				}
				segPos += 2 // seq

				remaining := bodyEnd - segPos
				isComplete := msgSize <= remaining
				if msgSize > remaining {
					msgSize = remaining
				}
				if msgSize >= 2 {
					opcode := uint16(seg[segPos]) | uint16(seg[segPos+1])<<8
					msgData := make([]byte, msgSize)
					copy(msgData, seg[segPos:segPos+msgSize])
					messages = append(messages, extractedMessage{
						opcode:   opcode,
						data:     msgData,
						complete: isComplete,
					})
				}
				segPos += msgSize
				continue
			}

			// Unreliable (0xFC) — opcode + data
			if msgType == 0xFC {
				remaining := bodyEnd - segPos
				isComplete := msgSize <= remaining
				if msgSize > remaining {
					msgSize = remaining
				}
				if msgSize >= 2 {
					opcode := uint16(seg[segPos]) | uint16(seg[segPos+1])<<8
					msgData := make([]byte, msgSize)
					copy(msgData, seg[segPos:segPos+msgSize])
					messages = append(messages, extractedMessage{
						opcode:   opcode,
						data:     msgData,
						complete: isComplete,
					})
				}
				segPos += msgSize
				continue
			}

			// State channel — skip (not opcode messages)
			if msgType < 0xF8 {
				segPos += 2 // seq
				segPos++    // refnum
				// Skip RLE data until we hit segment end or next message
				for segPos < bodyEnd {
					if seg[segPos] == 0x00 {
						segPos++
						break
					}
					rb := seg[segPos]
					segPos++
					var copyCount int
					if rb&0x80 != 0 {
						copyCount = int(rb & 0x7F)
						segPos++ // zero count
					} else {
						copyCount = int(rb >> 4)
					}
					segPos += copyCount
				}
				continue
			}

			// Unknown — skip
			segPos += msgSize
		}
	}

	return messages
}

func readVarint64(data []byte) (uint64, int) {
	var val uint64
	var shift uint
	for i, b := range data {
		val |= uint64(b&0x7F) << shift
		shift += 7
		if b&0x80 == 0 {
			return val, i + 1
		}
		if i >= 9 {
			return val, i + 1
		}
	}
	return val, len(data)
}

// batchSyntheticVerify runs each handler with synthetic test payloads
// constructed from the documented wire formats.
func batchSyntheticVerify(eeDump []byte) {
	type testCase struct {
		opcode  uint16
		name    string
		addr    uint32
		payload []byte
		desc    string
	}

	// Helper: zigzag encode int32 to varint bytes
	zigzagEncode := func(v int32) []byte {
		n := uint32((v << 1) ^ (v >> 31))
		var buf []byte
		for n >= 0x80 {
			buf = append(buf, byte(n)|0x80)
			n >>= 7
		}
		buf = append(buf, byte(n))
		return buf
	}

	// Helper: raw int32 LE
	raw32 := func(v int32) []byte {
		b := make([]byte, 4)
		b[0] = byte(v)
		b[1] = byte(v >> 8)
		b[2] = byte(v >> 16)
		b[3] = byte(v >> 24)
		return b
	}

	// Helper: string with 4-byte length prefix
	encString := func(s string, maxLen int) []byte {
		b := raw32(int32(len(s)))
		b = append(b, []byte(s)...)
		return b
	}

	// Build test payloads from decompiled wire formats
	var tests []testCase

	// 0x0020 GrantXP: zigzag(HP) + zigzag(MaxHP)
	{
		var p []byte
		p = append(p, zigzagEncode(500)...)
		p = append(p, zigzagEncode(1000)...)
		tests = append(tests, testCase{0x0020, "GrantXP", 0x00BD19D8, p, "HP=500, MaxHP=1000"})
	}

	// 0x001C NPCInteraction: raw(4,1) → interactionCounter
	tests = append(tests, testCase{0x001C, "NPCInteraction", 0x00BD1A24, raw32(42), "counter=42"})

	// 0x0775 ResourceUpdate: raw(4,1) + raw(1,1)
	{
		p := raw32(12345)
		p = append(p, 0x17) // animAction=23
		tests = append(tests, testCase{0x0775, "ResourceUpdate", 0x00BD1930, p, "entityID=12345, action=23"})
	}

	// 0x001D UpdateTrainingPts: zigzag(tp)
	tests = append(tests, testCase{0x001D, "UpdateTrainingPts", 0x00BD271C, zigzagEncode(150), "tp=150"})

	// 0x0052 PlayerTunar: zigzag(tunar)
	tests = append(tests, testCase{0x0052, "PlayerTunar", 0x00BD263C, zigzagEncode(99999), "tunar=99999"})

	// 0x1253 ConfirmBankTunar: zigzag(tunar)
	tests = append(tests, testCase{0x1253, "ConfirmBankTunar", 0x00BD2690, zigzagEncode(50000), "tunar=50000"})

	// 0x00B1 CastingCombat: raw(4,1)*2 + raw(1,1) + raw(2,1) + raw(2,1) + raw(4,1)
	{
		var p []byte
		p = append(p, raw32(100)...)   // targetID
		p = append(p, raw32(200)...)   // sourceID
		p = append(p, 0x01)            // unk
		p = append(p, 0xE8, 0x03)     // timer_ms = 1000
		p = append(p, 0x00, 0x00)     // pad
		p = append(p, raw32(0x1234)...) // animHash
		tests = append(tests, testCase{0x00B1, "CastingCombat", 0x00BD1F10, p, "target=100,source=200,timer=1000"})
	}

	// 0x00DB DamageIndication: zigzag(damage)
	tests = append(tests, testCase{0x00DB, "DamageIndication", 0x00BD2D70, zigzagEncode(-50), "damage=-50"})

	// 0x00D9 HPResourceUpdate: zigzag(hp) + zigzag(maxhp) + zigzag(power) + zigzag(maxpower)
	{
		var p []byte
		p = append(p, zigzagEncode(800)...)
		p = append(p, zigzagEncode(1000)...)
		p = append(p, zigzagEncode(400)...)
		p = append(p, zigzagEncode(500)...)
		tests = append(tests, testCase{0x00D9, "HPResourceUpdate", 0x00BD2D5C, p, "hp=800/1000 pow=400/500"})
	}

	// 0x0046 DialogueBox: string_read(512)
	tests = append(tests, testCase{0x0046, "DialogueBox", 0x00BD1A4C, encString("Hello adventurer!", 512), "text='Hello adventurer!'"})

	// 0x0A7B ColoredChat: raw(1,1) + string_read(512) + string_read(512)
	{
		var p []byte
		p = append(p, 0x03) // color type
		p = append(p, encString("Player", 512)...)
		p = append(p, encString("Hello world!", 512)...)
		tests = append(tests, testCase{0x0A7B, "ColoredChat", 0x00BD1590, p, "color=3 name='Player' msg='Hello world!'"})
	}

	// 0x07D1 Camera1: raw(4,1)
	tests = append(tests, testCase{0x07D1, "Camera1", 0x00BD2E08, raw32(0x03), "camera=3"})

	// 0x0060 AdjustItemHP: zigzag(slot) + zigzag(hp)
	{
		var p []byte
		p = append(p, zigzagEncode(5)...)
		p = append(p, zigzagEncode(100)...)
		tests = append(tests, testCase{0x0060, "AdjustItemHP", 0x00BD2A14, p, "slot=5, hp=100"})
	}

	// 0x0013 Time: raw(4,1)
	tests = append(tests, testCase{0x0013, "Time", 0x00BD2AC8, raw32(360000), "time=360000"})

	// 0x00C5 WeatherControl: zigzag(weatherA) + zigzag(weatherB)
	{
		var p []byte
		p = append(p, zigzagEncode(1)...)
		p = append(p, zigzagEncode(2)...)
		tests = append(tests, testCase{0x00C5, "WeatherControl", 0x00BD2CC0, p, "weatherA=1 weatherB=2"})
	}

	// 0x00C7 SkyboxWeather: zigzag(weatherB)
	tests = append(tests, testCase{0x00C7, "SkyboxWeather", 0x00BD2CEC, zigzagEncode(3), "weatherB=3"})

	// 0x00F8 PlayerSpeed: zigzag(speed)
	tests = append(tests, testCase{0x00F8, "PlayerSpeed", 0x00BD2BE0, zigzagEncode(100), "speed=100"})

	// 0x007B UpdateQuestProgress: zigzag(questID) + zigzag(progress)
	{
		var p []byte
		p = append(p, zigzagEncode(42)...)
		p = append(p, zigzagEncode(3)...)
		tests = append(tests, testCase{0x007B, "UpdateQuestProgress", 0x00BD2B38, p, "quest=42 progress=3"})
	}

	// 0x0000 DiscVersion: raw(4,1)
	tests = append(tests, testCase{0x0000, "DiscVersion", 0x00BD1AC8, raw32(0x12), "version=18"})

	// 0x0036 QuestStageComplete: raw(1,1) + zigzag(xp)
	{
		var p []byte
		p = append(p, 0x01)                // questNotifyType
		p = append(p, zigzagEncode(250)...) // questXPReward
		tests = append(tests, testCase{0x0036, "QuestStageComplete", 0x00BD26CC, p, "type=1 xp=250"})
	}

	// 0x00B8 EntityDetailUpdate: raw(4,1) + zigzag(channelID)
	{
		var p []byte
		p = append(p, raw32(5000)...)
		p = append(p, zigzagEncode(3)...)
		tests = append(tests, testCase{0x00B8, "EntityDetailUpdate", 0x00BD212C, p, "session=5000 channel=3"})
	}

	// 0x00FD EntityDataUpdate: raw(4,1) + raw(4,1) + raw(4,1)
	{
		var p []byte
		p = append(p, raw32(5000)...) // sessionObjID
		p = append(p, raw32(7)...)    // entityChannelID
		p = append(p, raw32(1)...)    // dataValue
		tests = append(tests, testCase{0x00FD, "EntityDataUpdate", 0x00BD2118, p, "session=5000 ch=7 val=1"})
	}

	// 0x0085 LootOptions: raw(1,1)
	tests = append(tests, testCase{0x0085, "LootOptions", 0x00BD1E88, []byte{0x02}, "mode=2 (RoundRobin)"})

	// 0x0019 ClientLoot: raw(1,1) + zigzag(itemID)
	{
		var p []byte
		p = append(p, 0x03)                  // slotIndex
		p = append(p, zigzagEncode(12345)...) // itemID
		tests = append(tests, testCase{0x0019, "ClientLoot", 0x00BD1D88, p, "slot=3 item=12345"})
	}

	// 0x001A LootResult: no reads
	tests = append(tests, testCase{0x001A, "LootResult", 0x00BD1DC0, []byte{}, "no data"})

	// 0x005E CharacterDied: no reads
	tests = append(tests, testCase{0x005E, "CharacterDied", 0x00BD2A04, []byte{}, "no data"})

	// 0x003D ArrangeItem: raw(1,1)*7 (3 slot pairs + flags)
	{
		p := []byte{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00}
		tests = append(tests, testCase{0x003D, "ArrangeItem", 0x00BD1CA4, p, "srcA=1 dstA=2 srcB=3 dstB=4 srcC=5 dstC=6 flags=0"})
	}

	// 0x003F EquipItem: raw(1,1) + raw(1,1) + raw(1,1) + raw(1,1) + zigzag(tint)
	{
		var p []byte
		p = append(p, 0x05)                 // srcSlot
		p = append(p, 0x01)                 // equipSlot
		p = append(p, 0x00)                 // equipFlags
		p = append(p, 0x00)                 // tintSlot
		p = append(p, zigzagEncode(0)...)   // tintValue
		tests = append(tests, testCase{0x003F, "EquipItem", 0x00BD1D20, p, "src=5 equip=1"})
	}

	// 0x0040 UnequipItem: raw(1,1) + raw(1,1) + zigzag(tint)
	{
		var p []byte
		p = append(p, 0x00)               // unequipFlags
		p = append(p, 0x00)               // tintSlot
		p = append(p, zigzagEncode(0)...) // tintValue
		tests = append(tests, testCase{0x0040, "UnequipItem", 0x00BD1D60, p, "flags=0"})
	}

	// 0x07B0 ZoneTransferInit: zigzag(zoneID) + string(zonePath)
	{
		var p []byte
		p = append(p, zigzagEncode(100)...)
		p = append(p, encString("/zones/freeport", 256)...)
		tests = append(tests, testCase{0x07B0, "ZoneTransferInit", 0x00BD2DDC, p, "zone=100 path=/zones/freeport"})
	}

	// 0x0790 ServerAssignment: zigzag(serverID)
	tests = append(tests, testCase{0x0790, "ServerAssignment", 0x00BD2CAC, zigzagEncode(1), "server=1"})

	// 0x07A3 ChatMessage: zigzag(chatType) + string(msg)
	{
		var p []byte
		p = append(p, zigzagEncode(1)...)
		p = append(p, encString("Test chat message", 512)...)
		tests = append(tests, testCase{0x07A3, "ChatMessage", 0x00BD2D28, p, "type=1 msg='Test chat message'"})
	}

	// 0x00CE ErrorMessage: raw(4,1) + string(512) + string(512)
	{
		var p []byte
		p = append(p, raw32(1)...)
		p = append(p, encString("Error", 512)...)
		p = append(p, encString("Details", 512)...)
		tests = append(tests, testCase{0x00CE, "ErrorMessage", 0x00BD1628, p, "code=1"})
	}

	// 0x07D2 CharModifiedDC: no reads (state change only)
	tests = append(tests, testCase{0x07D2, "CharModifiedDC", 0x00BD2E2C, []byte{}, "no data"})

	// 0x0010 ClassicTargetInfo: zigzag*6 + more
	{
		var p []byte
		p = append(p, zigzagEncode(100)...)  // targetID or field1
		p = append(p, zigzagEncode(50)...)   // field2
		p = append(p, zigzagEncode(200)...)  // field3
		p = append(p, zigzagEncode(1000)...) // field4
		p = append(p, zigzagEncode(500)...)  // field5
		p = append(p, zigzagEncode(0)...)    // field6
		tests = append(tests, testCase{0x0010, "ClassicTargetInfo", 0x00BD277C, p, "target fields"})
	}

	// 0x069E GroupChat: raw(4,1) + blob(2048) + blob(2048)
	{
		var p []byte
		p = append(p, raw32(999)...)   // senderEntityID
		// blob: 4-byte charCount + charCount*2 bytes UTF-16
		msg := "Hi"
		p = append(p, raw32(int32(len(msg)))...)
		for _, c := range msg {
			p = append(p, byte(c), 0) // UTF-16LE
		}
		name := "Bob"
		p = append(p, raw32(int32(len(name)))...)
		for _, c := range name {
			p = append(p, byte(c), 0)
		}
		tests = append(tests, testCase{0x069E, "GroupChat", 0x00BD15B8, p, "sender=999 msg='Hi' name='Bob'"})
	}

	// 0x03D5 BuddyListEntry: complex — try basic read
	{
		var p []byte
		p = append(p, zigzagEncode(1)...) // entryType or count
		tests = append(tests, testCase{0x03D5, "BuddyListEntry", 0x00BD2280, p, "entry test"})
	}

	// 0x007D DeleteQuest: string_read(64) + raw(1,1) + more sub-function reads
	{
		var p []byte
		p = append(p, encString("QuestNPC", 64)...)
		p = append(p, 0x00) // chatColorCode
		tests = append(tests, testCase{0x007D, "DeleteQuest", 0x00BD2C28, p, "name='QuestNPC' color=0"})
	}

	// 0x00D3 NPCMessage: zigzag(npcID) + string(message)
	{
		var p []byte
		p = append(p, zigzagEncode(500)...)
		p = append(p, encString("Welcome traveler", 512)...)
		tests = append(tests, testCase{0x00D3, "NPCMessage", 0x00BD2C48, p, "npc=500 msg='Welcome traveler'"})
	}

	// 0x00B3 FullEntityCreate: raw(4,1) + raw(4,1) + full entity data
	{
		var p []byte
		p = append(p, raw32(1001)...) // sessionObjID
		p = append(p, raw32(1)...)    // entitySlotID
		// Entity data buffer would follow (func_00BDCBE0) — 15 zigzag fields etc.
		// Just test the initial reads
		tests = append(tests, testCase{0x00B3, "FullEntityCreate", 0x00BD1FE0, p, "session=1001 slot=1"})
	}

	// 0x006E StubNOP: no reads (just falls through to shared tail)
	tests = append(tests, testCase{0x006E, "StubNOP", 0x00BD2B28, []byte{}, "no data"})

	// 0x00D7 InventoryFull: falls through to weather finalize tail
	tests = append(tests, testCase{0x00D7, "InventoryFull", 0x00BD2D18, []byte{}, "no data (tail only)"})

	// ── Remaining handlers (batch 2) ──

	// 0x000A PrepareItem: string_read(24)
	tests = append(tests, testCase{0x000A, "PrepareItem", 0x00BD2A94, encString("TestItem", 24), "name='TestItem'"})

	// 0x000D DSPDataSection: raw(4,1) + zigzag + zigzag + sub-func
	{
		var p []byte
		p = append(p, raw32(1)...)          // dspSectionID
		p = append(p, zigzagEncode(5)...)   // rowCount
		p = append(p, zigzagEncode(3)...)   // fieldCount
		tests = append(tests, testCase{0x000D, "DSPDataSection", 0x00BD21EC, p, "section=1 rows=5 fields=3"})
	}

	// 0x0016 ClientCloseLoot: no reads
	tests = append(tests, testCase{0x0016, "ClientCloseLoot", 0x00BD1E50, []byte{}, "no data"})

	// 0x0018 Loot: delegates to sub-func (needs entity state)
	tests = append(tests, testCase{0x0018, "Loot", 0x00BD1DD0, []byte{}, "delegates to sub-func"})

	// 0x002C CharacterSelect: delegates to func_00BD2E80 (needs entity clientState=2/3)
	tests = append(tests, testCase{0x002C, "CharacterSelect", 0x00BD1BD4, []byte{}, "needs clientState 2/3"})

	// 0x002E WorldEntry: no reads (sends msg 0x010D, decrements counter)
	tests = append(tests, testCase{0x002E, "WorldEntry", 0x00BD1B5C, []byte{}, "no data"})

	// 0x002F NameTaken: no reads (sends error msg)
	tests = append(tests, testCase{0x002F, "NameTaken", 0x00BD1B98, []byte{}, "no data"})

	// 0x0034 QuestPopupMulti: delegates to func_00BDBD58
	{
		var p []byte
		p = append(p, zigzagEncode(1)...) // quest section count or ID
		tests = append(tests, testCase{0x0034, "QuestPopupMulti", 0x00BD1A38, p, "quest test"})
	}

	// 0x003B AddInvItem: zigzag*5 + raw(4,1) + sub-func
	{
		var p []byte
		for i := 0; i < 5; i++ {
			p = append(p, zigzagEncode(int32(i+1))...)
		}
		p = append(p, raw32(100)...) // itemField6
		tests = append(tests, testCase{0x003B, "AddInvItem", 0x00BD1C24, p, "5 zigzag + raw32"})
	}

	// 0x0045 QuestDialogueResp: zigzag(responseID)
	tests = append(tests, testCase{0x0045, "QuestDialogueResp", 0x00BD2B08, zigzagEncode(5), "response=5"})

	// 0x0056 TradeUI: raw(1,1) + raw(4,1)
	{
		var p []byte
		p = append(p, 0x01)            // tradeSubOpcode
		p = append(p, raw32(999)...)   // tradeTargetID
		tests = append(tests, testCase{0x0056, "TradeUI", 0x00BD28A8, p, "sub=1 target=999"})
	}

	// 0x0057 TradeRequest: raw(4,1) + raw(4,1)
	{
		var p []byte
		p = append(p, raw32(100)...) // requestorObjID
		p = append(p, raw32(200)...) // targetObjID
		tests = append(tests, testCase{0x0057, "TradeRequest", 0x00BD2868, p, "requestor=100 target=200"})
	}

	// 0x0061 BlackSmithMenu: raw(4,1)
	tests = append(tests, testCase{0x0061, "BlackSmithMenu", 0x00BD2A5C, raw32(555), "itemID=555"})

	// 0x0065 ItemListUpdate: complex (char select + inv + entity refresh)
	tests = append(tests, testCase{0x0065, "ItemListUpdate", 0x00BD1B7C, []byte{}, "complex handler"})

	// 0x00B4 EntityAppearanceUpdate: raw(4,1) + raw(1,1) + raw(1,1)
	{
		var p []byte
		p = append(p, raw32(5000)...) // sessionObjID
		p = append(p, 0x01)           // updateTypeA
		p = append(p, 0x02)           // updateTypeB
		tests = append(tests, testCase{0x00B4, "EntityAppearanceUpdate", 0x00BD1FF4, p, "session=5000 typeA=1 typeB=2"})
	}

	// 0x00B5 EntityUpdate: raw(4,1) + raw(1,1) + raw(1,1)
	{
		var p []byte
		p = append(p, raw32(5001)...) // sessionObjID
		p = append(p, 0x03)           // updateTypeA
		p = append(p, 0x04)           // updateTypeB
		tests = append(tests, testCase{0x00B5, "EntityUpdate", 0x00BD2064, p, "session=5001 typeA=3 typeB=4"})
	}

	// 0x00B6 EntityUpdateB: raw(4,1) + raw(1,1) + raw(1,1)
	{
		var p []byte
		p = append(p, raw32(5002)...) // sessionObjID
		p = append(p, 0x05)           // updateTypeA
		p = append(p, 0x06)           // updateTypeB
		tests = append(tests, testCase{0x00B6, "EntityUpdateB", 0x00BD20C0, p, "session=5002 typeA=5 typeB=6"})
	}

	// 0x00B7 MerchantBox: same as DSP (raw(4,1) + zigzag + zigzag)
	{
		var p []byte
		p = append(p, raw32(2)...)          // sectionID
		p = append(p, zigzagEncode(10)...)  // rowCount
		p = append(p, zigzagEncode(4)...)   // fieldCount
		tests = append(tests, testCase{0x00B7, "MerchantBox", 0x00BD21EC, p, "section=2 rows=10 fields=4"})
	}

	// 0x00B9 EntityAttributeUpdate: raw(1,1) + raw(4,1) + zigzag
	{
		var p []byte
		p = append(p, 0x01)                  // attributeType
		p = append(p, raw32(5003)...)         // sessionObjID
		p = append(p, zigzagEncode(42)...)    // attributeValue
		tests = append(tests, testCase{0x00B9, "EntityAttributeUpdate", 0x00BD2168, p, "attr=1 session=5003 val=42"})
	}

	// 0x00BA EntityFullStateUpdate: raw(1,1) + float3 + float3 (delegates)
	{
		var p []byte
		p = append(p, 0x01) // cameraEffectType
		// Would need float3_read — just test first byte
		tests = append(tests, testCase{0x00BA, "EntityFullStateUpdate", 0x00BD21BC, p, "cameraType=1"})
	}

	// 0x00FC EntityStateRefresh: raw(1,1)
	tests = append(tests, testCase{0x00FC, "EntityStateRefresh", 0x00BD21CC, []byte{0x01}, "state=1"})

	// 0x00FF FullEntityRefresh: delegates to func_00BCF2D0
	tests = append(tests, testCase{0x00FF, "FullEntityRefresh", 0x00BD2050, raw32(5004), "session=5004"})

	// 0x03D6 BuddyOnlineStatus: zigzag + string(24)
	{
		var p []byte
		p = append(p, zigzagEncode(3)...)
		p = append(p, encString("BuddyName", 24)...)
		tests = append(tests, testCase{0x03D6, "BuddyOnlineStatus", 0x00BD23CC, p, "idx=3 name='BuddyName'"})
	}

	// 0x03D8 AddBuddyFailed: string(24) + raw(1,1)
	{
		var p []byte
		p = append(p, encString("FailedBuddy", 24)...)
		p = append(p, 0x01) // statusByte
		tests = append(tests, testCase{0x03D8, "AddBuddyFailed", 0x00BD2420, p, "name='FailedBuddy' status=1"})
	}

	// 0x0468 GuildInfo: delegates to sub-funcs (complex)
	tests = append(tests, testCase{0x0468, "GuildInfo", 0x00BD24A8, []byte{}, "complex delegates"})

	// 0x057A GuildInviteStuff: delegates to sub-funcs
	tests = append(tests, testCase{0x057A, "GuildInviteStuff", 0x00BD2510, []byte{}, "complex delegates"})

	// 0x062F ServerDisbandGroup: no documented reads
	tests = append(tests, testCase{0x062F, "ServerDisbandGroup", 0x00BD18C0, []byte{}, "no data"})

	// 0x069A RemoveGroupMember: raw(1,1)
	tests = append(tests, testCase{0x069A, "RemoveGroupMember", 0x00BD17DC, []byte{0x01}, "subOp=1"})

	// 0x069C GroupStuff: raw(1,1)
	tests = append(tests, testCase{0x069C, "GroupStuff", 0x00BD1684, []byte{0x01}, "subOp=1"})

	// 0x0700 AdminCodeResponse: delegates to sub-funcs
	tests = append(tests, testCase{0x0700, "AdminCodeResponse", 0x00BD24C4, []byte{}, "complex delegates"})

	// 0x0729 BadLoginPassword: no reads (disconnect)
	tests = append(tests, testCase{0x0729, "BadLoginPassword", 0x00BD1B00, []byte{}, "disconnect"})

	// 0x072B NotSubscribed: no reads (disconnect)
	tests = append(tests, testCase{0x072B, "NotSubscribed", 0x00BD1B18, []byte{}, "disconnect"})

	// 0x076F BuddyInvite: string(24) + raw(1,1)
	{
		var p []byte
		p = append(p, encString("Inviter", 24)...)
		p = append(p, 0x01) // inviteAction
		tests = append(tests, testCase{0x076F, "BuddyInvite", 0x00BD2434, p, "name='Inviter' action=1"})
	}

	// 0x0771 BuddyListStuff: raw(1,1) + string(24)
	{
		var p []byte
		p = append(p, 0x02) // actionType
		p = append(p, encString("Buddy", 24)...)
		tests = append(tests, testCase{0x0771, "BuddyListStuff", 0x00BD2468, p, "action=2 name='Buddy'"})
	}

	// 0x0773 GroupInviteResponse: delegates to sub-funcs
	tests = append(tests, testCase{0x0773, "GroupInviteResponse", 0x00BD17B8, []byte{}, "complex delegates"})

	// 0x0777 DisconnectNoError: no reads (clean disconnect)
	tests = append(tests, testCase{0x0777, "DisconnectNoError", 0x00BD1C14, []byte{}, "disconnect"})

	// 0x077A OutMatched: enters shared tail (camera + heading)
	tests = append(tests, testCase{0x077A, "OutMatched", 0x00BD280C, []byte{}, "shared tail"})

	// 0x07C1 AdminShutdown: state 18 + tunar + tail
	tests = append(tests, testCase{0x07C1, "AdminShutdown", 0x00BD2630, []byte{}, "state transition"})

	// 0x07E2 FactionPage: delegates to sub-funcs
	tests = append(tests, testCase{0x07E2, "FactionPage", 0x00BD24E0, []byte{}, "complex delegates"})

	// 0x0A7A ClientMessage: blob(2048)
	{
		msg := "System message"
		var p []byte
		p = append(p, raw32(int32(len(msg)))...)
		for _, c := range msg {
			p = append(p, byte(c), 0) // UTF-16LE
		}
		tests = append(tests, testCase{0x0A7A, "ClientMessage", 0x00BD15A4, p, "msg='System message'"})
	}

	// 0x0D41 MailDelivery: delegates to quest/mail sub-func
	tests = append(tests, testCase{0x0D41, "MailDelivery", 0x00BD2548, []byte{}, "complex delegates"})

	// 0x0E02 WhoListResponse: delegates to map/zone sub-func
	tests = append(tests, testCase{0x0E02, "WhoListResponse", 0x00BD2564, []byte{}, "complex delegates"})

	// 0x0F02 WhoListFiltered: delegates to social sub-func
	tests = append(tests, testCase{0x0F02, "WhoListFiltered", 0x00BD2580, []byte{}, "complex delegates"})

	// 0x1102 IgnoreListResult: delegates to ignore sub-func
	tests = append(tests, testCase{0x1102, "IgnoreListResult", 0x00BD259C, []byte{}, "complex delegates"})

	// 0x1206 AuctionStuff: delegates to auction sub-func
	tests = append(tests, testCase{0x1206, "AuctionStuff", 0x00BD26B0, []byte{}, "complex delegates"})

	// 0x124D BankUI: scene validation + bank UI init
	tests = append(tests, testCase{0x124D, "BankUI", 0x00BD2654, []byte{}, "scene + bank init"})

	// 0x1402 ClassMasteryServer: raw(1,1) cmSubCommand
	tests = append(tests, testCase{0x1402, "ClassMasteryServer", 0x00BD25B8, []byte{0x01}, "subCmd=1"})

	// Run all tests
	fmt.Printf("=== Synthetic Opcode Handler Verification ===\n")
	fmt.Printf("Testing %d handlers with constructed payloads\n\n", len(tests))

	pass, fail := 0, 0
	for _, tc := range tests {
		result := mips.RunOpcodeHandlerFull(eeDump, tc.addr, tc.payload)

		totalRead := 0
		for _, r := range result.Reads {
			totalRead += r.Size
		}

		status := "PASS"
		detail := ""
		if result.Steps == 0 {
			status = "FAIL"
			detail = " (no execution)"
			fail++
		} else if len(result.Reads) == 0 && len(tc.payload) > 0 {
			status = "FAIL"
			detail = " (no reads)"
			fail++
		} else {
			pass++
		}

		fmt.Printf("  0x%04X %-24s %s  reads=%d/%dB writes=%d steps=%d%s\n",
			tc.opcode, tc.name, status, len(result.Reads), totalRead,
			len(result.Writes), result.Steps, detail)

		// Show reads for failures
		if status == "FAIL" {
			for i, r := range result.Reads {
				fmt.Printf("         [%d] %s pos=%d size=%d val=%d\n", i, r.Type, r.Pos, r.Size, r.IVal)
			}
		}
	}

	fmt.Printf("\n=== TOTAL: %d pass, %d fail out of %d handlers ===\n", pass, fail, len(tests))
}

func lookupHandler(opcode uint16) *opcodeHandler {
	for i := range opcodeHandlers {
		if opcodeHandlers[i].opcode == opcode {
			return &opcodeHandlers[i]
		}
	}
	return nil
}

type opcodeHandler struct {
	opcode uint16
	addr   uint32
	name   string
}

var opcodeHandlers = []opcodeHandler{
	{0x0000, 0x00BD1AC8, "DiscVersion"},
	{0x000A, 0x00BD2A94, "PrepareItem"},
	{0x000D, 0x00BD21EC, "DSPDataSection"},
	{0x0010, 0x00BD277C, "ClassicTargetInfo"},
	{0x0013, 0x00BD2AC8, "Time"},
	{0x0016, 0x00BD1E50, "ClientCloseLoot"},
	{0x0018, 0x00BD1DD0, "Loot"},
	{0x0019, 0x00BD1D88, "ClientLoot"},
	{0x001A, 0x00BD1DC0, "LootResult"},
	{0x001C, 0x00BD1A24, "NPCInteraction"},
	{0x001D, 0x00BD271C, "UpdateTrainingPts"},
	{0x0020, 0x00BD19D8, "GrantXP"},
	{0x002C, 0x00BD1BD4, "CharacterSelect"},
	{0x002E, 0x00BD1B5C, "WorldEntry"},
	{0x002F, 0x00BD1B98, "NameTaken"},
	{0x0034, 0x00BD1A38, "QuestPopupMulti"},
	{0x0036, 0x00BD26CC, "QuestStageComplete"},
	{0x003A, 0x00BD1C14, "DeleteInvItem"},
	{0x003B, 0x00BD1C24, "AddInvItem"},
	{0x003D, 0x00BD1CA4, "ArrangeItem"},
	{0x003F, 0x00BD1D20, "EquipItem"},
	{0x0040, 0x00BD1D60, "UnequipItem"},
	{0x0045, 0x00BD2B08, "QuestDialogueResp"},
	{0x0046, 0x00BD1A4C, "DialogueBox"},
	{0x0052, 0x00BD263C, "PlayerTunar"},
	{0x0056, 0x00BD28A8, "TradeUI"},
	{0x0057, 0x00BD2868, "TradeRequest"},
	{0x005E, 0x00BD2A04, "CharacterDied"},
	{0x0060, 0x00BD2A14, "AdjustItemHP"},
	{0x0061, 0x00BD2A5C, "BlackSmithMenu"},
	{0x0065, 0x00BD1B7C, "ItemListUpdate"},
	{0x006E, 0x00BD2B28, "StubNOP"},
	{0x007B, 0x00BD2B38, "UpdateQuestProgress"},
	{0x007D, 0x00BD2C28, "DeleteQuest"},
	{0x0085, 0x00BD1E88, "LootOptions"},
	{0x00B1, 0x00BD1F10, "CastingCombat"},
	{0x00B3, 0x00BD1FE0, "FullEntityCreate"},
	{0x00B4, 0x00BD1FF4, "EntityAppearanceUpdate"},
	{0x00B5, 0x00BD2064, "EntityUpdate"},
	{0x00B6, 0x00BD20C0, "EntityUpdateB"},
	{0x00B7, 0x00BD21EC, "MerchantBox"},
	{0x00B8, 0x00BD212C, "EntityDetailUpdate"},
	{0x00B9, 0x00BD2168, "EntityAttributeUpdate"},
	{0x00BA, 0x00BD21BC, "EntityFullStateUpdate"},
	{0x00C5, 0x00BD2CC0, "WeatherControl"},
	{0x00C7, 0x00BD2CEC, "SkyboxWeather"},
	{0x00CD, 0x00BD2168, "CooldownTimer"},
	{0x00CE, 0x00BD1628, "ErrorMessage"},
	{0x00D3, 0x00BD2C48, "NPCMessage"},
	{0x00D7, 0x00BD2D18, "InventoryFull"},
	{0x00D9, 0x00BD2D5C, "HPResourceUpdate"},
	{0x00DB, 0x00BD2D70, "DamageIndication"},
	{0x00EC, 0x00BD2790, "ForceDisconnect"},
	{0x00F8, 0x00BD2BE0, "PlayerSpeed"},
	{0x00FC, 0x00BD21CC, "EntityStateRefresh"},
	{0x00FD, 0x00BD2118, "EntityDataUpdate"},
	{0x00FF, 0x00BD2050, "FullEntityRefresh"},
	{0x03D5, 0x00BD2280, "BuddyListEntry"},
	{0x03D6, 0x00BD23CC, "BuddyOnlineStatus"},
	{0x03D8, 0x00BD2420, "AddBuddyFailed"},
	{0x0468, 0x00BD24A8, "GuildInfo"},
	{0x0579, 0x00BD252C, "GuildWorldEntry"},
	{0x057A, 0x00BD2510, "GuildInviteStuff"},
	{0x062F, 0x00BD18C0, "ServerDisbandGroup"},
	{0x069A, 0x00BD17DC, "RemoveGroupMember"},
	{0x069C, 0x00BD1684, "GroupStuff"},
	{0x069E, 0x00BD15B8, "GroupChat"},
	{0x0700, 0x00BD24C4, "AdminCodeResponse"},
	{0x0728, 0x00BD1B30, "ServerLoginResponse"},
	{0x0729, 0x00BD1B00, "BadLoginPassword"},
	{0x072B, 0x00BD1B18, "NotSubscribed"},
	{0x0758, 0x00BD1A94, "ReconnectWorldEntry"},
	{0x076F, 0x00BD2434, "BuddyInvite"},
	{0x0771, 0x00BD2468, "BuddyListStuff"},
	{0x0773, 0x00BD17B8, "GroupInviteResponse"},
	{0x0775, 0x00BD1930, "ResourceUpdate"},
	{0x0777, 0x00BD1C14, "DisconnectNoError"},
	{0x077A, 0x00BD280C, "OutMatched"},
	{0x0790, 0x00BD2CAC, "ServerAssignment"},
	{0x07A1, 0x00BD25FC, "EntityUpdateAudio"},
	{0x07A2, 0x00BD2610, "EntityUpdateMinimal"},
	{0x07A3, 0x00BD2D28, "ChatMessage"},
	{0x07A4, 0x00BD2790, "LoggedInElsewhere"},
	{0x07B0, 0x00BD2DDC, "ZoneTransferInit"},
	{0x07B2, 0x00BD2DF8, "ZoneTransferConfirm"},
	{0x07C0, 0x00BD2624, "EntityUpdateDirect"},
	{0x07C1, 0x00BD2630, "AdminShutdown"},
	{0x07D1, 0x00BD2E08, "Camera1"},
	{0x07D2, 0x00BD2E2C, "CharModifiedDC"},
	{0x07E0, 0x00BD24FC, "ServerReady"},
	{0x07E2, 0x00BD24E0, "FactionPage"},
	{0x07F0, 0x00BD25E8, "EntityUpdateMapPanel"},
	{0x07F4, 0x00BD25D4, "EntityUpdateFullUI"},
	{0x0A7A, 0x00BD15A4, "ClientMessage"},
	{0x0A7B, 0x00BD1590, "ColoredChat"},
	{0x0D41, 0x00BD2548, "MailDelivery"},
	{0x0E02, 0x00BD2564, "WhoListResponse"},
	{0x0F02, 0x00BD2580, "WhoListFiltered"},
	{0x1102, 0x00BD259C, "IgnoreListResult"},
	{0x1206, 0x00BD26B0, "AuctionStuff"},
	{0x124D, 0x00BD2654, "BankUI"},
	{0x1253, 0x00BD2690, "ConfirmBankTunar"},
	{0x1402, 0x00BD25B8, "ClassMasteryServer"},
}
