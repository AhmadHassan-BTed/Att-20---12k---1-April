from characterSelect import *
from utilities import technique_orig as technique
from utilities import helpPack
import struct
#test
from utilities import s2c


class Gear:

  def __init__(self, myRdpComm, Location, itemLocation, myItem):
    self.myRdpComm     = myRdpComm
    self.itemLocation  = itemLocation #Actual order items are stored in locations
    self.Location      = Location
    self.myItem        = myItem
    #Store gears attributes here
    #Unique identifier for each piece of gear. This allows me to know player A owns itemid 1 of said staff, while player B owns itemid2 of same staff
    self.itemid        = myItem.charInventory.itemid
    #Do we need this? Characters ServerID that owns the gear. Seems kinda irrelevant.
    self.serverid      = myItem.charInventory.serverid
    self.stackleft     = myItem.charInventory.stackleft
    self.remainHP      = myItem.charInventory.remainHP
    self.remaincharge  = myItem.charInventory.remaincharge
    self.equiploc      = myItem.charInventory.equiploc
    self.location      = myItem.charInventory.location
    self.listnumber    = myItem.charInventory.listnumber
    self.patternid     = myItem.itemPattern.patternid
    self.patternfam    = myItem.itemPattern.patternfam
    self.unk1          = myItem.itemPattern.unk1
    self.itemicon      = myItem.itemPattern.itemicon
    self.unk2          = myItem.itemPattern.unk2
    self.equipslot     = myItem.itemPattern.equipslot
    self.unk3          = myItem.itemPattern.unk3
    self.trade         = myItem.itemPattern.trade
    self.rent          = myItem.itemPattern.rent
    self.unk4          = myItem.itemPattern.unk4
    self.attacktype    = myItem.itemPattern.attacktype
    self.weapondamage  = myItem.itemPattern.weapondamage
    self.unk5          = myItem.itemPattern.unk5
    self.levelreq      = myItem.itemPattern.levelreq
    self.maxstack      = myItem.itemPattern.maxstack
    self.maxhp         = myItem.itemPattern.maxhp
    self.duration      = myItem.itemPattern.duration
    self.classuse      = myItem.itemPattern.classuse
    self.raceuse       = myItem.itemPattern.raceuse
    self.procanim      = myItem.itemPattern.procanim
    self.lore          = myItem.itemPattern.lore
    self.unk6          = myItem.itemPattern.unk6
    self.craft         = myItem.itemPattern.craft
    self.itemname      = myItem.itemPattern.itemname
    self.itemdesc      = myItem.itemPattern.itemdesc
    self.model         = myItem.itemPattern.model
    self.color         = myItem.itemPattern.color    
    #Don't pack this now... need true values for character stats
    self.str           = myItem.itemPattern.str
    self.sta           = myItem.itemPattern.sta
    self.agi           = myItem.itemPattern.agi
    self.dex           = myItem.itemPattern.dex
    self.wis           = myItem.itemPattern.wis
    self.int           = myItem.itemPattern.int
    self.cha           = myItem.itemPattern.cha
    self.hpmax         = myItem.itemPattern.HPMAX
    self.powmax        = myItem.itemPattern.POWMAX
    self.PoT           = myItem.itemPattern.PoT
    self.HoT           = myItem.itemPattern.HoT
    self.AC            = myItem.itemPattern.AC
    self.PR            = myItem.itemPattern.PR
    self.DR            = myItem.itemPattern.DR
    self.FR            = myItem.itemPattern.FR
    self.CR            = myItem.itemPattern.CR
    self.LR            = myItem.itemPattern.LR
    self.AR            = myItem.itemPattern.AR
    self.stats         = b''
    self.packStats()
 
  def logGear(self):
    #
    self.myRdpComm.log.info(' >>>> Cycling through {}. Has {} stats.'.format(self.itemname, s2c(struct.unpack('<B', self.stats[:1])[0])))
  
  def packGear(self):
    self.myGear = helpPack(technique(self.stackleft)) + helpPack(technique(self.remainHP)) + helpPack(technique(self.remaincharge)) + helpPack(technique(self.equiploc)) + helpPack(technique(self.location)) + struct.pack('<L', self.listnumber) + helpPack(technique(self.patternid )) + helpPack(technique(self.patternfam )) + helpPack(technique(self.unk1)) + helpPack(technique(self.itemicon)) + helpPack(technique(self.unk2)) + helpPack(technique(self.equipslot)) + helpPack(technique(self.unk3)) + helpPack(technique(self.trade)) + helpPack(technique(self.rent)) + helpPack(technique(self.unk4)) + helpPack(technique(self.attacktype)) + helpPack(technique(self.weapondamage)) + helpPack(technique(self.unk5)) + helpPack(technique(self.levelreq)) + helpPack(technique(self.maxstack)) + helpPack(technique(self.maxhp)) + helpPack(technique(self.duration)) + helpPack(technique(self.classuse)) + helpPack(technique(self.raceuse)) + helpPack(technique(self.procanim)) + helpPack(technique(self.lore)) + helpPack(technique(self.unk6)) + helpPack(technique(self.craft)) + struct.pack('<L{}s'.format(len(self.itemname) * 2), len(self.itemname), self.itemname.encode('utf-16-le')) + struct.pack('<L{}s'.format(len(self.itemdesc) * 2), len(self.itemdesc), self.itemdesc.encode('utf-16-le')) + helpPack(technique(self.model)) + struct.pack('<L', self.color) + self.stats
                
    return self.myGear


  def packStats(self):
    stats = 0
    if self.str:
      stats += 1
      self.stats += struct.pack('<B', 0) + helpPack(technique(self.str))
      self.myRdpComm.log.info(' Packing {} Strength'.format(self.str))

    if self.sta:
      stats += 1
      self.stats += struct.pack('<B', 2) + helpPack(technique(self.sta))
      self.myRdpComm.log.info(' Packing {} Stamina'.format(self.sta))
    
    if self.agi:
      stats += 1
      self.stats += struct.pack('<B', 4) + helpPack(technique(self.agi))
      self.myRdpComm.log.info(' Packing {} Agility'.format(self.agi))

    if self.dex:
      stats += 1
      self.stats += struct.pack('<B', 6) + helpPack(technique(self.dex))
      self.myRdpComm.log.info(' Packing {} Dexterity'.format(self.dex))
   
    if self.wis:
      stats += 1
      self.stats += struct.pack('<B', 8) + helpPack(technique(self.wis))
      self.myRdpComm.log.info(' Packing {} Wisdom'.format(self.wis))

    if self.int:
      stats += 1
      self.stats += struct.pack('<B', 10) + helpPack(technique(self.int))
      self.myRdpComm.log.info(' Packing {} Intelligence'.format(self.int))

    if self.cha:
      stats += 1
      self.stats += struct.pack('<B', 12) + helpPack(technique(self.cha))
      self.myRdpComm.log.info(' Packing {} Charisma'.format(self.cha))

    if self.hpmax:
      stats += 1
      self.stats += struct.pack('<B', 16) + helpPack(technique(self.hpmax))
      self.myRdpComm.log.info(' Packing {} HPMAX'.format(self.hpmax))

    if self.powmax:
      stats += 1
      self.stats += struct.pack('<B', 20) + helpPack(technique(self.powmax))
      self.myRdpComm.log.info(' Packing {} POWMAX'.format(self.powmax))

    if self.PoT:
      stats += 1
      self.stats += struct.pack('<B', 24) + helpPack(technique(self.PoT))
      self.myRdpComm.log.info(' Packing {} PoT'.format(self.PoT))

    if self.HoT:
      stats += 1
      self.stats += struct.pack('<B', 26) + helpPack(technique(self.HoT))
      self.myRdpComm.log.info(' Packing {} HoT'.format(self.HoT))

    if self.AC:
      stats += 1
      self.stats += struct.pack('<B', 28) + helpPack(technique(self.AC))
      self.myRdpComm.log.info(' Packing {} AC'.format(self.AC))

    if self.PR:
      stats += 1
      self.stats += struct.pack('<B', 44) + helpPack(technique(self.PR))
      self.myRdpComm.log.info(' Packing {} PR'.format(self.PR))

    if self.DR:
      stats += 1
      self.stats += struct.pack('<B', 46) + helpPack(technique(self.DR))
      self.myRdpComm.log.info(' Packing {} DR'.format(self.DR))

    if self.FR:
      stats += 1
      self.stats += struct.pack('<B', 48) + helpPack(technique(self.FR))
      self.myRdpComm.log.info(' Packing {} FR'.format(self.FR))

    if self.CR:
      stats += 1
      self.stats += struct.pack('<B', 50) + helpPack(technique(self.CR))
      self.myRdpComm.log.info(' Packing {} CR'.format(self.CR))

    if self.LR:
      stats += 1
      self.stats += struct.pack('<B', 52) + helpPack(technique(self.LR))
      self.myRdpComm.log.info(' Packing {} LR'.format(self.LR))

    if self.AR:
      stats += 1
      self.stats += struct.pack('<B', 54) + helpPack(technique(self.AR))
      self.myRdpComm.log.info(' Packing {} AR'.format(self.AR))


    self.stats = struct.pack('<B', technique(stats)) + self.stats
    




