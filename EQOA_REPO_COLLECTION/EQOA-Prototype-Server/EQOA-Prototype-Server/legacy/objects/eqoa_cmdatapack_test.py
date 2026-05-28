'''

created december 25th = 2016
@author: Devin Dallas
'''
import unittest
import eqoa_cmdatapack

class testCmDataPack(unittest.TestCase):
    
    def test_cmDataPack(self):
    
    
        expectedMessageBytes     = [0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x0F, 0xAE, 0x98, 0x4C,0x00, 0x55, 0x55, 0x0D, 0x41, 0xE6, 0x01, 0x96, 0x01, 0x78, 0x96, 0x01, 0x00, 0x00, 0x00, 0xDE, 0x02, 0xDE, 0x02, 0x00, 0xFA, 0x01, 0x00, 0x00, 0x00, 0xE8, 0x07, 0x00, 0x5A, 0x00, 0x00, 0x04, 0x00, 0x0C, 0x4F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDE, 0x02, 0xDE, 0x02, 0x00, 0xFA, 0x01, 0x00, 0x00, 0x00]
        expectedMessageByteArray = "".join( chr( val) for val in expectedMessageBytes)  # Converts to byte array vs list.
        
        
        cmdatapack = 0
        unspentcm  = 0
        spentcm    = 976
        xptocm     = 624151
        unknown1   = 0
        moverate   = 8.83333301544
        STRbase    = 115
        STAbase    = 75
        AGIbase    = 60
        DEXbase    = 75
        WISbase    = 0
        INTbase    = 0
        CHAbase    = 0
        STRmax     = 175
        STAmax     = 175
        AGImax     = 0
        DEXmax     = 125
        WISmax     = 0
        INTmax     = 0
        CHAmax     = 0
        HPmax      = 500
        PWRmax     = 0
        HoT        = 45
        PoT        = 0
        defmod     = 0
        offmod     = 2
        ACbase     = 0
        HPfactor   = 6
        FR         = -40
        LR         = 0
        CR         = 0
        AR         = 0
        PR         = 0
        DR         = 0
        unknown2   = 0
        STRmax2    = 175
        STAmax2    = 175
        AGImax2    = 0
        DEXmax2    = 125
        WISmax2    = 0
        INTmax2    = 0
        CHAmax2    = 0
        
        t = eqoa_cmdatapack.cmDataPack(cmdatapack, unspentcm, spentcm, xptocm, unknown1, moverate, STRbase, STAbase, AGIbase, DEXbase, WISbase, INTbase, CHAbase, STRmax, STAmax, AGImax, DEXmax, WISmax, INTmax, CHAmax, HPmax, PWRmax, HoT, PoT, defmod, offmod, ACbase, HPfactor, FR, LR, CR, AR, PR, DR, unknown2, STRmax2, STAmax2, AGImax2, DEXmax2, WISmax2, INTmax2, CHAmax2)
        #
        #
        # t.printcmDataPack()
        #
        #
        receivedMessageByteArray = t.encodecmDataPack()
        #
        #
        #print
        #print "CALCD : "+ " ".join("{:02X}".format(ord(c)) for c in receivedMessageByteArray)
        #print "EXPECT: "+ " ".join("{:02X}".format(ord(c)) for c in expectedMessageByteArray)
        #
        self.assertEquals(receivedMessageByteArray,expectedMessageByteArray)
     
     
if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testName']
    unittest.main()
        
        