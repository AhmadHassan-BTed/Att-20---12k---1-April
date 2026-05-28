'''

created december 25th, 20x01,6
@author: Devin Dallas
'''

import unittest
import eqoa_datapack

class test_dataPack(unittest.TestCase):

    def test_dataPack(self):
        
        expectedMessageBytes     = [0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
        expectedMessageByteArray = "".join( chr( val) for val in expectedMessageBytes)  # Converts to byte array vs list.
        
        packid    = 1
        packkey1  = 1
        packkey2  = 0
        packvalue = 0
        
        #
        #
        t = eqoa_datapack.dataPack(packid, packkey1 ,packkey2, packvalue)
        #
        #t.printDataPack()
        #
        #
        #
        receivedMessageByteArray = t.encodepack()
        #
        #print
        #print "CALCD : "+ " ".join("{:02X}".format(ord(c)) for c in receivedMessageByteArray)
        #print "EXPECT: "+ " ".join("{:02X}".format(ord(c)) for c in expectedMessageByteArray)
        #
        self.assertEquals(receivedMessageByteArray,expectedMessageByteArray)
     
     
if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testName']
    unittest.main()