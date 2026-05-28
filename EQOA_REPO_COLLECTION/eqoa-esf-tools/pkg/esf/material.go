package esf

import "fmt"

// MaterialLayer holds the data for a single layer within a Material.
type MaterialLayer struct {
	Flags       int32
	TexID       int32
	WrapMode    int32
	BlendMode   int32
	Color       [4]byte
	UVTransform [9]float32
	LODBias     float32
	URate       float32
	VRate       float32
	Surface     *Surface
}

// Material represents a material object in an ESF file, composed of
// one or more texture layers with blending and transform parameters.
type Material struct {
	info        *ObjInfo
	Tessellate  int32
	EmissiveCol [4]byte
	Layers      []*MaterialLayer
}

func (m *Material) ObjInfo() *ObjInfo { return m.info }

// Load reads the material data from the ESF file, including all layers
// and their associated surface (texture) references.
func (m *Material) Load(file *ObjFile) error {
	file.Seek(m.info.Offset)

	nlayers := file.readInt32()
	ver := m.info.Version

	if ver > 1 {
		m.Tessellate = file.readInt32()
	}
	if ver > 2 {
		m.EmissiveCol = file.readColor()
	}

	m.Layers = make([]*MaterialLayer, nlayers)
	for i := int32(0); i < nlayers; i++ {
		layer := &MaterialLayer{}
		m.Layers[i] = layer

		layer.Flags = file.readInt32()

		texID := file.readInt32()
		layer.TexID = texID
		if texID != 0 {
			obj, err := file.FindObject(texID)
			if err != nil {
				return fmt.Errorf("material layer %d: finding surface 0x%x: %w", i, texID, err)
			}
			if obj != nil {
				if surf, ok := obj.(*Surface); ok {
					layer.Surface = surf
				}
			}
		}

		layer.WrapMode = file.readInt32()
		layer.BlendMode = file.readInt32()
		layer.Color = file.readColor()

		for j := 0; j < 9; j++ {
			layer.UVTransform[j] = file.readFloat32()
		}

		if ver == 0 {
			_ = file.readFloat32() // discard
			layer.LODBias = 0.80000001
		} else {
			layer.LODBias = file.readFloat32()
		}

		layer.URate = file.readFloat32()
		layer.VRate = file.readFloat32()
	}

	return nil
}

// MaterialPalette is a collection of materials, loaded by navigating
// to the child MaterialArray node and collecting all Material children.
type MaterialPalette struct {
	info      *ObjInfo
	Materials []*Material
}

func (mp *MaterialPalette) ObjInfo() *ObjInfo { return mp.info }

// Load populates the palette by finding the MaterialArray child and
// loading each Material within it.
func (mp *MaterialPalette) Load(file *ObjFile) error {
	arrayInfo := mp.info.Child(TypeMaterialArray)
	if arrayInfo == nil {
		return nil
	}

	matInfos := arrayInfo.ChildrenOfType(TypeMaterial)
	mp.Materials = make([]*Material, 0, len(matInfos))
	for _, mi := range matInfos {
		obj, err := file.GetObject(mi)
		if err != nil {
			return fmt.Errorf("loading material at offset 0x%x: %w", mi.Offset, err)
		}
		if obj != nil {
			if mat, ok := obj.(*Material); ok {
				mp.Materials = append(mp.Materials, mat)
			}
		}
	}

	return nil
}
