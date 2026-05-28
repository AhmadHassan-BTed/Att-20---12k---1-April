from sqlalchemy import *
from sqlalchemy import Column, Integer, String
from sqlalchemy import create_engine, ForeignKey
from sqlalchemy.orm  import sessionmaker, relationship
from sqlalchemy.exc import SQLAlchemyError
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy_utils import database_exists, create_database

import configparser

engine_string = 'mysql://fooUser:fooPass@0.0.0.0/eqoabase'

#print('Using this to attach to db: ',engine_string)
engine = create_engine(engine_string, pool_pre_ping = True)
if not database_exists(engine.url):
    create_database(engine.url)
#print('Does the database exist?: ',database_exists(engine.url))

Base = declarative_base()
Base.metadata.bind = engine

Session = sessionmaker(bind=engine)

def aMethod():
    session = Session()
    session.autocommit = False
    return session

# Decent start to define a character within it's own table.
class Characters(Base):
    __tablename__ = "Characters"
    
    charName     = Column(VARCHAR(32), nullable = False, primary_key = True)    
    accountid    = Column(INT, nullable = False)
    serverid     = Column(INT, nullable = False, primary_key = True)
    modelid      = Column(BigInteger, nullable = False)
    tclass       = Column(INT, nullable = False)
    race         = Column(INT, nullable = False)
    level        = Column(INT, nullable = False)
    haircolor    = Column(INT, nullable = False)
    hairlength   = Column(INT, nullable = False)
    hairstyle    = Column(INT, nullable = False)
    faceoption   = Column(INT, nullable = False)
    classIcon    = Column(INT, nullable = False) # Test this once in world. Thought is it is menu ui Class icon
    totalXP      = Column(INT, nullable = False)
    debt         = Column(INT, nullable = False)
    breath       = Column(INT, nullable = False)
    tunar        = Column(INT, nullable = False)
    bankTunar    = Column(INT, nullable = False)
    unusedTP     = Column(INT, nullable = False)
    totalTP      = Column(INT, nullable = False)
    world        = Column(INT, nullable = False)
    x            = Column(BigInteger, nullable = False)
    y            = Column(BigInteger, nullable = False)
    z            = Column(BigInteger, nullable = False)
    facing       = Column(BigInteger, nullable = False)
    unknown      = Column(BigInteger, nullable = False)
    
    #
#
class Hotkeys(Base):
    __tablename__ = "Hotkeys"
    hotkeyid     = Column(INT, autoincrement = True, nullable = False, primary_key = True)
    charid       = Column(INT, nullable = False)
    direction    = Column(VARCHAR(10), nullable = False)
    Nlabel       = Column(VARCHAR(128))
    Nmessage     = Column(VARCHAR(128))
    Wlabel       = Column(VARCHAR(128))
    Wmessage     = Column(VARCHAR(128))
    Elabel       = Column(VARCHAR(128))
    Emessage     = Column(VARCHAR(128))
    Slabel       = Column(VARCHAR(128))
    Smessage     = Column(VARCHAR(128))

    
class itemPattern(Base):
	__tablename__ = "itemPattern"
	
	patternid    = Column(INT, nullable = False, primary_key = True)
	patternfam   = Column(INT, nullable = False)
	unk1         = Column(INT, nullable = False)
	itemicon     = Column(BigInteger, nullable = False)
	unk2         = Column(INT, nullable = False)
	equipslot    = Column(INT, nullable = False)
	unk3         = Column(INT, nullable = False)
	trade        = Column(INT, nullable = False)
	rent         = Column(INT, nullable = False)
	unk4         = Column(INT, nullable = False)
	attacktype   = Column(INT, nullable = False)
	weapondamage = Column(INT, nullable = False)
	unk5         = Column(INT, nullable = False)
	levelreq     = Column(INT, nullable = False)
	maxstack     = Column(INT, nullable = False)
	maxhp        = Column(INT, nullable = False)
	duration     = Column(INT, nullable = False)
	classuse     = Column(INT, nullable = False)
	raceuse      = Column(INT, nullable = False)
	procanim     = Column(INT, nullable = False)
	lore         = Column(INT, nullable = False)
	unk6         = Column(INT, nullable = False)
	craft        = Column(INT, nullable = False)
	itemname     = Column(VARCHAR(32), nullable = False)
	itemdesc     = Column(VARCHAR(256), nullable = False)
	model        = Column(BigInteger, nullable = False)
	color        = Column(BigInteger, nullable = False)
	str          = Column(INT) 
	sta          = Column(INT) 
	agi          = Column(INT) 
	wis          = Column(INT) 
	dex          = Column(INT) 
	cha          = Column(INT) 
	int          = Column(INT) 
	HPMAX        = Column(INT) 
	POWMAX       = Column(INT) 
	PoT          = Column(INT) 
	HoT          = Column(INT) 
	AC           = Column(INT) 
	PR           = Column(INT) 
	DR           = Column(INT) 
	FR           = Column(INT) 
	CR           = Column(INT) 
	LR           = Column(INT)
	AR           = Column(INT)


class charInventory(Base):
    __tablename__ = "charInventory"

    itemid       = Column(INT, autoincrement = True, nullable = False, primary_key = True)
    serverid     = Column(INT, nullable = False)
    stackleft    = Column(INT, nullable = False)
    remainHP     = Column(INT, nullable = False)
    remaincharge = Column(INT, nullable = False)
    patternid    = Column(INT, nullable = False)
    equiploc     = Column(INT, nullable = False)
    location     = Column(INT, nullable = False)
    listnumber   = Column(INT, nullable = False)	

class weaponHotBar(Base):
    __tablename__ = "weaponHotBar"
    
    weaponsetID   = Column(INT, nullable = False, autoincrement = True, primary_key = True)
    charid        = Column(INT, nullable = False)
    hotbarname    = Column(VARCHAR(64), nullable = False)
    weaponID      = Column(INT, nullable = False)	
    secondaryID   = Column(INT, nullable = False)

class Spells(Base):
    __tablename__ = "Spells"	
    id            = Column(INT, nullable = False, autoincrement = True, primary_key = True)
    charid        = Column(INT, nullable = False)
    spellid       = Column(INT, nullable = False) 
    addedorder    = Column(INT, nullable = False)
    onHotBar      = Column(INT, nullable = False)
    whereonBar    = Column(INT, nullable = False)
    unk1          = Column(INT, nullable = False)
    showhide      = Column(INT, nullable = False)    


class spellPattern(Base):
    __tablename__ = "spellPattern"
    spellid       = Column(INT, nullable = False, primary_key = True)
    abilitylvl    = Column(INT, nullable = False)
    unk2          = Column(INT, nullable = False)
    unk3          = Column(INT, nullable = False)
    range         = Column(INT, nullable = False)
    casttime      = Column(INT, nullable = False)
    power         = Column(INT, nullable = False)
    iconColor     = Column(INT, nullable = False)
    icon          = Column(INT, nullable = False)
    scope         = Column(INT, nullable = False)
    recast        = Column(INT, nullable = False)
    eqprequire    = Column(INT, nullable = False)
    spellname     = Column(VARCHAR(32), nullable = False)
    spelldesc     = Column(VARCHAR(256), nullable = False)
	
#
###################################################################################
#
# Create all Tables 
#
Base.metadata.create_all(engine)
#
class createTestCharacters:
	
    def __init__(self, session):
    
        #our database session
        self.session = session
        
        #Build individual characters
        #Creates 3 character, dallasdevin, Beebster, and Leukocyte (accountid 2)
        self.characters = [['dallasdevin', 2, 1260509, 0x70D898C6, 0x00, 0x00, 0x3c, 0x06, 0x03, 0x03, 0x00, 0x00, 300000, 5625, 0xFF, 500000000, 100000000, 10, 350, 0, 0x5cecc546, 0xed805842, 0x8e117646, 0x9cebc5bf, 0x00],
                           ['Beebster', 2, 1260510, 0xE18919F7, 0x07, 0x01, 0x3c, 0x07, 0x02, 0x03, 0x02, 0x07, 25029476, 1, 0xFF, 345647345, 35000000, 15, 234, 0, 0xa98d5f46, 0x3eb16042, 0x793bb846, 0xdb0fc9bc, 0x00],
		                   ['Leukocyte', 2, 1260511, 0x4C6FA532, 0x0A, 0x05, 0x3c, 0x06, 0x03, 0x03, 0x00, 0x14, 45834723, 0, 0xFF, 0, 25000, 20, 12,  0, 0x817f8746, 0x5ea4fa42, 0xd8446c46, 0xe25ca33f, 0x00]]


        #Build unique items for individual characters
        #Current thought is we have characters serverid, which tells us this belongs to x character
        #for Simplicity, Remaining HP, charge, stacks etc will be 0 for now
        self.charInventory = [[1260509, 0x00, 0x00, 0x00, 31001, 0x02, 0x01, 0x00], #dallasdevin robe
                              [1260509, 0x00, 0x00, 0x00, 31002, 0x0C, 0x01, 0x01], #dallasdevin Primary
                              [1260509, 0x00, 0x00, 0x00, 31003, 0x0E, 0x01, 0x02], #dallasdevin secondary
                              [1260509, 0x00, 0x00, 0x00, 31000, 0x13, 0x01, 0x03], #dallasdevin gloves
                              [1260509, 0x00, 0x00, 0x00, 31004, 0x0B, 0x01, 0x04], #dallasdevin boots
                              [1260510, 0x00, 0x00, 0x00, 31005, 0x02, 0x01, 0x00], #Beebster robe
                              [1260510, 0x00, 0x00, 0x00, 31006, 0x0F, 0x01, 0x01], #Beebster 2hand Primary
                              [1260510, 0x00, 0x00, 0x00, 31007, 0x13, 0x01, 0x02], #Beebster gloves
                              [1260510, 0x00, 0x00, 0x00, 31008, 0x0B, 0x01, 0x03], #Beebster boots
			                  [1260511, 0x00, 0x00, 0x00, 31005, 0x02, -0x01, 0x00], #Leukocyte Robe
	       		              [1260511, 0x00, 0x00, 0x00, 31006, 0x0F, -0x01, 0x01], #Leukocyte 2hand Primary
		 	                  [1260511, 0x00, 0x00, 0x00, 31007, 0x13, -0x01, 0x02], #Leukocyte gloves
			                  [1260511, 0x00, 0x00, 0x00, 31008, 0x0B, -0x01, 0x04]] #Leukocyte Boots

        #Builds item patterns, that is,
        #Static information about an item that each object can pull to fully describe the item
        self.itemPattern = [[31000, 1, 0x00, 0xF2D9D224, 0x00, 0x13, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,   0x00, 0x3C, 0x01, 0x4844, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'Wind Golem Gloves', 'Basic Wind golem gloves', 0x04, 0x400080FF, 13, 65, 65, 76, 56, None, 112, None, 12, 987, None, 62, 23, 12, None, None, 927, 123], #plate gloves
                            [31001, 1, 0x00, 0x531DD90D, 0x00, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,   0x00, 0x3C, 0x01, 0x4844, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'Ceremonial Vestments', 'Rediculously Rare Robe, most would kill to have it', 0x00, 0xFFFFFFFF, 13, 65, 65, 76, 56, None, 112, None, 12, 987, None, 62, 23, 12, None, None, 927, 123], #all/all robe
                            [31002, 1, 0x00, 0xA05728FC, 0x00, 0x0C, 0x00, 0x01, 0x01, 0x00, 0x01, 0x03E7, 0x00, 0x3C, 0x01, 0x4844, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'Khal Sword', 'A gemless sword dropped by the Khal Warlord\'s', 0x6EB7224D, 0x00000000, 13, 65, 65, 76, 56, None, 112, None, 12, 987, None, 62, 23, 12, None, None, 927, 123], #Warrior Primary
                            [31003, 1, 0x00, 0x328F83B3, 0x00, 0x0E, 0x00, 0x01, 0x01, 0x00, 0x01, 0x03E7, 0x00, 0x3C, 0x01, 0x4844, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'Sapphire\'s Sword', 'Rare sword of Sapphire', 0xB44E7A96, 0x00000000, 13, 65, 65, 76, 56, None, 112, None, 12, 987, None, 62, 23, 12, None, None, 927, 123], #Warrior Secondary
                            [31004, 1, 0x00, 0x20BD4E47, 0x00, 0x0B, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,   0x00, 0x3C, 0x01, 0x4844, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'Wind Golem Boots', 'Basic wind golem boots', 0x04, 0x400080FF, 13, 65, 65, 76, 56, None, 112, None, 12, 987, None, 62, 23, 12, None, None, 927, 123], #Plate boots
                            [31005, 1, 0x00, 0xBB7C12BF, 0x00, 0x02, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,   0x00, 0x3C, 0x01, 0x4844, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'Khal Healer Robe', 'Rare robe from the legendary Khal Warlord', 0x02, 0xFAAF32FF, 13, 65, 65, 76, 56, None, 112, None, 12, 987, None, 62, 23, 12, None, None, 927, 123], #Healer Robe
                            [31006, 1, 0x00, 0x857E4829, 0x00, 0x0F, 0x00, 0x01, 0x01, 0x00, 0x04, 0x03E7, 0x00, 0x3C, 0x01, 0x4844, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'Staff of the Jade Forest', 'Legendary Staff from Nagafen', 0x2B340FD4, 0x00000000, 13, 65, 65, 76, 56, None, 112, None, 12, 987, None, 62, 23, 12, None, None, 927, 123], #Healer wep
                            [31007, 1, 0x00, 0x04930349, 0x00, 0x13, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,   0x00, 0x3C, 0x01, 0x4844, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'Healer gloves', 'These gloves are for healers', 0x02, 0x008000FF, 13, 65, 65, 76, 56, None, 112, None, 12, 987, None, 62, 23, 12, None, None, 927, 123], #Healer gloves
                            [31008, 1, 0x00, 0x6739D57E, 0x00, 0x0B, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,   0x00, 0x3C, 0x01, 0x4844, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'Healer boots', 'These boots are for healers', 0x02, 0x008000FF, 13, 65, 65, 76, 56, None, 112, None, 12, 987, None, 62, 23, 12, None, None, 927, 123]] #Healer boots
		
        #weaponHotbars
        self.weaponHotbar = [[1260509, 'Perfect Combo', 31002, 31003],
                             [1260509, 'Combo Perfect', 31003, 31002]]
                             
                             
        #Hotkeys 
        self.hotkeys      = [[1260509, 'Main Menu', ' ', 'Responses', '', 'Options', ' ', 'Group', ' ', 'Communicate'], 
                             [1260509, 'N', 'I don\'t care, I just want some taco bell..', 'i dont care', 'I don\'t know, I\'m mexican.','i dont know', None, 'coolness', ' ', 'More Responses'],
                             [1260509, 'W', ' ', 'Chat Mode', ' ', 'Pet Command', ' ', 'Grouping', ' ', 'Chat Filter'],
                             [1260509, 'WN', '|14', 'Normal Say', '|17', 'Guild Speak', '|15', 'Group Speak', '|16', 'Shout'],
                             [1260509, 'E', ' ', 'Attacking', ' ', 'Creation', ' ', 'Readiness', ' ', 'Important!'],
                             [1260509, 'EN', 'I am attacking %4', 'Pulling', '|22', 'Assist Target', '|21', 'Assist Leader', 'I am attacking %4, please assist me!', 'Assist me'],
                             [1260509, 'S', ' ', 'Player', ' ', 'Navigation', ' ', 'Ask Help', ' ', 'Action'],
                             [1260509, 'SN', 'Hail %4', 'Hail', '|19', 'Tell', '|18', 'Reply', 'Adios Amigos! %4', 'Goodbye'],
                             [1260509, 'WW', '|24', 'Passive', ' ', 'More Pet Commands', '|25', 'Defensive', '|26', 'Aggressive'],
                             [1260509, 'EW', '|4', 'Invite', ' ', 'Organization', '%2 %3 seeking group!', 'Need Group', ' ', 'Hunting'],
                             [1260509, 'SW', None, 'more', None, 'gasdf', None, 'more', None, 'more'],
                             [1260509, 'SWN', '%1\'s Daily Advice for today: You shouldn\'t say things about', 'DA', 'Yo quiero taco bell!', 'taco bell', 'illiterate people, you should write it.', 'DA2', 'Great beard of Zeus!!', 'zues beard'],
                             [1260509, 'NE', '|32', 'time', '|31', 'follow', '|33', 'alarm', 'IM ROOTING %4! DONT ATTACK %4! GET=====PETS=====OFF!! BACK UP!!', 'root'],
                             [1260509, 'WE', '|4', 'Invite', '|5', 'Boot Member', None, 'lfg/duel', '|6', 'Leave Group'],
                             [1260509, 'EE', 'Listo?', 'Ready?', None, 'Health/Power', 'Soy bueno ir!', 'Good to go!', 'I need to jump the border, I\'ll be right back in a minute!', 'Break 1min'],
                             [1260509, 'SE', None, 'phase', None, 'more', None, 'etc', None, 'more'],
                             [1260509, 'SEN', '/t %4 hello, would you by anychance want a guild?', 'guild tell', 'This is my spam hotkey, this is how I spam yr screen with spam.', 'spam', 'fukin Chuck Norris', 'chuck norris', 'Crid dra vilg ib oui pedlr ycc rua!', 'albhed stfu'],
                             [1260509, 'NS', None, 'Thanks', None, 'xtra', ' ', 'Greetings', ' ', 'Notifications'],
                             [1260509, 'NSN', 'Gracias gracias %4!', 'ty', 'Welcome back', 'wb', 'Congratulacions', 'cg', 'De nada, compadre.', 'De Nada'], 
                             [1260509, 'WS', '|7', 'Ignore Target', '|12', 'Ignore Guild', '|10', 'Ignore Shouts', ' ', 'Restore Commands'],
                             [1260509, 'ES', 'RUN! This is a border we cannot cross!', 'Retreat!', 'The Disconnection Fairy has struck again!', 'Link Dead!', 'HELP! I\'m being attacked, peel it off of me!', 'Peel!', 'MEDIC! I need healing badly!', 'Medic!'],
                             [1260509, 'SS', '|0', 'Wave', '|2', 'Point', '|3', 'Cheer', '|1', 'Bow'],
                             [1260509, 'WWW', '|27', 'Neutral', '|30', 'Pet Attack', '|29', 'Pet Backoff', '|28', 'Dismiss Pet'],
                             [1260509, 'EWW', 'Roll fo dis loot foo!', 'roll', 'Loot up dis loot foo!', 'Loot up!', '%4 Usted tiene gusto de agrupar?', 'Want Group?', '|23', 'Roll 0-99'],
                             [1260509, 'SWW', 'Fere mai la bouche %4!', 'shuddap', '/alarm 00:05', 'mantana alarm', 'If you were my midget, I\'d treat you like a princess %4.', 'midget', 'Fils de une chienne %4!', 'son of a b'],
                             [1260509, 'EEW', '%4 Su gusto del novia tiene gusto de la empanada de la cereza', 'yr gf', 'Low/out of power, I\'m resting.', 'power', '%4 Su meh enojado justo su extremo dei papa le violo...', 'mad', 'im not ready', 'ready'],
                             [1260509, 'SEW', 'LTT/LTS 655 assorted gems. PST. (I\'m serious.)', 'aucs', '%4 dicho me su un homo alegra del asno!!!', 'told me', 'Don\'t worry %4, I\'m mexican, not Michael Jackson.', 'mj', 'Lama mi tuercas %4!', 'lick'],
                             [1260509, 'NSW', None, None, 'Chupa mi pito %4!', 'chupa', None, None, None, None],
                             [1260509, 'SWE', 'Mexico\'s daily advice for today: Don\'t be gay, it\'s nasty.', 'DA 2', 'Montezuuma\'s Revenge has gotten you %4!!!!!!! MWAHAHAHAHAHAHA!!', 'revenge', 'SOW B1TCH!', 'sow', '/r Arch Magus', 'mea'],
                             [1260509, 'WEE', '|34', 'duel', '|35', 'surrender', '/lfg on', 'lfg', '/lfg off', 'not lfg'],
                             [1260509, 'SEE', 'Hola, como estas usted hoy %4?', 'Hello', '(D"-")D~~~~~~~~~~~~~~~~~~~~~~~~O(";")O,', 'kurby 2', 'o("-")o=========================c(\'-\'c)', 'kurby', 'THEY TOOK \'ER JERBS!', ' took jerbs'],
                             [1260509, 'NSE', 'Bueno.', 'Good', 'Mal.', 'Bad', 'Que pasa?', 'How\'s it?', 'Bien.', 'Okay'],
                             [1260509, 'EWS', 'Follow me.', 'follow me', 'Let\'s find another place to hunt.', 'New Hunt', 'Who wants to handle pulling?', 'Puller?', 'Where shall we hunt?', 'Where hunt?'],
                             [1260509, 'SWS', 'F is for friends who do stuff together! U is for u and me!', 'fu', 'why dont you have an ice cold glass of shut the hell up', 'cold stfu', 'N is 4 newhere n neplace n netime down here n da deep blue sea', 'n', 'Debt, debt, go away, come again another day!', 'debt'],
                             [1260509, 'SES', 'Filha de puta!', 'SoB', '/r Hail %4', 'adair hail', 'Cerrado el jode arribe!', 'stfu', 'praise be to yevon', 'yevon'],
                             [1260509, 'NSS', 'Sorry, I am jumpin the border at this moment.', 'Sorry, busy', 'I must jump the border and am logging out of %1, Adios!', 'Quit', 'I need to jump the border and will not be present for a while.', 'Long Break', 'It will be difficult for me to respond sometimes (I\'m mexican).', 'mexican'],
                             [1260509, 'WSS', '|9', 'Privacy List', '|13', 'Restore guild', '|11', 'Restore Shout', '|8', 'Stop Ignoring']]
                             
        
    def create(self):
        
        #Inputs Characters into database from list
        for i in range(len(self.characters)):
            print('Working on {}.'.format(self.characters[i][0]))
            character = Characters(charName     = self.characters[i][0],
                                   accountid    = self.characters[i][1],
                                   serverid     = self.characters[i][2],
                                   modelid      = self.characters[i][3],
                                   tclass       = self.characters[i][4],
                                   race         = self.characters[i][5],
                                   level        = self.characters[i][6],
                                   haircolor    = self.characters[i][7],
                                   hairlength   = self.characters[i][8],
                                   hairstyle    = self.characters[i][9],
                                   faceoption   = self.characters[i][10],
                                   classIcon    = self.characters[i][11],
                                   totalXP      = self.characters[i][12],
                                   debt         = self.characters[i][13],
                                   breath       = self.characters[i][14],
                                   tunar        = self.characters[i][15],
                                   bankTunar    = self.characters[i][16],
                                   unusedTP     = self.characters[i][17],
                                   totalTP      = self.characters[i][18],
                                   world        = self.characters[i][19],
                                   x            = self.characters[i][20],
                                   y            = self.characters[i][21],
                                   z            = self.characters[i][22],
                                   facing       = self.characters[i][23],
                                   unknown      = self.characters[i][24])
                                   
            try:
                self.session.add(character)
                self.session.flush()
                self.session.commit()
                self.session.close()
               
            except SQLAlchemyError as e:
                print(str(e))
                
        print('Characters have been entered into database')

        for i in range(len(self.charInventory)):
            charinv = charInventory(serverid     = self.charInventory[i][0],
                                    stackleft    = self.charInventory[i][1],
                                    remainHP     = self.charInventory[i][2],
                                    remaincharge = self.charInventory[i][3],
                                    patternid    = self.charInventory[i][4],
                                    equiploc     = self.charInventory[i][5],
                                    location     = self.charInventory[i][6],
                                    listnumber   = self.charInventory[i][7])
                                   
            try:
                self.session.add(charinv)
                self.session.flush()
                self.session.commit()
                self.session.close()
               
            except SQLAlchemyError as e:
                print(str(e))
                
        print('Character Items have been entered into database')        
        
        for i in range(len(self.itemPattern)):
            itemPat = itemPattern(patternid    = self.itemPattern[i][0],
                                  patternfam   = self.itemPattern[i][1],
                                  unk1         = self.itemPattern[i][2],
                                  itemicon     = self.itemPattern[i][3],
                                  unk2         = self.itemPattern[i][4],
                                  equipslot    = self.itemPattern[i][5],
                                  unk3         = self.itemPattern[i][6],
                                  trade        = self.itemPattern[i][7],
                                  rent         = self.itemPattern[i][8],
                                  unk4         = self.itemPattern[i][9],
                                  attacktype   = self.itemPattern[i][10],
                                  weapondamage = self.itemPattern[i][11],
                                  unk5         = self.itemPattern[i][12],
                                  levelreq     = self.itemPattern[i][13],
                                  maxstack     = self.itemPattern[i][14],
                                  maxhp        = self.itemPattern[i][15],
                                  duration     = self.itemPattern[i][16],
                                  classuse     = self.itemPattern[i][17],
                                  raceuse      = self.itemPattern[i][18],
                                  procanim     = self.itemPattern[i][19],
                                  lore         = self.itemPattern[i][20],
                                  unk6         = self.itemPattern[i][21],
                                  craft        = self.itemPattern[i][22],
                                  itemname     = self.itemPattern[i][23],
                                  itemdesc     = self.itemPattern[i][24],
                                  model        = self.itemPattern[i][25],
                                  color        = self.itemPattern[i][26],
                                  str          = self.itemPattern[i][27],
                                  sta          = self.itemPattern[i][28],
                                  agi          = self.itemPattern[i][29],
                                  wis          = self.itemPattern[i][30],
                                  dex          = self.itemPattern[i][31],
                                  cha          = self.itemPattern[i][32],
                                  int          = self.itemPattern[i][33],
                                  HPMAX        = self.itemPattern[i][34],
                                  POWMAX       = self.itemPattern[i][35],
                                  PoT          = self.itemPattern[i][36],
                                  HoT          = self.itemPattern[i][37],
                                  AC           = self.itemPattern[i][38],
                                  PR           = self.itemPattern[i][39],
                                  DR           = self.itemPattern[i][40],
                                  FR           = self.itemPattern[i][41],
                                  CR           = self.itemPattern[i][42],
                                  LR           = self.itemPattern[i][43],
                                  AR           = self.itemPattern[i][44])
                                  
            try:
                self.session.add(itemPat)
                self.session.flush()
                self.session.commit()
                self.session.close()
               
            except SQLAlchemyError as e:
                print(str(e))
                
        print('Item patterns have been entered into database')
        
        for i in range(len(self.weaponHotbar)):
            weps = weaponHotBar(charid      = self.weaponHotbar[i][0],
                                hotbarname  = self.weaponHotbar[i][1],
                                weaponID    = self.weaponHotbar[i][2],
                                secondaryID = self.weaponHotbar[i][3])
                                   
            try:
                self.session.add(weps)
                self.session.flush()
                self.session.commit()
                self.session.close()
               
            except SQLAlchemyError as e:
                print(str(e))
                 
        print('Weapon Hotbars entered into Database')
        
        for i in range(len(self.hotkeys)):
            hots = Hotkeys(charid       = self.hotkeys[i][0],
                           direction    = self.hotkeys[i][1],
                           Nlabel       = self.hotkeys[i][3],
                           Nmessage     = self.hotkeys[i][2],
                           Wlabel       = self.hotkeys[i][5],
                           Wmessage     = self.hotkeys[i][4],
                           Elabel       = self.hotkeys[i][7],
                           Emessage     = self.hotkeys[i][6],
                           Slabel       = self.hotkeys[i][9],
                           Smessage     = self.hotkeys[i][8])
                            
            try:
                self.session.add(hots)
                self.session.flush()
                self.session.commit()
                self.session.close()
               
            except SQLAlchemyError as e:
                print(str(e))
                
        print('Hotkeys entered into Database')
        
#session = aMethod()
#creation = createTestCharacters(session)
#creation.create()
