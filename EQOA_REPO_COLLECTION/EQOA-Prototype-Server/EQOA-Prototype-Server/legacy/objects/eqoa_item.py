'''
Created December 11th, 2016

@author: Devin Dallas
'''
import eqoa_utilities
import struct
           
WHERE_EQUIPPED = eqoa_utilities.enum(   NOT_EQUIPPED   = - 1,
                                        HELM           =   0,
                                        GLOVES         =   1,
                                        LEFT_EAR       =   2,
                                        RIGHT_EAR      =   3,
                                        NECK           =   4,
                                        CHEST          =   5,
                                        RIGHT_BRACELET =   6,
                                        LEFT_BRACELET  =   7,
                                        BRACERS        =   8,
                                        LEFT_RING      =   9,
                                        RIGHT_RIGHT    =  10,
                                        BELT           =  11,
                                        LEGS           =  12,
                                        FEET           =  13,
                                        PRIMARY        =  14,
                                        SECONDARY      =  15,
                                        UNKNOWN1       =  16,
                                        UNKNOWN2       =  17,
                                        UNKNOWN3       =  18,
                                        UNKNOWN4       =  19,
                                        UNKNOWN5       =  20,
                                        ROBE           =  21,
                                        UNKNOWN6       =  22,
                                        UNKNOWN7       =  23)    
                                        
ITEM_LOCATION = eqoa_utilities.enum(   INVENTORY   = - 1,
                                       BANK        =   1,
                                       UNKNOWN     =   0,
                                       AUCTION     =   4) 

EQUIPMENTSLOT = eqoa_utilities.enum(    NON_EQUIPMENT = - 1,
                                        HELM          =   1,
                                        ROBE          =   2,
                                        EARRING       =   3,
                                        NECK          =   4,
                                        TORSO         =   5,
                                        BRACELET      =   6,
                                        BRACERS       =   7,
                                        RING          =   8,
                                        BELT          =   9,
                                        LEGS          =   10,
                                        BOOTS         =   11,
                                        PRIMARY       =   12,
                                        SHIELD        =   13,
                                        SECONDARY     =   14,
                                        TWO_HAND      =   15,
                                        BOW           =   16,
                                        THROW         =   17,
                                        HELD          =   18,
                                        GLOVES        =   19,
                                        FISHING       =   20,
                                        BAIT          =   21,
                                        WEAPON_CRAFT  =   22,
                                        ARMOR_CRAFT   =   23,
                                        TAILORING     =   24,
                                        JEWEL_CRAFT   =   25,
                                        CARPENTRY     =   26,
                                        ALCHEMY       =   27) 
 
STAT = eqoa_utilities.enum( STA       = 0,
                            STR       = 1,
                            AGI       = 2,
                            DEX       = 3,
                            WIS       = 4,
                            INT       = 5,
                            CHA       = 6,
                            UNKNOWN1  = 7,
                            HPMAX     = 8,
                            UNKNOWN2  = 9,
                            POWMAX    = 10,
                            UNKNOWN3  = 11,
                            HOT       = 12,
                            POT       = 13,
                            AC        = 14,
                            UNKNOWN4  = 15,
                            UNKNOWN5  = 16,
                            UNKNOWN6  = 17,
                            UNKNOWN7  = 18,
                            UNKNOWN8  = 19,
                            UNKNOWN9  = 20,
                            UNKNOWN10 = 21,
                            PR        = 22,
                            DR        = 23,
                            FR        = 24,
                            CR        = 25,
                            LR        = 26,
                            AR        = 27,
                            UNKNOWN11 = 28,
                            UNKNOWN12 = 29,
                            UNKNOWN13 = 30)
                            
TRADEABLE = eqoa_utilities.enum( YES = 0,
                                 NO  = 1)  
                                
RENTABLE = eqoa_utilities.enum( NO  = 0,
                                YES = 1) 
                                
LORE = eqoa_utilities.enum( NO  = 0,                               
                            YES = 1)                                       
#
#########################################################################################
#   

class item():
    #Basic item information
    def __init__(self, stacks_remain, hp_remain, charge_remain, patternObject):
        #
        self.stacks_remain = stacks_remain
        self.hp_remain     = hp_remain
        self.charge_remain = charge_remain
        self.pattern       = patternObject

    def encodeItem(self):
        #
        packingList = [ self.stacks_remain,      
                        self.hp_remain, 
                        self.charge_remain]

        encodedItemString = eqoa_utilities.packVariable(packingList)
        
        #encodedItemString += self.pattern.encodeItemPattern()
        
        return encodedItemString;
        
    def printItem(self):
    
        print ' ITEM QUANTITIES'
        print ' -----------------------'
        print ' STACKS_REMAIN  :    {:8d}'.format(self.stacks_remain)
        print ' HP_REMAIN      :    {:8d}'.format(self.hp_remain)
        print ' CHARGE_REMAIN  :    {:8d}'.format(self.charge_remain)
        print
        self.pattern.printItemPattern()


class gotItem():
    #Process of gathering unique item information
    
    def __init__(self, where_equipped, location, num_in_list, itemObject):

      self.where_equipped = where_equipped
      self.location       = location
      self.num_in_list    = num_in_list
      self.item           = itemObject      
        
    def encodeGotItem(self):
      #
      encodedGotItem = self.item.encodeItem()
      #
      packingList = [ self.where_equipped,
                      self.location]    
      encodedGotItem += eqoa_utilities.packVariable(packingList)
      packingList = [];
      encode_fmt = '<I'; packingList.append(self.num_in_list)  #
      encodedGotItem += eqoa_utilities.packFixed(encode_fmt,packingList)
      #
      encodedGotItem += self.item.pattern.encodeItemPattern()
      #   
      return encodedGotItem;
   
    def printGotItem(self):
      #    
      print
      print ' GOT ITEM QUANTITIES'
      print ' -----------------------'
      print ' WHERE_EQUIPPED :    {:8x}'.format(self.where_equipped)
      print ' ITEM_LOCATION  :    {:8x}'.format(self.location)
      print ' NUM IN LIST    :    {:8d}'.format(self.num_in_list)
      print 
      self.item.printItem()
      
      
class gotItemList():

    def __init__(self, myGotItemList):
        #
        self.myGotItemList = myGotItemList
        
    def encodeGotItemList(self):
        #
        encodedGotItemList = ''
        #
        for i in self.myGotItemList:
            encodedGotItemList += i.encodeGotItem()
        #
        return encodedGotItemList;
        
    def printGotItemList(self):
        #
        print 'List of Got Items'
        print 'Total Number of Items :',len(self.myGotItemList)
        print '-----------------'
        for i,item in enumerate(self.myGotItemList):
            print
            print 'ITEM '+'{:d}'.format(i+1) + ' of '+'{:d}'.format(len(self.myGotItemList))            
            item.printGotItem()
            print
        print  
        print '==================================================='
        print     
  
#
#########################################################################################
#     
class itemPattern():
    #Processing new item pattern information
    def __init__(self, pattern_id, pattern_family, unknown1, itemicon, unknown2, equipmentslot, unknown3, tradeable, rentable, unknown4, attacktype, weapondamage, unknown5, levelreq, maxstack, hpmax, duration, toon_class, race,procanimation, lore, unknown6, craftable, item_name, item_desc, statObject):
    #
        self.pattern_id    = pattern_id
        self.pattern_family= pattern_family
        self.unknown1      = unknown1
        self.itemicon      = itemicon
        self.unknown2      = unknown2
        self.equipmentslot = equipmentslot
        self.unknown3      = unknown3
        self.tradeable     = tradeable
        self.rentable      = rentable
        self.unknown4      = unknown4
        self.attacktype    = attacktype
        self.weapondamage  = weapondamage
        self.unknown5      = unknown5
        self.levelreq      = levelreq
        self.maxstack      = maxstack
        self.hpmax         = hpmax
        self.duration      = duration
        self.toon_class    = toon_class
        self.race          = race
        self.procanimation = procanimation
        self.lore          = lore
        self.unknown6      = unknown6
        self.craftable     = craftable
        self.item_name     = item_name
        self.item_desc     = item_desc        
        self.statObject    = statObject
    
    def encodeItemPattern(self):
      #
      packingList = [self.pattern_id,
                     self.pattern_family,
                     self.unknown1,
                     self.itemicon,
                     self.unknown2, 
                     self.equipmentslot,
                     self.unknown3,
                     self.tradeable,
                     self.rentable,                                    
                     self.unknown4,
                     self.attacktype,
                     self.weapondamage,
                     self.unknown5,                                        
                     self.levelreq,
                     self.maxstack,
                     self.hpmax,
                     self.duration,
                     self.toon_class,
                     self.race, 
                     self.procanimation,
                     self.lore,
                     self.unknown6,
                     self.craftable]
      #              
      encodedItemString = eqoa_utilities.packVariable(packingList)
      #
      packingList = [self.item_name,
                     self.item_desc]       
      
      encodedItemString += eqoa_utilities.packStringAsUnicode(packingList)
      #
      encodedItemString += self.statObject.encodeStat()
      #
      return encodedItemString;

      
    def printItemPattern(self):
      #    
      print ' ITEM PATTERN QUANTITIES'
      print ' -----------------------'
      print ' PATTERN ID    :    {:8d}'.format(self.pattern_id)
      print ' PATTERN FAMILY:    {:8d}'.format(self.pattern_family)
      print ' UNKNOWN 1     :    {:8d}'.format(self.unknown1)
      print ' ITEM ICON     :    {:8d}'.format(self.itemicon)
      print ' UNKNOWN 2     :    {:8x}'.format(self.unknown2)
      print ' ITEM SLOT     :    {:8x}'.format(self.equipmentslot)
      print ' UNKNOWN 3     :    {:8d}'.format(self.unknown3)
      print ' TRADEABLE     :    {:8d}'.format(self.tradeable)
      print ' RENTABLE      :    {:8x}'.format(self.rentable)
      print ' UNKNOWN 4     :    {:8d}'.format(self.unknown4)
      print ' ATTACK TYPE   :    {:8d}'.format(self.attacktype)
      print ' WEAPON DAMAGE :    {:8d}'.format(self.weapondamage)
      print ' UNKNOWN 5     :    {:8x}'.format(self.unknown5)
      print ' LEVEL REQ     :    {:8x}'.format(self.levelreq)
      print ' MAX STACKS    :    {:8d}'.format(self.maxstack)
      print ' MAX HP        :    {:8d}'.format(self.hpmax)
      print ' DURATION      :    {:8x}'.format(self.duration)
      print ' CLASS         :    {:8d}'.format(self.toon_class)
      print ' RACE          :    {:8d}'.format(self.race)
      print ' PROC/ANIMA    :    {:8d}'.format(self.procanimation)
      print ' LORE          :    {:8x}'.format(self.lore)
      print ' UNKNOWN 6     :    {:8x}'.format(self.unknown6)
      print ' CRAFTABLE     :    {:8d}'.format(self.craftable)
      print ' ITEM NAME     :    {:s}'.format(self.item_name)
      print ' ITEM DESC     :    {:s}'.format(self.item_desc)
      print    
      #
      self.statObject.printShortStat()
      
#
#########################################################################################
#  

class Stat():
#Processes stat's of gear information

    def __init__(self):
            self.STA       = 0
            self.STR       = 0
            self.AGI       = 0
            self.DEX       = 0
            self.WIS       = 0
            self.INT       = 0
            self.CHA       = 0
            self.UNKNOWN1  = 0
            self.HPMAX     = 0
            self.UNKNOWN2  = 0
            self.POWMAX    = 0
            self.UNKNOWN3  = 0
            self.HOT       = 0
            self.POT       = 0
            self.AC        = 0
            self.UNKNOWN4  = 0
            self.UNKNOWN5  = 0
            self.UNKNOWN6  = 0
            self.UNKNOWN7  = 0
            self.UNKNOWN8  = 0
            self.UNKNOWN9  = 0
            self.UNKNOWN10 = 0
            self.PR        = 0
            self.DR        = 0
            self.FR        = 0
            self.CR        = 0
            self.LR        = 0
            self.AR        = 0
            self.UNKNOWN11 = 0
            self.UNKNOWN12 = 0
            self.UNKNOWN13 = 0  

    def buildStat(self, STA, STR, AGI, DEX, WIS, INT, CHA, UNKNOWN1, HPMAX, UNKNOWN2, POWMAX, UNKNOWN3, HOT, POT, AC,
                UNKNOWN4, UNKNOWN5, UNKNOWN6, UNKNOWN7, UNKNOWN8, UNKNOWN9, UNKNOWN10, PR, DR, FR, CR, LR, AR, 
                UNKNOWN11, UNKNOWN12, UNKNOWN13):
                    
            self.STA       = STA
            self.STR       = STR
            self.AGI       = AGI
            self.DEX       = DEX
            self.WIS       = WIS
            self.INT       = INT
            self.CHA       = CHA
            self.UNKNOWN1  = UNKNOWN1
            self.HPMAX     = HPMAX
            self.UNKNOWN2  = UNKNOWN2
            self.POWMAX    = POWMAX
            self.UNKNOWN3  = UNKNOWN3
            self.HOT       = HOT
            self.POT       = POT
            self.AC        = AC
            self.UNKNOWN4  = UNKNOWN4
            self.UNKNOWN5  = UNKNOWN5
            self.UNKNOWN6  = UNKNOWN6
            self.UNKNOWN7  = UNKNOWN7
            self.UNKNOWN8  = UNKNOWN8
            self.UNKNOWN9  = UNKNOWN9
            self.UNKNOWN10 = UNKNOWN10
            self.PR        = PR
            self.DR        = DR
            self.FR        = FR
            self.CR        = CR
            self.LR        = LR
            self.AR        = AR
            self.UNKNOWN11 = UNKNOWN11
            self.UNKNOWN12 = UNKNOWN12
            self.UNKNOWN13 = UNKNOWN13
            
            
    def buildStatFromList(self,statList):
      #
      # Init a full 31 STAT list to zeros
      #
      myFullStatList = [0 for i in range(31)]
      #
      # Then loop over lists in the statList and assign them to the 
      # correct position/index
      #     
      for statPair in statList:  
        myFullStatList[statPair[0]] = int(statPair[1])
      #
      # Now we have a full Stat List and call just call the normal build
      #      
      self.buildStat(*myFullStatList)
            

    def encodeStat(self):
      
      myShortStatList = []
      if self.STA != 0:      myShortStatList.append([STAT.STA,self.STA])
      if self.STR != 0:      myShortStatList.append([STAT.STR,self.STR])
      if self.AGI != 0:      myShortStatList.append([STAT.AGI,self.AGI])
      if self.DEX != 0:      myShortStatList.append([STAT.DEX,self.DEX])
      if self.WIS != 0:      myShortStatList.append([STAT.WIS,self.WIS])
      if self.INT != 0:      myShortStatList.append([STAT.INT,self.INT])
      if self.CHA != 0:      myShortStatList.append([STAT.CHA,self.CHA])
      if self.UNKNOWN1 != 0: myShortStatList.append([STAT.UNKNOWN1,self.UNKNOWN1])
      if self.HPMAX != 0:    myShortStatList.append([STAT.HPMAX,self.HPMAX])
      if self.UNKNOWN2 != 0: myShortStatList.append([STAT.UNKNOWN2,self.UNKNOWN2])
      if self.POWMAX != 0:   myShortStatList.append([STAT.POWMAX,self.POWMAX])
      if self.UNKNOWN3 != 0: myShortStatList.append([STAT.UNKNOWN3,self.UNKNOWN3])
      if self.HOT != 0:      myShortStatList.append([STAT.HOT,self.HOT])
      if self.POT != 0:      myShortStatList.append([STAT.POT,self.POT])
      if self.AC != 0:       myShortStatList.append([STAT.AC,self.AC])
      if self.UNKNOWN4 != 0: myShortStatList.append([STAT.UNKNOWN4,self.UNKNOWN4])
      if self.UNKNOWN5 != 0: myShortStatList.append([STAT.UNKNOWN5,self.UNKNOWN5])
      if self.UNKNOWN6 != 0: myShortStatList.append([STAT.UNKNOWN6,self.UNKNOWN6])
      if self.UNKNOWN7 != 0: myShortStatList.append([STAT.UNKNOWN7,self.UNKNOWN7])
      if self.UNKNOWN8 != 0: myShortStatList.append([STAT.UNKNOWN8,self.UNKNOWN8])
      if self.UNKNOWN9 != 0: myShortStatList.append([STAT.UNKNOWN,self.UNKNOWN9])
      if self.UNKNOWN10 != 0:myShortStatList.append([STAT.UNKNOWN10,self.UNKNOWN10])
      if self.PR != 0:       myShortStatList.append([STAT.PR,self.PR])
      if self.DR != 0:       myShortStatList.append([STAT.DR,self.DR])
      if self.FR != 0:       myShortStatList.append([STAT.FR,self.FR])
      if self.CR != 0:       myShortStatList.append([STAT.CR,self.CR])
      if self.LR != 0:       myShortStatList.append([STAT.LR,self.LR])
      if self.AR != 0:       myShortStatList.append([STAT.AR,self.AR])
      if self.UNKNOWN11 != 0:myShortStatList.append([STAT.UNKNOWN11,self.UNKNOWN11])
      if self.UNKNOWN12 != 0:myShortStatList.append([STAT.UNKNOWN12,self.UNKNOWN12])
      if self.UNKNOWN13 != 0:myShortStatList.append([STAT.UNKNOWN13,self.UNKNOWN13])
      #
      # Expand short list to master list
      #
      flattenedList = [item for sublist in myShortStatList for item in sublist]
      #
      # Add length of original list of list to front of aout
      #
      flattenedList.insert(0,len(myShortStatList))
      # 
      # Actually pack the Variable list  in flattenedList
      # 
      encodedStatString = eqoa_utilities.packVariable(flattenedList)
      #
      return encodedStatString;
  
   
    def printFullStat(self):
      #    
      print ' FULL STAT ATTRIBUTES'
      print ' --------------------'      
      print ' STA      :    {:8d}'.format(self.STA)
      print ' STR      :    {:8d}'.format(self.STR)
      print ' AGI      :    {:8d}'.format(self.AGI)
      print ' DEX      :    {:8d}'.format(self.DEX)
      print ' WIS      :    {:8d}'.format(self.WIS)
      print ' INT      :    {:8d}'.format(self.INT)
      print ' CHA      :    {:8d}'.format(self.CHA)
      print ' UNKNOWN1 :    {:8d}'.format(self.UNKNOWN1)
      print ' HPMAX    :    {:8d}'.format(self.HPMAX)
      print ' UNKNOWN2 :    {:8d}'.format(self.UNKNOWN2)
      print ' POWMAX   :    {:8d}'.format(self.POWMAX)
      print ' UNKNOWN3 :    {:8d}'.format(self.UNKNOWN3)
      print ' HOT      :    {:8d}'.format(self.HOT)
      print ' POT      :    {:8d}'.format(self.POT)
      print ' AC       :    {:8d}'.format(self.AC)
      print ' UNKNOWN4 :    {:8d}'.format(self.UNKNOWN4)
      print ' UNKNOWN5 :    {:8d}'.format(self.UNKNOWN5)
      print ' UNKNOWN6 :    {:8d}'.format(self.UNKNOWN6)
      print ' UNKNOWN7 :    {:8d}'.format(self.UNKNOWN7)
      print ' UNKNOWN8 :    {:8d}'.format(self.UNKNOWN8)
      print ' UNKNOWN9 :    {:8d}'.format(self.UNKNOWN9)
      print ' UNKNOWN10:    {:8d}'.format(self.UNKNOWN10)
      print ' PR       :    {:8d}'.format(self.PR)
      print ' DR       :    {:8d}'.format(self.DR)
      print ' FR       :    {:8d}'.format(self.FR)
      print ' CR       :    {:8d}'.format(self.CR)
      print ' LR       :    {:8d}'.format(self.LR)
      print ' AR       :    {:8d}'.format(self.AR)
      print ' UNKNOWN11:    {:8d}'.format(self.UNKNOWN11)
      print ' UNKNOWN12:    {:8d}'.format(self.UNKNOWN12)
      print ' UNKNOWN13:    {:8d}'.format(self.UNKNOWN13)
      print
      
      
    def printShortStat(self):
      #    
      print ' SHORT STAT ATTRIBUTES'
      print ' ---------------------'  
      if self.STA != 0:      print ' STA      :    {:8d}'.format(self.STA) 
      if self.STR != 0:      print ' STR      :    {:8d}'.format(self.STR) 
      if self.AGI != 0:      print ' AGI      :    {:8d}'.format(self.AGI) 
      if self.DEX != 0:      print ' DEX      :    {:8d}'.format(self.DEX) 
      if self.WIS != 0:      print ' WIS      :    {:8d}'.format(self.WIS) 
      if self.INT != 0:      print ' INT      :    {:8d}'.format(self.INT) 
      if self.CHA != 0:      print ' CHA      :    {:8d}'.format(self.CHA) 
      if self.UNKNOWN1 != 0: print ' UNKNOWN1 :    {:8d}'.format(self.UNKNOWN1)
      if self.HPMAX != 0:    print ' HPMAX    :    {:8d}'.format(self.HPMAX)
      if self.UNKNOWN2 != 0: print ' UNKNOWN2 :    {:8d}'.format(self.UNKNOWN2)
      if self.POWMAX != 0:   print ' POWMAX   :    {:8d}'.format(self.POWMAX) 
      if self.UNKNOWN3 != 0: print ' UNKNOWN3 :    {:8d}'.format(self.UNKNOWN3)
      if self.HOT != 0:      print ' HOT      :    {:8d}'.format(self.HOT)
      if self.POT != 0:      print ' POT      :    {:8d}'.format(self.POT) 
      if self.AC != 0:       print ' AC       :    {:8d}'.format(self.AC)
      if self.UNKNOWN4 != 0: print ' UNKNOWN4 :    {:8d}'.format(self.UNKNOWN4)
      if self.UNKNOWN5 != 0: print ' UNKNOWN5 :    {:8d}'.format(self.UNKNOWN5)
      if self.UNKNOWN6 != 0: print ' UNKNOWN6 :    {:8d}'.format(self.UNKNOWN6)
      if self.UNKNOWN7 != 0: print ' UNKNOWN7 :    {:8d}'.format(self.UNKNOWN7)
      if self.UNKNOWN8 != 0: print ' UNKNOWN8 :    {:8d}'.format(self.UNKNOWN8)
      if self.UNKNOWN9 != 0: print ' UNKNOWN9 :    {:8d}'.format(self.UNKNOWN9)
      if self.UNKNOWN10 != 0:print ' UNKNOWN10:    {:8d}'.format(self.UNKNOWN10) 
      if self.PR != 0:       print ' PR       :    {:8d}'.format(self.PR)
      if self.DR != 0:       print ' DR       :    {:8d}'.format(self.DR)
      if self.FR != 0:       print ' FR       :    {:8d}'.format(self.FR)
      if self.CR != 0:       print ' CR       :    {:8d}'.format(self.CR)
      if self.LR != 0:       print ' LR       :    {:8d}'.format(self.LR)
      if self.AR != 0:       print ' AR       :    {:8d}'.format(self.AR)
      if self.UNKNOWN11 != 0:print ' UNKNOWN11:    {:8d}'.format(self.UNKNOWN11)
      if self.UNKNOWN12 != 0:print ' UNKNOWN12:    {:8d}'.format(self.UNKNOWN12)
      if self.UNKNOWN13 != 0:print ' UNKNOWN13:    {:8d}'.format(self.UNKNOWN13)
      



              
              