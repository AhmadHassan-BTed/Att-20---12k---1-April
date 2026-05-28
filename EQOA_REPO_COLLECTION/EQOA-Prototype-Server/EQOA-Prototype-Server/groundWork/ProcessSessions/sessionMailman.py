#!/usr/bin/env python
#
# Devin and Ben 
# January 28, 2019 
#

import asyncio, socket, uvloop, struct, logging,sys

sys.path.insert(0, '../ProcessCoreIO')

from sessionManager import sessionAction
#
#################################################################################################################
#
class sessionMailman:

  def __init__(self,mailQueueIn,sessionInfo, serverShared): 
    #
    log = logging.getLogger('sessMailman')
    log.setLevel(logging.INFO)
    self.log = log 
    #
    self.mailQueueIn = mailQueueIn
    self.sessionInfo = sessionInfo
    self.serverShared = serverShared
    self.p_queue_i = None
    self.p_queue_o = None # how will this be used? 
    #
    log.info("      Initializing Session Mailman...")
    #
    #
  def start(self,loop): # when this gets called, everything in place, just looping over task
    #
    self.log.info("      Starting Session Mailman...")
    self.loop = loop
    #
    #self.p_queue_i = asyncio.Queue(0)
    #self.p_queue_o = asyncio.Queue(0)
    #
    self.loop.create_task(self.processMailIn(self.mailQueueIn,self.sessionInfo)) # asyncio calls start in here
    #Do we need a mail out? Why not send packets straight from rdpCommunicator.py to commOut portion of CommManager.py with serverShared queue
    #self.loop.create_task(self.processMailOut(mailQueueOut))


#
############################################################################
#
  async def processMailIn(self,queue,sessionInfo):
    #
    log   = logging.getLogger('PS_Mail:In')
    #
    try:
      while True: 
        if queue.empty() == True:
          await asyncio.sleep(0)
        
        else:
          item    = queue.get() # gets from multi-processing queue 
          log.info('        Incoming Mail from ProcessCoreIO.  ')
          #
          # process a NEW Session 
          # 
          if item[0] == sessionAction.NEW:
            log.info('          Addding Session to Track: 0x{:08X}'.format(item[1].sessionID))
            await sessionInfo.addSession(item[1]) # when we add session, we need to add queues once on this process 
          #
          # process a CLOSE Session
          #
          elif item[0] == sessionAction.CLOSE:
            log.info('          Removing Session from Track: 0x{:08X}'.format(item[1]))
            await sessionInfo.removeSession(item[1]) # should be sessionID 
          #
          # process a DELIVER PAYLOAD
          #
          elif item[0] == sessionAction.DELIVERTO:
            log.info('          Delivering Bundle Payload to 0x{:08X}'.format(item[1]))
            await sessionInfo.deliverPayload(item[1],item[2],item[3]) # item[1] is sessionID, item[2] is payload 
     
    except KeyboardInterrupt:
      log.info('processMailIn detected KeyboardInterrupt...')
    finally:
      pass       
