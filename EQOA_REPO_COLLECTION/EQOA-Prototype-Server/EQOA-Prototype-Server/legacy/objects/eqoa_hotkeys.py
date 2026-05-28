'''
Created December 21st, 2016

@author: Devin Dallas
'''

import eqoa_utilities
import eqoa_hotkeys
import itertools


#
BRANCH     = ' '
PRINTLABEL = ''
actionCodeDict = {'|0' :'Wave', '|1' :'Bow','|2' :'Point','|3' :'Cheer','|4' :'Invite','|5' :'Boot Member',
                  '|6' :'Leave Group','|7' :'Ignore Target','|8' :'Stop Ignore','|9' :'Privacy List',
                  '|10':'Ignore Shouts','|11':'Restore Shouts','|12':'Ignore Guild','|13':'Restore Guild',
                  '|14':'Say','|15':'Group','|16':'Shout','|17':'Guild','|18':'Reply','|19':'Tell','|20':'Message',
                  '|21':'Assist Leader','|22':'Assist Target','|23':'Roll','|24':'Pet Passive','|25':'Pet Defensive',
                  '|26':'Pet Aggressive','|27':'Pet Neutral','|28':'Pet Dismiss','|29':'Pet Backoff','|30':'Pet Attack',
                  '|31':'Follow','|32':'Time','|33':'Alarm','|34':'Duel','|35':'Surrender'}


COMPASS    = eqoa_utilities.enum(
                    NONE  = 0x99,
                    NORTH = 0x00,
                    WEST  = 0x01,
                    EAST  = 0x02,
                    SOUTH = 0x03)
                    
COMPASS_STRING = ['NORTH', 'WEST', 'EAST', 'SOUTH']                    
#    
############################################
#
# Gets menuID from list of direction. N-N-N-N = [1,1,1,1], N-W-E-S = [1,2,3,4]
#
class hotkeyMenuId:
    
    def __init__(self):
      self.dirList   = []
      self.dirString = ''
      self.menuId    = 0x00
        
    def listToId(self):    
       sum = 0
       for i,dir in enumerate(self.dirList):
         if dir > 0:
           sum += + 1 +(dir-1)*(4**(i+1))
       self.menuId = sum    
    #
    def IdToList(self):   # dictionary probably better this maybe slow
      #  
      ans = [0]; outlist = []  # First generate all possible options
      for rr in range(0,4):
        templist  = list(itertools.product('12345', repeat=rr))
        for entry in templist:
          entry = [int(i) for i in entry]
          outlist.append(list(entry))
      outlist[0] = [0]   # First entry is blank, use to specify base menu
      #      
      for olist in outlist:   # Now evaluate all options till match found
        sum=0
        for i,dir in enumerate(olist):
         if dir > 0:
           sum += + 1 +(dir-1)*(4**(i+1)) 
        if sum == self.menuId:
          ans = olist
          break
      self.dirList= ans    

    def listToString(self):    
       myListDict = {1:'N',2:'W',3:'E',4:'S'}
       myString = ''
       for i,dir in enumerate(self.dirList):
         myString += myListDict[dir] + '-'
       myString = myString[:-1]
       self.dirString = myString
       
    def StringToList(self):    
       myStringdict = {'N':1,'W':2,'E':3,'S':4}
       myList = self.dirString.split('-')
       for i,value in enumerate(myList):
          myList[i] = myStringdict[value]
       self.dirList = myList
           
#
class hotkeyMenuList:
#
    def __init__(self,myhotkeyMenuList):
      self.myhotkeyMenuList = myhotkeyMenuList   

    def encodeHotkeyMenuList(self):  # this is not complete - 
      #           
      # think I need another value in here. check PCAP
      encodedHotkeyMenuListString = eqoa_utilities.packVariable([len(self.myhotkeyMenuList)])   # packVariable uses technique  
      #
      for index,menu in enumerate(self.myhotkeyMenuList):
         encodedHotkeyMenuListString += menu.encodeHotkeyMenu()
      #
      return  encodedHotkeyMenuListString       
 
    def printHotkeyMenuList(self):
      #    
      print 
      print ' NUM HOTKEY MENUS :   {:}'.format(len(self.myhotkeyMenuList))
        
      for index,menu in enumerate(self.myhotkeyMenuList):
         print ' MENU           : ' + '{:3}'.format(index+1) + ' of ' + '{:}'.format(len(self.myhotkeyMenuList))
         menu.printHotkeyMenu()   
         print         
      #
      print
      return          
        
#
class hotkeyMenu:
#
    def __init__(self):
      self.menuID = 0x00   # Default - Center
      nullHotkeyTab = eqoa_hotkeys.hotkeyTab(BRANCH,'Null')  
      self.hotkeyTabList = [nullHotkeyTab,nullHotkeyTab,nullHotkeyTab,nullHotkeyTab]  
#
    def buildHotkeyMenu(self,menuID,hotkeyTabList):
        self.menuID     = menuID
        self.hotkeyTabList = hotkeyTabList
#       
    def encodeHotkeyMenu(self):
      #        
      encodedHotkeyMenuString = eqoa_utilities.packVariable([self.menuID]) 
      #
      for index,tab in enumerate(self.hotkeyTabList):
        encodedHotkeyMenuString += eqoa_utilities.packVariable([index])        
        encodedHotkeyMenuString += tab.encodeHotkeyTab()
      #
      return  encodedHotkeyMenuString   
      
    def printHotkeyMenu(self):    
      #
      print ' MENU ID        :    0x{:04x}'.format(self.menuID)
      print ' MENU ID STRING :    {:}'.format(self.menuID)  # here it will query menu ID
      print ' -------------------------------------------'
      for index,tab in enumerate(self.hotkeyTabList):
        print ' TAB: {:6}'.format(COMPASS_STRING[index]) + '(0x{:02d})'.format(index),
        tab.printHotkeyTab()     
      #
      return       
      
      
class hotkeyTab:
  #
  def __init__(self,action,label):
    self.label        = label
    self.action       = action
        
  def encodeHotkeyTab(self):
      #
      packingList = [ self.action,
                      self.label]  
      #  
      encodedHotkeyTabString = eqoa_utilities.packStringAsUnicode(packingList)
      #
      return  encodedHotkeyTabString  
        
  def printHotkeyTab(self):
      #
      # could also put in command codes
      #
      if self.action == BRANCH:
        printme =  ' ACTION: BRANCH'        
      elif self.action == PRINTLABEL:
        printme =  ' ACTION: PRINT_LABEL'
      elif self.action[0] == '|':  # command code
        printme =  ' ACTION: COMMAND CODE {:}'.format(actionCodeDict.get(self.action))        
      else:
        printme =  ' ACTION: WRITE {:}'.format(self.action)
        
      print printme + '   with LABEL: ''{:}'' '.format(self.label)

    
        
        
        
    