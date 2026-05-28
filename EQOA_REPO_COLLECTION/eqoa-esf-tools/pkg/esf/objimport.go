package esf

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// ImportedMesh holds parsed OBJ data.
type ImportedMesh struct {
	Positions [][3]float32
	UVs       [][2]float32
	Normals   [][3]float32
	Colors    [][3]float32 // per-vertex colors (from v x y z r g b extension)
	Groups    []ImportedGroup
}

// ImportedGroup is a set of triangles sharing a material.
type ImportedGroup struct {
	Material string
	Faces    [][3][3]int // [face][vertex_in_face][pos/uv/normal index] (1-based)
}

// ImportOBJ reads a Wavefront OBJ file and returns the parsed mesh data.
// Supports v, vt, vn, usemtl, and f directives. Quads and n-gons are
// fan-triangulated. The optional v x y z r g b extension is supported
// for per-vertex colors.
func ImportOBJ(path string) (*ImportedMesh, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	mesh := &ImportedMesh{}
	currentMat := "default"
	groupMap := map[string]int{}

	scanner := bufio.NewScanner(f)
	scanner.Buffer(make([]byte, 1024*1024), 1024*1024)

	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" || line[0] == '#' {
			continue
		}

		fields := strings.Fields(line)
		if len(fields) == 0 {
			continue
		}

		switch fields[0] {
		case "v":
			if len(fields) < 4 {
				continue
			}
			x, _ := strconv.ParseFloat(fields[1], 32)
			y, _ := strconv.ParseFloat(fields[2], 32)
			z, _ := strconv.ParseFloat(fields[3], 32)
			mesh.Positions = append(mesh.Positions, [3]float32{float32(x), float32(y), float32(z)})
			// Optional per-vertex color (v x y z r g b)
			if len(fields) >= 7 {
				r, _ := strconv.ParseFloat(fields[4], 32)
				g, _ := strconv.ParseFloat(fields[5], 32)
				b, _ := strconv.ParseFloat(fields[6], 32)
				mesh.Colors = append(mesh.Colors, [3]float32{float32(r), float32(g), float32(b)})
			} else {
				mesh.Colors = append(mesh.Colors, [3]float32{1, 1, 1})
			}

		case "vt":
			if len(fields) < 3 {
				continue
			}
			u, _ := strconv.ParseFloat(fields[1], 32)
			v, _ := strconv.ParseFloat(fields[2], 32)
			mesh.UVs = append(mesh.UVs, [2]float32{float32(u), float32(v)})

		case "vn":
			if len(fields) < 4 {
				continue
			}
			nx, _ := strconv.ParseFloat(fields[1], 32)
			ny, _ := strconv.ParseFloat(fields[2], 32)
			nz, _ := strconv.ParseFloat(fields[3], 32)
			mesh.Normals = append(mesh.Normals, [3]float32{float32(nx), float32(ny), float32(nz)})

		case "usemtl":
			if len(fields) >= 2 {
				currentMat = fields[1]
			}

		case "f":
			if len(fields) < 4 {
				continue
			}
			verts := make([][3]int, 0, len(fields)-1)
			for _, fv := range fields[1:] {
				verts = append(verts, parseFaceVertex(fv))
			}
			// Get or create material group
			gi, ok := groupMap[currentMat]
			if !ok {
				gi = len(mesh.Groups)
				groupMap[currentMat] = gi
				mesh.Groups = append(mesh.Groups, ImportedGroup{Material: currentMat})
			}
			// Fan-triangulate (works for tris, quads, n-gons)
			for i := 1; i < len(verts)-1; i++ {
				mesh.Groups[gi].Faces = append(mesh.Groups[gi].Faces,
					[3][3]int{verts[0], verts[i], verts[i+1]})
			}
		}
	}

	return mesh, scanner.Err()
}

func parseFaceVertex(s string) [3]int {
	parts := strings.Split(s, "/")
	var result [3]int
	if len(parts) >= 1 {
		result[0], _ = strconv.Atoi(parts[0])
	}
	if len(parts) >= 2 && parts[1] != "" {
		result[1], _ = strconv.Atoi(parts[1])
	}
	if len(parts) >= 3 {
		result[2], _ = strconv.Atoi(parts[2])
	}
	return result
}

// ToPrimBuffer converts the imported mesh into a PrimBuffer suitable for
// ESF serialization. Each material group becomes a set of 3-vertex faces
// (individual triangles). Vertex colors come from the OBJ data or default
// to white.
func (m *ImportedMesh) ToPrimBuffer() *PrimBuffer {
	pb := &PrimBuffer{
		BBox: NewBox(),
	}

	for gi, group := range m.Groups {
		for _, face := range group.Faces {
			vl := VertexList{
				Material: gi,
				Layer:    0,
				Layers:   1,
			}
			for _, vi := range face {
				var vtx Vertex
				vtx.R, vtx.G, vtx.B, vtx.A = 1, 1, 1, 1

				if vi[0] > 0 && vi[0] <= len(m.Positions) {
					p := m.Positions[vi[0]-1]
					vtx.X, vtx.Y, vtx.Z = p[0], p[1], p[2]
					// Apply per-vertex color if available
					if vi[0]-1 < len(m.Colors) {
						c := m.Colors[vi[0]-1]
						vtx.R, vtx.G, vtx.B = c[0], c[1], c[2]
					}
				}
				if vi[1] > 0 && vi[1] <= len(m.UVs) {
					uv := m.UVs[vi[1]-1]
					vtx.U = uv[0]
					vtx.V = 1 - uv[1] // flip V back (OBJ convention vs ESF)
				}
				if vi[2] > 0 && vi[2] <= len(m.Normals) {
					n := m.Normals[vi[2]-1]
					vtx.NX, vtx.NY, vtx.NZ = n[0], n[1], n[2]
				}

				pb.BBox.Add(vtx.X, vtx.Y, vtx.Z)
				vl.Vertices = append(vl.Vertices, vtx)
			}
			pb.VertexLists = append(pb.VertexLists, vl)
		}
	}

	return pb
}

// ToPrimBufferStrips converts the imported mesh into a PrimBuffer using
// degenerate triangle strips — one strip per material group. This is more
// efficient than individual triangles (fewer face headers).
func (m *ImportedMesh) ToPrimBufferStrips() *PrimBuffer {
	pb := &PrimBuffer{
		BBox: NewBox(),
	}

	for gi, group := range m.Groups {
		if len(group.Faces) == 0 {
			continue
		}

		vl := VertexList{
			Material: gi,
			Layer:    0,
			Layers:   1,
		}

		for fi, face := range group.Faces {
			verts := [3]Vertex{}
			for j, vi := range face {
				verts[j].R, verts[j].G, verts[j].B, verts[j].A = 1, 1, 1, 1
				if vi[0] > 0 && vi[0] <= len(m.Positions) {
					p := m.Positions[vi[0]-1]
					verts[j].X, verts[j].Y, verts[j].Z = p[0], p[1], p[2]
					if vi[0]-1 < len(m.Colors) {
						c := m.Colors[vi[0]-1]
						verts[j].R, verts[j].G, verts[j].B = c[0], c[1], c[2]
					}
				}
				if vi[1] > 0 && vi[1] <= len(m.UVs) {
					uv := m.UVs[vi[1]-1]
					verts[j].U = uv[0]
					verts[j].V = 1 - uv[1]
				}
				if vi[2] > 0 && vi[2] <= len(m.Normals) {
					n := m.Normals[vi[2]-1]
					verts[j].NX, verts[j].NY, verts[j].NZ = n[0], n[1], n[2]
				}
				pb.BBox.Add(verts[j].X, verts[j].Y, verts[j].Z)
			}

			if fi > 0 {
				// Insert degenerate connector: repeat last vertex of previous
				// triangle, then first vertex of this triangle
				last := vl.Vertices[len(vl.Vertices)-1]
				vl.Vertices = append(vl.Vertices, last)
				vl.Vertices = append(vl.Vertices, verts[0])
				// If strip length is now odd, we need an extra degenerate to
				// fix winding for the next real triangle
				if len(vl.Vertices)%2 == 1 {
					vl.Vertices = append(vl.Vertices, verts[0])
				}
			}

			vl.Vertices = append(vl.Vertices, verts[0], verts[1], verts[2])
		}

		pb.VertexLists = append(pb.VertexLists, vl)
	}

	return pb
}

// Stats returns a summary string of the imported mesh.
func (m *ImportedMesh) Stats() string {
	totalFaces := 0
	for _, g := range m.Groups {
		totalFaces += len(g.Faces)
	}
	return fmt.Sprintf("%d vertices, %d UVs, %d normals, %d groups, %d triangles",
		len(m.Positions), len(m.UVs), len(m.Normals), len(m.Groups), totalFaces)
}
