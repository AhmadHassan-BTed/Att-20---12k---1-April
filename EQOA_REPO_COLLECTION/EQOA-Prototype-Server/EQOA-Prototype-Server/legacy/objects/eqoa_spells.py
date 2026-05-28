'''
Created December 13th, 2016

@author: Devin Dallas
'''
import eqoa_utilities


INHOTBAR = eqoa_utilities.enum(YES = 1,
                               NO  = 0)

HOTBAR_LOCATION = eqoa_utilities.enum( NONE           = - 1,
                                       BAR1_SLOT1     =   0,
                                       BAR1_SLOT2     =   1,
                                       BAR1_SLOT3     =   2,
                                       BAR1_SLOT4     =   3,
                                       BAR1_SLOT5     =   4,
                                       BAR2_SLOT1     =   5,
                                       BAR2_SLOT2     =   6,
                                       BAR2_SLOT3     =   7,
                                       BAR2_SLOT4     =   8,
                                       BAR2_SLOT5     =   9)                           


SHOWORNOT = eqoa_utilities.enum(SHOW = 1,
                                HIDE = 0)

class Spell():
    """Process of gathering unique spell informaion"""
    
    def __init__(self, booklocation, inhotbar, hotbarlocation, unknown, showornot, pattern):
      #
      self.booklocation    = booklocation
      self.inhotbar        = inhotbar
      self.hotbarlocation  = hotbarlocation
      self.unknown         = unknown
      self.showornot       = showornot
      self.pattern         = pattern
      
        
    def encodeSpell(self):
      #    
      # Encode spell information
      #
      packingList = [ self.pattern.spell_id,
                      self.booklocation,      
                      self.inhotbar,      
                      self.hotbarlocation,
                      self.unknown,
                      self.showornot]
      #
      encodedSpellString = eqoa_utilities.packVariable(packingList)
      #    
      # Probably have to encode pattern by calling pattern encoding from here
      #   
      encodedSpellString += self.pattern.encodeSpellPattern()   
      #              

      return encodedSpellString;
  
  
    def printSpell(self):
      #    
        print 
        print ' BOOK_LOCATION   :    {:8d}'.format(self.booklocation)
        print ' INHOTBAR        :    {:8d}'.format(self.inhotbar)
        print ' HOTBAR_LOCATION :    {:8x}'.format(self.hotbarlocation)
        print ' UNKNOWN         :    {:8x}'.format(self.unknown)
        print ' SHOWORNOT       :    {:8d}'.format(self.showornot)
        print ' PATTERN         :    {:8d}'.format(self.pattern)
        print 
      

class spellPattern():

    def __init__(self, spell_id, level, unknown1, unknown2, range, casttime, power,
    iconcolor, icon, scope, recast, spellreq, spell_name, spell_desc):
    
        self.spell_id      = spell_id
        self.level         = level
        self.unknown1      = unknown1
        self.unknown2      = unknown2
        self.range         = range
        self.casttime      = casttime
        self.power         = power
        self.iconcolor     = iconcolor
        self.icon          = icon
        self.scope         = scope
        self.recast        = recast
        self.spellreq      = spellreq
        self.spell_name    = spell_name
        self.spell_desc    = spell_desc



    def encodeSpellPattern(self):

        packingList = [self.level, 
                       self.unknown1,
                       self.unknown2,
                       self.range, 
                       self.casttime, 
                       self.power, 
                       self.iconcolor, 
                       self.icon, 
                       self.scope, 
                       self.recast, 
                       self.spellreq]
                       
        #              
        encodedSpellPattern = eqoa_utilities.packVariable(packingList)
        #
        packingList = [self.spell_name,
                       self.spell_desc]       
      
        encodedSpellPattern += eqoa_utilities.packStringAsUnicode(packingList)
        #                  
        packingList = [0x00]  # NULL TERMINATOR
        #
        encodedSpellPattern += eqoa_utilities.packVariable(packingList)      
        return encodedSpellPattern
        
    def printSpell(self):
      #    
        print 
        print ' LEVEL           :    {:8x}'.format(self.level)
        print ' UNKNOWN1        :    {:8x}'.format(self.unknown1)
        print ' UNKNONW2        :    {:8x}'.format(self.unknown2)
        print ' RANGE           :    {:8x}'.format(self.range)
        print ' CASTTIME        :    {:8x}'.format(self.casttime)
        print ' POWER           :    {:8x}'.format(self.power)
        print ' ICONCOLOR       :    {:8x}'.format(self.iconcolor)
        print ' ICON            :    {:8x}'.format(self.icon)
        print ' SCOPE           :    {:8x}'.format(self.scope)
        print ' RECAST          :    {:8x}'.format(self.recast)
        print ' SPELLREQ        :    {:8x}'.format(self.spellreq)
        print ' SPELL_NAME      :    {:s}'.format(self.spell_name)
        print ' SPELL_DESC      :    {:s}'.format(self.spell_desc)
        print 

      