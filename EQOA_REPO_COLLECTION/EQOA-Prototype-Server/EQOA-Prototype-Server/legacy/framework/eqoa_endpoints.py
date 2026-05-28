"""
Created on May 1, 2016

@author: Ben Turi
"""

from scapy.all import *
import eqoa_packets
import eqoa_bundles
import eqoa_sessions

import sys
import struct


#
###############################################################################
#     
class EndpointInfo:
    # EndpointInfo Object is fairly basic. Will be used to keep track of a Servers own EndpointInformation
    # Each Server will contain a list of EndpointInfo associated with each attached client
    #
    # myName is a human readable name
    # myEndpointID is  my endpoint ID
    # myIPaddress  is my IP address
    # myPort is the port the ServerEndpoint listens on
    #
    # Endpoints associated with Area Servers are created when they are init'd
    # Client Endpoints are created when the client first attaches to the Area Server.
    #
    def __init__(self, myName, myEndpointID, myIPaddress, myPort):
        self.IPaddress = myIPaddress
        self.Port = myPort
        self.EndpointID = myEndpointID
        self.Name = myName

    def printep(self):
        epout = 'Name: ' + self.Name + '  ID: ' + '0x{:4X}'.format(
            self.EndpointID) + '  IP: ' + self.IPaddress + '  Port: ' + '{:5d}'.format(self.Port)

        return epout


#
###############################################################################
#     
class AreaServer:
    # An Area Server Object. The clients connect directly with one of these servers.
    #
    # A client first contacts an Area Server with an init packet that causes the area
    # server to save an new Endpoint entity and Session entity
    #
    # There are three primary functions of the Area Server:
    #
    # 1) Listen on a specified socket/port for packets destined for this area server
    #
    # 2) Process the incoming list of messages and take action on them
    #
    # 3) Take fully formed session packets and send them back out to the clients
    # This object would be one of the few that knows the clients IP and PORT and the only one
    # that actually communicates to the outside world.
    #
    # myEndpoint is  an endpoint object
    #
    def __init__(self, myEndpointInfo):
        self.myEndpointInfo = myEndpointInfo
        #
        self.connectedEndpointIDs = set()
        self.connectedEndpoints = []

        self.mySessions = []
        self.mySlaveSessions = []
        self.myMasterSessions = []
        self.myTransferOutSessions = []
        self.myTransferInSessions = []

        self.myPacketList = []

    def printConnectedEndpoints(self):
        print
        print 'Total client Endpoints on this Area Server: ', len(self.connectedEndpoints)
        print '-------------------------------------------------------------'
        for i in self.connectedEndpoints:
            print i.printep()
    #
    #
    def printConnectedSessions(self):
        print

        print 'Total Sessions on this Area Server (' + '0x{:4X}'.format(self.myEndpointInfo.EndpointID) + ') : ' + '{:}'.format(len(self.mySessions))
        print '-------------------------------------------------------------'
        for j,i in enumerate(self.mySessions):
          print j, i.printSession()
        print 
        
    def startAreaServer(self, my_tQueue):
        #
        inn = "SERVER_" + self.myEndpointInfo.Name + ".txt"
        sys.stdout = open(inn, 'w')
        #
        print "Starting Area Server : " + self.myEndpointInfo.Name
        print
        #
        while 1 == 1:
            data = my_tQueue.get()
            packet_num = data[0]
            if packet_num != 0xBEEF:
                #
                payload = data[1]
                ip = data[2]
                port = data[3]
                #
                print 'Packet Number         : ', packet_num
                # print 'Length of payload     : ',len(payload)
                # print 'Source Port           : ','{:5d}'.format(port)
                # print 'Source IP             : ', ip
                #
                # Determine Packet Type - Read First four bytes
                #
                s = struct.unpack('<HH', payload[0:4])
                packet_source = s[0]
                packet_destin = s[1]
                #
                #
                if packet_source == eqoa_packets.PacketTypes.TRANSFER:
                    #
                    # Need to unencode transfer packet
                    # Make sure we have incoming transfer in incoming session/message queue
                    # if not, ,keep checking. if so, place it into normal sessions and then send ACK
                    # and also send ACK to old area_server it it can remove it from list
                    # might not make a list, might just flag as incoming
                    #
                    print 'PACKET TYPE           : TRANSFER (' + '0x{:4X}'.format(packet_source) + ')'

                elif (packet_destin == eqoa_packets.PacketTypes.FIRST_CONTACT) | (
                    packet_destin == self.myEndpointInfo.EndpointID):
                    #
                    if packet_destin == eqoa_packets.PacketTypes.FIRST_CONTACT:
                        #
                        print 'PACKET TYPE           : FIRST CONTACT (' + '0x{:4X}'.format(packet_destin) + ')'
                        print
                        #
                        # Create EndpointInfo object for remote endpoint and save to area server endpoint list
                        #
                        ept = EndpointInfo(str(packet_source), packet_source, ip, port)
                        self.connectedEndpoints.append(ept)
                        self.connectedEndpointIDs.add(ept.EndpointID)
                        print ' >> New EndpointInfo Object Created'
                        self.printConnectedEndpoints()
                    #
                    # Create Standard EQOA Packet Object
                    #
                    myStandardEQOAPacketObject = eqoa_packets.StandardEQOAPacket()
                    myStandardEQOAPacketObject.decodeEQOAPacket(payload)
                    myStandardEQOAPacketObject.printme()
                    #
                    # Check CRC and that remote endpoint is already connected or print error
                    #
                    if myStandardEQOAPacketObject.crcPassed == 1:
                        print 'PACKET CRC TEST    : PASSED'
                        #
                        if myStandardEQOAPacketObject.remoteEndpointID in self.connectedEndpointIDs:
                            print 'ENDPOINT ON SERVER : TRUE'
                            #
                            # extract bundles in payload using bundle headers
                            #
                            bundlepayload = payload[4:len(payload) - 4]
                            myBundleHeader = eqoa_bundles.BundleHeader()
                            myBundleHeader.decodeBundleHeader(bundlepayload)
                            #
                            # Now take Session action if we need
                            #
                            if myBundleHeader.fromMaster:  # will have a Session Action byte
                                #
                                if myBundleHeader.SessionAction == eqoa_sessions.SessionAction.NEW:
                                    # might need to pass back to areaServer
                                    
                                    print ' >> New Session Object Created'
                                    #                       
                                    masterEndpointInfo   = self.myEndpointInfo # just for now 0x00  # decide if I want, need this here
                                    slaveEndpointInfo    = self.myEndpointInfo
                                    sessionID_A          = myBundleHeader.SessionID_A
                                    sessionID_B          = myBundleHeader.SessionID_B                              
                                    #
                                    mySession = eqoa_sessions.Session()                                    
                                    mySession.buildSession(masterEndpointInfo, slaveEndpointInfo, sessionID_A, sessionID_B)
                                    print mySession.printSession()
                                    
                                    
                                    self.mySessions.append(mySession)
                                    self.printConnectedSessions()
                                    
                                    #
                                    # create new session, maybe pass the payload in to the session
                                    # Deliver payload to Correct Session

                                elif myBundleHeader.SessionAction == eqoa_sessions.SessionAction.CLOSE:
                                    #
                                    print 'Closing Existing Session'
                                    #
                                    # master saying to close session, so do this and send reply ack
                                    #
                                    # Need to remove client endpoint if this is the last session on this endpoint
                                    #
                                    # Delete object for remote endpoint and remove from area server endpoint list
                                    #
                                    for cep in self.connectedEndpoints:
                                        if cep.EndpointID == packet_source:  # need to delete endpoint in my endpoints and in set
                                            self.connectedEndpointIDs.remove(cep.EndpointID)
                                            self.connectedEndpoints.remove(cep)
                                            print ' >> EndpointInfo Object Destroyed'

                                            self.printConnectedEndpoints()
                                        #
                                elif myBundleHeader.SessionAction == eqoa_sessions.SessionAction.CONTINUING:
                                    #
                                    print 'Continuing Existing Session'
                                    # Master says session already in session, continue
                                    # Deliver payload to Correct Session
                                else:
                                    print
                                    print 'Unknown Session Action'

                                    # Deliver payload to Correct Session

                            else:  # Bundle is from Slave and I just need to check SessionIDs and deliver payload
                                pass


                                #
                        else:  # probably log error and drop packet
                            print 'ENDPOINT ON SERVER : FALSE'
                        print
                        #
                    else:  # probably log error and drop packet
                        print 'PACKET CRC TEST    : FAILED'
                        #

                        #
                        #
                        # Process Bundle Headers in Payload - deliver to session or create session, or end session
                        #


                else:  # UNKNOWN packet
                    print ' PACKET TYPE           : UNKNOWN'
                    # probably log packet and print error

                print hexdump(payload)
                print '----------------------------------------------------------------------------------'
                print

            else:
                print 'Got Terminator....'
                print '0x{:4X}'.format(packet_num)
                print
                print '  Connected Endpoints: '
                print '  ---------------------'
                for n, ep in enumerate(self.connectedEndpoints):
                    print '  Connected Endpoint ' + str(n) + ' --> ' + ep.printep()
                print
                break

#
###############################################################################
