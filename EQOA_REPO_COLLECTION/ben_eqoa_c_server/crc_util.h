//
// Prototypes
//
int gen_hex_string(void* packet, int len, int flag, unsigned char* hexbuf, int *hexbuf_len);
int AppendCRC(unsigned char* hexstr, unsigned char* packet, int pSize);