package vi

import (
	"os"
	"testing"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

func TestParseSound_CHAR(t *testing.T) {
	file, err := esf.Open("/tmp/CHAR.ESF")
	if err != nil {
		t.Skipf("CHAR.ESF not found: %v", err)
	}

	root, err := file.Root()
	if err != nil {
		t.Fatalf("Root: %v", err)
	}

	rawData, err := os.ReadFile("/tmp/CHAR.ESF")
	if err != nil {
		t.Fatalf("ReadFile: %v", err)
	}

	var nodes []*esf.ObjInfo
	var find func(n *esf.ObjInfo)
	find = func(n *esf.ObjInfo) {
		if n.Type == TypeSound {
			nodes = append(nodes, n)
		}
		for _, c := range n.Children {
			find(c)
		}
	}
	find(root)

	if len(nodes) == 0 {
		t.Skip("No Sound objects found")
	}

	tested := 0
	for _, node := range nodes {
		if tested >= 10 {
			break
		}

		children := make(map[uint16][]byte)
		for _, child := range node.Children {
			bodyEnd := child.Offset + int(child.Size) - 4
			if child.Offset >= 0 && bodyEnd > child.Offset && bodyEnd <= len(rawData) {
				children[child.Type] = rawData[child.Offset:bodyEnd]
			}
		}

		snd := ParseSound(children)
		if snd == nil || snd.DictID == 0 {
			continue
		}

		t.Logf("[%d] dictID=0x%08X type=%d sampleRate=%d channels=%d samples=%d vol=%.2f minDist=%.1f",
			tested, snd.DictID, snd.Type, snd.SampleRate, snd.NumChannels, snd.NumSamples,
			snd.Volume, snd.MinDistance)
		tested++
	}

	t.Logf("Total Sounds in CHAR.ESF: %d, tested: %d", len(nodes), tested)
}
