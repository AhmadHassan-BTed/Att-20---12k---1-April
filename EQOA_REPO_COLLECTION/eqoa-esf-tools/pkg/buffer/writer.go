package buffer

import (
	"encoding/binary"
	"errors"
	"math"
	"unicode/utf16"
)

var (
	ErrWriterOverflow = errors.New("buffer overflow: write past end of buffer")
)

// Writer provides binary write operations on a byte buffer with little-endian encoding.
type Writer struct {
	buf []byte
	pos int
}

// NewWriter creates a new Writer over the provided byte slice.
func NewWriter(buf []byte) *Writer {
	return &Writer{buf: buf, pos: 0}
}

// NewWriterSize creates a new Writer with a buffer of the specified size.
func NewWriterSize(size int) *Writer {
	return &Writer{buf: make([]byte, size), pos: 0}
}

// Buffer returns the underlying byte slice.
func (w *Writer) Buffer() []byte {
	return w.buf
}

// Bytes returns the written portion of the buffer.
func (w *Writer) Bytes() []byte {
	return w.buf[:w.pos]
}

// Position returns the current write position.
func (w *Writer) Position() int {
	return w.pos
}

// SetPosition sets the write position.
func (w *Writer) SetPosition(pos int) error {
	if pos < 0 || pos > len(w.buf) {
		return ErrInvalidPosition
	}
	w.pos = pos
	return nil
}

// Length returns the total length of the buffer.
func (w *Writer) Length() int {
	return len(w.buf)
}

// Remaining returns the number of bytes remaining to be written.
func (w *Writer) Remaining() int {
	return len(w.buf) - w.pos
}

// Advance moves the position forward by count bytes.
func (w *Writer) Advance(count int) error {
	if count < 0 || count > w.Remaining() {
		return ErrWriterOverflow
	}
	w.pos += count
	return nil
}

// WriteByte writes a single byte.
func (w *Writer) WriteByte(b byte) error {
	if w.Remaining() < 1 {
		return ErrWriterOverflow
	}
	w.buf[w.pos] = b
	w.pos++
	return nil
}

// WriteBytes writes a byte slice.
func (w *Writer) WriteBytes(data []byte) error {
	if len(data) > w.Remaining() {
		return ErrWriterOverflow
	}
	copy(w.buf[w.pos:], data)
	w.pos += len(data)
	return nil
}

// WriteInt8 writes a signed 8-bit integer.
func (w *Writer) WriteInt8(val int8) error {
	return w.WriteByte(byte(val))
}

// WriteUint16 writes a little-endian unsigned 16-bit integer.
func (w *Writer) WriteUint16(val uint16) error {
	if w.Remaining() < 2 {
		return ErrWriterOverflow
	}
	binary.LittleEndian.PutUint16(w.buf[w.pos:], val)
	w.pos += 2
	return nil
}

// WriteInt16 writes a little-endian signed 16-bit integer.
func (w *Writer) WriteInt16(val int16) error {
	return w.WriteUint16(uint16(val))
}

// WriteUint32 writes a little-endian unsigned 32-bit integer.
func (w *Writer) WriteUint32(val uint32) error {
	if w.Remaining() < 4 {
		return ErrWriterOverflow
	}
	binary.LittleEndian.PutUint32(w.buf[w.pos:], val)
	w.pos += 4
	return nil
}

// WriteUint32BE writes a big-endian unsigned 32-bit integer.
func (w *Writer) WriteUint32BE(val uint32) error {
	if w.Remaining() < 4 {
		return ErrWriterOverflow
	}
	binary.BigEndian.PutUint32(w.buf[w.pos:], val)
	w.pos += 4
	return nil
}

// WriteInt32 writes a little-endian signed 32-bit integer.
func (w *Writer) WriteInt32(val int32) error {
	return w.WriteUint32(uint32(val))
}

// WriteUint64 writes a little-endian unsigned 64-bit integer.
func (w *Writer) WriteUint64(val uint64) error {
	if w.Remaining() < 8 {
		return ErrWriterOverflow
	}
	binary.LittleEndian.PutUint64(w.buf[w.pos:], val)
	w.pos += 8
	return nil
}

// WriteInt64 writes a little-endian signed 64-bit integer.
func (w *Writer) WriteInt64(val int64) error {
	return w.WriteUint64(uint64(val))
}

// WriteFloat32 writes a little-endian 32-bit float.
func (w *Writer) WriteFloat32(val float32) error {
	return w.WriteUint32(math.Float32bits(val))
}

// WriteFloat64 writes a little-endian 64-bit float.
func (w *Writer) WriteFloat64(val float64) error {
	return w.WriteUint64(math.Float64bits(val))
}

// Write7BitEncodedUInt64 writes a 7-bit encoded unsigned 64-bit integer.
// Each byte uses 7 bits for data and the high bit indicates continuation.
func (w *Writer) Write7BitEncodedUInt64(val uint64) error {
	for {
		b := byte(val & 0x7F)
		val >>= 7
		if val != 0 {
			b |= 0x80
		}
		if err := w.WriteByte(b); err != nil {
			return err
		}
		if val == 0 {
			break
		}
	}
	return nil
}

// Write7BitEncodedInt64 writes a 7-bit encoded signed 64-bit integer.
// Uses zigzag encoding where the sign bit is stored in the LSB.
func (w *Writer) Write7BitEncodedInt64(val int64) error {
	// Zigzag encode: negative numbers become odd, positive become even
	var uval uint64
	if val < 0 {
		uval = (uint64(^val) << 1) + 1
	} else {
		uval = uint64(val) << 1
	}
	return w.Write7BitEncodedUInt64(uval)
}

// WriteSize writes a message size value.
// If size >= 255, writes 0xFF followed by the size as uint16.
// Otherwise, writes size as a single byte.
func (w *Writer) WriteSize(size int) error {
	if size >= 255 {
		if err := w.WriteByte(0xFF); err != nil {
			return err
		}
		return w.WriteUint16(uint16(size))
	}
	return w.WriteByte(byte(size))
}

// WriteStringUTF8 writes a UTF-8 encoded string with a length prefix.
func (w *Writer) WriteStringUTF8(s string) error {
	if err := w.WriteInt32(int32(len(s))); err != nil {
		return err
	}
	return w.WriteBytes([]byte(s))
}

// WriteStringUTF8Raw writes a UTF-8 encoded string without a length prefix.
func (w *Writer) WriteStringUTF8Raw(s string) error {
	return w.WriteBytes([]byte(s))
}

// WriteStringUnicode writes a UTF-16LE encoded string with a length prefix.
func (w *Writer) WriteStringUnicode(s string) error {
	runes := []rune(s)
	u16s := utf16.Encode(runes)

	if err := w.WriteInt32(int32(len(runes))); err != nil {
		return err
	}

	for _, u := range u16s {
		if err := w.WriteUint16(u); err != nil {
			return err
		}
	}
	return nil
}

// WriteStringUnicodeRaw writes a UTF-16LE encoded string without a length prefix.
func (w *Writer) WriteStringUnicodeRaw(s string) error {
	u16s := utf16.Encode([]rune(s))
	for _, u := range u16s {
		if err := w.WriteUint16(u); err != nil {
			return err
		}
	}
	return nil
}

// Reset resets the writer position to the beginning.
func (w *Writer) Reset() {
	w.pos = 0
}

// Clear resets the position and zeroes the buffer.
func (w *Writer) Clear() {
	w.pos = 0
	for i := range w.buf {
		w.buf[i] = 0
	}
}
