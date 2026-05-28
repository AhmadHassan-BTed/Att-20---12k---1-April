///////////////////////////////////////////////////////////////
//Main.c 
//
//CREATED: 01/31/2015 BEN
//Based almost entirely on KRAZUS server.c 
//
#include <stdio.h>      
#include <string.h>
#include <stdlib.h>    
#include <sys/types.h> 
//
#include <errno.h>
#include "UDPServer.h"
//
// MAIN
//
int main(int argc, char *argv[])
{
//	
//command line: ./EQOA_Emu <IPaddress> <Port>
//
//INPUTS: IP Address(string), Port (int)
//
///////////////////////////////////////////////////////////////
//      
    if  (argc !=  3) //check for command line arguments 
    {
		printf("usage: server <IPaddress> <Port>\n");
		exit(1);
    }
//
	int sockfd = StartServer(argv[1],argv[2]); // Start UDP Server 
//
	SendRecvLoop(sockfd);       // Enter SendRecvLoop
//
    StopServer(sockfd);         // Stop UDP Server
//
    return 0;
}
