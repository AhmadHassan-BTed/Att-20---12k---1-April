// VIFloraSprite — vegetation model with PrimBuffer for terrain scattering.
// 1,642 in TUNARIA. Trees, bushes, grass.
//
// Structure: header(0x2F01) + MaterialPal(0x1110) + FloraPrimBuffer(0x1230).
// PS2 ParseFloraSpriteObj at 0x0043C230.
// Transpiler trace: 86 reads per object.
package vi

// VIFloraSprite is a vegetation model used by the radial flora system.
type VIFloraSprite struct {
	DictID uint32
	BBox   VIBox

	// Flora PrimBuffer data (from child 0x1230)
	// Parsed as VIPrimBuffer with flora-specific vertex format
	HasFloraPrimBuffer bool
}

const TypeFloraSprite = 0x2F00

// ParseFloraSprite reads a FloraSprite header from ESF tree data.
func ParseFloraSprite(data []byte, children map[uint16][]byte) (*VIFloraSprite, error) {
	fs := &VIFloraSprite{}
	if hdr, ok := children[0x2F01]; ok {
		pos := 0
		fs.DictID = ru32(hdr, &pos)
		fs.BBox.Min.X = rf32(hdr, &pos)
		fs.BBox.Min.Y = rf32(hdr, &pos)
		fs.BBox.Min.Z = rf32(hdr, &pos)
		fs.BBox.Max.X = rf32(hdr, &pos)
		fs.BBox.Max.Y = rf32(hdr, &pos)
		fs.BBox.Max.Z = rf32(hdr, &pos)
	}
	if _, ok := children[0x1230]; ok {
		fs.HasFloraPrimBuffer = true
	}
	return fs, nil
}
