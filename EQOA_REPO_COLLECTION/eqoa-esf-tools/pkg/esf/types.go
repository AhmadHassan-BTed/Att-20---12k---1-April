package esf

const (
	TypeSurface                   uint16 = 0x1000
	TypeSurfaceArray              uint16 = 0x1001
	TypeMaterial                  uint16 = 0x1100
	TypeMaterialArray             uint16 = 0x1101
	TypeMaterialPalette           uint16 = 0x1110
	TypeMaterialPaletteHeader     uint16 = 0x1111
	TypePrimBuffer                uint16 = 0x1200
	TypeSkinPrimBuffer            uint16 = 0x1210
	TypeColorBuffer               uint16 = 0x1220
	TypeFloraPrimBuffer           uint16 = 0x1230
	TypeSimpleSprite              uint16 = 0x2000
	TypeSimpleSpriteHeader        uint16 = 0x2001
	TypeHSprite                   uint16 = 0x2200
	TypeHSpriteHeader             uint16 = 0x2210
	TypeHSpriteArray              uint16 = 0x2220
	TypeHSpriteHierarchy          uint16 = 0x2400
	TypeHSpriteTriggers           uint16 = 0x2450
	TypeHSpriteAttachments        uint16 = 0x2500
	TypeHSpriteAnim               uint16 = 0x2600
	TypeSimpleSubSprite           uint16 = 0x2310
	TypeSimpleSubSpriteHeader     uint16 = 0x2311
	TypeSkinSubSprite             uint16 = 0x2320
	TypeSkinSubSprite2            uint16 = 0x2321
	TypeRefMap                    uint16 = 0x5000
	TypeCSprite                   uint16 = 0x2700
	TypeCSpriteHeader             uint16 = 0x2710
	TypeCSpriteArray              uint16 = 0x2800
	TypeLODSprite                 uint16 = 0x2a10
	TypeLODSpriteArray            uint16 = 0x2a20
	TypeCSpriteSkinList           uint16 = 0x2900
	TypeCSpritePlayList           uint16 = 0x2910
	TypeCSpriteNodeIDList         uint16 = 0x2915
	TypeCSpriteASlotList          uint16 = 0x2920
	TypeCSpriteTSlotList          uint16 = 0x2930
	TypeCSpriteVariant            uint16 = 0x2a40
	TypeCSpriteVariantHeader      uint16 = 0x2a50
	TypeCSpriteVariantMeshes      uint16 = 0x2a60
	TypeCSpriteVariantFooter      uint16 = 0x2a70
	TypePointLight                uint16 = 0x2b00
	TypeGroupSprite               uint16 = 0x2c00
	TypeGroupSpriteHeader         uint16 = 0x2c10
	TypeGroupSpriteArray          uint16 = 0x2c20
	TypeGroupSpriteMembers        uint16 = 0x2c30
	TypeStreamAudioSprite         uint16 = 0x2e00
	TypePointSprite               uint16 = 0x2d00
	TypeFloraSprite               uint16 = 0x2f00
	TypeFloraSpriteHeader         uint16 = 0x2f01
	TypeZone                      uint16 = 0x3000
	TypeZoneResources             uint16 = 0x3100
	TypeZoneBase                  uint16 = 0x3200
	TypeZoneTree                  uint16 = 0x3220
	TypeZoneRooms                 uint16 = 0x3230
	TypeZoneRoom                  uint16 = 0x3240
	TypeZonePreTranslations       uint16 = 0x3250
	TypeZoneRoomActors            uint16 = 0x3270
	TypeZoneRoomActors2           uint16 = 0x3280
	TypeZoneActors                uint16 = 0x3290
	TypeZoneRoomStaticLightings2  uint16 = 0x32a0
	TypeZoneStaticLightnings      uint16 = 0x32b0
	TypeZoneStaticTable           uint16 = 0x32c0
	TypeZoneFlora                 uint16 = 0x32d0
	TypeZoneFloraSpriteArray      uint16 = 0x32e0
	TypeZoneFloraSets             uint16 = 0x32f0
	TypeZoneFloraDistArray        uint16 = 0x32f4
	TypeZoneFloraSetArray         uint16 = 0x32f8
	TypeCollBuffer                uint16 = 0x4200
	TypeZoneActor                 uint16 = 0x6000
	TypeStaticLighting            uint16 = 0x6010
	TypeStaticLightingObj         uint16 = 0x6020
	TypeZoneRoomStaticLightings3  uint16 = 0x6030
	TypeZoneRoomActors3           uint16 = 0x6040
	TypeFont                      uint16 = 0x7000
	TypeRoot                      uint16 = 0x8000
	TypeWorld                     uint16 = 0x8100
	TypeWorldBase                 uint16 = 0x8200
	TypeWorldZoneProxies          uint16 = 0x8210
	TypeWorldBaseHeader           uint16 = 0x8220
	TypeWorldTree                 uint16 = 0x8230
	TypeWorldRegions              uint16 = 0x8240
	TypeResourceTable             uint16 = 0x9000
	TypeResourceDir               uint16 = 0xa000
	TypeResourceDir2              uint16 = 0xa010
	TypeAdpcm                     uint16 = 0xb000
	TypeAdpcmHeader               uint16 = 0xb010
	TypeAdpcmData                 uint16 = 0xb020
	TypeXm                        uint16 = 0xb030
	TypeSoundSprite               uint16 = 0xb100
	TypeSoundSpriteHeader         uint16 = 0xb101
	TypeParticleDefinition        uint16 = 0xc000
	TypeParticleDefHeader         uint16 = 0xc010
	TypeParticleDefData           uint16 = 0xc020
	TypeParticleSprite            uint16 = 0xc100
	TypeParticleSpriteHeader      uint16 = 0xc101
	TypeSpellEffect               uint16 = 0xc200
	TypeSpellEffectHeader         uint16 = 0xc210
	TypeSpellEffectData           uint16 = 0xc220
	TypeEffectVolumeSprite        uint16 = 0xc300
	TypeEffectVolumeSpriteHeader  uint16 = 0xc310
	TypeEffectVolumeParticle      uint16 = 0xc320
	TypeEffectVolumeParams        uint16 = 0xc330
	TypeCSpritePartDefs           uint16 = 0x2950
	TypeCSpritePartEmitters       uint16 = 0x2960
)

type objTypeDef struct {
	Name       string
	HasDictID  bool
	IsIDContainer bool
}

var objTypeRegistry = map[uint16]objTypeDef{
	TypeSurface:                  {"Surface", true, false},
	TypeSurfaceArray:             {"SurfaceArray", false, false},
	TypeMaterial:                 {"Material", false, false},
	TypeMaterialArray:            {"MaterialArray", false, false},
	TypeMaterialPalette:          {"MaterialPalette", false, false},
	TypeMaterialPaletteHeader:    {"MaterialPaletteHeader", true, true},
	TypePrimBuffer:               {"PrimBuffer", true, false},
	TypeSkinPrimBuffer:           {"SkinPrimBuffer", false, false},
	TypeColorBuffer:              {"ColorBuffer", true, false},
	TypeFloraPrimBuffer:          {"FloraPrimBuffer", true, false},
	TypeSimpleSprite:             {"SimpleSprite", false, false},
	TypeSimpleSpriteHeader:       {"SimpleSpriteHeader", true, true},
	TypeHSprite:                  {"HSprite", false, false},
	TypeHSpriteHeader:            {"HSpriteHeader", true, true},
	TypeHSpriteArray:             {"HSpriteArray", false, false},
	TypeHSpriteHierarchy:         {"HSpriteHierarchy", false, false},
	TypeHSpriteTriggers:          {"HSpriteTriggers", false, false},
	TypeHSpriteAttachments:       {"HSpriteAttachments", false, false},
	TypeHSpriteAnim:              {"HSpriteAnim", false, false},
	TypeSimpleSubSprite:          {"SimpleSubSprite", false, false},
	TypeSimpleSubSpriteHeader:    {"SimpleSubSpriteHeader", false, false},
	TypeSkinSubSprite:            {"SkinSubSprite", true, false},
	TypeSkinSubSprite2:           {"SkinSubSprite2", false, false},
	TypeRefMap:                   {"RefMap", true, false},
	TypeCSprite:                  {"CSprite", false, false},
	TypeCSpriteHeader:            {"CSpriteHeader", true, true},
	TypeCSpriteArray:             {"CSpriteArray", false, false},
	TypeCSpritePlayList:          {"CSpritePlayList", false, false},
	TypeCSpriteNodeIDList:        {"CSpriteNodeIDList", false, false},
	TypeCSpriteASlotList:         {"CSpriteASlotList", false, false},
	TypeCSpriteTSlotList:         {"CSpriteTSlotList", false, false},
	TypeLODSprite:                {"LODSprite", true, false},
	TypeLODSpriteArray:           {"LODSpriteArray", false, false},
	TypeCSpriteVariant:           {"CSpriteVariant", false, false},
	TypeCSpriteVariantHeader:     {"CSpriteVariantHeader", false, false},
	TypeCSpriteVariantMeshes:     {"CSpriteVariantMeshes", false, false},
	TypeCSpriteVariantFooter:     {"CSpriteVariantFooter", false, false},
	TypePointLight:               {"PointLight", false, false},
	TypeGroupSprite:              {"GroupSprite", false, false},
	TypeGroupSpriteHeader:        {"GroupSpriteHeader", true, true},
	TypeGroupSpriteArray:         {"GroupSpriteArray", false, false},
	TypeGroupSpriteMembers:       {"GroupSpriteMembers", false, false},
	TypeStreamAudioSprite:        {"StreamAudioSprite", true, false},
	0x2e10:                       {"StreamAudioSpriteHeader", false, false},
	TypePointSprite:              {"PointSprite", false, false},
	TypeFloraSprite:              {"FloraSprite", false, false},
	TypeFloraSpriteHeader:        {"FloraSpriteHeader", true, true},
	TypeZone:                     {"Zone", false, false},
	TypeZoneResources:            {"ZoneResources", false, false},
	TypeZoneBase:                 {"ZoneBase", false, false},
	TypeZoneTree:                 {"ZoneTree", false, false},
	TypeZoneRooms:                {"ZoneRooms", false, false},
	TypeZoneRoom:                 {"ZoneRoom", false, false},
	TypeZonePreTranslations:      {"ZonePreTranslations", false, false},
	TypeZoneRoomActors:           {"ZoneRoomActors", false, false},
	TypeZoneRoomActors2:          {"ZoneRoomActors2", false, false},
	TypeZoneActors:               {"ZoneActors", false, false},
	TypeZoneRoomStaticLightings2: {"ZoneRoomStaticLightings2", false, false},
	TypeZoneStaticLightnings:     {"ZoneStaticLightnings", false, false},
	TypeZoneStaticTable:          {"ZoneStaticTable", false, false},
	TypeZoneFlora:                {"ZoneFlora", false, false},
	TypeZoneFloraSpriteArray:     {"ZoneFloraSpriteArray", false, false},
	TypeZoneFloraSets:            {"ZoneFloraSets", false, false},
	TypeZoneFloraDistArray:       {"ZoneFloraDistArray", true, true},
	TypeZoneFloraSetArray:        {"ZoneFloraSetArray", true, true},
	TypeCollBuffer:               {"CollBuffer", true, false},
	TypeZoneActor:                {"ZoneActor", false, false},
	TypeStaticLighting:           {"StaticLighting", false, false},
	TypeStaticLightingObj:        {"StaticLightingObj", false, false},
	TypeZoneRoomStaticLightings3: {"ZoneRoomStaticLightings3", false, false},
	TypeZoneRoomActors3:          {"ZoneRoomActors3", false, false},
	TypeFont:                     {"Font", false, false},
	TypeRoot:                     {"Root", false, false},
	TypeWorld:                    {"World", false, false},
	TypeWorldBase:                {"WorldBase", false, false},
	TypeWorldZoneProxies:         {"WorldZoneProxies", false, false},
	TypeWorldBaseHeader:          {"WorldBaseHeader", false, false},
	TypeWorldTree:                {"WorldTree", false, false},
	TypeWorldRegions:             {"WorldRegions", false, false},
	TypeResourceTable:            {"ResourceTable", false, false},
	TypeResourceDir:              {"ResourceDir", false, false},
	TypeResourceDir2:             {"ResourceDir2", false, false},
	TypeAdpcm:                    {"Adpcm", false, false},
	TypeAdpcmHeader:              {"AdpcmHeader", false, false},
	TypeAdpcmData:               {"AdpcmData", false, false},
	TypeXm:                       {"Xm", false, false},
	0xB040:                       {"XmHeader", false, false},
	0xB060:                       {"XmSampleData", false, false},
	TypeSoundSprite:              {"SoundSprite", false, false},
	TypeSoundSpriteHeader:        {"SoundSpriteHeader", true, true},
	TypeParticleDefinition:       {"ParticleDefinition", false, false},
	TypeParticleDefHeader:        {"ParticleDefHeader", true, true},
	TypeParticleDefData:          {"ParticleDefData", false, false},
	TypeParticleSprite:           {"ParticleSprite", false, false},
	TypeParticleSpriteHeader:     {"ParticleSpriteHeader", true, true},
	TypeSpellEffect:              {"SpellEffect", false, false},
	TypeSpellEffectHeader:        {"SpellEffectHeader", true, true},
	TypeSpellEffectData:          {"SpellEffectData", false, false},
	TypeEffectVolumeSprite:       {"EffectVolumeSprite", false, false},
	TypeEffectVolumeSpriteHeader: {"EffectVolumeSpriteHeader", true, true},
	TypeEffectVolumeParticle:     {"EffectVolumeParticle", false, false},
	TypeEffectVolumeParams:       {"EffectVolumeParams", false, false},
}

func TypeName(t uint16) string {
	if def, ok := objTypeRegistry[t]; ok {
		return def.Name
	}
	return "Unknown"
}

func typeHasDictID(t uint16) bool {
	if def, ok := objTypeRegistry[t]; ok {
		return def.HasDictID
	}
	return false
}

func typeIsIDContainer(t uint16) bool {
	if def, ok := objTypeRegistry[t]; ok {
		return def.IsIDContainer
	}
	return false
}
