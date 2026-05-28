#!/usr/bin/env python
#
# Devin and Ben 
# December 15, 2018 
#
import logging, sys
sys.path.insert(0, './ProcessCoreIO')
#
from commManager import endpointInfo
from rdpServer import * 
##
##  Start up Logging
##
LOG_FORMAT = '%(asctime)-15s | %(name)-12s | %(message)s'
logging.basicConfig(filename='eqoa_rdpServer_2.log',level=logging.INFO,format=LOG_FORMAT)
log = logging.getLogger('EQOA_EMUv4')
log.setLevel(logging.INFO)
##
log.info("")
log.info("")
log.info("----------------------------------------------------------------------------------")
log.info("")
log.info("Starting EQOA EMU v4...")
log.info("")
#
# eventually read DB or similiar to get AreaServer info. Just specify for now.
# probably use some module that makes it easy to represent IP address
#
myName         = 'CLW2'
#IP address for server here
myIPaddress    = ''
myPortList     = [10071] 
myEndpointID   = 60124 
print('Starting {}; End point {} with an IP of {} and Port(\'s) of {}'.format(myName,hex(myEndpointID).upper(), myIPaddress, myPortList))
myEndpointInfo = endpointInfo(myName, myEndpointID, myIPaddress, myPortList)
#
# Init and start up the Servers
#

myAreaServer = rdpServer(myEndpointInfo).start(myPortList)

#    # An Area Server Object. The clients connect directly with one of these servers.
#    #
#    # A client first contacts an Area Server with an init packet that causes the area
#    # server to save an new Endpoint entity and Session entity
#    #
#    # There are three primary functions of the Area Server:
#    #
#    # 1) Listen on a specified socket/port for packets destined for this area server
#    #
#    # 2) Process the incoming list of messages and take action on them
#    #
#    # 3) Take fully formed session packets and send them back out to the clients
#    # This object would be one of the few that knows the clients IP and PORT and the only one
#    # that actually communicates to the outside world.
#    #
#    # myEndpoint is  an endpoint object

#
log.info("")
log.info("Stopping EQOA EMU v4...")
log.info("")
log.info("----------------------------------------------------------------------------------")

