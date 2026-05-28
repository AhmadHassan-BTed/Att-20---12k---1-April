package buffer

import (
	"bytes"
	"testing"
)

func TestWriter_WriteByte(t *testing.T) {
	buf := make([]byte, 10)
	writer := NewWriter(buf)

	err := writer.WriteByte(0x42)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	if buf[0] != 0x42 {
		t.Errorf("expected 0x42, got 0x%02X", buf[0])
	}
	if writer.Position() != 1 {
		t.Errorf("expected position 1, got %d", writer.Position())
	}
}

func TestWriter_WriteUint16(t *testing.T) {
	buf := make([]byte, 10)
	writer := NewWriter(buf)

	err := writer.WriteUint16(0x1234)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	// Little-endian: 0x1234 = bytes [0x34, 0x12]
	if buf[0] != 0x34 || buf[1] != 0x12 {
		t.Errorf("expected [0x34, 0x12], got [0x%02X, 0x%02X]", buf[0], buf[1])
	}
}

func TestWriter_WriteUint32(t *testing.T) {
	buf := make([]byte, 10)
	writer := NewWriter(buf)

	err := writer.WriteUint32(0x12345678)
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}

	// Little-endian: 0x12345678 = bytes [0x78, 0x56, 0x34, 0x12]
	expected := []byte{0x78, 0x56, 0x34, 0x12}
	if !bytes.Equal(buf[:4], expected) {
		t.Errorf("expected %v, got %v", expected, buf[:4])
	}
}

func TestWriter_Write7BitEncodedUInt64(t *testing.T) {
	tests := []struct {
		name     string
		value    uint64
		expected []byte
	}{
		{"zero", 0, []byte{0x00}},
		{"one", 1, []byte{0x01}},
		{"127", 127, []byte{0x7F}},
		{"128", 128, []byte{0x80, 0x01}},
		{"255", 255, []byte{0xFF, 0x01}},
		{"256", 256, []byte{0x80, 0x02}},
		{"16383", 16383, []byte{0xFF, 0x7F}},
		{"16384", 16384, []byte{0x80, 0x80, 0x01}},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			buf := make([]byte, 10)
			writer := NewWriter(buf)

			err := writer.Write7BitEncodedUInt64(tt.value)
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}

			result := buf[:writer.Position()]
			if !bytes.Equal(result, tt.expected) {
				t.Errorf("expected %v, got %v", tt.expected, result)
			}
		})
	}
}

func TestWriter_Write7BitEncodedInt64(t *testing.T) {
	tests := []struct {
		name     string
		value    int64
		expected []byte
	}{
		{"zero", 0, []byte{0x00}},
		{"one", 1, []byte{0x02}},
		{"minus one", -1, []byte{0x01}},
		{"two", 2, []byte{0x04}},
		{"minus two", -2, []byte{0x03}},
		{"63", 63, []byte{0x7E}},
		{"minus 64", -64, []byte{0x7F}},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			buf := make([]byte, 10)
			writer := NewWriter(buf)

			err := writer.Write7BitEncodedInt64(tt.value)
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}

			result := buf[:writer.Position()]
			if !bytes.Equal(result, tt.expected) {
				t.Errorf("expected %v, got %v", tt.expected, result)
			}
		})
	}
}

func TestWriter_WriteSize(t *testing.T) {
	tests := []struct {
		name     string
		size     int
		expected []byte
	}{
		{"small", 16, []byte{0x10}},
		{"254", 254, []byte{0xFE}},
		{"255", 255, []byte{0xFF, 0xFF, 0x00}},
		{"256", 256, []byte{0xFF, 0x00, 0x01}},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			buf := make([]byte, 10)
			writer := NewWriter(buf)

			err := writer.WriteSize(tt.size)
			if err != nil {
				t.Fatalf("unexpected error: %v", err)
			}

			result := buf[:writer.Position()]
			if !bytes.Equal(result, tt.expected) {
				t.Errorf("expected %v, got %v", tt.expected, result)
			}
		})
	}
}

func TestWriter_Overflow(t *testing.T) {
	buf := make([]byte, 1)
	writer := NewWriter(buf)

	_ = writer.WriteByte(0x01)

	err := writer.WriteByte(0x02)
	if err != ErrWriterOverflow {
		t.Errorf("expected ErrWriterOverflow, got %v", err)
	}
}

func TestWriter_Bytes(t *testing.T) {
	buf := make([]byte, 10)
	writer := NewWriter(buf)

	writer.WriteByte(0x01)
	writer.WriteByte(0x02)
	writer.WriteByte(0x03)

	result := writer.Bytes()
	expected := []byte{0x01, 0x02, 0x03}
	if !bytes.Equal(result, expected) {
		t.Errorf("expected %v, got %v", expected, result)
	}
}

func Test7BitRoundtrip(t *testing.T) {
	tests := []uint64{
		0, 1, 127, 128, 255, 256, 16383, 16384,
		65535, 1000000, 0xFFFFFFFF, 0xFFFFFFFFFFFFFFFF,
	}

	for _, val := range tests {
		buf := make([]byte, 10)
		writer := NewWriter(buf)

		err := writer.Write7BitEncodedUInt64(val)
		if err != nil {
			t.Fatalf("write error for %d: %v", val, err)
		}

		reader := NewReader(buf[:writer.Position()])
		result, err := reader.Read7BitEncodedUInt64()
		if err != nil {
			t.Fatalf("read error for %d: %v", val, err)
		}

		if result != val {
			t.Errorf("roundtrip failed: wrote %d, read %d", val, result)
		}
	}
}

func Test7BitSignedRoundtrip(t *testing.T) {
	tests := []int64{
		0, 1, -1, 63, -64, 127, -128, 1000, -1000,
		0x7FFFFFFF, -0x80000000, 0x7FFFFFFFFFFFFFFF,
	}

	for _, val := range tests {
		buf := make([]byte, 12)
		writer := NewWriter(buf)

		err := writer.Write7BitEncodedInt64(val)
		if err != nil {
			t.Fatalf("write error for %d: %v", val, err)
		}

		reader := NewReader(buf[:writer.Position()])
		result, err := reader.Read7BitEncodedInt64()
		if err != nil {
			t.Fatalf("read error for %d: %v", val, err)
		}

		if result != val {
			t.Errorf("roundtrip failed: wrote %d, read %d", val, result)
		}
	}
}
