package vi

// VIVect3 is a PS2 3D vector (12 bytes).
type VIVect3 struct {
	X, Y, Z float32
}

// VIVect2 is a PS2 2D vector (8 bytes).
type VIVect2 struct {
	U, V float32
}

// VIColor32 is a PS2 RGBA color (4 bytes, 0-255 per channel).
type VIColor32 struct {
	R, G, B, A byte
}

// VIColor32F is a PS2 RGBA color as floats (0.0-1.0).
type VIColor32F struct {
	R, G, B, A float32
}

// VIBox is a PS2 axis-aligned bounding box.
type VIBox struct {
	Min, Max VIVect3
}
