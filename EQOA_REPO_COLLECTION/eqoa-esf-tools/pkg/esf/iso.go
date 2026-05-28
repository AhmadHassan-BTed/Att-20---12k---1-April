package esf

import (
	"fmt"
	"log"
	"os"
)

const sectorSize = 2048

// isoFileEntry describes an ESF or CSF file's location within the EQOA ISO.
type isoFileEntry struct {
	Sector int64 // start sector on disc
	Size   int64 // file size in bytes
	IsCSF  bool  // true = compressed CSF, needs decompression
}

// isoFileTable maps logical file names (as the client uses them) to their
// ISO disc locations. Files that only exist as CSF on disc are mapped from
// their ESF name so callers don't need to know about compression.
//
// Sectors and sizes from: isoinfo -R -l on EQOA Frontiers (SLUS-20744).
var isoFileTable = map[string]isoFileEntry{
	// Core ESF files (uncompressed on disc)
	"TUNARIA.ESF":  {Sector: 520000, Size: 997239555, IsCSF: false},
	"CHAR.ESF":     {Sector: 3578, Size: 148370972, IsCSF: false},
	"ITEM.ESF":     {Sector: 76052, Size: 4101595, IsCSF: false},
	"ITEMICON.ESF": {Sector: 601, Size: 3532704, IsCSF: false},
	"CHARFACE.ESF": {Sector: 78055, Size: 525272, IsCSF: false},

	// CSF files (compressed on disc, decompressed on read)
	"CHARCUST.CSF": {Sector: 83457, Size: 686060, IsCSF: true},
	"CHARFACE.CSF": {Sector: 79634, Size: 430718, IsCSF: true},
	"SETUP.CSF":    {Sector: 82606, Size: 1303077, IsCSF: true},
	"SKY.CSF":      {Sector: 83246, Size: 225960, IsCSF: true},
	"SPELLFX.CSF":  {Sector: 84322, Size: 398377, IsCSF: true},
	"UI.CSF":       {Sector: 78312, Size: 351790, IsCSF: true},
	"ARIAL.CSF":    {Sector: 79845, Size: 5700, IsCSF: true},
	"ARIALUI.CSF":  {Sector: 83357, Size: 203231, IsCSF: true},

	// Audio ESF files
	"AMBTRACK.ESF": {Sector: 2331, Size: 2532489, IsCSF: false},

	// World ESF files (other continents, uncompressed)
	"ODUS.ESF":     {Sector: 1006934, Size: 192453151, IsCSF: false},
	"RATHE.ESF":    {Sector: 1110589, Size: 99410791, IsCSF: false},
	"LAVASTM.ESF":  {Sector: 1159130, Size: 17054919, IsCSF: false},
	"PLANESKY.ESF": {Sector: 1100906, Size: 19830601, IsCSF: false},
	"SECRETS.ESF":  {Sector: 1167458, Size: 36671513, IsCSF: false},

	// Files that the client references as .ESF but only exist as .CSF on disc.
	// Map the ESF name to the CSF disc location so callers can use either name.
	"SETUP.ESF":   {Sector: 82606, Size: 1303077, IsCSF: true},
	"SKY.ESF":     {Sector: 83246, Size: 225960, IsCSF: true},
	"SPELLFX.ESF": {Sector: 84322, Size: 398377, IsCSF: true},
	"UI.ESF":      {Sector: 78312, Size: 351790, IsCSF: true},

	// BGM files — /BGM/ directory (race homelands, title, battle music)
	"BARBARIA.BGM": {Sector: 1416830, Size: 9306112},
	"THEME.BGM":    {Sector: 1421374, Size: 4849664},
	"ERUDITES.BGM": {Sector: 1423742, Size: 2883584},
	"QEYNOS.BGM":   {Sector: 1425150, Size: 2621440},
	"ELVES.BGM":    {Sector: 1426430, Size: 3670016},
	"TROLLS.BGM":   {Sector: 1428222, Size: 2752512},
	"BATTLE02.BGM": {Sector: 1429566, Size: 6815744},
	"HALFLING.BGM": {Sector: 1432894, Size: 3276800},
	"BATTLE05.BGM": {Sector: 1434494, Size: 7077888},
	"BATTLE07.BGM": {Sector: 1437950, Size: 3407872},
	"BATTLE04.BGM": {Sector: 1439614, Size: 7340032},
	"OGRE.BGM":     {Sector: 1443198, Size: 3276800},
	"DWARVES.BGM":  {Sector: 1444798, Size: 9437184},
	"BATTLE06.BGM": {Sector: 1449406, Size: 7602176},
	"BATTLE01.BGM": {Sector: 1453118, Size: 7208960},
	"GNOMES.BGM":   {Sector: 1456638, Size: 2359296},
	"FREEPORT.BGM": {Sector: 1457790, Size: 11534336},
	"DARKELVE.BGM": {Sector: 1463422, Size: 9568256},
	"BATTLE08.BGM": {Sector: 1485454, Size: 6553600},
	"BATTLE03.BGM": {Sector: 1488654, Size: 7602176},

	// BGM files — /MUSIC/MUSIC0/ directory (zone/dungeon music)
	"JUSTICE.BGM":  {Sector: 1185365, Size: 9699328},
	"VALOR.BGM":    {Sector: 1190101, Size: 8388608},
	"GUNTHAK.BGM":  {Sector: 1194197, Size: 9306112},
	"INNOVATE.BGM": {Sector: 1198741, Size: 9437184},
	"EARTH.BGM":    {Sector: 1203349, Size: 9830400},
	"TACTICS.BGM":  {Sector: 1208149, Size: 9568256},
	"DISEASE.BGM":  {Sector: 1212821, Size: 9568256},
	"COMBAT_1.BGM": {Sector: 1217493, Size: 3276800},
	"NADOX.BGM":    {Sector: 1219093, Size: 9306112},
	"THUNDER.BGM":  {Sector: 1223637, Size: 8781824},
	"WAR.BGM":      {Sector: 1227925, Size: 9306112},
	"TIME.BGM":     {Sector: 1232469, Size: 9437184},
	"DECAY.BGM":    {Sector: 1237077, Size: 9961472},
	"FIRE.BGM":     {Sector: 1241941, Size: 10223616},
	"TRANQ.BGM":    {Sector: 1246933, Size: 9306112},
	"STORMS.BGM":   {Sector: 1251477, Size: 10354688},
	"KNOWL.BGM":    {Sector: 1256533, Size: 12058624},
	"SOL_RO.BGM":   {Sector: 1262421, Size: 10223616},
	"SKY.BGM":      {Sector: 1267413, Size: 9699328},
	"COMBAT_2.BGM": {Sector: 1272149, Size: 3276800},
	"TORMENT.BGM":  {Sector: 1273749, Size: 9568256},
	"TORGIRAN.BGM": {Sector: 1278421, Size: 9961472},
	"DULAK.BGM":    {Sector: 1283285, Size: 10354688},
	"HONOR.BGM":    {Sector: 1288341, Size: 11534336},
	"NIGHTM.BGM":   {Sector: 1293973, Size: 9568256},
	"DRUNDER.BGM":  {Sector: 1298645, Size: 9568256},
	"WATER.BGM":    {Sector: 1303317, Size: 9961472},

	// BGM files — /MUSIC/MUSIC1/ directory (build/battle variants)
	"BUILD5K_.BGM": {Sector: 1308181, Size: 7995392},
	"BUILD10K.BGM": {Sector: 1312085, Size: 9568256},
	"BATTLE7K.BGM": {Sector: 1316757, Size: 7340032},
	"BATTLE3K.BGM": {Sector: 1320341, Size: 6815744},
	"BATTLE9K.BGM": {Sector: 1323669, Size: 7602176},
	"BATTLE8K.BGM": {Sector: 1327381, Size: 7077888},
	"BATTLE2K.BGM": {Sector: 1330837, Size: 7208960},
	"BUILD7K.BGM":  {Sector: 1334357, Size: 8912896},
	"BATTLE4K.BGM": {Sector: 1338709, Size: 7602176},
	"BUILD_2K.BGM": {Sector: 1342421, Size: 9830400},
	"BUILD3K_.BGM": {Sector: 1347221, Size: 8912896},
	"BATTLE1K.BGM": {Sector: 1351573, Size: 7208960},
	"BUILD9K.BGM":  {Sector: 1355093, Size: 5636096},
	"BATTLE5K.BGM": {Sector: 1357845, Size: 7208960},
	"BUILD8K.BGM":  {Sector: 1361365, Size: 8126464},
	"BUILD6K_.BGM": {Sector: 1365333, Size: 9437184},

	// BGM files — /BGM/VO2/ directory (class/race tutorials)
	"EEGG_4.BGM":   {Sector: 1468094, Size: 163840},
	"SL1_A.BGM":    {Sector: 1468174, Size: 1114112},
	"GC_1.BGM":     {Sector: 1468718, Size: 655360},
	"GC_2.BGM":     {Sector: 1469038, Size: 884736},
	"SL1_B.BGM":    {Sector: 1469470, Size: 1114112},
	"RT_1.BGM":     {Sector: 1470014, Size: 458752},
	"SS_1.BGM":     {Sector: 1470238, Size: 1376256},
	"TC6.BGM":      {Sector: 1470910, Size: 1015808},
	"RT_2.BGM":     {Sector: 1471406, Size: 786432},
	"TC1.BGM":      {Sector: 1471790, Size: 163840},
	"TC3.BGM":      {Sector: 1471870, Size: 1277952},
	"RT_3.BGM":     {Sector: 1472494, Size: 557056},
	"TC2_A.BGM":    {Sector: 1472766, Size: 327680},
	"PW_1.BGM":     {Sector: 1472926, Size: 917504},
	"TC4.BGM":      {Sector: 1473374, Size: 425984},
	"TC5.BGM":      {Sector: 1473582, Size: 425984},
	"RS_1.BGM":     {Sector: 1473790, Size: 1540096},
	"GC_3.BGM":     {Sector: 1474542, Size: 753664},
	"GC_4.BGM":     {Sector: 1474910, Size: 819200},
	"SL2.BGM":      {Sector: 1475310, Size: 622592},
	"SS_1_ALT.BGM": {Sector: 1475614, Size: 1409024},
	"TC2_B.BGM":    {Sector: 1476302, Size: 655360},

	// BGM files — /BGM/VO1/ directory (character creation)
	"ATTRIB_1.BGM": {Sector: 1476622, Size: 2588672},
	"CS_4.BGM":     {Sector: 1477886, Size: 327680},
	"CS_2.BGM":     {Sector: 1478046, Size: 262144},
	"EE_1.BGM":     {Sector: 1478174, Size: 131072},
	"AK.BGM":       {Sector: 1478238, Size: 884736},
	"DC_1.BGM":     {Sector: 1478670, Size: 360448},
	"CC_4.BGM":     {Sector: 1478846, Size: 622592},
	"AC1.BGM":      {Sector: 1479150, Size: 1310720},
	"CLASS_1.BGM":  {Sector: 1479790, Size: 1900544},
	"CC_3.BGM":     {Sector: 1480718, Size: 720896},
	"EE_2.BGM":     {Sector: 1481070, Size: 229376},
	"CN_2.BGM":     {Sector: 1481182, Size: 950272},
	"CHARREV.BGM":  {Sector: 1481646, Size: 786432},
	"CN_1.BGM":     {Sector: 1482030, Size: 557056},
	"CHARNAME.BGM": {Sector: 1482302, Size: 786432},
	"CD_3.BGM":     {Sector: 1482686, Size: 786432},
	"CS_1.BGM":     {Sector: 1483070, Size: 1605632},
	"CC_2.BGM":     {Sector: 1483854, Size: 589824},
	"CD_2.BGM":     {Sector: 1484142, Size: 557056},
	"BL_1.BGM":     {Sector: 1484414, Size: 983040},
	"CS_3.BGM":     {Sector: 1484894, Size: 360448},
	"CC_1.BGM":     {Sector: 1485070, Size: 786432},
}

// OpenISOFile reads a named ESF/CSF file from an EQOA game ISO.
// CSF files are automatically decompressed to ESF. The returned ObjFile
// is ready for BuildDictionary/FindObject.
// Returns an error if the file name is not in the ISO file table.
func OpenISOFile(isoPath, fileName string) (*ObjFile, error) {
	entry, ok := isoFileTable[fileName]
	if !ok {
		return nil, fmt.Errorf("unknown ISO file %q", fileName)
	}

	// CSF files must be decompressed — can't stream.
	if entry.IsCSF {
		data, err := readISORange(isoPath, entry.Sector*sectorSize, entry.Size)
		if err != nil {
			return nil, fmt.Errorf("reading %s from ISO: %w", fileName, err)
		}
		data, err = DecompressCSFBytes(data)
		if err != nil {
			return nil, fmt.Errorf("decompressing %s: %w", fileName, err)
		}
		f, err := OpenBytes(data)
		if err != nil {
			return nil, fmt.Errorf("parsing %s: %w", fileName, err)
		}
		f.ISOBase = entry.Sector * sectorSize
		return f, nil
	}

	// Large uncompressed ESF files (>10MB): use streaming mode.
	// Parse index from full read, then switch to on-demand 256KB windows.
	offset := entry.Sector * sectorSize
	if entry.Size > 10*1024*1024 {
		fd, err := os.Open(isoPath)
		if err != nil {
			return nil, err
		}
		// Read full file — ReadAt may short-read for large files, so loop.
		data := make([]byte, entry.Size)
		n := 0
		for int64(n) < entry.Size {
			nr, err := fd.ReadAt(data[n:], offset+int64(n))
			n += nr
			if err != nil {
				break
			}
		}
		if n < 32 {
			fd.Close()
			return nil, fmt.Errorf("reading %s from ISO: only got %d bytes", fileName, n)
		}
		data = data[:n]

		f := &ObjFile{
			data:       data,
			objCache:   make(map[int]Object),
			ISOBase:    offset,
			fileHandle: fd,
			fileBase:   offset,
			fileSize:   int64(n),
			winStart:   0,
			winEnd:     n, // full data loaded — ensureData won't reload
		}
		if err := f.readFileHeader(); err != nil {
			fd.Close()
			return nil, fmt.Errorf("parsing %s: %w", fileName, err)
		}
		// Parse index then release bulk data.
		if _, err := f.Root(); err != nil {
			fd.Close()
			return nil, fmt.Errorf("parsing %s index: %w", fileName, err)
		}
		log.Printf("esf: %s index parsed (%d objects). Streaming mode — releasing %d MB",
			fileName, len(f.objects), len(f.data)/(1024*1024))
		f.data = nil
		f.winStart = 0
		f.winEnd = 0
		return f, nil
	}

	// Small ESF files: load fully into memory.
	data, err := readISORange(isoPath, offset, entry.Size)
	if err != nil {
		return nil, fmt.Errorf("reading %s from ISO: %w", fileName, err)
	}
	f, err := OpenBytes(data)
	if err != nil {
		return nil, fmt.Errorf("parsing %s: %w", fileName, err)
	}
	f.ISOBase = offset
	return f, nil
}

// ISOHasFile returns true if the named file exists in the ISO file table.
func ISOHasFile(fileName string) bool {
	_, ok := isoFileTable[fileName]
	return ok
}

// ISOFileLocation describes a file's position within the ISO.
type ISOFileLocation struct {
	Offset int64
	Size   int64
}

// ISOGetFileLocation returns the byte offset and size of a named file in the ISO.
// Returns (location, true) if found, (zero, false) if not.
func ISOGetFileLocation(fileName string) (ISOFileLocation, bool) {
	entry, ok := isoFileTable[fileName]
	if !ok {
		return ISOFileLocation{}, false
	}
	return ISOFileLocation{
		Offset: entry.Sector * sectorSize,
		Size:   entry.Size,
	}, true
}

// ReadISOFileRaw reads raw bytes of a named file from the ISO without parsing.
// Use this for non-ESF files (e.g. BGM audio). Returns nil, error if not found.
func ReadISOFileRaw(isoPath, fileName string) ([]byte, error) {
	entry, ok := isoFileTable[fileName]
	if !ok {
		return nil, fmt.Errorf("unknown ISO file %q", fileName)
	}
	return readISORange(isoPath, entry.Sector*sectorSize, entry.Size)
}

// readISORange reads size bytes from offset in the ISO file.
func readISORange(isoPath string, offset, size int64) ([]byte, error) {
	fd, err := os.Open(isoPath)
	if err != nil {
		return nil, err
	}
	defer fd.Close()

	if _, err := fd.Seek(offset, 0); err != nil {
		return nil, fmt.Errorf("seek to offset %d: %w", offset, err)
	}

	data := make([]byte, size)
	n, err := fd.Read(data)
	if err != nil {
		return nil, fmt.Errorf("read %d bytes: %w", size, err)
	}
	return data[:n], nil
}
