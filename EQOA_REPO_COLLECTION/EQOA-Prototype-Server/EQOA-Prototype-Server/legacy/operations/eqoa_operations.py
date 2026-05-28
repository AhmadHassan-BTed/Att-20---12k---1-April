'''
Created on Apr 29, 2016

@author: Stefan McDaniel
'''

import logging
import eqoa_loginserver
import eqoa_messages
import eqoa_bundles


#Base Operation class, Op Codes are mapped to sub-classes of Operation
#Operations can perform an immediate action, or they can cause an event to happen in the future if need be

class Operation():

   def __init__(self, session, bundle, message, opCode):
      self.session = session
      self.bundle = bundle
      self.message = message
      self.opCode = opCode
      
   def performOperation(self):
      #this is the default implementation of the performOperation method. It should be overriden in operation
      #sub-classes. Log a warning for now since it is anticipated that some op codes will not yet have operations.
      logging.warning("Op Code (" + hex(self.opCode) + ") received that has no mapped Operation in Session (A:" + 
                      hex(self.session.sessionIdA) + "B:" + hex(self.session.sessionIdB) + ")")
      logging.debug("Bundle Type was (" + hex(self.bundle.bundleType) + ")")
      logging.debug("Message Data was:" + str(self.message.message))


#validate game disc version message
class GameDiscVersionOperation(Operation):
   
   OP_CODE = 0x0000
   
   def performOperation(self):
      logging.info("GameDiscVersionOperation Called")
      if self.message.gameVersion == 25:
         message = eqoa_messages.GameVersionMessage();
         message.buildMessage(eqoa_messages.MessageType.STANDARD_MESSAGE, self.OP_CODE, 25);
         
         bundle = eqoa_bundles.Bundle()
         bundle.buildBundle(eqoa_bundles.BundleType.SES_ACK_MSG_ACK_MSG_BUNDLE_TYPE6)
         bundle.bundleNumber = -1
         bundle.sessionAck = eqoa_bundles.SessionACK()
         bundle.communicationReport = eqoa_bundles.ComReport()
         bundle.messageContainer = eqoa_bundles.MessageContainer()
         bundle.messageContainer.messageList = [message]
      
         self.session.sendBundleList([bundle])
      else:
         logging.info("Invalid Game Disc Version: " + self.message.gameVersion + "reported in Session (A:" + 
                      hex(self.session.sessionIdA) + "B:" + hex(self.session.sessionIdB) + "), no response " + 
                      "return returned")
         

#This operation logs a user into the world server
#Op Code 0x0904
class LoginServerLoginOperation(Operation):
   
   OP_CODE = 0x0904
   
   def performOperation(self):
      logging.info("LoginServerLoginOperation Called")

      #authenticate the login (Operation expects a LoginMessage)
      if eqoa_loginserver.loginServer.authenticateAccount(self.message.username, self.message.password):
         #authentication successful, send the server list message
         message = eqoa_messages.ServerListingMessage()
         message.buildMessage(eqoa_messages.MessageType.LONG_STANDARD_MESSAGE, 
                                                        0x07B3,
                                                        eqoa_loginserver.loginServer.getServerList())
         bundle = eqoa_bundles.Bundle()
         bundle.buildBundle(eqoa_bundles.BundleType.MESSAGE_BUNDLE_TYPE2)
         bundle.bundleNumber = -1;
         bundle.messageContainer = eqoa_bundles.MessageContainer()
         bundle.messageContainer.messageList = [message]
         self.session.sendBundleList([bundle])
      else:
         #authentication failed
         logging.info("Authentication failed for user " + self.message.username + " in Session (A:" + 
                      hex(self.session.sessionIdA) + "B:" + hex(self.session.sessionIdB) + "), no response " + 
                      "return returned")
