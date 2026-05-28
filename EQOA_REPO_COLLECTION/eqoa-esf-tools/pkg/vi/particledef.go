// VIParticleDefinition — particle effect template.
// 1,056 in TUNARIA, 332 in SPELLFX. Fire, smoke, sparkles.
//
// PS2 ParseParticleDefinitionObj at 0x0043C830.
// Transpiler trace: 2 reads (dictID from header child).
package vi

// VIParticleDefinition defines a particle system template.
type VIParticleDefinition struct {
	DictID uint32
	// Particle emitter config lives in children (0xC010, 0xC020)
	// parsed by ParticleEmitter sub-parsers
}

const TypeParticleDefinition = 0xC000
