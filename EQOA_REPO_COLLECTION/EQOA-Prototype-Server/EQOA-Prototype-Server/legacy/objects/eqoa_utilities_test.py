'''
Created on July 3, 2016

@author:  Ben Turi
'''

import unittest
import eqoa_utilities


class TechniqueTest(unittest.TestCase):
 
   def test_Technique(self):
  
      trueValues   = [          4,  #
                               75,  #
                               -1,  #
                              127,  #
                              175,  # new test...
                             128,  #
                             -128,  #                             
                                0,  #  
                         0x116808,  # Ferry Character ID (Dudderz)
                       0x61CA3EDF,  # Barbarian Male Character Model
                       0x7ED89DAA,  # Dark Elf Male Character Model
                       0xD8D56FF8,  #0x272a9007,  # Elf Male Character Model -657100808 - possibly large negative numbers
                       0xAA5B0447]  # Dwarf Male Character Model
                       
      techValues   = [       0x08,  # These are the values after the technique
                           0x0196,
                             0x01,
                           0x01FE,  # 
                           0x02DE,  # 
                           0x0280,  # 
                           0x01FF,  #                    
                              0x0,  #
                       0x018BA090,  # Ferry Character ID (Dudderz )        
                     0x0C9CD1FBBE,
                     0x0FEDC4F6D4,
                     0x1B8DABBFF0,
                     0x15A5D8918E] 
               

      testValues = []
      for v in trueValues:
        testValues.append(eqoa_utilities.technique(v)[0])
        
      self.assertEquals(testValues,techValues)
      #self.assertEqual(len(message.messageByteArray), 0)  # Not sure this is needed, but doesn't hurt

class BundleLengthConversionTest(unittest.TestCase):

   def test_BunLenDecode(self): # after receiving encoded bundle payload length, decode it.
  
      EncodedValues = [    0x0CF, 
                           0x095,  # 
                           0x98D,
                           0x29C ] 

      DecodedValues = [       79,  #
                              21,  #
                            1165,  #
                             284]  # 
                       

      testValues = []
      for v in EncodedValues:
        testValues.append(eqoa_utilities.bunLenDecode(v))
      #  
      self.assertEquals(testValues,DecodedValues)

   def test_BunLenEncode(self):  # knowing true length of bundle payload, what is encode value to send over wire

      TrueValues =    [       79,  #
                              21,  #
                            1165,  #
                             284]  # 

      EncodedValues = [    0x0CF, 
                           0x095,  # 
                           0x98D,
                           0x29C ] 
              
      testValues = []
      for v in TrueValues:
        testValues.append(eqoa_utilities.bunLenEncode(v))
      #
      self.assertEquals(testValues,EncodedValues)

      
if __name__ == "__main__":
   #import sys;sys.argv = ['', 'Test.testName']
   unittest.main()