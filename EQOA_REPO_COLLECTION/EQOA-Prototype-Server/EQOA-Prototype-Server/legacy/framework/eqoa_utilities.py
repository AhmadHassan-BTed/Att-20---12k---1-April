'''
Created on July 3, 2016

@author:  Ben Turi
'''
#
import math
import struct

# this is a utility method that allows the creation of a Python 2.7 compatible enumerations
def enum(**enums):
   return type('Enum', (), enums)
#
################################################
#
   def count(iter):
    try:
        return len(iter)
    except TypeError:
        return sum(1 for _ in iter)
##        
##
#
################################################
#
# Routines for Encoding/Decoding bundle lengths
#
# These are always simple, just need to shift one bit I believe
# never negative, less than about 1300 
# These are alway only 12 bits or less
   
def bunLenDecode(value):  # Should convert 'true' Bundle Length to 'transport' value using technique
  # 
  top = (0x0F00&value)>>1
  bot = (0x00FF&value)&0x7F
  #
  answer = int(top+bot)
  #
  return answer
  
def bunLenEncode(value):  # Should convert 'true' Bundle Length to 'transport' value using technique
  #  
  top = (value<<1)&0x0F00
  bot = (0x007F&value)+0x80
  #
  answer = int(top+bot)
  #
  return answer  
#
##########################################################
#
#def packTechnique8(input):  # Try to pack arbitrary byte lengths - probably not needed. Use int_to_bytes
#  #  
#  value = int(input[0])
#  bytes = int(input[1])
#  #
#  s = struct.unpack('>Q',struct.pack('<Q',value))
#  #
#  shift = (8-int(bytes))*8   # to get shifted bits
#  #
#  answer = s[0]>>int(shift)
#  #
#  return [answer, bytes]   
  
def int_to_bytes(val, num_bytes):
    return [(val & (0xff << pos*8)) >> pos*8 for pos in range(num_bytes)]

#
#######################################################
#
def packFixed(fmt_str,packingList):  # 
  # 
  #print(fmt_str)
  #print(packingList) 
  packedString = struct.pack(fmt_str,*packingList) # the *packList pulls values out of list
  #
  return packedString
#
def packVariable(packingList):  # 
  #
  encodedObjects = [];  # 
  for p in packingList:
    t = technique(p)
    encodedObjects.append(int_to_bytes(t[0],t[1])) 
  #
  aout = [item for sublist in encodedObjects for item in sublist]      
  packedString = "".join( chr(val) for val in aout)        
  #
  return packedString
#
def packStringAsUnicode(packingList):  # 
  #
  # might use 'unicode-escape' instead of 'string-escape'  see if it breaks anything
  # all tests still run with this mod
  #
  packedString = ""
  for p in packingList:
    packedString += packFixed('<I',[len(p.encode('unicode-escape'))]) # packs length as 4 byte (typically required)
    packedString += p.encode('utf-16_le')    
  #
  return packedString     
#
def packStringAsASCII(packingList):  # 
  #
  packedString = ""
  for p in packingList:
    packedString += packFixed('<I',[len(p.encode('string-escape'))]) # packs length as 4 byte (typically required)
    packedString += p.encode('string-escape')    
  #
  return packedString
###############################################
#
#  assume all values will be integers. could be negative or positive
#
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
  return [technique_sum,num_bytes]; 
  
def technique_orig(value):  # Should convert 'true' database value to 'transport' value using technique
  #
  debug = True
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
  return [technique_sum,num_bytes]     
#
