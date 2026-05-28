package esf

import "math"

// CollVertex holds a single collision mesh vertex with position, vertex group,
// and flora type information.
type CollVertex struct {
	X, Y, Z     float32
	VertexGroup int
	FloraType   int
}

// CollVertexList is a triangle strip of collision vertices with a bounding box.
type CollVertexList struct {
	Vertices []CollVertex
	BBox     Box
}

// NumTriangles returns the number of triangles in this strip.
func (vl *CollVertexList) NumTriangles() int {
	n := len(vl.Vertices) - 2
	if n < 0 {
		return 0
	}
	return n
}

// CollBuffer is a simplified 3D collision mesh without textures.
// It is a sub-object of a Sprite. This is a port of CollBuffer.java.
type CollBuffer struct {
	info  *ObjInfo
	Lists []CollVertexList
}

func (cb *CollBuffer) ObjInfo() *ObjInfo { return cb.info }

// Load reads the CollBuffer data from the ESF file.
func (cb *CollBuffer) Load(file *ObjFile) error {
	file.Seek(cb.info.Offset)

	ver := cb.info.Version
	var cbtype int32
	if ver > 1 {
		cbtype = file.readInt32()
	}

	_ = file.readInt32()            // numPrimgroups
	numVertexGroups := file.readInt32() // number of vertex groups (loop count)
	_ = file.readInt32()            // numSomething

	var packing int32
	if ver >= 2 {
		packing = file.readInt32()
	}

	p := float32(1.0 / math.Pow(2, float64(packing)))

	// Resolve preTranslations if parent is SimpleSubSprite
	var preTranslations []Point
	if cb.info.Parent != nil && cb.info.Parent.Type == TypeSimpleSubSprite {
		preTranslations = cb.resolvePreTranslations(file)
	}

	for i := int32(0); i < numVertexGroups; i++ {
		num := file.readInt32()
		_ = file.readInt32() // primg
		_ = file.readInt32() // list

		vl := CollVertexList{
			BBox: NewBox(),
		}

		switch cbtype {
		case 0:
			// PS2: 3 × ReadFloat32 per vertex (no vertex group, no flora).
			for j := int32(0); j < num; j++ {
				vx := file.readFloat32()
				vy := file.readFloat32()
				vz := file.readFloat32()

				vl.Vertices = append(vl.Vertices, CollVertex{
					X:           vx,
					Y:           vy,
					Z:           vz,
					VertexGroup: -1,
					FloraType:   -1,
				})
			}

		case 1:
			// PS2: 3 × ReadInt16 per vertex (no vertex group, no flora).
			for j := int32(0); j < num; j++ {
				x := file.readInt16()
				y := file.readInt16()
				z := file.readInt16()

				vl.Vertices = append(vl.Vertices, CollVertex{
					X:           float32(x) * p,
					Y:           float32(y) * p,
					Z:           float32(z) * p,
					VertexGroup: -1,
					FloraType:   -1,
				})
			}

		case 2:
			// PS2: 3 × ReadInt16 + ReadInt16(vertex group byte) per vertex.
			// PS2 reads 4 int16 values (8 bytes) but truncates the 4th to a
			// signed byte for the vertex group index.
			for j := int32(0); j < num; j++ {
				x := file.readInt16()
				y := file.readInt16()
				z := file.readInt16()
				vgroup := int(int8(file.readInt16()))

				vx := float32(x) * p
				vy := float32(y) * p
				vz := float32(z) * p

				if preTranslations != nil {
					if vgroup >= 0 && vgroup < len(preTranslations) {
						vx += preTranslations[vgroup].X
						vy += preTranslations[vgroup].Y
						vz += preTranslations[vgroup].Z
					}
				}

				vl.Vertices = append(vl.Vertices, CollVertex{
					X:           vx,
					Y:           vy,
					Z:           vz,
					VertexGroup: vgroup,
					FloraType:   -1,
				})
			}

		case 3:
			// PS2: 3 × ReadInt16 + ReadSChar(vgroup) + ReadSChar(flora).
			// PS2 uses Read__9VIObjFileRSc (signed char) for both fields.
			for j := int32(0); j < num; j++ {
				x := file.readInt16()
				y := file.readInt16()
				z := file.readInt16()
				vgroup := int(int8(file.readByte()))
				flora := int(int8(file.readByte()))

				vx := float32(x) * p
				vy := float32(y) * p
				vz := float32(z) * p

				// Apply preTranslation offset if available.
				// Negative vgroup (-1) means "no group" — skip translation.
				if preTranslations != nil && vgroup >= 0 && vgroup < len(preTranslations) {
					vx += preTranslations[vgroup].X
					vy += preTranslations[vgroup].Y
					vz += preTranslations[vgroup].Z
				}

				vl.Vertices = append(vl.Vertices, CollVertex{
					X:           vx,
					Y:           vy,
					Z:           vz,
					VertexGroup: vgroup,
					FloraType:   flora,
				})
			}
		}

		cb.Lists = append(cb.Lists, vl)
	}

	return nil
}

// resolvePreTranslations walks up the object tree to find the parent Zone and
// returns its preTranslation table.
func (cb *CollBuffer) resolvePreTranslations(file *ObjFile) []Point {
	zoneInfo := cb.info.ParentOfType(TypeZone)
	if zoneInfo == nil {
		return nil
	}
	zoneObj, err := file.GetObject(zoneInfo)
	if err != nil {
		return nil
	}
	zone, ok := zoneObj.(*Zone)
	if !ok {
		return nil
	}
	base, err := zone.GetZoneBase(file)
	if err != nil || base == nil {
		return nil
	}
	return base.PreTranslations
}

// CalculateBoxes computes the bounding box for each vertex list.
func (cb *CollBuffer) CalculateBoxes() {
	for i := range cb.Lists {
		cb.Lists[i].BBox = NewBox()
		for j := range cb.Lists[i].Vertices {
			v := &cb.Lists[i].Vertices[j]
			cb.Lists[i].BBox.Add(v.X, v.Y, v.Z)
		}
	}
}
