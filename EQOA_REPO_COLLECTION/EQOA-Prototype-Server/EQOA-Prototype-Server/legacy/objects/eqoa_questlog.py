'''
Created December 23rd, 2016

@author: Devin Dallas
'''

import eqoa_utilities

class questlog():

    def __init__ (self, questLogText):
    
        self.questLogText = questLogText
        
    def encodeQuestLog(self):
    
        packingList = [ self.questLogText]
        
        encodedQuestLog = eqoa_utilities.packStringAsUnicode(packingList)
        
        return encodedQuestLog;
        
    def printQuestLog(self):
    
        print 
        print 'Quest Log Description: {:s}'.format(self.questLogText)
        print
        
class questloglist():

    def __init__ (self, myQuestLogList):
    
        self.questLogList = myQuestLogList

    def encodeQuestLogList(self):
      #         
      encodedQuestLogList = ''
      for wbl in self.questLogList:
        encodedQuestLogList += wbl.encodeQuestLog()

      #  
      return encodedQuestLogList 
        
        
    def printQuestLogList(self):
        
      print 'Quest Log List'
      print '------------------------------'   
      for i,qll in enumerate(self.questLogList):
        print 'Quest Log Number      : {:8d}'.format(i+1)
        print 'Quest Log Desc.       : {:s}'.format(qll.questLogText)
        print           