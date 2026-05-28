'''
Created July 20, 2017

@author: Benturi
'''
import eqoa_utilities
import struct

from datetime import datetime

           
def dnp3_to_datetime(octets):
    milliseconds = 0
    for i, value in enumerate(octets):
        milliseconds = milliseconds | (ord(value) << (i*8))
    date = datetime.utcfromtimestamp(milliseconds/1000.)
    return date.strftime('%b %d, %Y %H:%M:%S.%f UTC')          

class player():
    #Basic player information
    def __init__(self, name, password, playerid, result, acc_stat, subtime, partime, unknown1_8, unknown2_8, pad_48, unknown3_4, unknown4_4):
        #
        self.name       = name
        self.password   = password
        self.playerid   = playerid
        self.result     = result
        self.acc_stat   = acc_stat
        self.subtime    = subtime
        self.partime    = partime
        self.unknown1_8 = unknown1_8
        self.unknown2_8 = unknown2_8
        self.pad_48     = pad_48
        self.unknown3_4 = unknown3_4
        self.unknown4_4 = unknown4_4
        
    def printPlayer(self):
        #
        print ' PLAYER QUANTITIES'
        print ' -----------------------'
        print ' NAME       : {:32s}'.format(self.name)
        print ' PWD        : {:32X}'.format(self.password)
        print ' ID         : {:10d}'.format(self.playerid)
        print ' RESULT     : {:8d}'.format(self.result)
        print ' STATUS     : {:8d}'.format(self.acc_stat)
        print ' SUBTIME    : {:8d}'.format(self.subtime)        
        print ' PARTIME    : {:8d}'.format(self.partime)
        print ' UNKNOWN1_8 : 0x{:08x}'.format(self.unknown1_8)
        print ' UNKNOWN2_8 : 0x{:08x}'.format(self.unknown2_8)
        print ' PAD_48     : 0x{:048x}'.format(self.pad_48)        
        print ' UNKNOWN3_4 : 0x{:04x}'.format(self.unknown3_4)
        print ' UNKNOWN4_4 : 0x{:04x}'.format(self.unknown4_4)
        print

# save to database - playerdb
# read from database
# query from database

#
# once you have playerID, you can query world servers for associated toons.
#        
        
        
playerList = []
playerList.append(player(        'ddrlear2007',  # name
                 0x98503B30A41A438CB7E945682991D3C2FE3EEE6251919A5E95E0AB7ED546D69E, # password (encrypted)
                                    0x26C15D5A,  # playerid
                                             0,  # result
                                             3,  # acc_stat 
                                    0x001E7F4F,  # subtime
                                    0x001E7F4F,  # partime
                            0x716C443236636878,  # unknown1_8
                            0x726B53314A746776,  # unknown2_8
                                    0x00000000,  # pad_48       
                                             1,  # unknown3_4
                                             3)  # unknown4_4
                )


         
              