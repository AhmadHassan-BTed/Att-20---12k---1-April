#!/usr/bin/env python
#
# Devin and Ben 
# February 6, 2019 
#
import asyncio, logging
#
#################################################################################################################
#
class sessionJockey:

  def __init__(self,sessionInfo): 
    #
    log = logging.getLogger('sessJockey')
    log.setLevel(logging.INFO)
    self.log = log 
    #
    self.sessionInfo = sessionInfo
    #
    log.info("      Initializing Session Jockey...")
    #
    #
  def start(self,loop): # when this gets called, everything in place, just looping over task
    #
    self.log.info("      Starting Session Jockey...")
    self.loop = loop
    #
    self.loop.create_task(self.executeSessions()) # session info already in self
#
############################################################################
#
  async def executeSessions(self):
    #
    log   = logging.getLogger('Jockey')
    log.info('')
    #
    while True:
      await asyncio.sleep(0)
      if len(self.sessionInfo.sessionList) >0:
        for session in self.sessionInfo.sessionList:
          await session.execute()

