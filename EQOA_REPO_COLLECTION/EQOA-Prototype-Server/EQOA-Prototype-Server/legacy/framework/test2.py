import struct, binascii
from eqoa_messages import LoginMessage, GameVersionMessage

class ProcessData:

    def __init__(self, GVM, loginMessage):
        self.my_messageType   = None
        self.my_messageLength = None
        self.my_messageNumber = None
        self.my_messageOpcode = None
        self.my_bundleNumber  = 0
        self.response         = None
        self.clientBundle     = None
        self.loginMessage     = loginMessage
        self.GVM              = GVM
        
    def processBundle(self, payload):
        
        print('One or more message to process')
            
        hdr_fmt1 = '<H'   # (little endian)[OPCODE]
        s = struct.unpack(hdr_fmt1,payload[0:2]) #  
        self.clientBundle = s[0]
        print('Message {} from client'.format(self.clientBundle))
        payload = payload[2:]
        
        while True:

            if payload != None:
                hdr_fmt1 = '<BBHH'   # (little endian)[OPCODE]
                s = struct.unpack(hdr_fmt1,payload[0:6]) #  
                self.my_messageType   = s[0]
                self.my_messageLength = s[1]
                self.my_messageNumber = s[2]
                self.my_messageOpcode = s[3]
                payload          = payload[4:]

                if self.my_messageOpcode == 0x0000:
                    payload = self.GVM.decodeMessage(payload) # Should we make this method do a check for 'EQOA', then return a true or false? 
                    # Might be a good idea otherwise may causing unforseen errors between client/server
                    #result = GVM.decodeMEssage(payload)
                    # if result == True:
                    # response.append('01')
                    self.response = self.GVM.encodeMessage()

                elif self.my_messageOpcode == 0x904:
                    payload = self.loginMessage.decodeMessage(payload)
                    
            else:
                print('Read entire bundle')
                break       

            
    def generateResponse(self):
        self.my_bundleNumber =+ 1
        encode_fmt = '<HHH{}s'.format(len(self.response))
        response = struct.pack(encode_fmt, self.my_bundleNumber, self.clientBundle, self.my_messageNumber, self.response)
        
        print(response)
            
payload = bytearray.fromhex('20 01 00 FB 06 01 00 00 00 25 00 00 00 FB 3E 02 00 04 09 00 03 00 00 00 04 00 00 00 45 51 4F 41 0A 00 00 00 6B 69 65 73 68 61 65 73 68 61 01 FA 10 69 22 1C D4 45 BC FD 68 3C 56 22 87 D9 70 B7 1C 12 AE 76 C4 98 FD F3 CE EB 44 4A 0A 49 B5')
loginMessage = LoginMessage()
GVM = GameVersionMessage()

hdr_fmt1 = '<b'   # (little endian)[OPCODE]
s = struct.unpack(hdr_fmt1,payload[0:1]) #  
my_bundleType = s[0]
payload = payload[1:]

process = ProcessData(GVM, loginMessage)
process.processBundle(payload)
process.generateResponse()