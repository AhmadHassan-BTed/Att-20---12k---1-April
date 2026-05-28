import struct
from characterSelect import *
from utilities import technique_orig as technique
from utilities import helpPack

class Abilities:

  def __init__(self, myRdpComm, spell):
    self.myRdpComm      = myRdpComm
    self.spell          = spell
    self.charid         = self.spell.Spells.charid
    self.addedorder     = self.spell.Spells.addedorder
    self.onHotBar       = self.spell.Spells.onHotBar
    self.whereonBar     = self.spell.Spells.whereonBar
    self.unk1           = self.spell.Spells.unk1
    self.showhide       = self.spell.Spells.showhide
    self.abilitylvl     = self.spell.spellPattern.abilitylvl
    self.unk2           = self.spell.spellPattern.unk2
    self.unk3           = self.spell.spellPattern.unk3
    self.range          = self.spell.spellPattern.range
    self.casttime       = self.spell.spellPattern.casttime
    self.power          = self.spell.spellPattern.power
    self.iconColor      = self.spell.spellPattern.iconColor
    self.icon           = self.spell.spellPattern.icon
    self.scope          = self.spell.spellPattern.scope
    self.recast         = self.spell.spellPattern.recast
    self.requirement    = self.spell.spellPattern.eqprequire
    self.spellname      = self.spell.spellPattern.spellname
    self.spelldesc      = self.spell.spellPattern.spelldesc
    
  def logSpell(self):
    self.myRdpComm.log.info('Processed {} ability/spell.'.format(self.spellname))
    
  def packSpell(self):
    self.thisSpell = helpPack(technique(self.charid)) + helpPack(technique(self.addedorder)) + helpPack(technique(self.onHotBar)) + helpPack(technique(self.whereonBar)) + helpPack(technique(self.unk1)) + helpPack(technique(self.showhide)) + helpPack(technique(self.abilitylvl)) + helpPack(technique(self.unk2)) + helpPack(technique(self.unk3)) + struct.pack('<f', self.range).split(b'\x00', maxsplit = 2)[2] + helpPack(technique(self.casttime)) + helpPack(technique(self.power)) + helpPack(technique(self.iconColor)) + helpPack(technique(self.icon)) + helpPack(technique(self.scope)) + helpPack(technique(self.recast)) + helpPack(technique(self.requirement)) + struct.pack('<L{}s'.format(len(self.spellname)), len(self.spellname) * 2, self.spellname.encode('utf-16-le')) + struct.pack('<L{}s'.format(len(self.spelldesc)), len(self.spelldesc) * 2, self.spelldesc.encode('utf-16-le'))

    return self.thisSpell
