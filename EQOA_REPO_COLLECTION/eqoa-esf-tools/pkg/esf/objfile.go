package esf

import (
	"encoding/binary"
	"fmt"
	"log"
	"os"
	"strings"
)

// ObjInfo represents a node in the ESF object tree.
type ObjInfo struct {
	Type          uint16
	Version       int16
	Size          int32
	NumSubObjects int32
	Offset        int // byte offset of body start in file
	DictID        int32
	Children      []*ObjInfo
	Parent        *ObjInfo
}

func (o *ObjInfo) Child(typ uint16) *ObjInfo {
	for _, c := range o.Children {
		if c.Type == typ {
			return c
		}
	}
	return nil
}

func (o *ObjInfo) ChildrenOfType(typ uint16) []*ObjInfo {
	var out []*ObjInfo
	for _, c := range o.Children {
		if typ == 0 || c.Type == typ {
			out = append(out, c)
		}
	}
	return out
}

func (o *ObjInfo) ParentOfType(typ uint16) *ObjInfo {
	p := o.Parent
	for p != nil {
		if typ == 0 || p.Type == typ {
			return p
		}
		p = p.Parent
	}
	return nil
}

func (o *ObjInfo) NextSibling() *ObjInfo {
	if o.Parent == nil {
		return nil
	}
	for i, c := range o.Parent.Children {
		if c == o && i+1 < len(o.Parent.Children) {
			return o.Parent.Children[i+1]
		}
	}
	return nil
}

// ReadChildData extracts the raw body bytes for an ObjInfo node.
// Used by the VI bridge to pass raw ESF data to PS2-verified parsers.
func (f *ObjFile) ReadChildData(info *ObjInfo) []byte {
	if info == nil {
		return nil
	}
	start := info.Offset
	end := start + int(info.Size)
	f.ensureData(start, end)
	if end > len(f.data) {
		end = len(f.data)
	}
	if start >= end {
		return nil
	}
	out := make([]byte, end-start)
	copy(out, f.data[start:end])
	return out
}

// Available returns unread bytes remaining in this object's body.
func (o *ObjInfo) Available(filePos int) int {
	return int(o.Size) - (filePos - o.Offset)
}

func (o *ObjInfo) String() string {
	return fmt.Sprintf("%s(0x%04x) offset=0x%x size=0x%x subs=%d dict=0x%08x",
		TypeName(o.Type), o.Type, o.Offset, o.Size, o.NumSubObjects, o.DictID)
}

// ObjFile is the main ESF file parser.
// Supports two modes:
//   - In-memory: data[] holds the entire file (small ESF/CSF files)
//   - Streaming: fileHandle is set, data is loaded on demand via ensureData()
type ObjFile struct {
	data     []byte
	pos      int
	root     *ObjInfo
	objects  []*ObjInfo
	dict     map[int32]*ObjInfo
	objCache map[int]Object

	Debug   bool
	ISOBase int64 // byte offset of TUNARIA data within the ISO (0 for standalone ESF)

	// Streaming mode fields
	fileHandle *os.File // persistent file handle for streaming reads (nil = in-memory mode)
	fileBase   int64    // byte offset within file where ESF data starts
	fileSize   int64    // total ESF data size
	winStart   int      // start offset of current window in data[]
	winEnd     int      // end offset of current window in data[]
}

// Object is implemented by all parsed ESF objects.
type Object interface {
	Load(file *ObjFile) error
	ObjInfo() *ObjInfo
}

// Open parses an ESF file from disk. If the path ends in .iso,
// it extracts TUNARIA.ESF from the ISO at the known sector offset.
func Open(path string) (*ObjFile, error) {
	if strings.HasSuffix(strings.ToLower(path), ".iso") {
		return OpenISO(path)
	}
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	return OpenBytes(data)
}

// OpenISO opens TUNARIA.ESF from an EQOA ISO image in streaming mode.
// Only reads the header + object index (~1MB). Object data is loaded on demand
// via ensureData() when GetObject/reader functions access specific offsets.
func OpenISO(isoPath string) (*ObjFile, error) {
	const (
		sectorSize         = 2048
		tunariaStartSector = 520000
		tunariaEndSector   = 1006934
		tunariaByteOffset  = tunariaStartSector * sectorSize // 1064960000
		tunariaByteSize    = (tunariaEndSector - tunariaStartSector) * sectorSize
	)

	fd, err := os.Open(isoPath)
	if err != nil {
		return nil, err
	}

	// Read the full file into memory for index parsing (parse() walks the entire
	// object tree to build the offset map). After parsing, we switch to streaming
	// mode — drop the bulk data and only keep the file handle.
	data := make([]byte, tunariaByteSize)
	n := 0
	for n < tunariaByteSize {
		nr, err := fd.ReadAt(data[n:], int64(tunariaByteOffset)+int64(n))
		n += nr
		if err != nil {
			break
		}
	}
	if n < 32 {
		fd.Close()
		return nil, fmt.Errorf("ISO read TUNARIA: only got %d bytes", n)
	}
	data = data[:n]

	magic := string([]byte{data[3], data[2], data[1], data[0]})
	if magic != "OBJF" {
		fd.Close()
		return nil, fmt.Errorf("no OBJF magic at ISO offset 0x%x, got %q", tunariaByteOffset, magic)
	}

	f := &ObjFile{
		data:       data,
		objCache:   make(map[int]Object),
		ISOBase:    int64(tunariaByteOffset),
		fileHandle: fd,
		winEnd:     n, // full data loaded — ensureData won't reload during parsing
		fileBase:   int64(tunariaByteOffset),
		fileSize:   int64(n),
	}
	if err := f.readFileHeader(); err != nil {
		fd.Close()
		return nil, err
	}

	// Parse the full object tree to build index (needs data in memory).
	if _, err := f.Root(); err != nil {
		fd.Close()
		return nil, err
	}

	// Switch to streaming mode — release bulk data, keep only file handle.
	// Object data will be loaded on demand via ensureData().
	log.Printf("esf: TUNARIA index parsed (%d objects). Switching to streaming mode — releasing %d MB",
		len(f.objects), len(f.data)/(1024*1024))
	f.data = nil
	f.winStart = 0
	f.winEnd = 0

	return f, nil
}

// OpenBytes parses an ESF file from a byte slice.
func OpenBytes(data []byte) (*ObjFile, error) {
	f := &ObjFile{
		data:     data,
		objCache: make(map[int]Object),
	}
	if err := f.readFileHeader(); err != nil {
		return nil, err
	}
	return f, nil
}

func (f *ObjFile) readFileHeader() error {
	if len(f.data) < 32 {
		return fmt.Errorf("file too small for header")
	}
	// Magic is stored reversed
	magic := string([]byte{f.data[3], f.data[2], f.data[1], f.data[0]})
	if magic != "OBJF" {
		return fmt.Errorf("missing OBJF magic, got %q", magic)
	}
	f.pos = 4
	_ = f.readInt32()  // numObjects
	_ = f.readInt32()  // fileType
	_ = f.readInt32()  // unknown
	offset := f.readInt64() // data offset
	_ = f.readInt64()  // unknown2
	f.pos = int(offset)
	return nil
}

// Root returns the root ObjInfo, parsing the tree if needed.
func (f *ObjFile) Root() (*ObjInfo, error) {
	if f.root == nil {
		if err := f.parse(); err != nil {
			return nil, err
		}
	}
	return f.root, nil
}

func (f *ObjFile) parse() error {
	// Read the first top-level object.
	root, err := f.readObject(nil)
	if err != nil {
		return err
	}
	f.root = root

	// Some ESF files (e.g. addart.esf) have multiple top-level objects
	// at the same level. If there's more data, keep reading siblings
	// and attach them as children of a synthetic root.
	var siblings []*ObjInfo
	for f.pos+12 <= len(f.data) {
		// Peek at the next type — if 0 or invalid, stop.
		nextType := binary.LittleEndian.Uint16(f.data[f.pos:])
		if nextType == 0 {
			break
		}
		sib, err := f.readObject(nil)
		if err != nil {
			break // non-fatal: stop reading siblings
		}
		siblings = append(siblings, sib)
	}

	if len(siblings) > 0 {
		// Wrap everything under a synthetic root so the tree stays navigable.
		synth := &ObjInfo{
			Type:          TypeRoot,
			NumSubObjects: int32(1 + len(siblings)),
			Children:      append([]*ObjInfo{root}, siblings...),
		}
		for _, c := range synth.Children {
			c.Parent = synth
		}
		f.root = synth
	}

	if f.Debug {
		log.Printf("parsed %d objects", len(f.objects))
	}
	return nil
}

func (f *ObjFile) readObject(parent *ObjInfo) (*ObjInfo, error) {
	info := &ObjInfo{Parent: parent}

	if f.pos+12 > len(f.data) {
		return nil, fmt.Errorf("unexpected EOF reading object header at pos %d", f.pos)
	}

	info.Type = binary.LittleEndian.Uint16(f.data[f.pos:])
	info.Version = int16(binary.LittleEndian.Uint16(f.data[f.pos+2:]))
	info.Size = int32(binary.LittleEndian.Uint32(f.data[f.pos+4:]))
	info.NumSubObjects = int32(binary.LittleEndian.Uint32(f.data[f.pos+8:]))
	f.pos += 12
	info.Offset = f.pos

	if parent != nil {
		parent.Children = append(parent.Children, info)
	}

	// Read dict ID if this type has one
	if typeHasDictID(info.Type) && info.Size >= 4 {
		info.DictID = int32(binary.LittleEndian.Uint32(f.data[f.pos:]))
	}

	f.objects = append(f.objects, info)

	// Recursively read sub-objects
	for i := int32(0); i < info.NumSubObjects; i++ {
		if _, err := f.readObject(info); err != nil {
			return nil, fmt.Errorf("reading sub-object %d of %s: %w", i, info, err)
		}
	}

	// Skip any unread body bytes
	end := info.Offset + int(info.Size)
	if f.pos < end {
		f.pos = end
	}

	return info, nil
}

// BuildDictionary builds the global ID→ObjInfo lookup table.
func (f *ObjFile) BuildDictionary() error {
	if _, err := f.Root(); err != nil {
		return err
	}
	f.dict = make(map[int32]*ObjInfo)
	for _, o := range f.objects {
		if o.DictID != 0 {
			if _, exists := f.dict[o.DictID]; !exists {
				f.dict[o.DictID] = o
			}
		}
	}
	if f.Debug {
		log.Printf("built dictionary with %d entries", len(f.dict))
	}
	return nil
}

// DictLen returns the number of entries in the dictionary.
func (f *ObjFile) DictLen() int {
	return len(f.dict)
}

// DictKeys returns all dictionary IDs (for debugging).
func (f *ObjFile) DictKeys() []int32 {
	keys := make([]int32, 0, len(f.dict))
	for k := range f.dict {
		keys = append(keys, k)
	}
	return keys
}

// ReadInt32At reads an int32 at the given byte offset in the file.
func (f *ObjFile) ReadInt32At(offset int) int32 {
	f.ensureData(offset, 4)
	sp := f.streamPos(offset)
	return int32(binary.LittleEndian.Uint32(f.data[sp:]))
}

// FindObject looks up an object by dictionary ID.
func (f *ObjFile) FindObject(id int32) (Object, error) {
	if f.dict == nil {
		if err := f.BuildDictionary(); err != nil {
			return nil, err
		}
	}
	info, ok := f.dict[id]
	if !ok {
		return nil, nil
	}
	// ID containers (headers) → return parent object
	if typeIsIDContainer(info.Type) && info.Parent != nil {
		info = info.Parent
	}
	return f.GetObject(info)
}

// ReleaseCache drops the parsed object cache, freeing memory for GC.
// The ObjFile remains usable — objects will be re-parsed on next GetObject call.
func (f *ObjFile) ReleaseCache() {
	for k := range f.objCache {
		delete(f.objCache, k)
	}
}

// GetObject loads and caches an object from its ObjInfo.
func (f *ObjFile) GetObject(info *ObjInfo) (Object, error) {
	if cached, ok := f.objCache[info.Offset]; ok {
		return cached, nil
	}
	obj := f.createObject(info)
	if obj == nil {
		return nil, nil
	}
	// Save/restore position
	savedPos := f.pos
	f.pos = info.Offset
	err := obj.Load(f)
	f.pos = savedPos
	if err != nil {
		return nil, fmt.Errorf("loading %s: %w", info, err)
	}
	f.objCache[info.Offset] = obj
	return obj, nil
}

func (f *ObjFile) createObject(info *ObjInfo) Object {
	switch info.Type {
	case TypeSurface:
		return &Surface{info: info}
	case TypeMaterialPalette:
		return &MaterialPalette{info: info}
	case TypeMaterial:
		return &Material{info: info}
	case TypePrimBuffer, TypeSkinPrimBuffer:
		return &PrimBuffer{info: info}
	case TypeColorBuffer:
		return &ColorBuffer{info: info}
	case TypeSimpleSprite:
		return &SimpleSprite{info: info, UsePretrans: true}
	case TypeSimpleSubSprite:
		return &SimpleSubSprite{SimpleSprite: SimpleSprite{info: info, UsePretrans: true}}
	case TypeSkinSubSprite:
		return &SkinSubSprite{SimpleSubSprite: SimpleSubSprite{SimpleSprite: SimpleSprite{info: info, UsePretrans: true}}}
	case TypeGroupSprite:
		return &GroupSprite{SimpleSprite: SimpleSprite{info: info, UsePretrans: true}}
	case TypeHSprite:
		return &HSprite{GroupSprite: GroupSprite{SimpleSprite: SimpleSprite{info: info, UsePretrans: true}}}
	case TypeCSprite:
		return &CSprite{GroupSprite: GroupSprite{SimpleSprite: SimpleSprite{info: info, UsePretrans: true}}}
	case TypeFloraSprite:
		return &FloraSprite{SimpleSprite: SimpleSprite{info: info}}
	case TypePointSprite:
		return &PointSprite{info: info}
	case TypeFont:
		return &Font{info: info}
	case TypeLODSprite:
		return &LODSprite{SimpleSprite: SimpleSprite{info: info, UsePretrans: true}}
	case TypeCSpriteVariant:
		return &SkinLODSprite{info: info}
	case TypeZone:
		return &Zone{info: info}
	case TypeZoneBase:
		return &ZoneBase{info: info}
	case TypeZonePreTranslations:
		return &ZonePreTranslations{info: info}
	case TypeZoneActors:
		return &ZoneActors{info: info}
	case TypeZoneActor:
		return &ZoneActor{info: info}
	case TypeCollBuffer:
		return &CollBuffer{info: info}
	case TypeWorld:
		return &World{info: info}
	case TypeWorldBase:
		return &WorldBase{info: info}
	case TypeWorldBaseHeader:
		return &WorldBaseHeader{info: info}
	case TypeWorldZoneProxies:
		return &WorldZoneProxies{info: info}
	case TypeZoneStaticTable:
		return &ZoneStaticTable{info: info}
	case TypeHSpriteHierarchy:
		return &HSpriteHierarchy{info: info}
	case TypeHSpriteAnim:
		return &HSpriteAnim{info: info}
	case TypeSpellEffect:
		return &SpellEffect{info: info}
	case TypeParticleSprite:
		return &ParticleSprite{info: info}
	case TypeParticleDefinition:
		return &ParticleDefinition{info: info}
	case TypeEffectVolumeSprite:
		return &EffectVolumeSprite{info: info}
	case TypeStreamAudioSprite:
		return &StreamAudioSprite{info: info}
	case TypeAdpcm:
		return &Adpcm{info: info}
	case TypeXm:
		return &Xm{info: info}
	case TypeSoundSprite:
		return &SoundSprite{info: info}
	case TypePointLight:
		return &PointLight{info: info}
	default:
		return &GenericObj{info: info}
	}
}

// Close releases the file handle for streaming mode.
func (f *ObjFile) Close() {
	if f.fileHandle != nil {
		f.fileHandle.Close()
		f.fileHandle = nil
	}
}

// RawBytes returns a copy of file data at the given offset and size.
func (f *ObjFile) Data() []byte { return f.data }

func (f *ObjFile) RawBytes(offset, size int) []byte {
	f.ensureData(offset, size)
	sp := f.streamPos(offset)
	end := sp + size
	if end > len(f.data) {
		end = len(f.data)
	}
	if sp >= end {
		return nil
	}
	out := make([]byte, end-sp)
	copy(out, f.data[sp:end])
	return out
}

// Seek sets the read position.
func (f *ObjFile) Seek(offset int) {
	f.pos = offset
}

// ensureData guarantees that f.data[pos:pos+n] is readable.
// In memory mode (fileHandle == nil), data is always available.
// In streaming mode, loads a window from the file handle if needed.
func (f *ObjFile) ensureData(pos, n int) {
	if f.fileHandle == nil {
		return // in-memory mode — data[] covers everything
	}
	end := pos + n
	if pos >= f.winStart && end <= f.winEnd {
		return // already in window
	}
	// Load a window that fully covers [pos, pos+n).
	// Use at least 256KB to amortize sequential reads, but expand if n is larger.
	const minWindow = 256 * 1024
	needed := n
	if needed < minWindow {
		needed = minWindow
	}
	winStart := pos
	if winStart > needed/4 {
		winStart -= needed / 4 // read a bit before pos for context
	}
	winSize := needed + needed/4 // extra padding after
	if int64(winStart+winSize) > f.fileSize {
		winSize = int(f.fileSize) - winStart
	}
	if winSize <= 0 {
		return
	}

	buf := make([]byte, winSize)
	// ReadAt may return short for large reads on ISO — loop until complete.
	var nr int
	for nr < winSize {
		n, err := f.fileHandle.ReadAt(buf[nr:], f.fileBase+int64(winStart)+int64(nr))
		nr += n
		if err != nil {
			break
		}
	}
	if nr <= 0 {
		return
	}
	f.data = buf[:nr]
	f.winStart = winStart
	f.winEnd = winStart + nr
}

// streamPos translates a logical ESF offset to an index into f.data[].
// In memory mode, returns pos unchanged. In streaming mode, returns pos - winStart.
func (f *ObjFile) streamPos(pos int) int {
	if f.fileHandle == nil {
		return pos
	}
	return pos - f.winStart
}

// Pos returns the current read position.
func (f *ObjFile) Pos() int {
	return f.pos
}

// --- Reader helpers (all little-endian) ---

func (f *ObjFile) readByte() byte {
	f.ensureData(f.pos, 1)
	sp := f.streamPos(f.pos)
	f.pos++
	if sp < 0 || sp >= len(f.data) {
		return 0
	}
	return f.data[sp]
}

func (f *ObjFile) readInt16() int16 {
	f.ensureData(f.pos, 2)
	sp := f.streamPos(f.pos)
	f.pos += 2
	if sp < 0 || sp+2 > len(f.data) {
		return 0
	}
	return int16(binary.LittleEndian.Uint16(f.data[sp:]))
}

func (f *ObjFile) readUint16() uint16 {
	f.ensureData(f.pos, 2)
	sp := f.streamPos(f.pos)
	f.pos += 2
	if sp < 0 || sp+2 > len(f.data) {
		return 0
	}
	return binary.LittleEndian.Uint16(f.data[sp:])
}

func (f *ObjFile) readInt32() int32 {
	f.ensureData(f.pos, 4)
	sp := f.streamPos(f.pos)
	f.pos += 4
	if sp < 0 || sp+4 > len(f.data) {
		return 0
	}
	return int32(binary.LittleEndian.Uint32(f.data[sp:]))
}

func (f *ObjFile) readUint32() uint32 {
	f.ensureData(f.pos, 4)
	sp := f.streamPos(f.pos)
	f.pos += 4
	if sp < 0 || sp+4 > len(f.data) {
		return 0
	}
	return binary.LittleEndian.Uint32(f.data[sp:])
}

func (f *ObjFile) readInt64() int64 {
	f.ensureData(f.pos, 8)
	sp := f.streamPos(f.pos)
	f.pos += 8
	if sp < 0 || sp+8 > len(f.data) {
		return 0
	}
	return int64(binary.LittleEndian.Uint64(f.data[sp:]))
}

func (f *ObjFile) readFloat32() float32 {
	f.ensureData(f.pos, 4)
	sp := f.streamPos(f.pos)
	f.pos += 4
	if sp < 0 || sp+4 > len(f.data) {
		return 0
	}
	bits := binary.LittleEndian.Uint32(f.data[sp:])
	return float32frombits(bits)
}

func (f *ObjFile) readBytes(n int) []byte {
	f.ensureData(f.pos, n)
	sp := f.streamPos(f.pos)
	v := make([]byte, n)
	if sp >= 0 && sp+n <= len(f.data) {
		copy(v, f.data[sp:sp+n])
	}
	f.pos += n
	return v
}

func (f *ObjFile) skipBytes(n int) {
	f.pos += n
}

func (f *ObjFile) readPoint() Point {
	return Point{f.readFloat32(), f.readFloat32(), f.readFloat32()}
}

func (f *ObjFile) readBox() Box {
	return Box{
		MinX: f.readFloat32(), MinY: f.readFloat32(), MinZ: f.readFloat32(),
		MaxX: f.readFloat32(), MaxY: f.readFloat32(), MaxZ: f.readFloat32(),
	}
}

func (f *ObjFile) readColor() [4]byte {
	f.ensureData(f.pos, 4)
	sp := f.streamPos(f.pos)
	var c [4]byte
	copy(c[:], f.data[sp:sp+4])
	f.pos += 4
	return c
}

func (f *ObjFile) readString() (string, error) {
	length := int(f.readInt16())
	if length < 0 || length > 1024 {
		return "", fmt.Errorf("readString sanity: len=%d", length)
	}
	f.ensureData(f.pos, length)
	sp := f.streamPos(f.pos)
	s := string(f.data[sp : sp+length])
	f.pos += length
	return s, nil
}

// AllObjects returns all parsed ObjInfo nodes in file order.
func (f *ObjFile) AllObjects() []*ObjInfo {
	return f.objects
}

// GenericObj is used for unimplemented object types.
type GenericObj struct {
	info *ObjInfo
}

func (g *GenericObj) Load(_ *ObjFile) error { return nil }
func (g *GenericObj) ObjInfo() *ObjInfo      { return g.info }
