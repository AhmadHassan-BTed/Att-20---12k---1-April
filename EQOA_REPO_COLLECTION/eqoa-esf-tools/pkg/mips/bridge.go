package mips

import "math"

// ESF bridge: intercepts JAL calls to known VIObjFile Read* functions
// and routes them through the ESFStream instead of executing MIPS code.
//
// TIER 2 DESIGN: Default to native execution.
// The EE dump contains ALL SUPPORT module code. Every function can execute
// natively. We only intercept functions that:
//   1. Read ESF data (must route through our Go ESF stream)
//   2. Access PS2 hardware (VIRaster GPU, GS registers — can't execute)
//   3. Manage runtime objects (VIDictionary, VIScene — need Go-side tracking)
//   4. Math library functions (powf — faster to intercept than emulate)
//
// Everything else runs as native MIPS code in the interpreter.

// Known Read* function addresses (from SUPPORT symbol map).
// These MUST be intercepted — they read from the ESF data stream.
var readFuncs = map[uint32]string{
	0x003EB0A8: "ReadBegin",
	0x003EB0C8: "ReadBegin2",
	0x003EB3C8: "ReadEnd",
	0x003EA350: "ObjectVersion",
	0x003EA330: "NumSubObjects",
	0x003EA390: "ObjectSize",
	0x003EB6E8: "Read_Ri",
	0x003EB780: "Read_RUi",
	0x003EB948: "Read_Rf",
	0x003EB5B8: "Read_Rs",
	0x003EB550: "Read_RUc",
	0x003EB4E8: "Read_RSc",
	0x003EB480: "Read_Rc",
	0x003EB650: "Read_RUs",
	0x003EB818: "Read_Rl",
	0x003EBA78: "Read_PUci",
}

// hardwareStubs lists functions that touch PS2 hardware and cannot execute
// natively. These return fake success values or heap-allocated pointers.
// Format: address → return behavior.
var hardwareStubs = map[uint32]stubAction{
	// VISurface::Init — runs natively but SetDMAHeaders touches GS.
	// VIScene — pools initialized manually via RunCall. These stubs provide
	// valid return values for sprite/animation/refmap management.
	0x00463698: {ret: retZero},    // CreateSprite
	0x004638D8: {ret: retHeap512}, // Sprite
	0x00463B90: {ret: retZero},    // CreateAnimation
	0x00463C00: {ret: retHeap512}, // Animation
	0x00463CE8: {ret: retZero},    // CreateRefMap
	0x00463D78: {ret: retHeap256}, // RefMap
	0x00463C18: {ret: retZero},    // ShareAnimation
	0x00463D90: {ret: retZero},    // ShareRefMap
	0x00446820: {ret: retZero},    // AddPlay
	0x00446568: {ret: retZero},    // AttachSkin
	0x004465C0: {ret: retZero},    // Attach

	// VICSprite post-parse initialization — no ESF reads, operate on sprite struct
	0x004251C0: {ret: retZero},    // SetDefaultEntries
	0x00425688: {ret: retZero},    // InitTextSlots
	0x004259F8: {ret: retZero},    // SetAnimPriorities
	0x00425BE0: {ret: retZero},    // SetAnimSoundChannels
	0x00425710: {ret: retZero},    // Share (VICSprite)
	0x00425048: {ret: retZero},    // SetDefaults

	// VISoundDevice — audio hardware stubs
	0x0049AD30: {ret: retZero},    // CreateSound
	0x0049AE30: {ret: retHeap256}, // Sound (get pointer)
	0x0049AE80: {ret: retZero},    // ReleaseSound
	0x0049D950: {ret: retHeap256}, // AsWave (get wave object)
	0x0049D980: {ret: retHeap256}, // AsXm (get XM object)
	0x0049D6D0: {ret: retZero},    // VIXm::Create
	0x0049D470: {ret: retZero},    // VIWave::SetSampleRate
	0x0049D2F8: {ret: retZero},    // VIWave::Create
	0x0049D3D8: {ret: retZero},    // VIWave::Format
	0x0049D3F8: {ret: retZero},    // VIWave::LockBuffer
	0x0049D418: {ret: retZero},    // VIWave::UnlockBuffer
	0x0043E190: {ret: retZero},    // AdpcmToPcm (audio decode)
	// 0x00446820 AddPlay — already listed above

	0x0040E498: {ret: retZero}, // VISurface::SetDMAHeaders (GS DMA transfer setup)
	// VIRaster Share functions — reference counting on pool entries that may not exist
	0x00404388: {ret: retZero}, // ShareMaterialPal
	0x004032B8: {ret: retZero}, // SharePrimBuffer
	0x00489568: {ret: retZero}, // ShareCollBuffer

	// DestroyPool — VIPool variants that can infinite-loop during Allocate realloc.
	// Only stub the VIRaster pool DestroyPools. Other DestroyPools (VIMap, VIScene)
	// run natively.
	// DestroyPool — all VIPool variants. These iterate pool entries and can
	// infinite-loop during Allocate realloc when the entry linked list has
	// a self-referencing node. Safe to stub since we use a bump allocator.
	// VIRaster pools:
	0x004097C8: {ret: retZero}, // DestroyPool<VIPool<Surface*>>
	0x00409428: {ret: retZero}, // DestroyPool<VIPool<PrimBuffer*>>
	0x00409B18: {ret: retZero}, // DestroyPool<VIPool<MaterialPal*>>
	0x00409C88: {ret: retZero}, // DestroyPool<VIPool<RasterMaterialLRU>>
	0x00409DF8: {ret: retZero}, // DestroyPool<VIPool<RFont*>>
	0x00409658: {ret: retZero}, // DestroyPool<VIPool<ColorBuffer*>>
	// VIScene pools:
	0x0046D2C8: {ret: retZero}, // DestroyPool<VIPool<Sprite*>>
	0x0046D438: {ret: retZero}, // DestroyPool<VIPool<HSpriteAnim*>>
	0x0046D5A8: {ret: retZero}, // DestroyPool<VIPool<RefMap*>>
	0x0046D718: {ret: retZero}, // DestroyPool<VIPool<StaticLighting*>>
	// VICollide pools:
	0x0048DEB8: {ret: retZero}, // DestroyPool<VIPool<CollBuffer*>>
	// VIHSprite pools:
	0x00427278: {ret: retZero}, // DestroyPool<VIPool<HSpriteAttachment>>
	0x004276A8: {ret: retZero}, // DestroyPool<VIPool<HSpritePlay>>
	0x00427B28: {ret: retZero}, // DestroyPool<VIPool<HSpritePlayNode>>
	// VIVector/VIArray/VIMap DestroyPools:
	0x00426960: {ret: retZero}, // DestroyPool<VIVector<HSpriteNode>>
	0x00426D80: {ret: retZero}, // DestroyPool<VIVector<Matrix44>>
	0x00427018: {ret: retZero}, // DestroyPool<VIArray<HSpriteTrigger>>
	0x0040A790: {ret: retZero}, // DestroyPool<VIArray<int>>
	0x003E45D0: {ret: retZero}, // DestroyPool<VIMap<DictEntry>>
	0x00415F20: {ret: retZero}, // DestroyPool<VIMap<int,int>>
	0x004604C0: {ret: retZero}, // DestroyPool<VIArray<RadialFloraInst>>
	0x0043ED68: {ret: retZero}, // DestroyPool<VIArray<ResourceElem>>
	0x00459300: {ret: retZero}, // DestroyPool<VIPool<ParticleDefinition*>>
	0x0045B1E8: {ret: retZero}, // DestroyPool<VIPool<ParticleMotif*>>
	0x0046D8B8: {ret: retZero}, // DestroyPool<VIVector<SceneDisplayElem>>
	0x004704F0: {ret: retZero}, // DestroyPool<VIVector<int>>
	0x0047CC48: {ret: retZero}, // DestroyPool<VIPool<SpellEffect*>>
	// VISurface::Init — stub. CalcSurfaceOffsets uses a jump table for bitdepth
	// that requires valid data section access. Lock/Unlock/Convert also stubbed.
	0x0040DF10: {ret: retZero}, // VISurface::Init
	0x0040E030: {ret: retZero}, // LockMipLevel
	0x0040E090: {ret: retZero}, // UnlockMipLevel
	0x0040DFF4: {ret: retZero}, // LockPalette
	0x0040E158: {ret: retZero}, // UnlockPalette
	// Convert functions write to pixel buffer — no-op since Lock returns null
	0x0043DA90: {ret: retZero}, // Convert32
	0x0043DDA8: {ret: retZero}, // Convert16
	0x0043DF28: {ret: retZero}, // Convert8
	0x0043DFF8: {ret: retZero}, // Convert4
	0x0043DC08: {ret: retZero}, // Convert24

	// VIRasterTess — PS2 VU1 tessellation engine.
	0x004149D8: {ret: retZero}, // VIRasterTess::Init

	// GS register / DMA functions
	0x003FF1C0: {ret: retZero}, // InitGSRegisters
	0x003FF050: {ret: retZero}, // InitDMADoubleBuffer
	0x003FEEF8: {ret: retZero}, // UploadRasterMicro (VU1 microcode)
	0x003FEE48: {ret: retZero}, // sceGsSyncV
	0x003FEE60: {ret: retZero}, // sceGsSwapDBuffDc
}

type retType int

const (
	retZero    retType = iota // return 0 (success)
	retIndex                  // return incrementing index via FakeRaster/FakeScene
	retHeap256                // return heapAlloc(256) pointer
	retHeap512                // return heapAlloc(512) pointer
)

type stubAction struct {
	ret retType
}

// handleJAL intercepts JAL calls. Returns true if the call was handled
// (caller should skip to return address), false to execute natively.
func (m *Interp) handleJAL(target uint32) bool {
	// Call trace (PCSX2 DebugInterface style)
	if m.callTraceEnabled {
		name := ""
		if n, ok := readFuncs[target]; ok {
			name = n
		}
		m.callTrace = append(m.callTrace, CallEntry{
			Target: target, Caller: m.pc, Name: name,
		})
	}

	// 1. Runtime object functions (VIDictionary, VIScene — need Go-side tracking)
	if m.handleRuntime(target) {
		return true
	}

	// 2a. DRDP buffer reads — route through DRDPStream
	if m.drdpStream != nil {
		if m.handleDRDPRead(target, m.drdpStream) {
			return true
		}
	}

	// 2b. CLIENT stream reads — route through PacketStream
	if m.packetStream != nil {
		if m.handleClientRead(target, m.packetStream) {
			return true
		}
	}

	// 2b. ESF Read* functions — route through our Go ESF stream
	if name, ok := readFuncs[target]; ok {
		m.handleRead(name)
		m.Intercepted++
		return true
	}

	// 3. Memory allocation — route through our heap allocator
	if target == 0x004C4638 || target == 0x004C47D8 { // __builtin_new / __builtin_vec_new
		size := uint32(m.rReg(4)) // $a0 = size
		if size == 0 {
			size = 16
		}
		ptr := m.heapAlloc(size)
		m.wReg32(2, int64(ptr))
		m.Intercepted++
		return true
	}
	if target == 0x004C45C0 || target == 0x004C4760 { // __builtin_delete / __builtin_vec_delete
		m.wReg32(2, 0)
		m.Intercepted++
		return true
	}

	// 4. Skip-stubs: subsystems with deep VIRaster/VIScene dependencies.
	for _, skipAddr := range []uint32{
		0x00435240, // ParseMaterialPal — VIRaster material/surface chain
		0x00434D08, // ParseSurfaceArray — VIRaster Surface creation
		0x00435F00, // ParseHSpriteSpriteArray — needs full VIScene sprite management
		0x004379D8, // ParseCSpriteSpriteArray — same
		// ParseSoundArray runs natively — captures audio resource references
	} {
		if target == skipAddr {
			if m.Reader != nil {
				m.Reader.ReadBegin()
				m.Reader.ReadEnd()
			}
			m.wReg32(2, 0)
			m.Intercepted++
			return true
		}
	}

	// 5. powf — math library, faster to intercept
	if target == 0x00127328 {
		base := m.fregs[12]
		exp := m.fregs[13]
		result := float32(math.Pow(float64(base), float64(exp)))
		m.fregs[0] = result
		m.Intercepted++
		return true
	}

	// 4. Hardware stubs — functions that touch PS2 GPU/GS
	if stub, ok := hardwareStubs[target]; ok {
		switch stub.ret {
		case retZero:
			m.wReg32(2, 0)
		case retIndex:
			m.wReg32(2, 0) // index 0, caller uses it to fetch pointer
		case retHeap256:
			m.wReg32(2, int64(m.heapAlloc(256)))
		case retHeap512:
			m.wReg32(2, int64(m.heapAlloc(512)))
		}
		m.Intercepted++
		return true
	}

	// 5. DEFAULT: execute natively. The EE dump has all SUPPORT code.
	// No auto-stub — let the interpreter run the actual PS2 instructions.
	return false
}

func (m *Interp) handleRead(name string) {
	r := m.Reader
	if r == nil {
		m.wReg32(2, -1)
		return
	}

	switch name {
	case "ReadBegin":
		typ, ver, _ := r.ReadBegin()
		a1 := uint32(m.rReg(5))
		a2 := uint32(m.rReg(6))
		m.store16(a1, typ)
		m.store16(a2, ver)
		if typ > 0 {
			m.wReg32(2, 0)
		} else {
			m.wReg32(2, -1)
		}

	case "ReadBegin2":
		typ, ver, size := r.ReadBegin()
		a1 := uint32(m.rReg(5))
		a2 := uint32(m.rReg(6))
		a3 := uint32(m.rReg(7))
		t0 := uint32(m.rReg(8))
		m.store16(a1, typ)
		m.store16(a2, ver)
		if a3 != 0 {
			m.store32(a3, size)
		}
		if t0 != 0 {
			m.store32(t0, 0)
		}
		if typ > 0 {
			m.wReg32(2, 0)
		} else {
			m.wReg32(2, -1)
		}

	case "ReadEnd":
		r.ReadEnd()
		m.wReg32(2, 0)

	case "ObjectVersion":
		m.wReg32(2, int64(r.ObjectVersion()))

	case "NumSubObjects":
		m.wReg32(2, int64(r.NumSubObjects()))

	case "ObjectSize":
		m.wReg32(2, int64(r.ObjectSize()))

	case "Read_Ri":
		a1 := uint32(m.rReg(5))
		v := r.ReadInt32()
		m.store32(a1, uint32(v))
		m.wReg32(2, 0)

	case "Read_RUi":
		a1 := uint32(m.rReg(5))
		v := r.ReadUint32()
		m.store32(a1, v)
		m.wReg32(2, 0)

	case "Read_Rf":
		a1 := uint32(m.rReg(5))
		v := r.ReadFloat32()
		m.storeFloat(a1, v)
		m.wReg32(2, 0)

	case "Read_Rs":
		a1 := uint32(m.rReg(5))
		v := r.ReadInt16()
		m.store16(a1, uint16(v))
		m.wReg32(2, 0)

	case "Read_RUc":
		a1 := uint32(m.rReg(5))
		v := r.ReadUint8()
		m.store8(a1, v)
		m.wReg32(2, 0)

	case "Read_RSc":
		a1 := uint32(m.rReg(5))
		v := r.ReadInt8()
		m.store8(a1, byte(v))
		m.wReg32(2, 0)

	case "Read_Rc":
		a1 := uint32(m.rReg(5))
		v := r.ReadUint8()
		m.store8(a1, v)
		m.wReg32(2, 0)

	case "Read_RUs":
		a1 := uint32(m.rReg(5))
		v := r.ReadInt16()
		m.store16(a1, uint16(v))
		m.wReg32(2, 0)

	case "Read_Rl":
		a1 := uint32(m.rReg(5))
		lo := r.ReadInt32()
		hi := r.ReadInt32()
		m.store32(a1, uint32(lo))
		m.store32(a1+4, uint32(hi))
		m.wReg32(2, 0)

	case "Read_PUci":
		a1 := uint32(m.rReg(5))
		count := int(m.rReg(6) & 0xFFFFFFFF)
		data := r.ReadBytes(count)
		for i, b := range data {
			m.store8(a1+uint32(i), b)
		}
		m.wReg32(2, 0)

	default:
		m.wReg32(2, 0)
	}
}

// zeroFakePointers zeros memory at all fake pointer addresses so that
// reads from fake objects return 0 instead of stale EE dump data.
func ZeroFakePointersExported(interp *Interp) { zeroFakePointers(interp) }

func zeroFakePointers(interp *Interp) {
	for _, base := range []uint32{0x01F40000, 0x01F50000, 0x01F60000, 0x01F70000,
		0x01F80000, 0x01F90000, 0x01FA0000, 0x01FB0000, 0x01FC0000, 0x01FD0000} {
		for i := uint32(0); i < 256; i++ {
			interp.Store8At(base+i, 0)
		}
	}
}

// setupVIESFParse initializes the VIESFParse context with manually
// initialized pools. Uses RunCall for pool Init only (no default
// resource creation that would pollute the dictionary).
// The RunCall stack issue is mitigated by clearing the main stack
// region after all RunCalls complete.
func setupVIESFParse(interp *Interp) uint32 {
	zeroFakePointers(interp)
	thisAddr := uint32(0x01FE0000)
	for i := uint32(0); i < 512; i++ {
		interp.Store8At(thisAddr+i, 0)
	}

	// Allocate runtime objects
	raster := interp.heapAlloc(0x5000)
	collide := interp.heapAlloc(4096)
	scene := interp.heapAlloc(0x5000)
	dict := interp.heapAlloc(4096)

	// Initialize VIRaster pools manually (matching VIPool::Clear)
	if raster != 0 {
		interp.store32(raster+0x3C, 1) // initialized flag
		for _, off := range []uint32{0x4B78, 0x4B9C, 0x4BB4, 0x4BE4, 0x4BFC, 0x4C34} {
			interp.store32(raster+off+0x0C, 0xFFFFFFFF) // freeHead = -1
			interp.store32(raster+off+0x14, 0xFFFFFFFF) // usedTail = -1
		}
		for off := uint32(0x45A0); off <= 0x45BC; off += 4 {
			interp.store32(raster+off, 0xFFFFFFFF)
		}
	}

	// Initialize VIScene pools via RunCall (just pool Init, no default resources)
	if scene != 0 {
		interp.store32(scene+0x4B9C, raster)
		interp.store32(scene+0x4BA4, dict)
		interp.store32(scene+0x4BA8, collide)
		interp.RunCall(0x0046DE30, scene+0x3E4, 0) // VIPool<Sprite*>::Init
		interp.RunCall(0x0046E098, scene+0x3FC, 0) // VIPool<HSpriteAnim*>::Init
		interp.RunCall(0x0046E300, scene+0x414, 0) // VIPool<RefMap*>::Init
		interp.RunCall(0x0046DBC8, scene+0x03CC)   // VIList<Actor>::Init
		interp.RunCall(0x0046DBF0, scene+0x03D8)   // VIList<SceneOccup>::Init
	}

	// Clear the main stack region to prevent RunCall stale data
	// from interfering with the main Run's stack frames
	for addr := uint32(0x01FEF000); addr < 0x01FF0000; addr += 4 {
		interp.store32(addr, 0)
	}

	// Store pointers in VIESFParse context
	interp.Store32At(thisAddr+0x0C, raster)
	interp.Store32At(thisAddr+0x14, collide)
	interp.Store32At(thisAddr+0x18, scene)
	interp.Store32At(thisAddr+0x20, dict)
	interp.Store32At(thisAddr+0x24, 0x01FD0000) // VIObjFile*

	return thisAddr
}

// RunParserV0 runs a v0 sub-parser on object data.
func RunParserV0(eeDump []byte, parserAddr uint32, objData []byte) (int32, []ReadEntry) {
	interp := New(eeDump)
	esf := NewESFStream(objData)
	esf.ReadBegin()
	thisAddr := setupVIESFParse(interp)
	interp.Reader = esf
	result := interp.Run(parserAddr, nil, thisAddr)
	return result, esf.Reads
}

// RunParser sets up a VIESFParse context and runs a parser function.
func RunParser(eeDump []byte, parserAddr uint32, objData []byte) (int32, []ReadEntry) {
	interp := New(eeDump)
	esf := NewESFStream(objData)
	thisAddr := setupVIESFParse(interp)
	result := interp.Run(parserAddr, esf, thisAddr)
	return result, esf.Reads
}
