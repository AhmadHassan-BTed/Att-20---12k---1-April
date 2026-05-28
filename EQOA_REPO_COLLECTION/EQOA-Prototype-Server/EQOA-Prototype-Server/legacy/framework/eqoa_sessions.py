'''
Created on Apr 30, 2016

@author: Stefan McDaniel
'''

import logging
import eqoa_operations
import eqoa_utilities
import eqoa_endpoints

# this is a utility method that allows the creation of a Python 2.7 compatible enumerations
#def enum(**enums):
#   return type('Enum', (), enums)

# This enumeration covers the possible session actions that a packet could specify
SessionAction = eqoa_utilities.enum(NONE = 0x00,
                     NEW = 0x21,
                     CLOSE = 0x14,
                     CONTINUING = 0x01)

# The Session class represents a connection between two different endpoints. It (might) contain
# a single connection/communcation object that will handle sending and receiving the Bundles
# of Messages. The incoming queue accepts a Bundle object.  The virgin outgoing queue will 
# send Bundles of messages while the outstanding queue contains all of the Messages that have been sent
# and are 're-bundled' and sent again if an ACK is not received in a specified time

class Session:
   # constructor for a Session object between two endpoints
   # masterEndpointID is the Endpoint ID of the Session master
   # slaveEndpointID  is the Endpoint ID of the Session slave
   # sessionIdA and sessionIdB are the two session Ids associated with the session
   #
   def __init__(self):
      self.masterEndpointInfo = eqoa_endpoints.EndpointInfo(0,0,0,0)
      self.slaveEndpointInfo  = eqoa_endpoints.EndpointInfo(0,0,0,0)
      self.sessionIdA       = 0x0000 
      self.sessionIdB       = 0x000
      self.bundleList = []

      
   def buildSession(self, masterEndpointInfo, slaveEndpointInfo, sessionIdA, sessionIdB):
      self.masterEndpointInfo = masterEndpointInfo
      self.slaveEndpointInfo = slaveEndpointInfo
      self.sessionIdA = sessionIdA
      self.sessionIdB = sessionIdB
      self.bundleList = [];       


   def printSession(self):
     #   
     sessionID = '  SESSION ID: ' + '0x{0:08X}'.format(self.sessionIdA)
     if self.sessionIdB != 0x000:
        sessionID = sessionID + '  SESSION ID: ' + '0x{0:08X}'.format(self.sessionIdB) 
     sessionPrint = sessionID + '     MASTER ENDPOINT ID: ' + '0x{:08X}'.format(self.masterEndpointInfo.EndpointID) + '     SLAVE ENDPOINT ID: ' + '0x{:08X}'.format(self.slaveEndpointInfo.EndpointID)
     return sessionPrint      

      
   def handlePacket(self, packet):
      #iterate through the bundles in the packet
      for bundle in packet.bundleList:
         #iterate through the messages in the bundle
         for message in bundle.messageContainer.messageList:
            #call the appropriate operation for the message
            if message.messageOpcode == eqoa_operations.LoginServerLoginOperation.OP_CODE:
               operation = eqoa_operations.LoginServerLoginOperation(self, bundle, message, message.messageOpcode)
            elif message.messageOpcode == eqoa_operations.GameDiscVersionOperation.OP_CODE:
               operation = eqoa_operations.GameDiscVersionOperation(self, bundle, message, message.messageOpcode)
            else:
               operation = eqoa_operations.Operation(self, bundle, message, message.messageOpcode);
            operation.performOperation()

   #return a bundle list to the client
   def sendBundleList(self, bundleList):
      logging.info("Bundle list being returned through Session (A:" + hex(self.sessionIdA) + "B:" + hex(self.sessionIdB) + ")")
      logging.info(str(self.bundleList))

