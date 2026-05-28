// VIStaticLighting — baked lighting data for zone meshes.
// 26,048 in TUNARIA. Each one applies to a collision mesh or zone chunk.
//
// Structure: header(0x3280, empty) + light data(0x6040).
// PS2 ParseStaticLightingObj at 0x0043D2D0.
package vi

// VIStaticLighting holds pre-baked lighting for a zone mesh.
type VIStaticLighting struct {
	NumLights int32
	Flags     int32
	// Light color array from child 0x6040
	// Each entry: 4×uint8 (RGBA) per vertex or 4×float32 per light
	LightData []byte
}

const TypeStaticLighting = 0x3270

// ParseStaticLighting reads a StaticLighting from ESF child data.
func ParseStaticLighting(data []byte, children map[uint16][]byte) *VIStaticLighting {
	sl := &VIStaticLighting{}

	// Header reads (from ParseStaticLightingObj decompilation):
	// ReadInt32(numLights), ReadInt32(flags)
	if len(data) >= 8 {
		pos := 0
		sl.NumLights = ri32(data, &pos)
		sl.Flags = ri32(data, &pos)
	}

	// Light data child 0x6040
	if ld, ok := children[0x6040]; ok {
		sl.LightData = make([]byte, len(ld))
		copy(sl.LightData, ld)
	}

	return sl
}
