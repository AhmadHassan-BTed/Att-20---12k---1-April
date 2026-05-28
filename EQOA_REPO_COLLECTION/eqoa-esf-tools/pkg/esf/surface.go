package esf

import (
	"encoding/binary"
	"image"
	"image/color"
	"image/png"
	"os"
	"path/filepath"
)

// Surface represents a texture object in an ESF file.
type Surface struct {
	info     *ObjInfo
	DictID   int32
	W, H     int32
	Depth    int32
	Mip      int32
	Image    *image.NRGBA
	Alpha    *image.Gray
	HasAlpha bool
}

func (s *Surface) ObjInfo() *ObjInfo { return s.info }

// Load reads the surface data from the ESF file, decoding palettised
// pixel data into an NRGBA image and an optional grayscale alpha map.
func (s *Surface) Load(file *ObjFile) error {
	file.Seek(s.info.Offset)

	s.DictID = file.readInt32()
	s.W = file.readInt32()
	s.H = file.readInt32()
	s.Depth = file.readInt32()
	s.Mip = file.readInt32()

	// Read palette for palettised (depth < 2) textures.
	var palette []byte
	if s.Depth < 2 {
		datasize := file.readInt32()
		datasize <<= 2 // number of palette entries -> byte count (4 bytes each)
		palette = file.readBytes(int(datasize))
	}

	s.Image = image.NewNRGBA(image.Rect(0, 0, int(s.W), int(s.H)))
	s.Alpha = image.NewGray(image.Rect(0, 0, int(s.W), int(s.H)))
	s.HasAlpha = false

	if s.Mip > 0 {
		mipsize := file.readInt32() // row byte count for this mipmap level

		// Detect 4bpp nibble-packed textures: palette ≤16 entries and
		// row byte count is half the pixel width.
		palEntries := len(palette) / 4
		is4bpp := palEntries > 0 && palEntries <= 16 && mipsize < s.W

		for j := int32(0); j < s.H; j++ {
			rowData := file.readBytes(int(mipsize))

			if is4bpp {
				// 4bpp: each byte packs two pixel indices (high nibble first).
				for x := int32(0); x < s.W; x++ {
					byteIdx := x / 2
					var col int
					if x%2 == 0 {
						col = int(rowData[byteIdx]>>4) & 0x0F
					} else {
						col = int(rowData[byteIdx]) & 0x0F
					}
					s.setPixelFromPalette(int(x), int(j), col, palette)
				}
			} else {
				// 8bpp: one byte per pixel index.
				for x := int32(0); x < mipsize; x++ {
					col := int(rowData[x]) & 0xFF
					s.setPixelFromPalette(int(x), int(j), col, palette)
				}
			}
		}

		if !s.HasAlpha {
			s.Alpha = nil
		}

		// Halve dimensions for next mipmap level (not read here).
		s.W >>= 1
		if s.W == 0 {
			s.W = 1
		}
		s.H >>= 1
		if s.H == 0 {
			s.H = 1
		}
	}

	return nil
}

// setPixelFromPalette reads an RGBA color from the palette and sets the pixel.
func (s *Surface) setPixelFromPalette(x, y, col int, palette []byte) {
	off := col << 2
	if off+4 > len(palette) {
		return // out-of-range palette index — skip
	}
	rgba := binary.LittleEndian.Uint32(palette[off : off+4])
	b := uint8((rgba >> 16) & 0xFF)
	g := uint8((rgba >> 8) & 0xFF)
	r := uint8(rgba & 0xFF)
	a := uint8((rgba >> 24) & 0xFF)
	if a != 255 {
		s.HasAlpha = true
	}
	s.Image.SetNRGBA(x, y, color.NRGBA{R: r, G: g, B: b, A: a})
	s.Alpha.SetGray(x, y, color.Gray{Y: a})
}

// SaveTexture writes the decoded image as a PNG file. If the surface has
// an alpha channel, a separate grayscale alpha PNG is also written.
// dir is the output directory, fn is the image filename, and alphaFn is
// the alpha map filename.
func (s *Surface) SaveTexture(dir, fn, alphaFn string) error {
	if err := os.MkdirAll(dir, 0o755); err != nil {
		return err
	}

	// Write main image.
	imgPath := filepath.Join(dir, fn)
	imgFile, err := os.Create(imgPath)
	if err != nil {
		return err
	}
	if err := png.Encode(imgFile, s.Image); err != nil {
		imgFile.Close()
		return err
	}
	if err := imgFile.Close(); err != nil {
		return err
	}

	// Write alpha map if present.
	if s.Alpha != nil {
		alpPath := filepath.Join(dir, alphaFn)
		alpFile, err := os.Create(alpPath)
		if err != nil {
			return err
		}
		if err := png.Encode(alpFile, s.Alpha); err != nil {
			alpFile.Close()
			return err
		}
		if err := alpFile.Close(); err != nil {
			return err
		}
	}

	return nil
}
