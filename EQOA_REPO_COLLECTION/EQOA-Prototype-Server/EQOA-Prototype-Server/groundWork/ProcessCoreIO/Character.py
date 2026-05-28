from Gear import *
from HotKeys import *
from Hotbars import *
from Abilities import *
import struct, asyncio, logging
from characterSelect import *
from utilities import technique_orig as technique
from utilities import helpPack
import asyncio

#Pull in DB session with class initiation? Or maybe just start with it as None, then pull it in with character Dump
class CharacterData:

  def __init__(self, myRdpComm):
    
    self.myRdpComm       = myRdpComm
    #Maybe this isn't correct
    #self.Gear            = Gear(self.myRdpComm)
    #self.Hotbars        = Hotbars(self.myRdpComm)
    #Maybe this is better? see gatherGear()
    self.inventoryItems  = []
    self.bankItems       = []
    self.weaponHotBars   = []
    self.Hotkeys         = []
    self.mySpells        = []
    #Temporary variable to show if dump has started or not
    self.dumpstarted     = False
    #indicates whether client is ready for information or not
    self.ready           = False
    #Will hold our DB session when needed
    self.session         = None
    #Holds our outgoijng packets
    self.characterPacket = None
    #Should we store character data here? Base stats/maxes, resists, looks, xyz?
    #Character Data
    self.characterID     = None
    self.hairColor       = None
    self.hairLength      = None
    self.hairStyle       = None
    self.faceOption      = None
    self.charName        = None
    self.classIcon       = None
    self.race            = None
    self.totalXP         = None   
    self.debt            = None
    self.breath          = None
    self.tunar           = None
    self.bankTunar       = None
    self.unusedTP        = None
    self.totalTP         = None
    self.world           = None
    self.x               = None
    self.y               = None
    self.z               = None
    self.facing          = None
    self.unknown         = None


    
  def gatherSession(self):
    self.session = aMethod() 
    self.myRdpComm.log.info('  Opening DB Session...')
  
  def closeSession(self):
    #Close the session for DB here
    self.session.close()
    self.myRdpComm.log.info('  Closing DB Session...')
  #
  #
  #
  #
  '''
  This is the Character dump portion of the Character function
  '''
  #
  #
  #
  #    
  #start character memory dump
  #Should we also save physical changes here?
  #Such as hair, face, hair length/color?
  # May be a good idea, or maybe have opcode operations save it first then start a new DB session here
  #Would be called after opcode operations finished saving character data  
  
  #This function saves any changes to database
  def characterSelectSave (self, characterPayload):
    #Unpack selected character/options here
    self.characterID = struct.unpack('<L', characterPayload[:4])[0]
    self.hairColor   = struct.unpack('<L', characterPayload[4:8])[0]
    self.hairLength  = struct.unpack('<L', characterPayload[8:12])[0]
    self.hairStyle   = struct.unpack('<L', characterPayload[12:16])[0]
    self.faceOption  = struct.unpack('<L', characterPayload[16:20])[0]
    #
    self.myRdpComm.log.info(' >>>>>>>>>> Character ID is {}.'.format(hex(self.characterID)))
    #Save this to DB
    #Queries character, then changes options with newly selected options (If changed).
    self.myRdpComm.log.info('Client Character ID is {} and Server Character ID is {}.'.format(hex(self.characterID), hex(technique(self.characterID))))
    thisCharacter = self.session.query(Characters).filter(Characters.serverid == self.characterID)
    thisCharacter.haircolor  = self.hairColor
    thisCharacter.hairlength = self.hairLength
    thisCharacter.hairstyle  = self.hairStyle
    thisCharacter.faceoption = self.faceOption
    #commits changes to DB
    self.session.commit()

  async def characterDump(self, opcode):
    #Prepends our opcodes to the data
    self.characterPacket = struct.pack('<H', opcode)
    #Puts our DB session into self.session
    #Think this should be moved into the opcode portion
    #This would allow opcode to first call the gatherSession (Attach it to Character class)
    #When done using it, would allow the deletion/closing of that session
    self.gatherSession()
    
    #Do stuff to start the dump
    #Modify characterSelect for this, consider changing name to something more relevant then character Select
    await self.gatherCharacterData()
    #Call Hotkey creation
    await self.pullHotKeys()
    #Add quest dump, maybe just pack 0 and return for now.
    #Call gearCreation
    await self.pullGear()
    #Probably delete this
    #self.addGearToDump()
    #
    #Call hotbar creation
    await self.pullWeaponHotbars()
    #Call spell creation
    await self.pullSpells()
    self.thisStuff()
    #Do we need more? Maybe
    #Try this to stop it from blocking "processMessage"
    self.loop = asyncio.get_event_loop()
    self.loop.create_task(self.pullDump())
    self.myRdpComm.log.info('  Done Dumping Character information, sending to Client.....')
    
    
    
    
  async def gatherCharacterData(self):
    #Creates query
    thisCharacter = self.session.query(Characters).filter(Characters.serverid == self.characterID).one()
    #Should we first store these in memory? Then pack them? Seems like a better idea
    #Packs the data/, and characters serverid and Character name
    self.charName    = thisCharacter.charName
    self.level       = technique(thisCharacter.level)
    self.classIcon   = technique(thisCharacter.classIcon)
    self.totalXP     = technique(thisCharacter.totalXP)   
    self.debt        = technique(thisCharacter.debt)      
    self.race        = technique(thisCharacter.race)
    self.breath      = technique(thisCharacter.breath)
    self.tunar       = technique(thisCharacter.tunar)        
    self.bankTunar   = technique(thisCharacter.bankTunar)    
    self.unusedTP    = technique(thisCharacter.unusedTP)     
    self.totalTP     = technique(thisCharacter.totalTP)      
    self.world       = technique(thisCharacter.world)        
    self.x           = thisCharacter.x           
    self.y           = thisCharacter.y            
    self.z           = thisCharacter.z            
    self.facing      = thisCharacter.facing       
    self.unknown     = thisCharacter.unknown      
    self.characterPacket += struct.pack('<BL{}sLL{}sBBB'.format(len('data/tunaria.esf'), len(self.charName)), 0x00, len('data/tunaria.esf'), bytes('data/tunaria.esf', 'utf-8'), technique(self.characterID), len(self.charName), bytes(self.charName, 'utf-8'), self.classIcon, self.race, self.level)
    #Packs totalXP, debt since this is variable by player and it's total length, pack seperately and strip following b'\x00''s
    #Should packet data be saved like this? Seems right imo
    self.characterPacket += helpPack(self.totalXP)
    self.characterPacket += helpPack(self.debt)
    self.characterPacket += struct.pack('<B', self.breath)
    self.characterPacket += helpPack(self.tunar)
    self.characterPacket += helpPack(self.bankTunar)
    self.characterPacket += helpPack(self.unusedTP)
    self.characterPacket += helpPack(self.totalTP)  
    self.characterPacket += helpPack(self.world) 
    self.characterPacket += struct.pack('<LLLLLL', self.x, self.y, self.z, self.facing, self.unknown, 0x00000000)
    
    #character data completed.
    #CharacterID - Done
    #XYZ - Done
    #Class/Race - Done
    #Map name? /tunaria something -Done
    #Gather in character Data here
    #return data

  async def pullHotKeys(self):
    hotKeys = self.session.query(Hotkeys).filter(self.characterID == Hotkeys.charid).all() 
    if len(hotKeys) == 0:
      self.myRdpComm.log.info('Character has no hotkeys')
    else:
      for c, hotkey in enumerate(hotKeys):
        thisHotKey = HotKeys(self.myRdpComm, hotkey)
        await thisHotKey.hotkeyDump()
        self.Hotkeys.append(thisHotKey)
      #Test
      for keys in self.Hotkeys:
        keys.logHotKey()
    
  async def pullGear(self):
    #Should we use session to gather a count for gear in inventory vs. gear in bank? Sounds good.
    inventoryCount = self.session.query(charInventory, itemPattern).filter(self.characterID == charInventory.serverid).filter(charInventory.patternid == itemPattern.patternid).filter(charInventory.location == 1).order_by(charInventory.listnumber.asc()).all()
    if len(inventoryCount) == 0:
      self.myRdpComm.log.info('Character contains no items in inventory')
    else:
      for num, item in enumerate(inventoryCount):
        # 
        self.myRdpComm.log.info('Adding Inventory Gear {} to list instance.'.format(num))
        thisGear = Gear(self.myRdpComm, 1, num, item)
        self.inventoryItems.append(thisGear)
    #
    #Gather items in bank
    bankCount = self.session.query(charInventory, itemPattern).filter(self.characterID == charInventory.serverid).filter(charInventory.patternid == itemPattern.patternid).filter(charInventory.location == 2).order_by(charInventory.listnumber.asc()).all()
    if len(bankCount) == 0:
      self.myRdpComm.log.info('Character contains no bank items')
    else:
      for num, item in enumerate(inventoryCount):
        #
        thisGear = Gear(self.myRdpComm, 2, num, item)
        self.bankItems.append(thisGear)
    #Testing gear list's
    #for gear in self.inventoryItems:
      #gear.logGear()
          
  async def pullWeaponHotbars(self):
    #Queries our Characters weapon hotbars, if any
    wepHotbars = self.session.query(weaponHotBar).filter(weaponHotBar.charid == self.characterID).all()
    self.myRdpComm.log.info('Found {} hotbars.'.format(len(wepHotbars))) 
    if len(wepHotbars) == 0:
      self.myRdpComm.log.info('Character contains no weapon Hotbars')
    else:
      for num, item in enumerate(wepHotbars):
        # 
        self.myRdpComm.log.info('Adding Hotbar {} to list instance. Hotbar name is {}, with IDs {} and {}.'.format(num, item.hotbarname, item.weaponID, item.secondaryID))
        thisHotBar = Hotbars(self.myRdpComm, item)
        self.weaponHotBars.append(thisHotBar)    
        
    #Testing
    #for hotbar in self.weaponHotBars:
      #hotbar.logHotBar()
    

  async def pullSpells(self):
    #Gather spells into instances
    totSpells = self.session.query(Spells, spellPattern).filter(Spells.charid == self.characterID).filter(Spells.spellid == spellPattern.spellid).all()
    if len(totSpells) == 0:
      self.myRdpComm.log.info('Character has no spells')
    else:
      for c, spell in enumerate(totSpells):
        self.myRdpComm.log.info(' Processing Spell {}'.format(c))
        thisSpell = Abilities(self.myRdpComm, spell)
        self.mySpells.append(thisSpell)
      #Test spells
      #for spell in self.mySpells:
        #spell.logSpell()

  async def pullDump(self):
    #This resets our message counters, this transition seems to start a new "session interally", therefore resetting message counters.
    self.myRdpComm.initialSession = True
    self.myRdpComm.myRdpCommStatus.messageReset()
    #Accumulate the packs
    #Get length of hotkeys
    self.characterPacket += struct.pack('<B', len(self.Hotkeys) * 2)
    #Cycle through and add each hotkey to packet
    for hotkey in self.Hotkeys:
      self.characterPacket += hotkey.hotKeyPull()
    #Fake and say no quests
    self.characterPacket += struct.pack('B', 0x00)
    #Gather amount of inventory items
    self.characterPacket += struct.pack('B', len(self.inventoryItems) * 2)
    #Cycle through items in inventory and pack them in
    for gear in self.inventoryItems:
      self.characterPacket += gear.packGear()
    #Weapon hotbars
    for wephotbar in self.weaponHotBars:
      self.characterPacket += wephotbar.hotbarDump()
    #Gather amount of bank items
    self.characterPacket += struct.pack('B', len(self.bankItems) * 2)
    #cycle and pack bank items
    for bankitem in self.bankItems:
      self.characterPacket += bankitem.packGear()
    #Shows end of bank items
    self.characterPacket += struct.pack('B', 0x00)
    #0 items buying on auction
    self.characterPacket += struct.pack('B', 0x00)
    #0 items selling on auction
    self.characterPacket += struct.pack('B', 0x00)
    #total abilities
    self.characterPacket += helpPack(technique(len(self.mySpells)))
    #add abilities
    for mySpell in self.mySpells:
      self.characterPacket += mySpell.packSpell()
   
    #end of spells
    self.characterPacket += struct.pack('B', 0x00) 
    #Dumbing this for now due to uncertainty to what it is
    self.characterPacket += self.theseStats
    
    #Set this to true, so we can send first packet!
    self.dumpstarted     = True
    self.ready = True
    self.myRdpComm.log.info(' >>> Finished packing, length is {}.'.format(len(self.characterPacket)))
    
    #Done creating packet
    #Begin sending to client
    while True:
      #check for sending variable
      if self.ready:
        self.myRdpComm.log.info('  Generating packet for client')
        #Set this back to false. Client will let us know when ready
        self.ready = False
        self.myRdpComm.processedMessages = 1
        self.myRdpComm.outgoing_msg_bundle = True
        self.myRdpComm.outgoing_rdp_report = False
        if len(self.characterPacket) > 1156:
          #Grab our first 1156 bytes of data, let's keep it small
          self.outgoingPacket = self.characterPacket[:1156]
          
          #This removes the bytes we took out from the stored packet
          self.characterPacket = self.characterPacket[1156:]
          self.myRdpComm.log.info('  Placing packet into queue, {} bytes left to process.'.format(len(self.characterPacket)))
          #place into Fa queue, this is a multiple packet message and utilizes FA message type till the end
          self.myRdpComm.myRdpMessages.FA_messageListOut.put_nowait(self.outgoingPacket)
        
        #This should mean the remaining data (End of our dump) is < 1156 bytes
        else:
          self.myRdpComm.log.info('Less then 1025 bytes left')
          #End of the multi message packet utilizes the FB message type
          self.myRdpComm.processedMessages = True
          self.myRdpComm.log.info('Completed processing Dump packets')
          self.myRdpComm.myRdpMessages.FB_messageListOut.put_nowait(self.characterPacket)
          #Is this a good idea? Helps save some memory in the long run
          self.characterPacket = None
          #We should break when done with dump, amkes sense to get 
          #rid of this because it is no longer needed
          break
 
      else:
        #We sleep until we receive the go ahead
        await asyncio.sleep(0)
          
    #This creates our characters C9 (Dummy for now)
    self.createC9()
  #C9 creation for our character
  def createC9(self):
    self.myC9 = bytearray([0x00, 0xc9, 0x01, 0x00, 0x00, 0x40, 0x01, 0x69, 0x9c, 0x0d, 0x41, 0x82, 0x31, 0x74, 0xb5, 0x61, 0x1b, 0x10, 0x1e, 0xc1, 0x5c, 0x97, 0x6a, 0x01, 0xff, 0xc6, 0x98, 0xd8, 0x70, 0x26, 0x80, 0x3f, 0x88, 0x0a, 0xed, 0x80, 0x58, 0x42, 0x02, 0xef, 0x24, 0xc0, 0x61, 0xff, 0xff, 0xff, 0xff, 0x01, 0x05, 0x88, 0x02, 0x96, 0x7a, 0x4e, 0xb4, 0xd3, 0x35, 0x27, 0x94, 0x87, 0x10, 0x01, 0x01, 0x04, 0x04, 0x04, 0x04, 0x04, 0x91, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x23, 0xe0, 0x40, 0xff, 0x01, 0x22, 0x40, 0xff, 0x23, 0x40, 0xff, 0x51, 0x51, 0x0f, 0x0d, 0xff, 0x03, 0x22, 0x40, 0xff, 0x41, 0xff, 0xff, 0xff, 0xff, 0x61, 0xff, 0xff, 0xff, 0xff, 0x06, 0x03, 0x11, 0x03, 0x8b, 0x06, 0x44, 0x61, 0x6c, 0x6c, 0x61, 0x73, 0x64, 0x65, 0x76, 0x69, 0x6e, 0x3d, 0x3c, 0x01, 0x02, 0x45, 0xff, 0xff, 0xff, 0xff, 0x11, 0x15, 0x12, 0x01, 0x41, 0x74, 0x73, 0x72, 0x71, 0x00])
    
    #Need to create a special queue for C9 objects, as these have unique message counters for themselves.
    #Something like
    self.myRdpComm.myRdpMessages.C9_messageListOut.put_nowait(self.myC9)
    
  def thisStuff(self):
  
    self.theseStats = bytearray([0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x43, 22, 0x22, 0x1a, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xc6, 0xaf, 0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x48, 0x16, 0x4b, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0xe8, 0x0c, 0xec, 0xbf, 0x87, 0x08, 0x06, 0x22, 0x22, 0x1a, 0x41, 0x00, 0x1e, 0x64, 0x32, 0x00, 0x1e, 0x00, 0x00, 0xfa, 0x01, 0x00, 0x32, 0x00, 0xde, 0x02, 0x00, 0xe8, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x31, 0x00, 0x00, 0x00, 0x3c, 0x00, 0xfa, 0x01, 0x00, 0x32, 0x00, 0xde, 0x02, 0x00])

#
#
#
#
'''
Eventual Character functions for opcodes.
Thinking swapping gear around, hotbar deletion, creation, spells, etc
'''
#
#
#
#
