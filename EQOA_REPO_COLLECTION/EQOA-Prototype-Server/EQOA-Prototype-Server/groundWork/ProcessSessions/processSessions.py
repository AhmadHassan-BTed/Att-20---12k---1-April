#!/usr/bin/env python
#
# ProcessSession.py 
#
# Devin and Ben 
# January 26, 2019 
#
import asyncio, uvloop, logging, sys
#
sys.path.insert(0, '../ProcessCoreIO')

from sessionMailman import sessionMailman
from sessionJockey  import sessionJockey
from rdpCommunicate  import rdpCommunicator
#
######################################################################
#
class sessionInfo:
  #
  #  add logging to all of these
  #
  def __init__(self, serverShared):
    self.sessionIdSet = set()  
    self.sessionList  = [] 
    self.serverShared = serverShared
    self.log          = logging.getLogger('sessionInfo:')
  #
  async def addSession(self,sessionObject):
    #
    # add queues at this point?
    #
    sessionObject.qIN         = asyncio.Queue(0)
    sessionObject.qOut        = asyncio.Queue(0)
    sessionObject.qVirginOut  = asyncio.Queue(0)
    #Attachs sessionInfo to the session
    sessionObject.sessionInfo = self
    self.log.info('Creating session..')
    #
    self.log.info('Creating RDPComm..')
    sessionObject.myRDPComm = rdpCommunicator(sessionObject) # init the object
    self.log.info('Setting RDPComm queue\'s..')
    sessionObject.myRDPComm.setQueues(sessionObject.qIN,sessionObject.qOut,sessionObject.qVirginOut)   
    #
    self.sessionList.append(sessionObject)  # might just need to send minimum information and create session on ProcessSessions with 
    self.sessionIdSet.add(sessionObject.sessionID)
    print(' > Added Session      : 0x{:08X}'.format(sessionObject.sessionID))
    self.log.info(' > Added Session       : 0x{:08X}'.format(sessionObject.sessionID))
    self.printIds()
  #
  async def removeSession(self,sessionID): # don't need/won't have whole session Object to pass in
    # 
    # Remove Session 
    #
    # need to make sure queues are empty, and then null them out before removal
    #
    #Sometimes a removed sessionID tries to be removed again, unsure why, helps correct the issue
    try:
      self.sessionIdSet.discard(sessionID)
      for session in self.sessionList:
        if session.sessionID == sessionID:
          session.myRDPComm.clearQueues()
          self.sessionList.remove(session)
      print(' > Removed Session    : 0x{:08X}'.format(sessionID))
      self.printIds()
      #
    except:
      self.log.info('Error occured removing {}.'.format(hex(sessionID)))

    finally:
      pass
  async def deliverPayload(self,sessionID,payload,remoteMaster): 
    #
    for session in self.sessionList:         # slow but okay for now
      if session.sessionID == sessionID:
        session.remoteMaster = remoteMaster
        session.qIN.put_nowait(payload) 
        #session.qIN.put(payload)
        print(' > Delivered Payload  : 0x{:08X}'.format(sessionID))
  #
  def numIds(self):
    return len(self.sessionIdSet)
  #
  def numSessions(self):
    return len(self.sessionList)
  #
  def checkIDconnected(self,trialID):  
    return trialID in self.sessionIdSet 
  #
  def printIds(self):
    print (' -------------------- ')
    print (' Sessions Connected: ',end='')
    for id in self.sessionIdSet:
      print ('0x{:08X} '.format(id),end='')
    print ('')
    print (' -------------------- ')


 # transfer will need to handled higher up.

#
######################################################################
#
class processSessions:
  
  def __init__(self, serverShared):
    log = logging.getLogger('P_Sessions')
    log.setLevel(logging.INFO)
    log.info("    ")
    log.info("    Initializing ProcessSessions...")
    self.log = log
    self.serverShared = serverShared
    #
    self.mySessionInfo = sessionInfo(self.serverShared)  # will hold information on connected sessions
    #
    self.myMailman =  sessionMailman(self.serverShared.mailQueueIn,self.mySessionInfo, self.serverShared) # init the mailman  - eventually add mailQueueOut 
    #self.myJockey  =  sessionJockey(self.mySessionInfo) # init the jockey   (don't have to pass in since it is self)

  def start(self):
    log = self.log
    log.info("   ")
    log.info("    Starting ProcessSessions...")
    # 
    # Start asynic here
    #
    asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())
    sessionloop = asyncio.get_event_loop()
    #
    log.info("  ")
    self.myMailman.start(sessionloop)
    #self.myJockey.start(sessionloop)
    #
    try:
        log.info("  ")
        log.info("  Session Event Loop Listening...")
        sessionloop.run_forever()
    except KeyboardInterrupt:
        log.info("  ...")
        log.info("  Caught user interrupt (ProcessSessions)...")
        log.info("  ...")
    finally:
        log.info("    Session Event Loop Closing...")
        pending = asyncio.Task.all_tasks()
        sessionloop.run_until_complete(asyncio.gather(*pending))
        sessionloop.close()
#
#####################################################################            
#
