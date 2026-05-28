import math, struct
from datetime import datetime

hotKeyIn = {87 : 'WWW', 191 : 'SSE', 83 : 'NWW', 199 : 'WNS', 247 : 'WSS', 
            59 : 'ESN', 227 : 'NES', 115 : 'NSW', 51 : 'NSN', 47 : 'SEN', 
            19 : 'NWN', 179 : 'NSE', 7 : 'WNN', 151 : 'WWE', 143 : 'SNE', 
            223 : 'SWS', 219 : 'EWS', 139 : 'ENE', 239 : 'SES', 99 : 'NEW', 
            43 : 'EEN', 95 : 'SWW', 235 : 'EES', 195 : 'NNS', 207 : 'SNS', 
            215 : 'WWS', 103 : 'WEW', 231 : 'WES', 27 : 'EWN', 15 : 'SNN', 
            203 : 'ENS', 91 : 'EWW', 35 : 'NEN', 79 : 'SNW', 159 : 'SWE', 
            243 : 'NSS', 39 : 'WEN',167 : 'WEE', 127 : 'SSW', 75 : 'ENW', 
            147 : 'NWE', 211 : 'NWS', 63 : 'SSN', 163 : 'NEE', 255 : 'SSS', 
            135 : 'WNE', 155 : 'EWE', 31 : 'SWN', 107 : 'EEW', 175 : 'SEE', 
            23 : 'WWN', 71 : 'WNW', 3 : 'NNN', 187 : 'ESE', 67 : 'NNW', 
            131 : 'NNE', 119 : 'WSW', 183 : 'WSE', 55 : 'WSN', 171 : 'EEE', 
            251 : 'ESS', 11 : 'ENN', 123 : 'ESW', 111 : 'SEW', 58 : 'ES', 
            18 : 'NW', 14 : 'SN', 54 : 'WS', 22 : 'WW', 2 : 'NN', 38 : 'WE', 
            46 : 'SE', 10 : 'EN', 42 : 'EE', 34 : 'NE', 26 : 'EW', 50 : 'NS', 
            6 : 'WN', 62 : 'SS', 30 : 'SW', 564 : 'NSNE', 308 : 'NSNW', 
            532 : 'NWNE', 616 : 'WEWE', 828 : 'ESNS', 380 : 'ESWW', 480 : 'SWSW', 
            920 : 'WWES', 432 : 'SEEW', 724 : 'NWSE', 188 : 'ESEN', 80 : 'SNWN', 
            884 : 'NSWS', 60: 'ESNN', 976 : 'SNSS', 328 : 'WNWW', 972 : 'ENSS', 
            168 : 'WEEN', 716 : 'ENSE', 352 : 'SWWW',436 : 'NSEW', 384 : 'SSWW', 
            820 : 'NSNS', 264 : 'WNNW', 704 : 'SSEE', 744 : 'WESE', 356 : 'NEWW', 
            100 : 'NEWN', 340 : 'NWWW', 440 : 'WSEW', 904 : 'WNES', 776 : 'WNNS', 
            1008 : 'SESS', 104 : 'WEWN', 452 : 'NNSW', 136 : 'WNEN', 896 : 'SSWS', 
            496 : 'SESW', 292 : 'NENW', 200 : 'WNSN', 568 : 'WSNE', 644 : 'NNEE', 
            232 : 'WESN', 120 : 'WSWN', 756 : 'NSSE', 628 : 'NSWE', 888 : 'WSWS', 
            692 : 'NSEE', 476 : 'EWSW', 876 : 'EEWS', 860 : 'EWWS', 924 : 'EWES', 
            764 : 'ESSE', 36 : 'NENN', 240 : 'SESN', 204 : 'ENSN', 900 : 'NNES', 
            832 : 'SSNS', 96 : 'SWWN', 620 : 'EEWE', 256 : 'SSSN', 944 : 'SEES', 
            92 : 'EWWN', 412 : 'EWEW', 320 : 'SSNW', 608 : 'SWWE', 932 : 'NEES',
            464 : 'SNSW', 224: 'SWSN', 552 : 'WENE', 736 : 'SWSE', 316 : 'ESNW', 
            752 : 'SESE', 560 : 'SENE', 56 : 'WSNN', 248 : 'WSSN', 472 : 'WWSW', 
            580 : 'NNWE', 420 : 'NEEW', 444 : 'ESEW', 584 : 'WNWE', 288 : 'SWNW', 
            1016 : 'WSSS', 404 : 'NWEW', 696 : 'WSEE', 192 : 'SSEN', 236 : 'EESN', 
            508 : 'ESSW', 336 : 'SNWW', 780 : 'ENNS', 912 : 'SNES', 284 : 'EWNW', 
            788 : 'NWNS', 504 : 'WSSW', 576 : 'SSNE', 840 : 'WNWS', 844 : 'ENWS', 
            728 : 'WWSE', 108 : 'EEWN', 668 : 'EWEE', 1004 : 'EESS', 460 : 'ENSW', 
            708 : 'NNSE', 152 : 'WWEN', 968 : 'WNSS', 268 : 'ENNW', 8 : 'WNNN', 
            252 : 'ESSN', 760 : 'WSSE', 48 : 'SENN', 640 : 'SSWE', 184 : 'WSEN', 
            52 : 'NSNN', 588 : 'ENWE', 344 : 'WWWW', 544 : 'SWNE', 824 : 'WSNS', 
            740 : 'NESE', 804 : 'NENS', 296 : 'WENW', 500 : 'NSSW', 1020 : 'ESSS', 
            220 : 'EWSN', 592 : 'SNWE', 604 : 'EWWE', 324 : 'NNWW', 212 : 'NWSN', 
            196 : 'NNSN', 392 : 'WNEW', 88 : 'WWWN', 488 : 'WESW', 300 : 'EENW', 
            160 : 'SWEN', 24 : 'WWNN', 996 : 'NESS', 360 : 'WEWW', 936 : 'WEES', 
            216 : 'WWSN', 64 : 'SSNN', 16 : 'SNNN', 612 : 'NEWE', 940 : 'EEES', 
            960 : 'SSES', 180 : 'NSEN', 784 : 'SNNS', 648 : 'WNEE', 208 : 'SNSN', 
            76 : 'ENWN', 872 : 'WEWS', 272 : 'SNNW', 448 : 'SSEW', 672 : 'SWEE', 
            68 : 'NNWN', 684 : 'EEEE', 796 : 'EWNS', 516 : 'NNNE', 856 : 'WWWS', 
            304 : 'SENW', 980 : 'NWSS', 116 : 'NSWN', 928 : 'SWES', 624 : 'SEWE', 
            660 : 'NWEE', 388 : 'NNEW', 260 : 'NNNW', 808 : 'WENS', 848 : 'SNWS',
            12 : 'ENNN', 1012 : 'NSSS', 144 : 'SNEN', 84 : 'NWWN', 908 : 'ENES', 
            988 : 'EWSS', 700 : 'ESEE', 520 : 'WNNE', 72 : 'WNWN', 600 : 'WWWE', 
            40 : 'WENN', 792 : 'WWNS', 408 : 'WWEW', 768 : 'SSSE', 148 : 'NWEN', 
            372 : 'NSWW', 416 : 'SWEW', 140 : 'ENEN', 864 : 'SWWS', 632 : 'WSWE', 
            176 : 'SEEN', 772 : 'NNNS', 312 : 'WSNW', 572: 'ESNE', 28 : 'EWNN', 
            368 : 'SEWW', 528 : 'SNNE', 400 : 'SNEW', 680 : 'WEEE', 172 : 'EEEN', 
            244 : 'NSSN', 812 : 'EENS', 720 : 'SNSE', 424 : 'WEEW', 548 : 'NENE', 
            164 : 'NEEN', 156 : 'EWEN', 556 : 'EENE', 664 : 'WWEE', 396 : 'ENEW', 
            364 : 'EEWW', 128 : 'SSWN', 524 : 'ENNE', 948: 'NSES', 1024 : 'SSSS', 
            484 : 'NESW', 428 : 'EEEW', 492 : 'EESW', 800 : 'SWNS', 916 : 'NWES', 
            676 : 'NEEE', 712 : 'WNSE', 512 : 'SSSW', 228 : 'NESN', 892 : 'ESWS', 
            132 : 'NNEN', 652 : 'ENEE', 956 : 'ESES', 276 : 'NWNW', 732 : 'EWSE', 
            656 : 'SNEE', 4 : 'NNNN', 596 : 'NWWE', 44 : 'EENN', 124 : 'ESWN', 
            984: 'WWSS', 332 : 'ENWW', 540 : 'EWNE', 964 : 'NNSS', 1000 : 'WESS', 
            112 : 'SEWN', 880 : 'SEWS', 748 : 'EESE', 816 : 'SENS', 456 : 'WNSW', 
            836 : 'NNWS', 32 : 'SWNN', 348 : 'EWWW', 636 : 'ESWE', 13 : 'S', 1 : 'N',
            5 : 'W', 9 : 'E', 0 : 'Main Menu'}

hotKeyOut = {'NNNN' : 4, 'NNNW' : 260, 'NNNE' : 516, 'NNNS' : 772, 'NNWN' : 68, 
 'NNWW' : 324, 'NNWE' : 580, 'NNWS' : 836, 'NNEN' : 132, 'NNEW' : 388, 
 'NNEE' : 644, 'NNES' : 900, 'NNSN' : 196, 'NNSW' : 452, 'NNSE' : 708, 
 'NNSS' :964, 'NWNN' : 20, 'NWNW' : 276, 'NWNE' : 532, 'NWNS' : 788, 
 'NWWN' : 84, 'NWWW' : 340, 'NWWE' : 596, 'NWWS' : 852, 'NWEN' : 148, 
 'NWEW' : 404, 'NWEE' : 660, 'NWES' : 916, 'NWSN' : 212, 'NWSW' : 468, 
 'NWSE' : 724, 'NWSS' : 980, 'NENN' : 36, 'NENW' : 292, 'NENE' : 548, 
 'NENS' : 804, 'NEWN' : 100, 'NEWW' : 356, 'NEWE' : 612, 'NEWS' : 868, 
 'NEEN' : 164, 'NEEW' :420, 'NEEE' : 676, 'NEES' : 932, 'NESN' : 228, 
 'NESW' : 484, 'NESE' : 740, 'NESS' : 996, 'NSNN' : 52, 'NSNW' : 308, 
 'NSNE' : 564, 'NSNS' : 820,'NSWN' : 116, 'NSWW' : 372, 'NSWE' : 628, 
 'NSWS' : 884, 'NSEN' : 180, 'NSEW' : 436, 'NSEE' : 692, 'NSES' : 948, 
 'NSSN' : 244, 'NSSW' : 500, 'NSSE' : 756, 'NSSS' : 1012, 'WNNN' : 8, 
 'WNNW' : 264, 'WNNE' : 520, 'WNNS' : 776, 'WNWN' : 72, 'WNWW' : 328, 
 'WNWE' : 584, 'WNWS' : 840, 'WNEN' : 136, 'WNEW' : 392, 'WNEE' : 648, 
 'WNES' : 904, 'WNSN' : 200, 'WNSW' : 456, 'WNSE' : 712, 'WNSS' : 968, 
 'WWNN' : 24, 'WWNW' : 280, 'WWNE' : 536, 'WWNS' : 792, 'WWWN' : 88, 
 'WWWW' : 344, 'WWWE' : 600, 'WWWS' : 856, 'WWEN' : 152, 'WWEW' : 408, 
 'WWEE' : 664, 'WWES' : 920, 'WWSN' : 216, 'WWSW' : 472, 'WWSE' : 728, 
 'WWSS' : 984, 'WENN' : 40, 'WENW' : 296, 'WENE' : 552, 'WENS' : 808, 
 'WEWN' : 104, 'WEWW' : 360, 'WEWE' : 616, 'WEWS' : 872, 'WEEN' : 168, 
 'WEEW' : 424, 'WEEE' : 680, 'WEES' : 936, 'WESN' : 232, 'WESW' : 488, 
 'WESE' : 744, 'WESS' : 1000, 'WSNN' : 56, 'WSNW' : 312, 'WSNE' : 568, 
 'WSNS' : 824, 'WSWN' : 120, 'WSWW' : 376, 'WSWE' : 632, 'WSWS' : 888, 
 'WSEN' : 184, 'WSEW' : 440, 'WSEE' : 696, 'WSES' : 952, 'WSSN' :248, 
 'WSSW' : 504, 'WSSE' : 760, 'WSSS' : 1016, 'ENNN' : 12, 'ENNW' : 268, 
 'ENNE' : 524, 'ENNS' : 780, 'ENWN' : 76, 'ENWW' : 332, 'ENWE' : 588,
 'ENWS' : 844, 'ENEN' : 140, 'ENEW' : 396, 'ENEE' : 652, 'ENES' : 908, 
 'ENSN' : 204, 'ENSW' : 460, 'ENSE' : 716, 'ENSS' : 972, 'EWNN' : 28, 
 'EWNW' : 284, 'EWNE' : 540, 'EWNS' : 796, 'EWWN' : 92, 'EWWW' : 348, 
 'EWWE' :604, 'EWWS' : 860, 'EWEN' : 156, 'EWEW' : 412, 'EWEE' : 668, 
 'EWES' : 924, 'EWSN' : 220, 'EWSW' : 476, 'EWSE' : 732, 'EWSS' : 988, 
 'EENN' : 44,'EENW' : 300, 'EENE' : 556, 'EENS' : 812, 'EEWN' : 108, 
 'EEWW' : 364, 'EEWE' : 620, 'EEWS' : 876, 'EEEN' : 172, 'EEEW' : 428, 
 'EEEE' : 684, 'EEES' : 940, 'EESN' : 236, 'EESW' : 492, 'EESE' : 748, 
 'EESS' : 1004, 'ESNN' : 60, 'ESNW' : 316, 'ESNE' : 572, 'ESNS' : 828, 
 'ESWN' : 124, 'ESWW' : 380, 'ESWE' : 636, 'ESWS' : 892, 'ESEN' : 188, 
 'ESEW' : 444, 'ESEE' : 700, 'ESES' : 956, 'ESSN' : 252, 'ESSW' : 508, 
 'ESSE' : 764, 'ESSS' : 1020, 'SNNN' : 16, 'SNNW' : 272, 'SNNE' : 528, 
 'SNNS' : 784, 'SNWN' : 80, 'SNWW' : 336, 'SNWE' : 592, 'SNWS' : 848, 
 'SNEN' : 144, 'SNEW' : 400, 'SNEE' : 656, 'SNES' : 912, 'SNSN' : 208, 
 'SNSW' : 464, 'SNSE' : 720, 'SNSS' : 976, 'SWNN' : 32, 'SWNW' : 288, 
 'SWNE' : 544, 'SWNS' : 800, 'SWWN' : 96, 'SWWW' : 352, 'SWWE' : 608, 
 'SWWS' : 864, 'SWEN' : 160, 'SWEW' : 416,'SWEE' : 672, 'SWES' : 928, 
 'SWSN' : 224, 'SWSW' : 480, 'SWSE' : 736, 'SWSS' : 992, 'SENN' : 48, 
 'SENW' : 304, 'SENE' : 560, 'SENS' : 816, 'SEWN' : 112, 'SEWW' : 368, 
 'SEWE' : 624, 'SEWS' : 880, 'SEEN' : 176, 'SEEW' : 432, 'SEEE' : 688, 
 'SEES' : 944, 'SESN' : 240, 'SESW' : 496, 'SESE' : 752, 'SESS' : 1008, 
 'SSNN' : 64, 'SSNW' : 320, 'SSNE' : 576, 'SSNS' : 832, 'SSWN' : 128, 
 'SSWW' : 384, 'SSWE' : 640, 'SSWS' : 896, 'SSEN' : 192, 'SSEW' : 448, 
 'SSEE' : 704, 'SSES' : 960, 'SSSN' : 256, 'SSSW' : 512, 'SSSE' : 768, 
 'SSSS' : 1024, 'NNN' : 3, 'NNW' : 67, 'NNE' : 131, 'NNS' : 195, 
 'NWN' : 19, 'NWW' : 83, 'NWE' : 147, 'NWS' : 211, 'NEN' : 35, 
 'NEW' : 99, 'NEE' : 163, 'NES' : 227, 'NSN' : 51, 'NSW' : 115, 
 'NSE' : 179, 'NSS' : 243, 'WNN' : 7, 'WNW' : 71, 'WNE' : 135, 
 'WNS' : 199, 'WWN' : 23, 'WWW' : 87, 'WWE' :151, 'WWS' : 215, 
 'WEN' : 39, 'WEW' : 103, 'WEE' : 167, 'WES' : 231, 'WSN' : 55, 
 'WSW' : 119, 'WSE' : 183, 'WSS' : 247, 'ENN' : 11, 'ENW' : 75, 
 'ENE' : 139, 'ENS' : 203, 'EWN' : 27, 'EWW' : 91, 'EWE' : 155, 
 'EWS' : 219, 'EEN' : 43, 'EEW' : 107, 'EEE' : 171, 'EES' : 235, 
 'ESN' : 59, 'ESW' : 123, 'ESE' : 187, 'ESS' : 251, 'SNN' : 15, 
 'SNW' : 79, 'SNE' : 143, 'SNS' : 207, 'SWN' : 31, 'SWW' : 95, 
 'SWE' : 159, 'SWS' : 223, 'SEN' : 47, 'SEW' : 111, 'SEE' : 175, 
 'SES' : 239, 'SSN' : 63, 'SSW' : 127, 'SSE' : 191, 'SSS' : 255, 
 'NN' : 2, 'NW' : 18, 'NE' : 34, 'NS' : 50, 'WN' : 6, 'WW' : 22, 
 'WE' : 38, 'WS' : 54, 'EN' : 10, 'EW' : 26, 'EE' : 42, 'ES' : 58, 
 'SN': 14, 'SW' : 30, 'SE' : 46, 'SS' : 62, 'Submenu' : '20',
'N' : 1, 'W' : 5, 'E' : 9, 'S' : 13, 'Main Menu' : 0}

def technique_orig(value):  # Should convert 'true' database value to 'transport' value using technique
  #
  debug = False 
  #
  a = '{:b}'.format(value)
  num_bits = len(a)  # just count characters will include minus sign
  num_bytes = int(math.ceil(float(num_bits)/float(8.0)))   # how many bytes needed
  max_num = 2**(num_bytes*8-1)-1
  if debug:
    print() 
    print( ' Incoming Value    :',value) 
    print( ' Incoming (binary) :',a) 
    print( ' Number of bits    :',num_bits ) 
    print( ' Number of bytes   :',num_bytes) 
    print( ' Max Integer       :',max_num )
  #
  is_negative = False
  if value  < 0 or value > max_num:
    is_negative = True
    
  if is_negative:   
    yu = num_bytes
    negative_sum = 0
    for s in range(0,num_bytes):
      negative_sum += int(0xFF<<(s*8))
    value = negative_sum&(~value)
    if debug:
      print (' Max Negative      : {:X}'.format(negative_sum)) 
      print (' New Value         : {:X}'.format(value)) 
  #
  # Now process 
  #
  # Extract all bits into a list
  #
  mybits = []
  shift = 0
  while (value>>shift)!=0:
    mybits.append((value>>shift)&0b1)
    shift +=1
  if debug:
    print( mybits ) 

  #
  ####################################################
  #
  # Construct transfer 'value' 
  #
  if debug:
    print() 
  technique_sum = 0
  if is_negative:
    technique_sum = 0b1
  bit_position  = 2   # start at 2 for technique
  for bit in mybits:
    if (bit_position % 8 ==0):
      technique_sum += int(0b1<<(bit_position-1))
      bit_position +=1
  #    print '1', bit_position-1, technique_sum,'<<-- technique bit' 
  #    print
    #       
    technique_sum += int(bit<<(bit_position-1))
    bit_position +=1    
  #  print bit, bit_position-1, technique_sum   
  #
  #####################################################
  #
  # Determine how many bytes
  #
  num_bytes = (bit_position)//8   # how many bytes
  if (bit_position)%8 !=0:
    num_bytes = num_bytes+1

  #
  ######################################################
  #
  # Optional Output
  #
  if debug:
    print()  
    print( ' Original Value In : ',value) 
    print( ' BitFlip Performed : ',is_negative) 
    print( ' Technique Value   : ',"0x{value:0{bytes}X}".format(value = technique_sum,bytes = num_bytes*2)) 
    print( ' Technique Binary  : ','{value:0{bits}b}'.format(value = technique_sum, bits = num_bytes*8))
    print( ' Number of Bytes   : ',num_bytes)
    print()   
  #
  return technique_sum 


def technique(value):  # Should convert 'true' database value to 'transport' value using technique
  #
  debug = False
  #
  a = '{:b}'.format(value)
  num_bits = len(a)  # just count characters will include minus sign
  num_bytes = int(math.ceil(float(num_bits)/float(8.0)))   # how many bytes needed
  max_num_tech = 2**(num_bytes*8-1)-1  # the technique strips off top bit
  max_num_norm = 2**(num_bytes*8)-1    # in general the largest number
  if debug:
    print() 
    print( ' Incoming Value    :',value)
    print( ' Incoming (binary) :',a) 
    print( ' Number of bits    :',num_bits ) 
    print( ' Number of bytes   :',num_bytes) 
    print() 
    print( ' Max Integer (tech):',max_num_tech) 
    print( ' Max Integer (norm):',max_num_norm) 
  #
  #
  # if value is > max_num, then might need another byte
  #
  if value > max_num_tech: num_bytes =+1
  #
  is_negative = False
  if value  < 0 or value > max_num_norm:
    is_negative = True
    
  if is_negative:   
    yu = num_bytes
    negative_sum = 0
    for s in range(0,num_bytes):
      negative_sum += int(0xFF<<(s*8))
    value = negative_sum&(~value)
    if debug:
      print() 
      print( ' Max Negative      : {:X}'.format(negative_sum)) 
      print( ' New Value         : {:X}'.format(value)) 
      print() 
  #
  # Now process 
  #
  # Extract all bits into a list
  #
  mybits = []
  shift = 0
  while (value>>shift)!=0:
    mybits.append((value>>shift)&0b1)
    shift +=1
  if debug:
    print( mybits)  

  #
  ####################################################
  #
  # Construct transfer 'value' 
  #
  if debug:
    print() 
  technique_sum = 0
  if is_negative:
    technique_sum = 0b1
  bit_position  = 2   # start at 2 for technique
  for bit in mybits:
    if (bit_position % 8 ==0):
      technique_sum += int(0b1<<(bit_position-1))
      bit_position +=1
  #    print ('1', bit_position-1, technique_sum,'<<-- technique bit') 
  #    print() 
    #       
    technique_sum += int(bit<<(bit_position-1))
    bit_position +=1    
  #  print( bit, bit_position-1, technique_sum)   
  #
  #####################################################
  #
  # Determine how many bytes
  #
  num_bytes = (bit_position)//8   # how many bytes
  if (bit_position)%8 !=0:
    num_bytes = num_bytes+1

  #
  ######################################################
  #
  # Optional Output
  #
  if debug:
    print()  
    print( ' Original Value In : ',value) 
    print( ' BitFlip Performed : ',is_negative) 
    print( ' Technique Value   : ',"0x{value:0{bytes}X}".format(value = technique_sum,bytes = num_bytes*2)) 
    print( ' Technique Binary  : ','{value:0{bits}b}'.format(value = technique_sum, bits = num_bytes*8))
    print( ' Number of Bytes   : ',num_bytes) 
    print()   
  #
  return technique_sum

def s2c(value): # server to client representation - original
#
  bt =[]
  value1 = hex(value)[2:]
  v1 = len(str(value1)) # length of this value
  #log.info('Length is {}'.format(v1))
  if v1%2 !=0:         # if not even number, make longer
    v1=v1+1
  ty = '{:0'+(str(v1))+'X}' # this makes sure we have multiple of two - formats '{:08X}'
  s = ty.format(value)    # log.infos it out to 's'
#  
  for x in range(0,len(s),2):
    bt.append(int(s[x:x+2], 16)) # loops over each character byte and writes it to bt
#    
  bitflip = bt[len(bt)-1]%2 # determines if last bit is 1 or 0.  
#

  #log.info (bitflip, bt,s, len(bt))
  forneg     = 0 # This is value that might be used if bitflip is set
  shift      = 0
  fshift     = 0
  tot        = 0
  num_bytes = len(bt)
 #
  for y in range(num_bytes-1,-1,-1):  # 
     if bt[y] >= 0x80:
       bt[y] = bt[y] - 0x80
     #log.info(y,bt[y])  
#     
     forneg = int(forneg<<8)+0xFF # this is used if this is a bit flip. Creates a FFFF size of input
#       
     bt[y] = bt[y]<<shift
     shift = shift+8                 # keeps track of how many 
     tot = tot + int(bt[y]>>fshift)
     fshift = fshift +1
    
  tot = tot >>1  # drops the lowest bit for all
  
# sum and forneg main values updated. 
  #log.info ("INPUT  - ",'0x{:X}'.format(value))
  #log.info ("INPUT  -  0x"+ty.format(value))
  #log.info ("FORNEG - ",'0x{:X}'.format(forneg), num_bytes)
  #log.info ("SUM    - ",'0x{:X}'.format(tot))
  #log.info ("FLIPBIT- ",bitflip)

  if bitflip == 1:
    leng = len(hex(tot)[2:])
    leng1 = len(hex(forneg)[2:])
    if leng > 4 and leng < 8:
        if leng < 8 and leng1 < 8:
            #log.info('leng1 is {}'.format(leng1))
            total = 8 - leng1
            #log.info('moving to the left {}'.format(total))
            for i in range(total):
                #log.info(hex(forneg))
                forneg = int(forneg<<4) + 0xF

        elif leng < 8 and leng1 > 8:
            #log.info('leng1 is {}'.format(leng1))
            total = leng1 - 8
            #log.info('moving to the right {}'.format(total))
            for i in range(total):
                #log.info(hex(forneg))
                forneg = int(forneg>>4)
                


    elif leng >= 8:
        total = leng1 - leng
        #log.info('Moving {} to the right'.format(total))
        for i in range(total):
            forneg = int(forneg>>4)       
            
    else: 
        forneg = int(forneg>>8)

    #log.info('Sum is {} and forneg is {}'.format(leng, len(hex(forneg)[2:]))) 
    tot = forneg^tot
    # remove some bits from the top, then flip
  # need this for icons to be correct 
  # 
#
  
  return tot

def helpPack(value):
  if (struct.pack('<Q', value).rstrip(b'\x00')) == b'':
    return struct.pack('<B', 0)

  else:
    return struct.pack('<Q', value).rstrip(b'\x00')

def datetime_to_dnp3(date = None):
  if date is None:
    date = datetime.utcnow()

  seconds = (date - datetime(1970, 1, 1)).total_seconds()
  milliseconds = int(seconds * 1000)
  return milliseconds


