#!/usr/bin/env python
#
# Devin and Ben 
# June 28, 2018 
#
import asyncio, socket, uvloop, struct, logging
import configparser,datetime, ssl, sys 
from eqoa_Account import *


sys.path.insert(0, '../framework')
from eqoa_utilities import *
#
#  Read Config File
#
configfile_name = "../config/eqoa_revival_config.ini"
config = configparser.ConfigParser()
config.read(configfile_name)
#
#  Start up Logging
#
LOG_FORMAT = '%(asctime)-15s | %(name)-12s | %(message)s'
logging.basicConfig(filename='eqoa_loginserver.log',level=logging.INFO,format=LOG_FORMAT)
log = logging.getLogger('LoginServer3')
log.setLevel(logging.INFO)
#
#  Start up  DB Connector
#
from eqoa_BaseDBsetup import get_db_session 
#
# see https://stackoverflow.com/questions/49178659/sqlalchemy-classes-across-files-tables-are-not-created
#
# Set some local 
BUFFER_SIZE = int(config.get('loginserver','server_buffer')) 
SERVER_PORT = int(config.get('loginserver','server_port')) 
SERVER_IP   = config.get('loginserver','server_ip')
#
PW_BUFFER_SIZE = int(config.get('password','pw_buffer'))
PW_SERVER_PORT = int(config.get('password','pw_port'))
PW_SERVER_IP   = config.get('password','pw_host')
PW_SIMPLE_FLAG = config.getboolean('password','pw_simple_flag')
PW_SIMPLE      = config.get('password','pw_simple')
PW_SSL         = config.getboolean('password','pw_ssl') 

#
#
#####################################################################            
#
#  Simple Enumerations
#
# might need to divide into before authentication messages and after
# or CLIENT vs. SERVER MESSAGES #
#
class MessageType:  
    #
    DISABLED_FEATURE         = 0x01  
    LOGIN_RESPONSE           = 0x25  # FROM SERVER   # IMPLEMENTED
    ACCT_CREATE_RESPONSE     = 0x29  # FROM SERVER
    CHANGE_PASSWORD_RESPONSE = 0X2B  # FROM SERVER
    INVALID_KEY              = 0X2D  # FROM SERVER
    NO_KEY                   = 0X2F  # FROM SERVER
    REQUEST_TIMED_OUT        = 0X31  # FROM SERVER
    BILLING_RESPONSE         = 0X33  # FROM SERVER
    NO_SUB_TO_EQOA           = 0X35  # FROM SERVER
    GAME_CARD_RESPONSE       = 0X37  # FROM SERVER
    ACQUIRING_SUB            = 0X3F  # FROM SERVER
    UPDATE_ACCT              = 0X41  # FROM SERVER
    FORGOT_PASSWORD_RESPONSE = 0X43  # FROM SERVER
#
#
#  Appear to be all even, so gives a good idea where to search 
#
    REQUEST_CHANGE_PWD       = 0x02  # FROM CLIENT - sends two messages
    LOGIN_REQUEST            = 0x24  # FROM CLIENT   # IMPLEMENTED 
    SUBMIT_ACCT_CREATE       = 0x38  # FROM CLIENT   # IMPLEMENTED 
                                     # sends size, type, 4 bytes of zeros, then accountID,encrypted(old), encrypted(new)
                                     # should be able to implement
    CHANGE_PASSWORD          = 0X2A  # FROM CLIENT
    CONSUME_GAMECARD         = 0x2E  # FROM CLIENT
    REQUEST_ACCT_KEY         = 0x30  # FROM CLIENT
    REQUEST_CANCEL_ACCT      = 0x32  # FROM CLIENT
    REQUEST_GAME_CARD        = 0x36  # FROM CLIENT
    REQUEST_SUBSCRIPTION     = 0x3E  # FROM CLIENT
    REQUEST_UPDATE_ACCT      = 0x40  # FROM CLIENT
    FORGOT_PASSWORD          = 0X42  # FROM CLIENT


class LoginResults:
    #
    NO_USER    = 0x00 
    BAD_PASS   = 0x01 
    NAME_TAKEN = 0x02 
    ERROR      = 0x07 
    GOOD       = 0x08 

class AcctStatus:
    # 
    INACTIVE         = 0x00
    NORMAL           = 0x01
    REQUESTKEY       = 0x02
    UNKNOWN          = 0x03
    UNKNOWN_ERR      = 0x04
    NORMAL2          = 0x05
    TRIAL_EXP        = 0x06
    NORMAL_SUSPENDED = 0x07
    BANNED           = 0x08
    NORMAL_RENTAL    = 0x09
    EXPIRED_RENTAL   = 0x0A
    UNKNOWN_ERR2     = 0x0B
    NOT_TESTEDC      = 0x0C
    NOT_TESTEDD      = 0x0D
    NOT_TESTEDE      = 0x0E
    NOT_TESTEDF      = 0x0F
#
#####################################################################            
#
class tcpClient():
  totClients = 0
  nextID     = 1
 
  def __init__(self,ip,port,reader,writer): 
    self.TotAdd() # updates number of clients and last client number 
    self.ip = ip
    self.port = port
    self.reader = reader
    self.data = None  # for now, will get written in recieve_data 
    self.writer = writer 
    self.account = Account() 

  def TotAdd(self): 
    tcpClient.totClients +=1
    self.id = tcpClient.nextID 
    tcpClient.nextID += 1
 
  def TotSub(self): 
    tcpClient.totClients -=1 
#
#####################################################################            
#
def binaryToASCII(s):
  return bytes(s[:s.find(b'\x00')]).decode()
#
#####################################################################            
#

async def decryptPassword(ciphertext):
    #log.info(" Opening Connection to PWD Authenticator")
    #
    # this uses loop, but it is not sent in. Should we generate new loop here? or use old loop
    # think about this.. 
    #
    asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())
    loop2 = asyncio.get_event_loop()

    if PW_SSL:
        #log.info(" Using SSL for PWD AUTH")
        sslcontext = ssl.create_default_context(purpose=ssl.Purpose.SERVER_AUTH)
        sslcontext.check_hostname = True 
        sslcontext.load_verify_locations("../clientcerts/root.pem")
        sslcontext.load_cert_chain(certfile="../clientcerts/client.crt", keyfile="../clientcerts/client.key")
        reader, writer = await asyncio.open_connection(PW_SERVER_IP, PW_SERVER_PORT, loop = loop2,ssl = sslcontext)
    else:
        #log.info(" Not using SSL for PWD AUTH")
        reader, writer =  await asyncio.open_connection(PW_SERVER_IP, PW_SERVER_PORT, loop = loop2)
    #
    #log.info(" Packing Ciphertext")
    packed_cipher = struct.pack('>L32s',0,ciphertext) # zero is decrypt
    writer.write(packed_cipher)
    #log.info(" Transmited Ciphertext")
    #
    cleartext1  = await reader.read(PW_BUFFER_SIZE)
    #
    #log.info(" Received Cleartext")
    cleartext  = struct.unpack('>32s',cleartext1)[0].decode() 
    #
    #log.info(" Clearpass: %s",cleartext)  # Eventually will send back hash
    #
    return cleartext
#
#####################################################################################
#
class loginMessageBase:
    #
    char_pad = b'\x00'
    int_zero = 0
    int__one = 1 
    #
    def __init__(self, writer):
        self.writer         = writer
        self.packedResponse = None 

    def packMessage(self,encode_fmt,responseList): # packs the message give the List and the encode format 
      #
      print(responseList)
      self.packedResponse = packFixed(encode_fmt,responseList)


    def sendMessage(self): # prepends header and sends the message out
      #
      mylength = [(len(self.packedResponse)) + 4]
      packedLength = packFixed('>L', mylength)
      response = packedLength + self.packedResponse
      # 
      self.writer.write(response)

#
##################################################################################
#
class loginMessageGoodLogin(loginMessageBase):
  #
  def __init__(self, thisClient):
    #
    loginMessageBase.__init__(self, thisClient.writer)
    #
    responseList = [    MessageType.LOGIN_RESPONSE,
                         loginMessageBase.char_pad,
                      thisClient.account.accountid,
                         thisClient.account.result,
                     thisClient.account.acctstatus,
                        thisClient.account.subtime,
                        thisClient.account.partime,
                         loginMessageBase.char_pad,
                         loginMessageBase.char_pad,
                    thisClient.account.subfeatures,
                   thisClient.account.gamefeatures]
        
    encode_fmt = '>L72sLHHLL16s48sLL' 
    #
    self.packMessage(encode_fmt,responseList)  
    
#
#####################################################################
#
class loginMessageBadLogin(loginMessageBase):
  #
  def __init__(self, thisClient,type):
    #
    loginMessageBase.__init__(self, thisClient.writer)
    #
    if type == MessageType.LOGIN_REQUEST:        # ERROR with LOGIN_REQUEST
        message = b'Username or Password was incorrect.'
    #
    elif type == MessageType.SUBMIT_ACCT_CREATE: # ERROR with ACCT_CREATE
        message = b'An error occured or username is taken. Please try again and if unsuccessful, try another username. Thank you.'
    #
    elif type == MessageType.CHANGE_PASSWORD:    # ERROR with CHANGE PASSWORD
        message = b'An error occured. Please try again. If this persists try a new password.'
    #
    elif type == MessageType.DISABLED:           # REQUESTED DISABLED OPTION 
        message = b'This option is disabled in order to bring you content faster. If you feel this is an error,please contact the developer team for further assistance.'
    #
    else:                                        # NOT implemented, should send error code to client 
        message = b'You\'ve managed to do something wrong! Please let the developer team know you received this message.'
    #
    #  create encode_fmt 
    #
    lm = len(message)
    encode_fmt = '>LLLL'+'{}s'.format(lm)+'{}s'.format(256-lm)  
    #
    #  create the responseList 
    #
    responseList = [MessageType.ACCT_CREATE_RESPONSE,
                           loginMessageBase.int_zero,
                           loginMessageBase.int_zero,
                           loginMessageBase.int_zero,
                                             message,
                           loginMessageBase.char_pad] 
    #
    # Packed, ready to send after  ..
    #
    self.packMessage(encode_fmt,responseList)  
#
######################################################################
#
class loginMessageGoodChangePassword(loginMessageBase):
  #
  def __init__(self, thisClient):
    #
    loginMessageBase.__init__(self, thisClient.writer)
    #
    responseList = [MessageType.CHANGE_PASSWORD_RESPONSE,
                               loginMessageBase.int_zero,
                               loginMessageBase.int__one,
                               loginMessageBase.char_pad]
        
    encode_fmt = '>LLL256s'  
    #
    self.packMessage(encode_fmt,responseList)  

#
######################################################################
#
class loginMessageGoodCreate(loginMessageBase):
  #
  def __init__(self, thisClient):
    #
    loginMessageBase.__init__(self, thisClient.writer)
    #
    responseList = [MessageType.ACCT_CREATE_RESPONSE,
                           loginMessageBase.int_zero,
                           loginMessageBase.int__one,
                           loginMessageBase.int_zero,
                           loginMessageBase.char_pad]
        
    encode_fmt = '>LLLL256s' 
    #
    self.packMessage(encode_fmt,responseList)  

#
#####################################################################
#
#
async def processAccountLogin(baseDBsession, thisClient):

        log.info("   Client: %s - Unpacking Account data in processAccountLogin",thisClient.id) 
        d = struct.unpack('>Q32s32sL4s56s', thisClient.data[8:136+8])
        packetstuff  = d[0]  # probably print out to see if we ever see something different
        username     = binaryToASCII(d[1]) # only needed for strings
        encryptedpwd = d[2]
        unknown1     = d[3]
        gameid       = d[4]
        pad          = d[5]
        #
        #
        #Made this even simpler
        thisClient.account.username = username
        log.info("   Client: %s - Authenticating Login data in processAccountLogin",thisClient.id)
        myAccountDB_ops  = AccountDB_ops(baseDBsession,thisClient.account)  # attach DB session to Account
        if PW_SIMPLE_FLAG == True:
          thisClient.account.password = PW_SIMPLE
          if myAccountDB_ops.usernameExists():
            log.info("   Client: %s - Presented username exists...",thisClient.id)
            if myAccountDB_ops.authenticateUser():
              log.info("   Client: %s - Username and password authenticated. Proceeding",thisClient.id)
              myAccountDB_ops.pushLoginInfo(thisClient) # this should push time/IP info to DB for this account
              myAccountDB_ops.pullAccount()             # This should pull the Account info from the DB into thisClient.account
                                                      # should also log which account was logged into and what clientInfo
                                                      # should only need to read account here,
              loginMessageGoodLogin(thisClient).sendMessage()
            else:
              log.info("   Client: %s - Username and password not authenticated. Exiting",thisClient.id)
              loginMessageBadLogin(thisClient, MessageType.LOGIN_REQUEST).sendMessage()
          else:
            log.info("   Client: %s - Presented username does not exist..exiting..",thisClient.id)  # what do we really want to do here
            loginMessageBadLogin(thisClient, MessageType.LOGIN_REQUEST).sendMessage()
      
        else: 
          print('We are processing account information...')
          decryptedpwd = await decryptPassword(d[2])
          decryptedpwd = binaryToASCII(decryptedpwd.encode())

        #
        # print some of these out,
        # check that gameid is correct
        #
        #  assigns these to the account to go through authentication 
        #
          thisClient.account.password = decryptedpwd
        #

          if myAccountDB_ops.usernameExists():
            log.info("   Client: %s - Presented username exists...",thisClient.id)
            if myAccountDB_ops.authenticateUser():
              log.info("   Client: %s - Username and password authenticated. Proceeding",thisClient.id)    
              myAccountDB_ops.pushLoginInfo(thisClient) # this should push time/IP info to DB for this account
              myAccountDB_ops.pullAccount()             # This should pull the Account info from the DB into thisClient.account 
                                                      # should also log which account was logged into and what clientInfo 
                                                      # should only need to read account here,  
              loginMessageGoodLogin(thisClient).sendMessage()
            else:
              log.info("   Client: %s - Username and password not authenticated. Exiting",thisClient.id)
              loginMessageBadLogin(thisClient, MessageType.LOGIN_REQUEST).sendMessage()
          else:
            log.info("   Client: %s - Presented username does not exist..exiting..",thisClient.id)  # what do we really want to do here
            loginMessageBadLogin(thisClient, MessageType.LOGIN_REQUEST).sendMessage()
        
#
######################################################################
#
async def processAccountCreate(baseDBsession, thisClient):

        log.info("   Client: %s - Account Creation",thisClient.id)
        log.info("   Client: %s - Unpacking Account data in processAccountCreate",thisClient.id) 
        d = struct.unpack('>L32s32s32s32s16s32s32s32s32s16s16s16s16s128s',thisClient.data[8:468+8])
        header    = d[0]
        username  = binaryToASCII(d[1])
        encryptedpwd = d[2]
        ipaddress = thisClient.ip 
        first     = binaryToASCII(d[3])
        last      = binaryToASCII(d[6])
        middle    = binaryToASCII(d[5])
        unknown1  = binaryToASCII(d[4])
        unknown2  = binaryToASCII(d[7])
        country   = binaryToASCII(d[8])
        zip       = binaryToASCII(d[9])
        day       = binaryToASCII(d[11])
        month     = binaryToASCII(d[10])
        year      = binaryToASCII(d[12])
        sex       = binaryToASCII(d[13])
        email     = binaryToASCII(d[14])

        if PW_SIMPLE_FLAG == True: 
          decryptedpwd = PW_SIMPLE 
        else: 
          decryptedpwd = await decryptPassword(d[2])
          decryptedpwd = binaryToASCII(decryptedpwd.encode()) 
          
        #
        # create new account with the following attributes - later sub info will be set differently
        #
        thisClient.account.setHashPW(decryptedpwd)
        thisClient.account.setUserInfo(username,first,middle,last, unknown1, unknown2,country,zip,day,month,year,sex,email)
        thisClient.account.setAcctLvl(1)
        thisClient.account.setCreation(datetime.datetime.now())
        thisClient.account.setLastLogin(thisClient.ip,datetime.datetime.now())
        thisClient.account.setSubInfo(0, 1, 2592000,2592000,4,3)


        myAccountDB_ops  = AccountDB_ops(baseDBsession,thisClient.account)  # attach DB session to Account
        #        
        log.info("   Client: %s - Checking for Duplicate Account data in processAccountCreate",thisClient.id) 
        if myAccountDB_ops.usernameExists():
            log.info("   Client: %s - User attempted to register with registered name",thisClient.id)
            loginMessageBadLogin(thisClient, MessageType.SUBMIT_ACCT_CREATE).sendMessage()
            log.info("  Client: %s - Sending Bad Create Account Message (Name already Registerd)",thisClient.id) 
        else:
            log.info("   Client: %s - Creating Account/Inserting into DB in processAccountCreate",thisClient.id) 
            myAccountDB_ops.pushAccount()
            if myAccountDB_ops.usernameExists():
                myAccountDB_ops.pullAccount()  # this may just pull updates to last login, etc,  
                loginMessageGoodCreate(thisClient).sendMessage()  # does this continue on with login? 
                log.info("  Client: %s - Sending Good Account Create Message",thisClient.id) 
            else:
                log.warn("  Client: %s - Account not inserted into database",thisClient.id)
                log.info("  Client: %s - Sending Bad Login Message",thisClient.id) 
                loginMessageBadLogin(thisClient, MessageType.SUBMIT_ACCT_CREATE).sendMessage()  # does this continue on with login? 
                
#
#####################################################################
#

async def processChangePwd(baseDBsession, thisClient):
    log.info("   Client: %s - Unpacking Change PWD request in processChangePwd",thisClient.id) 
    d = struct.unpack('>LL32s32s', thisClient.data[8:72+8])
    oldpassword = await decryptPassword(d[2])
    newpassword = await decryptPassword(d[3])
    oldpassword = binaryToASCII(oldpassword.encode())
    newpassword = binaryToASCII(newpassword.encode())
    
    myAccountDB_ops  = AccountDB_ops(baseDBsession,thisClient.account)
    log.info("   Client: %s - Verifying Username",thisClient.id) 
    if myAccountDB_ops.verifyUser(oldpassword):
        log.info("   Client: %s - Username Verified",thisClient.id) 
        #create new sql object to write new password
        myAccountDB_ops.changePasswordDB(newpassword)
        loginMessageGoodChangePassword(thisClient).sendMessage()
        log.info("   Client: %s - Sent Password Changed Message",thisClient.id) 
    else:
        #call message failed
        log.info("   Client: %s - Username Not Verified",thisClient.id) 
        loginMessageBadLogin(thisClient, MessageType.CHANGE_PASSWORD).sendMessage()
#
#####################################################################            
#
async def processClientData(thisClient):
      s=struct.unpack('>LL',thisClient.data[:8])
      thisMessageLength = s[0]
      thisMessageType   = s[1]
      #Create SQL session
      log.info('Attempting session...')
      baseDBsession = createDBSession()
      log.info('Session created...')
      # 
      log.info("   Message Info:  Length %s   Type %s",thisMessageLength, thisMessageType)
      # 
      # Here is where we process the various message types  
      #
      if thisMessageType == MessageType.LOGIN_REQUEST:
          log.info("   Client: %s - Routing to processAccountLogin",thisClient.id)
          await processAccountLogin(baseDBsession, thisClient)
      #  
      elif thisMessageType == MessageType.SUBMIT_ACCT_CREATE:
          log.info("   Client: %s - Routing to processAccountCreate",thisClient.id) 
          await processAccountCreate(baseDBsession, thisClient)
      #
      elif thisMessageType == MessageType.CHANGE_PASSWORD:
          log.info("   Client: %s - Routing to ChangePassword",thisClient.id) 
          await processChangePwd(baseDBsession, thisClient)
      #
      elif (thisMessageType == MessageType.CONSUME_GAMECARD     or
            thisMessageType == MessageType.REQUEST_ACCT_KEY     or
            thisMessageType == MessageType.REQUEST_CANCEL_ACCT  or
            thisMessageType == MessageType.REQUEST_GAME_CARD    or
            thisMessageType == MessageType.REQUEST_SUBSCRIPTION or
            thisMessageType == MessageType.REQUEST_UPDATE_ACCT  or 
            thisMessageType == MessageType.FORGOT_PASSWORD):
             log.info('  Client: %s - "Disabled" features: %s',thisClient.id,thisMessageType)
             loginMessageBadLogin(thisClient, MessageType.DISABLED_FEATURE).sendMessage()
      else:
          log.warn("   Client: %s - Received unknown opcode: %s", thisClient.id,thisMessageType) 
          type = None
          #type = thisMessageType 
          loginMessageBadLogin(thisClient, type).sendMessage()    
      #
      baseDBsession.close()
      return

def createDBSession():
      baseDBsession = get_db_session() 
      return baseDBsession
#
######################################################
#   
class loginServer(asyncio.Protocol):

    def connection_made(self, transport):
        self.transport = transport
        self.address = transport.get_extra_info('peername')
        self.log = logging.getLogger(
            'loginServer_{}_{}'.format(*self.address)
        )
        self.log.info('Connection accepted...')
        #
        self.thisHostIP = self.address[0]
        self.thisHostPORT = self.address[1]
        # 
        self.thisClient = tcpClient(self.thisHostIP,self.thisHostPORT,self.transport,self.transport)
        # 

    def eof_received(self):
        self.log.info('Received FIN')
        if self.transport.can_write_eof():
            self.transport.write_eof()

    def connection_lost(self, error):
        if error:
            self.log.error('ERROR: {}'.format(error))
        else:
            self.log.info('Connection closing...')
            self.thisClient.TotSub()   # remove total number of currently connected Clients 
            self.log.info('Client Remaining on LoginServer: %s',tcpClient.totClients)
        super().connection_lost(error)


    def data_received(self, data):
        self.log.info(' Data Received {!r}'.format(data))
        #
        if len(data) > 8:  # has to be at least this big then continue, if not loop
          self.thisClient.data = data
          log.info("   Client: %s - Passing data to processClientData",self.thisClient.id)
          #
          #https://stackoverflow.com/questions/20746619/calling-coroutines-in-asyncio-protocol-data-received
          #
          asyncio.ensure_future(processClientData(self.thisClient))
        else:
          log.info("   Client: %s - Data Too Short",self.thisClient.id)
          # might not need this now. Doesn't hurt 
        
        #
#####################################################################            
#
#            MAIN LOOP
#
log.info("")
log.info("----------------------------------------------------------------------------------")
log.info("")
asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())
loop = asyncio.get_event_loop()
#
factory = loop.create_server(loginServer, SERVER_IP, SERVER_PORT)
log.info("Event Loop Starting...")
server  = loop.run_until_complete(factory)
log.info("Server Created....")
log.info('Listening on {} port {}'.format(SERVER_IP, SERVER_PORT))
try:
    loop.run_forever()

except KeyboardInterrupt:
    log.info(" ...")
    log.info("Caught a user interruption.")
    log.info(" ...")
finally:
    log.info('Server Closing...')
    server.close()
    loop.run_until_complete(server.wait_closed())
    log.info("Event Loop Closing...")
    loop.close()
    log.info("Server Closed")



