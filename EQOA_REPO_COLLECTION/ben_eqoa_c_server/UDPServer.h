int StartServer(const char* ipname,const char* portname);

void HandlePacket(unsigned char**, int);

void SendRecvLoop(int sockfd);
void StopServer(int sockfd);