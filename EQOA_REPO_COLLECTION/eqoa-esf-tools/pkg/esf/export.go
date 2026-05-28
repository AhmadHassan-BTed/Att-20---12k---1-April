package esf

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

// ObjExporter accumulates PrimBuffer vertex lists from multiple sprites into
// one combined mesh and writes Wavefront OBJ + MTL files with textures as
// PNGs. This is a port of ObjExport.java.
type ObjExporter struct {
	prim       *PrimBuffer // combined vertices
	matPals    []*MaterialPalette
	SizeCutoff float32
	ExportColl bool // if true, export collision buffers instead of prim buffers
}

// NewExporter creates a new ObjExporter with an empty combined PrimBuffer.
func NewExporter() *ObjExporter {
	return &ObjExporter{
		prim: &PrimBuffer{
			VertexLists: nil,
			BBox:        NewBox(),
		},
	}
}

// Add adds a single sprite placement to the export. If ExportColl is true,
// collision buffers are exported; otherwise prim buffers are exported.
func (e *ObjExporter) Add(sp *SpritePlacement, file *ObjFile) error {
	s, err := sp.GetSprite(file)
	if err != nil {
		return err
	}
	if s == nil {
		return nil
	}

	// Apply size cutoff, but don't omit SimpleSubSprite (terrain pieces).
	if e.SizeCutoff > 0 && s.BBox.Size() < e.SizeCutoff {
		if s.info.Type != TypeSimpleSubSprite {
			return nil
		}
	}

	if e.ExportColl {
		return e.addCollBuffer(sp, file, s)
	}
	return e.addPrimBuffer(sp, file, s)
}

// addPrimBuffer adds the sprite's PrimBuffer vertices to the combined mesh.
func (e *ObjExporter) addPrimBuffer(sp *SpritePlacement, file *ObjFile, s *SimpleSprite) error {
	pb, err := s.GetPrimBuffer(file)
	if err != nil {
		return err
	}
	if pb == nil {
		return nil
	}

	mp := pb.MatPal
	if mp == nil {
		// Try to get it from the sprite directly
		mp, _ = s.GetMatPal(file)
	}

	// Track material palettes for global material indexing.
	matIdx := -1
	if mp != nil {
		for i, existing := range e.matPals {
			if existing == mp {
				matIdx = i
				break
			}
		}
		if matIdx < 0 {
			matIdx = len(e.matPals)
			e.matPals = append(e.matPals, mp)
		}
	}

	// Compute material offset from all prior palettes.
	matOffset := 0
	for i := 0; i < matIdx; i++ {
		matOffset += len(e.matPals[i].Materials)
	}

	first := true
	for _, list := range pb.VertexLists {
		vl := VertexList{
			Material: list.Material + matOffset,
			Layer:    list.Layer,
			Layers:   list.Layers,
		}
		for _, v := range list.Vertices {
			p := Point{v.X, v.Y, v.Z}
			sp.Transform(&p)
			vtx := Vertex{
				X:  p.X,
				Y:  p.Y,
				Z:  p.Z,
				U:  v.U,
				V:  v.V,
				NX: v.NX,
				NY: v.NY,
				NZ: v.NZ,
				R:  v.R,
				G:  v.G,
				B:  v.B,
			}
			if first {
				first = false
				vtx.Comment = fmt.Sprintf("%s", s.info)
			}
			vl.Vertices = append(vl.Vertices, vtx)
		}
		e.prim.VertexLists = append(e.prim.VertexLists, vl)
	}
	return nil
}

// addCollBuffer adds the sprite's CollBuffer vertices to the combined mesh.
func (e *ObjExporter) addCollBuffer(sp *SpritePlacement, file *ObjFile, s *SimpleSprite) error {
	cb, err := s.GetCollBuffer(file)
	if err != nil {
		return err
	}
	if cb == nil {
		return nil
	}

	first := true
	for _, list := range cb.Lists {
		vl := VertexList{}
		for _, v := range list.Vertices {
			p := Point{v.X, v.Y, v.Z}
			if sp != nil {
				sp.Transform(&p)
			}
			vtx := Vertex{
				X: p.X,
				Y: p.Y,
				Z: p.Z,
			}
			if first {
				first = false
				vtx.Comment = fmt.Sprintf("%s", s.info)
			}
			vl.Vertices = append(vl.Vertices, vtx)
		}
		e.prim.VertexLists = append(e.prim.VertexLists, vl)
	}
	return nil
}

// AddAll adds multiple sprite placements to the export.
func (e *ObjExporter) AddAll(sps []*SpritePlacement, file *ObjFile) error {
	for _, sp := range sps {
		if err := e.Add(sp, file); err != nil {
			return err
		}
	}
	return nil
}

// Center centers all geometry around the origin.
func (e *ObjExporter) Center() {
	e.prim.Center()
}

// VertexCount returns the total number of vertices in the combined buffer.
func (e *ObjExporter) VertexCount() int {
	return e.prim.NumberOfVertices()
}

// Write writes OBJ + MTL + texture PNGs to disk. filename should end with
// ".obj". The MTL file and texture directory are derived from the OBJ name.
func (e *ObjExporter) Write(filename string) error {
	// Derive base name without extension.
	filebase := filename
	lower := strings.ToLower(filebase)
	if strings.HasSuffix(lower, ".obj") {
		filebase = filebase[:len(filebase)-4]
	}

	// Write MTL file first (this also saves textures).
	mtlFile := filebase + ".mtl"
	if err := e.writeMTL(mtlFile, filebase); err != nil {
		return fmt.Errorf("writing MTL: %w", err)
	}

	// Write OBJ file.
	if err := e.writeOBJ(filename, mtlFile); err != nil {
		return fmt.Errorf("writing OBJ: %w", err)
	}

	return nil
}

// writeOBJ writes the Wavefront OBJ file referencing the given MTL file.
func (e *ObjExporter) writeOBJ(filename, mtlFile string) error {
	f, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer f.Close()

	lists := e.prim.VertexLists

	// Track first vertex index for each vertex list (OBJ indices are 1-based).
	first := make([]int, len(lists))

	fmt.Fprintf(f, "mtllib %s\n", filepath.Base(mtlFile))

	idx := 0
	for s, list := range lists {
		first[s] = idx
		fmt.Fprintf(f, "# verts %d .. %d material %d\n",
			idx+1, idx+1+len(list.Vertices), list.Material)

		for _, v := range list.Vertices {
			if v.Comment != "" {
				fmt.Fprintf(f, "# %s\n", v.Comment)
			}
			fmt.Fprintf(f, "v %f %f %f\n", v.X, v.Y, v.Z)
		}
		for _, v := range list.Vertices {
			fmt.Fprintf(f, "vt %f %f\n", v.U, 1-v.V)
		}
		for _, v := range list.Vertices {
			fmt.Fprintf(f, "vn %f %f %f\n", v.NX, v.NY, v.NZ)
		}
		idx += len(list.Vertices)
	}

	fmt.Fprintln(f)

	for s, list := range lists {
		fmt.Fprintf(f, "list=%d layers=%d\n", s, list.Layers)
		fmt.Fprintf(f, "# layer 0/%d\n", list.Layers)
		fmt.Fprintf(f, "usemtl material%d-0\n", list.Material)
		fmt.Fprintf(f, "# first=%d size=%d\n", first[s], len(list.Vertices))

		odd := true
		for j := 1; j <= len(list.Vertices)-2; j++ {
			i := j + first[s]
			// OBJ indices are 1-based; i already represents the 0-based
			// global index of the second vertex in the strip window, so
			// the three vertices of this triangle are at i, i+1, i+2
			// (1-based).
			if !odd {
				fmt.Fprintf(f, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
					i, i, i, i+1, i+1, i+1, i+2, i+2, i+2)
			} else {
				fmt.Fprintf(f, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
					i, i, i, i+2, i+2, i+2, i+1, i+1, i+1)
			}
			odd = !odd
		}
	}

	return nil
}

// writeMTL writes the Wavefront MTL file and saves texture PNGs.
func (e *ObjExporter) writeMTL(filename, filebase string) error {
	// If no material palettes, write an empty MTL.
	mats := e.matPals
	if len(mats) == 0 {
		return os.WriteFile(filename, nil, 0o644)
	}

	f, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer f.Close()

	texDir := filebase + "_tex"
	relTexDir := filepath.Base(texDir) // relative path for MTL references
	matIndex := 0

	for _, mp := range mats {
		for _, m := range mp.Materials {
			if len(m.Layers) == 0 {
				matIndex++
				continue
			}
			layer := m.Layers[0]

			var texFile, alpFile string
			if layer.Surface != nil {
				texFile = fmt.Sprintf("%s%d-0.png", filepath.Base(filebase), matIndex)
				if layer.Surface.HasAlpha {
					alpFile = fmt.Sprintf("%s%d-0-alpha.png", filepath.Base(filebase), matIndex)
				}
				if err := layer.Surface.SaveTexture(texDir, texFile, alpFile); err != nil {
					// Non-fatal: continue even if texture save fails.
					texFile = ""
					alpFile = ""
				}
			}

			// Ambient color from layer 0.
			col := m.Layers[0].Color
			cr := float32(col[0]&0xFF) / 255.0
			cg := float32(col[1]&0xFF) / 255.0
			cb := float32(col[2]&0xFF) / 255.0

			fmt.Fprintf(f, "newmtl material%d-0\n", matIndex)
			fmt.Fprintf(f, "# blendmode=%d wrapmode=%d flags=%d uvrate=%f,%f uvt=[%f %f %f %f %f %f %f %f %f]\n",
				layer.BlendMode, layer.WrapMode, layer.Flags,
				layer.URate, layer.VRate,
				layer.UVTransform[0], layer.UVTransform[1], layer.UVTransform[2],
				layer.UVTransform[3], layer.UVTransform[4], layer.UVTransform[5],
				layer.UVTransform[6], layer.UVTransform[7], layer.UVTransform[8])
			if layer.Surface != nil {
				fmt.Fprintf(f, "# surface depth=%d mip=%d\n", layer.Surface.Depth, layer.Surface.Mip)
			}
			fmt.Fprintf(f, "Ka %f %f %f\n", cr, cg, cb)
			fmt.Fprintln(f, "Kd 1 1 1")
			fmt.Fprintln(f, "Ks 1 1 1")
			fmt.Fprintln(f, "illum 1")

			// UV scale from uvTransform.
			scale := ""
			if layer.UVTransform[0] != 1 || layer.UVTransform[4] != 1 {
				scale = fmt.Sprintf("%f %f 1 ", 1.0/layer.UVTransform[0], 1.0/layer.UVTransform[4])
			}

			if alpFile != "" {
				fmt.Fprintf(f, "map_d %s%s\n", scale, filepath.Join(relTexDir, alpFile))
			}
			if texFile != "" {
				fmt.Fprintf(f, "map_Kd %s%s\n", scale, filepath.Join(relTexDir, texFile))
			}
			fmt.Fprintln(f)

			matIndex++
		}
	}

	return nil
}
