#!/usr/bin/env python
#
# ProcessCoreIO.py 
#
# Devin and Ben 
# December 15, 2018 
#
import asyncio, uvloop, logging
#
from commManager     import commManager 
from sessionManager  import sessionManager 
from transferManager import transferManager 
#
#from multiprocessing import Process, Manager
#
from udpClasses import udpEndpoint, udpDatagramProtocol
#
######################################################################
#
class processQueues:
  
  def __init__(self):
    self.comm_q_i = asyncio.Queue(0) 
    self.comm_q_e = asyncio.Queue(0) 
    self.comm_q_o = asyncio.Queue(0)
    self.sess_q_i = asyncio.Queue(0) 
    self.tran_q_i = asyncio.Queue(0) 

# add enum later and use list 
#
######################################################################
#
class processCoreIO:
  
  def __init__(self,sharedProcessData):
    log = logging.getLogger('P_CoreIO')
    log.setLevel(logging.INFO)
    log.info("    ")
    log.info("    Initializing ProcessCoreIO...")
    self.log = log
    #
    self.serverShared  = sharedProcessData # cross processor sharing 
    #
  def start(self, port):
    log = self.log
    log.info("   ")
    log.info("    Starting ProcessCoreIO...")
    # 
    # Start asynic here
    #
    asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())
    loop = asyncio.get_event_loop()
    #
    self.myProcessQueues = processQueues()   # asyncio queues for this process
    #
    conduitTransports = []
    conduitProtocols  = []
    myConduitsInfo    = [] # Eventually be in config file or database - will be passed in by rdpServer object 
    for port in self.serverShared.serverPortList:  
      myConduitsInfo.append({'ip':'0.0.0.0','port':port,'queue_size':None})
    #myConduitsInfo.append({'ip':'192.168.1.112','port':10071,'queue_size':None})
    #
    log.info("      ")
    log.info("      Initializing IO Conduits...")
    #
    for c,conduit in enumerate(myConduitsInfo):
      thisConduit = loop.create_datagram_endpoint(
                      local_addr       = (conduit['ip'], conduit['port']),
                      protocol_factory = lambda: udpDatagramProtocol(udpEndpoint(self.myProcessQueues.comm_q_i,self.myProcessQueues.comm_q_o),c))
      transport, protocol  = loop.run_until_complete(thisConduit)
      conduitTransports.append(transport)
      conduitProtocols.append(protocol)
    #
    log.info("  ")
    self.myCommManager     =  commManager(    loop,self.serverShared,self.myProcessQueues)    # Just loads things into the loop to run
    self.mySessionManager  =  sessionManager( loop,self.serverShared,self.myProcessQueues)    # Just loads things into the loop to run
    self.myTransferManager =  transferManager(loop,self.serverShared,self.myProcessQueues)    # Just loads things into the loop to run
    #
    log.info("  ")
    self.myCommManager.start()
    self.mySessionManager.start()
    self.myTransferManager.start() 

    #
    try:
        log.info("  ")
        log.info("  CoreIO Event Loop Listening...")
        loop.run_forever()
    except KeyboardInterrupt:
        log.info("  ...")
        log.info("  Caught user interrupt...")
        log.info("  ...")
    finally:
        #pending = asyncio.Task.all_tasks()
        #loop.run_until_complete(asyncio.gather(*pending))
        #Test
        loop.run_until_complete(loop.shutdown_asyncgens())
        self.log.info("    Closing IO Conduits...")
        for t,transport in enumerate(conduitTransports):
          transport.close()  # 0 entry is the transport
          log.info("      Closing Conduit #{:}".format(t))
        log.info("    CoreIO Event Loop Closing...")
        loop.close()

#
