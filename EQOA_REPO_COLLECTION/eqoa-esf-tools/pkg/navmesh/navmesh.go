// Package navmesh provides A* pathfinding on walkable triangle meshes.
// Shared between go-eqoa-client (combat auto-approach) and go-eqoa-server (NPC AI).
package navmesh

import (
	"container/heap"
	"math"
)

// NavMesh is a navigation mesh built from walkable triangles.
// Triangles sharing edges form an adjacency graph for A* pathfinding.
type NavMesh struct {
	Tris       []NavTri   // all walkable triangles
	Edges      [][]NavEdge // adjacency list: Edges[triIdx] = neighbors
	Stitched   int        // number of proximity-stitched boundary edge connections
	Components int        // number of connected components
	LargestCC  int        // size of largest connected component
	compID     []int      // component ID per triangle (for same-component checks)
}

// NavTri stores a walkable triangle with precomputed centroid.
type NavTri struct {
	X0, Y0, Z0 float32
	X1, Y1, Z1 float32
	X2, Y2, Z2 float32
	CX, CY, CZ float32 // centroid
}

// NavEdge connects two adjacent triangles through a shared portal edge.
type NavEdge struct {
	To   int     // neighbor triangle index
	Cost float32 // distance between centroids
	// Portal edge endpoints (for funnel algorithm)
	PX0, PZ0 float32 // left portal vertex
	PX1, PZ1 float32 // right portal vertex
}

// Waypoint is a 2D point on a pathfound route.
type Waypoint struct {
	X, Z float32
}

// InputTriangle is the input format for BuildNavMesh.
// Callers convert their own triangle types to this.
type InputTriangle struct {
	X0, Y0, Z0 float32
	X1, Y1, Z1 float32
	X2, Y2, Z2 float32
}

// vertKey quantizes a vertex position for shared-edge detection.
type vertKey struct {
	x, y, z int32 // quantized to 0.01 units
}

func quantize(v float32) int32 {
	return int32(math.Round(float64(v) * 100))
}

func makeVertKey(x, y, z float32) vertKey {
	return vertKey{quantize(x), quantize(y), quantize(z)}
}

// edgeKey is an unordered pair of vertex keys (for shared edge lookup).
type edgeKey struct {
	a, b vertKey
}

func makeEdgeKey(a, b vertKey) edgeKey {
	if a.x < b.x || (a.x == b.x && a.y < b.y) || (a.x == b.x && a.y == b.y && a.z < b.z) {
		return edgeKey{a, b}
	}
	return edgeKey{b, a}
}

// Build constructs a NavMesh from a slice of walkable triangles.
func Build(triangles []InputTriangle) *NavMesh {
	if len(triangles) == 0 {
		return nil
	}

	// Phase 1: Convert to NavTri with centroids
	tris := make([]NavTri, len(triangles))
	for i, t := range triangles {
		tris[i] = NavTri{
			X0: t.X0, Y0: t.Y0, Z0: t.Z0,
			X1: t.X1, Y1: t.Y1, Z1: t.Z1,
			X2: t.X2, Y2: t.Y2, Z2: t.Z2,
			CX: (t.X0 + t.X1 + t.X2) / 3,
			CY: (t.Y0 + t.Y1 + t.Y2) / 3,
			CZ: (t.Z0 + t.Z1 + t.Z2) / 3,
		}
	}

	// Phase 2: Build adjacency via shared edges
	edgeMap := make(map[edgeKey][]int, len(tris)*3)
	for i, t := range tris {
		v0 := makeVertKey(t.X0, t.Y0, t.Z0)
		v1 := makeVertKey(t.X1, t.Y1, t.Z1)
		v2 := makeVertKey(t.X2, t.Y2, t.Z2)

		edgeMap[makeEdgeKey(v0, v1)] = append(edgeMap[makeEdgeKey(v0, v1)], i)
		edgeMap[makeEdgeKey(v1, v2)] = append(edgeMap[makeEdgeKey(v1, v2)], i)
		edgeMap[makeEdgeKey(v2, v0)] = append(edgeMap[makeEdgeKey(v2, v0)], i)
	}

	// Phase 3: Build adjacency lists from shared edges
	edges := make([][]NavEdge, len(tris))

	for ek, triList := range edgeMap {
		if len(triList) != 2 {
			continue
		}
		a, b := triList[0], triList[1]
		ta, tb := &tris[a], &tris[b]
		dx := ta.CX - tb.CX
		dz := ta.CZ - tb.CZ
		cost := float32(math.Sqrt(float64(dx*dx + dz*dz)))

		px0, pz0 := float32(ek.a.x)/100, float32(ek.a.z)/100
		px1, pz1 := float32(ek.b.x)/100, float32(ek.b.z)/100

		edges[a] = append(edges[a], NavEdge{To: b, Cost: cost, PX0: px0, PZ0: pz0, PX1: px1, PZ1: pz1})
		edges[b] = append(edges[b], NavEdge{To: a, Cost: cost, PX0: px0, PZ0: pz0, PX1: px1, PZ1: pz1})
	}

	// Phase 4: Proximity-based edge stitching.
	// Collision meshes from different zone actors don't share exact vertices.
	// Find boundary edges (1 triangle only) that are close to other boundary
	// edges and connect them. This bridges terrain↔building, building↔stairs, etc.
	type boundaryEdge struct {
		tri           int     // triangle index
		mx, my, mz    float32 // edge midpoint
		px0, pz0      float32 // endpoint 0
		px1, pz1      float32 // endpoint 1
	}
	var boundaries []boundaryEdge
	for ek, triList := range edgeMap {
		if len(triList) != 1 {
			continue
		}
		tri := triList[0]
		t := &tris[tri]
		// Resolve edge midpoint from quantized key
		ex0, ey0, ez0 := float32(ek.a.x)/100, float32(ek.a.y)/100, float32(ek.a.z)/100
		ex1, ey1, ez1 := float32(ek.b.x)/100, float32(ek.b.y)/100, float32(ek.b.z)/100
		_ = t
		boundaries = append(boundaries, boundaryEdge{
			tri: tri,
			mx:  (ex0 + ex1) / 2, my: (ey0 + ey1) / 2, mz: (ez0 + ez1) / 2,
			px0: ex0, pz0: ez0, px1: ex1, pz1: ez1,
		})
	}

	// For each boundary edge, find the closest boundary edge from a DIFFERENT triangle
	// within stitchDist and connect them.
	const stitchDist = float32(2.0) // max distance to stitch boundary edges
	stitchDistSq := stitchDist * stitchDist
	stitched := 0

	// Build spatial buckets for boundary edges to avoid O(n²)
	type bucketKey struct{ bx, bz int }
	const bucketSize = 4.0
	buckets := make(map[bucketKey][]int) // bucket → indices into boundaries
	for i, be := range boundaries {
		bk := bucketKey{int(be.mx / bucketSize), int(be.mz / bucketSize)}
		buckets[bk] = append(buckets[bk], i)
	}

	for i, be := range boundaries {
		bk := bucketKey{int(be.mx / bucketSize), int(be.mz / bucketSize)}
		bestJ := -1
		bestDist := stitchDistSq

		// Search 3×3 neighborhood
		for dbi := -1; dbi <= 1; dbi++ {
			for dbj := -1; dbj <= 1; dbj++ {
				nk := bucketKey{bk.bx + dbi, bk.bz + dbj}
				for _, j := range buckets[nk] {
					if j <= i {
						continue // avoid duplicates
					}
					bo := boundaries[j]
					if bo.tri == be.tri {
						continue
					}
					// Already connected?
					alreadyConnected := false
					for _, e := range edges[be.tri] {
						if e.To == bo.tri {
							alreadyConnected = true
							break
						}
					}
					if alreadyConnected {
						continue
					}
					// 3D distance between edge midpoints
					dx := be.mx - bo.mx
					dy := be.my - bo.my
					dz := be.mz - bo.mz
					d := dx*dx + dy*dy + dz*dz
					if d < bestDist {
						bestDist = d
						bestJ = j
					}
				}
			}
		}

		if bestJ >= 0 {
			bo := boundaries[bestJ]
			ta, tb := &tris[be.tri], &tris[bo.tri]
			dx := ta.CX - tb.CX
			dy := ta.CY - tb.CY
			dz := ta.CZ - tb.CZ
			cost := float32(math.Sqrt(float64(dx*dx + dy*dy + dz*dz)))

			// Portal = average of both edge midpoints
			pmx := (be.mx + bo.mx) / 2
			pmz := (be.mz + bo.mz) / 2

			edges[be.tri] = append(edges[be.tri], NavEdge{To: bo.tri, Cost: cost, PX0: pmx, PZ0: pmz, PX1: pmx, PZ1: pmz})
			edges[bo.tri] = append(edges[bo.tri], NavEdge{To: be.tri, Cost: cost, PX0: pmx, PZ0: pmz, PX1: pmx, PZ1: pmz})
			stitched++
		}
	}

	// Phase 5: Connected component analysis + cross-component stitching.
	// Find connected components via BFS, then stitch the largest components
	// by finding the closest centroid pairs between different components.
	compID := make([]int, len(tris))
	for i := range compID {
		compID[i] = -1
	}
	numComponents := 0
	compSizes := make(map[int]int)

	for start := range tris {
		if compID[start] >= 0 {
			continue
		}
		// BFS from this triangle
		cid := numComponents
		numComponents++
		queue := []int{start}
		compID[start] = cid
		size := 0
		for len(queue) > 0 {
			cur := queue[0]
			queue = queue[1:]
			size++
			for _, e := range edges[cur] {
				if compID[e.To] < 0 {
					compID[e.To] = cid
					queue = append(queue, e.To)
				}
			}
		}
		compSizes[cid] = size
	}

	// Find largest component
	largestCC := 0
	for _, sz := range compSizes {
		if sz > largestCC {
			largestCC = sz
		}
	}

	// Cross-component centroid stitching: for each small component, find the
	// closest triangle in a different component and connect them.
	// This handles cases where boundary edge stitching missed gaps > 2 units.
	const crossStitchDist = float32(8.0)
	crossStitchSq := crossStitchDist * crossStitchDist

	// Build centroid spatial buckets
	type cbKey struct{ bx, bz int }
	const cbSize = 16.0
	cBuckets := make(map[cbKey][]int)
	for i, t := range tris {
		bk := cbKey{int(t.CX / cbSize), int(t.CZ / cbSize)}
		cBuckets[bk] = append(cBuckets[bk], i)
	}

	crossStitched := 0
	for i, t := range tris {
		bk := cbKey{int(t.CX / cbSize), int(t.CZ / cbSize)}
		bestJ := -1
		bestDist := crossStitchSq

		for dbi := -1; dbi <= 1; dbi++ {
			for dbj := -1; dbj <= 1; dbj++ {
				nk := cbKey{bk.bx + dbi, bk.bz + dbj}
				for _, j := range cBuckets[nk] {
					if j <= i || compID[j] == compID[i] {
						continue
					}
					o := &tris[j]
					dx := t.CX - o.CX
					dy := t.CY - o.CY
					dz := t.CZ - o.CZ
					d := dx*dx + dy*dy + dz*dz
					if d < bestDist {
						bestDist = d
						bestJ = j
					}
				}
			}
		}

		if bestJ >= 0 {
			o := &tris[bestJ]
			dx := t.CX - o.CX
			dy := t.CY - o.CY
			dz := t.CZ - o.CZ
			cost := float32(math.Sqrt(float64(dx*dx + dy*dy + dz*dz)))
			pmx := (t.CX + o.CX) / 2
			pmz := (t.CZ + o.CZ) / 2

			edges[i] = append(edges[i], NavEdge{To: bestJ, Cost: cost, PX0: pmx, PZ0: pmz, PX1: pmx, PZ1: pmz})
			edges[bestJ] = append(edges[bestJ], NavEdge{To: i, Cost: cost, PX0: pmx, PZ0: pmz, PX1: pmx, PZ1: pmz})

			// Merge components
			oldComp := compID[bestJ]
			newComp := compID[i]
			if oldComp != newComp {
				for k := range compID {
					if compID[k] == oldComp {
						compID[k] = newComp
					}
				}
			}
			crossStitched++
		}
	}

	// Recount components after cross-stitching
	seen := make(map[int]bool)
	for _, c := range compID {
		seen[c] = true
	}

	return &NavMesh{
		Tris:       tris,
		Edges:      edges,
		Stitched:   stitched + crossStitched,
		Components: len(seen),
		LargestCC:  largestCC,
		compID:     compID,
	}
}

// TriCount returns the number of triangles in the navmesh.
func (nm *NavMesh) TriCount() int {
	if nm == nil {
		return 0
	}
	return len(nm.Tris)
}

// EdgeCount returns the number of edges (connections) in the navmesh.
func (nm *NavMesh) EdgeCount() int {
	if nm == nil {
		return 0
	}
	n := 0
	for _, el := range nm.Edges {
		n += len(el)
	}
	return n / 2
}

// findTri returns the index of the triangle containing point (x,y,z), or -1.
// When multiple triangles overlap in XZ (multi-level buildings), picks the one
// closest in Y to the query point.
func (nm *NavMesh) findTri(x, y, z float32) int {
	// Collect all XZ-containing triangles, pick closest in Y
	bestIdx := -1
	bestYDist := float32(math.MaxFloat32)
	for i, t := range nm.Tris {
		if pointInTri2D(x, z, t.X0, t.Z0, t.X1, t.Z1, t.X2, t.Z2) {
			yDist := float32(math.Abs(float64(y - t.CY)))
			if yDist < bestYDist {
				bestYDist = yDist
				bestIdx = i
			}
		}
	}
	if bestIdx >= 0 {
		return bestIdx
	}
	// Fallback: nearest centroid within 32 units (3D distance)
	bestDist := float32(math.MaxFloat32)
	for i, t := range nm.Tris {
		dx := x - t.CX
		dy := y - t.CY
		dz := z - t.CZ
		d := dx*dx + dy*dy + dz*dz
		if d < bestDist {
			bestDist = d
			bestIdx = i
		}
	}
	if bestDist < 32*32 {
		return bestIdx
	}
	return -1
}

func pointInTri2D(px, pz, ax, az, bx, bz, cx, cz float32) bool {
	d1 := sign2D(px, pz, ax, az, bx, bz)
	d2 := sign2D(px, pz, bx, bz, cx, cz)
	d3 := sign2D(px, pz, cx, cz, ax, az)
	hasNeg := d1 < 0 || d2 < 0 || d3 < 0
	hasPos := d1 > 0 || d2 > 0 || d3 > 0
	return !(hasNeg && hasPos)
}

func sign2D(px, pz, ax, az, bx, bz float32) float32 {
	return (px-bx)*(az-bz) - (ax-bx)*(pz-bz)
}

// FindPath returns a list of waypoints from (sx,sy,sz) to (gx,gy,gz) using A*.
// Y coordinates are used to disambiguate multi-level geometry (buildings with floors).
// Returns nil if no path exists or start/goal are off-mesh.
func (nm *NavMesh) FindPath(sx, sy, sz, gx, gy, gz float32) []Waypoint {
	if nm == nil || len(nm.Tris) == 0 {
		return nil
	}

	startTri := nm.findTri(sx, sy, sz)
	goalTri := nm.findTri(gx, gy, gz)
	if startTri < 0 || goalTri < 0 {
		return nil
	}
	if startTri == goalTri {
		return []Waypoint{{gx, gz}}
	}

	// A* search
	n := len(nm.Tris)
	gScore := make([]float32, n)
	parent := make([]int, n)
	parentEdge := make([]int, n)
	closed := make([]bool, n)
	for i := range gScore {
		gScore[i] = math.MaxFloat32
		parent[i] = -1
	}
	gScore[startTri] = 0

	goalCX := nm.Tris[goalTri].CX
	goalCZ := nm.Tris[goalTri].CZ

	heuristic := func(triIdx int) float32 {
		t := &nm.Tris[triIdx]
		dx := t.CX - goalCX
		dz := t.CZ - goalCZ
		return float32(math.Sqrt(float64(dx*dx + dz*dz)))
	}

	pq := &astarHeap{}
	heap.Init(pq)
	heap.Push(pq, astarNode{tri: startTri, f: heuristic(startTri)})

	found := false
	for pq.Len() > 0 {
		cur := heap.Pop(pq).(astarNode)
		if cur.tri == goalTri {
			found = true
			break
		}
		if closed[cur.tri] {
			continue
		}
		closed[cur.tri] = true

		for ei, edge := range nm.Edges[cur.tri] {
			if closed[edge.To] {
				continue
			}
			newG := gScore[cur.tri] + edge.Cost
			if newG < gScore[edge.To] {
				gScore[edge.To] = newG
				parent[edge.To] = cur.tri
				parentEdge[edge.To] = ei
				f := newG + heuristic(edge.To)
				heap.Push(pq, astarNode{tri: edge.To, f: f})
			}
		}
	}

	if !found {
		return nil
	}

	// Reconstruct portal sequence
	var portals []portal
	cur := goalTri
	for cur != startTri {
		p := parent[cur]
		ei := parentEdge[cur]
		e := nm.Edges[p][ei]
		portals = append(portals, portal{
			lx: e.PX0, lz: e.PZ0,
			rx: e.PX1, rz: e.PZ1,
		})
		cur = p
	}
	for i, j := 0, len(portals)-1; i < j; i, j = i+1, j-1 {
		portals[i], portals[j] = portals[j], portals[i]
	}

	return funnelSmooth(sx, sz, gx, gz, portals)
}

type portal struct {
	lx, lz float32
	rx, rz float32
}

func funnelSmooth(sx, sz, gx, gz float32, portals []portal) []Waypoint {
	if len(portals) == 0 {
		return []Waypoint{{gx, gz}}
	}

	// Orient portals consistently (left = left of path direction)
	for i := range portals {
		p := &portals[i]
		var refX, refZ float32
		if i == 0 {
			refX, refZ = sx, sz
		} else {
			refX = (portals[i-1].lx + portals[i-1].rx) / 2
			refZ = (portals[i-1].lz + portals[i-1].rz) / 2
		}
		dx := (p.lx+p.rx)/2 - refX
		dz := (p.lz+p.rz)/2 - refZ
		cross := dx*(p.lz-p.rz) - dz*(p.lx-p.rx)
		if cross < 0 {
			p.lx, p.lz, p.rx, p.rz = p.rx, p.rz, p.lx, p.lz
		}
	}

	portals = append(portals, portal{lx: gx, lz: gz, rx: gx, rz: gz})

	var path []Waypoint
	apexX, apexZ := sx, sz
	leftX, leftZ := sx, sz
	rightX, rightZ := sx, sz
	apexIdx, leftIdx, rightIdx := 0, 0, 0

	for i := 1; i < len(portals); i++ {
		pl := portals[i]

		if cross2D(apexX, apexZ, rightX, rightZ, pl.rx, pl.rz) <= 0 {
			if (apexX == rightX && apexZ == rightZ) || cross2D(apexX, apexZ, leftX, leftZ, pl.rx, pl.rz) > 0 {
				rightX, rightZ = pl.rx, pl.rz
				rightIdx = i
			} else {
				path = append(path, Waypoint{leftX, leftZ})
				apexX, apexZ = leftX, leftZ
				apexIdx = leftIdx
				leftX, leftZ = apexX, apexZ
				rightX, rightZ = apexX, apexZ
				leftIdx, rightIdx = apexIdx, apexIdx
				i = apexIdx
				continue
			}
		}

		if cross2D(apexX, apexZ, leftX, leftZ, pl.lx, pl.lz) >= 0 {
			if (apexX == leftX && apexZ == leftZ) || cross2D(apexX, apexZ, rightX, rightZ, pl.lx, pl.lz) < 0 {
				leftX, leftZ = pl.lx, pl.lz
				leftIdx = i
			} else {
				path = append(path, Waypoint{rightX, rightZ})
				apexX, apexZ = rightX, rightZ
				apexIdx = rightIdx
				leftX, leftZ = apexX, apexZ
				rightX, rightZ = apexX, apexZ
				leftIdx, rightIdx = apexIdx, apexIdx
				i = apexIdx
				continue
			}
		}
	}

	path = append(path, Waypoint{gx, gz})
	return path
}

func cross2D(ox, oz, ax, az, bx, bz float32) float32 {
	return (ax-ox)*(bz-oz) - (az-oz)*(bx-ox)
}

// --- A* priority queue ---

type astarNode struct {
	tri int
	f   float32
}

type astarHeap []astarNode

func (h astarHeap) Len() int            { return len(h) }
func (h astarHeap) Less(i, j int) bool  { return h[i].f < h[j].f }
func (h astarHeap) Swap(i, j int)       { h[i], h[j] = h[j], h[i] }
func (h *astarHeap) Push(x interface{}) { *h = append(*h, x.(astarNode)) }
func (h *astarHeap) Pop() interface{} {
	old := *h
	n := len(old)
	item := old[n-1]
	*h = old[:n-1]
	return item
}
