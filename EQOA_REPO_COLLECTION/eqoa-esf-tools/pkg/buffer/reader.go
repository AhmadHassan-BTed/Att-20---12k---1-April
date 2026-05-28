package buffer

import (
	"encoding/binary"
	"errors"
	"unicode/utf16"
	"unsafe"
)

var (
	ErrBufferOverflow  = errors.New("buffer overflow: read past end of buffer")
	ErrInvalidPosition = errors.New("invalid position")
)

// Reader provides binary read operations on a byte buffer with little-endian encoding.
type Reader struct {
	buf []byte
	pos int
}

// NewReader creates a new Reader over the provided byte slice.
func NewReader(buf []byte) *Reader {
	return &Reader{buf: buf, pos: 0}
}

// Buffer returns the underlying byte slice.
func (r *Reader) Buffer() []byte {
	return r.buf
}

// Position returns the current read position.
func (r *Reader) Position() int {
	return r.pos
}

// SetPosition sets the read position.
func (r *Reader) SetPosition(pos int) error {
	if pos < 0 || pos > len(r.buf) {
		return ErrInvalidPosition
	}
	r.pos = pos
	return nil
}

// Length returns the total length of the buffer.
func (r *Reader) Length() int {
	return len(r.buf)
}

// Remaining returns the number of bytes remaining to be read.
func (r *Reader) Remaining() int {
	return len(r.buf) - r.pos
}

// Advance moves the position forward by count bytes.
func (r *Reader) Advance(count int) error {
	if count < 0 || count > r.Remaining() {
		return ErrBufferOverflow
	}
	r.pos += count
	return nil
}

// Rewind moves the position backward by count bytes.
func (r *Reader) Rewind(count int) error {
	if count < 0 || count > r.pos {
		return ErrInvalidPosition
	}
	r.pos -= count
	return nil
}

// ReadByte reads a single byte.
func (r *Reader) ReadByte() (byte, error) {
	if r.Remaining() < 1 {
		return 0, ErrBufferOverflow
	}
	b := r.buf[r.pos]
	r.pos++
	return b, nil
}

// ReadBytes reads count bytes into a new slice.
func (r *Reader) ReadBytes(count int) ([]byte, error) {
	if count < 0 || count > r.Remaining() {
		return nil, ErrBufferOverflow
	}
	result := make([]byte, count)
	copy(result, r.buf[r.pos:r.pos+count])
	r.pos += count
	return result, nil
}

// ReadInt8 reads a signed 8-bit integer.
func (r *Reader) ReadInt8() (int8, error) {
	b, err := r.ReadByte()
	return int8(b), err
}

// ReadUint16 reads a little-endian unsigned 16-bit integer.
func (r *Reader) ReadUint16() (uint16, error) {
	if r.Remaining() < 2 {
		return 0, ErrBufferOverflow
	}
	val := binary.LittleEndian.Uint16(r.buf[r.pos:])
	r.pos += 2
	return val, nil
}

// ReadInt16 reads a little-endian signed 16-bit integer.
func (r *Reader) ReadInt16() (int16, error) {
	val, err := r.ReadUint16()
	return int16(val), err
}

// ReadUint24 reads a 3-byte big-endian unsigned integer (as in the C# reference).
func (r *Reader) ReadUint24() (uint32, error) {
	if r.Remaining() < 3 {
		return 0, ErrBufferOverflow
	}
	// C# code: (uint)(_buffer[_position++] << 16 | _buffer[_position++] << 8 | _buffer[_position++])
	val := uint32(r.buf[r.pos])<<16 | uint32(r.buf[r.pos+1])<<8 | uint32(r.buf[r.pos+2])
	r.pos += 3
	return val, nil
}

// ReadUint32 reads a little-endian unsigned 32-bit integer.
func (r *Reader) ReadUint32() (uint32, error) {
	if r.Remaining() < 4 {
		return 0, ErrBufferOverflow
	}
	val := binary.LittleEndian.Uint32(r.buf[r.pos:])
	r.pos += 4
	return val, nil
}

// ReadInt32 reads a little-endian signed 32-bit integer.
func (r *Reader) ReadInt32() (int32, error) {
	val, err := r.ReadUint32()
	return int32(val), err
}

// ReadUint64 reads a little-endian unsigned 64-bit integer.
func (r *Reader) ReadUint64() (uint64, error) {
	if r.Remaining() < 8 {
		return 0, ErrBufferOverflow
	}
	val := binary.LittleEndian.Uint64(r.buf[r.pos:])
	r.pos += 8
	return val, nil
}

// ReadInt64 reads a little-endian signed 64-bit integer.
func (r *Reader) ReadInt64() (int64, error) {
	val, err := r.ReadUint64()
	return int64(val), err
}

// ReadFloat32 reads a little-endian 32-bit float.
func (r *Reader) ReadFloat32() (float32, error) {
	if r.Remaining() < 4 {
		return 0, ErrBufferOverflow
	}
	bits := binary.LittleEndian.Uint32(r.buf[r.pos:])
	r.pos += 4
	return float32FromBits(bits), nil
}

// ReadFloat64 reads a little-endian 64-bit float.
func (r *Reader) ReadFloat64() (float64, error) {
	if r.Remaining() < 8 {
		return 0, ErrBufferOverflow
	}
	bits := binary.LittleEndian.Uint64(r.buf[r.pos:])
	r.pos += 8
	return float64FromBits(bits), nil
}

// Read7BitEncodedUInt64 reads a 7-bit encoded unsigned 64-bit integer.
// Each byte uses 7 bits for data and the high bit indicates continuation.
func (r *Reader) Read7BitEncodedUInt64() (uint64, error) {
	var result uint64
	var shift int

	for shift < 64 {
		b, err := r.ReadByte()
		if err != nil {
			return 0, err
		}
		result |= uint64(b&0x7F) << shift
		if (b & 0x80) == 0 {
			break
		}
		shift += 7
	}

	return result, nil
}

// Read7BitEncodedInt64 reads a 7-bit encoded signed 64-bit integer.
// Uses zigzag encoding where the sign bit is stored in the LSB.
func (r *Reader) Read7BitEncodedInt64() (int64, error) {
	val, err := r.Read7BitEncodedUInt64()
	if err != nil {
		return 0, err
	}

	// Zigzag decode: shift right by 1, XOR with sign extension
	result := int64(val >> 1)
	if (val & 1) != 0 {
		result = ^result
	}
	return result, nil
}

// ReadSize reads a message size value.
// If the first byte is 0xFF, the next 2 bytes are read as a uint16.
// Otherwise, the single byte is the size.
func (r *Reader) ReadSize() (int, error) {
	b, err := r.ReadByte()
	if err != nil {
		return 0, err
	}
	if b == 0xFF {
		val, err := r.ReadUint16()
		if err != nil {
			return 0, err
		}
		return int(val), nil
	}
	return int(b), nil
}

// ReadStringUTF8 reads a UTF-8 encoded string of the given byte length.
func (r *Reader) ReadStringUTF8(size int) (string, error) {
	if size < 0 || size > r.Remaining() {
		return "", ErrBufferOverflow
	}
	s := string(r.buf[r.pos : r.pos+size])
	r.pos += size
	return s, nil
}

// ReadStringUnicode reads a UTF-16LE encoded string of the given character count.
func (r *Reader) ReadStringUnicode(charCount int) (string, error) {
	byteSize := charCount * 2
	if byteSize < 0 || byteSize > r.Remaining() {
		return "", ErrBufferOverflow
	}

	u16s := make([]uint16, charCount)
	for i := 0; i < charCount; i++ {
		u16s[i] = binary.LittleEndian.Uint16(r.buf[r.pos+i*2:])
	}
	r.pos += byteSize

	return string(utf16.Decode(u16s)), nil
}

// Slice returns a slice of the buffer from the current position.
func (r *Reader) Slice(length int) ([]byte, error) {
	if length < 0 || length > r.Remaining() {
		return nil, ErrBufferOverflow
	}
	return r.buf[r.pos : r.pos+length], nil
}

// float32FromBits converts a uint32 to float32.
func float32FromBits(bits uint32) float32 {
	return *(*float32)(unsafe.Pointer(&bits))
}

// float64FromBits converts a uint64 to float64.
func float64FromBits(bits uint64) float64 {
	return *(*float64)(unsafe.Pointer(&bits))
}
