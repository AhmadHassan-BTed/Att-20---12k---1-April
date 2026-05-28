package vi

import (
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func TestParseZoneActor_TUNARIA(t *testing.T) {
	file, err := esf.Open("/home/sdg/Documents/eqoa/TUNARIA.ESF")
	if err != nil {
		t.Skipf("ESF not found: %v", err)
	}

	root, err := file.Root()
	if err != nil {
		t.Fatalf("Root: %v", err)
	}

	rawData, err := os.ReadFile("/home/sdg/Documents/eqoa/TUNARIA.ESF")
	if err != nil {
		t.Fatalf("ReadFile: %v", err)
	}

	var actors []*esf.ObjInfo
	var findActors func(n *esf.ObjInfo)
	findActors = func(n *esf.ObjInfo) {
		if n.Type == TypeZoneActor {
			actors = append(actors, n)
		}
		for _, c := range n.Children {
			findActors(c)
		}
	}
	findActors(root)

	if len(actors) == 0 {
		t.Fatal("No ZoneActors found")
	}

	tested := 0
	for _, node := range actors {
		if tested >= 20 {
			break
		}

		bodyEnd := node.Offset + int(node.Size) - 4
		if node.Offset < 0 || bodyEnd <= node.Offset || bodyEnd > len(rawData) {
			continue
		}
		body := rawData[node.Offset:bodyEnd]

		actor := ParseZoneActor(body)
		if actor == nil {
			continue
		}

		// Sanity checks
		if actor.Scale <= 0 || actor.Scale > 100 {
			// Scale 0 or negative is suspicious but not necessarily wrong
		}

		t.Logf("[%d] dictID=0x%08X pos=(%.1f, %.1f, %.1f) rot=(%.2f, %.2f, %.2f) scale=%.2f color=(%d,%d,%d,%d)",
			tested, uint32(actor.DictID),
			actor.PosX, actor.PosY, actor.PosZ,
			actor.RotX, actor.RotY, actor.RotZ,
			actor.Scale,
			actor.R, actor.G, actor.B, actor.A)
		tested++
	}

	t.Logf("Total ZoneActors in TUNARIA: %d, tested: %d", len(actors), tested)
}
