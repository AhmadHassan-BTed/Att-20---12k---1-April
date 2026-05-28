import struct
from characterSelect import *
from utilities import technique_orig as technique
from utilities import helpPack

class Hotbars:

  def __init__(self, myRdpComm, myHotbar):
    self.myRdpComm      = myRdpComm
    self.myHotbar       = myHotbar
    self.charid         = self.myHotbar.charid
    self.hotbarname     = self.myHotbar.hotbarname
    self.weaponID       = self.myHotbar.weaponID
    self.secondaryID    = self.myHotbar.secondaryID
    
  #Don't think we need this. This should be a super easy/straight forward to maintain  
  def hotbarDump(self):
    thisbar = helpPack(technique(self.weaponID)) + helpPack(technique(self.secondaryID)) + struct.pack('<L{}s'.format(len(self.hotbarname) * 2), len(self.hotbarname), self.hotbarname.encode('utf-16-le')) 
    return thisbar
  #Functions we would need for this
  #Add hotbar
  #Remove Hotbar
  #Should be it, will be a log tester, also.
  #This could change depending on information learned later on about weapon hotbars
  def logHotBar(self):
    self.myRdpComm.log.info(' >>>>>> Hotbar ({}) processed.'.format(self.hotbarname))
