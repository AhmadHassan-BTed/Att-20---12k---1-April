package buffer

import (
	"testing"
)

func TestReader_ReadByte(t *testing.T) {
	data := []byte{0x42, 0xFF, 0x00}
	reader := NewReader(data)

	b, err := reader.ReadByte()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if b != 0x42 {
		t.Errorf("expected 0x42, got 0x%02X", b)
	}

	if reader.Position() != 1 {
		t.Errorf("expected position 1, got %d", reader.Position())
	}
}

func TestReader_ReadUint16(t *testing.T) {
	// Little-endian: 0x1234 = bytes [0x34, 0x12]
	data := []byte{0x34, 0x12}
	reader := NewReader(data)

	val, err := reader.ReadUint16()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if val != 0x1234 {
		t.Errorf("expected 0x1234, got 0x%04X", val)
	}
}

func TestReader_ReadUint32(t *testing.T) {
	// Little-endian: 0x12345678 = bytes [0x78, 0x56, 0x34, 0x12]
	data := []byte{0x78, 0x56, 0x34, 0x12}
	reader := NewReader(data)

	val, err := reader.ReadUint32()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if val != 0x12345678 {
		t.Errorf("expected 0x12345678, got 0x%08X", val)
	}
}

func TestReader_Read7BitEncodedUInt64(t *testing.T) {
	tests := []struct {
		name     string
		input    []byte
		expected uint64
	}{
		{"zero", []byte{0x00}, 0},
		{"one", []byte{0x01}, 1},
		{"127", []byte{0x7F}, 127},
		{"128", []byte{0x80, 0x01}, 128},
		{"255", []byte{0xFF, 0x01}, 255},
		{"256", []byte{0x80, 0x02}, 256},
		{"16383", []byte{0xFF, 0x7F}, 16383},
		{"16384", []byte{0x80, 0x80, 0x01}, 16384},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			reader := NewReader(tt.input)
			val, err := reader.Read7BitEncodedUInt64()
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}
			if val != tt.expected {
				t.Errorf("expected %d, got %d", tt.expected, val)
			}
		})
	}
}

func TestReader_Read7BitEncodedInt64(t *testing.T) {
	tests := []struct {
		name     string
		input    []byte
		expected int64
	}{
		{"zero", []byte{0x00}, 0},
		{"one", []byte{0x02}, 1},
		{"minus one", []byte{0x01}, -1},
		{"two", []byte{0x04}, 2},
		{"minus two", []byte{0x03}, -2},
		{"63", []byte{0x7E}, 63},
		{"minus 64", []byte{0x7F}, -64},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			reader := NewReader(tt.input)
			val, err := reader.Read7BitEncodedInt64()
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}
			if val != tt.expected {
				t.Errorf("expected %d, got %d", tt.expected, val)
			}
		})
	}
}

func TestReader_ReadSize(t *testing.T) {
	tests := []struct {
		name     string
		input    []byte
		expected int
	}{
		{"small", []byte{0x10}, 16},
		{"254", []byte{0xFE}, 254},
		{"large", []byte{0xFF, 0x00, 0x01}, 256},
		{"max_small", []byte{0xFE}, 254},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			reader := NewReader(tt.input)
			val, err := reader.ReadSize()
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}
			if val != tt.expected {
				t.Errorf("expected %d, got %d", tt.expected, val)
			}
		})
	}
}

func TestReader_ReadUint24(t *testing.T) {
	// Big-endian: 0x123456 = bytes [0x12, 0x34, 0x56]
	data := []byte{0x12, 0x34, 0x56}
	reader := NewReader(data)

	val, err := reader.ReadUint24()
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if val != 0x123456 {
		t.Errorf("expected 0x123456, got 0x%06X", val)
	}
}

func TestReader_Overflow(t *testing.T) {
	data := []byte{0x01}
	reader := NewReader(data)

	_, _ = reader.ReadByte()

	_, err := reader.ReadByte()
	if err != ErrBufferOverflow {
		t.Errorf("expected ErrBufferOverflow, got %v", err)
	}
}

func TestReader_ReadStringUTF8(t *testing.T) {
	data := []byte("Hello, World!")
	reader := NewReader(data)

	str, err := reader.ReadStringUTF8(5)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if str != "Hello" {
		t.Errorf("expected 'Hello', got '%s'", str)
	}
}

func TestReader_Advance(t *testing.T) {
	data := []byte{0x01, 0x02, 0x03, 0x04, 0x05}
	reader := NewReader(data)

	err := reader.Advance(3)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if reader.Position() != 3 {
		t.Errorf("expected position 3, got %d", reader.Position())
	}

	err = reader.Advance(3)
	if err != ErrBufferOverflow {
		t.Errorf("expected ErrBufferOverflow, got %v", err)
	}
}

func TestReader_Rewind(t *testing.T) {
	data := []byte{0x01, 0x02, 0x03, 0x04, 0x05}
	reader := NewReader(data)

	reader.Advance(3)
	err := reader.Rewind(2)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if reader.Position() != 1 {
		t.Errorf("expected position 1, got %d", reader.Position())
	}
}
