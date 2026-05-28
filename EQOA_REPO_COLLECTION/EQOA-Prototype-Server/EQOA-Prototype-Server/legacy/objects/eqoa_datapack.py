'''

created december 25th, 2016
@author: Devin Dallas
'''

import eqoa_utilities

class dataPack():

    def __init__(self, packid, packkey1, packkey2, packvalue):
        
        self.packid    = packid
        self.packkey1  = packkey1
        self.packkey2  = packkey2
        self.packvalue = packvalue
        
    def encodepack(self):
    
        packingList = [];
        encode_fmt = '<l'; packingList.append(self.packid)          
        encode_fmt += 'l'; packingList.append(self.packkey1)        
        encode_fmt += 'l'; packingList.append(self.packkey2)          
        encode_fmt += 'l'; packingList.append(self.packvalue)     
        #
        encodedDataString = eqoa_utilities.packFixed(encode_fmt,packingList)
        
        return encodedDataString
    
    def printDataPack(self):
    
        print
        print ' PACKID                 :    {:8f}'.format(self.packid)
        print ' PACKKEY1               :    {:8f}'.format(self.packkey1)
        print ' PACKKEY2               :    {:8f}'.format(self.packkey2)
        print ' PACKVALUE              :    {:8f}'.format(self.packvalue)
        print