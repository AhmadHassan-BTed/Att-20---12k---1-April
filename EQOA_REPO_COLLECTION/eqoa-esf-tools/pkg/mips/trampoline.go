package mips


// buildTrampoline writes MIPS instructions to heap memory that:
// 1. Call VIRaster::Init(raster)
// 2. Call VIScene::Init(scene, 0, dict, raster, 0, collide)
// 3. Call VIDictionary::Init(dict, 0)
// 4. Call the actual parser function(parserAddr, thisAddr)
// 5. Return via jr $ra
//
// All Init calls run on the SAME stack as the parser, avoiding the
// stale-stack issue that RunCall causes.
//
// The trampoline uses $s0-$s6 to hold persistent values across calls
// (callee-saved registers preserved by each function).
func buildTrampoline(m *Interp, parserAddr, thisAddr, raster, scene, dict, collide uint32) uint32 {
	// Allocate space for trampoline code (256 bytes = 64 instructions max)
	tramp := m.heapAlloc(256)
	if tramp == 0 {
		return 0
	}

	var code []uint32

	// Prologue: save $ra, $s0-$s6 on stack
	// addiu $sp, $sp, -112
	code = append(code, 0x27BDFF90) // addiu $sp, $sp, -112
	// sq $ra, 96($sp)
	code = append(code, 0x7FBF0060)
	// sq $s0, 0($sp)
	code = append(code, 0x7FB00000)
	// sq $s1, 16($sp)
	code = append(code, 0x7FB10010)
	// sq $s2, 32($sp)
	code = append(code, 0x7FB20020)
	// sq $s3, 48($sp)
	code = append(code, 0x7FB30030)
	// sq $s4, 64($sp)
	code = append(code, 0x7FB40040)
	// sq $s5, 80($sp)
	code = append(code, 0x7FB50050)

	// Load persistent values into callee-saved registers
	// li $s0, thisAddr (lui + ori)
	code = append(code, 0x3C100000|uint32(thisAddr>>16))       // lui $s0, hi
	code = append(code, 0x36100000|uint32(thisAddr&0xFFFF))    // ori $s0, $s0, lo
	// li $s1, raster
	code = append(code, 0x3C110000|uint32(raster>>16))
	code = append(code, 0x36310000|uint32(raster&0xFFFF))
	// li $s2, scene
	code = append(code, 0x3C120000|uint32(scene>>16))
	code = append(code, 0x36520000|uint32(scene&0xFFFF))
	// li $s3, dict
	code = append(code, 0x3C130000|uint32(dict>>16))
	code = append(code, 0x36730000|uint32(dict&0xFFFF))
	// li $s4, collide
	code = append(code, 0x3C140000|uint32(collide>>16))
	code = append(code, 0x36940000|uint32(collide&0xFFFF))
	// li $s5, parserAddr
	code = append(code, 0x3C150000|uint32(parserAddr>>16))
	code = append(code, 0x36B50000|uint32(parserAddr&0xFFFF))

	// 1. Call VIDictionary::Init(dict, capacity=0)
	// daddu $a0, $s3, $zero  (dict)
	code = append(code, 0x0260202D)
	// jal 0x003E4270  (Dict::Init)
	code = append(code, 0x0C000000|(0x003E4270>>2))
	// move $a1, $zero  (capacity=0) — delay slot
	code = append(code, 0x0000282D)

	// 2. Call VIRaster::Init(raster)
	// daddu $a0, $s1, $zero  (raster)
	code = append(code, 0x0220202D)
	// jal 0x003FFA90  (VIRaster::Init)
	code = append(code, 0x0C000000|(0x003FFA90>>2))
	// nop (delay slot)
	code = append(code, 0x00000000)

	// 3. Call VIScene::Init(scene, 0LL, dict, raster, 0, collide)
	// daddu $a0, $s2, $zero  (scene)
	code = append(code, 0x0240202D)
	// move $a1, $zero  (flags=0)
	code = append(code, 0x0000282D)
	// daddu $a2, $s3, $zero  (dict)
	code = append(code, 0x0260302D)
	// daddu $a3, $s1, $zero  (raster)
	code = append(code, 0x0220382D)
	// move $t0, $zero  (soundDevice=0)
	code = append(code, 0x0000402D)
	// daddu $t1, $s4, $zero  (collide)
	code = append(code, 0x0280482D)
	// jal 0x004617A8  (VIScene::Init)
	code = append(code, 0x0C000000|(0x004617A8>>2))
	// nop (delay slot)
	code = append(code, 0x00000000)

	// 4. Call parser(thisAddr)
	// daddu $a0, $s0, $zero  (thisAddr = VIESFParse context)
	code = append(code, 0x0200202D)
	// jalr $s5  (parser function)
	code = append(code, 0x02A0F809)
	// nop (delay slot)
	code = append(code, 0x00000000)

	// Save parser result
	// daddu $s5, $v0, $zero  (save $v0 in $s5)
	code = append(code, 0x0040A82D)

	// Epilogue: restore and return
	// daddu $v0, $s5, $zero  (restore result)
	code = append(code, 0x02A0102D)
	// lq $s5, 80($sp)
	code = append(code, 0x7BB50050)
	// lq $s4, 64($sp)
	code = append(code, 0x7BB40040)
	// lq $s3, 48($sp)
	code = append(code, 0x7BB30030)
	// lq $s2, 32($sp)
	code = append(code, 0x7BB20020)
	// lq $s1, 16($sp)
	code = append(code, 0x7BB10010)
	// lq $s0, 0($sp)
	code = append(code, 0x7BB00000)
	// lq $ra, 96($sp)
	code = append(code, 0x7BBF0060)
	// jr $ra
	code = append(code, 0x03E00008)
	// addiu $sp, $sp, 112  (delay slot)
	code = append(code, 0x27BD0000|uint32(112))

	// Write instructions to heap memory via store32
	for i, insn := range code {
		m.store32(tramp+uint32(i*4), insn)
	}

	return tramp
}
