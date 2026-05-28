#!/usr/bin/env python
#
# Devin and Ben 
# Feb 23, 2019 
#
import asyncio, struct, logging, sys
from CharacterList import *
from Character import *
from utilities import datetime_to_dnp3
#
####################################################################################
#
#  These are where the actual opcodes are decoded
#  Should this be a class with an encode and decode? 
#
class discVersion:
  EQOA_FRONTIERS       = 0x25   #  add betas
  EQOA_VANILLA         = 0x12
  UNKNOWN              = 0x00
  # 
  lookup = {EQOA_FRONTIERS:'EQOA: Frontiers',EQOA_VANILLA:'EQOA: Vanilla',UNKNOWN:'Unknown'}

class opName:
  DISC_VERSION      = 0x0000  #
  AUTHENTICATE      = 0x0904  #
  SERVER_LISTING    = 0x07B3  #
  PRECHAR_MSG1      = 0x07D1  #
  PRECHAR_MSG2      = 0x07F5  #
  CHAR_LISTING      = 0x002C  #
  AUTHENTICATE2     = 0x0001  #
  SELECTED_CHAR     = 0x002A  #
  TIME              = 0x0013  #
  CHARACTER_DUMP    = 0x000D  #
  
#
####################################################################################
#
class opcodeOperations:
  
  def __init__(self, myRdpComm):
    self.myRdpComm           = myRdpComm
    self.createCharacterList = createCharacterList(self.myRdpComm)
    self.Character           = CharacterData(self.myRdpComm)
    self.opName              = opName 
   
  async def processMessage(self, myMessage):
    #
    # Extract opcode from Message
    #
    self.myRdpComm.log.info('Extracting OpCode')
    myOpcode        = struct.unpack('<H',myMessage[:2])[0]
    myOpcodePayload = myMessage[2:]
    #
    # Determine which opcode and decode
    #
    self.myRdpComm.log.info('  ')
    if (myOpcode == opName.DISC_VERSION):
      self.opcodeDiscVersion(myOpcodePayload)
    #
    elif (myOpcode == opName.AUTHENTICATE):
      await self.opcodeAuthenticate(myOpcodePayload, myOpcode)
    #
    elif (myOpcode == opName.AUTHENTICATE2):
      await self.opcodeAuthenticate(myOpcodePayload, myOpcode)
    #
    elif (myOpcode == opName.SELECTED_CHAR):
    #client sends this opcode inbetween each dump packet, seems to indicate it is ready for another packet?
      self.myRdpComm.outgoing_rdp_report = False
      if self.Character.dumpstarted == False:
        #collect DB session
        self.Character.gatherSession()
        #This updates options for character into database, then saves the characterID in the
        #Character class for character memory dump
        self.Character.characterSelectSave(myOpcodePayload)
        #Appear to need those timestamp opcode before memory dump
        #No idea why
        await self.createDNP3Time()
        #Character memory dump
        await self.Character.characterDump(opName.CHARACTER_DUMP)
        #Delete DB session
        self.Character.closeSession()
      else:
        #Means dump has started, client must be ready for another packet.
        self.myRdpComm.log.info('Client is ready for another Dump packet')
        self.Character.ready = True

    else:
      self.myRdpComm.log.info('  Unknown Opcode: ' + hex(myOpcode))
      print(myMessage)
  #
  def opcodeDiscVersion(self, opcodePayload):  # 0x0000
    #
    self.myRdpComm.log.info('  Opcode: DISC_VERSION (0x0000)')
    #myRdpComm.log.info('     Payload: '+' '.join('%02X' % i for i in opcodePayload))
    #  
    # DECODE
    #
    clientDiscVersion = struct.unpack('<I',opcodePayload)[0] 
    #
    # make this nicer later. but just get rough outline in
    #
    if clientDiscVersion == discVersion.EQOA_FRONTIERS:
      # 
      self.myRdpComm.log.info('     Client Disc Identified: '+ discVersion.lookup.get(clientDiscVersion))
      self.myRdpComm.mySession.clientDisc = clientDiscVersion 
      isGood = True
    # 
    elif clientDiscVersion == discVersion.EQOA_VANILLA:
      # 
      self.myRdpComm.log.info('     Client Disc Identified: '+ discVersion.lookup.get(clientDiscVersion)) 
      self.myRdpComm.mySession.clientDisc = clientDiscVersion 
      isGood = False # probably not supporting Vanilla for now
    else:
      # 
      self.myRdpComm.log.info('     Client Disc Unidentified: {:04X}'.format(clientDiscVersion)) 
      isGood = False # print it and stop - don't return message unless disc is frontiers
    #
    # Construct response (should this be in class and be method?)
    #
    self.myRdpComm.outgoing_msg_bundle = True  # so there is a response
    #
    if isGood:
      self.myRdpComm.outgoing_rdp_report = True
      myresponse = struct.pack('<HI',opName.DISC_VERSION,discVersion.EQOA_FRONTIERS)
      self.myRdpComm.myRdpMessages.FB_messageListOut.put_nowait(myresponse)
    #  
    # ACTIONS
    #  0) send log message (done) 
    #  1) need to save the disc the client is using to the client (done) 
    #  2) need to send acknowledgement message back to client

  #
  ####################################################################################
  #
  async def opcodeAuthenticate(self, opcodePayload, myOpcode):  # 0x0904
    #
    if myOpcode == opName.AUTHENTICATE:
      self.myRdpComm.log.info('  Opcode: AUTHENTICATE (0x0904)')
    elif myOpcode == opName.AUTHENTICATE2:
      self.myRdpComm.log.info('  Opcode: AUTHENTICATE2 (0x0001)')
      
    #myRdpComm.log.info('     Payload: '+' '.join('%02X' % i for i in opcodePayload))
    #
    # DECODE
    #
    [opcodeOption, Unknown1, gameLength] = struct.unpack('<BII',opcodePayload[:9])
    opcodePayload = opcodePayload[9:]
    self.myRdpComm.log.info('     Opcode Option   : {:}'.format(opcodeOption)) 
    self.myRdpComm.log.info('     Unknown Option  : {:}'.format(Unknown1)) 
    #
    [gameCode] = struct.unpack('<{:d}s'.format(gameLength),opcodePayload[:gameLength])
    self.myRdpComm.log.info('     Game Code       : {:}'.format(str(gameCode,'ascii'))) 
    opcodePayload = opcodePayload[gameLength:]
    #
    [nameLength] = struct.unpack('<I',opcodePayload[:4])
    opcodePayload = opcodePayload[4:]
    [username]   = struct.unpack('<{:}s'.format(nameLength),opcodePayload[:nameLength])
    #Helps removes bytes object and shave off any nulls
    #
    username = str(username,'ascii')
    #
    opcodePayload = opcodePayload[nameLength:]
    self.myRdpComm.log.info('     Username        : {:}'.format(username)) 
    # 
    opcodePayload = opcodePayload[1:]  # remove the 0x01 that marks end of username
    #myRdpComm.log.info('     Payload: '+' '.join('%02X' % i for i in opcodePayload))
    #
    # should I read as 32 byte value or 2 16 byte values?
    #  
    [encrypted1,encrypted2] = struct.unpack('<16s16s',opcodePayload[:32])
    self.myRdpComm.log.info('     Encrypted1: '+' '.join('%02X' % i for i in encrypted1))
    self.myRdpComm.log.info('     Encrypted2: '+' '.join('%02X' % i for i in encrypted2))
    #Calls method to decrypt the encrypted passwords with wiki server
    self.myRdpComm.log.info('Decrypting Password')
    #password1 = await decryptPassword(encrypted1, myRdpComm)
    password1 = 'password'
  
    response, accountID = self.authenticate(username, password1)
    if response:
      #This helps to pass accountID back into rdpComm session and eventually pass it into the master session.
      self.myRdpComm.mySession.accountID = accountID
      opcodePayload = opcodePayload[32:]
      self.myRdpComm.log.info('Username and Password have been authenticated')
    
      if len(opcodePayload) > 0: # shouldn't catch anything
        self.myRdpComm.log.info('     Extra Bytes in Payload (IDK): '+' '.join('%02X' % i for i in opcodePayload))
      #
      # need to check that pwd is correct and save authenticate to session
      # need to save opcode option to session 
      # need to save unknown value to session
      #
      #Do we really want to keep this?
      self.myRdpComm.mySession.authenticated       = True 
      self.myRdpComm.mySession.auth_opcode_option  = opcodeOption
      self.myRdpComm.mySession.auth_opcode_unknown = Unknown1
    else:
      #Authentication failed, should we just return?
      self.myRdpComm.log.info('Authentication failed with server')
      return
    #
    # Actions 
    # 0) send log message
    # 1) not sure what opcode option does (here 1 or 3) 
    # 2) make sure that EQOA is the correct game
    # 3) authenticate using password server( async?)
    # 4) if correct, set flag on session that it is authenticated
    #     ideally this would get checked before a message goes out



  #
  ####################################################################################
  #
  async def opcodeServerListing(self):  # 0x07B3
    #
    self.myRdpComm.log.info('     Requesting Servers from worldManager')

    reader, writer = await asyncio.open_connection('0.0.0.0', 9737)

    msg = 'Servers please'
    writer.write(msg.encode())
    data = await reader.read(1024)
    self.myRdpComm.log.info('  New server list status received')
    #Temporarily using to progress
    #Is this ok?
    self.myRdpComm.outgoing_msg_bundle = True
    self.myRdpComm.outgoing_rdp_report  = False
    self.myRdpComm.outgoing_session_ack = False
    #Packs our Server select opcode onto the data
    serverList = struct.pack('<H', opName.SERVER_LISTING) + data
      
    self.myRdpComm.myRdpMessages.FC_messageListOut.put_nowait(serverList)

  
  
  #
  ####################################################################################
  #
  async def decryptPassword(self, ciphertext):
    #myRdpComm.log.info(" Opening Connection to PWD Authenticator")
    #
    # this uses loop, but it is not sent in. Should we generate new loop here? or use old loop
    # think about this.. 
    #
    #myRdpComm.log.info('Grabbing Asyncio loop')
    #asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())
    loop2 = asyncio.get_event_loop()
    self.myRdpComm.log.info('Checking for SSL')
    if PW_SSL:
      #myRdpComm.log.info(" Using SSL for PWD AUTH")
      sslcontext = ssl.create_default_context(purpose=ssl.Purpose.SERVER_AUTH)
      sslcontext.check_hostname = True 
      sslcontext.load_verify_locations('../clientcerts/root.pem')
      sslcontext.load_cert_chain(certfile='../clientcerts/client.crt', keyfile='../clientcerts/client.key')
      reader, writer = await asyncio.open_connection('www.eqoadev.wiki', 9997, loop = loop2,ssl = sslcontext)
        
    else:
      #myRdpComm.log.info(" Not using SSL for PWD AUTH")
      reader, writer = await asyncio.open_connection('www.eqoadev.wiki', 9997, loop = loop2)
    #
    self.myRdpComm.log.info(" Packing Ciphertext")
    packed_cipher = struct.pack('>L16s',2,ciphertext) # zero is decrypt
    writer.write(packed_cipher)
    #myRdpComm.log.info(" Transmited Ciphertext")
    #
    cleartext1  = await reader.read(1000)
    #
    #myRdpComm.log.info(" Received Cleartext")
    cleartext  = struct.unpack('>16s',cleartext1)[0].decode() 
    #
    #myRdpComm.log.info(" Clearpass: %s",cleartext)  # Eventually will send back hash
    #
    writer.close()
    cleartext = cleartext.strip('\x00')
    return cleartext

#
#######################################################################################
#
  def authenticate(self, username, password):
    #myRdpComm.log.info('Authenticating user')
    #For skipping password auth server
    session = get_db_session()
    password = password.encode()
    row = session.query(AccountInfo).filter(AccountInfo.username == username).first()
    if (row.username == username):
      if bcrypt.checkpw(password, row.pwhash):
        #myRdpComm.log.info(' Password is True')
        self.myRdpComm.log.info('>>>> Account ID is {}.'.format(row.accountid))
        accountID = row.accountid
        session.close()
        return True, accountID
      else:
        session.close()
        return False
    else:
      session.close()
      return False
    
#
####################################################################################
#

  async def initiateSession(self):
    #Manually packing the packets 

    self.myRdpComm.log.info('Creating packet for Server Master Session') 
    Message1 = struct.pack('<HHH', 0x07D1, 0x0003, 0x0000)
    Message2 = struct.pack('<HHH', 0x07F5, 0x001B, 0x0000)
    totMessages = [Message1, Message2]
    for message in totMessages:
      self.myRdpComm.myRdpMessages.FB_messageListOut.put_nowait(message)
    self.myRdpComm.processedMessages         = 1
    self.myRdpComm.outgoing_msg_bundle       = True
    self.myRdpComm.outgoing_rdp_report       = False
    self.myRdpComm.outgoing_session_ack      = False
    
  async def createDNP3Time(self):
    outgoingPacket = struct.pack('<HQQ', opName.TIME, datetime_to_dnp3(), datetime_to_dnp3() + 1350000)
    self.myRdpComm.myRdpMessages.FB_messageListOut.put_nowait(outgoingPacket)
