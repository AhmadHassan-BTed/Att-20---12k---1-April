
///////////////////////////////////////////////////////////////

#include <stdio.h>      
#include <string.h>
#include <stdlib.h>    
#include <sys/types.h>  
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <netdb.h>      
#include <errno.h>
#include <signal.h>
#include <stdbool.h>

#include <fcntl.h> // for open
#include <unistd.h> // for close

#include "UDPServer.h"
#include "crc_util.h"
#include "crc.h"
#include "packets.h"

#define E_CODE  224               // [E0]
#define SESSION_REQUEST 33        // [E0 21]
#define SESSION_CLOSE   20        // [E0 14]
#define SERVER_COMPLEX_REQUEST 1  // [E0 01]
#define CHARACTER_REQUEST 112     // [70]

#define MAX_BUF_SIZE 1024
//
uint32_t Process_Dummy(unsigned char* buf,int msglen,unsigned char* response,int *resplen);
uint32_t Process_Session_Request(unsigned char* buf,int msglen,unsigned char* response,int *resplen);
uint32_t Process_Server_Complex_Request(unsigned char* buf,int msglen,unsigned char* response,int *resplen);
uint32_t Process_Character_Request(unsigned char* buf,int msglen,unsigned char *response,unsigned int *resplen);

//
//////////////////////////////////////////////////////////////
//
//
struct eqoa_packet_header
{
    uint16_t ClientCode;
    uint16_t ServerCode;
    uint8_t  PacketLength;
    uint8_t  OpCode;
    uint8_t  OpOption;
    uint32_t CRC;
};
//
static bool keepRunning = true;

void intHandler(int dummy)
{
    keepRunning = false;
}   
//
///////////////////////////////////////////////////////////////////////////
//
// Start Server
//  
int StartServer(const char* ipname,const char* portname) 
{
    struct sockaddr_in servaddr;
//
    int port=0;
    if (portname==NULL)
        port = 10070;
    else 
        port = atoi(portname);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ipname);
    servaddr.sin_port = htons(port);
//
    printf("\n");
    printf("> Starting EQOA Revival Server...\n");
    printf("\n");
//
// Ask for Socket from OS. If not successful, fail.
//
// Do I want this to block or not?
//
//
    int sockfd = -1;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("  Call to socket failed\n");
        printf("  Exiting Server with Error\n");
        exit(1);
    }
//
// Now we have socket, bind to it. Fail if not successful
//
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) 
    {
        close(sockfd);
        printf("  Call to bind failed, Closing Socket\n");
        printf("  Exiting Server with Error\n");
        exit(1);
    }
//
// Report out Successful start
//
    printf("> Server is listening on ip:port: %s:%i\n", ipname, port);
    printf("\n");
    
    return sockfd;

}
 ///////////////////////////////////////////////////////////////////////////
//
// Main Send/Receive Loop
//  
void SendRecvLoop(int sockfd)
{
    int j;
    int rNo = 0;
    int flag= 0, success;
    int msglen, clilen;
    int hexbuf_len,resp_len;
    struct sockaddr_in cliaddr;
    unsigned char buf[MAX_BUF_SIZE];
    unsigned char hexbuf[MAX_BUF_SIZE*3]; //each byte will become two bytes and a possible space in hexbuf
    unsigned char response[MAX_BUF_SIZE];
//    uint32_t crcinit;
    crc_t  crc; 

    printf("> Entering Main Send/Recv Loop\n");
    printf("\n");
    
    signal(SIGINT, intHandler);
    while (keepRunning) // This will continue to loop
    { 
    // Get incoming packets
    //
    clilen = sizeof(cliaddr);
    //
    
    uint need_response = 1; // Assume all received packets need response to start
    printf("> > Waiting for Client Packet...");
    printf(" \n");
    fflush(stdout);
    //
    // since this is recvfrom call is blocking, I won't be able to break out with CTRL-C, use CTRL-Z (not clean)
    //
    if ((msglen = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &clilen)) < 0) 
    {
      printf("Call to recvfrom failed\n");
      close(sockfd);
      exit(1);
    }
    //
    // reset response vector
    //
    for (j = 0; j<MAX_BUF_SIZE; j++)
    {
    response[j] = '\0'; 
    }
    //
    // 
    //print details of the client/peer and the data received
     printf("\n");
     printf("***************************************************************************\n");
     printf("\n");     
     printf("> Incoming Packet #%i \n",rNo);
     printf("> Received packet from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
     printf("> Packet Size: %05i bytes\n",msglen);
     flag = 1;
     success = gen_hex_string(buf, msglen, flag, hexbuf, &hexbuf_len);
     printf("> HexBuf Size: %d\n",hexbuf_len);
     printf("> Incoming Packet Data: %s\n",hexbuf);

     printf("> \n"); 
          
     struct eqoa_packet_header eph;
       memcpy(&eph.ClientCode, &buf, 2);
       memcpy(&eph.ServerCode, &buf[2], 2);
       memcpy(&eph.PacketLength, &buf[4], 1);
       memcpy(&eph.OpCode, &buf[5], 1);  
       memcpy(&eph.CRC, &buf[msglen-4], 4);  
       if(eph.OpCode==224)
       {
         memcpy(&eph.OpOption, &buf[6], 1); // read another byte if it is an extended opcode
       }            
     //
     success = gen_hex_string(&eph.ClientCode, sizeof(eph.ClientCode), flag, hexbuf, &hexbuf_len);
     printf("> Client Code   : %s\n",hexbuf);
     success = gen_hex_string(&eph.ServerCode, sizeof(eph.ServerCode), flag, hexbuf, &hexbuf_len);
     printf("> Server Code   : %s\n",hexbuf);
     success = gen_hex_string(&eph.PacketLength, sizeof(eph.PacketLength), flag, hexbuf, &hexbuf_len);
     printf("> Packet Length : %s\n",hexbuf);
     success = gen_hex_string(&eph.CRC, sizeof(eph.CRC), flag, hexbuf, &hexbuf_len);    
     printf("> CRC Checksum  : %s\n",hexbuf);
     success = gen_hex_string(&eph.OpCode, sizeof(eph.OpCode), flag, hexbuf, &hexbuf_len);
     printf("> Op Code       : %s\n",hexbuf);
//
// Process OpCodes
//  
     if(eph.OpCode==E_CODE)
     {
       success = gen_hex_string(&eph.OpOption, sizeof(eph.OpOption), flag, hexbuf, &hexbuf_len);
       printf("> Op Option     : %s\n",hexbuf);
       printf("\n"); 
       switch(eph.OpOption)
       {
         case SESSION_REQUEST:    
//
           printf(" ***    Session Request from Client    ***\n");
           Process_Session_Request(buf,msglen,response,&resp_len);
//           Process_Dummy(buf,msglen,response,&resp_len);
           printf("\n"); 
           printf(" *** Processed Session Response Packet ***\n");
//
           printf("\n");
           success = gen_hex_string(response, resp_len, flag, hexbuf, &hexbuf_len);
           printf("> Outgoing Packet Data: %s\n",hexbuf);
           printf("> \n"); 
           break;
//           
         case SESSION_CLOSE:    
//
           printf(" *** Session Close Request from Client ***\n");
           // Will eventually need to close actual session here
           need_response = 0; // no response needed for now
           break;

         case SERVER_COMPLEX_REQUEST:    
//
           printf(" *** Server Complex List Request from Client ***\n");
           Process_Server_Complex_Request(buf,msglen,response,&resp_len);
//
           printf("\n");
           success = gen_hex_string(response, resp_len, flag, hexbuf, &hexbuf_len);
           printf("> Outgoing Packet Data: %s\n",hexbuf);
           printf("> \n"); 
           break;
        }
     }
     else if(eph.OpCode==CHARACTER_REQUEST)
     {
//
       printf(" *** Character List Request from Client ***\n"); 
       Process_Character_Request(buf,msglen,response,&resp_len);
//
       printf("\n");
       success = gen_hex_string(response, resp_len, flag, hexbuf, &hexbuf_len);
       printf("> Outgoing Packet Data: %s\n",hexbuf);
       printf("> \n"); 
     }
//
//
//  Some packets don't require a response, so allow for this
//     
    if (need_response ==1)
    {
       printf(" ***         CALCULATING NEW CRC          ***\n");
       printf("> \n");  
//     
       resp_len = resp_len -4; // remove old from calculating new one
       crc = crc_init();
       crc = crc_update(crc,response, resp_len);
       crc = crc_finalize(crc);
//     crc = htonl(crc); // switch the order   
       resp_len = resp_len+4;
//
       printf("> Newly Calculated Packet CRC: 0x%08X\n",htonl(crc));
     
       memcpy(response+resp_len-4,&crc,4);
       printf("\n");
       success = gen_hex_string(response, resp_len, flag, hexbuf, &hexbuf_len);
       printf("> Outgoing Packet Data: %s\n",hexbuf);
       printf("> \n");      
       printf("\n"); 
       printf("> Now sending packet back to client \n");
       printf("\n"); 

//
// Now send the packet
//     
	  if ((msglen = sendto(sockfd, response, resp_len, 0, (struct sockaddr *)&cliaddr, clilen)) < 0)
	  {
	      printf("Call to sendto failed\n");
	      exit(1);
	  }
   //
   //
   //
    }; //  Need Response If

//
//
//    
     printf("\n"); 
     printf("***************************************************************************\n");
  
    rNo++;
    printf("\n");
    } // this should end empty received message loop
    

    
    printf("\n");
    printf("> Exiting Main Send/Recv Loop\n");
    printf("\n");
    return;
}

///////////////////////////////////////////////////////////////////////////
//
// StopServer
//  
void StopServer(int sockfd)
{
    int end;
    
    printf("> Stopping Server...\n");
    printf("\n");
    
    if(sockfd<0) 
    {
        printf("Invalid Socket File Descriptor, exiting....\n");
        return;
    }
    
    if ((end = close(sockfd)) < 0) 
    {
        perror("  ERROR: Close Failed");
        printf("  Exiting Server with Error\n");
    }
    else
    {
        printf("> Server Stopped\n");
        printf("\n");
    }   
    return;
}
//////////////////////////////////////////////////////////////////////////// 