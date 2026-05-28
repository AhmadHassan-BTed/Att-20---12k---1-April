#!/usr/bin/env python
#
# Devin and Ben 
# January 18, 2019 
#
import asyncio, struct, logging
from rdpCommunicate import rdpCommunicator
#
class sessionAction:
  #
  NEW        =  0x21 
  CLOSE      =  0x14
  CONTINUE   =  0x01
  DELIVERTO  =  0x01

  lookup = {NEW:'NEW',CLOSE:'CLOSE',CONTINUE:'CONTINUE'}

#
#################################################################################################################
#
class session:

  def __init__(self, remoteEndpoint, remoteMaster, sessionIDbase, sessionIDup, sessionPhase, accountID): 
    #
    self.remoteEndpoint = remoteEndpoint
    self.remoteMaster   = remoteMaster
    self.sessionIDbase  = sessionIDbase
    self.sessionIDup    = sessionIDup
    self.sessionPhase   = sessionPhase
    self.timeStarted    = 0   # add module for time of day, etc
    self.sessionACKd    = False
    self.accountID      = accountID
    #
    #If Client is master
    if self.sessionPhase == 6:
      self.sessionID      = int(self.sessionIDbase<<16)+self.sessionIDup 
    #If Server is master
    elif self.sessionPhase == 7:
      self.sessionID = int(self.sessionIDbase<<24)+self.sessionIDup
    #
    log = logging.getLogger('Session')
    log.setLevel(logging.INFO)
    #
    log.info('  Initializing Session Object: 0x{:08X}'.format(self.sessionID))
    #
#
  def printSession(self):
    #
    print('SessionID     : {:08X}'.format(self.sessionID)) 
    print('RemoteEndpoint: {:04X}'.format(self.remoteEndpoint)) 

  async def execute(self):
    #
    await self.myRDPComm.retrieveBundle()
    self.myRDPComm.processBundle()
    await self.myRDPComm.processMessages()
    self.myRDPComm.packageResponses()
#   
#################################################################################################################
#
class sessionManager:

  def __init__(self,loop,serverShared,myProcessQueues): #Just loads these up into the loop - doesnt start them yet.
    #
    log = logging.getLogger('sessManager')
    log.setLevel(logging.INFO)
    #
    self.loop              = loop
    self.log               = log 
    #
    self.serverShared = serverShared
    #
    self.myProcessQueues = myProcessQueues 
    #
    log.info("      Initializing sessManager...")
    #
    #Helps Session Manager to create master sessions
    #Hold on tight, here we go
    self.serverSession = 0x10000000
    
    #
  def start(self): # when this gets called, everything in place, just looping over task
    #
    self.log.info("      Starting sessManager...")
    #
    self.loop.create_task(self.processSessQueueIn(self.myProcessQueues.sess_q_i,self.serverShared))
    self.loop.create_task(self.createMasterSession(self.serverShared))
#
#
#
#
  async def injectSession(self,theSessionObject):
    # 
    self.log.info('      Injecting Session (0x{:08X}) into ProcessSessions...'.format(theSessionObject.sessionID))
    self.serverShared.mailQueueIn.put([sessionAction.NEW,theSessionObject])  # should be putting into processSession transportInQ 
    #print(' items in mailqueue: {:}'.format(self.serverShared.mailQueueIn.qsize()))
#
  async def removeSession(self,theSessionID,theRemoteEndpoint):
    # should actually remove from the processing queue. Not destry
    #
    self.log.info('      Removing Session (0x{:08X}) from ProcessSessions...'.format(theSessionID))
    self.myProcessQueues.comm_q_e.put_nowait(theRemoteEndpoint) # send endpointID to be removed from commManager   
    #print(' items in remove endpoint queue : {:}'.format(self.myProcessQueues.comm_q_e.qsize()))
    self.serverShared.mailQueueIn.put([sessionAction.CLOSE,theSessionID])  # should be putting into processSession transportInQ 
#
  async def deliverPayload(self,theSessionID,thePayload):
    # 
    # kinda stupid to add remote Master and sessionPhase here,but fix it on next update
    #
    self.log.info('      Delivering Payload for Session (0x{:08X}) to ProcessSessions...'.format(theSessionID))
    self.serverShared.mailQueueIn.put([sessionAction.DELIVERTO,theSessionID,thePayload,self.remoteMaster])  # should be putting into processSession transportInQ 
#
############################################################################
#
  def incrementSession(self):
    self.serverSession += 1024
#
###########################################################################################################
#

  async def createMasterSession(self, serverShared):
    self.log.info('Awaiting requests for master sessions...')
    try:  
      while True:
        if serverShared.masterSessionIn.empty() == True:
          await  asyncio.sleep(0)
       
        else:
        
          bundle   = serverShared.masterSessionIn.get_nowait()
          remoteEndpoint = bundle[0]
          remoteMaster   = bundle[1]
          accountID      = bundle[2]
  
          self.log.info('Creating internal master session with client {}.'.format(hex(remoteEndpoint)))
          #Attempting to create a server session, kinda static for now.
          #Maybe consider randomizing it with data we have  from Pcaps?
          #thisSession = random.randint(0x10000000, 0xFFFFFFFE) might be a decent start
          sessionIDbase = self.serverSession + 1024
          #
          #Increments our internal session variable for now
          self.incrementSession()

          sessionPhase = 7
          #
          #Data appears to support that this value actually utilizes the technique
          #static for now. Need to research if Technique is needed.
          sessionIDup = 0x0DBCD8
          #Create the session
          thisSession = session(remoteEndpoint, remoteMaster, sessionIDbase, sessionIDup, sessionPhase, accountID)
          await self.injectSession(thisSession)
    except KeyboardInterrupt:
        self.log.info('CreateMasterSession caught KeyboardInterrupt...')

    finally:
      pass


  async def processSessQueueIn(self,queue,serverShared):
    #
    # This is essentially sessManager
    #
    log          = logging.getLogger('sessMan:InQ')
    #
    try:
      while True:
        item    = await queue.get()
        self.clientID = item[0]
        self.payload = item[1]
        #
        log.info(' Consuming Message Length {:} from ClientID: 0x{:04X} '.format(len(self.payload),self.clientID))
        #
        # First need to read and process length and packet class and session action 
        # First need to determine if master or slave - packet action only if slave on receiving side.
       
        s = struct.unpack('<H',self.payload[:2]) # read two bytes bundle length/bundle classi
        self.payload = self.payload[2:] # remove the bytes we just read
        #Not sure this is correct? Appears most client sent values, when shifted 15 bits, are 1
        self.remoteMaster = s[0]>>15           # I believe shift 15 bits. If this is 1, client is master, if it is 0, server is master
        #SessionPhase seems the best indicator of who is master.
        self.sessionPhase = ((s[0]&0xF000)>>12)&0x7  # Think this is tied to which phase we are in pre-connect/world select/character select. not sure 
        self.bundleLength = int(int((s[0]&0x0F00)>>1) + int((s[0]&0x00FF)&0x7F)) 
        #
        # would like to get remoteMaster and sessionPhase to session object
        #
        #
        log.info(' Remote is Master: {:}  Session Phase: {:}  Bundle Length: {:}'.format(self.remoteMaster,self.sessionPhase,self.bundleLength))
        #
        # if bundle length is not entire payload, we need to load another bundle in this message payload
        # will check bundle length after we strip off remainder of session header  
        #
        #
        #
        if self.sessionPhase == 6:
          #
          #log.info('     Remote (Client) is MASTER')
          # Implies client is master and we need to read session action
          # Also need to read sessionID. Appears to be broken up into two
          # once we are master, we use 7 byte values? not really help
          #
          s = struct.unpack('<BHH',self.payload[:5]) # read session action and sessionID (slave type)
          #
          self.sessionAction = s[0]
          self.sessionID_A   = s[1]
          self.sessionID_B   = s[2]
          self.sessionID     = int(self.sessionID_A<<16)+self.sessionID_B 
          #
          log.info(' Session Action: {:}  SessionID_A: {:}/0x{:04X}  Client Uptime: {:} minutes /0x{:04X}'.format(sessionAction.lookup.get(self.sessionAction),self.sessionID_A,self.sessionID_A, self.sessionID_B,self.sessionID_B))
          self.payload = self.payload[5:] # remove the bytes we just read. CRC has already been stripped
          #
          # Now extract the bundle to be delivered to the session
          # Some packets have two bundles I believe so here will will read the 
          # first bundle and see if any bytes remain. If so, I think we have to read the 
          # bundle class and length again and deliver that message as well. this is
          # a rare case and I think the server may have done it and not the client.
          #
          #
          if self.sessionAction == sessionAction.NEW or self.sessionAction == sessionAction.CONTINUE: 
            if len(self.payload) != self.bundleLength:
              log.warn('     EXPECTED LENGTH: {:}'.format(self.bundleLength))
              log.warn('     ACTUAL   LENGTH: {:}'.format(len(self.payload)))
              log.warn('     More than 1 bundle sent in message')
              log.warn('     need to code 2 bundles in messages')
              # just drop second message for now
         
          #
          if self.sessionAction == sessionAction.NEW:
            #
            log.info('     Session Action Received: NEW SESSION')
            thisSession = session(self.clientID,self.remoteMaster,self.sessionID_A,self.sessionID_B, self.sessionPhase, None)
            await self.injectSession(thisSession)
            await self.deliverPayload(self.sessionID,self.payload)
            #
          elif self.sessionAction == sessionAction.CLOSE:
            #
            log.info('     Session Action Received: CLOSE SESSION')
            await self.removeSession(self.sessionID,self.clientID) # this session will be bulled from processSessions. just need sessionID for now
            #
          elif self.sessionAction == sessionAction.CONTINUE:
            #
            log.info('     Session Action Received: CONTINUE SESSION')
            #
            await self.deliverPayload(self.sessionID,self.payload)# this session will be pulled from processSessions, need sessionID and payload
            #
          else:
            #
            log.info('     Session Action Received: UNKNOWN [{:}]'.format(self.sessionAction))
            #
        #
        elif self.sessionPhase == 7: # local (server) is master
          #
          log.info('     Local (Server) is MASTER')
          #
          #Check for Close byte
          s = struct.unpack('<B', self.payload[:1])
          if s[0] == sessionAction.CLOSE:
            self.payload = self.payload[5:]
            s = struct.unpack('<LL',self.payload[:7] + b'\x00') # read sessionID - work on this. haven't done it yet
            self.sessionID_A   = s[0]
            self.sessionID_B   = s[1]
            self.sessionID     = int(self.sessionID_A<<24)+self.sessionID_B
            #Pass in to close session
            self.payload = self.payload[7:]
            log.info('     Session Action Received: CLOSE SESSION')
            await self.removeSession(self.sessionID, self.clientID)
          #
          #Client isn't requesting to close session 
          else:                                                                                                                                                                  
            #         
            s = struct.unpack('<LL',self.payload[:7] + b'\x00') # read sessionID - work on this. haven't done it yet
            #
            self.payload = self.payload[7:]
            self.sessionID_A   = s[0]
            self.sessionID_B   = s[1]
            #
            self.sessionID     = int(self.sessionID_A<<24)+self.sessionID_B
            #Continue session
            log.info('     CONTINUING SESSION')
            await self.deliverPayload(self.sessionID,self.payload)
            #log.info(' Session Action: {:}  SessionID_A: {:}/0x{:04X}  Server Uptime: {:} minutes /0x{:04X}'.format(sessionAction.lookup.get(self.sessionAction),self.sessionID_A,self.sessionID_A, self.sessionID_B,self.sessionID_B))
            #log.info(' SessionID_C : {:}/0x{:08X} '.format(self.sessionID_C,self.sessionID_C))
        
        #sleepy time
        else:
          await asyncio.sleep(0)
  
    except KeyboardInterrupt:
      self.log.info('ProcessSessionIn caught KeyboardInterrupt...')

    finally:
      pass 
