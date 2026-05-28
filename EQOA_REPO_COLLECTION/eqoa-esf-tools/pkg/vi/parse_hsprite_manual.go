// ParseHSprite — hand-written from PS2 trace analysis.
package vi

// ParseHSpriteFull reads an HSprite from ESF tree data.
func ParseHSpriteFull(rootData []byte, children map[uint16][]byte) (*VIHSprite, error) {
	hs := &VIHSprite{}

	// Header child 0x2210
	if hdr, ok := children[TypeHSpriteHeader]; ok {
		pos := 0
		hs.DictID = ru32(hdr, &pos)
		hs.BBox.Min.X = rf32(hdr, &pos)
		hs.BBox.Min.Y = rf32(hdr, &pos)
		hs.BBox.Min.Z = rf32(hdr, &pos)
		hs.BBox.Max.X = rf32(hdr, &pos)
		hs.BBox.Max.Y = rf32(hdr, &pos)
		hs.BBox.Max.Z = rf32(hdr, &pos)
	}

	// Hierarchy child 0x2400
	if data, ok := children[0x2400]; ok {
		hs.Hierarchy = parseHierarchy(data)
	}

	// Triggers child 0x2450
	if data, ok := children[0x2450]; ok {
		hs.Triggers = parseTriggers(data)
	}

	// Attachments child 0x2500
	if data, ok := children[0x2500]; ok {
		hs.Attachments = parseAttachments(data)
	}

	// Animation child 0x2600 — parsed by ParseHSpriteAnim
	// ParseHSpriteAnim expects full ESF object (type+ver+size header + body).
	// children map has body-only data, so we need to reconstruct.
	// For now, parse the animation data directly from the body.
	if data, ok := children[TypeHSpriteAnim]; ok && len(data) > 0 {
		hs.Animation = parseHSpriteAnimBody(data)
	}

	return hs, nil
}

// parseHSpriteAnimBody parses animation data from body bytes (no ESF header).
// This is the same as ParseHSpriteAnim but without the type/ver/size check.
func parseHSpriteAnimBody(data []byte) *VIHSpriteAnim {
	if len(data) < 16 {
		return nil
	}
	anim := &VIHSpriteAnim{}
	pos := 0

	// Same fields as ParseHSpriteAnim but body starts at pos=0 (no 8-byte header)
	anim.DictID = ru32(data, &pos)
	anim.Format = ri32(data, &pos)
	anim.NumNodes = ri32(data, &pos)
	anim.NumFrames = ri32(data, &pos)
	// numKeyframes only in ver >= 3 — but we don't have ver here.
	// The body might have it. Check: if remaining data matches with or without.
	// For now, try reading it and see if frame data aligns.
	anim.NumKeyframes = ri32(data, &pos)
	anim.FPS = rf32(data, &pos)
	anim.PlaySpeed = rf32(data, &pos)
	anim.PlaybackType = ri32(data, &pos)

	if anim.NumNodes < 0 || anim.NumNodes > 1000 || anim.NumFrames < 0 || anim.NumFrames > 100000 {
		return nil
	}

	anim.NodeRefIDs = make([]int32, anim.NumNodes)
	anim.Frames = make([][]VIAnimFrame, anim.NumNodes)

	for n := int32(0); n < anim.NumNodes; n++ {
		anim.NodeRefIDs[n] = ri32(data, &pos)
		anim.Frames[n] = make([]VIAnimFrame, anim.NumFrames)
		for f := int32(0); f < anim.NumFrames; f++ {
			fr := &anim.Frames[n][f]
			switch anim.Format {
			case 0:
				if pos+32 > len(data) { return anim }
				fr.PosX = rf32(data, &pos)
				fr.PosY = rf32(data, &pos)
				fr.PosZ = rf32(data, &pos)
				fr.QuatX = rf32(data, &pos)
				fr.QuatY = rf32(data, &pos)
				fr.QuatZ = rf32(data, &pos)
				fr.QuatW = rf32(data, &pos)
				fr.Scale = rf32(data, &pos)
			case 1, 2:
				if pos+16 > len(data) { return anim }
				fr.PosX = float32(ri16(data, &pos)) / 512.0
				fr.PosY = float32(ri16(data, &pos)) / 512.0
				fr.PosZ = float32(ri16(data, &pos)) / 512.0
				fr.QuatX = float32(ri16(data, &pos)) / 32767.0
				fr.QuatY = float32(ri16(data, &pos)) / 32767.0
				fr.QuatZ = float32(ri16(data, &pos)) / 32767.0
				fr.QuatW = float32(ri16(data, &pos)) / 32767.0
				fr.Scale = float32(ri16(data, &pos)) / 32767.0
			}
		}
	}
	return anim
}

func parseAttachments(data []byte) []VIAttachment {
	pos := 0
	count := ri32(data, &pos)
	if count < 0 || count > 10000 { return nil }
	atts := make([]VIAttachment, count)
	for i := int32(0); i < count && pos < len(data); i++ {
		atts[i].NodeIndex = ri32(data, &pos)
		atts[i].Type = ri32(data, &pos)
		atts[i].DictID = ru32(data, &pos)
		atts[i].Flags = ri32(data, &pos)
	}
	return atts
}
