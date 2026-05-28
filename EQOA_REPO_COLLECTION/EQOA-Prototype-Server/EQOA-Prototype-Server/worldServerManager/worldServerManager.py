#!/usr/bin/env python
# 
# Devin and Ben   
# January 21, 2019 
# 
import logging, asyncio, sys, queue, struct
import time, uvloop, socket, configparser


configfile_name = "../config/eqoa_revival_config.ini"
config = configparser.ConfigParser()
config.read(configfile_name)
##
##
##  Start up Logging
##
LOG_FORMAT = '%(asctime)-15s | %(name)-12s | %(message)s'
logging.basicConfig(filename='eqoa_worldServerManager.log',level=logging.INFO,format=LOG_FORMAT)
log = logging.getLogger('world_Server_Manager')
log.setLevel(logging.INFO)
##
log.info("")
log.info("")
log.info("")
log.info("")
log.info("----------------------------------------------------------------------------------")
log.info("")
log.info("Starting World Server Manager...")
log.info("")

#This servers IP/Port
SERVER_PORT = int(config.get('worldManager','server_port')) 
SERVER_IP   = config.get('worldManager','server_ip')
SERVER_COUNT= int(config.get('worldManager','server_total'))
SERVER_CMD  = config.get('worldManager', 'server_cmd')
#
#
# eventually read DB or similiar to get World Servers info. Just specify for now.
# probably use some module that makes it easy to represent IP address
#

class worldServers():
    
    def __init__(self): 
        self.worlds               = []
        self.worldServer_count    = 0
        self.command              = None #Command to be SSH'd in to start serverworlds.
        
    def retrieveWorlds(self):
        #Do stuff to retrieve worlds from database
        #Static for now
        self.command = SERVER_CMD
        self.worldServer_count = SERVER_COUNT
        for i in range(self.worldServer_count):
            i = i + 1
            SERVER_NAME = config.get('worldServer{}'.format(i), 'server_name')
            SERVER_IP   = config.get('worldServer{}'.format(i), 'server_ip')
            SERVER_PORT = int(config.get('worldServer{}'.format(i), 'server_port'))
            SERVER_DEF  = config.get('worldServer{}'.format(i), 'default_server')
            SERVER_REC  = config.get('worldServer{}'.format(i), 'recommended')
            SERVER_ENDP = int(config.get('worldServer{}'.format(i), 'endpoint'))
            #This is more of a test variable here and in config, can be removed later
            SERVER_DISC = config.get('worldServer{}'.format(i), 'disconnected')
            log.info('Retrieved {}'.format(SERVER_NAME))
            self.worlds.append([SERVER_NAME, SERVER_IP, SERVER_PORT, time.time(), SERVER_DEF, SERVER_REC, SERVER_ENDP, SERVER_DISC])    
         
async def worldSSH(world, host, command):
    pass
    '''
    async with asyncssh.connect(host, port = 22, username = 'pi', password = '*', known_hosts = None) as conn:
        log.info('Starting {}'.format(world))
        await conn.create_process(command)
    '''
    
class worldManager(asyncio.Protocol):
    
    def __init__(self, worldMonitor):
        self.worldMonitor = worldMonitor
        self.log = log
        
        
    def connection_made(self, transport):
        self.peername = transport.get_extra_info('peername')
        self.peername = self.peername[0]
        self.transport = transport
        self.log.info('Connection received')

    def data_received(self, data):
        msg = data.decode()
        
        if msg == 'Alive':
            queue.put_nowait(self.peername)
            self.log.info('Returning answer to world')
            self.transport.write(b'copy')
            
        elif msg == 'Servers please':
            self.log.info('Request for worlds received')
            worldList = self.collectWorlds()
            #print(worldList)
            self.log.info(worldList)
            self.transport.write(worldList)
            
        else:
            self.log.info('What does this user want? {}'.format(self.peername))

            
    def collectWorlds(self):
        self.log.info('Collecting worlds')
        totWorlds = bytearray()
        worldLen = 0
        myWorlds = bytearray()
        for world in self.worldMonitor.worlds:
            thisWorld = []
            if world[7] == 'yes':
                #Length of servername
                thisWorld.append(len(world[0]))
                #Servername with unicode
                thisWorld.append(world[0].encode('utf-16-le'))
                #recommend
                if world[5] == 'yes':
                  thisWorld.append(1)
                else:
                  thisWorld.append(0)
                #World server Endpoint  
                thisWorld.append(65535)
                #WorldServer Port
                thisWorld.append(65535)
                #WorldServer IP Address
                thisWorld.append('255.255.255.255')
                #static Language
                thisWorld.append(0)
                #Packs world length to Port
                theWorld = struct.pack('<L', thisWorld[0]) + thisWorld[1] + struct.pack('<BHH', thisWorld[2], thisWorld[3], thisWorld[4])
                #This packs IP address and appends
                theWorld += bytes(map(int, thisWorld[5].split('.')))[::-1]
                #This packs and appends Language
                theWorld += struct.pack('<B', thisWorld[6])
                #Puts this World into worldList
                myWorlds.extend(theWorld)
                #Our total length
                worldLen += 10 + len(world[0])
            
            else:
                #Length of servername
                thisWorld.append(len(world[0]))
                #Servername with unicode
                thisWorld.append(world[0].encode('utf-16-le'))
                #recommend
                if world[5] == 'yes':
                  thisWorld.append(1)
                else:
                  thisWorld.append(0)
                #World server Endpoint  

                thisWorld.append(world[6])
                #WorldServer Port
                thisWorld.append(world[2])
                #static Language
                thisWorld.append(0)
                #Packs world length to Port
                theWorld = struct.pack('<L', thisWorld[0]) + thisWorld[1] + struct.pack('<BHH', thisWorld[2], thisWorld[3], thisWorld[4])
                #This packs IP address and appends
                theWorld += bytes(map(int, world[1].split('.')))[::-1]
                #This packs and appends Language
                theWorld += struct.pack('<B', thisWorld[5])
                #Puts this World into worldList
                myWorlds.extend(theWorld)
                #Our total length
                worldLen += 10 + len(world[0])  
                self.log.info(thisWorld) 

        totWorlds.extend(struct.pack('B', SERVER_COUNT*2))
        totWorlds.extend(myWorlds)
        #print(totWorlds)
        print('World request sent')
        self.log.info('Sending worlds off')      
        return totWorlds            
                
            
        
class worldMonitor:
    
    def __init__(self):
        self.worlds   = None
        self.total    = None
        self.cmd      = None
        self.queue    = None
        self.peername = None
        self.log      = logging.getLogger('world_Monitor       ')
        
    async def monitorWorlds(self, world):
        self.worlds = world.worlds
        self.count  = world.worldServer_count
        self.cmd    = world.command
        self.queue  = queue
        
        while True:
            
            #Only 1 world 
            await asyncio.sleep(2)
            if self.queue.empty() == False:
                self.peername = await self.queue.get()                 
             
            if self.count == 1:
                for c, world in enumerate(self.worlds):
                    
                    #Received a response from the world and can update response time
                    if world[1] == self.peername:
                        self.worlds[c][3] = time.time()
                        self.log.info('Received heartbeat from {}'.format(self.worlds[c][0]))
                        self.peername = None
                        
                    #Means world may be down, let's attempt to revive it    
                    elif (time.time() - world[3]) > 60:
                        #Do this so it doesn't spam the world every True count on if
                        #Essentially will check in with world every 60 seconds with ssh to restart
                        self.worlds[c][3] = time.time()
                        self.log.info('Haven\'t heard from {} in over 60 seconds'.format(world[0]))
                        #await asyncio.ensure_future(worldSSH(world[0], world[1], self.cmd))
                        
                    else:
                        #Technically means world is responding or < 60 seconds
                        pass
                    
            #Multiple worlds            
            else:
                for c, world in enumerate(self.worlds):
                        
                    if world[1] == self.peername:
                        self.worlds[c][3] = time.time()
                        self.log.info('Received connection from {}'.format(self.worlds[c][0]))
                        self.peername = None
                            
                    elif (time.time() - world[3]) > 60:
                        #Do this so it doesn't spam the world every True count on if
                        #Essentially will check in with world every 60 seconds with ssh to restart
                        self.worlds[c][3] = time.time()
                        self.log.info('Haven\'t heard from {} in over 60 seconds'.format(world[0]))
                        #await asyncio.ensure_future(worldSSH(world[0], world[1], self.cmd))
                    
                    else:
                        #Technically means world is responding or < 60 seconds
                        pass 
    
#####################################################################            
#
#            MAIN LOOP
# 
#asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())
#asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())
loop = asyncio.get_event_loop()
#Queue to pass data between these two event's
queue = asyncio.Queue()
log.info('Collecting servers')

print('Listening for world requests...')
#Gathers our worlds
worldserver = worldServers()
worldserver.retrieveWorlds()
#Initializes the loop for monitoring world heartbeats
worldMonitors = worldMonitor()

log.info('Listening for world heartbeat\'s')
coro = loop.create_server(lambda: worldManager(worldMonitors), SERVER_IP, SERVER_PORT)
server = loop.run_until_complete(coro)    

log.info('Starting to Monitor World Heartbeats')
loop.run_until_complete(worldMonitors.monitorWorlds(worldserver))



try:        
    
    loop.run_forever()
finally:
    log.info('closing loop')
    loop.close()
    

