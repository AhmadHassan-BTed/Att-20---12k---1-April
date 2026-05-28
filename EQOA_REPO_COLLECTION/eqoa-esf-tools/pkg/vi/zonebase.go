// VIZoneBase — zone terrain and structure definition.
// 175 zones in TUNARIA. Contains rooms, actors, flora, lighting.
//
// PS2 ParseZoneBaseObj at 0x00438838 — tree parser with many children.
// Children: ZoneTree(0x3100), ZoneRooms(0x3200), ZoneFlora(0x32D0),
//           ZoneStaticLightings(0x3270), PreTranslations(0x3250), etc.
package vi

// VIZoneBase is the top-level zone definition.
type VIZoneBase struct {
	// Zone metadata (from ZoneBase header)
	CenterX, CenterZ float32
	SizeX, SizeZ     float32

	// Pre-translation table for collision meshes
	PreTranslations []VIVect3

	// Flora data
	FloraModels []VIFloraModel
	FloraDists  []VIFloraDist
	FloraSets   []VIFloraSet

	// Actor placement (from ZoneRoomActors)
	Actors []VIZoneActor
}

// VIFloraModel defines one flora mesh type.
type VIFloraModel struct {
	DictID int32
	Scale  float32
}

// VIFloraDist defines flora distribution parameters.
type VIFloraDist struct {
	Density    float32
	MinScale   float32
	MaxScale   float32
	SwayAmount float32
}

// VIFloraSet maps flora models to collision types.
type VIFloraSet struct {
	Models []int32 // indices into FloraModels
}

const (
	TypeZoneBase         = 0x3000
	TypeZoneResource     = 0x3100
	TypeZoneRoom         = 0x3200
	TypeZoneTree         = 0x3220
	TypeZoneRooms        = 0x3230
	TypeZoneRoomEntry    = 0x3240
	TypePreTranslations  = 0x3250
	TypeZoneFlora        = 0x32D0
	TypeFloraModelArray  = 0x32E0
	TypeFloraDistArray   = 0x32F4
	TypeFloraSetArray    = 0x32F8
)

// ParsePreTranslations reads the pre-translation table from 0x3250 body data.
// Each entry is 3×float32 (VIVect3) = 12 bytes.
func ParsePreTranslations(data []byte) []VIVect3 {
	if len(data) < 4 {
		return nil
	}
	// No count field — infer from data size
	count := len(data) / 12
	pts := make([]VIVect3, count)
	pos := 0
	for i := 0; i < count && pos+12 <= len(data); i++ {
		pts[i].X = rf32(data, &pos)
		pts[i].Y = rf32(data, &pos)
		pts[i].Z = rf32(data, &pos)
	}
	return pts
}

// ParseZoneRoomEntry reads a room-actor entry from 0x3240 body data.
// Each entry: int32(actorListOffset) + int32(preTransOffset) + ...
// The exact format is 44 bytes but version-dependent.
type VIZoneRoomEntry struct {
	// Room bounding info and actor references
	// Exact field layout TBD from PS2 decompilation
	RawData []byte
}

func ParseZoneRoomEntries(roomsNode []*VIZoneRoomEntry, data []byte) {
	// Rooms are parsed from 0x3230 children (each child is 0x3240)
}
