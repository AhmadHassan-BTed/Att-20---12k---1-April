#!/usr/bin/env python
#
# Devin and Ben 
# January 18, 2019 
#
import asyncio, logging
#
#################################################################################################################
#
class transferManager:

  def __init__(self,loop,serverShared,myProcessQueues): #Just loads these up into the loop - doesnt start them yet.
    #
    self.loop              = loop
    #
    log = logging.getLogger('transManager')
    log.setLevel(logging.INFO)
    self.log = log 
    #
    self.serverShared = serverShared
    #
    self.myProcessQueues = myProcessQueues
    #
    log.info("      Initializing transferManager...")
    #
    #        self.myTransferOutSessions = []
    #        self.myTransferInSessions = []

    #
  def start(self): # when this gets called, everything in place, just looping over task
    #
    self.log.info("      Starting transferManager...")
#
#
#####################################################################            
#
      #              #
      #              # Need to unencode transfer packet
      #              # Make sure we have incoming transfer in incoming session/message queue
      #              # if not, ,keep checking. if so, place it into normal sessions and then send ACK
      #              # and also send ACK to old area_server it it can remove it from list
      #              # might not make a list, might just flag as incoming
      #              #
      #              print 'PACKET TYPE           : TRANSFER (' + '0x{:4X}'.format(packet_source) + ')'



#
