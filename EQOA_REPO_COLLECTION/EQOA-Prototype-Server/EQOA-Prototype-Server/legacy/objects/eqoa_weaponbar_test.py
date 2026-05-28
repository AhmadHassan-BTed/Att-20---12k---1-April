'''
Created on December 24th, 2016

@author:  Devin Dallas
'''

import unittest
import eqoa_weaponbar

class weaponBarTest(unittest.TestCase):

    def test_weaponBar(self):
    
        expectedMessageBytes     = [0xB0, 0x82, 0x04, 0xA8, 0xA0, 0x01, 0x09, 0x00, 0x00, 0x00, 0x4B, 0x00, 0x68, 0x00, 0x61, 0x00, 0x6C, 0x00, 0x20, 0x00, 0x45, 0x00, 0x70, 0x00, 0x69, 0x00, 0x63, 0x00]
        expectedMessageByteArray = "".join( chr( val) for val in expectedMessageBytes)  # Converts to byte array vs list.          
        
        primaryHandGear     = 32920
        secondaryHandGear   = 10260
        weaponSetName       = 'Khal Epic'

        #
        t = eqoa_weaponbar.weaponBar(primaryHandGear, secondaryHandGear, weaponSetName)
        #
        #t.printweaponBar()
        #
        #
        # Decodes weapon bar object and compares with the expectedMessageByteArray
        #
        receivedMessageByteArray = t.encodeWeaponBar()
        #
        #print
        #print "CALCD : "+ " ".join("{:02X}".format(ord(c)) for c in receivedMessageByteArray)
        #print "EXPECT: "+ " ".join("{:02X}".format(ord(c)) for c in expectedMessageByteArray)
        #
        self.assertEquals(receivedMessageByteArray,expectedMessageByteArray)        

    def test_weaponBarList(self):
        #
        expectedMessageBytes = []
        expectedMessageBytes += [0xB0, 0x82, 0x04, 0xA8, 0xA0, 0x01, 0x09, 0x00, 0x00, 0x00, 0x4B, 0x00, 0x68, 0x00, 0x61, 0x00, 0x6C, 0x00, 0x20, 0x00, 0x45, 0x00, 0x70, 0x00, 0x69, 0x00, 0x63, 0x00]
        expectedMessageBytes += [0xB0, 0x82, 0x04, 0x92, 0xC5, 0x03, 0x09, 0x00, 0x00, 0x00, 0x4B, 0x00, 0x68, 0x00, 0x61, 0x00, 0x6C, 0x00, 0x2B, 0x00, 0x50, 0x00, 0x69, 0x00, 0x63, 0x00, 0x6B, 0x00]
        expectedMessageBytes += [0xB0, 0x82, 0x04, 0x98, 0xF0, 0x03, 0x05, 0x00, 0x00, 0x00, 0x4B, 0x00, 0x6C, 0x00, 0x69, 0x00, 0x63, 0x00, 0x6B, 0x00]
        expectedMessageBytes += [0xB4, 0x82, 0x04, 0x98, 0xF0, 0x03, 0x09, 0x00, 0x00, 0x00, 0x57, 0x00, 0x69, 0x00, 0x73, 0x00, 0x2B, 0x00, 0x4B, 0x00, 0x6C, 0x00, 0x69, 0x00, 0x63, 0x00, 0x6B, 0x00]
        #    
        expectedMessageByteArray = "".join( chr( val) for val in expectedMessageBytes)  # Converts to byte array vs list.          
        
        myweaponBarList = []
        #
        primaryHandGear     = 32920
        secondaryHandGear   = 10260
        weaponSetName       = 'Khal Epic'
        myweaponBarList.append(eqoa_weaponbar.weaponBar(primaryHandGear, secondaryHandGear, weaponSetName))
        #
        primaryHandGear     = 32920
        secondaryHandGear   = 29001
        weaponSetName       = 'Khal+Pick'
        myweaponBarList.append(eqoa_weaponbar.weaponBar(primaryHandGear, secondaryHandGear, weaponSetName))
        #
        primaryHandGear     = 32920
        secondaryHandGear   = 31756
        weaponSetName       = 'Klick'
        myweaponBarList.append(eqoa_weaponbar.weaponBar(primaryHandGear, secondaryHandGear, weaponSetName))
        #
        primaryHandGear     = 32922
        secondaryHandGear   = 31756
        weaponSetName       = 'Wis+Klick'
        myweaponBarList.append(eqoa_weaponbar.weaponBar(primaryHandGear, secondaryHandGear, weaponSetName))       
        #
        t = eqoa_weaponbar.weaponBarList(myweaponBarList)
        #
        # Decodes weapon bar list and compares with the expectedMessageByteArray
        #        
        receivedMessageByteArray = t.encodeWeaponBarList()

        #t.printWeaponBarList()
        
        
        #print
        #print "CALCD : "+ " ".join("{:02X}".format(ord(c)) for c in receivedMessageByteArray)
        #print "EXPECT: "+ " ".join("{:02X}".format(ord(c)) for c in expectedMessageByteArray)
        #
        self.assertEquals(receivedMessageByteArray,expectedMessageByteArray)  

        
if __name__ == "__main__":
   #import sys;sys.argv = ['', 'Test.testName']
   unittest.main()