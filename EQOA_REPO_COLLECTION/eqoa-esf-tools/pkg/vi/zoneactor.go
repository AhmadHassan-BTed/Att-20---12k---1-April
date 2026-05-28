// VIZoneActor — entity placement in the world.
// 229,777 instances in TUNARIA. Every NPC, object, and decoration.
//
// PS2 read sequence (from ParseZoneActor at 0x00439190):
//   uint32 dictID (sprite reference)
//   3×float32 position (x, y, z)
//   3×float32 rotation (x, y, z) in radians
//   float32 scale
//   4×uint8 color (r, g, b, a)
package vi

// VIZoneActor defines a placed entity in the world.
type VIZoneActor struct {
	DictID int32 // sprite reference (VIHashResourceID)
	PosX, PosY, PosZ float32
	RotX, RotY, RotZ float32
	Scale float32
	R, G, B, A uint8
}

const TypeZoneActor = 0x6000

// ParseZoneActor reads a ZoneActor from raw ESF body data.
// Body = Size - 4 bytes (past numSubObjects).
// ver=2: dictID(4) + pos(12) + rot(12) + scale(4) = 32 bytes
// ver=3+: + color(4) = 36 bytes
func ParseZoneActor(data []byte) *VIZoneActor {
	if len(data) < 32 {
		return nil
	}
	pos := 0
	a := &VIZoneActor{}
	a.DictID = ri32(data, &pos)
	a.PosX = rf32(data, &pos)
	a.PosY = rf32(data, &pos)
	a.PosZ = rf32(data, &pos)
	a.RotX = rf32(data, &pos)
	a.RotY = rf32(data, &pos)
	a.RotZ = rf32(data, &pos)
	a.Scale = rf32(data, &pos)
	// Color bytes present if body > 32
	if pos+4 <= len(data) {
		a.R = data[pos]; pos++
		a.G = data[pos]; pos++
		a.B = data[pos]; pos++
		a.A = data[pos]; pos++
	} else {
		a.R = 255; a.G = 255; a.B = 255; a.A = 255
	}
	return a
}
