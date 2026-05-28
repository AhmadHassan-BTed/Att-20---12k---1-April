#!/usr/bin/python -u
#
# Benturi
# June 21, 2016
#
#Matt Test FTP
from scapy.all import *
from multiprocessing import Process
from multiprocessing import Queue

import binascii
import struct
import time

import sys

sys.path.insert(0, './framework')
sys.path.insert(0, './loginserver')
sys.path.insert(0, './objects')
sys.path.insert(0, './operations')
sys.path.insert(0, './pcaps')

import eqoa_endpoints 

#
# Read PCAP information to play back
#   
pcap_info = []
pcap_info.append(["./legacy/MATT_ORIG_pkt_log.pcap", "192.168.1.109", 1300, 150, 0, 'Dudderz'])
jp = 0
#
PCAPNAME = pcap_info[jp][0]
CLIENTIP = pcap_info[jp][1]
PCAPCOUNT = pcap_info[jp][2]
PCAPUDP1ST = pcap_info[jp][3]
PCAPALL = pcap_info[jp][4]
OUTFILEBASE = pcap_info[jp][5]
#
# Name output file, not clean, but okay for now
# 
# lun1 = open(OUTFILEBASE+'.out', 'w')
OUTFILEBASE += '_PPS.out'
# sys.stdout = open(OUTFILEBASE, 'w')
#
# Write current date and time to output file
#
now = time.strftime("%c")
print '-- Processing: ', PCAPNAME
print '-- Date      : ' + time.strftime("%c")
#
##################################################################
#
# Create Area Server Endpoint Info
#  
myAreaServerEndpointsInfo = []
myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_SHARD", 0x73B0, "199.108.200.65", 10070))
myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_WORLD40", 0xE856, "199.108.10.40", 10070))
# myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_WORLD41", 0x0000, "199.108.10.41",  10070))
# myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_WORLD42", 0x0000, "199.108.10.42",  10070))
# myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_WORLD43", 0x0000, "199.108.10.43",  10070))
# myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_WORLD44", 0x0000, "199.108.10.44",  10070))
# myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_WORLD45", 0x0000, "199.108.10.45",  10070))
# myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_WORLD46", 0x0000, "199.108.10.46",  10070))
# myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_WORLD47", 0x0000, "199.108.10.47",  10070))
myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_WORLD48", 0x1F0A, "199.108.10.48", 10070))
# myAreaServerEndpointsInfo.append(eqoa_endpoints.EndpointInfo("CLW_WORLD49", 0x0000, "199.108.10.49",  10070))
#
# Create Area Server Object, Queues and Area Server Process Object
# 
myQueues = []
myAreaServers = []
myAreaServerProcesses = []
#
for t in range(0, len(myAreaServerEndpointsInfo)):
    myAreaServers.append(eqoa_endpoints.AreaServer(myAreaServerEndpointsInfo[t]))
    myQueues.append(Queue())
#
# create and start the Area Servers
#
for i, thisAreaServer in enumerate(myAreaServers):
    myAreaServerProcesses.append(Process(target=thisAreaServer.startAreaServer, args=(myQueues[i],)).start())
#
#
# Now just sort packets to the correct Area Server
# 
pcapture = rdpcap(PCAPNAME, PCAPCOUNT)  # reads the first 'count' packets
#  

for index, pkt in enumerate(pcapture):  # Loop over packets in pcap
    if index > PCAPUDP1ST:
        print 'Processing Packet #: ', index
        try:
            if pkt.haslayer(IP):  # only look at IP packets   TCP and UDP
                for num, ep in enumerate(myAreaServers):
                    if (pkt.getlayer(IP).dst == ep.myEndpointInfo.IPaddress):  #
                        print 'Match'
                        if pkt.haslayer(Raw):  # If UDP packet, it will have Raw (data) layer
                            myQueues[num].put([index, pkt.load, pkt.getlayer(IP).src, pkt.getlayer(UDP).sport])
                            # myQueues[num].put([index,pkt.load])
        except:
            for process in myAreaServerProcesses:
                if process:
                    process.join()
            raise
print

print 'Sending Terminators .....'
for q in myQueues:
    q.put([0xBEEF, 0xBEEF])
#
# Clean up any loose threads
#
for process in myAreaServerProcesses:
    if process:
        process.join()
