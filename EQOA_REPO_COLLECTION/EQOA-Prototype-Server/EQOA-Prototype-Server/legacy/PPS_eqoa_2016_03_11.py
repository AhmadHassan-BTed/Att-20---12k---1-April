#!/usr/bin/python -u
#
# Benturi
# March 06, 2016
#
from pycrc.crc_algorithms import Crc
from scapy.all import *
import binascii
import struct
import sys
import time
#
##############################################################
#
# Define Endpoint Class
#
class ServerEndpoint:
   'Common base class for all Area Server Endpoints'
   epCount = 0

   def __init__(self, id, name,ip, port):
      self.id   = id
      self.ip   = ip
      self.port = port
      self.name = name
      ServerEndpoint.epCount += 1
      self.connections=[]
      self.num_master_connections = 0
      self.num_slave_connections  = 0      
   
   def displayCount(self):
     print "Total Area Server Endpoints active %d" % ServerEndpoint.epCount

   def displayEndpoint(self,num):
      print "Area Server # : ",num
      print "Endpoint ID   : ", '0x{:04X}'.format(self.id)
      print "Endpoint Name : ", self.name
      print "Endpoint IP   : ", self.ip
      print "Endpoint Port : ", self.port
      print "Num Master Con: ",self.num_master_connections
      print "Num Slave  Con: ",self.num_slave_connections
      conlist = []
      for c in self.connections:
        conlist.append( '0x{:04X}'.format(c.remoteid))      
      print "Connection w/ : ",conlist
      print
      
   def startConnection(self, localid, remoteid, masterid, sessionid):
     if localid == masterid:
       self.num_master_connections +=1
       self.connections.append(ConnectionObject(localid, remoteid,localid, sessionid))
     elif remoteid == masterid:
       self.num_slave_connections +=1
       self.connections.append(ConnectionObject(localid, remoteid, remoteid, sessionid))
     
     
#      
##############################################################
#
# Define Endpoint Class
#
class ConnectionObject:
   'Common base class for all EQOA Connections'
  
   def __init__(self, localid, remoteid,masterid, sessionid):
     #
     # Info on where to send is in endpoint/area server object
     #
      self.localid   = localid
      self.remoteid  = remoteid
      self.masterid  = masterid
      self.sessionid = sessionid      
     # 
      self.incoming_queue=[]
      self.outgoing_virgin_queue=[]
      self.outstanding_queue=[]
     #
      self.incoming_queue_length        = 0
      self.outgoing_virgin_queue_length = 0
      self.outstanding_queue_length     = 0     
     # 
      self.timeout_tracking   = 0
      self.bandwidth_tracking = 0 
     #
   def addToIncomingQueue(self,packet):
      self.incoming_queue.append(packet)
      self.incoming_queue_length +=1
      
   def printConnectionInfo(self):
      print '  LOCAL  ENDPOINT ID: ','0x{:04X}'.format(self.localid)
      print '  REMOTE ENDPOINT ID: ','0x{:04X}'.format(self.remoteid)  
      if self.masterid == self.localid:      
        print '  LOCAL ENDPOINT IS MASTER: ','0x{:04X}'.format(self.masterid)
      else:
        print '  REMOTE ENDPOINT IS MASTER: ','0x{:04X}'.format(self.masterid)
        print      
      
   def printQueueLengths(self):
      print '  INCOMING    QUEUE LENGTH: ','0x{:04d}'.format(self.incoming_queue_length)
      print '  OUTGOING    QUEUE LENGTH: ','0x{:04d}'.format(self.outgoing_virgin_queue_length)
      print '  OUTSTANDING QUEUE LENGTH: ','0x{:04d}'.format(self.outstanding_queue_length)
      print   
#      
##################################################################
#
# Define EQOA_Packet Class
#      
class EQOApacket:
   'Common base class for all EQOAPackets'
#  
   def __init__(self, pkt):
     #
     self.rawpacket    = pkt.load
     self.length       = count(self.rawpacket)
     self.remoteid      = 0
     self.localid       = 0
     #
     self.crc32calcd   = 0
     self.crc32recv    = 0
     self.crcpass      = 0
     #
     self.isTransfer   = 0
    #          
     self.payload=[] # hold transfers/bundles in here to process
   
   def processPayloadHeader(self):
      hdr_fmt1 = '<HH'   # (little endian)[SRC ADDR][DST ADDR]
      s = struct.unpack(hdr_fmt1,self.rawpacket[0:4]) # using hdr_fmt1, unpack the first four bytes   
      if s[0] == 0xFFFF:       # Check to see if Transfer  
        self.isTransfer = 1 
        self.payload.append(self.rawpacket[4:self.length])
        #
      else:                    # This is a standard packet
        self.remoteid  = s[0]
        self.localid   = s[1]
    
   def printPacketHeader(self):
      print 
      if self.isTransfer==1:
        print '  TRANSFER PACKET : ','0x{:04X}'.format(0xFFFF)      
      else:
        print '  REMOTE ID      : ','0x{:04X}'.format(self.remoteid)
        print '  LOCAL  ID      : ','0x{:04X}'.format(self.localid)
        print '  PACKET LENGTH  : ','0x{:04X}'.format(self.length) , '[{:d}'.format(self.length),'bytes]'
      print      
     
   def processPacketFooter(self):
     payload = self.rawpacket
     crc = Crc(width = 32, poly = 0x04c11db7 ,
        reflect_in = True, xor_in  = 0xffffffff,
       reflect_out = True, xor_out = 0x11f19ed3)
#
     s = struct.unpack('<I',payload[len(payload)-4:len(payload)]) #  
     self.crc32recv  = s[0]
     self.crc32calcd = crc.bit_by_bit_fast(payload[0:len(payload)-4])      # calculate the CRC, using the bit-by-bit-fast algorithm.
#
     self.crcpass = 0
     if self.crc32recv == self.crc32calcd:
       self.crcpass = 1

   def printPacketFooter(self):
     print 
     print '  RECEIVED   CRC : ','0x{:08X}'.format(self.crc32recv)
     print '  CALCULATED CRC : ','0x{:08X}'.format(self.crc32calcd)
     if self.crcpass==1:
       print '  CRC STATUS     :  PASSED'
     else:
       print '  CRC STATUS     :  FAILED'
     print        
#  
   def printPacketHexdump(self):
     print
     print hexdump(self.rawpacket)     
#
#     
   def extractAndRouteBundles(self): # extract bundles to connections in area servers
     print   
     
   def extractBundleSession(self):
      message = self.rawpacket
      b = 4
     #
      u_in = 1                                        # specify number of units to extract
      b_in = 6                                        # number of bytes to read
      b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
      hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
      ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
      it = unit_extractor(u_in,b_in,ot)
      s  = it[2]
      b  = b+it[1]     
      
      self.bundle_size    = s2e(s[0])&0x000FFF
      self.bundle_class   = (s2e(s[0]&0x00F000)>>12)
      self.session_action = (s[0]&0xFF0000)>>16
      self.masterflag = 0
      if self.session_action != 0x0:
        self.masterflag = 1
#      
      print '  SESSION ACTION : ','0x{:02X}'.format(self.session_action)
      print 
      print '  BUNDLE CHANNEL : ','0x{:1X}'.format(self.bundle_class)
      print '  BUNDLE LENGTH  : ','0x{:03X}'.format(self.bundle_size),  '[{:d} bytes]'.format(self.bundle_size)  
#
      hdr_fmt1 = '<I'   # Read length of SERVER NAME
      s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
      b = b + 4
      print
      self.session_id1 = s[0]
      print '  SESSION ID 1   : ','0x{:08X}'.format(self.session_id1),'{:012d}'.format(self.session_id1)
#
      self.session_id2 = 0x0
      if s[0] > 0x0FFFFF: 
        u_in = 1                                        # specify number of units to extract
        b_in = 10                                       # number of bytes to read
        b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]      
        self.session_id2 = s[0]
        print '  SESSION ID 2   : ','0x{:08X}'.format(self.session_id2),'{:012d}'.format(self.session_id2)

#      
######################################################################      
#
def count(iter):
    try:
        return len(iter)
    except TypeError:
        return sum(1 for _ in iter)
#
#        
#
def s2c(value): # server to client representation
#
  bt =[]
  v1 = len(str(value)) # length of this value
  if v1%2 !=0:         # if not even number, make longer
    v1=v1+1
  ty = '{:0'+str(v1)+'X}' # this makes sure we have multiple of two - formats '{:08X}'
  s = ty.format(value)    # prints it out to 's'
#  
  for x in range(0,len(s),2):
    bt.append(int(s[x:x+2], 16)) # loops over each character byte and writes it to bt
#    
  bitflip = bt[len(bt)-1]%2 # determines if last bit is 1 or 0.  
#
#  print bitflip, bt,s, len(bt)
#
#
  #  forneg     = 0 # This is value that might be used if bitflip is set
  shift      = 0
  fshift     = 0
  sumfn      = 0
  sumbt      = 0
  num_bytes = len(bt)
 #
  fn =[]
  for y in range(num_bytes-1,-1,-1):  # remove upper most bit
     if bt[y] > 0x80:    # don't need to do this since we are dropping bit anyway
       bt[y] = bt[y] - 0x80
     fn.append(0xFF) # this is used if this is a bit flip. Creates a FFFF size of input
#  print
#  print fn,bt
#
# 
#
  for y in range(num_bytes-1,-1,-1): 
     bt[y] = int(bt[y])<<shift
     fn[y] = int(fn[y])<<shift
     sumbt = sumbt + int(bt[y])
     sumfn = sumfn + int(fn[y])
     shift = shift+8     # keeps track of how many
     
#     sum = sum + int(bt[y]>>fshift)
#     fshift = fshift +1
  
#  sumbt = sumbt >>1  # drops the lowest bit for all
#  sumfn = sumfn >>1  # drops the lowest bit for all
#  print
#  print '{:b}'.format(sumbt)
#  print '{:b}'.format(sumfn)
#  print '{:b}'.format(sumfn-sumbt)  
#  print '{:b}'.format(~(sumfn-sumbt))    
#  print
  if bitflip == 1:

#    sum=forneg&sum   # need this for icons to be correct 

#    sumfn = forneg >> (fshift*2-1)
#    print 
#    print '{:b}'.format(sum)
#    print '{:b}'.format(forneg)
    sumbt=sumfn&sumbt
    sumbt = ~sumbt  # this is a bitwize invert
#    if sum < -0xFF:
#      sum = forneg+sum
#    print
#    print ' FORNEG' ,'0x{:010X}'.format(forneg)
#    print 
# can I & with FFFFFFF as long as sum
#    sum=forneg&sum   # need this for icons to be correct 
#    sum = sum - forneg -1
#    print "FINA SUM ",'  {:d}'.format(sum)
#    print 
# 
#
  return sum
  
  
def s2e(value): # 
#
  bt =[]
  v1 = len(str(value)) # length of this value
  if v1%2 !=0:         # if not even number, make longer
    v1=v1+1
  ty = '{:0'+str(v1)+'X}' # this makes sure we have multiple of two - formats '{:08X}'
  s = ty.format(value)    # prints it out to 's'
#  
  for x in range(0,len(s),2):
    bt.append(int(s[x:x+2], 16)) # loops over each character byte and writes it to bt
#    
  shift      = 0
  fshift     = 0
  sumbt      = 0
  num_bytes = len(bt)
 #
  for y in range(num_bytes-1,-1,-1):  # remove upper most bit
     if bt[y] >= 0x80:                 # don't need to do this since we are dropping bit anyway
       bt[y] = bt[y] - 0x80
#     
  for y in range(num_bytes-1,-1,-1): 
     bt[y] = int(bt[y])<<shift
     sumbt = sumbt + int(bt[y])
     shift = shift+8     # keeps track of how many
     
  return sumbt  
  
  
  
def unit_extractor(u_in,b_in,s):
#
  num_unit = 0
  b_start  = 0
  units    = [];
#
  for iter, v in enumerate(s):
    if num_unit < u_in:   # only continue if we haven't extracted all units at this point
      if v < 0x80:
        units.append(s[b_start:iter+1])
        num_unit = num_unit+1  
        b_start = iter+1      
        last_b  = iter+1
#                         # shifts bits to correct length and takes care of little endianess
  s_units = []
  for this_unit in units:
    sum   = 0
    shift = 0  
    for this_byte in this_unit:
      sum = sum + int(this_byte<<shift)
      shift = shift + 8      
    s_units.append(sum)      
#

#    
  if num_unit < u_in:
     print ' Please check analysis. Could not extract desired number of units'
     print ' Units Requested: ',u_in
     print ' Units Extracted: ',num_unit
#
  return (num_unit,last_b,s_units)

#
def processBundle(payload):
  print

def processTransfer(payload):        
  print        
       
#
# Name output file, not clean, but okay for now
#
OUTFILEBASE='PPS.out'
sys.stdout = open(OUTFILEBASE+'.out', 'w')
#
# Write current date and time to output file
#
#now = time.strftime("%c")
print '-- Date      : ' + time.strftime("%c")
#
#####################################################################
#
# Generate some endpoint Objects
#
#
login_servers = []
#login_servers.append(LoginServer("LOGIN SERVER ","64.37.156.200",45339))
#
# First connect to SHARD server gets IP on memory card, but is unknown endpoint ID
shard_servers = []  # Clients connect here to choose World Server 
shard_servers.append(ServerEndpoint(0x73B0,"CLW - SHARD  ","199.108.200.65",10070))
#shard_servers.append(ServerEndpoint(0x5F76,"MEXICO - SHARD1","199.108.200.46",10070))
#shard_servers.append(ServerEndpoint(0x73B0,"MEXICO - SHARD2","199.108.200.40",10070))
#shard_servers.append(ServerEndpoint(0x5F76,"MEXICO - SHARD3","199.108.200.73",10070))
#shard_servers.append(ServerEndpoint(0x5F76,"2 - SHARD4","199.108.200.71",10070))
#shard_servers.append(ServerEndpoint(0x5F76,"2 - SHARD5","199.108.200.77",10070))
#
# Will initially init all the area_servers and start them up
#
area_servers = []
#area_servers.append(ServerEndpoint(0x73B1,"CLW - WORLD40","199.108.10.40", 10070))
#area_servers.append(ServerEndpoint(0x73B2,"CLW - WORLD41","199.108.10.41", 10070))
#area_servers.append(ServerEndpoint(0x73B3,"CLW - WORLD42","199.108.10.42", 10070))
#area_servers.append(ServerEndpoint(0x73B4,"CLW - WORLD43","199.108.10.43", 10070))
#area_servers.append(ServerEndpoint(0x73B5,"CLW - WORLD47","199.108.10.47", 10070))
area_servers.append(ServerEndpoint(0x1F0A,"CLW - WORLD48","199.108.10.48", 10070))      
 
UDP_servers=shard_servers+area_servers
 
#for idx,p in enumerate(UDP_servers):   
#  p.displayEndpoint(idx)  
#for idx,p in enumerate(UDP_servers):       
#   p.startConnectionAsMaster(0xAAAA,0x12345678)
#   if idx%2==0:
#     p.startConnectionAsMaster(0xBBBB,0x12345678)
for idx,p in enumerate(UDP_servers):    
  p.displayEndpoint(idx)     
#
#print
#for idx,p in enumerate(UDP_servers):
#  p.displayEndpoint(idx)
  
#IP
# Loop over live packets, but should specify ports, and stuff...
#
pcap_info=[]
pcap_info.append([0,0,0,0,0,0])  
pcap_info.append(["../1 PacketAnalysis/PCAPS/MATT_ORIG_pkt_log.pcap","192.168.1.109",300,150,0,'Dudderz'])
pcap_info.append(["../1 PacketAnalysis/PCAPS/Fixed_dump_with_username_removed.pcap","192.168.0.87",300,150,0,'Famorf'])
pcap_info.append(["../1 PacketAnalysis/PCAPS/Trif_with_username_removed.pcap","192.168.0.5",300,0,0,'Trifixion'])
pcap_info.append(["../1 PacketAnalysis/PCAPS/dread.pcap","192.168.0.69",300,0,0,'Dread']) #4
pcap_info.append(["../1 PacketAnalysis/PCAPS/DreadSolo.pcap","192.168.0.5",300,0,0,'DreadSolo'])
pcap_info.append(["../1 PacketAnalysis/PCAPS/fixed_Mexico_pkt_log.pcap","192.168.1.74",785,345,0,'Mexico']) # 4
#
jp = 1
#
PCAPNAME    = pcap_info[jp][0]
CLIENTIP    = pcap_info[jp][1]
PCAPCOUNT   = pcap_info[jp][2]
PCAPUDP1ST  = pcap_info[jp][3]
PCAPALL     = pcap_info[jp][4]
OUTFILEBASE = pcap_info[jp][5]  

PCAPALL     = 0

#PCAPNAME = "../1 PacketAnalysis/PCAPS/MATT_ORIG_pkt_log.pcap"
#PCAPALL = 1
#PCAPCOUNT = 2700 #165
#PCAPCOUNT = 500

if PCAPALL==1:
 pcap_file = rdpcap(PCAPNAME)  # reads the first 'count' packets
else:
 pcap_file = rdpcap(PCAPNAME,count=PCAPCOUNT)  # reads the first 'count' packets
print

#
# This is where we will be looping over live packets
#

#  for pkt in sniff(iface='eth1'):
for index, pkt in enumerate(pcap_file):
    try:
      for p in UDP_servers:
        if pkt.haslayer(IP) and (pkt.getlayer(IP).dst==p.ip or pkt.getlayer(IP).src==p.ip):  
#        
          a = EQOApacket(pkt)
#
          print '*****************************************************'
          print           
          print 'Packet : ',index
          print 'Accepted by Server Complex for Routing...'
          print 'Destination IP  : ',pkt.getlayer(IP).dst
          print 'Destination Name: ',p.name
          print 'Destination ID  : ','0x{:04X}'.format(a.remoteid) , '[',a.remoteid,']'
#
          a.printPacketHexdump()         
          a.processPayloadHeader()
          a.printPacketHeader()

          #
          if a.isTransfer==0:
            a.processPacketFooter()
            a.printPacketFooter()
            a.extractBundleSession()
#
            if a.session_action != 0x0:            # I am slave, need to do what master says
              print
              if a.session_action == 0x21:  # Then create new communication endpoint
                print '   -- MASTER SAYS TO START SESSION'
                p.startConnection(a.localid, a.remoteid, a.localid, a.session_id1)
              elif a.session_action == 0x14:  # Then delete communication endpoint
                 print '   --MASTER SAYS TO END SESSION'           
              elif a.session_action == 0x01:  # I am slave, communication channel open
                print '   --MASTER SAYS TO CONTINUE SESSION'            
              else:
                print ' **  UNKNOWN SESSION ACTION: ', a.session_action
            
            
             
              
           # processBundle(a.payload)
          else:
            print
            #processTransfer(a.payload)
#

            
          
 
#
# Deliver packet to correct Server Endpoint
#
 
          print
#        
    except:
        raise
#  
for idx,p in enumerate(UDP_servers):    
  p.displayEndpoint(idx)   


#  for pkt in sniff(iface='eth1'):
#    if IP in pkt and pkt[IP].src.endswith('5'):
#        pkt[IP].dst = '1.1.1.1'
#        sendp(pkt, iface='eth2')
 
#
# Notes for HTTP Python server
#
 
#import SimpleHTTPServer
#import SocketServer
#PORT = 8000
#Handler = SimpleHTTPServer.SimpleHTTPRequestHandler
#httpd = SocketServer.TCPServer(("", PORT), Handler)
#
#print "serving at port", PORT
#httpd.serve_forever()   

# DNS python infor
#https://thepacketgeek.com/scapy-p-09-scapy-and-dns/
#
