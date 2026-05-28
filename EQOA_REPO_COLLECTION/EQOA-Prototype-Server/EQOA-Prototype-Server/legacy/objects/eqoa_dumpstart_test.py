'''

created december 24th, 2016
@author: Devin Dallas
'''

import unittest
import eqoa_dumpstart

class testdumpStart(unittest.TestCase):

    def test_dumpstart(self):
    
        expectedMessageBytes     = [0x10, 0x00, 0x00, 0x00, 0x64, 0x61, 0x74, 0x61, 0x5c, 0x74, 0x75, 0x6E, 0x61, 0x72, 0x69, 0x61, 0x2E, 0x65, 0x73, 0x66, 0xF6, 0xC3, 0x95, 0x01, 0x07, 0x00, 0x00, 0x00, 0x44, 0x75, 0x64, 0x64, 0x65, 0x72, 0x7A, 0x00, 0x0A, 0x78, 0xF2, 0xAF, 0xC5, 0xAC, 0x01, 0x00, 0xFF, 0x00, 0xA6, 0xBE, 0x36, 0x00, 0xA0, 0x06, 0x00, 0x5C, 0xEC, 0xC5, 0x46, 0xED, 0x80, 0x58, 0x42, 0x8E, 0x11, 0x76, 0x46, 0x9C, 0xEB, 0xC5, 0xBF]
        expectedMessageByteArray = "".join( chr( val) for val in expectedMessageBytes)  # Converts to byte array vs list.
        
        worldname   = 'data\tunaria.esf'
        serverid    = 1224955
        name        = 'Dudderz'
        Class       = 0
        race        = 5
        level       = 60
        xp          = 180923385
        debt        = 0
        breath      = 255
        tunar       = 0
        banktunar   = 446355
        trainpoints = 0
        basemaxstat = 400
        world       = 0
        Y           = 25334.1796875
        Z           = 54.1259040833
        X           = 15748.3886719
        facing      = -1.54625272751
        
        #
        t = eqoa_dumpstart.dumpStart(worldname, serverid, name, Class, race, level, xp, debt, breath, tunar, banktunar, trainpoints, basemaxstat, world, Y, Z, X, facing)
        #
        #t.printdumpstart()
        #
        # 
        #
        receivedMessageByteArray = t.encodeddumpstart()
        #
        #print
        #print "CALCD : "+ " ".join("{:02X}".format(ord(c)) for c in receivedMessageByteArray)
        #print "EXPECT: "+ " ".join("{:02X}".format(ord(c)) for c in expectedMessageByteArray)
        #
        self.assertEquals(receivedMessageByteArray,expectedMessageByteArray)
     
     
if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testName']
    unittest.main()