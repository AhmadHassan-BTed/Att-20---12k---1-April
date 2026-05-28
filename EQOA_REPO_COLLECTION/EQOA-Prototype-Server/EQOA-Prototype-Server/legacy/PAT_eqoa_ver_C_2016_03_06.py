#!/usr/bin/python -u
#
# Benturi
# December 2, 2015
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
# Load up specific info on each of the pcaps
# [PCAPNAME, CLIENTIP, PCAPCOUNT, PCAPUDP1ST, PCAPALL, OUTFILEBASE]
#
pcap_info=[]
pcap_info.append([0,0,0,0,0,0])
pcap_info.append(["MATT_ORIG_pkt_log.pcap","192.168.1.109",1300,150,0,'Dudderz'])
pcap_info.append(["../PCAPS/Fixed_dump_with_username_removed.pcap","192.168.0.87",300,150,0,'Famorf'])
pcap_info.append(["../PCAPS/Trif_with_username_removed.pcap","192.168.0.5",1300,0,0,'Trifixion'])
pcap_info.append(["../PCAPS/dread.pcap","192.168.0.69",300,0,0,'Dread']) #4
pcap_info.append(["../PCAPS/DreadSolo.pcap","192.168.0.5",300,0,0,'DreadSolo'])
pcap_info.append(["../PCAPS/fixed_Mexico_pkt_log.pcap","192.168.1.74",785,345,0,'Mexico']) # 4
#
# Read which pcap from the command line
#
jp = int(sys.argv[1])
#
# Load into readable variable names
#
PCAPNAME    = pcap_info[jp][0]
CLIENTIP    = pcap_info[jp][1]
PCAPCOUNT   = pcap_info[jp][2]
PCAPUDP1ST  = pcap_info[jp][3]
PCAPALL     = pcap_info[jp][4]
OUTFILEBASE = pcap_info[jp][5]  
#
# Name output file, not clean, but okay for now
# 
#lun1 = open(OUTFILEBASE+'.out', 'w')
sys.stdout = open(OUTFILEBASE+'.out', 'w')
#
# Write current date and time to output file
#
now = time.strftime("%c")
print '-- Processing: ',PCAPNAME     
print '-- Date      : ' + time.strftime("%c")
#

##############################################################
#
# Define Machine Class
#
class Machine:
   'Common base class for all Machines'
   macCount = 0

   def __init__(self, name, ip):
      self.name = name
      self.ip   = ip
      Machine.macCount += 1
   
   def displayCount(self):
     print "Total Machines %d" % Machine.macCount

   def displayMachine(self):
      print "Name : ", self.name,  ", IP Address: ", self.ip

#
# Set up easy to read names and IP address
#      
machine=[]
machine.append(Machine("UNKNOWN      ","0.0.0.0"))
machine.append(Machine("PS2          ",CLIENTIP))
machine.append(Machine("DNS          ","192.168.1.1"))
machine.append(Machine("DNAS         ","203.105.78.163"))
machine.append(Machine("PATCH        ","64.37.156.30"))
machine.append(Machine("LOGIN        ","64.37.156.200"))
machine.append(Machine("CLW - SHARD  ","199.108.200.65"))
machine.append(Machine("CLW - WORLD40","199.108.10.40"))
machine.append(Machine("CLW - WORLD41","199.108.10.41"))
machine.append(Machine("CLW - WORLD42","199.108.10.42"))
machine.append(Machine("CLW - WORLD43","199.108.10.43"))
machine.append(Machine("CLW - WORLD47","199.108.10.47"))
machine.append(Machine("CLW - WORLD48","199.108.10.48"))

print
print " --- Known Machines in Capture ---"
for j in range(Machine.macCount):
  machine[j].displayMachine()
print
print '==================================================================='     
#
#################################################################
#
def Check_CRC(packet):
  payload = packet.load
#
# Determined by Ben Turi
#
#  crc = Crc(width = 32, poly = 0x04c11db7 ,
#          reflect_in = True, xor_in = 0xffffffff,
#          reflect_out = True, xor_out = 0x11f19ed3)
#
  s = struct.unpack('<I',payload[len(payload)-4:len(payload)]) #  
  received_crc = s[0]
#  
# calculated_crc = crc.bit_by_bit_fast(payload[0:len(payload)-4])      # calculate the CRC, using the bit-by-bit-fast algorithm.
  
  calculated_crc = ((binascii.crc32(payload[0:len(payload)-4])^0xFFFFFFFF)^0x11f19ed3)&0xFFFFFFFF

  print '  CALCULATED CRC : ','0x{:08X}'.format(calculated_crc)
  print '  RECEIVED   CRC : ','0x{:08X}'.format(received_crc)
#
  answer = -1
  if calculated_crc == received_crc:
    print '  CRC STATUS     :  PASSED'
    answer = 0
  else:
    print '  CRC STATUS     :  FAILED'
    print    
    print '  ** DROPPING PACKET **'      
#  
  return answer
#
################################################################  
#
def Print_Packet_Info(pkt,index,machine,macsrc,macdst):
 print
 print " Packet",index+1,": ",machine[macsrc].name, "  ----->  ",machine[macdst].name  
 print
 print hexdump(pkt.load)
 print
 print ' *** *** *** *** *** ***'
 return 
#
###########################################################
#
def Check_for_Transfer(packet):
#
  hdr_fmt1 = '<HHI'   # (little endian)[SRC ADDR][DST ADDR][possible transfer number]
  s = struct.unpack(hdr_fmt1,packet.load[0:8]) # using hdr_fmt1, unpack the first four bytes
#
  answer = 1            # Assume this is a transfer 
  if s[0] == 0xFFFF:    # Check to see if Transfer
    transfercode = s[1]     #  REQ or ACK
    transfernum  = s[2]     #  Overall transfer number
#   
    print
    print '  TRANSFER PACKET IDENTIFIED ','0x{:04X}'.format(0xFFFF)
    print '  ----------------------------------'
#
    if  transfercode == 0x0992:   # Transfer Request
      print '  TRANSFER OPCODE    :','0x{:04X}'.format(transfercode)
      print '  TRANSFER FUNCTION  : CLIENT REQUESTING TRANSFER TO NEW AREA SERVER'
      print '  TRANSFER NUMBER    :','0x{:04X}'.format(transfernum),'{0:05d}'.format(transfernum)
      print
#      
      s = struct.unpack('<HHBBBB',packet.load[8:16]) #  
      client_code      = s[0]
      new_server_code  = s[1]
      ip_address       = s[2:6]      
#
      print '  CLIENT ENDPOINT ID : ','0x{:04X}'.format(client_code),'{0:05d}'.format(client_code)
      print '  DUMMY PORT NUMBER  : ','0x{:04X}'.format(new_server_code),'{0:05d}'.format(new_server_code)
      print '  DUMMY IP ADDRESS   : ', "".join("{:s}.".format(str(y)) for y in reversed(ip_address)).rstrip('.') 
#                   
      Create_Endpoint(packet)   # Actually add endpoint for next time
      print
      print '  PACKET LENGTH      : ', '{0:#x}'.format(count(packet.load)) , '{0:0d}'.format(count(packet.load)),'bytes'
      print
# 
    elif transfercode == 0x0993:   # Transfer Acknowledgement  
      print '  TRANSFER OPCODE    :','0x{:04X}'.format(transfercode)
      print '  TRANSFER FUNCTION  : SERVER ACKNOWLEDGING TRANSFER IS GOOD TO CLIENT'
      print '  TRANSFER NUMBER    :','0x{:04X}'.format(transfernum),'{0:05d}'.format(transfernum)
      print
      print '  PACKET LENGTH      : ', '{0:#x}'.format(count(packet.load)) , '{0:0d}'.format(count(packet.load)),'bytes'
      print    
    else:
      print 'UNKNOWN TRANSFER CODE'
      quit()
#    answer = 0  
  else:
    answer = 0 # not a transfer, normal packet  
#
  return answer
    
#
def Process_Payload_Header(packet):
#
  hdr_fmt1 = '<HHI'   # (little endian)[SRC ADDR][DST ADDR][possible transfer number]
  s = struct.unpack(hdr_fmt1,packet.load[0:8]) # using hdr_fmt1, unpack the first four bytes
#
  myEndpointID = 0x73B0
  answer = 1            # Assume we will return correctly 
#
#  Wildcard Packet Code
#  
  if s[0] == 0xFFFE:  # Wildcard Destination
    Create_Endpoint(packet)
#
  elif s[1] == myEndpointID:  # This is destined for me
     answer = 1
  else:
     answer = -1  # not for this server
#
  if Endpoint_on_Server(s[0])==1:  # reorder, this could change things
     answer = 1  
     print
     print '  LOCAL  ADDR    : ', '{:4X}'.format(s[0]) , '{0:08d}'.format(s[0])
     print '  REMOTE ADDR    : ', '{:4X}'.format(s[1]) , '{0:08d}'.format(s[1])
     print
#      
#   
  else:
   answer = -1  
#
  packetsize   = count(packet.load)
  print '  PACKET LENGTH  : ', '{0:#x}'.format(packetsize) , '{0:08d}'.format(packetsize),'bytes'
  print
#    
  return answer

  
  
##
##
def Endpoint_on_Server(packet):
  answer = 1
  return answer
##
##
def Create_Endpoint(packet):
  answer = 1
  return answer    
##
def Destroy_Session():
  return 
##
def hex2float(s):
    bins = ''.join(chr(int(s[x:x+2], 16)) for x in range(0, len(s), 2))
    return struct.unpack('>f', bins)[0]
##
def Create_Session():
  return 
##
def Process_Message_Report(bundle,b):
 hdr_fmt1 = '<HHH'   # (little endian)
 s = struct.unpack(hdr_fmt1,bundle[b:b+6]) # using hdr_fmt1, unpack the first
 this_bundle           = s[0] 
 last_bundle_received  = s[1]
 last_message_received = s[2]
#
 print
 print '  THIS BUNDLE           :','0x{0:02x}'.format(this_bundle),'{0:08d}'.format(this_bundle)
 print '  LAST BUNDLE  RECEIVED :','0x{0:02x}'.format(last_bundle_received),'{0:08d}'.format(last_bundle_received)
 print '  LAST MESSAGE RECEIVED :','0x{0:02x}'.format(last_message_received),'{0:08d}'.format(last_message_received)
 print
 # 
 return  
##
def Process_Session_Ack(bundle):
 hdr_fmt1 = '<I'   # (little endian)[Session Code]
 s = struct.unpack(hdr_fmt1,bundle[1:5]) #
 ack_session = s[0] 
#
 print
 print '  ACK SESSION    :','0x{:08X}'.format(ack_session),'{0:09d}'.format(ack_session)
 # 
 return  
#
##########################################################################
#
def Process_Messages(bundle,b):
#
# 
 message_iter = 0
 while count(bundle) - 4 - b > 0 & message_iter < 100:   
#
  message_iter = message_iter + 1
#  
  hdr_fmt1 = '<H'   # (little endian)[]
  s = struct.unpack(hdr_fmt1,bundle[b:b+2]) # don't advance b 
  if s[0] > 0xFF00:              #Long Message Type
#  
    hdr_fmt1 = '<HH'   # (little endian)[]
    s = struct.unpack(hdr_fmt1,bundle[b:b+4])
    b = b + 4
    message_type   = s[0]
    message_length = s[1]
#
    if message_type   == 0xFFFB:  # Long Standard Message
      print '    MESSAGE ITER :','{:04d}'.format(message_iter)
      print '    MESSAGE TYPE :','0x{:04X}'.format(message_type)
      print '    MESSAGE DESC : LONG STANDARD MESSAGE'
      print '    MESSAGE LENG :','0x{:04X}'.format(message_length),'{0:08d}'.format(message_length)
      print      
      Process_Standard_Message(bundle[b:b+message_length+2],message_length)
      b = b + message_length + 2
    elif message_type == 0xFFFC:  # Long System Message
      print '    MESSAGE ITER :','{:04d}'.format(message_iter)
      print '    MESSAGE TYPE :','0x{:04X}'.format(message_type)
      print '    MESSAGE DESC : LONG SYSTEM MESSAGE'       
      print '    MESSAGE LENG :','0x{:04X}'.format(message_length),'{0:08d}'.format(message_length)
      print
      Process_System_Message(bundle[b:b+message_length])
      b = b + message_length      
    elif message_type == 0xFFFA:  # Continued Message
      print '    MESSAGE ITER :','{:04d}'.format(message_iter)
      print '    MESSAGE TYPE :','0x{:04X}'.format(message_type)
      print '    MESSAGE DESC : CONTINUED MESSAGE'    
      print '    MESSAGE LENG :','0x{:04X}'.format(message_length),'{0:08d}'.format(message_length)
      print  
      Process_Continue_Message(bundle[b:b+message_length+2],message_length)
      b = b + message_length     
    else:
      print 'Unknown Long Message Type'
      
  else:  # Short message Type
# 
    hdr_fmt1 = '<BB'   # (little endian)[]
    s = struct.unpack(hdr_fmt1,bundle[b:b+2]) #  
    b = b + 2
    message_type   = s[0]
    message_length = s[1]
#    
    if message_type   == 0xFB:  #  Standard Message
      print '    MESSAGE ITER :','{:04d}'.format(message_iter)
      print '    MESSAGE TYPE :','0x{:04X}'.format(message_type)
      print '    MESSAGE DESC : STANDARD MESSAGE'
      print '    MESSAGE LENG :','0x{:04X}'.format(message_length),'{0:08d}'.format(message_length)
      #print
      Process_Standard_Message(bundle[b:b+message_length+2],message_length)    
      b = b + message_length+2      
    elif message_type == 0xFC:  #  System Message
      print '    MESSAGE ITER :','{:04d}'.format(message_iter)
      print '    MESSAGE TYPE :','0x{:04X}'.format(message_type)
      print '    MESSAGE DESC : SYSTEM MESSAGE'     
      print '    MESSAGE LENG :','0x{:04X}'.format(message_length),'{0:08d}'.format(message_length)
      #print     
      Process_System_Message(bundle[b:b+message_length])
      b = b + message_length
    else:
      print 'Unknown Message Type'
#
 print
 return
#
#######################################################################################################
#
def Process_OPCODES(message):

  global is_frontiers

  hdr_fmt1 = '<H'   # (little endian)[OPCODE]
  s = struct.unpack(hdr_fmt1,message[0:2]) #  
  my_opcode = s[0]
  print '    OPCODE       :','0x{:04X}'.format(my_opcode),'{0:08d}'.format(my_opcode)
#
# Dictionaries
#  
  armor_dict       = {0x00:'None  ',0x01:'Padded', 0x02:'Leather', 0x03:'Chain Mail',0x04:'Plate Mail',0x05:'Split Mail',0x06:'Banded Mail',0x07:'Scale Mail',0x08:'Monk Wraps'}
#
  animate_dict     = {0x0000:'Standing',0x0001:'1H Slash', 0x0002:'2H Slash', 0x0003:'1H Blunt',0x0004:'2H Blunt',0x0005:'1H Pierce',\
                      0x0006:'2H Pierce',0x0007:'Bow',0x0008:'Fist',0x0009:'Crossbow',0x000a:'Throw',0x000b:'Fist',0x000c:'Nothing',\
                      0x0501:'1H Pierce (Offhand)',0x0101:'1H Slash (Offhand)',0x0301:'1H Blunt (Offhand)'}                     
#
  robe_dict        = {0x00000000:'Arcane Robe',0x00000001:'Divine Robe',0x00000002:'Silk Robe',0x00000003:'Fur Robe',0xFFFFFFFF:'No Robe'}  
#
  class_dict       = {0x00:"Warrior",0x02:"Ranger",0x04:"Paladin",0x06:"Shadowknight",0x08:"Monk",0x0a:"Bard",0x0c:"Rogue",0x0e:"Druid",\
                      0x10:"Shaman",0x12:"Cleric",0x14:"Magician",0x16:"Necromancer",0x18:"Enchanter",0x1a:"Wizard",0x1c:"Alchemist"}
#
  race_dict        = {0x00:"Human",0x02:"Elf",0x04:"Dark Elf",0x06:"Gnome",0x08:"Dwarf",0x0a:"Troll",0x0c:"Barbarian",0x0e:"Halfling",0x10:"Erudite",0x12:"Ogre"}
#
  sprite_dict      = {0x0FEDC4F6D4:"Dark Elf - Male",0x04F2D4C08F:'Elf - Male',0x09C6FD94E4:'Troll - Male',0x0E8DC4E38C:'Human - Male',
                      0x0BC2A680BB:'Halfling - Male',0x0ADAA7EEF1:'Dwarf - Male',0x0C9CD1FBBE:'Barbarian - Male'} 
#  
  hair_color_dict  = {0x00:'Option 1', 0x02:'Option 2',0x04:'Option 3',0x06:'Option 4',0x08:'Option 5', 0x0a:'Option 6',0x0c:'Option 7',0x0e:'Option 8'}
  hair_length_dict = {0x00:'Option 1', 0x02:'Option 2',0x04:'Option 3',0x06:'Option 4'}
  hair_style_dict  = {0x00:'Option 1', 0x02:'Option 2',0x04:'Option 3',0x06:'Option 4'}
  face_option_dict = {0x00:'Option 1', 0x02:'Option 2',0x04:'Option 3',0x06:'Option 4'}
#
#
  equipped_dict   = {0x00:'HELM',0x01:'NOT EQUIPPED',0x02:'GLOVES',0x04:'LEFT EAR',0x06:'RIGHT EAR',0x08:'NECK',\
                    0x0A:'CHEST',0x0C:'RIGHT BRACELET',0x0E:'LEFT BRACELET',0x10:'BRACERS',0x12:'LEFT RING',0x14:'RIGHT RING',\
                    0x16:'BELT',0x18:'LEGS',0x1A:'BOOTS',0x1C:'PRIMARY',0x1E:'SECONDARY',0x20:'2HAND',0x22:' add 22',\
                    0x24:'add 24',0x26:'add 26',0x28:'HELD',0x2A:'ROBE',0x2C:'add 2C',0x2E:'add 2E'}
 
  slot_dict       = {0x00:'UNKNOWN',0x01:'NOT EQUIPABLE',0x02:'HELM',0x04:'ROBE',0x06:'EARRING',0x08:'NECK',\
                    0x0A:'CHEST',0x0C:'BRACELET',0x0E:'BRACERS',0x10:'RING',0x12:'BELT',0x14:'LEGS',\
                    0x16:'BOOTS',0x18:'PRIMARY',0x1A:'SHIELD',0x1C:'SECONDARY',0x1E:'2HAND',0x20:' add 20',0x22:' add 22',\
                    0x24:'HELD',0x26:'GLOVES',0x28:'add 28',0x2A:'add 2A',0x2C:'add 2C',0x2E:'add 2E'}

  stat_dict       = {0x00:'STR',0x01:'UNKNOWN',0x02:'STA',0x04:'AGI',0x06:'DEX',0x08:'WIS',\
                    0x0A:'INT',0x0C:'CHA',0x0E:'add 0E',0x10:'HPMAX',0x12:'add 12',0x14:'POWMAX',\
                    0x16:'ADD 16',0x18:'PoT',0x1A:'HoT',0x1C:'AC',0x1E:'ADD 1E',0x20:' add 20',0x22:' add 22',\
                    0x24:'add 24',0x26:'add 26',0x28:'add 28',0x2A:'add 2A',0x2C:'PR',0x2E:'DR',\
                    0x30:'FR',0x32:'CR',0x34:'LR',0x36:'AR',0x38:'ADD 38',0x3A:'ADD 3A',0x3C:'ADD 3C'}
                    
  attack_dict       = {0x00:'NO ATTACK',0x01:'UNKNOWN',0x02:'1HS',0x04:'2HS',0x06:'1HB',0x08:'2HB',\
                      0x0A:'1HP',0x0C:'2HP',0x0E:'BOW',0x10:'1H CROSSBOW',0x12:'2H CROSSBOW',0x14:'THROWN'}                         
                     
                    
  gear_location_dict = {0x01:'TOON INVENTORY',0x02:'TOON BANK'}
  trade_dict         = {0x00:'TRADEABLE',0x02:'NO TRADE'}      
  rent_dict          = {0x00:'NO RENT'  ,0x02:'RENTABLE'}
  lore_type_dict     = {0x00:'NOT LORE' ,0x02:'LORE'}
  proc_type_dict     = {0x00:'NO PROC' ,0x02:'PROCS ON USE',0x04:'PROCS ON ATTACK',0x06:'PROCS ON DAMAGE TAKEN'}  
      
  
  
##
## 0x00 Family 
##
  if my_opcode == 0x0000:
      print '    OPCODE DESCR : GAME DISC VERSION'
#      
      hdr_fmt1 = '<I'   # (little endian)[DISK VERSION INFO]
      s = struct.unpack(hdr_fmt1,message[2:6]) #  
      disc_op = s[0]      
      if disc_op == 0x25:
        print '                 : USING EQOA Frontiers Disc'
        is_frontiers = 1
      elif disc_op == 0x12:
        print '                 : USING EQOA Vanilla Disc'  
      else:
        print '                 : USING Unknown Disc: Report Disc to DEV TEAM'  
  elif my_opcode == 0x0904 or my_opcode == 0x0001:
#      
      hdr_fmt1 = '<B'   # (little endian)[DISK VERSION INFO]
      s = struct.unpack(hdr_fmt1,message[2:3]) #  
      opcode_option = s[0] 
      if opcode_option == 0x00:
        print '    OPCODE DESCR : USER GAME LOGIN/AUTHENTICATION'
        if my_opcode == 0x0904:
          print '    OPCODE DESCR : WORLD SERVER AUTHENTICATION'
        elif my_opcode == 0x0001:
          print '    OPCODE DESCR : SHARD SERVER AUTHENTICATION'
        else:
          print '    OPCODE DESCR : -- UNKNOWN --'               
        hdr_fmt1 = '<II'   # (little endian)[DISK VERSION INFO]
        s = struct.unpack(hdr_fmt1,message[3:11]) #  
        op_section      = s[0]      
        op_gamecode_len = s[1]
        print '                 : NO IDEA (can be 01 or 03)   = ','0x{:04X}'.format(op_section),'{0:04d}'.format(op_section) 
        print '                 : LENGTH OF GAME CODE         = ','0x{:04X}'.format(op_gamecode_len),'{0:04d}'.format(op_gamecode_len)     
        hdr_fmt1 = '<4s'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[11:15]) #  
        op_game_code    = s[0]   
        print '                 : GAME CODE                   = ',op_game_code
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[15:19]) #  
        op_name_length    = s[0]   
        print '                 : USERNAME LENGTH             = ',op_name_length
        hdr_fmt1 = '<'+'{}'.format(op_name_length)+'s'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[19:19+op_name_length]) #  
        op_username    = s[0]   
        print '                 : USERNAME                    = ',op_username   
        hdr_fmt1 = '<b'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[19+op_name_length:19+op_name_length+1]) #  
        op_username_end    = s[0]   
        print '                 : USERNAME END                = ','0x{:02X}'.format(op_username_end),'{0:02d}'.format(op_username_end)   
        hdr_fmt1 = '<16s16s'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[19+op_name_length+1:19+op_name_length+1+32]) #  
        op_encrypt1    = s[0]
        op_encrypt2    = s[1]          
        print '                 : ENCRYPTED PWD PART1         = '," ".join("{:02X}".format(ord(c)) for c in op_encrypt1)
        print '                 : ENCRYPTED PWD PART2 (UNUSED)= '," ".join("{:02X}".format(ord(c)) for c in op_encrypt2) 
#        
  elif my_opcode == 0x07B3:  # SERVER SELECT SCREEN
#      
      print '    OPCODE DESCR : SERVER LISTING FOR SERVER SELECT'
      hdr_fmt1 = '<B'   # Read number of servers
      s = struct.unpack(hdr_fmt1,message[2:3]) #  
      opcode_num_servers = s[0]/2     
      print      
      print '                 : NUMBER OF SERVERS             = ','0x{:02X}'.format(opcode_num_servers),'{0:02d}'.format(opcode_num_servers) 
#
      b = 3
#      for server in xrange(1,2):
      for server in xrange(1,opcode_num_servers+1):      
        print
        print '                 : SERVER NUMBER               = ','{0:02d}'.format(server)        
        hdr_fmt1 = '<I'   # Read length of SERVER NAME
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4
        server_name_len = s[0]         
        print '                 : SERVER NAME LENGTH          = ','0x{:02X}'.format(server_name_len),'{0:02d}'.format(server_name_len)           
        hdr_fmt1 = '<'+'{}'.format(server_name_len)+'H'   # Read length of SERVER NAME
        s = struct.unpack(hdr_fmt1,message[b:b+server_name_len*2]) #  
        b = b + server_name_len*2
        server_name = s
        print '                 : SERVER NAME                 = ', "".join("{:s}".format(unichr(c)) for c in server_name)
        hdr_fmt1 = '<bHHBBBBb'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+10]) #  
        b = b + 10        
#
        recommend_flag    = s[0]
        endpoint_ID       = s[1]           
        endpoint_port     = s[2]
        ip_address        = s[3:7]
        language_flag     = s[7]
#        
        rec_desc = 'NO'
        if recommend_flag ==1:
          rec_desc = 'YES'
#
        if   language_flag == 0:
         lang_desc = 'US ENGLISH'        
        elif language_flag == 1:
          lang_desc = 'UK ENGLISH' 
        elif  language_flag == 2:
          lang_desc = 'FRENCH'       
        elif  language_flag == 3:
          lang_desc = 'GERMAN'  
        else:
          lang_desc = 'UNKNOWN LANGUAGE'        
#
#        print 
#
        print '                 : RECOMMEND FLAG              = ', '0x{:02X}'.format(recommend_flag),'{0:02d}'.format(recommend_flag)
        print '                 : RECOMMEND DESC              = ', rec_desc       
        print '                 : ENDPOINT ID                 = ', '{:4X}'.format(endpoint_ID) , '{0:08d}'.format(endpoint_ID)
        print '                 : IP ADDRESS                  = ', "".join("{:s}.".format(str(y)) for y in reversed(ip_address)).rstrip('.')    
        print '                 : PORT NUMBER                 = ', '0x{:02X}'.format(endpoint_port),'{0:02d}'.format(endpoint_port)      
        print '                 : LANGUAGE FLAG               = ', '0x{:02X}'.format(language_flag),'{0:02d}'.format(language_flag)        
        print '                 : LANGUAGE DESC               = ', lang_desc       
        print 
#
  elif my_opcode == 0x07D1:  # SERVER FAMILY OPCODE
#      
      print '    OPCODE DESCR : UNKNOWN - SELECTED SERVER SENDS THIS TO CLIENT BEFORE CHAR SELECT'
      hdr_fmt1 = '<I'   # Read number of servers
      s = struct.unpack(hdr_fmt1,message[2:6]) #  
      op_option = s[0]  
      print '                 : OPCODE OPTION                 = ','0x{:04X}'.format(op_option),'{0:05d}'.format(op_option) 
#      
  elif my_opcode == 0x07F5:  # SERVER FAMILY OPCODE
#      
      print '    OPCODE DESCR : UNKNOWN - SELECTED SERVER SENDS THIS TO CLIENT BEFORE CHAR SELECT'
      hdr_fmt1 = '<I'   # Read number of servers
      s = struct.unpack(hdr_fmt1,message[2:6]) #  
      op_option = s[0]  
      print '                 : OPCODE OPTION                 = ','0x{:04X}'.format(op_option),'{0:05d}'.format(op_option)   

  elif my_opcode == 0x002C:  # CHARACTER SELECT SCREEN
#      
      print '    OPCODE DESCR : CHARACTER LISTING FOR CHARACTER SELECT'
      hdr_fmt1 = '<B'   # Read number of characters
      s = struct.unpack(hdr_fmt1,message[2:3]) #  
      num_toons = s[0]/2     
      print      
      print '                 : NUMBER OF CHARACTERS          = ','0x{:02X}'.format(num_toons),'{0:02d}'.format(num_toons) 
#
      b = 3
#     for toon in xrange(1,2):
      for toon in xrange(1,num_toons+1):
        print
        print '                 : CHARACTER NUMBER            = ','{0:02d}'.format(toon)        
        hdr_fmt1 = '<I'   # Read length of TOON NAME
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4
        toon_name_len = s[0]         
        print '                 : CHARACTER NAME LENGTH       = ','0x{:02X}'.format(toon_name_len),'{0:02d}'.format(toon_name_len)           
        hdr_fmt1 = '<'+'{}'.format(toon_name_len)+'s'   # Read length of TOON NAME
        s = struct.unpack(hdr_fmt1,message[b:b+toon_name_len]) #  
        b = b + toon_name_len
        toon_name = s[0]
        print '                 : CHARACTER NAME              = ', toon_name
#        
        hdr_fmt1 = '<I'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #
        if s[0]>>24 == 0x01:
          toon_ID_server    = s[0]
          b = b + 4
        else:
          toon_ID_server    = s[0]&0x00FFFFFF
          b = b + 3           
#
        hdr_fmt1 = '<IB'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+5]) #  
        b = b + 5        
        model_ID  = int(s[0]) + (s[1]<<32)           
#        
        hdr_fmt1 = '<7B'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+7]) #  
        b = b + 7           
        
        toon_class        = s[0]
        toon_race         = s[1]
        toon_level        = s[2]
        toon_hair_color   = s[3]
        toon_hair_length  = s[4]
        toon_hair_style   = s[5]
        toon_face_option  = s[6]
#
        toon_ID_client  = s2c(toon_ID_server)
        model_ID_client = s2c(model_ID)
#        
        print '                 : CHARACTER SERVER ID         = ', '0x{:08X}'.format(toon_ID_server),'{0:012d}'.format(toon_ID_server)         
        print '                 : CHARACTER CLIENT ID (CALCd) = ', '0x{:08X}'.format(toon_ID_client),'{0:012d}'.format(toon_ID_client)         
        print '                 : CHARACTER SERVER MODEL ID   = ', '0x{:010X}'.format(model_ID),'{0:012d}'.format(model_ID), sprite_dict.get(model_ID)     
        print '                 : CHARACTER CLIENT MODEL ID   = ', '0x{:010X}'.format(model_ID_client),'{0:012d}'.format(model_ID_client)   
        print '                 : CHARACTER CLASS             = ', '0x{:02X}'.format(toon_class),'{0:04d}'.format(toon_class),class_dict[toon_class]  
        print '                 : CHARACTER RACE              = ', '0x{:02X}'.format(toon_race),'{0:04d}'.format(toon_race),  race_dict[toon_race] 
        print '                 : CHARACTER LEVEL             = ', '0x{:02X}'.format(toon_level),'{0:04d}'.format(toon_level),  'Level ','{0:02d}'.format(toon_level/2)
        print '                 : CHARACTER HAIR COLOR        = ', '0x{:02X}'.format(toon_hair_color),'{0:04d}'.format(toon_hair_color) ,hair_color_dict[toon_hair_color]    
        print '                 : CHARACTER HAIR LENGTH       = ', '0x{:02X}'.format(toon_hair_length),'{0:04d}'.format(toon_hair_length),hair_length_dict[toon_hair_length]        
        print '                 : CHARACTER HAIR STYLE        = ', '0x{:02X}'.format(toon_hair_style),'{0:04d}'.format(toon_hair_style), hair_style_dict[toon_hair_style]  
        print '                 : CHARACTER FACE OPTION       = ', '0x{:02X}'.format(toon_face_option),'{0:04d}'.format(toon_face_option), face_option_dict[toon_face_option]
        print
#
# Break up here for vanilla toons
#
        hdr_fmt1 = '<I'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4  
        robe              = s[0]
#
# Check for Vanilla Models
#        
        hdr_fmt1 = '<b'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+1]) #
        if s[0] != 0x00:  # Not quite correct, but close enought for now
         frontiers = 'YES'
        else:
         frontiers = 'NO'
         b = b + 1                  # only move b forward if vanilla characters
        print '                 : FRONTIERS CHARACTER         = ', frontiers
#                 
        hdr_fmt1 = '<3IH7B'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+21]) #  
        b = b + 21      
#
        primary           = s[0]           
        secondary         = s[1]
        shield            = s[2]
        toon_animation    = s[3]
        unknown           = s[4]
        chest_option      = s[5]
        bracer_option     = s[6]
        glove_option      = s[7]
        pants_option      = s[8]        
        boots_option      = s[9]
        helm_option       = s[10]
#
        if frontiers == 'YES':
#        
          hdr_fmt1 = '<HI'   #  These are actually vanilla color values if present.
          s = struct.unpack(hdr_fmt1,message[b:b+6]) #  
          b = b + 6 
          unknown2 = s[0]
          unknown3 = s[1]
          
          print '                 : CHARACTER UNKNOWN 2         = ', '0x{:04X}'.format(unknown2),'{0:05d}'.format(unknown2)        
          print '                 : CHARACTER UNKNOWN 3         = ', '0x{:08X}'.format(unknown3),'{0:09d}'.format(unknown3)          
  #
          hdr_fmt1 = '<10I'   # 
          s = struct.unpack(hdr_fmt1,message[b:b+40]) #  
          b = b + 40
          unknown1_color = ColorStuff(s[0])
          unknown2_color = ColorStuff(s[1])
          unknown3_color = ColorStuff(s[2])       
          chest_color    = ColorStuff(s[3])
          bracer_color   = ColorStuff(s[4])
          glove_color    = ColorStuff(s[5]) 
          pant_color     = ColorStuff(s[6])
          boot_color     = ColorStuff(s[7])
          helm_color     = ColorStuff(s[8])
          robe_color     = ColorStuff(s[9])
  #
          print '                 : CHARACTER ROBE MODEL        = ', '0x{:08X}'.format(robe),'{0:09d}'.format(robe), robe_dict.get(robe), '- Color: ',robe_color.name        
          print '                 : CHARACTER PRIMARY           = ', '0x{:08X}'.format(primary),'{0:09d}'.format(primary)  
          print '                 : CHARACTER SECONDARY         = ', '0x{:08X}'.format(secondary),'{0:09d}'.format(secondary)        
          print '                 : CHARACTER SHIELD            = ', '0x{:08X}'.format(shield),'{0:09d}'.format(shield)  
          print '                 : CHARACTER ANIMATION         = ', '0x{:04X}'.format(toon_animation),'{0:05d}'.format(toon_animation), animate_dict.get(toon_animation)
          print '                 : CHARACTER  - UNKNOWN-       = ', '0x{:02X}'.format(unknown),'{0:04d}'.format(unknown)
          print '                 : CHARACTER CHEST MODEL       = ', '0x{:02X}'.format(chest_option),'{0:04d}'.format(chest_option), armor_dict.get(chest_option),    '- Color: ',chest_color.name
          print '                 : CHARACTER BRACER MODEL      = ', '0x{:02X}'.format(bracer_option),'{0:04d}'.format(bracer_option), armor_dict.get(bracer_option), '- Color: ',bracer_color.name        
          print '                 : CHARACTER GLOVE MODEL       = ', '0x{:02X}'.format(glove_option),'{0:04d}'.format(glove_option), armor_dict.get(glove_option),    '- Color: ',glove_color.name 
          print '                 : CHARACTER PANTS MODEL       = ', '0x{:02X}'.format(pants_option),'{0:04d}'.format(pants_option), armor_dict.get(pants_option),    '- Color: ',pant_color.name
          print '                 : CHARACTER BOOTS MODEL       = ', '0x{:02X}'.format(boots_option),'{0:04d}'.format(boots_option), armor_dict.get(boots_option),    '- Color: ',boot_color.name 
          print '                 : CHARACTER HELM MODEL        = ', '0x{:02X}'.format(helm_option),'{0:04d}'.format(helm_option), armor_dict.get(helm_option),       '- Color: ',helm_color.name 
          print  
  #
          print '                 : CHARACTER UNKNOWN1 COLOR    = ', '0x{:06X}'.format(unknown1_color.hexrep),unknown1_color.name        
          print '                 : CHARACTER UNKNOWN2 COLOR    = ', '0x{:06X}'.format(unknown2_color.hexrep),unknown2_color.name        
          print '                 : CHARACTER UNKNOWN3 COLOR    = ', '0x{:06X}'.format(unknown3_color.hexrep),unknown3_color.name       
          print '                 : CHARACTER CHEST    COLOR    = ', '0x{:06X}'.format(chest_color.hexrep),chest_color.name        
          print '                 : CHARACTER BRACER   COLOR    = ', '0x{:06X}'.format(bracer_color.hexrep),bracer_color.name        
          print '                 : CHARACTER GLOVE    COLOR    = ', '0x{:06X}'.format(glove_color.hexrep),glove_color.name     
          print '                 : CHARACTER PANT     COLOR    = ', '0x{:06X}'.format(pant_color.hexrep),pant_color.name        
          print '                 : CHARACTER BOOT     COLOR    = ', '0x{:06X}'.format(boot_color.hexrep),boot_color.name        
          print '                 : CHARACTER HELM     COLOR    = ', '0x{:06X}'.format(helm_color.hexrep),helm_color.name        
          print '                 : CHARACTER ROBE     COLOR    = ', '0x{:06X}'.format(robe_color.hexrep),robe_color.name               
          print 
#         
        else:  # vanilla eqoa
#        
          hdr_fmt1 = '<6b'   #  These are actually vanilla color values if present.
          s = struct.unpack(hdr_fmt1,message[b:b+6]) #  
          b = b + 6 
#          
          chest_color    = ColorStuffVanilla(s[0])
          bracer_color   = ColorStuffVanilla(s[1])
          glove_color    = ColorStuffVanilla(s[2]) 
          pant_color     = ColorStuffVanilla(s[3])
          boot_color     = ColorStuffVanilla(s[4])
          helm_color     = ColorStuffVanilla(s[5])
          robe_color     = ColorStuffVanilla(s[5])          
           
  #
          print '                 : CHARACTER ROBE MODEL        = ', '0x{:08X}'.format(robe),'{0:09d}'.format(robe), robe_dict.get(robe), '- Color: ',robe_color.name        
          print '                 : CHARACTER PRIMARY           = ', '0x{:08X}'.format(primary),'{0:09d}'.format(primary)  
          print '                 : CHARACTER SECONDARY         = ', '0x{:08X}'.format(secondary),'{0:09d}'.format(secondary)        
          print '                 : CHARACTER SHIELD            = ', '0x{:08X}'.format(shield),'{0:09d}'.format(shield)  
          print '                 : CHARACTER ANIMATION         = ', '0x{:04X}'.format(toon_animation),'{0:05d}'.format(toon_animation), animate_dict.get(toon_animation)
          print '                 : CHARACTER  - UNKNOWN-       = ', '0x{:02X}'.format(unknown),'{0:04d}'.format(unknown)
          print '                 : CHARACTER CHEST MODEL       = ', '0x{:02X}'.format(chest_option),'{0:04d}'.format(chest_option), armor_dict.get(chest_option),    '- Color: ',chest_color.name
          print '                 : CHARACTER BRACER MODEL      = ', '0x{:02X}'.format(bracer_option),'{0:04d}'.format(bracer_option), armor_dict.get(bracer_option), '- Color: ',bracer_color.name        
          print '                 : CHARACTER GLOVE MODEL       = ', '0x{:02X}'.format(glove_option),'{0:04d}'.format(glove_option), armor_dict.get(glove_option),    '- Color: ',glove_color.name 
          print '                 : CHARACTER PANTS MODEL       = ', '0x{:02X}'.format(pants_option),'{0:04d}'.format(pants_option), armor_dict.get(pants_option),    '- Color: ',pant_color.name
          print '                 : CHARACTER BOOTS MODEL       = ', '0x{:02X}'.format(boots_option),'{0:04d}'.format(boots_option), armor_dict.get(boots_option),    '- Color: ',boot_color.name 
          print '                 : CHARACTER HELM MODEL        = ', '0x{:02X}'.format(helm_option),'{0:04d}'.format(helm_option), armor_dict.get(helm_option),       '- Color: ',helm_color.name 
          print  
  #    
          print '                 : CHARACTER CHEST    COLOR    = ', '0x{:02X}'.format(chest_color.index),chest_color.name        
          print '                 : CHARACTER BRACER   COLOR    = ', '0x{:02X}'.format(bracer_color.index),bracer_color.name        
          print '                 : CHARACTER GLOVE    COLOR    = ', '0x{:02X}'.format(glove_color.index),glove_color.name     
          print '                 : CHARACTER PANT     COLOR    = ', '0x{:02X}'.format(pant_color.index),pant_color.name        
          print '                 : CHARACTER BOOT     COLOR    = ', '0x{:02X}'.format(boot_color.index),boot_color.name        
          print '                 : CHARACTER HELM     COLOR    = ', '0x{:02X}'.format(helm_color.index),helm_color.name        
          print '                 : CHARACTER ROBE     COLOR    = ', '0x{:02X}'.format(robe_color.index),robe_color.name               
          print 
#                   
#     
  elif my_opcode == 0x002A:  # CHARACTER SELECTED
#      
      print '    OPCODE DESCR : CHARACTER SELECTED BY CLIENT with PERSONALIZATION'
      hdr_fmt1 = '<I4I'   # 
      s = struct.unpack(hdr_fmt1,message[2:22]) #  
      client_model = s[0]
      toon_hair_color   = s[1]
      toon_hair_length  = s[2]
      toon_hair_style   = s[3]
      toon_face_option  = s[4]  
#      
      print '                 : CHARACTER CLIENT ID          = ', '0x{:08X}'.format(client_model),'{0:09d}'.format(client_model) 
      print '                 : CHARACTER HAIR COLOR        = ', '0x{:08X}'.format(toon_hair_color),'{0:09d}'.format(toon_hair_color) ,hair_color_dict.get(2*toon_hair_color)    
      print '                 : CHARACTER HAIR LENGTH       = ', '0x{:08X}'.format(toon_hair_length),'{0:09d}'.format(toon_hair_length),hair_length_dict.get(2*toon_hair_length)        
      print '                 : CHARACTER HAIR STYLE        = ', '0x{:08X}'.format(toon_hair_style),'{0:09d}'.format(toon_hair_style), hair_style_dict.get(2*toon_hair_style)  
      print '                 : CHARACTER FACE OPTION       = ', '0x{:08X}'.format(toon_face_option),'{0:09d}'.format(toon_face_option), face_option_dict.get(2*toon_face_option)
      print      
 
  elif my_opcode == 0x0013:  # SERVER RELATED - SESSION RELATED
#      
      print '       OPCODE DESCR : UNKNOWN SERVER RELATED. AFTER CHAR SELECT. RELATED TO SESSIONS?'
      hdr_fmt1 = '<HHIHHI'   # 
      s = struct.unpack(hdr_fmt1,message[2:18]) #  
      session_UNK1 = s[0]
      session_B1   = s[1]
      length_1     = s[2]
      session_UNK2 = s[3]
      session_B2   = s[4]
      length_2     = s[5]  
#      
      print '       UNKNOWN SESSION (RELATED)   = ', '0x{:02X}'.format(session_UNK1),'{0:04d}'.format(session_UNK1) 
      print '       CURRENT SESSION (SECOND)    = ', '0x{:02X}'.format(session_B1),'{0:04d}'.format(session_B1) 
      print '       UNKNOWN LENGTH              = ', '0x{:04X}'.format(length_1),'{0:05d}'.format(length_1)
      print '       ---'
      print '       UNKNOWN SESSION (RELATED)   = ', '0x{:02X}'.format(session_UNK2),'{0:04d}'.format(session_UNK2) 
      print '       CURRENT SESSION (SECOND)    = ', '0x{:02X}'.format(session_B2),'{0:04d}'.format(session_B2) 
      print '       UNKNOWN LENGTH              = ', '0x{:04X}'.format(length_2),'{0:05d}'.format(length_2)             
      print      
      
  elif my_opcode == 0x0790:  # TRANSFER DIRECTIONS FROM SERVER
#      
      print '    OPCODE DESCR : SERVER GIVING CLIENT TRANSFER INSTRUCTIONS'
      hdr_fmt1 = '<HHBBBBIHHBBBB'   # 
      s = struct.unpack(hdr_fmt1,message[2:2+20]) #  
#
      new_server_ID    = s[0]
      new_server_PORT  = s[1]
      new_server_IP    = s[2:6] 
      transfernum      = s[6]      
      client_code      = s[7]
      dummy_port       = s[8]
      dummy_IP         = s[9:13]
#
      print 
      print '    NEW SERVER ID      : ','0x{:04X}'.format(new_server_ID),'{0:05d}'.format(new_server_ID)
      print '    NEW SERVER PORT    : ','0x{:04X}'.format(new_server_PORT),'{0:05d}'.format(new_server_PORT)
      print '    NEW SERVER IP      : ', "".join("{:s}.".format(str(y)) for y in reversed(new_server_IP)).rstrip('.') 
      print '    TRANSFER NUMBER    : ','0x{:04X}'.format(transfernum),'{0:05d}'.format(transfernum)
      print     
      print '    CLIENT ENDPOINT ID : ','0x{:04X}'.format(client_code),'{0:05d}'.format(client_code)
      print '    DUMMY PORT NUMBER  : ','0x{:04X}'.format(dummy_port),'{0:05d}'.format(dummy_port)
      print '    DUMMY IP ADDRESS   : ', "".join("{:s}.".format(str(y)) for y in reversed(dummy_IP)).rstrip('.') 
      print      
      
  elif my_opcode == 0x0A7A:  # WHITE MESSAGE
#      
      print '    OPCODE DESCR : WHITE MESSAGE FROM SERVER'
      b=2
      hdr_fmt1 = 'I'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
      b = b + 4
      message_length   = s[0]
      print
      print '      MESSAGE  LENGTH   = ', '0x{:04X}'.format(message_length),'{0:05d}'.format(message_length)     
      hdr_fmt1 = '<'+'{}'.format(message_length)+'H'   # 
      s = struct.unpack(hdr_fmt1,message[b:b+message_length*2]) #  
      b = b + message_length*2
      message_desc = s      
      print '      SERVER MESSAGE    = ', "".join("{:s}".format(unichr(c)) for c in message_desc)       
      print 

  elif my_opcode == 0x0A7B:  # COLORED MESSAGE
#      
      print '    OPCODE DESCR : COLORED MESSAGE FROM SERVER'
      b=2
      hdr_fmt1 = 'I'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
      b = b + 4
      message_length   = s[0]
      print
      print '      MESSAGE  LENGTH   = ', '0x{:04X}'.format(message_length),'{0:05d}'.format(message_length)     
      hdr_fmt1 = '<'+'{}'.format(message_length)+'H'   # 
      s = struct.unpack(hdr_fmt1,message[b:b+message_length*2]) #  
      b = b + message_length*2
      message_desc = s      
      print '      SERVER MESSAGE    = ', "".join("{:s}".format(unichr(c)) for c in message_desc)       
      hdr_fmt1 = '<BBB'   # 
      s = struct.unpack(hdr_fmt1,message[b:b+3]) #  
      b = b + 3
      text_color = s[0] # fix this here to process correctly. Have local color table for text
      print '      MESSAGE COLOR     = ', '0x{:06X}'.format(text_color)
      print 
      
  elif my_opcode == 0x1206:  # Auction Update Message?
#      
      b=2
      hdr_fmt1 = 'B'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+1]) #  
      b = b + 1
      opcode_option   = s[0]
      
      if opcode_option==0x89: # Auction update?
        print '    OPCODE DESCR : SOME TYPE OF AUCTION UPDATE MESSAGE?'
#
        u_in = 7                                       # specify number of units to extract
        b_in = 50                                      # number of bytes to read
        b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]          
#        
        print
        print '      AUCTION ID NUMBER        = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))        
        print '      UNKNOWN 1                = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1]))  
        print '      USED HP                  = ', '0x{:010X}'.format(s[2]), '{0:012d}'.format(s[2]), '0x{:010X}'.format(s2c(s[2])), '{0:012d}'.format(s2c(s[2]))        
        print '      UNKNOWN 2                = ', '0x{:010X}'.format(s[3]), '{0:012d}'.format(s[3]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3]))          
        print '      CURRENT BID              = ', '0x{:010X}'.format(s[4]), '{0:012d}'.format(s[4]), '0x{:010X}'.format(s2c(s[4])), '{0:012d}'.format(s2c(s[4]))        
        print '      MAXIMUM BID              = ', '0x{:010X}'.format(s[5]), '{0:012d}'.format(s[5]), '0x{:010X}'.format(s2c(s[5])), '{0:012d}'.format(s2c(s[5]))            
        print '      TIME LEFT?               = ', '0x{:010X}'.format(s[6]), '{0:012d}'.format(s[6]), '0x{:010X}'.format(s2c(s[6])), '{0:012d}'.format(s2c(s[6]))  
        print         
#        
        hdr_fmt1 = 'I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4
        toon_name_length   = s[0]
        print '      WINNER NAME LENGTH       = ', '0x{:04X}'.format(toon_name_length),'{0:05d}'.format(toon_name_length)
#       
        hdr_fmt1 = '<'+'{}'.format(toon_name_length)+'s'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+toon_name_length]) #  
        b = b + toon_name_length
        toon_name  = s[0]         
        print '      WINNER NAME              = ',toon_name
#
        u_in = 22                                       # specify number of units to extract
        b_in = 100                                      # number of bytes to read
        b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]   
#
        print 
        print '      ITEM ID?                 = ', '0x{:010X}'.format(s[ 0]), '{0:012d}'.format(s[ 0]), '0x{:010X}'.format(s2c(s[ 0])), '{0:012d}'.format(s2c(s[ 0]))
        print '      ITEM FAMILY?             = ', '0x{:010X}'.format(s[ 1]), '{0:012d}'.format(s[ 1]), '0x{:010X}'.format(s2c(s[ 1])), '{0:012d}'.format(s2c(s[ 1]))        
        print '      UNKNOWN COL 9            = ', '0x{:010X}'.format(s[ 2]), '{0:012d}'.format(s[ 2]), '0x{:010X}'.format(s2c(s[ 2])), '{0:012d}'.format(s2c(s[ 2]))         
        print '      ITEM ICON ID             = ', '0x{:010X}'.format(s[ 3]), '{0:012d}'.format(s[ 3]), '0x{:010X}'.format(s2c(s[ 3])), '{0:012d}'.format(s2c(s[ 3]))
        print '      UNKNOWN COL 11           = ', '0x{:010X}'.format(s[ 4]), '{0:012d}'.format(s[ 4]), '0x{:010X}'.format(s2c(s[ 4])), '{0:012d}'.format(s2c(s[ 4]))        
        print '      SLOT TYPE                = ', '0x{:010X}'.format(s[ 5]), '{0:012d}'.format(s[ 5]), '0x{:010X}'.format(s2c(s[ 5])), '{0:012d}'.format(s2c(s[ 5])),slot_dict.get(s[5])
        print '      UNKNOWN COL 13           = ', '0x{:010X}'.format(s[ 6]), '{0:012d}'.format(s[ 6]), '0x{:010X}'.format(s2c(s[ 6])), '{0:012d}'.format(s2c(s[ 6]))        
        print '      TRADE TYPE               = ', '0x{:010X}'.format(s[ 7]), '{0:012d}'.format(s[ 7]), '0x{:010X}'.format(s2c(s[ 7])), '{0:012d}'.format(s2c(s[ 7])),trade_dict.get(s[7])     
        print '      RENT TYPE                = ', '0x{:010X}'.format(s[ 8]), '{0:012d}'.format(s[ 8]), '0x{:010X}'.format(s2c(s[ 8])), '{0:012d}'.format(s2c(s[ 8])),rent_dict.get(s[8])
        print '      UNKNOWN COL 16           = ', '0x{:010X}'.format(s[ 9]), '{0:012d}'.format(s[ 9]), '0x{:010X}'.format(s2c(s[ 9])), '{0:012d}'.format(s2c(s[ 9]))        
        print                                                                                                                                                   
        print '      ATTACK TYPE              = ', '0x{:010X}'.format(s[10]), '{0:012d}'.format(s[10]), '0x{:010X}'.format(s2c(s[10])), '{0:012d}'.format(s2c(s[10])),attack_dict.get(s[10])
        print '      DAMAGE                   = ', '0x{:010X}'.format(s[11]), '{0:012d}'.format(s[11]), '0x{:010X}'.format(s2c(s[11])), '{0:012d}'.format(s2c(s[11]))        
        print '      UNKNOWN COL 19           = ', '0x{:010X}'.format(s[12]), '{0:012d}'.format(s[12]), '0x{:010X}'.format(s2c(s[12])), '{0:012d}'.format(s2c(s[12]))         
        print '      ITEM LEVEL               = ', '0x{:010X}'.format(s[13]), '{0:012d}'.format(s[13]), '0x{:010X}'.format(s2c(s[13])), '{0:012d}'.format(s2c(s[13]))
        print '      MAXIMUM STACK SIZE       = ', '0x{:010X}'.format(s[14]), '{0:012d}'.format(s[14]), '0x{:010X}'.format(s2c(s[14])), '{0:012d}'.format(s2c(s[14]))        
        print '      MAXIMUM HP               = ', '0x{:010X}'.format(s[15]), '{0:012d}'.format(s[15]), '0x{:010X}'.format(s2c(s[15])), '{0:012d}'.format(s2c(s[15]))
        print '      DURATION                 = ', '0x{:010X}'.format(s[16]), '{0:012d}'.format(s[16]), '0x{:010X}'.format(s2c(s[16])), '{0:012d}'.format(s2c(s[16]))        
        print '      CLASS EQUIPABLE          = ', '0x{:010X}'.format(s[17]), '{0:012d}'.format(s[17]), '0x{:010X}'.format(s2c(s[17])), '{0:012d}'.format(s2c(s[17]))#,class_equip_dict.get(s[17])     
        print '      RACE  EQUIPABLE          = ', '0x{:010X}'.format(s[18]), '{0:012d}'.format(s[18]), '0x{:010X}'.format(s2c(s[18])), '{0:012d}'.format(s2c(s[18]))#,race_equip_dict.get(s[18])
        print '      PROC TYPE                = ', '0x{:010X}'.format(s[19]), '{0:012d}'.format(s[19]), '0x{:010X}'.format(s2c(s[19])), '{0:012d}'.format(s2c(s[19])),proc_type_dict.get(s2c(s[19]))        
        print '      LORE TYPE                = ', '0x{:010X}'.format(s[20]), '{0:012d}'.format(s[20]), '0x{:010X}'.format(s2c(s[20])), '{0:012d}'.format(s2c(s[20])),lore_type_dict.get(s[20])     
        print '      UNKNOWN 28               = ', '0x{:010X}'.format(s[21]), '{0:012d}'.format(s[21]), '0x{:010X}'.format(s2c(s[21])), '{0:012d}'.format(s2c(s[21]))
#        print '      UNKNOWN 29               = ', '0x{:010X}'.format(s[22]), '{0:012d}'.format(s[22]), '0x{:010X}'.format(s2c(s[22])), '{0:012d}'.format(s2c(s[22]))           
        print
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '      GEAR NAME LENGTH         = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        gear_name_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(gear_name_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+gear_name_length*2]) #  
        b = b + gear_name_length*2
        gear_name = s      
        print '      GEAR NAME                = ', "".join("{:s}".format(unichr(c)) for c in gear_name)   
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '      GEAR DESC. LENGTH        = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        gear_desc_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(gear_desc_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+gear_desc_length*2]) #  
        b = b + gear_desc_length*2
        gear_desc = s      
        print '      GEAR DESCRIPTION         = ', "".join("{:s}".format(unichr(c)) for c in gear_desc)   
#
        u_in = 1                                      # specify number of units to extract
        b_in = 3                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]   
#        
        num_stats = s2c(s[0])        
        print
        print '      NUMBER OF STATS          = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])) 
        this_stat_num = 0
        for y in range(0,num_stats): 
          this_stat_num = this_stat_num +1     
          u_in = 2                                      # specify number of units to extract
          b_in = 10                                     # number of bytes to read
          b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
          hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
          ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
          it = unit_extractor(u_in,b_in,ot)
          s  = it[2]
          b  = b+it[1]   
          print
          print '          STAT NAME            = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])),stat_dict.get(s[0])  
          print '          STAT INCREASE        = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1])), s2c(s[1])     
#
      
    
      
  elif my_opcode == 0x000D:  # DUMP TO CLIENT MEMORY
#      
      print '    OPCODE DESCR : DUMP TO CLIENT MEMORY'
      hdr_fmt1 = '<BI'   # 
      s = struct.unpack(hdr_fmt1,message[2:7]) #  
      opcode_option      = s[0]
      world_file_length  = s[1]
#      
      print
      print '      OPCODE OPTION      = ', '0x{:01X}'.format(opcode_option),'{0:02d}'.format(opcode_option) 
      print '      WORLD FILE LENGTH  = ', '0x{:04X}'.format(world_file_length),'{0:05d}'.format(world_file_length)
#     
      hdr_fmt1 = '<'+'{}'.format(world_file_length)+'s'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[7:7+world_file_length]) #  
      b = 7 + world_file_length
      world_file  = s[0]         
      print '      WORLD FILE NAME    = ',world_file
#    
      u_in = 1                                      # specify number of units to extract
      b_in = 8                                      # number of bytes to read
      b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
      hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
      ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
      it = unit_extractor(u_in,b_in,ot)
      s  = it[2]
      b  = b+it[1]
 
      server_model_id     = s[0]
      print '      SERVER SIDE ID     = ', '0x{:010X}'.format(server_model_id),'{0:010d}'.format(server_model_id)
      print '      CLIENT SIDE ID     = ', '0x{:010X}'.format(s2c(server_model_id)),'{0:010d}'.format(s2c(server_model_id))
      
#  
      hdr_fmt1 = 'I'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
      b = b + 4
      toon_name_length   = s[0]
      print '      TOON NAME LENGTH   = ', '0x{:04X}'.format(toon_name_length),'{0:05d}'.format(toon_name_length)
#
      hdr_fmt1 = '<'+'{}'.format(toon_name_length)+'s'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+toon_name_length]) #  
      b = b + toon_name_length
      toon_name  = s[0]         
      print '      TOON NAME          = ',toon_name
#
      hdr_fmt1 = '3B'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+3]) #  
      b = b + 3
      toon_class  = s[0]
      toon_race   = s[1]
      toon_level  = s[2]
#
      print '      TOON CLASS         = ', '0x{:02X}'.format(toon_class),'{0:04d}'.format(toon_class),class_dict[toon_class]  
      print '      TOON RACE          = ', '0x{:02X}'.format(toon_race),'{0:04d}'.format(toon_race),  race_dict[toon_race] 
      print '      TOON LEVEL         = ', '0x{:02X}'.format(toon_level),'{0:04d}'.format(toon_level),  'Level ','{0:02d}'.format(toon_level/2)
#
      u_in = 2                                      # specify number of units to extract
      b_in = 30                                      # number of bytes to read
      b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
      hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
      ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
      it = unit_extractor(u_in,b_in,ot)
      s  = it[2]
      b  = b+it[1]
# 
      print
      print '      PROPERTY         SERVER HEX | SERVER DEC | CLIENT HEX | CLIENT DEC'
      print '      -----------------------------------------------------------------------'
      print '      XP INTO LVL   = ', '0x{:09X}'.format(s[0]),'{0:010d}'.format(s[0]), '  0x{:09X}'.format(s2c(s[0])),'  {0:010d}'.format(s2c(s[0]))
      print '      DEBT          = ', '0x{:09X}'.format(s[1]),'{0:010d}'.format(s[1]), '  0x{:09X}'.format(s2c(s[1])),'  {0:010d}'.format(s2c(s[1]))
#
      hdr_fmt1 = '<B'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+1]) #  
      b = b + 1 
      print '      TOON FLAG     = ', '0x{:09X}'.format(s[0]),'{0:010d}'.format(s[0])#, '  0x{:09X}'.format(s2c(s[0])),'  {0:08d}'.format(s2c(s[0]))     
#      
      u_in = 5                                      # specify number of units to extract
      b_in = 30                                      # number of bytes to read
      b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
      hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
      ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
      it = unit_extractor(u_in,b_in,ot)
      s  = it[2]
      b  = b+it[1]
#
      base_max_stat =  s2c(s[3])     
#      
      print '      TOON TUNAR    = ', '0x{:09X}'.format(s[0]),'{0:010d}'.format(s[0]), '  0x{:09X}'.format(s2c(s[0])),'  {0:010d}'.format(s2c(s[0]))
      print '      BANK TUNAR    = ', '0x{:09X}'.format(s[1]),'{0:010d}'.format(s[1]), '  0x{:09X}'.format(s2c(s[1])),'  {0:010d}'.format(s2c(s[1]))      
      print '      TRN PT AVAIL  = ', '0x{:09X}'.format(s[2]),'{0:010d}'.format(s[2]), '  0x{:09X}'.format(s2c(s[2])),'  {0:010d}'.format(s2c(s[2]))
      print '      BASE MAX STAT = ', '0x{:09X}'.format(s[3]),'{0:010d}'.format(s[3]), '  0x{:09X}'.format(s2c(s[3])),'  {0:010d}'.format(s2c(s[3]))
      print '      UNKNOWN       = ', '0x{:09X}'.format(s[4]),'{0:010d}'.format(s[4]), '  0x{:09X}'.format(s2c(s[4])),'  {0:010d}'.format(s2c(s[4]))
#  
      hdr_fmt1 = '<4I'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+16]) #  
      b = b + 16
#       
#     print '0x{:09X}'.format(s[0]),hex2float('{:8X}'.format(s[0]))
      print
      print '      Y COORDINATE             = ', '0x{:09X}'.format(s[0]),hex2float('{:8X}'.format(s[0]))
      print '      Z COORDINATE             = ', '0x{:09X}'.format(s[1]),hex2float('{:8X}'.format(s[1]))
      print '      X COORDINATE             = ', '0x{:09X}'.format(s[2]),hex2float('{:8X}'.format(s[2]))
      print '      FACING DIRECTION         = ', '0x{:09X}'.format(s[3]),hex2float('{:8X}'.format(s[3])) 
      print '      FACING ANGLE FROM N      = ', '0x{:09X}'.format(s[3]),float(hex2float('{:8X}'.format(s[3])))/3.14159*180.0, ' degrees' 
#
      print        
      print '#######################################'
      print '#             HOTKEYS                 #'
      print '#######################################'
#
      hdr_fmt1 = '<4BIB'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+9]) #  
      b = b + 9 
      print
      print '      UNKNOWN 1               = ', '0x{:09X}'.format(s[0]),'{0:010d}'.format(s[0])#, '  0x{:09X}'.format(s2c(s[0])),'  {0:08d}'.format(s2c(s[0]))     
      print '      UNKNOWN 2               = ', '0x{:09X}'.format(s[1]),'{0:010d}'.format(s[1])#, '  0x{:09X}'.format(s2c(s[1])),'  {0:08d}'.format(s2c(s[1]))     
      print '      UNKNOWN 3               = ', '0x{:09X}'.format(s[2]),'{0:010d}'.format(s[2])#, '  0x{:09X}'.format(s2c(s[2])),'  {0:08d}'.format(s2c(s[2]))     
      print '      UNKNOWN 4               = ', '0x{:09X}'.format(s[3]),'{0:010d}'.format(s[3])#, '  0x{:09X}'.format(s2c(s[3])),'  {0:08d}'.format(s2c(s[3]))     
      print
      print '      HOTKEY FLAG?            = ', '0x{:09X}'.format(s[4]),'{0:010d}'.format(s[4])#, '  0x{:09X}'.format(s2c(s[4])),'  {0:08d}'.format(s2c(s[4]))     
#       
      print '      NUMBER OF HOTKEY MENUS  = ', '0x{:02X}'.format(s2c(s[5])),'{0:04d}'.format(s2c(s[5])) 

      num_menus = s2c(s[5])  
#
      hotkey_dir_dict   = {00:'North Tab',02:'West Tab',04:'East Tab',06:'South Tab'}
#      
      this_menu_num = 0
      for y in range(0,num_menus):
        this_menu_num = this_menu_num +1
#        
#      Menu Number Unit
#
        u_in = 1                                      # specify number of units to extract
        b_in = 3                                      # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]   
        hotkey_menu_number = s[0]      
#
        print
        print '      -----------------------------------------------------------------------------'
        print '      HOTKEY MENU NUMBER      = ',this_menu_num,' out of ',num_menus           
        print '      HOTKEY MENU CODE        = ', '0x{:02X}'.format(s[0]),'{0:04d}'.format(s[0])      
        for x in range(0,4): # Loop over four tabs
#        
          hdr_fmt1 = '<BI'   # (little endian)
          s = struct.unpack(hdr_fmt1,message[b:b+5]) #  
          b = b + 5      
#          
          print
          print '         HOTKEY LOCATION         = ', '0x{:02X}'.format(s[0]),hotkey_dir_dict.get(s[0])    
          print '         ACTION LENGTH           = ', '0x{:08X}'.format(s[1]), '{0:08d}'.format(s[1])   
          
          action_length = s[1]
          if action_length != 0x0000:
            hdr_fmt1 = '<'+'{}'.format(action_length)+'H'   # Read length of SERVER NAME
            s = struct.unpack(hdr_fmt1,message[b:b+action_length*2]) #  
            b = b + action_length*2
            action_name = s
            print '         HOTKEY ACTION           =  Prints "', "".join("{:s}".format(unichr(c)) for c in action_name),'"'
#            
            if action_name[0]==0x0020:
              print '         HOTKEY ACTION           =  CREATE SUB MENU' 
          else:
            print '         HOTKEY ACTION           =  TAB LABEL PRINTED'
            
          hdr_fmt1 = '<I'   # (little endian)
          s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
          b = b + 4            
          print '         TAB LABEL LENGTH        = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
          tab_name_length = s[0]
          hdr_fmt1 = '<'+'{}'.format(tab_name_length)+'H'   # Read length of SERVER NAME
          s = struct.unpack(hdr_fmt1,message[b:b+tab_name_length*2]) #  
          b = b + tab_name_length*2
          tab_name = s      
          print '         TAB LABEL               =  >', "".join("{:s}".format(unichr(c)) for c in tab_name),'<'
#
# QUESTS ARE NEXT
#  
      print        
      print '#######################################'
      print '#             QUESTS                  #'
      print '#######################################'
#
      hdr_fmt1 = '<4BII'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+12]) #  
      b = b + 12 
      print
      print '      UNKNOWN 1               = ', '0x{:09X}'.format(s[0]),'{0:010d}'.format(s[0])#, '  0x{:09X}'.format(s2c(s[0])),'  {0:08d}'.format(s2c(s[0]))     
      print '      UNKNOWN 2               = ', '0x{:09X}'.format(s[1]),'{0:010d}'.format(s[1])#, '  0x{:09X}'.format(s2c(s[1])),'  {0:08d}'.format(s2c(s[1]))     
      print '      UNKNOWN 3               = ', '0x{:09X}'.format(s[2]),'{0:010d}'.format(s[2])#, '  0x{:09X}'.format(s2c(s[2])),'  {0:08d}'.format(s2c(s[2]))     
      print '      UNKNOWN 4               = ', '0x{:09X}'.format(s[3]),'{0:010d}'.format(s[3])#, '  0x{:09X}'.format(s2c(s[3])),'  {0:08d}'.format(s2c(s[3]))     
      print
      print '      QUEST FLAG?            = ', '0x{:09X}'.format(s[4]),'{0:010d}'.format(s[4])#, '  0x{:09X}'.format(s2c(s[4])),'  {0:08d}'.format(s2c(s[4]))     
#       
      print '      NUMBER OF QUESTS       = ', '0x{:02X}'.format(s[5]),'{0:04d}'.format(s[5]) 
      num_quests = s[5]  
      
      this_quest_num = 0
      for y in range(0,num_quests):  
        this_quest_num = this_quest_num +1
#
        print
        print '      -----------------------------------------------------------------------------'
        print '      QUEST NUMBER            = ', this_quest_num, ' out of ',num_quests         
#
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '         QUEST DESC. LENGTH      = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        quest_desc_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(quest_desc_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+quest_desc_length*2]) #  
        b = b + quest_desc_length*2
        quest_desc = s      
        print '         QUEST DESCRIPTION       =  ', "".join("{:s}".format(unichr(c)) for c in quest_desc)   
#
# INVENTORY GEAR
#  
      print        
      print '#######################################'
      print '#            GEAR ON TOON             #'
      print '#######################################'        

    
               
      hdr_fmt1 = '<BI'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+5]) #  
      b = b + 5      
#      
      print
      print '  GEAR NUMBER (BYTE)      = ', '0x{:02X}'.format(s2c(s[0])), '{0:02d}'.format(s2c(s[0]))     
      print '  GEAR NUMBER (INTEGER)   = ', '0x{:08X}'.format(s[1]), '{0:08d}'.format(s[1])    
      
      num_toon_gear = s2c(s[0])
        
      this_gear_num = 0
      for y in range(0,num_toon_gear):  
#      for y in range(0,1):  
        this_gear_num = this_gear_num +1              
#
        print
        print '    -----------------------------------------------------------------------------'
        print '    INV. GEAR ITEM NUMBER    = ', this_gear_num, ' out of ',num_toon_gear   
        print         

        u_in = 5                                      # specify number of units to extract
        b_in = 20                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]
# 
        print '      PROPERTY                    SERVER HEX  | SERVER DEC | CLIENT HEX | CLIENT DEC'
        print '      ---------------------------------------------------------------------------------'    
        print '      CURRENT STACK SIZE       = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
        print '      AVAILABLE HP             = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1]))        
        print '      REMAINING CHARGES        = ', '0x{:010X}'.format(s[2]), '{0:012d}'.format(s[2]), '0x{:010X}'.format(s2c(s[2])), '{0:012d}'.format(s2c(s[2]))         
        print '      CURRENT LOC. EQUIPPED    = ', '0x{:010X}'.format(s[3]), '{0:012d}'.format(s[3]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3])),  equipped_dict.get(s[3])
        print '      CURRENT LOCATION         = ', '0x{:010X}'.format(s[4]), '{0:012d}'.format(s[4]),  gear_location_dict.get(s[4])
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4           
        print '      ITEM NUMBER (INTEGER)    = ','0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0])  
        
        u_in = 23                                      # specify number of units to extract
        b_in = 100                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]        
#
        print '      ITEM ID?                 = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
        print '      ITEM FAMILY?             = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1]))        
        print '      UNKNOWN COL 9            = ', '0x{:010X}'.format(s[2]), '{0:012d}'.format(s[2]), '0x{:010X}'.format(s2c(s[2])), '{0:012d}'.format(s2c(s[2]))         
        print '      ITEM ICON ID             = ', '0x{:010X}'.format(s[3]), '{0:012d}'.format(s[3]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3]))
        print '      UNKNOWN COL 11           = ', '0x{:010X}'.format(s[4]), '{0:012d}'.format(s[4]), '0x{:010X}'.format(s2c(s[4])), '{0:012d}'.format(s2c(s[4]))        
        print '      SLOT TYPE                = ', '0x{:010X}'.format(s[5]), '{0:012d}'.format(s[5]), '0x{:010X}'.format(s2c(s[5])), '{0:012d}'.format(s2c(s[5])),slot_dict.get(s[5])
        print '      UNKNOWN COL 13           = ', '0x{:010X}'.format(s[6]), '{0:012d}'.format(s[6]), '0x{:010X}'.format(s2c(s[6])), '{0:012d}'.format(s2c(s[6]))        
        print '      TRADE TYPE               = ', '0x{:010X}'.format(s[7]), '{0:012d}'.format(s[7]), '0x{:010X}'.format(s2c(s[7])), '{0:012d}'.format(s2c(s[7])),trade_dict.get(s[7])     
        print '      RENT TYPE                = ', '0x{:010X}'.format(s[8]), '{0:012d}'.format(s[8]), '0x{:010X}'.format(s2c(s[8])), '{0:012d}'.format(s2c(s[8])),rent_dict.get(s[8])
        print '      UNKNOWN COL 16           = ', '0x{:010X}'.format(s[9]), '{0:012d}'.format(s[9]), '0x{:010X}'.format(s2c(s[9])), '{0:012d}'.format(s2c(s[9]))        

        print '      ATTACK TYPE              = ', '0x{:010X}'.format(s[10]), '{0:012d}'.format(s[10]), '0x{:010X}'.format(s2c(s[10])), '{0:012d}'.format(s2c(s[10])),attack_dict.get(s[10])
        print '      DAMAGE                   = ', '0x{:010X}'.format(s[11]), '{0:012d}'.format(s[11]), '0x{:010X}'.format(s2c(s[11])), '{0:012d}'.format(s2c(s[11]))        
        print '      UNKNOWN COL 19           = ', '0x{:010X}'.format(s[12]), '{0:012d}'.format(s[12]), '0x{:010X}'.format(s2c(s[12])), '{0:012d}'.format(s2c(s[12]))         
        print '      ITEM LEVEL               = ', '0x{:010X}'.format(s[13]), '{0:012d}'.format(s[13]), '0x{:010X}'.format(s2c(s[13])), '{0:012d}'.format(s2c(s[13]))
        print '      MAXIMUM STACK SIZE       = ', '0x{:010X}'.format(s[14]), '{0:012d}'.format(s[14]), '0x{:010X}'.format(s2c(s[14])), '{0:012d}'.format(s2c(s[14]))        
        print '      MAXIMUM HP               = ', '0x{:010X}'.format(s[15]), '{0:012d}'.format(s[15]), '0x{:010X}'.format(s2c(s[15])), '{0:012d}'.format(s2c(s[15]))
        print '      DURATION                 = ', '0x{:010X}'.format(s[16]), '{0:012d}'.format(s[16]), '0x{:010X}'.format(s2c(s[16])), '{0:012d}'.format(s2c(s[16]))        
        print '      CLASS EQUIPABLE          = ', '0x{:010X}'.format(s[17]), '{0:012d}'.format(s[17]), '0x{:010X}'.format(s2c(s[17])), '{0:012d}'.format(s2c(s[17]))#,class_equip_dict.get(s[17])     
        print '      RACE  EQUIPABLE          = ', '0x{:010X}'.format(s[18]), '{0:012d}'.format(s[18]), '0x{:010X}'.format(s2c(s[18])), '{0:012d}'.format(s2c(s[18]))#,race_equip_dict.get(s[18])
        print '      PROC TYPE                = ', '0x{:010X}'.format(s[19]), '{0:012d}'.format(s[19]), '0x{:010X}'.format(s2c(s[19])), '{0:012d}'.format(s2c(s[19])),proc_type_dict.get(s2c(s[19]))        
        print '      LORE TYPE                = ', '0x{:010X}'.format(s[20]), '{0:012d}'.format(s[20]), '0x{:010X}'.format(s2c(s[20])), '{0:012d}'.format(s2c(s[20])),lore_type_dict.get(s[20])     
        print '      UNKNOWN 28               = ', '0x{:010X}'.format(s[21]), '{0:012d}'.format(s[21]), '0x{:010X}'.format(s2c(s[21])), '{0:012d}'.format(s2c(s[21]))
        print '      UNKNOWN 29               = ', '0x{:010X}'.format(s[22]), '{0:012d}'.format(s[22]), '0x{:010X}'.format(s2c(s[22])), '{0:012d}'.format(s2c(s[22]))           
        print
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '      GEAR NAME LENGTH         = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        gear_name_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(gear_name_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+gear_name_length*2]) #  
        b = b + gear_name_length*2
        gear_name = s      
        print '      GEAR NAME                = ', "".join("{:s}".format(unichr(c)) for c in gear_name)   
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '      GEAR DESC. LENGTH        = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        gear_desc_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(gear_desc_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+gear_desc_length*2]) #  
        b = b + gear_desc_length*2
        gear_desc = s      
        print '      GEAR DESCRIPTION         = ', "".join("{:s}".format(unichr(c)) for c in gear_desc)   
#
        u_in = 1                                      # specify number of units to extract
        b_in = 3                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]   
#        
        num_stats = s2c(s[0])        
        print
        print '      NUMBER OF STATS          = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])) 
        this_stat_num = 0
        for y in range(0,num_stats): 
          this_stat_num = this_stat_num +1     
          u_in = 2                                      # specify number of units to extract
          b_in = 10                                     # number of bytes to read
          b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
          hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
          ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
          it = unit_extractor(u_in,b_in,ot)
          s  = it[2]
          b  = b+it[1]   
          print
          print '          STAT NAME            = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])),stat_dict.get(s[0])  
          print '          STAT INCREASE        = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1])), s2c(s[1])     
#
# Now read Gear sets
#
      if (is_frontiers==1):
        print        
        print '#######################################'
        print '#          GEAR WEAPON SETS           #'
        print '#######################################'     
        print
        num_gear_sets = 4
        this_gearset_num = 0
        for y in range(0,num_gear_sets):  
          this_gearset_num = this_gearset_num +1     
        
          print '      GEAR SET NUMBER = ', this_gearset_num, ' of ',num_gear_sets 
          u_in = 2                                      # specify number of units to extract
          b_in = 10                                     # number of bytes to read
          b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
          hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
          ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
          it = unit_extractor(u_in,b_in,ot)
          s  = it[2]
          b  = b+it[1] 
             
          print '      PROPERTY                    SERVER HEX  | SERVER DEC | CLIENT HEX | CLIENT DEC'
          print '      ---------------------------------------------------------------------------------'            
          print '        PRIMARY   HAND         = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))     
          print '        SECONDARY HAND         = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1]))    
          
          hdr_fmt1 = '<I'   # (little endian)
          s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
          b = b + 4            
          print '        WEAPON SET NAME LENGTH = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0])   
          wpn_desc_length = s[0]
          hdr_fmt1 = '<'+'{}'.format(wpn_desc_length)+'H'   # 
          s = struct.unpack(hdr_fmt1,message[b:b+wpn_desc_length*2]) #  
          b = b + wpn_desc_length*2
          wpn_desc = s      
          print '        WEAPON SET NAME        = ',  "".join("{:s}".format(unichr(c)) for c in wpn_desc)  
          print
#
      
      print '#######################################'
      print '#            GEAR IN BANK             #'
      print '#######################################'  
#        
      hdr_fmt1 = '<BI'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+5]) #  
      b = b + 5    
      print
      print '  GEAR NUMBER (BYTE)      = ', '0x{:02X}'.format(s2c(s[0])), '{0:02d}'.format(s2c(s[0]))     
      print '  GEAR NUMBER (INTEGER)   = ', '0x{:08X}'.format(s[1]), '{0:08d}'.format(s[1])    
      
      num_toon_gear = s2c(s[0])
        
      this_gear_num = 0
      for y in range(0,num_toon_gear):  
#      for y in range(0,1):  
        this_gear_num = this_gear_num +1              
#
        print
        print '    -----------------------------------------------------------------------------'
        print '    INV. GEAR ITEM NUMBER    = ', this_gear_num, ' out of ',num_toon_gear   
        print         

        u_in = 5                                      # specify number of units to extract
        b_in = 20                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]
# 
        print '      PROPERTY                    SERVER HEX  | SERVER DEC | CLIENT HEX | CLIENT DEC'
        print '      ---------------------------------------------------------------------------------'    
        print '      CURRENT STACK SIZE       = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
        print '      AVAILABLE HP             = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1]))        
        print '      REMAINING CHARGES        = ', '0x{:010X}'.format(s[2]), '{0:012d}'.format(s[2]), '0x{:010X}'.format(s2c(s[2])), '{0:012d}'.format(s2c(s[2]))         
        print '      CURRENT LOC. EQUIPPED    = ', '0x{:010X}'.format(s[3]), '{0:012d}'.format(s[3]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3])),  equipped_dict.get(s[3])
        print '      CURRENT LOCATION         = ', '0x{:010X}'.format(s[4]), '{0:012d}'.format(s[4]),  gear_location_dict.get(s[4])
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4           
        print '      ITEM NUMBER (INTEGER)    = ','0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0])  
        
        u_in = 23                                      # specify number of units to extract
        b_in = 100                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]        
#
        print '      ITEM ID?                 = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
        print '      ITEM FAMILY?             = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1]))        
        print '      UNKNOWN COL 9            = ', '0x{:010X}'.format(s[2]), '{0:012d}'.format(s[2]), '0x{:010X}'.format(s2c(s[2])), '{0:012d}'.format(s2c(s[2]))         
        print '      ITEM ICON ID             = ', '0x{:010X}'.format(s[3]), '{0:012d}'.format(s[3]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3]))
        print '      UNKNOWN COL 11           = ', '0x{:010X}'.format(s[4]), '{0:012d}'.format(s[4]), '0x{:010X}'.format(s2c(s[4])), '{0:012d}'.format(s2c(s[4]))        
        print '      SLOT TYPE                = ', '0x{:010X}'.format(s[5]), '{0:012d}'.format(s[5]), '0x{:010X}'.format(s2c(s[5])), '{0:012d}'.format(s2c(s[5])),slot_dict.get(s[5])
        print '      UNKNOWN COL 13           = ', '0x{:010X}'.format(s[6]), '{0:012d}'.format(s[6]), '0x{:010X}'.format(s2c(s[6])), '{0:012d}'.format(s2c(s[6]))        
        print '      TRADE TYPE               = ', '0x{:010X}'.format(s[7]), '{0:012d}'.format(s[7]), '0x{:010X}'.format(s2c(s[7])), '{0:012d}'.format(s2c(s[7])),trade_dict.get(s[7])     
        print '      RENT TYPE                = ', '0x{:010X}'.format(s[8]), '{0:012d}'.format(s[8]), '0x{:010X}'.format(s2c(s[8])), '{0:012d}'.format(s2c(s[8])),rent_dict.get(s[8])
        print '      UNKNOWN COL 16           = ', '0x{:010X}'.format(s[9]), '{0:012d}'.format(s[9]), '0x{:010X}'.format(s2c(s[9])), '{0:012d}'.format(s2c(s[9]))        

        print '      ATTACK TYPE              = ', '0x{:010X}'.format(s[10]), '{0:012d}'.format(s[10]), '0x{:010X}'.format(s2c(s[10])), '{0:012d}'.format(s2c(s[10])),attack_dict.get(s[10])
        print '      DAMAGE                   = ', '0x{:010X}'.format(s[11]), '{0:012d}'.format(s[11]), '0x{:010X}'.format(s2c(s[11])), '{0:012d}'.format(s2c(s[11]))        
        print '      UNKNOWN COL 19           = ', '0x{:010X}'.format(s[12]), '{0:012d}'.format(s[12]), '0x{:010X}'.format(s2c(s[12])), '{0:012d}'.format(s2c(s[12]))         
        print '      ITEM LEVEL               = ', '0x{:010X}'.format(s[13]), '{0:012d}'.format(s[13]), '0x{:010X}'.format(s2c(s[13])), '{0:012d}'.format(s2c(s[13]))
        print '      MAXIMUM STACK SIZE       = ', '0x{:010X}'.format(s[14]), '{0:012d}'.format(s[14]), '0x{:010X}'.format(s2c(s[14])), '{0:012d}'.format(s2c(s[14]))        
        print '      MAXIMUM HP               = ', '0x{:010X}'.format(s[15]), '{0:012d}'.format(s[15]), '0x{:010X}'.format(s2c(s[15])), '{0:012d}'.format(s2c(s[15]))
        print '      DURATION                 = ', '0x{:010X}'.format(s[16]), '{0:012d}'.format(s[16]), '0x{:010X}'.format(s2c(s[16])), '{0:012d}'.format(s2c(s[16]))        
        print '      CLASS EQUIPABLE          = ', '0x{:010X}'.format(s[17]), '{0:012d}'.format(s[17]), '0x{:010X}'.format(s2c(s[17])), '{0:012d}'.format(s2c(s[17]))#,class_equip_dict.get(s[17])     
        print '      RACE  EQUIPABLE          = ', '0x{:010X}'.format(s[18]), '{0:012d}'.format(s[18]), '0x{:010X}'.format(s2c(s[18])), '{0:012d}'.format(s2c(s[18]))#,race_equip_dict.get(s[18])
        print '      PROC TYPE                = ', '0x{:010X}'.format(s[19]), '{0:012d}'.format(s[19]), '0x{:010X}'.format(s2c(s[19])), '{0:012d}'.format(s2c(s[19])),proc_type_dict.get(s2c(s[19]))        
        print '      LORE TYPE                = ', '0x{:010X}'.format(s[20]), '{0:012d}'.format(s[20]), '0x{:010X}'.format(s2c(s[20])), '{0:012d}'.format(s2c(s[20])),lore_type_dict.get(s[20])     
        print '      UNKNOWN 28               = ', '0x{:010X}'.format(s[21]), '{0:012d}'.format(s[21]), '0x{:010X}'.format(s2c(s[21])), '{0:012d}'.format(s2c(s[21]))
        print '      UNKNOWN 29               = ', '0x{:010X}'.format(s[22]), '{0:012d}'.format(s[22]), '0x{:010X}'.format(s2c(s[22])), '{0:012d}'.format(s2c(s[22]))           
        print
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '      GEAR NAME LENGTH         = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        gear_name_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(gear_name_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+gear_name_length*2]) #  
        b = b + gear_name_length*2
        gear_name = s      
        print '      GEAR NAME                = ', "".join("{:s}".format(unichr(c)) for c in gear_name)   
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '      GEAR DESC. LENGTH        = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        gear_desc_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(gear_desc_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+gear_desc_length*2]) #  
        b = b + gear_desc_length*2
        gear_desc = s      
        print '      GEAR DESCRIPTION         = ', "".join("{:s}".format(unichr(c)) for c in gear_desc)   
#
        u_in = 1                                      # specify number of units to extract
        b_in = 3                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]   
#        
        num_stats = s2c(s[0])        
        print
        print '      NUMBER OF STATS          = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])) 
        this_stat_num = 0
        for y in range(0,num_stats): 
          this_stat_num = this_stat_num +1     
          u_in = 2                                      # specify number of units to extract
          b_in = 10                                     # number of bytes to read
          b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
          hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
          ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
          it = unit_extractor(u_in,b_in,ot)
          s  = it[2]
          b  = b+it[1]   
          print
          print '          STAT NAME            = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])),stat_dict.get(s[0])  
          print '          STAT INCREASE        = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1])), s2c(s[1])     
        
        
      print  
#
      print '#######################################'
      print '#       ITEMS BEING AUCTIONED         #'
      print '#######################################'  
#        
      hdr_fmt1 = '<B'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+1]) #  
      b = b + 1    
      print
      print '  ITEMS BEING AUCTIONED      = ', '0x{:02X}'.format(s2c(s[0])), '{0:02d}'.format(s2c(s[0]))     
      
      num_item_auction = s2c(s[0])      

      this_auction_item = 0
      for y in range(0,num_item_auction):  
#      for y in range(0,1):  
        this_auction_item = this_auction_item +1              
#
        print
        print '    -----------------------------------------------------------------------------'
        print '    AUCTION ITEM NUMBER      = ', this_auction_item, ' out of ',num_item_auction   
        print         

        u_in = 7                                      # specify number of units to extract
        b_in = 28                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]
        
        print '      PROPERTY                    SERVER HEX  | SERVER DEC | CLIENT HEX | CLIENT DEC'
        print '      ---------------------------------------------------------------------------------'    
        print '      GLOBAL AUCTION NUMBER    = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
        print '      UNKNOWN FLAG             = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1]))        
        print '      USED HP                  = ', '0x{:010X}'.format(s[2]), '{0:012d}'.format(s[2]), '0x{:010X}'.format(s2c(s[2])), '{0:012d}'.format(s2c(s[2]))         
        print '      REMAINING CHARGES        = ', '0x{:010X}'.format(s[3]), '{0:012d}'.format(s[3]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3]))
        print '      CURRENT BID              = ', '0x{:010X}'.format(s[4]), '{0:012d}'.format(s[4]), '0x{:010X}'.format(s2c(s[4])), '{0:012d}'.format(s2c(s[4]))
        print '      MAX BID                  = ', '0x{:010X}'.format(s[5]), '{0:012d}'.format(s[5]), '0x{:010X}'.format(s2c(s[5])), '{0:012d}'.format(s2c(s[5]))         
        print '      TIME LEFT IN AUCTION (S) = ', '0x{:010X}'.format(s[6]), '{0:012d}'.format(s[6]), '0x{:010X}'.format(s2c(s[6])), '{0:012d}'.format(s2c(s[6]))
        print 
        print '      TIME LEFT IN AUCTION (SECONDS) = ',s2c(s[6])
        print '      TIME LEFT IN AUCTION (MINUTES) = ',s2c(s[6])/60
        print '      TIME LEFT IN AUCTION (HOURS)   = ',s2c(s[6])/60/60
        print '      TIME LEFT IN AUCTION (DAYS)    = ',s2c(s[6])/60/60/24 
        print    
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4           
        print '      PLAYER PLACEHOLDERR (INTEGER)  = ','0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0])   
        print
        
        u_in = 23                                      # specify number of units to extract
        b_in = 100                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]        
#
        print '      ITEM ID?                 = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
        print '      ITEM FAMILY?             = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1]))        
        print '      UNKNOWN COL 9            = ', '0x{:010X}'.format(s[2]), '{0:012d}'.format(s[2]), '0x{:010X}'.format(s2c(s[2])), '{0:012d}'.format(s2c(s[2]))         
        print '      ITEM ICON ID             = ', '0x{:010X}'.format(s[3]), '{0:012d}'.format(s[3]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3]))
        print '      UNKNOWN COL 11           = ', '0x{:010X}'.format(s[4]), '{0:012d}'.format(s[4]), '0x{:010X}'.format(s2c(s[4])), '{0:012d}'.format(s2c(s[4]))        
        print '      SLOT TYPE                = ', '0x{:010X}'.format(s[5]), '{0:012d}'.format(s[5]), '0x{:010X}'.format(s2c(s[5])), '{0:012d}'.format(s2c(s[5])),slot_dict.get(s[5])
        print '      UNKNOWN COL 13           = ', '0x{:010X}'.format(s[6]), '{0:012d}'.format(s[6]), '0x{:010X}'.format(s2c(s[6])), '{0:012d}'.format(s2c(s[6]))        
        print '      TRADE TYPE               = ', '0x{:010X}'.format(s[7]), '{0:012d}'.format(s[7]), '0x{:010X}'.format(s2c(s[7])), '{0:012d}'.format(s2c(s[7])),trade_dict.get(s[7])     
        print '      RENT TYPE                = ', '0x{:010X}'.format(s[8]), '{0:012d}'.format(s[8]), '0x{:010X}'.format(s2c(s[8])), '{0:012d}'.format(s2c(s[8])),rent_dict.get(s[8])
        print '      UNKNOWN COL 16           = ', '0x{:010X}'.format(s[9]), '{0:012d}'.format(s[9]), '0x{:010X}'.format(s2c(s[9])), '{0:012d}'.format(s2c(s[9]))        

        print '      ATTACK TYPE              = ', '0x{:010X}'.format(s[10]), '{0:012d}'.format(s[10]), '0x{:010X}'.format(s2c(s[10])), '{0:012d}'.format(s2c(s[10])),attack_dict.get(s[10])
        print '      DAMAGE                   = ', '0x{:010X}'.format(s[11]), '{0:012d}'.format(s[11]), '0x{:010X}'.format(s2c(s[11])), '{0:012d}'.format(s2c(s[11]))        
        print '      UNKNOWN COL 19           = ', '0x{:010X}'.format(s[12]), '{0:012d}'.format(s[12]), '0x{:010X}'.format(s2c(s[12])), '{0:012d}'.format(s2c(s[12]))         
        print '      ITEM LEVEL               = ', '0x{:010X}'.format(s[13]), '{0:012d}'.format(s[13]), '0x{:010X}'.format(s2c(s[13])), '{0:012d}'.format(s2c(s[13]))
        print '      MAXIMUM STACK SIZE       = ', '0x{:010X}'.format(s[14]), '{0:012d}'.format(s[14]), '0x{:010X}'.format(s2c(s[14])), '{0:012d}'.format(s2c(s[14]))        
        print '      MAXIMUM HP               = ', '0x{:010X}'.format(s[15]), '{0:012d}'.format(s[15]), '0x{:010X}'.format(s2c(s[15])), '{0:012d}'.format(s2c(s[15]))
        print '      DURATION                 = ', '0x{:010X}'.format(s[16]), '{0:012d}'.format(s[16]), '0x{:010X}'.format(s2c(s[16])), '{0:012d}'.format(s2c(s[16]))        
        print '      CLASS EQUIPABLE          = ', '0x{:010X}'.format(s[17]), '{0:012d}'.format(s[17]), '0x{:010X}'.format(s2c(s[17])), '{0:012d}'.format(s2c(s[17]))#,class_equip_dict.get(s[17])     
        print '      RACE  EQUIPABLE          = ', '0x{:010X}'.format(s[18]), '{0:012d}'.format(s[18]), '0x{:010X}'.format(s2c(s[18])), '{0:012d}'.format(s2c(s[18]))#,race_equip_dict.get(s[18])
        print '      PROC TYPE                = ', '0x{:010X}'.format(s[19]), '{0:012d}'.format(s[19]), '0x{:010X}'.format(s2c(s[19])), '{0:012d}'.format(s2c(s[19])),proc_type_dict.get(s2c(s[19]))        
        print '      LORE TYPE                = ', '0x{:010X}'.format(s[20]), '{0:012d}'.format(s[20]), '0x{:010X}'.format(s2c(s[20])), '{0:012d}'.format(s2c(s[20])),lore_type_dict.get(s[20])     
        print '      UNKNOWN 28               = ', '0x{:010X}'.format(s[21]), '{0:012d}'.format(s[21]), '0x{:010X}'.format(s2c(s[21])), '{0:012d}'.format(s2c(s[21]))
        print '      UNKNOWN 29               = ', '0x{:010X}'.format(s[22]), '{0:012d}'.format(s[22]), '0x{:010X}'.format(s2c(s[22])), '{0:012d}'.format(s2c(s[22]))           
        print
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '      GEAR NAME LENGTH         = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        gear_name_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(gear_name_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+gear_name_length*2]) #  
        b = b + gear_name_length*2
        gear_name = s      
        print '      GEAR NAME                = ', "".join("{:s}".format(unichr(c)) for c in gear_name)   
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '      GEAR DESC. LENGTH        = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        gear_desc_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(gear_desc_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+gear_desc_length*2]) #  
        b = b + gear_desc_length*2
        gear_desc = s      
        print '      GEAR DESCRIPTION         = ', "".join("{:s}".format(unichr(c)) for c in gear_desc)   
#
        u_in = 1                                      # specify number of units to extract
        b_in = 3                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]   
#        
        num_stats = s2c(s[0])        
        print
        print '      NUMBER OF STATS          = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])) 
        this_stat_num = 0
        for y in range(0,num_stats): 
          this_stat_num = this_stat_num +1     
          u_in = 2                                      # specify number of units to extract
          b_in = 10                                     # number of bytes to read
          b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
          hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
          ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
          it = unit_extractor(u_in,b_in,ot)
          s  = it[2]
          b  = b+it[1]   
          print
          print '          STAT NAME            = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])),stat_dict.get(s[0])  
          print '          STAT INCREASE        = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1])), s2c(s[1])     

      print    
      print '#######################################'
      print '#    ITEMS BEING BIDDING ON           #'
      print '#######################################'  
#        
      hdr_fmt1 = '<B'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+1]) #  
      b = b + 1    
      print
      print '  ITEMS BEING BID ON         = ', '0x{:02X}'.format(s2c(s[0])), '{0:02d}'.format(s2c(s[0]))     

      num_item_bid = s2c(s[0])      
      this_bid_item = 0
      for y in range(0,num_item_bid):  
#      for y in range(0,1):  
        this_bid_item = this_bid_item +1              
#
        print
        print '    -----------------------------------------------------------------------------'
        print '    BID ITEM NUMBER          = ', this_bid_item, ' out of ',num_item_bid   
        print         

        u_in = 7                                      # specify number of units to extract
        b_in = 24                                     # number of bytes to read
        b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'          # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]
        
        print '      PROPERTY                    SERVER HEX  | SERVER DEC | CLIENT HEX | CLIENT DEC'
        print '      ---------------------------------------------------------------------------------'    
        print '      CURRENT WINNER (TOON ID) = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
        print '      GLOBAL AUCTION ID        = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1]))        
        print '      CURRENT WINNING BID      = ', '0x{:010X}'.format(s[2]), '{0:012d}'.format(s[2]), '0x{:010X}'.format(s2c(s[2])), '{0:012d}'.format(s2c(s[2]))         
        print '      UNKNOWN FLAG             = ', '0x{:010X}'.format(s[3]), '{0:012d}'.format(s[3]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3]))
        print '      TOONS MAX PROXY BID      = ', '0x{:010X}'.format(s[4]), '{0:012d}'.format(s[4]), '0x{:010X}'.format(s2c(s[4])), '{0:012d}'.format(s2c(s[4]))
        print '      UNKNOWN FLAG             = ', '0x{:010X}'.format(s[5]), '{0:012d}'.format(s[5]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3]))

         
 
        if s[6] != 0:
          print '      ITEM ID?                 = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
        else:
          b = b-1
#          
      u_in = 1                                        # specify number of units to extract
      b_in = 4                                        # number of bytes to read
      b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
      hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
      ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
      it = unit_extractor(u_in,b_in,ot)
      s  = it[2]
      b  = b+it[1]         
        
      print      
      print '      END OF BID FLAG          = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
         
         
         
      print    
      print '#######################################'
      print '#       SPELLS/ABILITIES              #'
      print '#######################################'  
#        
      hotbar_dict       = {0x00:'BAR 1, SLOT 1',0x01:'NOT SHOWN ON HOTBAR',0x02:'BAR 1, SLOT 2',0x04:'BAR 1, SLOT 3',0x06:'BAR 1, SLOT 4',0x08:'BAR 1, SLOT 5',\
                           0x0a:'BAR 2, SLOT 1',0x0c:'BAR 2, SLOT 2',0x0e:'BAR 2, SLOT 3',0x10:'BAR 2, SLOT 4',0x12:'BAR 2, SLOT 5'}                                      
      on_ability_bar_dict = {0x00:'NOT SHOWN ON HOTBAR',0x02:'SHOWN ON HOTBAR'}
      ability_type_dict   = {0x00:'ABILITY REPLACED BY UPGRADE ',0x02:'CM/BOUGHT ABILITY'}
      range_dict          = {0x0000:'RANGE 0.0',0x3F80:'RANGE 1.0',0x40A0:'RANGE 5.0',0x4120:'RANGE 10.0',0x4170:'RANGE 5.0',0x41A0:'RANGE 20.0',0x41F0:'RANGE 30.0'}      
      scope_dict          = {0x00:'SELF ',0x02:'TARGET', 0x04:'GROUP',0x06:'PET',0x08:'CORPSE?'}
      icon_back_color_dict= {0x03D0A8BDBE:'RED BACKGROUND',0x0DBDA7AB8B:'YELLOW BACKGROUND',0x03A3BF9AB4:'SILVER BACKGROUND',0x04B5E7F9D4:'PURPLE BACKGROUND',0x0BE38B8CBD:'GREEN BACKGROUND'}
      required_equip_dict = {0x03FE:'NONE/ALL',0x7E:'1HS/2HS/1HB/2HB/1HP/2HP'}
      

      u_in = 1                                        # specify number of units to extract
      b_in = 5                                        # number of bytes to read
      b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
      hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
      ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
      it = unit_extractor(u_in,b_in,ot)
      s  = it[2]
      b  = b+it[1]   
      
      print
      print '  SPELLS/ABILITIES OWNED     = ', '0x{:02X}'.format(s2c(s[0])), '{0:02d}'.format(s2c(s[0]))     

      num_abilities = s2c(s[0])      
      this_ability = 0
      for y in range(0,num_abilities):  
#      for y in range(0,1):  
        this_ability = this_ability +1      

        u_in = 9                                        # specify number of units to extract
        b_in = 50                                       # number of bytes to read
        b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]   

        print
        print '    -----------------------------------------------------------------------------'
        print '    SPELL/ABILITY NUMBER     = ', this_ability, ' out of ',num_abilities   
        print         

        print '      PROPERTY                    SERVER HEX  | SERVER DEC | CLIENT HEX | CLIENT DEC'
        print '      ---------------------------------------------------------------------------------'    
        print '      SPELL ID                 = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
        print '      UNKNOWN                  = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1]))
        print '      SHOW ON ABILITY HOTBAR   = ', '0x{:010X}'.format(s[2]), '{0:012d}'.format(s[2]), '0x{:010X}'.format(s2c(s[2])), '{0:012d}'.format(s2c(s[2])),on_ability_bar_dict.get(s[2])         
        print '      LOCATION ON ABILITY BAR  = ', '0x{:010X}'.format(s[3]), '{0:012d}'.format(s[3]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3])),hotbar_dict.get(s[3])          
        print '      UNKNOWN                  = ', '0x{:010X}'.format(s[4]), '{0:012d}'.format(s[4]), '0x{:010X}'.format(s2c(s[4])), '{0:012d}'.format(s2c(s[4]))
        print '      ABILITY TYPE             = ', '0x{:010X}'.format(s[5]), '{0:012d}'.format(s[5]), '0x{:010X}'.format(s2c(s[5])), '{0:012d}'.format(s2c(s[5])),ability_type_dict.get(s[5])
        print '      SPELL LEVEL              = ', '0x{:010X}'.format(s[6]), '{0:012d}'.format(s[6]), '0x{:010X}'.format(s2c(s[6])), '{0:012d}'.format(s2c(s[6])), 'LEVEL ',s2c(s[6])
        print '      UNKNOWN                  = ', '0x{:010X}'.format(s[7]), '{0:012d}'.format(s[7]), '0x{:010X}'.format(s2c(s[7])), '{0:012d}'.format(s2c(s[7]))        
        print '      UNKNOWN                  = ', '0x{:010X}'.format(s[8]), '{0:012d}'.format(s[8]), '0x{:010X}'.format(s2c(s[8])), '{0:012d}'.format(s2c(s[8]))         
#
        hdr_fmt1 = '<H'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+2]) #  
        b = b + 2    
        print '      ABILITY RANGE            = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])), range_dict.get(s[0])
#        
        u_in = 7                                        # specify number of units to extract
        b_in = 30                                       # number of bytes to read
        b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]          
        
        print '      CAST TIME                = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])), s2c(s[0]), ' SECONDS'
        print '      POWER REQUIRED           = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1])), s2c(s[1]), ' POWER'
        print '      ICON COLOR               = ', '0x{:010X}'.format(s[2]), '{0:012d}'.format(s[2]), '0x{:010X}'.format(s2c(s[2])), '{0:012d}'.format(s2c(s[2])),icon_back_color_dict.get(s[2])
        print '      ICON ID                  = ', '0x{:010X}'.format(s[3]), '{0:012d}'.format(s[3]), '0x{:010X}'.format(s2c(s[3])), '{0:012d}'.format(s2c(s[3]))        
        print '      SCOPE                    = ', '0x{:010X}'.format(s[4]), '{0:012d}'.format(s[4]), '0x{:010X}'.format(s2c(s[4])), '{0:012d}'.format(s2c(s[4])), scope_dict.get(s[4])        
        print '      RECAST                   = ', '0x{:010X}'.format(s[5]), '{0:012d}'.format(s[5]), '0x{:010X}'.format(s2c(s[5])), '{0:012d}'.format(s2c(s[5])), s2c(s[5]), ' SECONDS'
        print '      EQUIPMENT REQUIRED       = ', '0x{:010X}'.format(s[6]), '{0:012d}'.format(s[6]), '0x{:010X}'.format(s2c(s[6])), '{0:012d}'.format(s2c(s[6])),required_equip_dict.get(s[6])
        print
        
        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '      ABILITY NAME LENGTH      = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        ability_name_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(ability_name_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+ability_name_length*2]) #  
        b = b + ability_name_length*2
        ability_name = s      
        print '      ABILITY NAME             = ', "".join("{:s}".format(unichr(c)) for c in ability_name)   

        hdr_fmt1 = '<I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
        b = b + 4            
        print '      ABILITY DESC. LENGTH     = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
        ability_desc_length = s[0]
        hdr_fmt1 = '<'+'{}'.format(ability_desc_length)+'H'   # 
        s = struct.unpack(hdr_fmt1,message[b:b+ability_desc_length*2]) #  
        b = b + ability_desc_length*2
        ability_desc = s      
        print '      ABILITY DESCRIPTION      = ', "".join("{:s}".format(unichr(c)) for c in ability_desc) 
        print
        u_in = 1                                        # specify number of units to extract
        b_in = 4                                        # number of bytes to read
        b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
        hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
        ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
        it = unit_extractor(u_in,b_in,ot)
        s  = it[2]
        b  = b+it[1]                   
     
        if s[0] ==0:   
          print '      END OF ABILITY FLAG      = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
          print
        else:
          print '      SPELL USE REQUIRES ITEMS = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0]))
          u_in = 4                                        # specify number of units to extract
          b_in = 16                                       # number of bytes to read
          b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
          hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
          ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
          it = unit_extractor(u_in,b_in,ot)
          s  = it[2]
          b  = b+it[1]
          k  = s          
          print '      NUMBER OF ITEMS TOON HAS = ', '0x{:010X}'.format(s[0]), '{0:012d}'.format(s[0]), '0x{:010X}'.format(s2c(s[0])), '{0:012d}'.format(s2c(s[0])),s2c(s[0]) 
          print '      NUMBER REQUIRED TO USE   = ', '0x{:010X}'.format(s[1]), '{0:012d}'.format(s[1]), '0x{:010X}'.format(s2c(s[1])), '{0:012d}'.format(s2c(s[1])),s2c(s[1])
#
          hdr_fmt1 = '<I'   # (little endian)
          s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
          b = b + 4            
          print '      REQUIREMENT DESC. LENGTH = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])   
          require_desc_length = s[0]
          hdr_fmt1 = '<'+'{}'.format(require_desc_length)+'H'   # 
          s = struct.unpack(hdr_fmt1,message[b:b+require_desc_length*2]) #  
          b = b + require_desc_length*2
          require_desc = s      
          print '      REQUIRED ITEM            = ', "".join("{:s}".format(unichr(c)) for c in require_desc) 
          print
          print
          print '      END OF REQUIREMENTS      = ', '0x{:010X}'.format(k[2]), '{0:012d}'.format(k[2]), '0x{:010X}'.format(s2c(k[2])), '{0:012d}'.format(s2c(k[2]))
          print  
          print '      END OF ABILITY FLAG      = ', '0x{:010X}'.format(k[3]), '{0:012d}'.format(k[3]), '0x{:010X}'.format(s2c(k[3])), '{0:012d}'.format(s2c(k[3]))

      print    
      print '#######################################'
      print '#       ADDITIONAL INFORMATION        #'
      print '#######################################'  
      print
      hdr_fmt1 = '<2I'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+8]) #  
      b = b + 8            
      print '      BASE MOVEMENT RATE       = ', '0x{:08X}'.format(s[0]),hex2float('{:8X}'.format(s[0]))   
      print '      UNKNOWN - VALUE          = ', '0x{:08X}'.format(s[1]), '{0:08d}'.format(s[1])   

      num_data_packs = 6      
      this_data_pack = 0
      for y in range(0,num_data_packs):  
#      for y in range(0,1):  
        this_data_pack = this_data_pack +1   
        
        print '    --------------------------------------------------'
        print '    DATA PACK NUMBER         = ', this_data_pack, ' out of ',num_data_packs   
        
        hdr_fmt1 = '<4I'   # (little endian)
        s = struct.unpack(hdr_fmt1,message[b:b+16]) #  
        b = b + 16            
        print
        print '      DATA PACK ID             = ', '0x{:08X}'.format(s[0]), '{0:08d}'.format(s[0])
        print '      DATA PACK KEY1           = ', '0x{:08X}'.format(s[1]), '{0:08d}'.format(s[1])   
        print '      DATA PACK KEY2           = ', '0x{:08X}'.format(s[2]), '{0:08d}'.format(s[2])   
        print '      DATA PACK VALUE          = ', '0x{:08X}'.format(s[3]), '{0:08d}'.format(s2c(s[3]))
        print         
        
      hdr_fmt1 = '<I'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
      b = b + 4            
      print '    --------------------------------------------------'
      print '     CLASS MASTERY DATA PACK  = ', '0x{:08X}'.format(s[0]),'{0:08d}'.format(s[0])  
      print
      
      u_in = 4                                        # specify number of units to extract
      b_in = 20                                        # number of bytes to read
      b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
      hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
      ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
      it = unit_extractor(u_in,b_in,ot)
      s  = it[2]
      b  = b+it[1]   
      
      print '      UNSPENT CM POINTS?       = ', '0x{:08X}'.format(s[0]),'{0:08d}'.format(s2c(s[0]))  
      print '      SPENT CLASS MASTERY PTS  = ', '0x{:08X}'.format(s[1]),'{0:08d}'.format(s2c(s[1]))  
      print '      XP TILL NEXT CM POINT    = ', '0x{:08X}'.format(s[2]),'{0:08d}'.format(s2c(s[2]))  
      print '      UNKNOWN CM VALUE2        = ', '0x{:08X}'.format(s[3]),'{0:08d}'.format(s2c(s[3]))  
      print
 
      hdr_fmt1 = '<I'   # (little endian)
      s = struct.unpack(hdr_fmt1,message[b:b+4]) #  
      b = b + 4            

      print '      BASE MOVEMENT RATE       = ', '0x{:08X}'.format(s[0]),hex2float('{:8X}'.format(s[0]))   
      print
#      
      u_in = 36                                       # specify number of units to extract
      b_in = 160                                      # number of bytes to read
      b_in = min(b_in,count(message[b:]))             # make sure we don't try to read longer than the message
      hdr_fmt1 = '<'+'{}'.format(b_in)+'B'            # (little endian)     
      ot  = struct.unpack(hdr_fmt1,message[b:b+b_in]) #    
      it = unit_extractor(u_in,b_in,ot)
      s  = it[2]
      b  = b+it[1]    
 
      print '      STR: ADDED WITH CMS      = ', '0x{:08X}'.format(s[0]),'{0:08d}'.format(s2c(s[0]))  
      print '      STA: ADDED WITH CMS      = ', '0x{:08X}'.format(s[1]),'{0:08d}'.format(s2c(s[1]))  
      print '      AGI: ADDED WITH CMS      = ', '0x{:08X}'.format(s[2]),'{0:08d}'.format(s2c(s[2]))  
      print '      DEX: ADDED WITH CMS      = ', '0x{:08X}'.format(s[3]),'{0:08d}'.format(s2c(s[3]))  
      print '      WIS: ADDED WITH CMS      = ', '0x{:08X}'.format(s[4]),'{0:08d}'.format(s2c(s[4]))  
      print '      INT: ADDED WITH CMS      = ', '0x{:08X}'.format(s[5]),'{0:08d}'.format(s2c(s[5])) 
      print '      CHA: ADDED WITH CMS      = ', '0x{:08X}'.format(s[6]),'{0:08d}'.format(s2c(s[6]))  
      print
      print '      STR: ADDED TO MAX        = ', '0x{:08X}'.format(s[7]),'{0:08d}'.format(s2c(s[7])),   base_max_stat+s2c(s[7])
      print '      STA: ADDED TO MAX        = ', '0x{:08X}'.format(s[8]),'{0:08d}'.format(s2c(s[8])),   base_max_stat+s2c(s[8])  
      print '      AGI: ADDED TO MAX        = ', '0x{:08X}'.format(s[9]),'{0:08d}'.format(s2c(s[9])),   base_max_stat+s2c(s[9])  
      print '      DEX: ADDED TO MAX        = ', '0x{:08X}'.format(s[10]),'{0:08d}'.format(s2c(s[10])),   base_max_stat+s2c(s[10])  
      print '      WIS: ADDED TO MAX        = ', '0x{:08X}'.format(s[11]),'{0:08d}'.format(s2c(s[11])),   base_max_stat+s2c(s[11])  
      print '      INT: ADDED TO MAX        = ', '0x{:08X}'.format(s[12]),'{0:08d}'.format(s2c(s[12])),   base_max_stat+s2c(s[12])  
      print '      CHA: ADDED TO MAX        = ', '0x{:08X}'.format(s[13]),'{0:08d}'.format(s2c(s[13])),   base_max_stat+s2c(s[13])  
      print
      print '      HIT POINT MAX            = ', '0x{:08X}'.format(s[14]),'{0:08d}'.format(s2c(s[14]))  
      print '      POWER MAX?               = ', '0x{:08X}'.format(s[15]),'{0:08d}'.format(s2c(s[15])) 
      print '      HEAL OVER TIME (HoT)     = ', '0x{:08X}'.format(s[16]),'{0:08d}'.format(s2c(s[16]))  
      print '      POWER OVER TIME (PoT)    = ', '0x{:08X}'.format(s[17]),'{0:08d}'.format(s2c(s[17]))  
      print '      DEFENSIVE MOD            = ', '0x{:08X}'.format(s[18]),'{0:08d}'.format(s2c(s[18]))  
      print '      OFFENSIVE MOD            = ', '0x{:08X}'.format(s[19]),'{0:08d}'.format(s2c(s[19]))   
      print '      AC ADDED?                = ', '0x{:08X}'.format(s[20]),'{0:08d}'.format(s2c(s[20]))  
      print '      HIT POINT FACTOR         = ', '0x{:08X}'.format(s[21]),'{0:08d}'.format(s2c(s[21]))
      print      
      print '      FR: ADDED                = ', '0x{:08X}'.format(s[22]),'{0:08d}'.format(s2c(s[22]))  
      print '      LR: ADDED?               = ', '0x{:08X}'.format(s[23]),'{0:08d}'.format(s2c(s[23]))  
      print '      CR: ADDED                = ', '0x{:08X}'.format(s[24]),'{0:08d}'.format(s2c(s[24]))  
      print '      AR: ADDED?               = ', '0x{:08X}'.format(s[25]),'{0:08d}'.format(s2c(s[25])) 
      print '      PR: ADDED?               = ', '0x{:08X}'.format(s[26]),'{0:08d}'.format(s2c(s[26]))  
      print '      DR: ADDED?               = ', '0x{:08X}'.format(s[27]),'{0:08d}'.format(s2c(s[27])) 
      print      
      print '      VALUE 28                 = ', '0x{:08X}'.format(s[28]),'{0:08d}'.format(s2c(s[28]))  
      print
      print '      STR: ADDED TO MAX 2      = ', '0x{:08X}'.format(s[29]),'{0:08d}'.format(s2c(s[29])),   base_max_stat+s2c(s[29])
      print '      STA: ADDED TO MAX 2      = ', '0x{:08X}'.format(s[30]),'{0:08d}'.format(s2c(s[30])),   base_max_stat+s2c(s[30])  
      print '      AGI: ADDED TO MAX 2      = ', '0x{:08X}'.format(s[31]),'{0:08d}'.format(s2c(s[31])),   base_max_stat+s2c(s[31])  
      print '      DEX: ADDED TO MAX 2      = ', '0x{:08X}'.format(s[32]),'{0:08d}'.format(s2c(s[32])),   base_max_stat+s2c(s[32])  
      print '      WIS: ADDED TO MAX 2      = ', '0x{:08X}'.format(s[33]),'{0:08d}'.format(s2c(s[33])),   base_max_stat+s2c(s[33])  
      print '      INT: ADDED TO MAX 2      = ', '0x{:08X}'.format(s[34]),'{0:08d}'.format(s2c(s[34])),   base_max_stat+s2c(s[34])  
      print '      CHA: ADDED TO MAX 2      = ', '0x{:08X}'.format(s[35]),'{0:08d}'.format(s2c(s[35])),   base_max_stat+s2c(s[35])   
      
##########################      
#
# Just prints out next bytes to be processed
#  
    
      print
      print
      b_in = 120                                     # number of bytes to read
      b_in = min(b_in,count(message[b:]))           # make sure we don't try to read longer than the message      
      print "UPCOMING: "," ".join("{:02X}".format(ord(c)) for c in message[b:b+b_in])
      print
  else:  
    print
    
  
  print 
  return
#
#
#
class ColorStuffVanilla:
   'Common base class for representing Colors'            
   def __init__(self, index):

   #
   # These are not correct at the moment
   #
      vanilla_color_dict   = {0x00:'#Default',\
                              0x01:'#Blue',\
                              0x02:'#Brown',\
                              0x03:'#Cobalt',\
                              0x04:'#Green',\
                              0x05:'#Orange',\
                              0x06:'#Puce',\
                              0x07:'#Red',\
                              0x08:'#Sky',\
                              0x09:'#Steel',\
                              0x0a:'#Tan',\
                              0x0b:'#Teal',\
                              0x0c:'#Wine',\
                              0x0d:'#Yellow',\
                              0x0e:'#Black',\
                              0x0f:'#Unknown'}
#                               
      self.index = index
      self.name   = vanilla_color_dict.get(self.index)
#
#
#
class ColorStuff:
   'Common base class for representing Colors'            
   def __init__(self, ColorRep):
 
# http://www.htmlcsscolor.com/hex/203964
# 
      color_dict   = {0xFFFFFF:'Default',0x5C5C5C:'Vanilla Black',0x0000FF:'Vanilla Blue',0x966432:'Vanilla Brown',0x400080:'Vanilla Cobalt', \
                   0x008000:'Vanilla Green',0xFF8000:'Vanilla Orange',0x804040:'Vanilla Puce',0xF03C3C:'Vanilla Red',0xA5F5FF:'Vanilla Sky', \
                   0x808080:'Vanilla Steel',0x804040:'Vanilla Tan',0xFF00B4:'Vanilla Teal',0x96003C:'Vanilla Wine',0xF0F000:'Vanilla Yellow', \
                   0x203964:'Cataline Blue',0x800080:'Purple',0x3F7694:'Jelly Bean Blue',0x750075:'Purple 2',0x444488:'Jacksons Purple',\
                   0xFF0000:'Red',0x506400:'Verdun Green',0x004040:'Cyprus Blue/Green',0x1BED59:'Malachite (Bright Green)',\
                   0x7979BD:'Moody Blue (Purple)',0x2D843C:'Japanese Laurel (Green)',0x9E9C38:'Highball (Tanish)',0x80FFD6:'Aquamarine',\
                   0x009F9F:'Persian Green',0x004182:'Dark Cerulean (Blue)',0x0080C0:'Cerulean (Blue)',0x150000:'Black',\
                   0x316262:'Oracle (Green)',0x620000:'Maroon',0xA0AAB4:'Grey Chateau',0x5A5A5A:'Zambezi (Grey)',0x0000A0:'New Midnight Blue',\
                   0x00B4FF:'Deep Sky Blue (Light Blue)',0x00C832:'Dark Pastel Green (Light Green)',0xDCBE96:'Pancho (Sand)',\
                   0x00C8B4:'Robins Egg Blue (Teal)'}                
#                               
#      
      self.RE     = ColorRep&0xFF
      self.GR     = ColorRep>>8&0xFF
      self.BL     = ColorRep>>16&0xFF
      self.OP     = ColorRep>>24&0xFF
      self.hexrep = int(self.RE<<16) + int(self.GR<<8)+ int(self.BL)
      self.name   = color_dict.get(self.hexrep)
      # print '0x{:02X}'.format(self.RE)
      # print '0x{:02X}'.format(self.GR)
      # print '0x{:02X}'.format(self.BL)
      # print '0x{:08X}'.format(self.hexrep)
      # print '0x{:08X}'.format(ColorRep&0x00FFFFFF)
      # print color_dict.get(self.hexrep)
      # print     
#
#######################################################################################################
#


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
#
#
def s2c_tester(value): # server to client representation
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
  print bitflip, bt,s, len(bt)
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
  print
  print fn,bt
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
  return sumbt
#
def s2c(value): # server to client representation - original
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
  forneg     = 0 # This is value that might be used if bitflip is set
  shift      = 0
  fshift     = 0
  sum        = 0
  num_bytes = len(bt)
 #
  for y in range(num_bytes-1,-1,-1):  # 
     if bt[y] > 0x80:
       bt[y] = bt[y] - 0x80
#    print y,bt[y]  
#     
     forneg = int(forneg<<8)+0xFF # this is used if this is a bit flip. Creates a FFFF size of input
#       
     bt[y] = bt[y]<<shift
     shift = shift+8                 # keeps track of how many 
     sum = sum + int(bt[y]>>fshift)
     fshift = fshift +1
  
  sum = sum >>1  # drops the lowest bit for all

# sum and forneg main values updated. 
#  print "INPUT  - ",'0x{:X}'.format(value)
#  print "INPUT  -  0x"+ty.format(value)
#  print "FORNEG - ",'0x{:X}'.format(forneg), num_bytes
#  print "SUM    - ",'0x{:X}'.format(sum)
#  print "FLIPBIT- ",bitflip

  if bitflip == 1:
    # remove some bits from the top, then flip 
    sum=forneg&sum   # need this for icons to be correct 
    sum = ~sum  # this is a bitwize invert
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
#

#
#
##############################################
#  
def Process_Standard_Message(message,message_length):
#
  global continued_message
  global continued_message_string
  global message_in_progress
  global continued_message_length
#
  hdr_fmt1 = '<H'   # (little endian)[]
  s = struct.unpack(hdr_fmt1,message[0:2]) #  
  message_number = s[0]
  my_message = message[2:message_length+2]
#  
  print '    MESSAGE NUMB :','0x{:04X}'.format(message_number),'{0:08d}'.format(message_number)
  print '    OPCODE MESSG :'," ".join("{:02X}".format(ord(c)) for c in my_message)
  print 

  #
  if message_length > 150:  # continued message would be a longer message
  # check if continued message in progress for specific session
    if message_in_progress == 1:
      continued_message.append(my_message)
      continued_message_string.append(" ".join("{:02X}".format(ord(c)) for c in my_message))
      message_in_progress = 0
      print '    CONTINUED MESSAGES APPEAR TO HAVE ENDED. NOW PROCESSING COMPLETE MESSAGE (FFFB)'
      # combine all messages into new my_message 
      my_message = continued_message[0]
      for message in continued_message[1:]:
        my_message = my_message + message
      #init all continue variables
      
  Process_OPCODES(my_message)
  return
# 
def Process_Continue_Message(message,message_length):
#
  global continued_message
  global continued_message_string
  global message_in_progress
  global continued_message_length
#
  message_in_progress = 1 # true
#
  hdr_fmt1 = '<H'   # (little endian)[]
  s = struct.unpack(hdr_fmt1,message[0:2]) #  
  message_number = s[0]
  my_message = message[2:message_length+2]
#  
  print '    MESSAGE NUMB :','0x{:04X}'.format(message_number),'{0:08d}'.format(message_number)
  print
  print '    CONTING MESG :'," ".join("{:02X}".format(ord(c)) for c in my_message)
  print '    FOLLOWING MESSAGE WILL BE APPENDED. ENTIRE MESSAGE PROCESSED WHEN COMPLETE'
  print 
  continued_message_length = continued_message_length + len(my_message) # message_length # maybe plus 1
  continued_message.append(my_message)
  continued_message_string.append(" ".join("{:02X}".format(ord(c)) for c in my_message))
  if message_length <1156:      # should be able to process if not complete packet
    message_in_progress = 0
    # combine all messages here into single message and process
    print '    CONTINUED MESSAGES APPEAR TO HAVE ENDED. NOW PROCESSING COMPLETE MESSAGE (FFFA)'
    #init all continue variables
    
  return
# 
def Process_System_Message(my_message):
#
  print '    SYS OPCODE   :'," ".join("{:02X}".format(ord(c)) for c in my_message)
  print 
  Process_OPCODES(my_message)
  return
##########################################################################
#
def Process_Bundle_Number(bundle,b):
 hdr_fmt1 = '<H'   # (little endian)[Overall Bundle Number]
 s = struct.unpack(hdr_fmt1,bundle[b:b+2]) #
 bundle_number_tot = s[0] 
#
 print '  OVERALL BUNDLE :','0x{:04X}'.format(bundle_number_tot),'{0:08d}'.format(bundle_number_tot)
 print
 # 
 return 
## 
##
def Process_Bundle(bundle):  # really need to pass in session ID and save to that.
 #hexdump(bundle)
 print 
 hdr_fmt1 = '<B'   # (little endian)[bundle Type]
 s = struct.unpack(hdr_fmt1,bundle[0:1]) # using hdr_fmt1, unpack the first 
 bundle_type = s[0]
 print '  BUNDLE TYPE    :','0x{0:02x}'.format(bundle_type)
#
 if bundle_type == 0x00:
   print '  BUNDLE DESC    : MESSAGE BUNDLE (0x00)'
   Process_Bundle_Number(bundle,1)
   Process_Messages(bundle,3)
 elif bundle_type == 0x03:
   print '  BUNDLE DESC    : MESSAGE REPORT (0x03)'
   Process_Message_Report(bundle,1)
 elif bundle_type == 0x13:
   print '  BUNDLE DESC    : MESSAGE REPORT (0x13)'
 elif bundle_type == 0x20:
   print '  BUNDLE DESC    : MESSAGE BUNDLE (0x20)'
   Process_Bundle_Number(bundle,1)
   Process_Messages(bundle,3)
 elif bundle_type == 0x23:
   print '  BUNDLE DESC    : MESSAGE REPORT (0x23)'   
   Process_Message_Report(bundle,1)
 elif bundle_type == 0x63: 
   print '  BUNDLE DESC    : SESSION ACK/MESSAGE REPORT/MESSAGE BUNDLE (0x63)'
   Process_Session_Ack(bundle)  # could just pass relevant portions
   Process_Message_Report(bundle,5)
   b = 11
   if count(bundle) - b > 0 :
     Process_Messages(bundle,b)
 else:
   print '  BUNDLE DESC    : UNKNOWN (','0x{0:02x}'.format(bundle_type),')'

 return 
##
def Separator():
 print
 print '==================================================================='   
 return  
##
##
def count(iter):
    try:
        return len(iter)
    except TypeError:
        return sum(1 for _ in iter)
##        
##
def Extract_Payload_Bundles(pkt):
#
 bundle_number  = 0
 session_action = 0
 initial_session_kill = 0
 bundle_desc = ["REMOTE","LOCAL"]
 packetsize   = count(pkt.load)
#
 b = 4
 while packetsize - 4 - b > 0 & bundle_number < 3:  
   bundle_number = bundle_number+1 
   hdr_fmt1 = '<BB'   # (little endian)[Packet Class and Length]
   s = struct.unpack(hdr_fmt1,pkt.load[b:b+2]) # using hdr_fmt1, unpack the first 
   b = b + 2
   bundle_class      = s[1]&0xF0
   bundle_multipler  = s[1]&0x0F
   bundle_factor     = s[0]
   bundle_master     = 0
   if bundle_class > 0x80:
     bundle_master = 1
  #
  # Calculate bundle Length
  #
   if (bundle_factor>=0x80):  # 
     bundle_length = bundle_factor- 0x80 + bundle_multipler*0x80
   else:  # bundle_factor is less than 0x80
     bundle_length = bundle_factor
  #
  # If Master, read Session Action
  #
   if (bundle_master)==1: # need to read session action
    hdr_fmt1 = '<B'       # (little endian)[Session Action]
    s = struct.unpack(hdr_fmt1,pkt.load[b:b+1]) # 
    b = b + 1
    session_action = s[0]
  #  
  # Take action on Session
  #
    if session_action == 0x01:
      bundle_master_desc = 'CONTINUING SESSION'
    elif session_action == 0x21:
      bundle_master_desc = 'REQUESTING NEW SESSION'
      Create_Session()
    elif session_action == 0x14:
      bundle_master_desc = 'INITIAL CLOSE SESSION REQUEST'
      initial_session_kill = 1
  #
  # Read Session ID here
  #
   hdr_fmt1 = '<I'   # (little endian)[Session Code]
   s = struct.unpack(hdr_fmt1,pkt.load[b:b+4]) #
   b = b + 4
   session_id_a = s[0]
   session_id_b = 0x0000
   if session_id_a > 0x00FFFFFF:  # read longer session ID
     hdr_fmt1 = '<I'          # (little endian)[Session Code B]
     s = struct.unpack(hdr_fmt1,pkt.load[b:b+4]) #
     b = b + 3 # should be three
     session_id_b = s[0]&0x00FFFFFF #
  #
  # Ouput to file
  #
   print '  SESSION ID A   : ','0x{:08X}'.format(session_id_a),'{0:09d}'.format(session_id_a)
   if session_id_b > 0:
     print '  SESSION ID B   : ','0x{:08X}'.format(session_id_b),'{0:09d}'.format(session_id_b)
     print 
    #    
   if bundle_master == 1:
     print
     print '  SESSION ACTION : ','0x{0:02x}'.format(session_action),'{0:08b}'.format(session_action)
     print '  SESSION DESCRP : ',bundle_master_desc
     print 
#
   if initial_session_kill == 1 and bundle_class !=0xA0:
     hdr_fmt1 = '<I'       # (little endian)[Session Code]
     s = struct.unpack(hdr_fmt1,pkt.load[b:b+4]) # 
     b = b + 4
     session_id_aa = s[0]
     session_id_bb = 0x0000
     if session_id_aa > 0x00FFFFFF:  # read longer session ID
       hdr_fmt1 = '<I'          # (little endian)[Session Code B]
       s = struct.unpack(hdr_fmt1,pkt.load[b:b+4]) #
       b = b + 3 # should be three
       session_id_bb = s[0]&0x00FFFFFF 
#
     if session_id_a == session_id_aa and session_id_b == session_id_bb:
       Destroy_Session()  
       print '  SESSION DESCRP :  CLOSE SESSION REQUEST GRANTED'
       print
     else:
       print '  SESSION DESCRP :  CLOSE SESSION REQUEST NOT GRANTED'
       print       

   else:   
    #     
     bstatus = (bundle_class&0b01110000)>>4
     bstatus_dict = {0x00:'UNKNOWN (0x0)',0x01:'UNKNOWN (0x1)', 0x02:'ERROR (0x2)', 0x03:'IN-WORLD (0x3)',0x04:'SERVER REQUESTING NEW SESSION TO BE MASTER (0x4)',0x05:'UNKNOWN (0x5)',0x06:'PRE-CHARACTER SELECT (0x6)',0x07:'CHARACTER SELECT/LOAD WORLD (0x7)',0x08:'UNKNOWN (0x8)'}

     print 
     print '  BUNDLE NUMBER  : ',bundle_number
     print '  BUNDLE MASTER  : ',bundle_desc[bundle_master]
     print '  BUNDLE CLASS   : ','0x{0:X}'.format(bundle_class)  , '{0:08b}'.format(bundle_class)
     print '  BUNDLE STATUS  : ','0x{0:X}'.format(bstatus)  , '{0:04b}'.format(bstatus)
     print '  BUNDLE STATUS D: ', bstatus_dict.get(bstatus)  
     print '  BUNDLE LENGTH  : ','0x{:04X}'.format(bundle_length) , '{0:08d}'.format(bundle_length),'bytes'
     print
        
    #
    # Now read rest of bundle payload (we know length)
    #
     this_bundle = pkt.load[b:b+bundle_length]
     b = b + bundle_length
     if count(this_bundle)>0:
       Process_Bundle(this_bundle)
#
   print '  BYTES LEFT     :', packetsize - 4 - b
   print 
  #

  
 return
  
########################################################################################################################################
# Read PCAP, count, etc
#
if PCAPALL==1:
 pcap_file = rdpcap(PCAPNAME)  # reads the first 'count' packets
else:
 pcap_file = rdpcap(PCAPNAME,count=PCAPCOUNT)  # reads the first 'count' packets
print
#
# Init some Lists
#
bad_crc_list             = [];
sessionlist              = [];
message_all              = []; 
bundlelist               = [];
continued_message        = [];
continued_message_string = [];
#
# Init some Variables
#
is_frontiers             = 0; # used to decide if we need to print GEAR quick menu sets
bad_crc_packet           = 0; 
message_in_progress      = 0; # no message in progress
continued_message_length = 0;
#
##########################################################################################################
#
# Begin Main Loop over all packets in PCAP, not just EQOA packets
#
for index, pkt in enumerate(pcap_file):
    try:
        if pkt.haslayer(IP):     # only look at IP packets   TCP and UDP
#
# These next bits are old and are from the original python code. Better way to do this now.
#        
            macsrc = 0
            macdst = 0
#            
            for j in range(Machine.macCount):          # Match SRC MACHINE
              if machine[j].ip==pkt.getlayer(IP).src:
                macsrc = j
                
            for k in range(Machine.macCount):          # Match DST MACHINE
              if machine[k].ip==pkt.getlayer(IP).dst:
                macdst = k
#
            if (index > PCAPUDP1ST):                 # UDP packets don't start till after this
              if ((macsrc == 1) | (macdst == 1)):    # Write out ony if PS2 involved in message
                if pkt.haslayer(Raw):                # If UDP packet, it will have Raw (data) layer
#               
                  Print_Packet_Info(pkt,index,machine,macsrc,macdst)
#                  
                  if Check_for_Transfer(pkt)<1:      # It is not a transfer
#
                    if Check_CRC(pkt)<0:               # Checks CRC. Returns 0 if good, -1 if bad
                      bad_crc_list.append(index)
                      bad_crc_packet += 1
                      Separator()
                      continue                         # Drops packet and gets next one if bad
#                   
                    if Process_Payload_Header(pkt)<1:  # Process EQOA Payload Header - continue if transfer packet or bad packet
                      Separator() 
                      continue
#                  
                    Extract_Payload_Bundles(pkt)    #
#                 
                  Separator()                   
#         
    except:
        raise

print ' Number of BAD CRC Packets dropped: ',bad_crc_packet        
print ' Bad CRC Packets :',bad_crc_list
print  
print 'Done'
print