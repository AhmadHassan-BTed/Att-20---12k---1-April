'''
Created on December 24th, 2016

@author:  Devin Dallas
'''

import eqoa_utilities

class weaponBar():

    def __init__(self, primaryHandGear, secondaryHandGear, weaponSetName):
        
        self.primaryHandGear      = primaryHandGear
        self.secondaryHandGear    = secondaryHandGear
        self.weaponSetName        = weaponSetName       
        
    def encodeWeaponBar(self):
        #
        packingList = [ self.primaryHandGear,       # Gear for Primary Hand
                        self.secondaryHandGear]     # Gear for Secondary Hand
                                     
        #
        encodedWeaponBar = eqoa_utilities.packVariable(packingList)
        #    
        packingList = [self.weaponSetName]  
        #
        encodedWeaponBar += eqoa_utilities.packStringAsUnicode(packingList)
        #
        return encodedWeaponBar;
        
    def printWeaponBar(self):
        #
        print
        print 'GearID for Primary    : 0x{:8X}'.format(self.primaryHandGear)
        print 'GearID for Secondary  : 0x{:8X}'.format(self.secondaryHandGear)
        print 'Name of Weapon Set    : {:s}'.format(self.weaponSetName)
        print

#
#
#        
class weaponBarList():

    def __init__(self, myWeaponBarList):
        self.myWeaponBarList = myWeaponBarList
      
    def encodeWeaponBarList(self):
      #         
      encodedWeaponBarList = ''
      for wbl in self.myWeaponBarList:
        encodedWeaponBarList += wbl.encodeWeaponBar()
      #  
      return encodedWeaponBarList
         
         
    def printWeaponBarList(self):
      #  
      print 'Weapon Set List'
      print '------------------------------'   
      for i,wbl in enumerate(self.myWeaponBarList):
        print 'Weapon Set Number     : {:8d}'.format(i+1)
        print 'GearID for Primary    : 0x{:08X}'.format(wbl.primaryHandGear)
        print 'GearID for Secondary  : 0x{:08X}'.format(wbl.secondaryHandGear)
        print 'Name of Weapon Set    : {:s}'.format(wbl.weaponSetName)
        print        