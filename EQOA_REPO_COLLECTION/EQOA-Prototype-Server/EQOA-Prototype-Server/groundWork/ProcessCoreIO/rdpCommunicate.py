#!/usr/bin/env python
#
# Devin and Ben 
# Febrary 13, 2019 
#
import asyncio, struct, logging, time
from opcodeOperations import *
import queue
#
#########################################################################
#
class rdpMessages:

  def __init__(self,myRDPComm):
    #
    self.FA_messageListIn = asyncio.Queue(0) 
    self.FB_messageListIn = asyncio.Queue(0)
    self.FC_messageListIn = asyncio.Queue(0) 
  #
    self.FA_messageListOut = asyncio.Queue(0) 
    self.FB_messageListOut = asyncio.Queue(0) 
    self.FC_messageListOut = asyncio.Queue(0)
    self.F9_messageListOut = asyncio.Queue(0)
    #
    self.myRDPComm = myRDPComm
  #
  #
  def injectMessage(self,messageNumber,messageType,message):
    self.myRDPComm.log.info('Processing message type {}.'.format(hex(messageType)))
    if messageType == 0xFA:
      self.FA_messageListIn.put_nowait(message)
    elif messageType == 0xFB:
      self.myRDPComm.log.info('Placing message into queue. Message is {}'.format(message))
      self.FB_messageListIn.put_nowait(message)
    elif messageType == 0xFC:
      self.FC_messageListIn.put_nowait(message)
    else:  #IDK
      pass # error
    self.myRDPComm.myRdpCommStatus.updateMessageLastRecv(messageNumber) 
  #
#
#########################################################################
#
class rdpCommStatus:

  def __init__(self,myRDPComm):
   #
   # should I have update function for these so messages are removed from virgin queue as they are ackd?
   #
    self.myRDPComm = myRDPComm 
    
    self.message_last_sent  = 0
    self.message_last_ackd  = 0 # in order

    self.bundle__last_sent  = 0
    self.bundle__last_ackd  = 0 # in order

    self.bundle__last_recv  = 0 
    self.message_last_recv  = 0 # in order
  
  def generateReport(self):
    return [self.bundle__last_sent+1,self.bundle__last_recv,self.message_last_recv]
 
  def processReport(self,thisBundle, ackBundle, ackMessage):
    self.updateMessageLastAckd(ackMessage)
    self.updateBundleLastAckd(ackBundle)
    self.updateBundleLastRecv(thisBundle)
    
  def logReport(self,log):
     #
     log.info('  RDP Comm Report :: Last bundle Recv {:} | Last bundle ACKd {:} | Last message ACKd {:}'.format(\
                self.bundle__last_recv,self.bundle__last_ackd,self.message_last_ackd))

  def updateMessageLastSent(self,value): # this is local, so shouldn't need checking
   if value == self.message_last_sent + 1:
     self.message_last_sent  = value
     self.myRDPComm.log.info('     Sent Message: {:}'.format(value))
   else: 
     self.myRDPComm.log.info('     Error with Sent Message: {}, expected {}'.format(value, self.message__last_sent))
  #
  def updateBundleLastSent(self,value): # this is local, so shouldn't need checking
   if value == self.bundle__last_sent + 1:
     self.bundle__last_sent = value
     self.myRDPComm.log.info('    Last Sent Bundle: {:}'.format(value))
   else:
     self.myRDPComm.log.info('    Error with Sent Bundle: {}, expected {}'.format(value, self.bundle__last_sent))
  #
  def updateMessageLastRecv(self,value): # 
   if value == self.message_last_recv + 1:
     self.message_last_recv = value 
     self.myRDPComm.log.info('    Last Received Message: {:}'.format(value))
   else:
     self.myRDPComm.log.info('    Error with Receive Message: {}, expected {}'.format(value, self.message_last_recv))
  #
  def updateBundleLastRecv(self,value): # 
   if value == self.bundle__last_recv + 1:
     self.bundle__last_recv = value
     if (self.bundle__last_recv >= 2) and (self.myRDPComm.mySession.sessionPhase == 6) and (self.myRDPComm.mySession.sessionIDbase == self.myRDPComm.mySession.remoteEndpoint):
       self.myRDPComm.collectServers = True 
     elif (self.bundle__last_recv >= 2) and (self.myRDPComm.mySession.sessionPhase == 6) and (self.myRDPComm.mySession.sessionIDbase == (self.myRDPComm.mySession.remoteEndpoint + 1)):
       self.myRDPComm.createSession = True
     elif (self.bundle__last_recv == 1) and (self.myRDPComm.mySession.sessionPhase == 7) and not self.myRDPComm.opcodeOperations.Character.dumpstarted:
       self.myRDPComm.characterSelect = True
     self.myRDPComm.log.info('    Last Received Bundle: {:}'.format(value))
   else:
     self.myRDPComm.log.info('    Error with Receive Bundle: {}, expected {}'.format(value, self.bundle__last_recv))
  #
  def updateMessageLastAckd(self,value): 
   self.myRDPComm.log.info('Message last sent is {} and last ack\'d is {}.'.format(self.message_last_sent, self.message_last_ackd)) 
   if value <= self.message_last_sent:
     self.message_last_ackd = value 
     self.myRDPComm.log.info('    Last Ackd Message: {:}'.format(value))
   # remove off of virgin queue 
   else:
     self.myRDPComm.log.info('    Error with Ack Message: {}, expected {}'.format(value, self.message_last_ackd))
  #
  def updateBundleLastAckd(self,value): # 
   if value == self.bundle__last_ackd + 1:
     self.bundle__last_ackd = value 
     self.myRDPComm.log.info('    Last Ackd Bundle: {:}'.format(value))
   else:
     self.myRDPComm.log.info('    Error with Ack Bundle: {}, expected {}'.format(value, self.bundle__last_ackd))
  #
  def messageReset(self):

    self.message_last_sent  = 0
    self.message_last_ackd  = 0 # in order

    self.bundle__last_sent  = 0
    self.bundle__last_ackd  = 0 # in order

    self.bundle__last_recv  = 0
    self.message_last_recv  = 0 # in order

#
#######################################################################################
#

class rdpCommunicator:

  def __init__(self,mySession):
    #
    log = logging.getLogger('RDP'+'_{:08X}'.format(mySession.sessionID))
    log.setLevel(logging.INFO)
    self.log = log
    self.mySession = mySession
    #
    self.has_session_ack = False 
    self.has_rdp_report  = False 
    self.has_msg_bundle  = False 
    #
    self.outgoing_session_ack = True  # on init set this to true since we need to ack this session 
    self.outgoing_rdp_report  = False 
    self.outgoing_msg_bundle  = False 
    #
    self.bundle = bytes()
    self.bundle_type = -1 
    self.processedBundle   = 1
    self.processedMessages = 0
    #
    self.outgoingBundle = bytes()
    self.outgoingBundleLength = 0
    #self.outgoingMessages = asyncio.Queue(0)
    self.outgoingMessages = queue.Queue(0)
    #
    self.outgoing_comm = asyncio.Queue(0)
    #A variable for testing server creation portion
    self.collectServers = False
    #Initiates our messageQueue
    self.messageQueue = messageQueue(self)
    #Initializes our Packet sender for CommManager
    self.loop = asyncio.get_event_loop()
    self.loop.create_task(self.packetSender())
    self.loop.create_task(self.retrieveBundle())
    self.loop.create_task(self.processMessages())
    self.loop.create_task(self.packageResponses())
    #self.loop.create_task(self.messageQueue.monitorAckList())
    #Session Timer, if session is connected longer then 30 seconds without response from client, 
    #server will disconnect the session
    #self.loop.create_task(self.sessionTracker())
    #
    #Tester section...
    self.close = False
    self.createSession = False
    self.timer = time.time()
    self.myCount = 0
    self.collectServers = False
    self.characterSelect = False
    self.firstConnect = False
    self.initialSession = False
    if self.mySession.sessionPhase == 7:
      self.log.info('Session Phase is 7, setting variable to create packet for client...')
      self.firstConnect   = True
      self.initialSession = True
    # need timers in here too. time since last message/contact etc.
    #
    ##################
    #
    self.myRdpCommStatus  = rdpCommStatus(self)
    self.myRdpMessages    = rdpMessages(self)
    self.opcodeOperations = opcodeOperations(self)
#
  def resetForNext(self):
    #
    self.has_session_ack = False 
    self.has_rdp_report  = False 
    self.has_msg_bundle  = False 
    #
    self.outgoing_session_ack = False 
    self.outgoing_rdp_report  = False 
    self.outgoing_msg_bundle  = False 
    #
    self.bundle = bytes()
    self.bundle_type = -1 
    self.processedBundle = 1
    self.processedMessages = 0
    #
    self.outgoingBundle = bytes()
#
  def setQueues(self,qInput,qOutput,qVirgin):
    self.qInput  = qInput
    self.qOutput = qOutput
    self.qVirgin = qVirgin
  
  def clearQueues(self):
    #
    # make sure these are empty before setting to None
    #
    self.qInput  = None 
    self.qOutput = None
    self.qVirgin = None
    #Set this to True to close this instance
    self.close = True

  async def retrieveBundle(self):
    #
    self.log.info('Starting retrieveBundle Loop...')
    while True:
      if self.close:
        break
      
      #Packet received from client, update the timer
      #Is this a good place? Works for now...
      if self.qInput.empty() == True:
        await asyncio.sleep(0)
      else:
        self.timer = time.time()
        self.bundle = bytes()
        self.bundle_type = -1 
        #
        self.log.info('Session Phase is: {}'.format(self.mySession.sessionPhase))
        self.log.info(' Executing Cycle in Session: 0x{:08X}'.format(self.mySession.sessionID))
        bundle = await self.qInput.get()
        bundle_type = bundle[0]
        self.log.info('  Got Bundle in Session of length: {:}'.format(len(bundle)))
        #self.log.info('  Bundle Type                    : 0x{:02X}'.format(bundle_type))
        #self.log.info('  Bundle Payload                 : '+' '.join('%02X' % i for i in bundle))
        self.bundle = bundle[1:]   # need to make sure I removed bundle_type from payload
        self.bundle_type = bundle_type
        self.processedBundle = 0
        #self.outgoing_rdp_report = True
        self.processBundle()

  def processBundle(self):  # may async
    #
    if self.processedBundle != 1:
    #
    # get flags associated with bundle type using bit shift
    #
      self.has_session_ack = False 
      self.has_rdp_report  = False 
      self.has_msg_bundle  = False 
    #
      if self.bundle_type == 0x63:
        self.has_session_ack = True 
        self.has_rdp_report  = True 
        self.has_msg_bundle  = True
      elif self.bundle_type == 0x23:
        self.has_rdp_report  = True 
        self.outgoing_rdp_report  = False
      elif self.bundle_type == 0x20:
        self.has_msg_bundle  = True
      elif self.bundle_type == 0x03:
        self.has_rdp_report  = True 
      elif self.bundle_type == 0x00:
        self.has_msg_bundle  = True
      else:
        self.log.info('   Unhandled bundle type: {:02X}'.format(self.bundle_type))  # put in log eventually
    #
      self.log.info('   Bundle Type: {:02x} |'.format(self.bundle_type)+'SES_ACK: {:} |'.format(self.has_session_ack)+'MSG_BUNDLE: {:} |'.format(self.has_msg_bundle)+'RDP_REPORT : {:} '.format(self.has_rdp_report)) 
    #
      if self.has_session_ack:
        self.processSessionACK()
    #
      if self.has_rdp_report:
        self.processMessageReport()
    #
      if self.has_msg_bundle: 
        self.processMessageBundle()
    #
      self.processedBundle = 1  # last thing in here
    

  def encodeBundle(self):  # may async
    #
    thisBundleType = 0x99  # init to bad number
    if self.outgoing_session_ack and self.outgoing_rdp_report and self.outgoing_msg_bundle:
      thisBundleType = 0x63
    if not self.outgoing_session_ack and self.outgoing_rdp_report and not self.outgoing_msg_bundle:
      thisBundleType = 0x23
      #thisBundleType = 0x03  # not sure what the difference is at this point. maybe over message 17?
    if not self.outgoing_session_ack and not self.outgoing_rdp_report and self.outgoing_msg_bundle:
      thisBundleType = 0x20
      #thisBundleType = 0x00   # not sure what the difference is at this point. maybe over message 17?
    #
    #self.outgoingBundle += struct.pack('<B',thisBundleType)
    self.outgoingBundle = struct.pack('<B',thisBundleType) + self.outgoingBundle
    self.log.info('    Including Bundle Type ')

    #
    #################################################################
    #
  def processSessionACK(self):
   #
   # we know how to parse, but where should this information go. Basically the session master will wait for a reponse back
   # from the session slave before sending additional information out on session. Probably should not accept payloads until session has been ACKED
   # if we are master and we have requested a new session, we must wait till we hear back from client (this is when this will be used). probably save in
   # session object and make sure we have previous communicated over this session (we have messages from slave) or a particular flag is set
   #
   # sessionID length can vary. 4 bytes when server is slave, 7 when server is master (might save as session attribute)
   # however, the Session ACK appears to only check the first 4 bytes regardless
   #
   returnedSessionID = struct.unpack('<I',self.bundle[:4])[0]
   self.bundle = self.bundle[4:]
   if returnedSessionID == self.mySession.sessionID:
     self.mySession.sessionACKd = True
     self.log.info('  Session 0x{:08X} has been ACKd from Client'.format(self.mySession.sessionID))

  def encodeSessionACK(self):  # will be opposite of above - > processSessionACK
    #
    # maybe append to packet being generated
    self.outgoingBundle += struct.pack('<HH',self.mySession.sessionIDbase, self.mySession.sessionIDup) 
    self.log.info('    Including Session 0x{:08X} Acknowledgement'.format(self.mySession.sessionID))

  def processMessageReport(self):
   [this_bundle,ackd_bundle,ackd_message]  = struct.unpack('<HHH',self.bundle[:6])
   self.messageQueue.removeFromList(ackd_message)
   self.bundle = self.bundle[6:]
   #
   self.myRdpCommStatus.processReport(this_bundle,ackd_bundle,ackd_message)
   self.myRdpCommStatus.logReport(self.log)
   #
  def encodeMessageReport(self):  # opposite of above
    [this_bundle, last_bundle_recv, last_message_recv] = self.myRdpCommStatus.generateReport()
    self.outgoingBundle += struct.pack('<HHH',this_bundle, last_bundle_recv, last_message_recv) 
    self.log.info('    Including RDP Message Report - Bundle: {:}'.format(this_bundle+1))  # need to send NEXT bundle

  def processMessageBundle(self):
    # if gotBundle and gotMessage don't give success, drop appropriate message
    # now extract incoming bundle number. then need to read generic message length and type  
    #  0xFB is message less than say 255 bytes and not continued
    #  0xFFFC is long system message but is [FC FF] in stream. 
    #
    self.log.info('  Msg Bundle Payload: '+' '.join('%02X' % i for i in self.bundle))
    #
    # first extract incoming bundle number (always 2 bytes). 
    # Then determine if message length is 2 bytes or 1 byte
    # Probably some smart way to do this based on bundle type etc, but doing brute force for now
    #
    #
    try:
      rec_bun_num = struct.unpack('<H',self.bundle[:2])[0]
      self.bundle = self.bundle[2:]
      self.myRdpCommStatus.updateBundleLastRecv(rec_bun_num) 
      #
      # Now extract reliable messages. read 2 bytes and go from there.
      #
      # this should loop over reliable messages. still have total message length in Session
      #
      while (len(self.bundle) > 0):
        #
        testMessageLength = struct.unpack('<H',self.bundle[:2])[0]
        if testMessageLength > 0xFF00:  # long messages - there is definitely a better way to do this but stay clugey for now
          [messageType, messageLength, messageNumber]  = struct.unpack('<HHH',self.bundle[:6])
          self.bundle = self.bundle[6:]
          messageType = messageType&0x00FF  # makes it look like short messageType 
        else:
          [messageType, messageLength, messageNumber]  = struct.unpack('<BBH',self.bundle[:4])
          self.bundle = self.bundle[4:]
          #
          # deliver to message processing center
          #
        self.myRdpMessages.injectMessage(messageNumber, messageType, self.bundle[:messageLength])
        self.bundle = self.bundle[messageLength:]
   
        #for msg in self.myRdpMessages.FB_messageListIn:
        # self.log.info('     Message: '+' '.join('%02X' % i for i in msg))
        if self.myRdpMessages.FB_messageListIn.empty() == False:
          self.log.info('Message is in queue') 
    
    except:
      self.log.info('Error occured during processMessageBundle... Payload is {}, if empty this is 1st packet back from client'.format(self.bundle))
    finally:
      #Just skip for now..
      pass

  def encodeMessageBundle(self):
      # pull together message and pull into bundle
      # this will be loke processMEssages, but pull from Out list
      # need to have thing to take opcode messages and generate message bundle 
      # this will be until empty or a certain size/length of message
      #
      self.log.info('    Constructing Message Bundle')
      thisBundleNumber  = self.myRdpCommStatus.bundle__last_sent+1  
      #
      while (self.myRdpMessages.FA_messageListOut.qsize() > 0 or self.myRdpMessages.F9_messageListOut.qsize() > 0 or self.myRdpMessages.FC_messageListOut.qsize() > 0 or self.myRdpMessages.FB_messageListOut.qsize() > 0) and len(self.outgoingBundle) < 1400:  # random number for now. add parameter
        if self.myRdpMessages.FB_messageListOut.empty() == True:
          if self.myRdpMessages.FA_messageListOut.empty() == True:
            if self.myRdpMessages.FC_messageListOut.empty() == True:
              if self.myRdpMessages.F9_messageListOut.empty() == True:
                pass
              else:
                thisMessage = self.myRdpMessages.F9_messageListOut.get_nowait()
                self.log.info('     Message: '+' '.join('%02X' % i for i in thisMessage))
                #
                if len(thisMessage) > 0xFF:
                  tpart = struct.pack('<HH', 0xFFF9, len(thisMessage))
                else:
                  tpart = struct.pack('<BB', 0xF9, len(thisMessage))
              thisMessageNumber = self.myRdpCommStatus.message_last_sent + 1
              self.myRdpCommStatus.updateMessageLastSent(thisMessageNumber) 
              tpart += struct.pack('<H', thisMessageNumber) 
              tpart += thisMessage

              self.outgoingBundle += tpart
            else:
              thisMessage = self.myRdpMessages.FC_messageListOut.get_nowait()
              self.log.info('     Message: '+' '.join('%02X' % i for i in thisMessage)) 
              #
              if len(thisMessage) > 0xFF:
                tpart = struct.pack('<HH',0xFFFC,len(thisMessage))
              else:
                tpart = struct.pack('<BB',0xFC,len(thisMessage))
                
              tpart += thisMessage

              self.outgoingBundle += tpart
          else:
            thisMessage = self.myRdpMessages.FA_messageListOut.get_nowait()
            self.log.info('     Message: '+' '.join('%02X' % i for i in thisMessage))
            if len(thisMessage) > 0xFF:
              tpart = struct.pack('<HH',0xFFFA,len(thisMessage))
            else:
              tpart = struct.pack('<BB',0xFA,len(thisMessage))
            #
            self.log.info('Creating Message {}.'.format(self.myRdpCommStatus.message_last_sent + 1))
            thisMessageNumber = self.myRdpCommStatus.message_last_sent+1
            self.myRdpCommStatus.updateMessageLastSent(thisMessageNumber)
            tpart += struct.pack('<H',thisMessageNumber)
            #
            tpart += thisMessage
            #Add messages to our ack queue here?
            self.messageQueue.addToList(thisMessageNumber, thisMessage)
            self.outgoingBundle += tpart

        else:
          thisMessage = self.myRdpMessages.FB_messageListOut.get_nowait()
          self.log.info('     Message: '+' '.join('%02X' % i for i in thisMessage))
          #
          if len(thisMessage) > 0xFF:
            tpart = struct.pack('<HH',0xFFFB,len(thisMessage))
          else:
            tpart = struct.pack('<BB',0xFB,len(thisMessage))
          #
          self.log.info('Creating Message {}.'.format(self.myRdpCommStatus.message_last_sent + 1))
          thisMessageNumber = self.myRdpCommStatus.message_last_sent+1
          self.myRdpCommStatus.updateMessageLastSent(thisMessageNumber)
          tpart += struct.pack('<H',thisMessageNumber)
          #
          tpart += thisMessage

          #Add messages to our ack queue here?
          self.messageQueue.addToList(thisMessageNumber, thisMessage)
          self.outgoingBundle += tpart
      if not self.outgoing_rdp_report:
        self.log.info('No RDP report, adding Bundle #')
        self.outgoingBundle = struct.pack('<H',thisBundleNumber) + self.outgoingBundle   

        self.log.info('    Including Message Bundle')

  async def processMessages(self):
    #
    #  Pull from Message Queues 
    #  might just do FB messages first
    #
    #This cycles through possiblities for Opcode messages.
    self.log.info('Starting processMessage loop...')
    while True:

      if self.close:
        break

      if self.myRdpMessages.FB_messageListIn.empty() == True:
        if self.myRdpMessages.FC_messageListIn.empty() == True:
          if self.myRdpMessages.FA_messageListIn.empty() == True:
            #Creates our server listing once we have received our first Ack from client and when our sessionPhase is 6 (Server select)
            if ((self.myCount < 2) and self.collectServers):
              self.myCount += 1
              await self.opcodeOperations.opcodeServerListing()
              self.processedMessages = 1
              
            #Creates internal master session
            elif self.createSession:
              self.log.info('Beginning creation of internal master session...')
              self.createSession = False
              #Maybe a good idea to make sure account exists moving forward into the master Session
              if self.mySession.accountID != None and self.mySession.accountID >= 1:
                self.log.info('>>> Sending AccountID {} for internal master session.'.format(self.mySession.accountID))
                self.mySession.sessionInfo.serverShared.masterSessionIn.put_nowait([self.mySession.remoteEndpoint, 0, self.mySession.accountID])
              
              else:
                self.log.info('Error accoured with account when attempting to create internal master session...')
 
            elif self.firstConnect:
              self.firstConnect = False
              #Create packet for client with server as master
              await self.opcodeOperations.initiateSession()

            elif self.characterSelect:
              self.characterSelect = False
              #Generate character list
              await self.opcodeOperations.createCharacterList.createCharacters() 

            else:
              await asyncio.sleep(0) 
              
        
          else:
            thisMessage = await self.myRdpMessages.FA_messageListIn.get()
            await self.opcodeOperations.processMessage(thisMessage) 
            if self.myRdpMessages.FA_messageListIn.empty():
              self.processedMessages = 1
        else:
          thisMessage = await self.myRdpMessages.FC_messageListIn.get()
          await self.opcodeOperations.processMessage(thisMessage) 
          if self.myRdpMessages.FC_messageListIn.empty():
            self.processedMessages = 1 
      else:
        self.log.info('Gathering message')
        thisMessage = await self.myRdpMessages.FB_messageListIn.get()
        self.log.info('Sending message to be processed...')
        await self.opcodeOperations.processMessage(thisMessage)
        self.log.info('Passed this process....') 
        if self.myRdpMessages.FB_messageListIn.empty():
          self.processedMessages = 1 
    #
  #
  async def packageResponses(self):
    #
    #
    self.log.info('Starting packageResponse loop...')
    while True:
      if self.close:
        break

      if self.processedMessages == 1:
        if self.outgoing_session_ack or self.outgoing_rdp_report or self.outgoing_msg_bundle:
          self.log.info('  ') 
          self.log.info('  Generating Response ') 
          self.log.info('    SessionACK: {:}    RdpReport: {:}    MSG_BUNDLE: {:}'.format(self.outgoing_session_ack,self.outgoing_rdp_report,self.outgoing_msg_bundle)) 
          if self.outgoing_session_ack: # Generate Session ACK
            self.encodeSessionACK()     # and append to outgoing message 
  
          if self.outgoing_rdp_report: # Generate RDP message report
            self.encodeMessageReport() # append to outgoingmessage 
       
          if self.outgoing_msg_bundle: # Generate MSG Bundle
            self.encodeMessageBundle()
          #
          self.encodeBundle()  # need to encode bundle (put on bundle type)
          #
          self.addSessionHeader()
          #
          thisBundle = self.myRdpCommStatus.bundle__last_sent + 1 
          self.myRdpCommStatus.updateBundleLastSent(thisBundle) # maybe have this here. 
          #
          self.log.info('    OutMessage Bundle {:}: '.format(thisBundle)+' '.join('%02X' % i for i in self.outgoingBundle)) 
          self.resetForNext()   # reset flags for next set of messages  
          # 
          # need to send back out now. what is best way. need to send to virgin queue on rdp for sure
          # need to have async over outgoing queue and virgin queue  
          #
        else:
          await asyncio.sleep(0)
          
      else:
        await asyncio.sleep(0) 

  def addSessionHeader(self):
    #
    # 1) get length of bundle message
    # 2) attach sessionID
    # 3) attach bundle class and bundle length
    # 
    self.outgoingBundleLength = len(self.outgoingBundle)
    #Client is master, sessionID is only 4 bytes
    if self.mySession.sessionPhase == 6:
      self.outgoingBundle = struct.pack('<HH',self.mySession.sessionIDbase, self.mySession.sessionIDup)+ self.outgoingBundle
    #
    #Server is master, 7 byte sessionID
    elif self.mySession.sessionPhase == 7:
      thisObj = struct.pack('<LL', self.mySession.sessionIDbase, self.mySession.sessionIDup)
      self.outgoingBundle = thisObj[0:7] + self.outgoingBundle
    # need to add bundle class/session phase to below. 
    #
    if self.mySession.remoteMaster == 1:
      if self.mySession.remoteMaster == 1:  # don't add extra byte
        encodedBundleLength = bunLenEncode(self.outgoingBundleLength)
        outPhase = int(self.mySession.sessionPhase<<12 ) 
        self.outgoingBundle = struct.pack('<H',int(encodedBundleLength+outPhase)) + self.outgoingBundle 
      else:
        self.outgoingBundle = struct.pack('<B',0x01) + self.outgoingBundle
        encodedBundleLength = bunLenEncode(self.outgoingBundleLength)
        outPhase = int(self.mySession.sessionPhase<<12 + 0x01<<15) 
        #
        self.outgoingBundle = struct.pack('<H',int(encodedBundleLength+outPhase)) + self.outgoingBundle 
    
    #Server is master
    else:
      #Creating a session with client
      if self.initialSession:
        self.initialSession = False
        self.outgoingBundle = struct.pack('<B', 0x21) + self.outgoingBundle
        encodedBundleLength = bunLenEncode(self.outgoingBundleLength)
        outPhase = int((self.mySession.sessionPhase+0x05)<<12)
        self.outgoingBundle = struct.pack('<H',int(encodedBundleLength+outPhase)) + self.outgoingBundle 

      #Continuing session
      else:
        #print('The Session Phase is ', self.mySession.sessionPhase)
        self.outgoingBundle = struct.pack('<B',0x01) + self.outgoingBundle
        encodedBundleLength = bunLenEncode(self.outgoingBundleLength) 
        outPhase = int((self.mySession.sessionPhase+0x05)<<12) 
        #
        #print(hex(encodedBundleLength), ' and ', hex(outPhase))
        self.outgoingBundle = struct.pack('<H',int(encodedBundleLength+outPhase)) + self.outgoingBundle 
        #self.log.info('Remote endpoint is {}, Remote Master is {}, Session IDBase is {}, SessIDup is {}'.format(hex(self.mySession.remoteEndpoint), hex(self.mySession.remoteMaster), hex(self.mySession.sessionIDbase), hex(self.mySession.sessionIDup)))

    #
    #print('Information to be sent out; ', self.outgoingBundle)
    self.log.info('Remote endpoint is {} and sessionIDbase is {}...'.format(self.mySession.remoteEndpoint, self.mySession.sessionIDbase))
    self.outgoingBundle = struct.pack('<H', self.mySession.remoteEndpoint) + self.outgoingBundle
    #Insert a asyncio queue here to attach to packetSender?
    self.outgoing_comm.put_nowait(self.outgoingBundle)
    self.log.info('    Attaching Session Header')
    #

  async def packetSender(self):
    #Initialize this with Class
    self.log.info('Awaiting outgoing packets')
    while True:
      if self.close:
        break

      if self.outgoing_comm.empty() == True:
        await asyncio.sleep(0)
        
      #Think this works well
      else:
        self.log.info('Sending formed packet to commManager')
        packet = await self.outgoing_comm.get()
        self.mySession.sessionInfo.serverShared.outboundQueue.put_nowait(packet)
  
  async def sessionTracker(self):
    #Start monitoring client connections, prepping for a time out
    self.log.info(' Session connection monitoring beginning...')
    while True:
      
      #When the session closes, this helps to clear tasks out by allowing it to close.
      if self.close:
        break
      #If client doesn't send a packet for 30 seconds, server will terminate session.
      #This may not work well due to server select, though?
      if (time.time() - self.timer) > 25:
        self.timer = time.time() 
        #Timer variable is set upon instance creation, timer resets in def retrieveBundle
        #Is this an appropriate place for that? Would be a good indicator that the client is sending *something*
        self.log.info('     Haven\'t heard from client, Endpoint {} in 30 seconds. Closing Session.'.format(self.mySession.remoteEndpoint))
        self.mySession.sessionInfo.removeSession(self.mySession.sessionID)  
        #Await this function to break out to other tasks
      else:
        await asyncio.sleep(0)

#
################################################################################################################
#
class messageQueue():
    def __init__(self, myRdpComm):

        self.myRdpComm   = myRdpComm
        self.messageList = []
        self.time        = None

    def addToList(self, messageNumber, Message):
        #self.myRdpComm.log.info(' Adding Message (edit out to save space)to Ack request List, Number: {}.'.format(messageNumber))
        self.messageList.append([messageNumber, Message])
    #This takes the client rdp report and uses last ack'd message to remove the sent, or up to message
    def removeFromList(self, messageNumber):
        try:
          while True:
            self.myRdpComm.log.info('Processing ack list.')
            if self.messageList[0][0] <= messageNumber:
              self.myRdpComm.log.info('Removing message {} from Ack List.'.format(self.messageList[0]))
              self.messageList.remove(self.messageList[0])
            else:
              self.myRdpComm.log.info('Done removing Messages from Ack List')
              break
        except:
          self.myRdpComm.log.info('Ack List is empty')
        finally:
          self.myRdpComm.log.info('Done processing Ack List')
 
    async def monitorAckList(self):
      self.myRdpComm.log.info('Monitoring Ack List...')
      self.time = time.time()
      while True:
        if self.myRdpComm.close == True:
          break
        if ((time.time() - self.time) > 1):
          self.time = time.time()
          if self.messageList:
            for message in self.messageList:
              #Put items in list into the FB Queue
              self.myRdpComm.log.info('Resending message {}.'.format(message[0]))
              self.myRdpComm.myRdpMessages.FB_messageListOut.put_nowait(message[1])
              self.myRdpComm.processedMessages = 1

          else:
            await asyncio.sleep(0)

        else:
          await asyncio.sleep(0)       
        
def bunLenEncode(value):  # Should convert 'true' Bundle Length to 'transport' value using technique
  #  
  top = (value<<1)&0x0F00
  bot = (0x007F&value)+0x80
  #
  answer = int(top+bot)
  #
  return answer 

#
