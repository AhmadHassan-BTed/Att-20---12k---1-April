'''
Created on Apr 28, 2016

@author: Stefan McDaniel
'''

import logging
import struct

# this is a utility method that allows the creation of a Python 2.7 compatible enumerations
def enum(**enums):
   return type('Enum', (), enums)

#
###############################################################################
#   
OpCode     = enum(GAME_VERSION      = 0x0000,
                  LOGIN             = 0x0904,
                  SERVER_LISTING    = 0x07B3,
				  PRECHAR_MSG1      = 0x07D1,
				  PRECHAR_MSG2      = 0x07F5,
                  CHAR_LISTING      = 0x002C,
				  MEM_DUMP			= 0x0001)
                  
                       
# enumeration to represent the different message types 
MessageType = enum(STANDARD_MESSAGE       = 0x00FB,
                   CONTINUED_MESSAGE      = 0xFFFA,
                   LONG_STANDARD_MESSAGE  = 0xFFFB,
                   LONG_SYSTEM_MESSAGE    = 0xFFFC)
 
# Represents an individual message
class Message():

   def __init__(self):
      self.messageType = None
      self.messageOpcode = None
   
   # messageType   - MessageType enumeration
   # messageOpcode - Explicitly pull out the Opcode  
   def buildMessage(self, messageType, messageOpCode):
      self.messageType = messageType  # System message, standard message, short/long etc
      self.messageOpcode = messageOpCode
   
   # This method returns the byte list of the message once it has been serialized. The default implementation
   # returns any empty list.
   def encodeMessage(self):
      logging.warn("Default encorder (with does nothing) used for message with op code: " + self.messageOpcode)
      return []
   
   # This method returns the constructed Message object from the serialized data that composes it. The default
   # implementation returns None.
   def decodeMessage(self, messageByteList):
      logging.warn("Default decoder (with does nothing) used for message with op code: " + self.messageOpcode)
      
#
##########################################################################################################
#
GAME_VERSION = enum(EQOA_FRONTIERS       = 0x25,
                    EQOA_VANILLA         = 0x12,
                    UNKNOWN_VERSION      = 0x00)
# This message contains the game version
class GameVersionMessage(Message):
     
   def __init__(self):
      Message.__init__(self)
      self.gameVersion = None
      
   # messageType   - MessageType enumeration
   # messageOpcode - Explicitly pull out the Opcode  
   # gameVersion - the game version of the client
   def buildMessage(self, messageType, messageOpCode, gameVersion):
      Message.buildMessage(self, messageType, messageOpCode)
      self.gameVersion = gameVersion
      
   # This method returns the constructed Message object from the serialized data that composes it.
   def decodeMessage(self, messageByteArray):
   
      # Currently in the test, the opcode is included in the messageByteList and should be processed
      decode_fmt  = '<'  # pack as little endian
      decode_fmt += 'H'  # opcode -  two bytes unsigned
      decode_fmt += 'I'  # game version - four bytes unsigned    
      #
      s = struct.unpack(decode_fmt,messageByteArray)             # actually unpacks the messageByteList
      num_bytes_read = struct.calcsize(decode_fmt)               # calculate how many bytes will be read
      self.messageByteArray = messageByteArray[num_bytes_read:]  # remove the bytes we read from the message
      #
      self.messageOpcode = s[0] #store  the Op Code (should be 0x0000)
      self.gameVersion   = s[1] #store the game disc version
      
   def encodeMessage(self):
      #Assumption that the packet or bundle will be handling the placing of the Op Code at the beginning

      encode_fmt  = '<'  # pack as little endian
      encode_fmt += 'H'  # opcode -  two bytes unsigned
      encode_fmt += 'I'  # game version - four bytes unsigned        
      encodedMessage = struct.pack(encode_fmt,
                                   self.messageOpcode,
                                   self.gameVersion) # actually packs the message      

      return encodedMessage;
#
##########################################################################################################
#
# This message contians the username and password
class LoginMessage(Message):
   
   # messageType   - MessageType enumeration
   # messageOpcode - Explicitly pull out the Opcode  
   # username - username of the account
   # password - password of the account
   def __init__(self):
      Message.__init__(self)
      self.username = None
      self.password = None
      
   # messageType   - MessageType enumeration
   # messageOpcode - Explicitly pull out the Opcode  
   # username - username of the account
   # password - password of the account
   def buildMessage(self, messageType, messageOpcode, username, password):
      Message.buildMessage(self, messageType, messageOpcode)
      self.opcodeOption      = 0x00
      self.valueOne          = 3
      self.gameCode          = 'EQOA'
      self.gameCodeLength    = len(self.gameCode)
      self.username          = username
      self.usernameLength    = len(self.username)
      self.password          = password
      self.usernameDelimiter = 0x01
      
   # This method returns the constructed Message object from the serialized data that composes it.
   def decodeMessage(self, messageByteArray):

      self.messageByteArray = messageByteArray
      # Currently in the test, the opcode is included in the messageByteList and should be processed
      decode_fmt  = '<'  # unpack as little endian
      decode_fmt += 'H'  # 2 bytes unsigned opcode
      decode_fmt += 'B'  # 1 byte  option byte
      decode_fmt += 'I'  # 4 byte integer (1 or 3)
      decode_fmt += 'I'  # 4 byte integer - length of game code
      #
      # unpacket now since length of game code can be variable
      num_bytes_read = struct.calcsize(decode_fmt)                          # calculate how many bytes will be read
      s = struct.unpack(decode_fmt,self.messageByteArray[0:num_bytes_read]) # actually unpacks the messageByteList
      self.messageByteArray = self.messageByteArray[num_bytes_read:]        # remove the bytes we read from the message      
      #      
      self.messageOpcode    = s[0] # store  the Op Code (should be 0x0904)
      self.opcodeOption     = s[1] # opcode option
      self.valueOne         = s[2] # unknown at this point
      self.gameCodeLength   = s[3] # Length of game code
      #
      decode_fmt  = '<'  # unpack as little endian
      decode_fmt += '{}'.format(self.gameCodeLength)+'s' # variable length of game code
      decode_fmt += 'I'  # username length
      #
      # unpacket now since length of name can be variable
      num_bytes_read = struct.calcsize(decode_fmt)                    # calculate how many bytes will be read
      s = struct.unpack(decode_fmt,self.messageByteArray[0:num_bytes_read])             # actually unpacks the messageByteList
      self.messageByteArray = self.messageByteArray[num_bytes_read:]  # remove the bytes we read from the message            
      #
      self.gameCode         = s[0] #
      self.usernameLength   = s[1] #      
      #
      decode_fmt  = '<'  # unpack as little endian
      decode_fmt += '{}'.format(self.usernameLength)+'s' # variable length of username
      decode_fmt += 'B'  # 01 delimiter to end username
      decode_fmt += '32s'# password
      #
      num_bytes_read = struct.calcsize(decode_fmt)                          # calculate how many bytes will be read
      s = struct.unpack(decode_fmt,self.messageByteArray[0:num_bytes_read]) # actually unpacks the messageByteList
      self.messageByteArray = self.messageByteArray[num_bytes_read:]        # remove the bytes we read from the message       
      
      self.username          = s[0]
      self.usernameDelimiter = s[1]
      self.password          = s[2]

  # This method serializes the data from a Message object
   def encodeMessage(self):

      #Assumption that the packet or bundle will be handling the placing of the Op Code at the beginning

      # Currently in the test, the opcode is included in the messageByteList and should be processed
      encode_fmt  = '<'  # unpack as little endian
      encode_fmt += 'H'  # 2 bytes unsigned opcode
      encode_fmt += 'B'  # 1 byte option byte
      encode_fmt += 'I'  # 4 byte integer (1 or 3)
      encode_fmt += 'I'  # 4 byte integer - length of game code
      encode_fmt += '{}'.format(self.gameCodeLength)+'s' # variable length of game code
      encode_fmt += 'I'  # username length
      encode_fmt += '{}'.format(self.usernameLength)+'s' # variable length of username
      encode_fmt += 'B'  # 01 delimiter to end username
      encode_fmt += '32s'# password
    
      encodedMessage = struct.pack(encode_fmt,
                                   self.messageOpcode,  
                                   self.opcodeOption,   
                                   self.valueOne,             
                                   self.gameCodeLength, 
                                   self.gameCode,        
                                   self.usernameLength,
                                   self.username,                                            
                                   self.usernameDelimiter,
                                   self.password)  
      return encodedMessage;                  
      
#
##########################################################################################################
# 

class PreCharSelect1Message(Message):
	"""Processes pre-character select message # 1."""
	
	def __init__(self):
		Message.__init__(self)
		self.preCharMsg1 = 0
		
   # messageType   - MessageType enumeration
   # messageOpcode - Explicitly pull out the Opcode
   # preCharMsg1   - Unknown what this message is for at this time.	
	def buildMessage(self, messageType, messageOpCode, preCharMsg1):
		Message.buildMessage(self, messageType, messageOpCode)
		self.preCharMsg1 = preCharMsg1
		
	def encodeMessage(self):
	# Assumption that the packet or bundle will be handling the placing of the 
	# Op Code at the beginning.	
		encode_fmt = '<'  # Pack as little endian.
		encode_fmt += 'H' # Opcode - two bytes unsigned.
		encode_fmt += 'I' # Unknown - 4 bytes unsigned
		
		encodedMessage = struct.pack(encode_fmt, 
									self.messageOpcode,
									self.preCharMsg1
									) # Actually packs the message.
		return encodedMessage;
		
class PreCharSelect2Message(Message):
	"""Processes pre-character select message # 2."""
	
	def __init__(self):
		Message.__init__(self)
		self.preCharMsg2 = 0
		
   # messageType   - MessageType enumeration
   # messageOpcode - Explicitly pull out the Opcode
   # preCharMsg2   - Unknown what this message is for at this time.	
	def buildMessage(self, messageType, messageOpCode, preCharMsg2):
		Message.buildMessage(self, messageType, messageOpCode)
		self.preCharMsg2 = preCharMsg2
		
	def encodeMessage(self):
	# Assumption that the packet or bundle will be handling the placing of the 
	# Op Code at the beginning.
		encode_fmt = '<'  # Pack as little endian.
		encode_fmt += 'H' # Opcode - two bytes unsigned.
		encode_fmt += 'I' # Unknown - 4 bytes unsigned
		
		encodedMessage = struct.pack(encode_fmt, 
									self.messageOpcode,
									self.preCharMsg2
									) # Actually packs the message.
		return encodedMessage;		
#
################################################################################		
#

LANGUAGE = enum(US_ENGLISH      = 0x00,
                UK_ENGLISH      = 0x01,
                FRENCH          = 0x02,
                GERMAN          = 0x03)
                
RECOMMENDED = enum(NO  = 0x00,
                   YES = 0x01)
#     
# Message that contains the list of servers to be shown to the user
class ServerListingMessage(Message):
   
   def __init__(self):
      Message.__init__(self)
      self.serverListings = None
      
   # messageType   - MessageType enumeration
   # messageOpcode - Explicitly pull out the Opcode
   # serverListings - list of ServerListing objects
   def buildMessage(self, messageType, messageOpcode, serverListings):
      Message.buildMessage(self, messageType, messageOpcode)
      self.serverListings = serverListings

   def encodeMessage(self):
      # 
      encode_fmt  = '<'  # pack as little endian
      encode_fmt += 'H'  # 2 bytes unsigned opcode
      encode_fmt += 'B'  # 1 byte  unsigned number of servers (need to send through the technique)   
      #
      
      encodedMessage = struct.pack(encode_fmt,
                                   self.messageOpcode,  
                                   len(self.serverListings) * 2)               # should use the technique     

      #iterate over each server listing
      for thisServer in self.serverListings:
        # 
        encode_fmt  = '<'  # pack as little endian
        encode_fmt += 'I'  # 4 byte integer for length of server name 
        encode_fmt += '{}'.format(len(2*thisServer.serverName))+'s'             # character name - in unicode   so us Hs     
        encode_fmt += 'b'  # single byte for recommended flag 
        encode_fmt += 'H'  # two bytes unsigned for endpoint ID         
        encode_fmt += 'H'  # two bytes unsigned for endpoint PORT         
        encode_fmt += '4s'  # four bytes for IP address
        encode_fmt += 'B'  # one byte for language flag        
             
        num_bytes_read = struct.calcsize(encode_fmt)                          # calculate how many bytes will be read
        
        encodedMessage += struct.pack(encode_fmt,
                                     len(thisServer.serverName),
                                     packUnicode(thisServer.serverName),    # need a routine to generate unicode     
                                     thisServer.recommended,
                                     thisServer.endPointId,
                                     thisServer.portNumber,
                                     packIPAddress(thisServer.ipAddress),
                                     thisServer.language)
                        
              
      return encodedMessage
      
#
# These will move to utils eventually
#      
      
def packIPAddress(myip):  # short little routine to prepare IP addresses for packing
  return struct.pack('4B', *(int(x) for x in reversed(myip.split('.'))))
  
def packUnicode(mystring):
   packed = ''
   for c in mystring:
     packed += struct.pack('<H',ord(c))
   return packed  

#
################################################################################		
#
					 
						
# Message displays the Character list for the sever that was selected. 					

class CharListingMessage(Message):
	"""Presents the characters to the client during character selection."""
	
	def __init__(self):
		Message.__init__(self)
		self.charListing = None
		
		
	def buildMessage(self, messageType, messageOpCode, charListing):
		Message.buildMessage(self, messageType, messageOpCode)
		self.charListing = charListing
		
		
	def encodeMessage(self):
		encode_fmt  =  '<'  # Pack as little endian.
		encode_fmt += 'H' # Opcode - two bytes unsigned.
		encode_fmt += 'B' # Number of Toons Described (Doubled)
		
		encodedMessage = struct.pack(encode_fmt,
                                     self.messageOpcode,  
                                     len(self.charListing)*2)
								   
		for thisChar in self.charListing:	

			encode_fmt  = '<' # Pack as little endian.
			encode_fmt += 'I' # 4 Byte integer for length of character name.
			encode_fmt += '{}'.format(len(thisChar.charName))+'s' # Name of character.
			encode_fmt += 'I' # 4 Byte unsigned for Server Char ID.
			encode_fmt += 'B' # Byte 1 for Model Id
			encode_fmt += 'B' # Byte 2 for Model Id
			encode_fmt += 'B' # Byte 3 for Model Id
			encode_fmt += 'B' # Byte 4 for Model Id
			encode_fmt += 'B' # Byte 5 for Model Id
			encode_fmt += 'B' # 1 Byte for Char Class.
			encode_fmt += 'B' # 1 Byte for Char Race.
			encode_fmt += 'B' # 1 Byte for Char Level.
			encode_fmt += 'B' # 1 Byte for Char Hair Color.
			encode_fmt += 'B' # 1 Byte for Char Hair Length.
			encode_fmt += 'B' # 1 Byte for Char Hair Style.
			encode_fmt += 'B' # 1 Byte for Char Face Option.
			encode_fmt += 'I' # 4 Byte for Robe Graphic.
			encode_fmt += 'I' # 4 Byte for Primary Hand Graphic.
			encode_fmt += 'I' # 4 Byte for Secondary Hand Graphic.
			encode_fmt += 'I' # 4 Byte for Shield Graphic.
			encode_fmt += 'H' # 1 Byte for Toon Animation.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Value 1.
			encode_fmt += 'B' # 1 Byte for Chest Graphic.
			encode_fmt += 'B' # 1 Byte for Bracer Graphic.
			encode_fmt += 'B' # 1 Byte for Glove Graphic.
			encode_fmt += 'B' # 1 Byte for Pants Graphic.
			encode_fmt += 'B' # 1 Byte for Boots Graphic.
			encode_fmt += 'B' # 1 Byte for Helm Graphic.
			encode_fmt += 'H' # 2 Byte for Vanilla Unused Value 2.
			encode_fmt += 'I' # 4 Byte for Vanilla Unused Value 3.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 1 R.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 1 G.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 1 B.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 1 A.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 2 R.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 2 G.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 2 B.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 2 A.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 3 R.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 3 G.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 3 B.
			encode_fmt += 'B' # 1 Byte for Vanilla Unused Color 3 A.
			encode_fmt += 'B' # 1 Byte for Char Chest Color R.
			encode_fmt += 'B' # 1 Byte for Char Chest Color G.
			encode_fmt += 'B' # 1 Byte for Char Chest Color B.
			encode_fmt += 'B' # 1 Byte for Char Chest Color A.
			encode_fmt += 'B' # 1 Byte for Char Bracer Color R.
			encode_fmt += 'B' # 1 Byte for Char Bracer Color G.
			encode_fmt += 'B' # 1 Byte for Char Bracer Color B.
			encode_fmt += 'B' # 1 Byte for Char Bracer Color A.
			encode_fmt += 'B' # 1 Byte for Char Glove Color R.
			encode_fmt += 'B' # 1 Byte for Char Glove Color G.
			encode_fmt += 'B' # 1 Byte for Char Glove Color B.
			encode_fmt += 'B' # 1 Byte for Char Glove Color A.
			encode_fmt += 'B' # 1 Byte for Char Pants Color R.
			encode_fmt += 'B' # 1 Byte for Char Pants Color G.
			encode_fmt += 'B' # 1 Byte for Char Pants Color B.
			encode_fmt += 'B' # 1 Byte for Char Pants Color A.
			encode_fmt += 'B' # 1 Byte for Char Boots Color R.
			encode_fmt += 'B' # 1 Byte for Char Boots Color G.
			encode_fmt += 'B' # 1 Byte for Char Boots Color B.
			encode_fmt += 'B' # 1 Byte for Char Boots Color A.
			encode_fmt += 'B' # 1 Byte for Char Helm Color R.
			encode_fmt += 'B' # 1 Byte for Char Helm Color G.
			encode_fmt += 'B' # 1 Byte for Char Helm Color B.
			encode_fmt += 'B' # 1 Byte for Char Helm Color A.
			encode_fmt += 'B' # 1 Byte for Char Robe Color R.
			encode_fmt += 'B' # 1 Byte for Char Robe Color G.
			encode_fmt += 'B' # 1 Byte for Char Robe Color B.
			encode_fmt += 'B' # 1 Byte for Char Robe Color A.

			
    
            
			encodedMessage += struct.pack(encode_fmt,
										  len(thisChar.charName),
										  thisChar.charName, 
										  thisChar.charServerID, 
										  thisChar.charServerModel[4],
										  thisChar.charServerModel[3],
										  thisChar.charServerModel[2],
										  thisChar.charServerModel[1],
										  thisChar.charServerModel[0],
										  thisChar.charClass,
										  thisChar.charRace, 
										  thisChar.charLevel*2,  # Need technique
										  thisChar.hairColor, 
										  thisChar.hairLen, 
										  thisChar.hairStyle,
										  thisChar.faceOpt, 
										  thisChar.robeType, 
										  thisChar.primaryHand, 
										  thisChar.secondaryHand, 
										  thisChar.shieldSlot,
										  thisChar.charAnimation, 
										  thisChar.vanUnusedValue1, 
										  thisChar.chestSlot, 
										  thisChar.bracerSlot, 
										  thisChar.gloveSlot, 
										  thisChar.pantsSlot, 
										  thisChar.bootSlot, 
										  thisChar.helmSlot, 
										  thisChar.vanUnusedValue2, 
										  thisChar.vanUnusedValue3, 
										  thisChar.vanUnusedColor1_R,
										  thisChar.vanUnusedColor1_G,
										  thisChar.vanUnusedColor1_B,
										  thisChar.vanUnusedColor1_A,
										  thisChar.vanUnusedColor2_R,
										  thisChar.vanUnusedColor2_G,
										  thisChar.vanUnusedColor2_B,
										  thisChar.vanUnusedColor2_A,
										  thisChar.vanUnusedColor3_R,
										  thisChar.vanUnusedColor3_G,
										  thisChar.vanUnusedColor3_B,
										  thisChar.vanUnusedColor3_A,
										  thisChar.chestColor_R,
										  thisChar.chestColor_G,
										  thisChar.chestColor_B,
										  thisChar.chestColor_A,
										  thisChar.bracerColor_R,
										  thisChar.bracerColor_G,
										  thisChar.bracerColor_B,
										  thisChar.bracerColor_A,
										  thisChar.gloveColor_R,
										  thisChar.gloveColor_G,
										  thisChar.gloveColor_B,
										  thisChar.gloveColor_A,
										  thisChar.pantsColor_R,
										  thisChar.pantsColor_G,
										  thisChar.pantsColor_B,
										  thisChar.pantsColor_A,
										  thisChar.bootColor_R,
										  thisChar.bootColor_G,
										  thisChar.bootColor_B,
										  thisChar.bootColor_A,
										  thisChar.helmColor_R,
										  thisChar.helmColor_G,
										  thisChar.helmColor_B,
										  thisChar.helmColor_A,
										  thisChar.robeColor_R,
										  thisChar.robeColor_G,
										  thisChar.robeColor_B,
										  thisChar.robeColor_A)
										
		return encodedMessage;
		
class CharSelectMessage(Message):
	"""Handles the client's character selection for play."""
	
	def __init__(self):
		Message.__init__(self)

		
	def decodeMessage(self, messageByteArray):

	  # Currently in the test, the opcode is included in the messageByteList and should be processed
		decode_fmt  = '<'  # Unpack as little endian
		decode_fmt += 'H'  # 2 Byte Opcode
		decode_fmt += 'I'  # 4 Byte Character Side ID
		decode_fmt += 'I'  # 4 Byte Face Option
		decode_fmt += 'I'  # 4 Byte Hair Style
		decode_fmt += 'I'  # 4 Byte Hair Length
		decode_fmt += 'I'  # 4 Byte Hair Color
	  #
		s = struct.unpack(decode_fmt,messageByteArray)             # Actually unpacks the messageByteList
		num_bytes_read = struct.calcsize(decode_fmt)               # Calculate how many bytes will be read
		self.messageByteArray = messageByteArray[num_bytes_read:]  # Remove the bytes we read from the message
		#
		self.messageOpcode = s[0] # Store  the Op Code (should be 0x002A)
		self.charID        = s[1] # Store the Character Side ID
		self.faceOption    = s[2] # Store the Face Option
		self.hairStyle     = s[3] # Store the Hair Style
		self.hairLen       = s[4] # Store the Hair Length
		self.hairColor	   = s[5] # Store the Hair Color
		
		
#class memDump(message):
#
#	def __init__(self):
#	  Message.__init__(self)
#
#	def encodeMessage(self):
#	
#	
#	def buildMessage(self):

	
	
	
		