'''
Created on May 1, 2016

@author: Ben Turi
'''
from scapy.all import *
#from pycrc.crc_algorithms import Crc
#from pycrc.crc_algorithms import *
#import pycrc.crc_algorithms

import eqoa_bundles
import eqoa_utilities
import logging
import struct
import binascii

PacketTypes = eqoa_utilities.enum(NONE          = 0x0000,
                                  TRANSFER      = 0xFFFF,
                                  FIRST_CONTACT = 0xFFFE)

                                 
                  
###############################################################################
#   
class StandardEQOAPacket():

   # Constructs a null EQOA packet object to be encoded or decoded later
   #
   # localEndpointID  is the ID of the endpoint sending the Packet
   # remoteEndpointID is the ID of the intended destination endpoint
   # bundleList will hold the bundles
   # crcCalculated will be used to check the incoming EQOApackets and checksum the outgoing EQOAPackets 
   # crcReceived will be the CRC32 of an incoming EQOApacket
   # crcPassed will indicate if the Calculated and Recevieved values are the same.
   def __init__(self):
      self.localEndpointID = 0
      self.remoteEndpointID = 0
      self.bundleList = []
      #
      self.crcCalculated = 0
      self.crcReceived = 0 
      self.crcPassed = 0
      #
      return

     
   ##############################################
   #
   # This method will build an EQOApacket object from known quantities  
   def buildEQOAPacket(self, localEndpointID, remoteEndpointID, bundleList):
     self.localEndpointID = localEndpointID
     self.remoteEndpointID = remoteEndpointID
     self.bundleList = bundleList
     # 1) will need to concatenate bundles going out in this EQOApacket
     # 2) will need to add EQOApacket header to beginning (endpoint ids)
     # 3) will need to calculate CRC32 of this entity and attach to end.
     # 4) eventually should get placed in ServerEndpoint outgoing packet list
     #
     # self.crcCalculated = crcCalculated
   ###############################################
   #
   # This method will extract a EQOApacket object from raw bytes 
   def decodeEQOAPacket(self, payload):
     # 1) extract the first four bytes and save client and server endpoint IDs
     # 2) extract the last four bytes and save as self.crcReceived
     # 3) calculate CRC32 of entire payload and save as self.crcCalculated
     # 4) determine value of crcPassed
     # 5) extract the bundles and save in self.bundleList
     #
     # 
     # Extract Standard EQOA Packet Header
     #
     decode_fmt  = '<'  # unpack as little endian
     decode_fmt += 'H'  # 2 byte SRC EndpointID
     decode_fmt += 'H'  # 2 byte DST EndpointID
     #
     s = struct.unpack(decode_fmt,payload[0:4])                # actually unpacks the PacketPayload
     #
     self.remoteEndpointID = s[0] 
     self.localEndpointID  = s[1]
     #
     # Extract incoming CRC
     #      
     decode_fmt  = '<'  # unpack as little endian
     decode_fmt += 'I'  # 4 byte Received CRC value
     #
     s = struct.unpack(decode_fmt,payload[len(payload)-4:])                # actually unpacks the PacketPayload
     #
     self.crcReceived      = s[0]
     #
     # Code to Check CRC here
     #
     self.crcCalculated   = 0
     crcPayload     = bytearray()
     crcPayload.extend(payload[0:len(payload)-4])
     self.crcCalculated = ((binascii.crc32(crcPayload)^0xFFFFFFFF)^0x11f19ed3)&0xFFFFFFFF
     #
     if self.crcCalculated == self.crcReceived:
        self.crcPassed = 1
     #
     
     #
     return
      
   def printme(self):
     #
     print
     print 'Remote Endpoint ID : ','0x{:04X}'.format(self.remoteEndpointID)
     print 'Local Endpoint ID  : ','0x{:04X}'.format(self.localEndpointID)
     print 'CRC Received       : ','0x{:08X}'.format(self.crcReceived)
     print 'CRC Calculated     : ','0x{:08X}'.format(self.crcCalculated)
     print
     #
     return      
     
   ###############################################
   #
   # This method will encode a EQOApacket object to raw bytes
   # It may have been an EQOAPAcket we just created with buildEQOApacket   
   def encodeEQOAPacket(self, an_EQOApacket):

     pass 
   ###############################################
   #
   # This method will extract the CRC32 from the last four bytes of an incoming EQOAPacket    
   def extractCRC(self, payload):
     # self.crcReceived = 0
     pass        
     
   #
   # This method will calculate the CRC32 for the outgoing packet    
   def calculateCRC(self, payload):
   # self.crcCalculated = crcCalculated
     pass       

   #
   # This will extract the bundle data from the Packet and form bundle objects for each,
   # then the resulting bundle objects will be placed in the self.bundleList
   def extractBundles(self, payload):  # probably needs to move to bundles
     #self.rawBundleList = []
     #self.bundleList = []
     #
     # 1) this will call extractBundleLengthClass and return the length of the first bundle in the EQOAPacket
     #    sending only two bytes
     # 2) Next, we  instantiate a Bundle objects and append it to the list
     #    self.bundleList.append(Bundle.decodeBundle(rawBundlebytes) # only processes down to BundlePayloadType
     #    Basically it means the Bundle object will have a bundleClass, Length, 
     #    SessionIDA, SessionIDB, bundlePayloadType, bundlePayload
     # 3) repeat for any remaining bundles
     #
     pass       

#
###############################################################################
#   
TransferAction = eqoa_utilities.enum(NONE = 0x0000,
                                     REQUEST_TRANSFER = 0x0992,
                                     ACKNOWL_TRANSFER = 0x0993)

class TransferEQOAPacket():

   # 1) Server will tell client it needs to transfer to a new Area Server in reliable standard message
   # 2) Client will then send a TransferEQOAPacket to the new Area Server
   # 3) The new Area Server should be able to add an Endpoint object when it gets
   # this message, but it doesn't know the session yet. That will come later. Actually, maybe current Area Server
   # transfers the session internally and it waits till it hears from the client and then connects the endpoint
   # with the session
   # Inits a transfer packet object
   # Used to ACK client transfer from one area server to another
   # Unique structure compared to standard EQOA Packet (no header or footer)
   #
   # transferNumber is the sequential ID associated with each transfer of the session
   # Might need to make TransferObjects that travel with Sessions
   # Transfer numbers should travel with session
   # action says if this is a REQUEST or need to make ACKNOWLEDGEMENT
   # toEndpoint is 
   #
   def __init__(self):
      self.transferNumber   = 0
      self.action           = TransferAction.NONE
      self.incomingEndpoint = 0
      
   ########################################
   #
   # This will be used to build a TransferPacket Object in the code
   #        
   def buildTransferPacket(self, mytransferNumber, myAction, incomingEndpoint):
     self.transferNumber   = mytransferNumber
     self.action           = myAction
     self.incomingEndpoint = incomingEndpoint  # might actually use dummy values except endpointID
     return  

   ########################################
   #
   # When the incomingQueue gets this message, it will identify it as a Transfer Message
   # and create a TransferPacket object.   Once this messages is received, the operation
   # should create a new session/endpoint on the receiving area server so it is ready for the 
   # standard message from the same client
   #        
   def decodeTransferPacket(self, networkPacketPayload):
    
      decode_fmt  = '<'    # pack as little endian
      decode_fmt += 'H'    # Transfer Indicator 0xFFFF
      decode_fmt += 'H'    # Action Indicator 0x0992 ( for decode)
      decode_fmt += 'I'    # Transfer Number
      decode_fmt += 'H'    # incoming EndpointID
      decode_fmt += 'H'    # incoming Endpoint Port (might be dummy as well)
      decode_fmt += 'BBBB' # ipAddress         (probably dummy. Already known since we sent a packet)
      #
      s = struct.unpack(decode_fmt,networkPacketPayload)         # actually unpacks the messageByteList
      num_bytes_read = struct.calcsize(decode_fmt)               # calculate how many bytes will be read
      #self.messageByteArray = messageByteArray[num_bytes_read:]  # remove the bytes we read from the message
      #
      self.transferNumber     = s[2]
      self.action             = s[1]
      self.incomingEndpoint = Endpoint(s[5:9],s[4], s[3],'TRANSFER')
      
      return      
 

   ########################################
   #
   # The Area Server will only ever send an ACK to the original message
   # So we really only have to encode the ACK and decode the request
   # How will we know where to send it back to? keep original request?        
   #
   def encodeTransferPacket(self, a_TransferPacket):
    #
      encode_fmt  = '<'  # pack as little endian
      encode_fmt += 'H'  # Special Packet Case - two bytes unsigned ( Transfer Packet)
      encode_fmt += 'H'  # TransferAction      - two bytes unsigned
      encode_fmt += 'I'  # transferNumber      - four bytes unsigned        
      encodedMessage = struct.pack(encode_fmt,
                                   eqoa_packets.SpecialPacketCase.TRANSFER_PACKET,
                                   eqoa_packets.TransferAction.ACKNOWL_TRANSFER,
                                   self.transferNumber) # actually packs the message      

      return encodedMessage;    #
    #
    
