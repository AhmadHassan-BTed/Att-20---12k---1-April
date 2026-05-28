#!/usr/bin/env python
#
# this is rdpServer.py
#
#
# Devin and Ben 
# December 15, 2018 
#
import sys, asyncio, logging
from multiprocessing import Manager, Process

sys.path.insert(0, './ProcessCoreIO')
sys.path.insert(1, './ProcessSessions')
from processCoreIO   import *
from processSessions import *

class sharedProcessData:

  def __init__(self,myManager):
    self.myManager = myManager
  

class rdpServer:

  def __init__(self,myEndpointInfo):
    log = logging.getLogger('rdpServer')
    log.setLevel(logging.INFO)
    self.Name       = myEndpointInfo.Name
    self.IP         = myEndpointInfo.IPaddress
    self.EndpointID = myEndpointInfo.EndpointID  
    self.PortList   = myEndpointInfo.PortList 
    #
    log.info("  Initializing RDP Server...")
    log.info("  Name: "+self.Name + "  IP: " + self.IP)
    for i in self.PortList:
      log.info("  Port: " + '{}'.format(i))
    # 
    # Set up sharing info on init
    #
    self.mySharedProcessData                  = sharedProcessData(Manager()) 
    self.mySharedProcessData.serverEndpointID = self.EndpointID
    self.mySharedProcessData.serverPortList   = self.PortList
    self.mySharedProcessData.mailQueueIn      = self.mySharedProcessData.myManager.Queue(0)
    self.mySharedProcessData.outboundQueue    = self.mySharedProcessData.myManager.Queue(0)
    self.mySharedProcessData.masterSessionIn  = self.mySharedProcessData.myManager.Queue(0)
 
  def start(self, myPorts):
    log = logging.getLogger(self.Name)
    log.setLevel(logging.INFO)
    log.info("  Starting RDP Server...")
    # 
    myProcessCoreIO   = processCoreIO(self.mySharedProcessData)   # init ProcessCoreIO unit
    myProcessSessions = processSessions(self.mySharedProcessData)   # init ProcessSessions unit

    rdpServerProcesses = [ Process(target = myProcessCoreIO.start,     args=(myPorts,)),
                           Process(target = myProcessSessions.start,   args=())]#,
                         #  Process(target = ProcessActors,           args=(serverShared,))]
    

    try:
      for process in rdpServerProcesses:
        process.start()
      for process in rdpServerProcesses:
        process.join()
    except KeyboardInterrupt:
      log.info("  ...")
      log.info("  Caught user interrupt...")
      log.info("  ...")
    finally:
      for process in rdpServerProcesses:
        process.terminate()
      for process in rdpServerProcesses:
        process.join() 


