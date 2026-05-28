from utilities import hotKeyOut, hotKeyIn, s2c, helpPack
from utilities import technique_orig as technique
import struct


class HotKeys:

  def __init__(self, myRdpComm, hotkey):
    self.myRdpComm      = myRdpComm
    #Hotkey dict for directions
    self.hotKeyOut      = hotKeyOut
    self.hotkey         = hotkey
    self.direction      = self.hotKeyOut.get(self.hotkey.direction)
    self.Nlabel         = self.hotkey.Nlabel
    self.Nmessage       = self.hotkey.Nmessage
    self.Wlabel         = self.hotkey.Wlabel
    self.Wmessage       = self.hotkey.Wmessage
    self.Elabel         = self.hotkey.Elabel
    self.Emessage       = self.hotkey.Emessage
    self.Slabel         = self.hotkey.Slabel
    self.Smessage       = self.hotkey.Smessage


  #Handles packing our hotkeys up
  #Seemed cleaner in it's own function    
  async def hotkeyDump(self):
    #Pack North messages
    if self.Nlabel == None and self.Nmessage == None:
      self.North = struct.pack('<BLL', 0, 0, 0)
    elif self.Nlabel == None and self.Nmessage != None:
      self.North = struct.pack('<BL{}sL'.format(len(self.Nmessage) * 2), 0, len(self.Nmessage), self.Nmessage.encode('utf-16-le'), 0)
    elif self.Nlabel != None and self.Nmessage == None:
      self.North = struct.pack('<BLL{}s'.format(len(self.Nlabel) * 2), 0, 0, len(self.Nlabel), self.Nlabel.encode('utf-16-le')) 
    else:
      self.North = struct.pack('<BL{}sL{}s'.format(len(self.Nmessage)*2, len(self.Nlabel)*2), 0, len(self.Nmessage), self.Nmessage.encode('utf-16-le'), len(self.Nlabel), self.Nlabel.encode('utf-16-le'))

    #Pack West messages
    if self.Wlabel == None and self.Wmessage == None:
      self.West = struct.pack('<BLL', 2, 0, 0)
    elif self.Wlabel == None and self.Wmessage != None:
      self.West = struct.pack('<BL{}sL'.format(len(self.Wmessage) * 2), 2, len(self.Wmessage), self.Wmessage.encode('utf-16-le'), 0)
    elif self.Wlabel != None and self.Wmessage == None:
      self.West = struct.pack('<BLL{}s'.format(len(self.Wlabel) * 2), 2, 0, len(self.Wlabel), self.Wlabel.encode('utf-16-le'))
    else:
      self.West = struct.pack('<BL{}sL{}s'.format(len(self.Wmessage)*2, len(self.Wlabel)*2), 2, len(self.Wmessage), self.Wmessage.encode('utf-16-le'), len(self.Wlabel), self.Wlabel.encode('utf-16-le'))

    #Pack East messages
    if self.Elabel == None and self.Emessage == None:
      self.East = struct.pack('<BLL', 4, 0, 0)
    elif self.Elabel == None and self.Emessage != None:
      self.East = struct.pack('<BL{}sL'.format(len(self.Emessage) * 2), 4, len(self.Emessage), self.Emessage.encode('utf-16-le'), 0)
    elif self.Elabel != None and self.Emessage == None:
      self.East = struct.pack('<BLL{}s'.format(len(self.Elabel) * 2), 4, 0, len(self.Elabel), self.Elabel.encode('utf-16-le'))
    else:
      self.East = struct.pack('<BL{}sL{}s'.format(len(self.Emessage)*2, len(self.Elabel)*2), 4, len(self.Emessage), self.Emessage.encode('utf-16-le'), len(self.Elabel), self.Elabel.encode('utf-16-le'))

    #Pack South messages
    if self.Slabel == None and self.Smessage == None:
      self.South = struct.pack('<BLL', 6, 0, 0)
    elif self.Slabel == None and self.Smessage != None:
      self.South = struct.pack('<BL{}sL'.format(len(self.Smessage) * 2), 6, len(self.Smessage), self.Smessage.encode('utf-16-le'), 0)
    elif self.Slabel != None and self.Smessage == None:
      self.South = struct.pack('<BLL{}s'.format(len(self.Slabel) * 2), 6, 0, len(self.Slabel), self.Slabel.encode('utf-16-le'))
    else:
      self.South = struct.pack('<BL{}sL{}s'.format(len(self.Smessage)*2, len(self.Slabel)*2), 6, len(self.Smessage), self.Smessage.encode('utf-16-le'), len(self.Slabel), self.Slabel.encode('utf-16-le'))

  def logHotKey(self):
    self.myRdpComm.log.info(' >> Cycling through Hotkey: {}'.format(hotKeyIn.get(self.direction)))
    self.myRdpComm.log.info(' >>>> North is Message {} and Label {}.'.format(self.Nmessage, self.Nlabel))
    self.myRdpComm.log.info(' >>>> West is  Message {} and Label {}.'.format(self.Wmessage, self.Wlabel))
    self.myRdpComm.log.info(' >>>> East is  Message {} and Label {}.'.format(self.Emessage, self.Elabel))
    self.myRdpComm.log.info(' >>>> South is Message {} and Label {}.'.format(self.Smessage, self.Slabel))
    
  def hotKeyPull(self):
    self.outgoing = helpPack(self.direction) + self.North + self.West + self.East + self.South
    return self.outgoing






