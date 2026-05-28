'''
Created on Apr 30, 2016

@author: Stefan McDaniel
'''

import unittest
import eqoa_bundles
import eqoa_sessions
import eqoa_messages
import eqoa_packets

# a sub-class of Session that records what bundleLists have been sent for the purpose of unit testing
#class BundleListRecordingSession(eqoa_sessions.Session):
#   
#   # override constructor to create a bundle lists sent
#   def __init__(self, masterEndpointID, slaveEndpointID, sessionIdA, sessionIdB):
#      eqoa_sessions.Session.__init__(self, masterEndpointID, slaveEndpointID, sessionIdA, sessionIdB)
#      self.bundleListsSent = []
#   
#   # override the sendBundleList method so that the sent bundles can be recorded
#   def sendBundleList(self, bundleList):
#      eqoa_sessions.Session.sendBundleList(self, bundleList)
#      self.bundleListsSent.append(bundleList)


# test the bundle list handling of the Session class
#class SessionTest(unittest.TestCase):
#
#   # test the Session object with a bundle that contains the game version and the login information
#   def testLoginBundle(self):
#      message1 = eqoa_messages.GameVersionMessage()
#      message1.buildMessage(eqoa_messages.MessageType.STANDARD_MESSAGE, 
#                                                    0x0000, 
#                                                    25)
#      message2 = eqoa_messages.LoginMessage()
#      message2.buildMessage(eqoa_messages.MessageType.STANDARD_MESSAGE, 
#                                              0x0904, 
#                                              "user1", 
#                                              "password1")
#      bundle = eqoa_bundles.Bundle()
#      bundle.buildBundle(eqoa_bundles.BundleType.MESSAGE_BUNDLE_TYPE2)
#      bundle.bundleNumber = 1
#      bundle.messageContainer = eqoa_bundles.MessageContainer()
#      bundle.messageContainer.messageList = [message1, message2]
#                         
#      eqoapacket = eqoa_packets.StandardEQOAPacket()
#      eqoapacket.buildEQOAPacket(0x01, 0x02, [ bundle ])
#      
#      #perform a test that handles a packet containing the login messages
#      testSession = BundleListRecordingSession(0x01, 0x02, 0x01, 0x01)
#      testSession.handlePacket(eqoapacket)
#     
#      #ensure the correct bundles have been sent
#      self.assertEquals(len(testSession.bundleListsSent), 2)
#      self.assertIsInstance(testSession.bundleListsSent[0][0].messageContainer.messageList[0], eqoa_messages.GameVersionMessage)
#      self.assertIsInstance(testSession.bundleListsSent[1][0].messageContainer.messageList[0], eqoa_messages.ServerListingMessage)
#      
#      encodedMessage = testSession.bundleListsSent[1][0].messageContainer.messageList[0].encodeMessage();
      
      #print encodedMessage


if __name__ == '__main__':
   unittest.main()
