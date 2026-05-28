'''
Created on May 7, 2016

@author: Stefan McDaniel
'''
import unittest
import eqoa_messages
from eqoa_loginserver import LoginServer, ServerListing
from eqoa_messages import LANGUAGE, RECOMMENDED
from eqoa_character import CharListing
from eqoa_character import (CHAR_RACE, CHAR_CLASS, CHAR_HAIRCOLOR, 
						    CHAR_HAIRLEN, CHAR_HAIRSTYLE, CHAR_FACE,
						    CHAR_ANIMATE, CHAR_SERVERID, CHAR_MODELID,
						    CHAR_ROBE, CHAR_ARMOR, CHAR_GEARCOLOR,
						    CHAR_PRIHAND, CHAR_SECHAND, CHAR_SHIELD,
                            ALPHA)


class MessagesTest(unittest.TestCase):

   def test_LoginMessageDecoding(self):
      messageBytes = [0x04, 0x09, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x45, 0x51, 0x4F, 
                      0x41, 0x0A, 0x00, 0x00, 0x00, 0x6B, 0x69, 0x65, 0x73, 0x68, 0x61, 0x65, 0x73, 0x68, 
                      0x61, 0x01, 0xFA, 0x10, 0x69, 0x22, 0x1C, 0xD4, 0x45, 0xBC, 0xFD, 0x68, 0x3C, 0x56, 
                      0x22, 0x87, 0xD9, 0x70, 0xB7, 0x1C, 0x12, 0xAE, 0x76, 0xC4, 0x98, 0xFD, 0xF3, 0xCE, 
                      0xEB, 0x44, 0x4A, 0x0A, 0x49, 0xB5]
      messageByteArray = "".join( chr( val) for val in messageBytes)
               
      
      message = eqoa_messages.LoginMessage()
      message.decodeMessage(messageByteArray)
      
      self.assertEquals(message.username, "kieshaesha")
      self.assertEqual(len(message.messageByteArray), 0)  # Not sure this is needed, but doesn't hurt

      
   def test_GameVersionDecoding(self):
      # Actual message will probably be stored in a bytearry. For now we can just convert the messageByte list into a messageByteArray. Can put in util file.py
      messageBytes = [0x00, 0x00, 0x25, 0x00, 0x00, 0x00]
      messageByteArray = "".join( chr( val) for val in messageBytes)  

      message = eqoa_messages.GameVersionMessage()
      message.decodeMessage(messageByteArray)
      
      self.assertEquals(message.gameVersion, 0x25)
      self.assertEqual(len(message.messageByteArray), 0)  # Not sure this is needed, but doesn't hurt

      
   def test_GameVersionEncoding(self):
      expectedMessageBytes     = [0x00, 0x00, 0x12, 0x00, 0x00, 0x00]
      expectedMessageByteArray = "".join( chr( val) for val in expectedMessageBytes)  # Converts to byte array instead of list. 
      
      message = eqoa_messages.GameVersionMessage()
      message.buildMessage(eqoa_messages.MessageType.STANDARD_MESSAGE,
                           eqoa_messages.OpCode.GAME_VERSION,
                           eqoa_messages.GAME_VERSION.EQOA_VANILLA)
      
      encodedMessage = message.encodeMessage()
      
      self.assertEqual(encodedMessage, expectedMessageByteArray)
      
   def test_ServerListingMessageEncoding(self):
      # This is server listing message found in Matt's Original PCAP
      expectedMessageBytes     = [0xB3,0x07,0x0E,0x10,0x00,0x00,0x00,0x43,0x00,0x61,0x00,0x73,0x00,0x74,0x00,0x6C,0x00,0x65,0x00,0x20,
                                  0x00,0x4C,0x00,0x69,0x00,0x67,0x00,0x68,0x00,0x74,0x00,0x77,0x00,0x6F,0x00,0x6C,0x00,0x66,0x00,0x00,
                                  0x0A,0x1F,0x57,0x27,0x30,0x0A,0x6C,0xC7,0x00,0x0A,0x00,0x00,0x00,0x44,0x00,0x69,0x00,0x72,0x00,0x65,
                                  0x00,0x6E,0x00,0x20,0x00,0x48,0x00,0x6F,0x00,0x6C,0x00,0x64,0x00,0x00,0x01,0x3E,0x56,0x27,0x48,0x0A,
                                  0x6C,0xC7,0x00,0x0D,0x00,0x00,0x00,0x46,0x00,0x65,0x00,0x72,0x00,0x72,0x00,0x61,0x00,0x6E,0x00,0x27,
                                  0x00,0x73,0x00,0x20,0x00,0x48,0x00,0x6F,0x00,0x70,0x00,0x65,0x00,0x00,0x1C,0x24,0x56,0x27,0x74,0x0A,
                                  0x6C,0xC7,0x00,0x08,0x00,0x00,0x00,0x48,0x00,0x6F,0x00,0x64,0x00,0x73,0x00,0x74,0x00,0x6F,0x00,0x63,
                                  0x00,0x6B,0x00,0x00,0x91,0xB9,0x56,0x27,0x84,0x0A,0x6C,0xC7,0x00,0x0B,0x00,0x00,0x00,0x4D,0x00,0x61,
                                  0x00,0x72,0x00,0x72,0x00,0x27,0x00,0x73,0x00,0x20,0x00,0x46,0x00,0x69,0x00,0x73,0x00,0x74,0x00,0x00,
                                  0x9E,0xBD,0x57,0x27,0x25,0xC8,0x6C,0xC7,0x00,0x11,0x00,0x00,0x00,0x50,0x00,0x72,0x00,0x6F,0x00,0x75,
                                  0x00,0x64,0x00,0x70,0x00,0x69,0x00,0x6E,0x00,0x65,0x00,0x20,0x00,0x4F,0x00,0x75,0x00,0x74,0x00,0x70,
                                  0x00,0x6F,0x00,0x73,0x00,0x74,0x00,0x00,0xA9,0xD7,0x56,0x27,0x44,0xC8,0x6C,0xC7,0x00,0x0D,0x00,0x00,
                                  0x00,0x48,0x00,0x61,0x00,0x67,0x00,0x6C,0x00,0x65,0x00,0x79,0x00,0x20,0x00,0x28,0x00,0x54,0x00,0x65,
                                  0x00,0x73,0x00,0x74,0x00,0x29,0x00,0x01,0x35,0x9B,0x56,0x27,0xEB,0x0A,0x6C,0xC7,0x00]
                                 
      expectedMessageByteArray = "".join( chr( val) for val in expectedMessageBytes)  # Converts to byte array instead of list. 
           
      #build a list of servers - These are servers from Dudderz.
      _serverList = []
      _serverList.append(ServerListing("Castle Lightwolf",  RECOMMENDED.NO  , 0x1F0A, "199.108.10.48" , 10071, LANGUAGE.US_ENGLISH))
      _serverList.append(ServerListing("Diren Hold",        RECOMMENDED.NO  , 0x3E01, "199.108.10.72" , 10070, LANGUAGE.US_ENGLISH))
      _serverList.append(ServerListing("Ferran's Hope",     RECOMMENDED.NO  , 0x241C, "199.108.10.116", 10070, LANGUAGE.US_ENGLISH))
      _serverList.append(ServerListing("Hodstock",          RECOMMENDED.NO  , 0xB991, "199.108.10.132", 10070, LANGUAGE.US_ENGLISH))
      _serverList.append(ServerListing("Marr's Fist",       RECOMMENDED.NO  , 0xBD9E, "199.108.200.37", 10071, LANGUAGE.US_ENGLISH))
      _serverList.append(ServerListing("Proudpine Outpost", RECOMMENDED.NO  , 0xD7A9, "199.108.200.68", 10070, LANGUAGE.US_ENGLISH))
      _serverList.append(ServerListing("Hagley (Test)",     RECOMMENDED.YES , 0x9B35, "199.108.10.235", 10070, LANGUAGE.US_ENGLISH))
   
      #build a couple test user accounts
      _userAccounts = { "user1": "password1", "user2":"password2" }

      #create the login server instance
      testloginServer = LoginServer(_serverList, _userAccounts)
      #
      #       
      message = eqoa_messages.ServerListingMessage()
      message.buildMessage(eqoa_messages.MessageType.STANDARD_MESSAGE,
                           eqoa_messages.OpCode.SERVER_LISTING,
                           testloginServer.getServerList())
           
      
      encodedMessage = message.encodeMessage()
      #print ",".join("0x{:02X}".format(ord(c)) for c in encodedMessage)
          
      self.assertEqual(encodedMessage, expectedMessageByteArray)      # put this back in after we get everything square.
  

   def test_PreCharSelect1Encoding(self):
      expectedMessageBytes     = [0xD1, 0x07, 0x03, 0x00, 0x00, 0x00]
      expectedMessageByteArray = "".join( chr( val) for val in expectedMessageBytes)  # Converts to byte array instead of list. 
      
      message = eqoa_messages.PreCharSelect2Message()
      message.buildMessage(eqoa_messages.MessageType.STANDARD_MESSAGE,
                           eqoa_messages.OpCode.PRECHAR_MSG1,
                           0x03)
      
      encodedMessage = message.encodeMessage()
      
      self.assertEqual(encodedMessage, expectedMessageByteArray)	  
	  
   def test_PreCharSelect2Encoding(self):
      expectedMessageBytes     = [0xF5, 0x07, 0x1B, 0x00, 0x00, 0x00]
      expectedMessageByteArray = "".join( chr( val) for val in expectedMessageBytes)  # Converts to byte array instead of list. 
      
      message = eqoa_messages.PreCharSelect2Message()
      message.buildMessage(eqoa_messages.MessageType.STANDARD_MESSAGE,
                           eqoa_messages.OpCode.PRECHAR_MSG2,
                           0x1B)
      
      encodedMessage = message.encodeMessage()
      
      self.assertEqual(encodedMessage, expectedMessageByteArray)      

   def test_CharListingEncoding(self):        
		expectedMessageBytes   = [0x2c, 0x00, 0x10, 0x05, 0x00, 0x00, 0x00, 0x46, 0x65, 0x72, 0x72, 0x79, 0x90, 0xa0, 0x8b, 0x01, 
								  0xbe, 0xfb, 0xd1, 0x9c, 0x0c, 0x10, 0x0c, 0x78, 0x00, 0x00, 0x04, 0x06, 0x00, 0x00, 0x00, 0x00, 
								  0x85, 0x6c, 0x40, 0xd8, 0xcb, 0x7d, 0x7e, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 
								  0x00, 0x03, 0x03, 0x03, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
								  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
								  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x80, 0x00, 0xff, 0xf0, 0xf0, 0x00, 0xff, 0x00, 
								  0x80, 0x00, 0xff, 0x08, 0x00, 0x00, 0x00, 0x44, 0x61, 0x79, 0x64, 0x72, 0x69, 0x66, 0x74, 0xc6, 
								  0xbb, 0x8b, 0x01, 0x8f, 0xc0, 0xd4, 0xf2, 0x04, 0x18, 0x02, 0x78, 0x00, 0x04, 0x02, 0x02, 0x00, 
								  0x00, 0x00, 0x00, 0xa9, 0xbe, 0x7d, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x29, 0x60, 0xc5, 0x2e, 0x03, 
								  0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0xff, 0xff, 
								  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0x39, 0x64, 0xff, 0x80, 0x00, 0x80, 0xff, 0xff, 0xff, 
								  0xff, 0xff, 0x3f, 0x76, 0x94, 0xff, 0x80, 0x00, 0x80, 0xff, 0x75, 0x00, 0x75, 0xff, 0x44, 0x44, 
								  0x88, 0xff, 0xff, 0x00, 0x00, 0xff, 0x04, 0x00, 0x00, 0x00, 0x4c, 0x65, 0x61, 0x72, 0xfa, 0xd5, 
								  0x8b, 0x01, 0x8f, 0xc0, 0xd4, 0xf2, 0x04, 0x0e, 0x02, 0x78, 0x02, 0x06, 0x02, 0x02, 0x00, 0x00, 
								  0x00, 0x00, 0x85, 0x6c, 0x40, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x73, 0xe8, 0xa6, 0x0a, 0x03, 0x00, 
								  0x00, 0x02, 0x00, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0xff, 0xff, 0xff, 
								  0xff, 0xff, 0xff, 0xff, 0xff, 0x50, 0x64, 0x00, 0xff, 0x00, 0x40, 0x40, 0xff, 0xff, 0xff, 0xff, 
								  0xff, 0x00, 0x40, 0x40, 0xff, 0x00, 0x40, 0x40, 0xff, 0x00, 0x40, 0x40, 0xff, 0x50, 0x64, 0x00, 
								  0xff, 0x00, 0x80, 0x00, 0xff, 0x07, 0x00, 0x00, 0x00, 0x4b, 0x65, 0x6e, 0x63, 0x61, 0x64, 0x65, 
								  0xc4, 0x89, 0x8d, 0x01, 0x8f, 0xc0, 0xd4, 0xf2, 0x04, 0x12, 0x02, 0x78, 0x00, 0x06, 0x06, 0x02, 
								  0x00, 0x00, 0x00, 0x00, 0x85, 0x6c, 0x40, 0xd8, 0xa8, 0xe4, 0x2c, 0x99, 0x00, 0x00, 0x00, 0x00, 
								  0x03, 0x00, 0x00, 0x04, 0x00, 0x04, 0x04, 0x04, 0x04, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00, 0xff, 
								  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1b, 0xed, 0x59, 0xff, 0xff, 
								  0xff, 0xff, 0xff, 0x79, 0x79, 0xbd, 0xff, 0x2d, 0x84, 0x3c, 0xff, 0x79, 0x79, 0xbd, 0xff, 0x9e, 
								  0x9c, 0x38, 0xff, 0x00, 0x80, 0x00, 0xff, 0x0b, 0x00, 0x00, 0x00, 0x48, 0x79, 0x6d, 0x6e, 0x6f, 
								  0x66, 0x70, 0x6f, 0x77, 0x65, 0x72, 0xf2, 0x81, 0x93, 0x01, 0x8f, 0xc0, 0xd4, 0xf2, 0x04, 0x0a, 
								  0x02, 0x78, 0x00, 0x06, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x87, 0xf9, 0x17, 0x1a, 0x99, 
								  0xdd, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 
								  0x00, 0x02, 0x03, 0x03, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
								  0xff, 0x80, 0xff, 0xd6, 0xff, 0x00, 0x9f, 0x9f, 0xff, 0x00, 0x41, 0x82, 0xff, 0x00, 0x9f, 0x9f, 
								  0xff, 0x00, 0x80, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x15, 0x00, 0x00, 0xff, 0x06, 0x00, 0x00, 
								  0x00, 0x4e, 0x65, 0x63, 0x6e, 0x6f, 0x6b, 0xac, 0xc3, 0x94, 0x01, 0xd5, 0xd3, 0x9c, 0xe6, 0x0a, 
								  0x16, 0x06, 0x78, 0x06, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa9, 0xbe, 0x7d, 0x7c, 0x7c, 
								  0x8b, 0x08, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
								  0xff, 0xff, 0x96, 0x00, 0x3c, 0xff, 0xff, 0xff, 0xff, 0xff, 0x96, 0x00, 0x3c, 0xff, 0x96, 0x00, 
								  0x3c, 0xff, 0x31, 0x62, 0x62, 0xff, 0x15, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x07, 0x00, 
								  0x00, 0x00, 0x44, 0x75, 0x64, 0x64, 0x65, 0x72, 0x7a, 0xf6, 0xc3, 0x95, 0x01, 0xe4, 0x94, 0xfd, 
								  0xc6, 0x09, 0x00, 0x0a, 0x78, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x22, 0xb7, 
								  0x6e, 0x0b, 0x87, 0xf9, 0x17, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x04, 0x00, 0x04, 0x04, 
								  0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
								  0xff, 0xff, 0xff, 0xff, 0x62, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x40, 0x00, 0x80, 0xff, 
								  0xa0, 0xaa, 0xb4, 0xff, 0x40, 0x00, 0x80, 0xff, 0x40, 0x00, 0x80, 0xff, 0x00, 0x00, 0xff, 0xff, 
								  0x0c, 0x00, 0x00, 0x00, 0x43, 0x6f, 0x72, 0x73, 0x74, 0x65, 0x6e, 0x73, 0x62, 0x61, 0x6e, 0x6b, 
								  0xba, 0xef, 0x99, 0x01, 0x8c, 0xe3, 0xc4, 0x8d, 0x0e, 0x14, 0x00, 0x02, 0x02, 0x04, 0x04, 0x04, 
								  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
								  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
								  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
								  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff]



  
								
		expectedMessageByteArray = "".join( chr( val) for val in expectedMessageBytes)  # Converts to byte array instead of list.

		# Build a list of Characters. - These are characters from Dudderz. Still need to add the last 4. 

		_charList = []
		_charList.append(CharListing("Ferry", CHAR_SERVERID.FERRY, CHAR_MODELID.BAR_MALE, CHAR_CLASS.SHA, CHAR_RACE.BAR, 60, CHAR_HAIRCOLOR.BLACK, 
									CHAR_HAIRLEN.LEN1, CHAR_HAIRSTYLE.STYLE3, CHAR_FACE.FACE4, CHAR_ROBE.ARCANE, CHAR_PRIHAND.CLERIC_EPIC_GH,
									CHAR_SECHAND.LEATHER_BOOK, CHAR_SHIELD.NOSHIELD, CHAR_ANIMATE.B1H, 0X00, CHAR_ARMOR.CHAIN, CHAR_ARMOR.NOARMOR,
									CHAR_ARMOR.CHAIN, CHAR_ARMOR.CHAIN, CHAR_ARMOR.CHAIN, CHAR_ARMOR.SCALE, 0X0000, 0X00000000, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_GREEN.R, CHAR_GEARCOLOR.VAN_GREEN.G, CHAR_GEARCOLOR.VAN_GREEN.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_YELLOW.R, CHAR_GEARCOLOR.VAN_YELLOW.G, CHAR_GEARCOLOR.VAN_YELLOW.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_GREEN.R, CHAR_GEARCOLOR.VAN_GREEN.G, CHAR_GEARCOLOR.VAN_GREEN.B, ALPHA.ON))
									
		_charList.append(CharListing("Daydrift", CHAR_SERVERID.DAYDRIFT, CHAR_MODELID.ELF_MALE, CHAR_CLASS.ENC, CHAR_RACE.ELF, 60, CHAR_HAIRCOLOR.BLACK, 
									CHAR_HAIRLEN.LEN3, CHAR_HAIRSTYLE.STYLE2, CHAR_FACE.FACE2, CHAR_ROBE.ARCANE, CHAR_PRIHAND.WIZ_EPIC_STAFF,
									CHAR_SECHAND.DEFAULT, CHAR_SHIELD.FORCE_SHIELD, CHAR_ANIMATE.B1H, 0X00, CHAR_ARMOR.PADDED, CHAR_ARMOR.NOARMOR,
									CHAR_ARMOR.PADDED, CHAR_ARMOR.PADDED, CHAR_ARMOR.PADDED, CHAR_ARMOR.PADDED, 0X0000, 0X00000302, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.CAT_BLUE.R, CHAR_GEARCOLOR.CAT_BLUE.G, CHAR_GEARCOLOR.CAT_BLUE.B, ALPHA.ON,
									CHAR_GEARCOLOR.PURPLE1.R, CHAR_GEARCOLOR.PURPLE1.G, CHAR_GEARCOLOR.PURPLE1.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.JB_BLUE.R, CHAR_GEARCOLOR.JB_BLUE.G, CHAR_GEARCOLOR.JB_BLUE.B, ALPHA.ON,
									CHAR_GEARCOLOR.PURPLE1.R, CHAR_GEARCOLOR.PURPLE1.G, CHAR_GEARCOLOR.PURPLE1.B, ALPHA.ON,
									CHAR_GEARCOLOR.PURPLE2.R, CHAR_GEARCOLOR.PURPLE2.G, CHAR_GEARCOLOR.PURPLE2.B, ALPHA.ON,
									CHAR_GEARCOLOR.JACK_PURPLE.R, CHAR_GEARCOLOR.JACK_PURPLE.G, CHAR_GEARCOLOR.JACK_PURPLE.B, ALPHA.ON,
									CHAR_GEARCOLOR.RED2.R, CHAR_GEARCOLOR.RED2.G, CHAR_GEARCOLOR.RED2.B, ALPHA.ON))

		_charList.append(CharListing("Lear", CHAR_SERVERID.LEAR, CHAR_MODELID.ELF_MALE, CHAR_CLASS.DRD, CHAR_RACE.ELF, 60, CHAR_HAIRCOLOR.BROWN, 
									CHAR_HAIRLEN.LEN4, CHAR_HAIRSTYLE.STYLE2, CHAR_FACE.FACE2, CHAR_ROBE.ARCANE, CHAR_PRIHAND.CLERIC_EPIC_GH,
									CHAR_SECHAND.DEFAULT, CHAR_SHIELD.BANGLE_SHIELD, CHAR_ANIMATE.B1H, 0X00, CHAR_ARMOR.LEATHER, CHAR_ARMOR.NOARMOR,
									CHAR_ARMOR.LEATHER, CHAR_ARMOR.LEATHER, CHAR_ARMOR.LEATHER, CHAR_ARMOR.LEATHER, 0X0000, 0X00030303, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.VER_GREEN.R, CHAR_GEARCOLOR.VER_GREEN.G, CHAR_GEARCOLOR.VER_GREEN.B, ALPHA.ON,
									CHAR_GEARCOLOR.CYPRUS.R, CHAR_GEARCOLOR.CYPRUS.G, CHAR_GEARCOLOR.CYPRUS.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.CYPRUS.R, CHAR_GEARCOLOR.CYPRUS.G, CHAR_GEARCOLOR.CYPRUS.B, ALPHA.ON,
									CHAR_GEARCOLOR.CYPRUS.R, CHAR_GEARCOLOR.CYPRUS.G, CHAR_GEARCOLOR.CYPRUS.B, ALPHA.ON,
									CHAR_GEARCOLOR.CYPRUS.R, CHAR_GEARCOLOR.CYPRUS.G, CHAR_GEARCOLOR.CYPRUS.B, ALPHA.ON,
									CHAR_GEARCOLOR.VER_GREEN.R, CHAR_GEARCOLOR.VER_GREEN.G, CHAR_GEARCOLOR.VER_GREEN.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_GREEN.R, CHAR_GEARCOLOR.VAN_GREEN.G, CHAR_GEARCOLOR.VAN_GREEN.B, ALPHA.ON))							

		_charList.append(CharListing("Kencade", CHAR_SERVERID.KENCADE, CHAR_MODELID.ELF_MALE, CHAR_CLASS.CL, CHAR_RACE.ELF, 60, CHAR_HAIRCOLOR.BLACK, 
									CHAR_HAIRLEN.LEN4, CHAR_HAIRSTYLE.STYLE4, CHAR_FACE.FACE2, CHAR_ROBE.ARCANE, CHAR_PRIHAND.CLERIC_EPIC_GH,
									CHAR_SECHAND.LIGHT_TOTEM, CHAR_SHIELD.NOSHIELD, CHAR_ANIMATE.B1H, 0X00, CHAR_ARMOR.PLATE, CHAR_ARMOR.NOARMOR,
									CHAR_ARMOR.PLATE, CHAR_ARMOR.PLATE, CHAR_ARMOR.PLATE, CHAR_ARMOR.PLATE, 0X0001, 0X00030003, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.MALACHITE.R, CHAR_GEARCOLOR.MALACHITE.G, CHAR_GEARCOLOR.MALACHITE.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.MOODY_BLUE.R, CHAR_GEARCOLOR.MOODY_BLUE.G, CHAR_GEARCOLOR.MOODY_BLUE.B, ALPHA.ON,
									CHAR_GEARCOLOR.JAP_LAUREL.R, CHAR_GEARCOLOR.JAP_LAUREL.G, CHAR_GEARCOLOR.JAP_LAUREL.B, ALPHA.ON,
									CHAR_GEARCOLOR.MOODY_BLUE.R, CHAR_GEARCOLOR.MOODY_BLUE.G, CHAR_GEARCOLOR.MOODY_BLUE.B, ALPHA.ON,
									CHAR_GEARCOLOR.HIGHBALL.R, CHAR_GEARCOLOR.HIGHBALL.G, CHAR_GEARCOLOR.HIGHBALL.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_GREEN.R, CHAR_GEARCOLOR.VAN_GREEN.G, CHAR_GEARCOLOR.VAN_GREEN.B, ALPHA.ON))

		_charList.append(CharListing("Hymnofpower", CHAR_SERVERID.HYMNOFPOWER, CHAR_MODELID.ELF_MALE, CHAR_CLASS.BRD, CHAR_RACE.ELF, 60, CHAR_HAIRCOLOR.BLACK, 
									CHAR_HAIRLEN.LEN4, CHAR_HAIRSTYLE.STYLE3, CHAR_FACE.FACE2, CHAR_ROBE.DIVINE, CHAR_PRIHAND.WAR_EPIC_LS,
									CHAR_SECHAND.BRD_EPIC_RAP, CHAR_SHIELD.NOSHIELD, CHAR_ANIMATE.P1HO, 0X00, CHAR_ARMOR.CHAIN, CHAR_ARMOR.CHAIN,
									CHAR_ARMOR.CHAIN, CHAR_ARMOR.CHAIN, CHAR_ARMOR.CHAIN, CHAR_ARMOR.NOARMOR, 0X0003, 0X00030302, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.AQUA.R, CHAR_GEARCOLOR.AQUA.G, CHAR_GEARCOLOR.AQUA.B, ALPHA.ON,
									CHAR_GEARCOLOR.PER_GREEN.R, CHAR_GEARCOLOR.PER_GREEN.G, CHAR_GEARCOLOR.PER_GREEN.B, ALPHA.ON,
									CHAR_GEARCOLOR.DARK_CER.R, CHAR_GEARCOLOR.DARK_CER.G, CHAR_GEARCOLOR.DARK_CER.B, ALPHA.ON,
									CHAR_GEARCOLOR.PER_GREEN.R, CHAR_GEARCOLOR.PER_GREEN.G, CHAR_GEARCOLOR.PER_GREEN.B, ALPHA.ON,
									CHAR_GEARCOLOR.CERULEAN.R, CHAR_GEARCOLOR.CERULEAN.G, CHAR_GEARCOLOR.CERULEAN.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.BLACK.R, CHAR_GEARCOLOR.BLACK.G, CHAR_GEARCOLOR.BLACK.B, ALPHA.ON))								

		_charList.append(CharListing("Necnok", CHAR_SERVERID.NECNOK, CHAR_MODELID.GNO_MALE, CHAR_CLASS.NEC, CHAR_RACE.GNO, 60, CHAR_HAIRCOLOR.GREY, 
									CHAR_HAIRLEN.LEN4, CHAR_HAIRSTYLE.STYLE2, CHAR_FACE.FACE1, CHAR_ROBE.ARCANE, CHAR_PRIHAND.WIZ_EPIC_STAFF,
									CHAR_SECHAND.ENC_WAND_CHARM, CHAR_SHIELD.NOSHIELD, CHAR_ANIMATE.B1H, 0X00, CHAR_ARMOR.PADDED, CHAR_ARMOR.NOARMOR,
									CHAR_ARMOR.PADDED, CHAR_ARMOR.PADDED, CHAR_ARMOR.PADDED, CHAR_ARMOR.PADDED, 0X0000, 0X00000000, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_WINE.R, CHAR_GEARCOLOR.VAN_WINE.G, CHAR_GEARCOLOR.VAN_WINE.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_WINE.R, CHAR_GEARCOLOR.VAN_WINE.G, CHAR_GEARCOLOR.VAN_WINE.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_WINE.R, CHAR_GEARCOLOR.VAN_WINE.G, CHAR_GEARCOLOR.VAN_WINE.B, ALPHA.ON,
									CHAR_GEARCOLOR.ORACLE.R, CHAR_GEARCOLOR.ORACLE.G, CHAR_GEARCOLOR.ORACLE.B, ALPHA.ON,
									CHAR_GEARCOLOR.BLACK.R, CHAR_GEARCOLOR.BLACK.G, CHAR_GEARCOLOR.BLACK.B, ALPHA.ON,
									CHAR_GEARCOLOR.RED2.R, CHAR_GEARCOLOR.RED2.G, CHAR_GEARCOLOR.RED2.B, ALPHA.ON))
									
		_charList.append(CharListing("Dudderz", CHAR_SERVERID.DUDDERZ, CHAR_MODELID.TRL_MALE, CHAR_CLASS.WAR, CHAR_RACE.TRL, 60, CHAR_HAIRCOLOR.BLACK, 
									CHAR_HAIRLEN.LEN4, CHAR_HAIRSTYLE.STYLE1, CHAR_FACE.FACE3, CHAR_ROBE.ARCANE, CHAR_PRIHAND.SK_EPIC_GS,
									CHAR_SECHAND.WAR_EPIC_LS, CHAR_SHIELD.NOSHIELD, CHAR_ANIMATE.S1HO, 0X00, CHAR_ARMOR.PLATE, CHAR_ARMOR.NOARMOR,
									CHAR_ARMOR.PLATE, CHAR_ARMOR.PLATE, CHAR_ARMOR.PLATE, CHAR_ARMOR.PLATE, 0X0000, 0X00000000, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.MAROON.R, CHAR_GEARCOLOR.MAROON.G, CHAR_GEARCOLOR.MAROON.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_COBALT.R, CHAR_GEARCOLOR.VAN_COBALT.G, CHAR_GEARCOLOR.VAN_COBALT.B, ALPHA.ON,
									CHAR_GEARCOLOR.CHATEAU.R, CHAR_GEARCOLOR.CHATEAU.G, CHAR_GEARCOLOR.CHATEAU.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_COBALT.R, CHAR_GEARCOLOR.VAN_COBALT.G, CHAR_GEARCOLOR.VAN_COBALT.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_COBALT.R, CHAR_GEARCOLOR.VAN_COBALT.G, CHAR_GEARCOLOR.VAN_COBALT.B, ALPHA.ON,
									CHAR_GEARCOLOR.VAN_BLUE.R, CHAR_GEARCOLOR.VAN_BLUE.G, CHAR_GEARCOLOR.VAN_BLUE.B, ALPHA.ON))

		_charList.append(CharListing("Corstensbank", CHAR_SERVERID.CORSTENSBANK, CHAR_MODELID.HUM_MALE, CHAR_CLASS.MAG, CHAR_RACE.HUM, 1, CHAR_HAIRCOLOR.BROWN, 
									CHAR_HAIRLEN.LEN3, CHAR_HAIRSTYLE.STYLE3, CHAR_FACE.FACE3, CHAR_ROBE.NOROBE, CHAR_PRIHAND.DEFAULT,
									CHAR_SECHAND.DEFAULT, CHAR_SHIELD.NOSHIELD, CHAR_ANIMATE.STND, 0X00, CHAR_ARMOR.NOARMOR, CHAR_ARMOR.NOARMOR,
									CHAR_ARMOR.NOARMOR, CHAR_ARMOR.NOARMOR, CHAR_ARMOR.NOARMOR, CHAR_ARMOR.NOARMOR, 0X0000, 0X00000000, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON, 
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON,
									CHAR_GEARCOLOR.DEFAULT.R, CHAR_GEARCOLOR.DEFAULT.G, CHAR_GEARCOLOR.DEFAULT.B, ALPHA.ON))										
									
		message = eqoa_messages.CharListingMessage()
		message.buildMessage(eqoa_messages.MessageType.STANDARD_MESSAGE,
						     eqoa_messages.OpCode.CHAR_LISTING, _charList)
							 
			    
		encodedMessage = message.encodeMessage() 
		
        
		self.assertEqual(encodedMessage, expectedMessageByteArray)
        
   def test_CharSelectDecoding(self):
	# Actual message will probably be stored in a bytearry.
		messageBytes = [0x2A, 0x00, 0xFB, 0xB0, 0x12, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
						0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
		messageByteArray = "".join( chr( val) for val in messageBytes)  

		message = eqoa_messages.CharSelectMessage()
		message.decodeMessage(messageByteArray)

		self.assertEquals(message.messageOpcode, 0x002A)
		self.assertEquals(message.charID,        0x0012B0FB)
		self.assertEquals(message.faceOption,    0x00000002)
		self.assertEquals(message.hairStyle,     0x00000000)
		self.assertEquals(message.hairLen,       0x00000003)
		self.assertEquals(message.hairColor,     0x00000000)
		self.assertEqual(len(message.messageByteArray), 0)  # Not sure this is needed, but doesn't hurt      
	  
if __name__ == "__main__":
   #import sys;sys.argv = ['', 'Test.testName']
   unittest.main()