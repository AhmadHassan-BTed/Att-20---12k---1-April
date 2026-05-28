package vi

import (
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func TestParseZoneBase_TUNARIA(t *testing.T) {
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

	// Find all zones and test PreTranslations + Actor count
	tested := 0
	var findZones func(n *esf.ObjInfo)
	findZones = func(n *esf.ObjInfo) {
		if n.Type == TypeZoneBase && tested < 5 {
			// Find PreTranslations child (inside 0x3200)
			for _, c := range n.Children {
				if c.Type != TypeZoneRoom {
					continue
				}
				// Count rooms and actors
				roomCount := 0
				actorCount := 0
				var preTransData []byte
				for _, rc := range c.Children {
					if rc.Type == TypePreTranslations {
						bodyEnd := rc.Offset + int(rc.Size) - 4
						if rc.Offset >= 0 && bodyEnd > rc.Offset && bodyEnd <= len(rawData) {
							preTransData = rawData[rc.Offset:bodyEnd]
						}
					}
					if rc.Type == TypeZoneRooms {
						roomCount = len(rc.Children)
						// Count actors in all rooms
						for _, room := range rc.Children {
							actorCount += len(room.Children)
						}
					}
				}

				pts := ParsePreTranslations(preTransData)

				t.Logf("[%d] ver=%d preTrans=%d rooms=%d actors=%d",
					tested, n.Version, len(pts), roomCount, actorCount)

				if len(pts) > 0 {
					t.Logf("  preTrans[0]=(%.1f, %.1f, %.1f)", pts[0].X, pts[0].Y, pts[0].Z)
				}
			}
			tested++
		}
		for _, c := range n.Children {
			findZones(c)
		}
	}
	findZones(root)

	if tested == 0 {
		t.Fatal("No ZoneBases tested")
	}
}
