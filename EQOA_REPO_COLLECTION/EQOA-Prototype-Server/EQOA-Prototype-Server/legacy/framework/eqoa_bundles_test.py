'''
Created on May 7, 2016

@author: mcda0
'''
import unittest
import eqoa_bundles
import eqoa_messages

class BundleTest(unittest.TestCase):

   def test_BundleTypeX20Decode(self):
      bundleByteList = [0x20, 0x01, 0x00, 0xFB, 0x06,
                        0x01, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0xFB, 0x3E, 0x02, 0x00, 0x04, 0x09, 0x00, 0x03,
                        0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x45, 0x51, 0x4F, 0x41, 0x0A, 0x00, 0x00, 0x00, 0x6B,
                        0x69, 0x65, 0x73, 0x68, 0x61, 0x65, 0x73, 0x68, 0x61, 0x01, 0xFA, 0x10, 0x69, 0x22, 0x1C, 0xD4,
                        0x45, 0xBC, 0xFD, 0x68, 0x3C, 0x56, 0x22, 0x87, 0xD9, 0x70, 0xB7, 0x1C, 0x12, 0xAE, 0x76, 0xC4,
                        0x98, 0xFD, 0xF3, 0xCE, 0xEB, 0x44, 0x4A, 0x0A, 0x49, 0xB5]
      bundleByteArray = "".join( chr( val) for val in bundleByteList)                 

      bundle = eqoa_bundles.Bundle()
      bundle.decodeBundle(bundleByteArray)
      
      #ensure there are two messages, a GameVersionMessage and a LoginMessage
      self.assertEquals(len(bundle.messageContainer.messageList), 2)
      self.assertIsInstance(bundle.messageContainer.messageList[0], eqoa_messages.GameVersionMessage)
      self.assertIsInstance(bundle.messageContainer.messageList[1], eqoa_messages.LoginMessage)
      
   def test_ComReportDecode(self):
      comReportByteList  = [0x16, 0x00, 0x16, 0x00, 0x17, 0x00]
      comReportByteArray = "".join( chr( val) for val in comReportByteList)     
   
      comReport = eqoa_bundles.ComReport()
      comReport.decodeComReport(comReportByteArray)
      
      #ensure report was properly decoded
      self.assertEqual(comReport.thisBundleBeingSent, 22)
      self.assertEqual(comReport.lastBundleReceived, 22)
      self.assertEqual(comReport.lastMessageReceived, 23)


   def test_ComReportEncode(self):
      expectedComReportByteList = [0x16, 0x00, 0x16, 0x00, 0x17, 0x00]
      expectedComReportByteArray = "".join( chr( val) for val in expectedComReportByteList)      

      comReport = eqoa_bundles.ComReport()
      comReport.buildComReport(22,22,23)
      encodedComReport = comReport.encodeComReport()
      
      self.assertIsInstance(comReport, eqoa_bundles.ComReport)
      self.assertEqual(encodedComReport,expectedComReportByteArray)
            
      
if __name__ == "__main__":
   #import sys;sys.argv = ['', 'Test.testName']
   unittest.main()