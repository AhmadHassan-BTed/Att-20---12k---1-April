'''

created december 24th, 2016
@author: Devin Dallas
'''

import eqoa_utilities

CLASS = eqoa_utilities.enum( WAR = 0,
                             RAN = 1,
                             PAL = 2,
                             SK  = 3,
                             MNK = 4,
                             BRD = 5,
                             ROG = 6,
                             DRD = 7,
                             SHA = 8,
                             CL  = 9,
                             MAG = 10,
                             ENC = 11,
                             WIZ = 12,
                             ALC = 13)

RACE = eqoa_utilities.enum( HUM  = 0,
                            ELF  = 1,
                            DELF = 2,
                            GNO  = 3,
                            DWF  = 4,
                            TRL  = 5,
                            BARB = 6,
                            HALF = 7,
                            ERU  = 8,
                            OGR  = 9)

class dumpStart():

    def __init__(self, worldname, serverid, name, Class, race, level, xp, debt,
                 breath, tunar, banktunar, trainpoints, basemaxstat, world, Y, Z, X, facing):
        
        self.worldname   = worldname
        self.serverid    = serverid
        self.name        = name
        self.Class       = Class
        self.race        = race
        self.level       = level
        self.xp          = xp
        self.debt        = debt
        self.breath      = breath
        self.tunar       = tunar
        self.banktunar   = banktunar
        self.trainpoints = trainpoints
        self.basemaxstat = basemaxstat
        self.world       = world
        self.Y           = Y
        self.Z           = Z
        self.X           = X
        self.facing      = facing
    
    
    def encodeddumpstart(self):
    
        #Adds world name
        packingList = [self.worldname]       
      
        encodedDumpString = eqoa_utilities.packStringAsASCII(packingList)
        
        # Adds server and client id
        packingList = [self.serverid]
                       
        encodedDumpString += eqoa_utilities.packVariable(packingList)
        
        # Adds player name to stream
        packingList = [self.name]
        
        encodedDumpString += eqoa_utilities.packStringAsASCII(packingList)
        
        # Adds Class, race, level, xp, debt, breath, tunar, banktunar, trainpoints, basemaxstat and world
        
        packingList = [self.Class,
                       self.race,
                       self.level,
                       self.xp,
                       self.debt]
                       
        encodedDumpString += eqoa_utilities.packVariable(packingList)
               
                       
        packingList = [];
        encode_fmt = '<B'; packingList.append(self.breath)          # single byte ranging from 0 to 255 for breath

        encodedDumpString += eqoa_utilities.packFixed(encode_fmt,packingList)
               
         
        packingList = [self.tunar,
                       self.banktunar,
                       self.trainpoints,
                       self.basemaxstat,
                       self.world]
        
        encodedDumpString += eqoa_utilities.packVariable(packingList)
        
        # converts World coordinates from float to hex
        
#
        packingList = [];
        encode_fmt = '<f'; packingList.append(self.Y)          # Toon Y value 4 byte float
        encode_fmt += 'f'; packingList.append(self.Z)          # Toon Z value 4 byte float
        encode_fmt += 'f'; packingList.append(self.X)          # Toon X value 4 byte float
        encode_fmt += 'f'; packingList.append(self.facing)     # Toon Facing Angle (radians) 4 byte float N = 0.00
        #
        encodedDumpString += eqoa_utilities.packFixed(encode_fmt,packingList)
      
        return encodedDumpString;
        
                       
    def printdumpstart(self):
    
        print
        print ' WORLDNAME       :    {:s}'.format(self.worldname)
        print ' SERVERID        :    {:8x}'.format(self.serverid)
        print ' NAME            :    {:s}'.format(self.name)
        print ' CLASS           :    {:8x}'.format(self.Class)
        print ' RACE            :    {:8x}'.format(self.race)
        print ' LEVEL           :    {:8x}'.format(self.level)
        print ' XP              :    {:8x}'.format(self.xp)
        print ' DEBT            :    {:8x}'.format(self.debt)
        print ' BREATH          :    {:8x}'.format(self.breath)
        print ' TUNAR           :    {:8x}'.format(self.tunar)
        print ' BANKTUNAR       :    {:8x}'.format(self.banktunar)
        print ' TRAINPOINTS     :    {:8x}'.format(self.trainpoints)
        print ' BASEMAXSTAT     :    {:8x}'.format(self.basemaxstat)
        print ' WORLD           :    {:8x}'.format(self.world)
        print ' Y               :    {:8f}'.format(self.Y)
        print ' Z               :    {:8f}'.format(self.Z)
        print ' X               :    {:8f}'.format(self.X)
        print ' FACING          :    {:8f}'.format(self.facing)
        print