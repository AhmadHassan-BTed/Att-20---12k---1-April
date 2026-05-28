import sys, struct, asyncio, logging
from characterSelect import *
from utilities import technique_orig as technique
sys.path.insert(0, '../loginserver2/')
from eqoa_BaseDBsetup import get_db_session 
from eqoa_Account import *

#
##########################################################################################
#
class createCharacterList:
  def __init__(self, myRdpComm):
    self.session       = aMethod() 
    self.myRdpComm     = myRdpComm
    self.robe          = 0x00000000
    self.Primary       = 0x00000000
    self.Secondary     = 0x00000000
    self.shieldGraphic = 0x00000000
    self.toonAnimation = 0x0000
    self.chest         = 0x00
    self.bracer        = 0x00
    self.glove         = 0x00
    self.pants         = 0x00
    self.boot          = 0x00
    self.helm          = 0x00
    self.chestColor    = 0xFFFFFFFF
    self.bracerColor   = 0xFFFFFFFF
    self.gloveColor    = 0xFFFFFFFF
    self.pantsColor    = 0xFFFFFFFF
    self.bootColor     = 0xFFFFFFFF
    self.helmColor     = 0xFFFFFFFF
    self.robecolor     = 0xFFFFFFFF

  def resetVariables(self):
    self.robe          = 0x00000000
    self.Primary       = 0x00000000
    self.Secondary     = 0x00000000
    self.shieldGraphic = 0x00000000
    self.toonAnimation = 0x0000
    self.chest         = 0x00
    self.bracer        = 0x00
    self.glove         = 0x00
    self.pants         = 0x00
    self.boot          = 0x00
    self.helm          = 0x00
    self.chestColor    = 0xFFFFFFFF
    self.bracerColor   = 0xFFFFFFFF
    self.gloveColor    = 0xFFFFFFFF
    self.pantsColor    = 0xFFFFFFFF
    self.bootColor     = 0xFFFFFFFF
    self.helmColor     = 0xFFFFFFFF
    self.robecolor     = 0xFFFFFFFF
    
  async def createCharacters(self):
    
    self.myRdpComm.log.info('    Using AccountID {} to request Characters....'.format(self.myRdpComm.mySession.accountID))
    #Collect total Characters
    totCharacters = self.session.query(Characters).filter(Characters.accountid == self.myRdpComm.mySession.accountID)
    #Uses technique to "Double" value and pack into message
    thisMessage = struct.pack('<B', technique(totCharacters.count()))
    for character in totCharacters:
      #Adds Character Name, serverID, modelID, class, race, level, hair color
      #hair length, hair style and face option
      thisMessage += struct.pack('<L{}s'.format(len(character.charName)), len(character.charName), bytes(character.charName, 'utf-8'))
      #Does this work better?
      self.myRdpComm.log.info('Character name is {} and serverid is {}.'.format(character.charName, hex(technique(character.serverid))))
      thisMessage += struct.pack('<L', technique(character.serverid))

      #Place ModelID conversion here
      thisMessage += struct.pack('<q', technique(character.modelid))[0:5]
   
      #Remaining Character information
      thisMessage += struct.pack('<BBBBBBB', technique(character.tclass), technique(character.race), technique(character.level), technique(character.haircolor), technique(character.hairlength), technique(character.hairstyle), technique(character.faceoption)) 

      #This queries for player gear
      charGear = self.session.query(charInventory, itemPattern).filter(character.serverid == charInventory.serverid).filter(charInventory.patternid == itemPattern.patternid).filter(charInventory.equiploc > 0) 

      #Loop over gear getting appropriate information
      for gear in charGear:
        self.myRdpComm.log.info('Processing.... {}.'.format(gear.itemPattern.itemname))  

        #Helm
        if gear.charInventory.equiploc == 1: 
            self.helm = gear.itemPattern.model
            self.helmColor = gear.itemPattern.color
            #print("Helm: {}".format(hex(self.helm)))

        #Robe
        if gear.charInventory.equiploc == 2:
            self.robe = gear.itemPattern.model
            self.robeColor = gear.itemPattern.color
            #print("Robe: {}".format(hex(self.robe)))
            #self.myRdpComm.log.info('Robe color is {}.'.format(struct.pack('>L', self.robeColor)))

        #Chest
        if gear.charInventory.equiploc == 5:
            self.chest = gear.itemPattern.model
            self.chestColor = gear.itemPattern.color
            #print("Chest: {}".format(hex(self.chest))) 

        #Gloves
        if gear.charInventory.equiploc == 19:
            self.glove = gear.itemPattern.model
            self.gloveColor = gear.itemPattern.color
            #print("Gloves: {}".format(hex(self.glove))) 
            #self.myRdpComm.log.info(' Glove color is {}.'.format(hex(self.gloveColor)))
 
        #Bracers
        if gear.charInventory.equiploc == 6:
            self.bracer = gear.itemPattern.model
            self.bracerColor = gear.itemPattern.color
            #print("Bracers: {}".format(hex(self.bracer))) 

        #Pants
        if gear.charInventory.equiploc == 10:
            self.pants = gear.itemPattern.model
            self.pantsColor = gear.itemPattern.color
            #print("Pants: {}".format(hex(self.pants)))

        #Feet
        if gear.charInventory.equiploc == 11:
            self.feet = gear.itemPattern.model
            self.feetColor = gear.itemPattern.color
            #print("Feet: {}".format(hex(self.feet)))
            #self.myRdpComm.log.info('Boot color is {}.'.format(hex(self.feetColor)))

        #Primary
        if gear.charInventory.equiploc == 12:
            self.Primary = gear.itemPattern.model
            #print("PrimaryWep Location Set")

        #Shield
        if gear.charInventory.equiploc == 13:
            self.shield = gear.itemPattern.model
            self.shieldColor = gear.itemPattern.color
            #print("Shield Location Set")

        #Secondary
        if gear.charInventory.equiploc == 14:
            self.Secondary = gear.itemPattern.model
            #print("SecondaryWep Location Set")

        #2Hand
        if gear.charInventory.equiploc == 15:
            self.Primary = gear.itemPattern.model
            #print("2HandWep Location Set")

        #Bow
        if gear.charInventory.equiploc == 16:
            self.Primary = gear.itemPattern.model
            #print("BowWep Location Set") 

        #Thrown
        if gear.charInventory.equiploc == 17:
            self.Primary = gear.itemPattern.model
            #print("ThrowWep Location Set") 

        #Held
        if gear.charInventory.equiploc == 18:
            self.held = gear.itemPattern.model
            #print("HeldItem Location Set")
      
      #This packs in armour types and weapons     
      thisMessage += struct.pack('<LLLLHBBBBBBBHL', self.robe, self.Primary, self.Secondary, self.shieldGraphic, self.toonAnimation, 0x00, self.chest, self.bracer, self.glove, self.pants, self.boot, self.helm, 0x00, 0x00) 

      #This packs in all the colors
      gearColors = struct.pack('>LLLLLLLLLL', 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, self.chestColor, self.bracerColor, self.gloveColor, self.pantsColor, self.bootColor, self.helmColor, self.robeColor)
      #self.myRdpComm.log.info('Colors are {}.'.format(gearColors))
      thisMessage += gearColors
      #Resets for next character
      self.resetVariables() 

    
    opcode = 0x002C
    myPacket = struct.pack('<H', opcode) + thisMessage
    self.myRdpComm.myRdpMessages.FB_messageListOut.put_nowait(myPacket)
    self.myRdpComm.outgoing_msg_bundle = True
    self.myRdpComm.outgoing_rdp_report = False
    self.myRdpComm.processedMessages   = 1  
    self.myRdpComm.log.info('Completed collecting character and closing DB session')
    self.session.close()    

