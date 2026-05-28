#!/usr/bin/env python
#
# Devin and Ben 
# December 15, 2018 
#
import asyncio,  struct, logging, binascii
#
#
#################################################################################################################
#
class connectedEndpoints:  # modeled after sessionInfo
  #
  #  add logging to all of these
  #
  def __init__(self):
    self.endpointIdSet = set()
    self.endpointList  = []
  #
  def addEndpoint(self,endpointObject):
    #
    self.endpointList.append(endpointObject)  #
    self.endpointIdSet.add(endpointObject.EndpointID)
    print(' > Added Endpoint     : 0x{:04X}'.format(endpointObject.EndpointID))
    self.printIds()
  #
  def removeEndpoint(self,endpointID):
    #
    self.endpointIdSet.remove(endpointID)
    for endpoint in self.endpointList:
      if endpoint.EndpointID == endpointID:
        self.endpointList.remove(endpoint)
        print(' > Removed Endpoint   : 0x{:04X}'.format(endpointID))
        self.printIds()
  #
  def numIds(self):
    return len(self.endpointIdSet)
  #
  def numEndpoints(self):
    return len(self.endpointList)
  #
  def checkIDconnected(self,trialID):
    return trialID in self.endpointIdSet
  #
  def printIds(self):
    print (' -------------------- ')
    print (' Endpoints Connected: ',end='')
    for id in self.endpointIdSet:
      print ('0x{:04X} '.format(id),end='')
    print ('')
    print (' -------------------- ')

#
#################################################################################################################
#
class commManager:

  def __init__(self,loop,serverShared,myProcessQueues): #Just loads these up into the loop - doesnt start them yet.
    #
    log = logging.getLogger('commManager')
    log.setLevel(logging.INFO)
    #
    self.loop              = loop
    self.log               = log 
    #
    self.serverShared = serverShared
    #
    self.myProcessQueues = myProcessQueues 
    #
    self.myConnectedEndpoints = connectedEndpoints()
    #
    log.info("      Initializing commManager...")
    #
  def start(self): # when this gets called, everything in place, just looping over task
    #
    self.log.info("      Starting commManager...")
    #
    self.loop.create_task(self.processCommQueueIn(self.myProcessQueues.comm_q_i,self.serverShared))
    self.loop.create_task(self.processCommQueueOut(self.myProcessQueues.comm_q_o))
    self.loop.create_task(self.checkCloseEndpoint(self.myProcessQueues.comm_q_e))
    #
    #####################################################
    #
    #  Will have an outgoing queue that will also be processed.  Will need to look up Endpoint information on client
    #  for PORT and IP
    #
    ##################################################################
    #
#
##################################################################### 
#
  async def checkCloseEndpoint(self,queue):
      #
    try:
      while True:
        if queue.empty() == True:
          await asyncio.sleep(0)
        else:
          item    = await queue.get() # item should be endpointID to remove
          self.log.info(' Got request to remove endpoint 0x{:04X} from commManager'.format(item))
          self.myConnectedEndpoints.removeEndpoint(item)
    except KeyboardInterrupt:
      self.log.info('checkCloseEndpoint detected KeyboardInterrupt, closing...')
    finally:
      pass
#
#####################################################################            
#
  async def processCommQueueIn(self,queue,serverShared):
    #
    # This is essentially commManager
    #
    log          = logging.getLogger('commMan:InQ')
    #
    # initialize drop counters
    #
    self.dropped = [0,0,0,0] # Failed CRC/Wrong DestinationID/Endpoint not on Server/Unknown
    #
    try:
      while True:
        if queue.empty() == True:
          await asyncio.sleep(0)

        else:
          item    = await queue.get()
          self.payload = item[0]
          self.ip      = item[1][0]
          self.port    = item[1][1]
          #
          #
          log.info(' Consuming Message Length {2} from IP: {0} PORT: {1} '.format(self.ip,self.port,len(self.payload)))
          #
          # should count all packets in and out (even if not processed)
          # this is used to get packet load on this server bytes/second, etc
          # 
          #
          # Determine if TRANSFER, NEW CONTACT, or CONNECTED CLIENT 
          #
          s = struct.unpack('<HH', self.payload[0:4]) # read first 4 bytes
          packet_source = s[0]                   # assume as if we have standard source/destination values on header 
          packet_destin = s[1]
          #
          # First check if TRANSFER Packet 
          #
          if packet_source == packetTypes.TRANSFER: 
            log.info(' Transfer packet identified, sending to TransferManager ')
            self.myProcessQueues.tran_q_i.put_nowait(self.payload)  # sends payload to transfer Manager
            #
          else: # Not TRANSFER. Now check for correct CRC
            # 
            if calculateCRC(self.payload) == 1:
              log.info(' CRC Passed ')
              log.info(' Standard Packet -  SourceID: [0x{:04X}]/DestinationID: [0x{:04X}]'.format(packet_source,packet_destin)) 
              #
              self.payload = self.payload[4:-4] 
              #
              # Check to see if this endpoint was desired destination
              # destination ID of 0xFFFE is also okay
              serverEndpointID = serverShared.serverEndpointID # will soon get from self.myserverEndpointID 
              #
              #log.info(' My EndpointID is : {:04X}'.format(serverEndpointID)) # no need to log soon
              #
              if not self.myConnectedEndpoints.checkIDconnected(packet_source): # implies this is our first time to connect 
                #
                if packet_destin == serverEndpointID or packet_destin == packetTypes.FIRST_CONTACT:
                  #
                  # this below probably doesn't go here. will be when we send payload to session.
                  # Actually, need to read message length from header. Sometimes there are two messages in a single packet.
                  # read the length, then send that in to the session queue, and then read the next header ( I think just the length)
                  # this doesn't happen a lot, but can happen and probably means we should parse message length here.
                  # or since packet class and this are so tightly coupled (same bytes), maybe we allow for session manager to process
                  # the two messages in the same packet.  that is probably better idea. 
                  #
                  # Create new Endpoint Object
                  #
                  log.info(' First Contact from New Client (Creating Endpoint)') 
                  newEndpoint = endpointInfo(str(packet_source), packet_source, self.ip, [self.port])
                  #
                  # Store new Endpoint Info here in commManager
                  # used when sending responses back to clients (to get IP and PORT)
                  #
                  log.info(' Associating New Client Endpoint with Server ')
                  #
                  self.myConnectedEndpoints.addEndpoint(newEndpoint)
                  #self.myConnectedEndpoints.printIds()
                  #
                  # Now send to Session Mananger Queue
                  #
                  log.info(' Payload sent to Session Manager (New Client)')
                  self.myProcessQueues.sess_q_i.put_nowait([packet_source,self.payload])
                else:# unknown first contact method 
                  log.warn(' Unknown first contact method for client to this server')
                  self.dropPacket(dropReason.EPT_NOT_ON_SERVER)  # Client Endpoint not on Server 
                  # 
              else:  # client is already established -- how do we know when to remove endpoint on close session - might not, or send from session
                     # can we tell when connection is closed from conduit? 
                  log.info(' Payload sent to Session Manager (Attached Client) ')
                  self.myProcessQueues.sess_q_i.put_nowait([packet_source,self.payload]) # puts in session Manager incoming queue
            else:    
              self.dropPacket(dropReason.CRC_FAILED)  # Bad CRC Check
            # 
    except KeyboardInterrupt:
      self.log.info('processCommQueueIn detected KeyboardInterrupt...')
    finally:
      pass        
#
##################################################################### 
#
  async def processCommQueueOut(self, outgoingqueue):
    #
    # This is essentially commManager out queue
    #
    log     = logging.getLogger('commMan: Out')
    log.info('Waiting for clientbound packets for CRC build outs...')
    try:
      while True:
        #Constantly checks serverShareds outgoing queue
        if self.serverShared.outboundQueue.empty() == True:
          await asyncio.sleep(0)
        #If data is in queue... pull and process.
        else:
          item    = self.serverShared.outboundQueue.get_nowait()
          #Gets us clientID to look up ip address
          s = struct.unpack('<H', item[:2])
          clientID = s[0]
          log.info('Processing')
          #log.info(' Sending Message Length {2} to IP: {0} PORT: {1} '.format(item[1][0],item[1][1],len(self.payload)))
          for endpoint in self.myConnectedEndpoints.endpointList: 
            if endpoint.EndpointID == clientID:
              
              #Get's Server endpoint and prepends to bundle
              item = struct.pack('<H', self.serverShared.serverEndpointID) + item
              addr = (endpoint.IPaddress, endpoint.PortList[0])
              #Calculates crc
              crc = crcMethod(item)
              #Appends crc
              item = item + struct.pack('<I', crc)
              #Format's our packet into a list
              packet = [item, addr]
              #Puts our packet into the queue which feeds it into UDP protocol
              self.log.info('Sending packet to UDP Layer')
              outgoingqueue.put_nowait(packet)
     
    except KeyboardInterrupt:
      self.log.info('processCommQueueOut detected KeyboardInterrupt...')
    finally:
      pass      
        
#
##################################################################### 
#
  def dropPacket(self,reason):
    #  
    log     = logging.getLogger('dropPacket')
    #
    # print out hex for dropped packet, keep track of total dropped packets (and why)
    #
    #
    if reason == dropReason.CRC_FAILED:
      #
      log.info(' CRC Failed. Dropping Packet Payload ')
      self.dropped[0] += 1
      #
    elif reason == dropReason.WRONG_DEST_ID:
      #
      log.info(' Wrong Destination: Dropping Packet Payload ')
      self.dropped[1] += 1
      #
    elif reason == dropReason.EPT_NOT_ON_SERVER:
      #
      log.info(' Client EndpointID not on Server: Dropping Packet Payload ')
      self.dropped[2] += 1
      #
    else:
      #
      log.info(' Unknown Error: Dropping Packet Payload')
      self.dropped[3] += 1
      #
    
    self.log.info(' Dropped  Message from IP: {:} PORT: {:} '.format(self.ip, self.port))
    self.log.info(' Data: {:}'.format(self.payload.hex()))
    self.log.info(' Drop Totals: {:}'.format(self.dropped))
#
#################################################################
# 
class dropReason:
  #
  CRC_FAILED        = 0
  WRONG_DEST_ID     = 1
  EPT_NOT_ON_SERVER = 2
#
class packetTypes:
  #
  NONE              = 0x0000
  TRANSFER          = 0xFFFF
  FIRST_CONTACT     = 0xFFFE
#
#
###############################################################################
#     
class endpointInfo:
    #
    # myName 		is a human readable name
    # myEndpointID 	is my endpoint ID
    # myIPaddress  	is my IP address
    # myPort 		is list of ports the ServerEndpoint listens on
    #
    # EndpointInfo associated with rdpServers are created when they are init'd
    # Client EndpointInfo is created when the client first attaches to the Complex 
    #
    def __init__(self, myName, myEndpointID, myIPaddress, myPortList):
        self.Name       = myName
        self.EndpointID = myEndpointID
        self.IPaddress  = myIPaddress
        self.PortList   = myPortList
        log = logging.getLogger('Endpoint')
        log.setLevel(logging.INFO)
        self.log = log
        log.info(' >> New EndpointInfo Object Created: {:}'.format(self.Name))

    

    def printep(self):
        epout = ''
        for p in self.PortList:
          epout += 'Name: ' + self.Name + '  ID: ' + '0x{:4X}'.format(self.EndpointID) + '  IP: ' + self.IPaddress + '  Port: ' + '{:5d}'.format(p) + '\n'
        return epout

#
#
#################################################################
# 
def calculateCRC(payload):
  #
  decode_fmt  = '<'  # unpack as little endian
  decode_fmt += 'I'  # 4 byte Received CRC value
  s = struct.unpack(decode_fmt,payload[len(payload)-4:])
  crcReceived      = s[0]
  crcPayload       = bytearray()
  crcPayload.extend(payload[0:len(payload)-4])
  crcCalculated    = ((binascii.crc32(crcPayload)^0xFFFFFFFF)^0x11f19ed3)&0xFFFFFFFF
  #
  result = 0
  if crcCalculated == crcReceived:
    result = 1
  #
  return result 

#
###################################################################
#
def crcMethod(payload):
    crcPayload       = bytearray()
    crcPayload.extend(payload[0:len(payload)])
    crcCalculated    = ((binascii.crc32(crcPayload)^0xFFFFFFFF)^0x11f19ed3)&0xFFFFFFFF
    return crcCalculated
