package esf

import (
	"encoding/binary"
	"math"
)

// AnimPackFrame holds a single packed keyframe for one bone track.
// Quaternion components are normalized such that |q| ≈ 32767.
// Scale is encoded as int16 (512 ≈ 1.0).
// Position components are int16 offsets.
type AnimPackFrame struct {
	Quat  [4]int16 // quaternion (x, y, z, w)
	Scale int16    // scale factor (512 = 1.0)
	Pos   [3]int16 // position (x, y, z)
}

// AnimNode represents one animation track (one bone's keyframes).
type AnimNode struct {
	RefID  int32          // bone reference ID (hash, looked up in RefMap)
	Frames []AnimPackFrame // keyframes for this bone
}

// AnimKeyframe holds extra keyframe timing data.
// PS2: SetKeyframe__13VIHSpriteAnimiif — (keyframeIndex, refID, time).
type AnimKeyframe struct {
	RefID int32   // tag reference ID (hash)
	Time  float32 // keyframe trigger time in seconds
}

// HSpriteAnim contains parsed animation data from ESF type 0x2600.
// PS2: ParseHSpriteAnimObj (0x00436560).
//
// PS2 version-dependent header layout (ParseHSpriteAnimObj 0x00436560):
//
//	v0: dictID, numNodes, numFrames, fps                                        (16 bytes)
//	v1: dictID, numNodes, numFrames, fps, playSpeed, playbackType               (24 bytes)
//	v2: dictID, format, numNodes, numFrames, fps, playSpeed, playbackType       (28 bytes)
//	v3: dictID, format, numNodes, numFrames, numKeyframes, fps, playSpeed, playbackType (32 bytes)
//
// Defaults when field absent: format=0, playSpeed=1.0, playbackType=0, numKeyframes=0.
// 17433/17434 animations in EQOA Frontiers are v3; 1 is v2 (SPELLFX.ESF).
//
//	Per node (numNodes times):
//	  int32 refID
//	  numFrames × 16 bytes (VIHSpritePackFrame: 8 × int16)
//
//	Keyframes (numKeyframes times, v3 only):
//	  int32 refID, float32 time (seconds)
type HSpriteAnim struct {
	info *ObjInfo

	DictID       int32
	Format       int32 // 1 = packed (VIHSpritePackFrame)
	NumNodes     int32
	NumFrames    int32
	FPS          float32
	PlaySpeed    float32
	PlaybackType int32 // 0=loop, 1=once
	Keyframes    []AnimKeyframe
	Nodes        []AnimNode
}

func (a *HSpriteAnim) ObjInfo() *ObjInfo { return a.info }

func (a *HSpriteAnim) Load(file *ObjFile) error {
	raw := file.RawBytes(a.info.Offset, int(a.info.Size))
	ver := a.info.Version

	// Version-dependent header (PS2: ParseHSpriteAnimObj 0x00436560)
	off := 0
	a.DictID = int32(binary.LittleEndian.Uint32(raw[off:])); off += 4
	if ver >= 2 {
		a.Format = int32(binary.LittleEndian.Uint32(raw[off:])); off += 4
	}
	a.NumNodes = int32(binary.LittleEndian.Uint32(raw[off:])); off += 4
	a.NumFrames = int32(binary.LittleEndian.Uint32(raw[off:])); off += 4
	var numKF int32
	if ver >= 3 {
		numKF = int32(binary.LittleEndian.Uint32(raw[off:])); off += 4
	}
	a.FPS = math.Float32frombits(binary.LittleEndian.Uint32(raw[off:])); off += 4
	if ver >= 1 {
		a.PlaySpeed = math.Float32frombits(binary.LittleEndian.Uint32(raw[off:])); off += 4
		a.PlaybackType = int32(binary.LittleEndian.Uint32(raw[off:])); off += 4
	} else {
		a.PlaySpeed = 1.0
	}

	// Read per-node data: refID + frames.
	// PS2 ParseHSpriteAnimObj dispatches on Format:
	//   format=0: 8 × float32 per frame (32 bytes) — converted to int16 via ×32767 (quat) / ×512 (pos/scale)
	//   format=1: 8 × int16 per frame (16 bytes) — stored directly as VIHSpritePackFrame
	// 17433/17434 EQOA animations are v3 format=1. 1 animation (SPELLFX.ESF v2) uses format=0.
	if a.NumNodes > 0 && a.NumFrames > 0 {
		a.Nodes = make([]AnimNode, a.NumNodes)
		for n := int32(0); n < a.NumNodes; n++ {
			a.Nodes[n].RefID = int32(binary.LittleEndian.Uint32(raw[off:]))
			off += 4

			a.Nodes[n].Frames = make([]AnimPackFrame, a.NumFrames)
			for f := int32(0); f < a.NumFrames; f++ {
				pf := &a.Nodes[n].Frames[f]
				if a.Format == 0 {
					// Format 0: 8 × float32 per frame (32 bytes).
					// PS2 reads as floats, then converts to packed int16:
					//   quat components × 32767 (1/0x38000100 inverse)
					//   scale/pos components × 512 (1/0x3B000000 inverse)
					q0 := math.Float32frombits(binary.LittleEndian.Uint32(raw[off+0:]))
					q1 := math.Float32frombits(binary.LittleEndian.Uint32(raw[off+4:]))
					q2 := math.Float32frombits(binary.LittleEndian.Uint32(raw[off+8:]))
					q3 := math.Float32frombits(binary.LittleEndian.Uint32(raw[off+12:]))
					sc := math.Float32frombits(binary.LittleEndian.Uint32(raw[off+16:]))
					px := math.Float32frombits(binary.LittleEndian.Uint32(raw[off+20:]))
					py := math.Float32frombits(binary.LittleEndian.Uint32(raw[off+24:]))
					pz := math.Float32frombits(binary.LittleEndian.Uint32(raw[off+28:]))
					pf.Quat[0] = int16(q0 * 32767)
					pf.Quat[1] = int16(q1 * 32767)
					pf.Quat[2] = int16(q2 * 32767)
					pf.Quat[3] = int16(q3 * 32767)
					pf.Scale = int16(sc * 512)
					pf.Pos[0] = int16(px * 512)
					pf.Pos[1] = int16(py * 512)
					pf.Pos[2] = int16(pz * 512)
					off += 32
				} else {
					// Format 1: 8 × int16 per frame (16 bytes) — direct packed storage.
					pf.Quat[0] = int16(binary.LittleEndian.Uint16(raw[off+0:]))
					pf.Quat[1] = int16(binary.LittleEndian.Uint16(raw[off+2:]))
					pf.Quat[2] = int16(binary.LittleEndian.Uint16(raw[off+4:]))
					pf.Quat[3] = int16(binary.LittleEndian.Uint16(raw[off+6:]))
					pf.Scale = int16(binary.LittleEndian.Uint16(raw[off+8:]))
					pf.Pos[0] = int16(binary.LittleEndian.Uint16(raw[off+10:]))
					pf.Pos[1] = int16(binary.LittleEndian.Uint16(raw[off+12:]))
					pf.Pos[2] = int16(binary.LittleEndian.Uint16(raw[off+14:]))
					off += 16
				}
			}
		}
	}

	// Read extra keyframe timing data (after node data).
	if numKF > 0 {
		a.Keyframes = make([]AnimKeyframe, numKF)
		for k := int32(0); k < numKF; k++ {
			if off+8 <= len(raw) {
				a.Keyframes[k].RefID = int32(binary.LittleEndian.Uint32(raw[off:]))
				a.Keyframes[k].Time = math.Float32frombits(binary.LittleEndian.Uint32(raw[off+4:]))
			}
			off += 8
		}
	}

	return nil
}

// Duration returns the animation duration in seconds.
func (a *HSpriteAnim) Duration() float32 {
	if a.FPS <= 0 {
		return 0
	}
	return float32(a.NumFrames) / a.FPS
}

// IsLoop returns true if this animation loops.
func (a *HSpriteAnim) IsLoop() bool {
	return a.PlaybackType == 0
}

// BoneRefMap holds the mapping from animation refID hashes to bone indices.
// Parsed from a RefMap (0x5000) object on CSprite/HSprite.
type BoneRefMap struct {
	Entries map[int32]int32 // refID hash → bone index
}

// ParseBoneRefMap reads a RefMap object's raw data and extracts the refID→bone mapping.
// Format: dictID(int32) + count(int32) + count × (refID(int32) + boneIndex(int32))
func ParseBoneRefMap(file *ObjFile, info *ObjInfo) *BoneRefMap {
	raw := file.RawBytes(info.Offset, int(info.Size))
	if len(raw) < 8 {
		return nil
	}
	// Skip dictID (4 bytes), read count.
	count := int32(binary.LittleEndian.Uint32(raw[4:]))
	expected := int32(8) + count*8
	if expected != info.Size || count <= 0 || count > 1000 {
		return nil
	}
	m := &BoneRefMap{Entries: make(map[int32]int32, count)}
	for i := int32(0); i < count; i++ {
		off := 8 + int(i)*8
		refID := int32(binary.LittleEndian.Uint32(raw[off:]))
		boneIdx := int32(binary.LittleEndian.Uint32(raw[off+4:]))
		m.Entries[refID] = boneIdx
	}
	return m
}

// ResolveBoneIndex looks up a refID in the RefMap and returns the bone index.
// Returns -1 if not found.
func (m *BoneRefMap) ResolveBoneIndex(refID int32) int32 {
	if m == nil {
		return -1
	}
	if idx, ok := m.Entries[refID]; ok {
		return idx
	}
	return -1
}
