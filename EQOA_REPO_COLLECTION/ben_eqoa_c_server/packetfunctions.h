//
typedef struct __attribute__((packed)) packetheader
{
    uint16_t clientcode;
    uint16_t servercode;
    uint8_t  plength;
	uint16_t opcode; 
};
//prototypes - put in h files
int sendReply(unsigned char*, int, int, struct sockaddr*, socklen_t, int);
//
void changeCodes(packetheader, unsigned char*, unsigned char*); 
void changeCodesv2(packetheader, unsigned char*, unsigned char*);
