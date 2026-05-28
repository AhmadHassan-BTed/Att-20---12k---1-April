#!/usr/bin/env python
#
#
# Devin and Ben
# June 28, 2018
#

from sqlalchemy import *
from sqlalchemy import Column, Integer, String
from sqlalchemy import create_engine, ForeignKey
from sqlalchemy.orm  import sessionmaker, relationship
from sqlalchemy.exc import SQLAlchemyError
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.dialects import mysql
from sqlalchemy_utils import database_exists, create_database

import configparser

#
#####################################################################################################
#
Base = declarative_base()

def sessionCreate(login, pwd, db):
    
    configfile_name = "./Creator_config.ini"
    config = configparser.ConfigParser()
    config.read(configfile_name)\
    
    engine_string  = 'mysql://'
    engine_string += login + ':'
    engine_string += pwd + '@'
    host           = db
    engine_string += host   + '/'
    database       = 'creator'
    engine_string += database
    #
    
    print('Using this to attach to db: ',engine_string)
    try:
        engine = create_engine(engine_string)
        
    except SQLAlchemyError as e:
        print(e)
        print('An error occured with the Database connection')

    Base.metadata.bind = engine
    
    Session = sessionmaker(bind=engine)
    session = Session()
    #May take this out in final product, database should always be there and normal users won't have permission to create or destroy
    if not database_exists(engine.url):
        create_database(engine.url)
        Base.metadata.create_all(engine)
        task = createBasicData(session)
        task.create()
  
    print('Does the database exist?: ',database_exists(engine.url))

    
    string = "SELECT CURRENT_ROLE;"
    
    users = engine.execute(string).fetchall() 
    priv = users[0][0]
    access = None
    if priv == 'Admin':
        access = 0
        
    elif priv == 'Creator':
        access = 1
        
    elif priv == 'Editor':
        access = 2
        
    elif priv == 'QC':
        access = 3
        
    elif priv == 'Viewer':
        access = 4
        
    #No permissions for database per Roles    
    else:
        pass
    

    session.autocommit = False
    

    Base.metadata.create_all(engine)
    return session, database, access
    
#
#####################################################################################################
#

class equipTypes(Base):
    __tablename__ = "equipTypes"

    hexid            = Column(INT, nullable = False, primary_key = True)
    equipmenttype    = Column(VARCHAR(32), primary_key = True)

#
###################################################################################
#

class attackTypes(Base):
    __tablename__ = "attackTypes"
    
    hexid         = Column(Integer, nullable = False, primary_key = True)
    attacktype    = Column(VARCHAR(32), primary_key = True)    

#
###################################################################################
#

class procAnimation(Base):
    __tablename__ = "procAnimation"
    
    hexid         = Column(INT, nullable = False, primary_key = True)
    procAnim      = Column(VARCHAR(32))    

#
###################################################################################
#
'''
class toonClass(Base):
    __tablename__ = "toonClass"
    
    hexid         = Column(INT, nullable = False, primary_key = True)
    toonclass        = Column(VARCHAR(32), nullable = False, primary_key = True)    
 
#
###################################################################################
#

class toonRace(Base):
    __tablename__ = "toonRace"
    
    hexid       = Column(INT
, nullable = False, primary_key = True)
    toonrace    = Column(VARCHAR(32), nullable = False, primary_key = True)    
#
###################################################################################
#
'''

class regions(Base):
    __tablename__ = "regions"
    
    region    = Column(VARCHAR(32), primary_key = True)

#
#######################################################
#

class icons(Base):  
    __tablename__ = "icons"
     
    iconhex    = Column(BigInteger, primary_key = True)
    image      = Column(BLOB)
    iconname   = Column(VARCHAR(48))
    iconid     = Column(INT, ForeignKey("icontype.iconid"))
    
    iconsid    = relationship("icontype", foreign_keys=[iconid])
#
#############################################################################################
#


class icontype(Base):
    __tablename__ = "icontype"
    
    iconid      = Column(INT, primary_key = True)
    icontype    = Column(VARCHAR(32), primary_key = True)

#
#############################################################################################
#

class item(Base):
    __tablename__ = "item"
    
    itemhex     = Column(BigInteger, primary_key = True)
    image       = Column(BLOB)
    itemname    = Column(VARCHAR(60))
    itemid      = Column(INT, ForeignKey("itemtype.itemid")) 
    
    itemstype  = relationship("itemtype", foreign_keys=[itemid]) 
#
#############################################################################################
#

class itemtype(Base):
    __tablename__ = "itemtype"
    
    itemid      = Column(INT, primary_key = True)
    itemtype    = Column(VARCHAR(32), primary_key = True)
    
#
#################################################################################################
#

class Helm(Base):
    __tablename__ = 'Helm'
    
    itemhex     = Column(VARCHAR(10), primary_key = True)
    image       = Column(BLOB)
    itemname    = Column(VARCHAR(60), primary_key = True)
    
    
#
##############################################################################################
#

class Robe(Base):
    __tablename__ = 'Robe'
    
    itemhex     = Column(VARCHAR(10), primary_key = True)
    image       = Column(BLOB)
    itemname    = Column(VARCHAR(60), primary_key = True)
    
#
##############################################################################################
#

class Armor(Base):
    __tablename__ = 'Armor'
    
    itemhex     = Column(VARCHAR(10), primary_key = True)
    image       = Column(BLOB)
    itemname    = Column(VARCHAR(60), primary_key = True)

#
#############################################################################################
#

class Rarity(Base):
    __tablename__ = 'Rarity'
    
    rarity      = Column(VARCHAR(24), primary_key = True)
    
#
#############################################################################################
#

class items(Base):
    __tablename__ = "items"
    
    Itemid         = Column(INT, nullable = False, primary_key = True)
    Itemfamily     = Column(VARCHAR(32))
    Itemicon       = Column(BigInteger, ForeignKey("icons.iconhex"))
    Slot           = Column(INT, ForeignKey("equipTypes.hexid"), nullable = False)
    Trade          = Column(INT, nullable = False)
    Rent           = Column(INT, nullable = False)
    Attacktype     = Column(INT, ForeignKey("attackTypes.hexid"), nullable = False)
    Damage         = Column(INT, nullable = False)
    Level          = Column(INT, nullable = False)
    Maxstack       = Column(INT, nullable = False)
    HPMAX          = Column(INT, nullable = False)
    Durability     = Column(INT, nullable = False)
    Class          = Column(INT, nullable = False)
    Race           = Column(INT, nullable = False)
    Wepproc        = Column(INT)
    Armproc        = Column(INT)
    Procname       = Column(VARCHAR(24))
    Lore           = Column(INT, nullable = False)
    Craft          = Column(INT, nullable = False)
    Itemname       = Column(VARCHAR(32), nullable = False)
    Itemdesc       = Column(VARCHAR(128), nullable = False)
    Strength       = Column(INT)
    Stamina        = Column(INT)
    Agility        = Column(INT)
    Dexterity      = Column(INT)
    Wisdom         = Column(INT)
    Intelligence   = Column(INT)
    Charisma       = Column(INT)
    HP_MAX         = Column(INT)
    PWR_MAX        = Column(INT)
    PoT            = Column(INT) 
    HoT            = Column(INT)
    AC             = Column(INT)
    PR             = Column(INT)
    DR             = Column(INT)
    FR             = Column(INT)
    CR             = Column(INT)
    LR             = Column(INT)
    AR             = Column(INT)
    Rarity         = Column(VARCHAR(24))
    region         = Column(VARCHAR(32))
    Imagegraphic   = Column(BigInteger, ForeignKey("item.itemhex"))
    Armor          = Column(VARCHAR(10), ForeignKey("Armor.itemhex"))
    Helm           = Column(VARCHAR(10), ForeignKey("Helm.itemhex"))
    Robe           = Column(VARCHAR(10), ForeignKey("Robe.itemhex"))
    Color          = Column(BigInteger)
    Creator        = Column(VARCHAR(20), nullable = False)
    Creatortime    = Column(DATETIME)
    Modifier       = Column(VARCHAR(20))
    Modifiertime   = Column(DATETIME)
    Verifier       = Column(VARCHAR(20))
    Verifiertime   = Column(DATETIME)
    Notes          = Column(VARCHAR(256))
    
    iconRelation   = relationship("icons", foreign_keys=[Itemicon])
    itemRelation   = relationship("item", foreign_keys=[Imagegraphic])
    atkRelation    = relationship("attackTypes", foreign_keys=[Attacktype])
    equipRelation  = relationship("equipTypes", foreign_keys=[Slot])
    armorRelation  = relationship("Armor", foreign_keys=[Armor])
    helmRelation   = relationship("Helm", foreign_keys=[Helm])
    robeRelation   = relationship("Robe", foreign_keys=[Robe])

#
###################################################################################
#
def acquireIcons():
    icon_key_dict = {10:'Armor',15:'Epic',20:'Fish',25:'Food',30:'Gem',35:'Jewlery',40:'Misc.',45:'Robe',50:'Shield',55:'Wand',60:'Weapon'}
    #
    # Read and Sort
    #
    image = 0
    f = open('./Item_Icons_All.txt', 'r')
    all_icons = f.readlines()
    all_icons.sort()
    #
    item = []
    obj = [0, b'0', 0, 0]
    item.append(obj)
    count = 0
    for key in icon_key_dict:
        for line in all_icons:
            codes = []; desc = [];
            icontype = int(line[0:2])
            if icontype == key:
                count = count +1     
                codes.append(line[5:7])
                codes.append(line[8:10])
                codes.append(line[11:13])
                codes.append(line[14:16])
                desc.append(line[19:])
                giffile = ''.join(codes[::-1])+'.gif'
                icon_id = "".join(codes) 
                big_endian = 0
                cr = codes[::-1]
                big_endian = '0x'+''.join(cr)
                try:
                    with open('./ICONS/'+giffile, 'rb') as f:
                        image = f.read()
                    #image = '../Downloads/ICONS/'+giffile
                except:
                    #print('Error importing {}'.format(giffile))
                    image = 0
                icon = [int(big_endian, 16), image, desc[0].rstrip('\r\n'), icontype]

                item.append(icon)
    return item
#
#####################################################################################
#

def acquireItems():
    item_key_dict = {2:'Axes',4:'Bows',6:'Clubs',8:'Crossbows',10:'Daggers',12:'God Gear',14:'Epics',16:'Foils',18:'Great Axes',20:'Great Clubs',22:'Great Hammers',24:'Great Swords',26:'Hammers',28:'Knuckles',30:'Long Swords',32:'Maces',34:'Martial Rods',36:'Pole Arms',38:'Short Swords',40:'Spears',42:'Staves', 50: 'Armor', 52: 'Helm', 54: 'Robe',62:'Ammunition',64:'Books',66:'Totems',68:'Wands',80:'Shields'}
    #
    # Read and Sort
    #
    image = 0
    f = open('./3dImages.txt', 'r')
    all_icons = f.readlines()
    all_icons.sort()
    #
    item  = []
    armor = []
    helm  = []
    robe  = []
    count = 0
    for key in item_key_dict:
        for line in all_icons:
            codes = [];
            icons = line.split("-", 3)
            icontype = int(icons[0])
            if icontype == key:
                count = count +1 
                giffile = icons[3].lstrip(" ").rstrip(" \r\n")
                desc = icons[2]
                icon_id = "".join(icons[1]) 
                big_endian = 0
                cr = icons[1]
                big_endian = '0x'+''.join(cr)
                try:
                    with open('./3dgifs/'+giffile, 'rb') as f:
                        image = f.read()
                    
                    #image = '../Downloads/ICONS/'+giffile
                except:
                
                    if giffile == None or giffile == '':
                        print('No Gif for None type (Armor or Helm)')
                    else:
                        print('Error importing {}.gif'.format(giffile))
                        image = 0
                            
                if icontype == 50:
                    graphic = [int(icon_id, 16), image, desc.rstrip('\r\n').replace(' ', '')]
                    armor.append(graphic)
                    
                elif icontype == 54:
                    graphic = [int(icon_id, 16), image, desc.rstrip('\r\n').replace(' ', '')]
                    robe.append(graphic)
                
                elif icontype == 52:
                    graphic = [int(icon_id, 16), image, desc.rstrip('\r\n').replace(' ', '')]
                    helm.append(graphic)
                
                else:
                    icon = [int(icon_id, 16), image, desc.rstrip('\r\n').replace(' ', ''), icontype]
                    item.append(icon)

        icon_id  = None
        image    = None
        desc     = None
        icontype = None

    return item, armor, helm, robe
#
###########################################################################################
#

class createBasicData:
    
    def __init__(self, session):
        self.session = session
        self.regions = ['Artic',
                        'Desert',
                        'Forest',
                        'Generic',
                        'Marine',
                        'Mountain',
                        'Plains',
                        'Swamp',
                        'Deep Jungle (Fronts)',
                        'Rathe Mountains (Fronts)',
                        'Red Desert (Fronts)',
                        'Wastelands (Fronts)',
                        'Wetlands (Fronts)']
        self.rarity    = ['Common',
                          'Uncommon',
                          'Rare',
                          'Ultra Rare',
                          'Unique']
        self.iconTypes = [[0, 'NONE'],
                          [10,'Armor'],
                          [15,'Epic'],
                          [20,'Fish'],
                          [25,'Food'],
                          [30,'Gem'],
                          [35,'Jewlery'],
                          [40,'Misc.'],
                          [45,'Robe'],
                          [50,'Shield'],
                          [55,'Wand'],
                          [60,'Weapon']]
                          
        self.imageTypes = [[0, 'NONE'],
                           [2,'Axes'],
                           [4,'Bows'],
                           [6,'Clubs'],
                           [8,'Crossbows'],
                           [10,'Daggers'],
                           [12,'God Gear'],
                           [14,'Epics'],
                           [16,'Foils'],
                           [18,'Great Axes'],
                           [20,'Great Clubs'],
                           [22,'Great Hammers'],
                           [24,'Great Swords'],
                           [26,'Hammers'],
                           [28,'Knuckles'],
                           [30,'Long Swords'],
                           [32,'Maces'],
                           [34,'Martial Rods'],
                           [36,'Pole Arms'],
                           [38,'Short Swords'],
                           [40,'Spears'],
                           [42,'Staves'],
                           [62,'Ammunition'],
                           [64,'Books'],
                           [66,'Totems'],
                           [68,'Wands'],
                           [80,'Shields']]


        
        self.AttackTypes = [[0, 'NONE'],
                            [1, '1HS'],
                            [2, '2HS'],
                            [3, '1HB'],
                            [4, '2HB'],
                            [5, '1HP'],
                            [6, '2HP'],
                            [7, 'BOW'],
                            [8, '1H CROSSBOW'],
                            [9, '2H CROSSBOW'],
                            [10, 'THROWN']]
        
        self.eqpType = [[0, 'NONE'],
                        [1, 'HELM'],
                        [2, 'ROBE'],
                        [3, 'EARRING'],
                        [4, 'NECK'],
                        [5, 'CHEST'],
                        [6, 'BRACELET'],
                        [7, 'BRACERS'],
                        [8, 'RING'],
                        [9, 'BELT'],
                        [10, 'LEGS'],
                        [11, 'FEET'],
                        [12, 'PRIMARY'],
                        [13, 'SHIELD'],
                        [14, 'SECONDARY'],
                        [15, '2 HAND'],
                        [16, 'BOW'],
                        [17, 'THROWN'],
                        [18, 'HELD'],
                        [19, 'GLOVES'],
                        [20, 'FISHING'],
                        [21, 'BAIT'],
                        [22, 'WEAPON CRAFT'],
                        [23, 'ARMOR CRAFT'],
                        [24, 'TAILORING'],
                        [25, 'JEWEL CRAFT'],
                        [26, 'CARPENTRY'],
                        [27, 'ALCHEMY']]
        
        self.procAnim = [[0, 'None'],
                         [10, 'Corrosive'],
                         [40, 'Draught']]

        self.icons    = None
        self.items    = None
        self.armor    = None
        self.helm     = None
        self.robe     = None
        
    def create(self):
        len1 = len(self.AttackTypes)
        len2 = len(self.eqpType)
        len3 = len(self.procAnim)
        print('Writing Weapon Attack types to database... ', end = '')
        for i in range(len1):
            atkType = attackTypes(hexid      = self.AttackTypes[i][0],
                                  attacktype = self.AttackTypes[i][1])
            
            try:
                self.session.add(atkType)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                print(str(e))
        print('completed.')

        print('Writing Equipment Types to database... ', end = '')
        for i in range(len2):
            eqpType = equipTypes(hexid         = self.eqpType[i][0],
                                equipmenttype  = self.eqpType[i][1])
             
            try:
                self.session.add(eqpType)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                print(str(e))
        print('completed.')

        print('Writing Proc Animations to database... ', end = '')
        for i in range(len3):
            Procanim = procAnimation(hexid         = self.procAnim[i][0],
                                     procAnim      = self.procAnim[i][1])
            
            try:
                self.session.add(Procanim)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                print(str(e))
        print('completed.')
        
        len7 = len(self.iconTypes)
        print('Writing Icon Types to Database... ', end = '')
        for i in range(len7):
            obj = icontype(iconid   = self.iconTypes[i][0],
                           icontype = self.iconTypes[i][1])
            
            try:
                self.session.add(obj)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                #pass
                print(str(e))
        print('completed.')
        
        self.icons = acquireIcons()
        len5 = len(self.icons)
        print('Writing Icons base to database... ', end = '')
        for i in range(len5):
            Icons = icons(iconhex   = self.icons[i][0],
                          image     = self.icons[i][1],
                          iconname  = self.icons[i][2],
                          iconid    = self.icons[i][3])
            try:
                self.session.add(Icons)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                #pass
                print(str(e))
        print('completed.')
        
        len6 = len(self.regions)
        print('Writing Regions to Database... ', end = '')
        for i in range(len6):
            Reg = regions(region = self.regions[i])
            
            try:
                self.session.add(Reg)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                #pass
                print(str(e))
        print('completed.')
        
        len9 = len(self.imageTypes)
        for i in range(len9):
            obj = itemtype(itemid   = self.imageTypes[i][0],
                           itemtype = self.imageTypes[i][1])
            
            try:
                self.session.add(obj)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                #pass
                print(str(e))
        print('Writing Item Types to Database')        
        
        self.items, self.armor, self.helm, self.robe = acquireItems()
        len8 = len(self.items)
        print('Writing 3dImages to Database... ', end = '')
        for i in range(len8):
            Items = item(itemhex  = self.items[i][0],
                         image    = self.items[i][1],
                         itemname = self.items[i][2],
                         itemid   = self.items[i][3])
            
            try:
                self.session.add(Items)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                #pass
                print(str(e))
        print('completed.')
        
        len9 = len(self.armor)
        print('Writing 3D Armor to Database... ', end = '')
        for i in range(len9):
            armor = Armor(itemhex  = self.armor[i][0],
                          image    = self.armor[i][1],
                          itemname = self.armor[i][2])
            
            try:
                self.session.add(armor)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                #pass
                print(str(e))
        print('completed.')
        
        len10 = len(self.helm)
        print('Writing 3d Helms to Database... ', end = '')        
        for i in range(len10):
            helm  = Helm(itemhex  = self.helm[i][0],
                         image    = self.helm[i][1],
                         itemname = self.helm[i][2])
            
            try:
                self.session.add(helm)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                #pass
                print(str(e))
        print('completed.')
        
        len11 = len(self.robe)
        print('Writing 3dImages to Database... ', end = '')
        for i in range(len11):
            robe  = Robe(itemhex  = self.robe[i][0],
                         image    = self.robe[i][1],
                         itemname = self.robe[i][2])
            try:
                self.session.add(robe)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                #pass
                print(str(e))
        print('completed.')
            
        len12 = len(self.rarity)
        print('Writing Rarity to Database... ', end = '')
        for i in range(len12):
            myRarity  = Rarity(rarity  = self.rarity[i])
            
            try:
                self.session.add(myRarity)
                self.session.flush()
                self.session.commit()
                self.session.close()
                
            except SQLAlchemyError as e:
                #pass
                print(str(e))
        print('completed.')
