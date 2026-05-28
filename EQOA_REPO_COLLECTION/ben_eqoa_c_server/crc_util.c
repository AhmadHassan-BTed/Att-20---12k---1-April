///////////////////////////////////////////////////////////////
//crc_util.c 
//CREATED: 01/31/2015 BEN
//
//This file holds the crc utility subroutines
//Actual crc_routine is  separate 
//
///////////////////////////////////////////////////////////////

#include <stdio.h>      
#include <string.h>

#include <stdlib.h>    
#include <sys/types.h>  
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <netdb.h>      
#include <errno.h>

//
// Generates a HEXSTRING from byte array
// This is supposed to convert an unsigned char buffer into a hexadecimal interpretation
//
int gen_hex_string(void* packet, int len, int flag, unsigned char* hexbuf, int *hexbuf_len)
 {
    u_int8_t* bytes = (u_int8_t*)packet;
    int i = 0;
    int j;
    unsigned char hex[2+flag];
    
    for (j = 0; j<len*3; j++)
    {
    hexbuf[j] = '\0'; 
    }

    while (i < len){
	  if (flag==0){
		  sprintf(hex, "%02X", bytes[i]);
	  }
	  else{
		  sprintf(hex, " %02X", bytes[i]);
	  }		  
	strcat(hexbuf, hex);
	i++;
    }
    *hexbuf_len = len*(2+flag);
	return 0;
 }

int AppendCRC(unsigned char* hexstr, unsigned char* packet, int pSize) {
    char *start = packet+pSize;
    unsigned int u;
    int i = 0;
    while (i < 4 && sscanf(hexstr,"%2x",&u) == 1) {
	*start++ = u;
	hexstr += 2;
	i++;
	printf("copied: %x\n", u);
    }
    return 0;
}