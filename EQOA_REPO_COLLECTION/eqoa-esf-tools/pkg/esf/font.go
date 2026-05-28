package esf

import (
	"encoding/binary"
	"image"
	"image/color"
)

// Font represents a PS2 bitmap font from a CSF/ESF file.
// PS2: ParseFontObj (0x0043A830), type 0x7000.
// Children: 0x7010 (header), 0x7020 (display widths), 0x7030 (CLUT + per-char pixel data).
//
// 0x7030 layout:
//   [8-byte header] [CLUT: 255 RGBA entries = 1020 bytes]
//   [per-char × numChars: charCode(u16) + rowBytes(i32) + rowBytes*charHeight pixels (8-bit alpha)]
type Font struct {
	info       *ObjInfo
	DictID     uint32
	NumChars   int
	CharHeight int
	BitDepth   int
	Widths     []byte // display widths per char (from 0x7020)
	Glyphs     []FontGlyph
}

// FontGlyph holds one character's bitmap data.
type FontGlyph struct {
	CharCode uint16
	RowBytes int // pixel width = bytes per row (8-bit alpha)
	Pixels   []byte // rowBytes * charHeight bytes, row-major 8-bit alpha
}

func (f *Font) ObjInfo() *ObjInfo { return f.info }

func (f *Font) Load(file *ObjFile) error {
	// Header (0x7010)
	hdr := f.info.Child(0x7010)
	if hdr != nil {
		file.Seek(hdr.Offset)
		f.DictID = file.readUint32()
		f.NumChars = int(file.readInt32())
		f.CharHeight = int(file.readInt32())
		f.BitDepth = int(file.readInt32())
	}

	// Display widths (0x7020) — 1 byte per character
	ct := f.info.Child(0x7020)
	if ct != nil {
		f.Widths = file.RawBytes(int(ct.Offset), int(ct.Size))
	}

	// Pixel data (0x7030): CLUT + per-char glyph data
	px := f.info.Child(0x7030)
	if px == nil {
		return nil
	}
	pxData := file.RawBytes(int(px.Offset), int(px.Size))
	if len(pxData) < 1028 {
		return nil
	}

	// Skip 8-byte header + 1020-byte CLUT (255 RGBA entries)
	offset := 1028

	f.Glyphs = make([]FontGlyph, 0, f.NumChars)
	for i := 0; i < f.NumChars && offset+6 <= len(pxData); i++ {
		charCode := binary.LittleEndian.Uint16(pxData[offset:])
		rowBytes := int(binary.LittleEndian.Uint32(pxData[offset+2:]))
		offset += 6

		if rowBytes <= 0 || rowBytes > 256 || offset+rowBytes*f.CharHeight > len(pxData) {
			break
		}

		size := rowBytes * f.CharHeight
		g := FontGlyph{
			CharCode: charCode,
			RowBytes: rowBytes,
			Pixels:   make([]byte, size),
		}
		copy(g.Pixels, pxData[offset:offset+size])
		offset += size

		f.Glyphs = append(f.Glyphs, g)
	}

	return nil
}

// GlyphByCode finds a glyph by character code. Returns nil if not found.
func (f *Font) GlyphByCode(code uint16) *FontGlyph {
	for i := range f.Glyphs {
		if f.Glyphs[i].CharCode == code {
			return &f.Glyphs[i]
		}
	}
	return nil
}

// GlyphImage extracts a single glyph as an alpha image.
func (f *Font) GlyphImage(charCode uint16) *image.Alpha {
	g := f.GlyphByCode(charCode)
	if g == nil || g.RowBytes == 0 {
		return nil
	}
	img := image.NewAlpha(image.Rect(0, 0, g.RowBytes, f.CharHeight))
	for y := 0; y < f.CharHeight; y++ {
		for x := 0; x < g.RowBytes; x++ {
			img.SetAlpha(x, y, color.Alpha{A: g.Pixels[y*g.RowBytes+x]})
		}
	}
	return img
}

// GlyphRect describes a glyph's position in a texture atlas.
type GlyphRect struct {
	X, Y, W, H int
}

// Atlas generates a texture atlas of all printable ASCII glyphs (32-127).
func (f *Font) Atlas() (*image.NRGBA, map[uint16]GlyphRect) {
	cols := 16
	maxW := 0
	for _, g := range f.Glyphs {
		if g.CharCode >= 32 && g.CharCode < 128 && g.RowBytes > maxW {
			maxW = g.RowBytes
		}
	}
	if maxW == 0 {
		maxW = 16
	}
	cellW := maxW + 1
	numPrintable := 0
	for _, g := range f.Glyphs {
		if g.CharCode >= 32 && g.CharCode < 128 {
			numPrintable++
		}
	}
	rows := (numPrintable + cols - 1) / cols
	atlasW := cols * cellW
	atlasH := rows * f.CharHeight

	img := image.NewNRGBA(image.Rect(0, 0, atlasW, atlasH))
	// Background stays (0,0,0,0) — Raylib handles alpha blending correctly.
	rects := make(map[uint16]GlyphRect)

	idx := 0
	for _, g := range f.Glyphs {
		if g.CharCode < 32 || g.CharCode >= 128 {
			continue
		}
		col := idx % cols
		row := idx / cols
		x0 := col * cellW
		y0 := row * f.CharHeight

		for y := 0; y < f.CharHeight; y++ {
			for x := 0; x < g.RowBytes; x++ {
				v := g.Pixels[y*g.RowBytes+x]
				if v > 0 {
					// 0=transparent, 1=semi-opaque, 2=fully opaque
					a := uint8(255)
					if v == 1 {
						a = 128
					}
					img.Set(x0+x, y0+y, color.NRGBA{255, 255, 255, a})
				}
			}
		}
		rects[g.CharCode] = GlyphRect{X: x0, Y: y0, W: g.RowBytes, H: f.CharHeight}
		idx++
	}

	return img, rects
}
