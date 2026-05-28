package esf

import "math"

// matPalProvider is implemented by sprite types that can provide a MaterialPalette.
// SimpleSprite, SimpleSubSprite, and SkinSubSprite all satisfy this interface.
type matPalProvider interface {
	GetMatPal(file *ObjFile) (*MaterialPalette, error)
}

// Vertex holds a single vertex with position, texture coordinates, normal, color,
// vertex group, bone weights, and an optional comment (used for OBJ export).
type Vertex struct {
	X, Y, Z        float32
	U, V           float32
	NX, NY, NZ     float32
	R, G, B, A     float32
	VGroup         int16
	BoneIdx        [4]byte // bone indices (from skinned pbtype=5)
	BoneWeight     [4]byte // bone weights (from skinned pbtype=5), sum ≈ 127 (PS2 signed bytes)
	Comment        string
}

// VertexList is a triangle strip with associated material and layer information.
type VertexList struct {
	Vertices []Vertex
	Material int
	Layer    int
	Layers   int
}

// PrimBuffer contains mesh/vertex data: vertices, texture UVs, faces (triangle strips),
// normals, and texturing information. Vertex coordinates are stored as packed 16-bit
// integers that are converted to world coordinates using a packing factor and optionally
// adding preTranslation offsets from the parent zone.
type PrimBuffer struct {
	info        *ObjInfo
	VertexLists []VertexList
	MatPal      *MaterialPalette
	BBox        Box
}

func (pb *PrimBuffer) ObjInfo() *ObjInfo { return pb.info }

// Load reads the PrimBuffer data from the ESF file. This is a port of PrimBuffer.java load().
func (pb *PrimBuffer) Load(file *ObjFile) error {
	file.Seek(pb.info.Offset)
	pb.BBox = NewBox()

	ver := pb.info.Version
	if ver == 0 {
		return pb.loadV0(file)
	}

	// version > 1: skip dict_id (already parsed into ObjInfo.DictID)
	if ver > 1 {
		_ = file.readInt32() // dict_id
	}

	pbtype := file.readInt32()
	_ = file.readInt32() // nmats
	nfaces := file.readInt32()
	_ = file.readInt32() // unknown
	p1 := file.readInt32()
	p2 := file.readInt32()
	p3 := file.readInt32()

	packing1 := float32(1.0 / math.Pow(2, float64(p1)))
	packing2 := float32(1.0 / math.Pow(2, float64(p2)))
	_ = p3 // packing3 not currently used

	// Resolve preTranslations if parent is SimpleSubSprite with pretrans enabled
	var preTranslations []Point
	if pb.info.Parent != nil && pb.info.Parent.Type == TypeSimpleSubSprite {
		parentObj, err := file.GetObject(pb.info.Parent)
		if err == nil && parentObj != nil {
			if ss, ok := parentObj.(*SimpleSubSprite); ok && ss.UsePretrans {
				preTranslations = pb.resolvePreTranslations(file)
			}
		}
	}

	// Get MaterialPalette from parent sprite
	if pb.info.Parent != nil {
		parentObj, err := file.GetObject(pb.info.Parent)
		if err == nil && parentObj != nil {
			if mp, ok := parentObj.(matPalProvider); ok {
				pb.MatPal, _ = mp.GetMatPal(file)
			}
		}
	}

	for fi := int32(0); fi < nfaces; fi++ {
		nverts := file.readInt32()
		mat := file.readInt32()

		vl := VertexList{
			Material: int(mat),
			Layer:    0,
			Layers:   1,
		}

		switch pbtype {
		case 2, 4:
			for i := int32(0); i < nverts; i++ {
				x := file.readInt16()
				y := file.readInt16()
				z := file.readInt16()
				u := file.readInt16()
				v := file.readInt16()

				normal := file.readBytes(3)
				color := file.readBytes(4)

				var vgroup int16
				if pbtype == 4 {
					vgroup = file.readInt16()
				}

				px := float32(x) * packing1
				py := float32(y) * packing1
				pz := float32(z) * packing1

				if preTranslations != nil && int(vgroup) < len(preTranslations) {
					px += preTranslations[vgroup].X
					py += preTranslations[vgroup].Y
					pz += preTranslations[vgroup].Z
				}

				pb.BBox.Add(px, py, pz)

				vtx := Vertex{
					X:      px,
					Y:      py,
					Z:      pz,
					U:      float32(u) * packing2,
					V:      float32(v) * packing2,
					NX:     float32(int8(normal[0])) / 127.0,
					NY:     float32(int8(normal[1])) / 127.0,
					NZ:     float32(int8(normal[2])) / 127.0,
					R:      float32(color[0]) / 255.0,
					G:      float32(color[1]) / 255.0,
					B:      float32(color[2]) / 255.0,
					A:      float32(color[3]) / 255.0,
					VGroup: vgroup,
				}
				vl.Vertices = append(vl.Vertices, vtx)
			}

		case 5:
			for i := int32(0); i < nverts; i++ {
				x := file.readInt16()
				y := file.readInt16()
				z := file.readInt16()
				u := file.readInt16()
				v := file.readInt16()

				normal := file.readBytes(3)
				bones := file.readBytes(4)
				weights := file.readBytes(4)

				px := float32(x) * packing1
				py := float32(y) * packing1
				pz := float32(z) * packing1

				pb.BBox.Add(px, py, pz)

				vtx := Vertex{
					X:      px,
					Y:      py,
					Z:      pz,
					U:      float32(u) * packing2,
					V:      float32(v) * packing2,
					NX:     float32(int8(normal[0])) / 127.0,
					NY:     float32(int8(normal[1])) / 127.0,
					NZ:     float32(int8(normal[2])) / 127.0,
					A:      1.0,
					VGroup: -1,
				}
				copy(vtx.BoneIdx[:], bones)
				copy(vtx.BoneWeight[:], weights)
				vl.Vertices = append(vl.Vertices, vtx)
			}
		}

		pb.VertexLists = append(pb.VertexLists, vl)
	}

	return nil
}

// loadV0 handles version 0 PrimBuffers where coordinates are stored as floats
// directly, without packing or preTranslations.
func (pb *PrimBuffer) loadV0(file *ObjFile) error {
	_ = file.readInt32() // numMaterials
	numFaces := file.readInt32()
	_ = file.readInt32() // numSomething

	// Get MaterialPalette from parent sprite
	if pb.info.Parent != nil {
		parentObj, err := file.GetObject(pb.info.Parent)
		if err == nil && parentObj != nil {
			if mp, ok := parentObj.(matPalProvider); ok {
				pb.MatPal, _ = mp.GetMatPal(file)
			}
		}
	}

	for i := int32(0); i < numFaces; i++ {
		numVertices := file.readInt32()
		mat := file.readInt32()

		vl := VertexList{
			Material: int(mat),
			Layer:    0,
			Layers:   1,
		}

		for j := int32(0); j < numVertices; j++ {
			vertex := file.readPoint()
			u := file.readFloat32()
			v := file.readFloat32()
			normal := file.readPoint()
			color := file.readBytes(4)

			pb.BBox.Add(vertex.X, vertex.Y, vertex.Z)

			vtx := Vertex{
				X:  vertex.X,
				Y:  vertex.Y,
				Z:  vertex.Z,
				U:  u,
				V:  v,
				NX: normal.X,
				NY: normal.Y,
				NZ: normal.Z,
				R:  float32(color[0]) / 255.0,
				G:  float32(color[1]) / 255.0,
				B:  float32(color[2]) / 255.0,
				A:  float32(color[3]) / 255.0,
			}
			vl.Vertices = append(vl.Vertices, vtx)
		}

		pb.VertexLists = append(pb.VertexLists, vl)
	}

	return nil
}

// resolvePreTranslations walks up the object tree to find the parent Zone and
// returns its preTranslation table.
func (pb *PrimBuffer) resolvePreTranslations(file *ObjFile) []Point {
	zoneInfo := pb.info.ParentOfType(TypeZone)
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

// Center offsets all vertices so that the centroid is at the origin.
func (pb *PrimBuffer) Center() {
	box := NewBox()
	for i := range pb.VertexLists {
		for j := range pb.VertexLists[i].Vertices {
			v := &pb.VertexLists[i].Vertices[j]
			box.Add(v.X, v.Y, v.Z)
		}
	}
	c := box.Center()
	for i := range pb.VertexLists {
		for j := range pb.VertexLists[i].Vertices {
			v := &pb.VertexLists[i].Vertices[j]
			v.X -= c.X
			v.Y -= c.Y
			v.Z -= c.Z
		}
	}
}

// GetBox computes and returns the bounding box of all vertices.
func (pb *PrimBuffer) GetBox() Box {
	ret := NewBox()
	for i := range pb.VertexLists {
		for j := range pb.VertexLists[i].Vertices {
			v := &pb.VertexLists[i].Vertices[j]
			ret.Add(v.X, v.Y, v.Z)
		}
	}
	return ret
}

// Translate offsets all vertices by the given point.
func (pb *PrimBuffer) Translate(p Point) {
	for i := range pb.VertexLists {
		for j := range pb.VertexLists[i].Vertices {
			v := &pb.VertexLists[i].Vertices[j]
			v.X += p.X
			v.Y += p.Y
			v.Z += p.Z
		}
	}
}

// FaceCountOfMaterial returns the number of triangle faces for a given material group.
// Pass materialGroup < 0 to count all faces.
func (pb *PrimBuffer) FaceCountOfMaterial(materialGroup int) int {
	count := 0
	for i := range pb.VertexLists {
		if materialGroup < 0 || pb.VertexLists[i].Material == materialGroup {
			n := len(pb.VertexLists[i].Vertices) - 2
			if n > 0 {
				count += n
			}
		}
	}
	return count
}

// NumberOfVertices returns the total number of vertices across all vertex lists.
func (pb *PrimBuffer) NumberOfVertices() int {
	count := 0
	for i := range pb.VertexLists {
		count += len(pb.VertexLists[i].Vertices)
	}
	return count
}

// VertexCountOfMaterial returns the vertex count for a given material group.
// Pass materialGroup < 0 to count all vertices.
func (pb *PrimBuffer) VertexCountOfMaterial(materialGroup int) int {
	count := 0
	for i := range pb.VertexLists {
		if materialGroup < 0 || pb.VertexLists[i].Material == materialGroup {
			count += len(pb.VertexLists[i].Vertices)
		}
	}
	return count
}
