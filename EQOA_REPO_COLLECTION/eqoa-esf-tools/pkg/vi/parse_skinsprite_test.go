package vi

import (
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func TestParseSkinSprite_TUNARIA(t *testing.T) {
	file, err := esf.Open("/home/sdg/Documents/eqoa/TUNARIA.ESF")
	if err != nil { t.Skipf("ESF not found: %v", err) }
	root, err := file.Root()
	if err != nil { t.Fatalf("Root: %v", err) }
	rawData, err := os.ReadFile("/home/sdg/Documents/eqoa/TUNARIA.ESF")
	if err != nil { t.Fatalf("ReadFile: %v", err) }

	var nodes []*esf.ObjInfo
	var find func(n *esf.ObjInfo)
	find = func(n *esf.ObjInfo) {
		if n.Type == TypeSkinSprite { nodes = append(nodes, n) }
		for _, c := range n.Children { find(c) }
	}
	find(root)

	tested := 0
	for _, node := range nodes {
		if tested >= 10 { break }
		children := make(map[uint16][]byte)
		for _, child := range node.Children {
			bodyEnd := child.Offset + int(child.Size) - 4
			if child.Offset >= 0 && bodyEnd > child.Offset && bodyEnd <= len(rawData) {
				children[child.Type] = rawData[child.Offset:bodyEnd]
			}
		}

		ss, err := ParseSkinSprite(children)
		if err != nil { continue }

		t.Logf("[%d] dictID=0x%08X bbox=(%.1f,%.1f,%.1f)-(%.1f,%.1f,%.1f) lodNear=%.1f lodFar=%.1f",
			tested, ss.DictID,
			ss.BBox.Min.X, ss.BBox.Min.Y, ss.BBox.Min.Z,
			ss.BBox.Max.X, ss.BBox.Max.Y, ss.BBox.Max.Z,
			ss.LODNear, ss.LODFar)
		tested++
	}
	t.Logf("Total SkinSprites: %d, tested: %d", len(nodes), tested)
}

func TestParseStaticLighting_TUNARIA(t *testing.T) {
	file, err := esf.Open("/home/sdg/Documents/eqoa/TUNARIA.ESF")
	if err != nil { t.Skipf("ESF not found: %v", err) }
	root, err := file.Root()
	if err != nil { t.Fatalf("Root: %v", err) }
	rawData, err := os.ReadFile("/home/sdg/Documents/eqoa/TUNARIA.ESF")
	if err != nil { t.Fatalf("ReadFile: %v", err) }

	var nodes []*esf.ObjInfo
	var find func(n *esf.ObjInfo)
	find = func(n *esf.ObjInfo) {
		if n.Type == TypeStaticLighting { nodes = append(nodes, n) }
		for _, c := range n.Children { find(c) }
	}
	find(root)

	tested := 0
	for _, node := range nodes {
		if tested >= 10 { break }
		// Get the header body (from StaticLighting node itself, not children)
		bodyEnd := node.Offset + int(node.Size) - 4
		var headerData []byte
		if node.Offset >= 0 && bodyEnd > node.Offset && bodyEnd <= len(rawData) {
			headerData = rawData[node.Offset:bodyEnd]
		}

		children := make(map[uint16][]byte)
		for _, child := range node.Children {
			cEnd := child.Offset + int(child.Size) - 4
			if child.Offset >= 0 && cEnd > child.Offset && cEnd <= len(rawData) {
				children[child.Type] = rawData[child.Offset:cEnd]
			}
		}

		sl := ParseStaticLighting(headerData, children)
		if sl == nil { continue }

		t.Logf("[%d] numLights=%d flags=%d lightDataSize=%d",
			tested, sl.NumLights, sl.Flags, len(sl.LightData))
		tested++
	}
	t.Logf("Total StaticLightings: %d, tested: %d", len(nodes), tested)
}
