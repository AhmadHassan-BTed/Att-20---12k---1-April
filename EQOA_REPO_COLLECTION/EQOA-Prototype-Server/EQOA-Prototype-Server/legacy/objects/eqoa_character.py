

def enum(**enums):
   return type('Enum', (), enums)
   
   
#####################################################################
##########  These enums are for Dudderz toons only.  ################
#####################################################################						 
CHAR_SERVERID	=	enum(FERRY        = 0x018BA090,  
                         DAYDRIFT     = 0x018BBBC6,  
                         LEAR         = 0x018BD5FA,  
						 KENCADE      = 0X018D89C4,
						 HYMNOFPOWER  = 0x019381F2,
						 NECNOK		  = 0x0194C3AC,
						 DUDDERZ      = 0X0195C3F6,
						 CORSTENSBANK = 0X0199EFBA)
						 
CHAR_PRIHAND = enum(DEFAULT         =  0X00000000,
					CLERIC_EPIC_GH  =  0XD8406C85,
					WIZ_EPIC_STAFF  =  0X7C7DBEA9,
					WAR_EPIC_LS		=  0X17F9870B,
					SK_EPIC_GS	    =  0X6EB7224D)
					
CHAR_SECHAND = enum(DEFAULT        = 0X00000000,
					LEATHER_BOOK   = 0XBF7E7DCB,
					LIGHT_TOTEM    = 0X992CE4A8,
					BRD_EPIC_RAP   = 0XF4DD991A,
					ENC_WAND_CHARM = 0X0C088B7C,
					WAR_EPIC_LS	   = 0X17F9870B)
					
CHAR_SHIELD =  enum(NOSHIELD      = 0X00000000,
					FORCE_SHIELD  = 0X2EC56029,
					BANGLE_SHIELD = 0X0AA6E873) 					
#####################################################################
#####################################################################
#####################################################################

#####################################################################
##########  These enums can be used for any toon.  ##################
#####################################################################						 
CHAR_MODELID	=	enum(DELF_MALE  = [0x0F, 0xED, 0xC4, 0xF6, 0xD4], #Dark Elf - Male
						 ELF_MALE   = [0x04, 0xF2, 0xD4, 0xC0, 0x8F], #Elf - Male
						 TRL_MALE   = [0x09, 0xC6, 0xFD, 0x94, 0xE4], #Troll - Male
						 HUM_MALE   = [0x0E, 0x8D, 0xC4, 0xE3, 0x8C], #Human - Male
						 HLF_MALE   = [0x0B, 0xC2, 0xA6, 0x80, 0xBB], #Halfling - Male
						 DWF_MALE   = [0x0A, 0xDA, 0xA7, 0xEE, 0xF1], #Dwarf - Male
						 BAR_MALE   = [0x0C, 0x9C, 0xD1, 0xFB, 0xBE], #Barbarian - Male
						 GNO_MALE   = [0X0A, 0XE6, 0X9C, 0XD3, 0XD5]) #Gnome - Male
						 
CHAR_CLASS 	 = 	   enum(WAR  =  0x00, #Warrior
						RAN  =  0x02, #Ranger
						PAL  =  0x04, #Paladin
						SK   =  0x06, #Shadowknight
						MNK  =  0x08, #Monk
						BRD  =  0x0a, #Bard
						RGE  =  0x0c, #Rogue
						DRD  =  0x0e, #Druid
                        SHA  =  0x10, #Shaman
						CL   =  0x12, #Cleric
						MAG  =  0x14, #Magician
						NEC  =  0x16, #Necromancer
						ENC  =  0x18, #Enchanter
						WIZ  =  0x1a, #Wizard
						ALC  =  0x1c) #Alchemist
      
CHAR_RACE	 =     enum(HUM    =  0x00, #Human
						ELF    =  0x02, #Elf
						DELF   =  0x04, #Dark Elf
						GNO	   =  0x06, #Gnome
						DWF	   =  0x08, #Dwarf
						TRL	   =  0x0a, #Troll	
						BAR	   =  0x0c, #Barbarian
						HLF	   =  0x0e, #Halfling
						ERU	   =  0x10, #Erudite
						OGR	   =  0x12) #Ogre
	
CHAR_HAIRCOLOR	=	enum(BLACK  = 0x00,  #Option 1 
						 BROWN  = 0x02,  #Option 2
						 BLONDE = 0x04,  #Option 3
						 GREY   = 0x06,  #Option 4
						 ORANGE = 0x08,  #Option 5 
						 GREEN  = 0x0a,  #Option 6
						 TEAL   = 0x0c,  #Option 7
						 PINK   = 0x0e)  #Option 8
						
CHAR_HAIRLEN	=	enum(LEN1   = 0x00,   #Length 1 
						 LEN2   = 0x02,   #Length 2
						 LEN3   = 0x04,   #Length 3
						 LEN4   = 0x06)   #Length 4
						 
CHAR_HAIRSTYLE	=	enum(STYLE1   = 0x00,   #Style 1 
						 STYLE2   = 0x02,   #Style 2
						 STYLE3   = 0x04,   #Style 3
						 STYLE4   = 0x06)   #Style 4
						 
CHAR_FACE	=	enum(FACE1   = 0x00,   #Face 1 
					 FACE2   = 0x02,   #Face 2
					 FACE3   = 0x04,   #Face 3
					 FACE4   = 0x06)   #Face 4
					 
CHAR_ROBE	=	enum(ARCANE = 0x00000000, #Arcane Robe
					 DIVINE = 0x00000001, #Divine Robe
					 SILK   = 0x00000002, #Silk Robe
					 FUR    = 0x00000003, #Fur Robe
					 NOROBE = 0xFFFFFFFF) #No Robe

CHAR_ARMOR = enum(NOARMOR  = 0x00, #None
				  PADDED   = 0x01, #Padded 
				  LEATHER  = 0x02, #Leather 
				  CHAIN    = 0x03, #Chain Mail
				  PLATE    = 0x04, #Plate Mail
				  SPLIT    = 0x05, #Split Mail
				  BANDED   = 0x06, #Banded Mail
				  SCALE    = 0x07, #Scale Mail
				  WRAPS    = 0x08) #Monk Wraps					 
					
CHAR_ANIMATE	=	enum(STND  = 0x0000, #Standing
						 S1H   = 0x0001, #1H Slash 
						 S2H   = 0x0002, #2H Slash 
						 B1H   = 0x0003, #1H Blunt
						 B2H   = 0x0004, #2H Blunt
						 P1H   = 0x0005, #1H Pierce
						 P2H   = 0x0006, #2H Pierce
						 BOW   = 0x0007, #Bow
						 FIST1 = 0x0008, #Fist
						 CBOW  = 0x0009, #Crossbow
						 THR   = 0x000a, #Throw
						 FIST2 = 0x000b, #Fist
						 NOT   = 0x000c, #Nothing
                         P1HO  = 0x0501, #1H Pierce (Offhand)
						 S1HO  = 0x0101, #1H Slash (Offhand)
						 B1HO  = 0x0301) #1H Blunt (Offhand)

   
CHAR_GEARCOLOR = enum(DEFAULT     = enum(R = 0xFF, 
                                         G = 0xFF, 
                                         B = 0xFF),
                      VAN_BLACK   = enum(R = 0x5C, 
                                         G = 0x5C, 
                                         B = 0x5C), 
                      VAN_BLUE    = enum(R = 0x00, 
                                         G = 0x00, 
                                         B = 0xFF),  
                      VAN_BROWN   = enum(R = 0x96, 
                                         G = 0x64, 
                                         B = 0x32), 
                      VAN_COBALT  = enum(R = 0x40, 
                                         G = 0x00, 
                                         B = 0x80), 
                      VAN_GREEN   = enum(R = 0x00, 
                                         G = 0x80, 
                                         B = 0x00), 
                      VAN_ORANGE  = enum(R = 0xFF, 
                                         G = 0x80, 
                                         B = 0x00), 
                      VAN_PUCE    = enum(R = 0x80, 
                                         G = 0x40, 
                                         B = 0x40), 
                      VAN_RED     = enum(R = 0xF0, 
                                         G = 0x3C, 
                                         B = 0x3C), 
                      VAN_SKY     = enum(R = 0xA5, 
                                         G = 0xF5, 
                                         B = 0xFF), 
                      VAN_STEEL   = enum(R = 0x80, 
                                         G = 0x80, 
                                         B = 0x80), 
                      VAN_TAN     = enum(R = 0x80, 
                                         G = 0x40, 
                                         B = 0x40), 
                      VAN_TEAL    = enum(R = 0xFF, 
                                         G = 0x00, 
                                         B = 0xB4), 
                      VAN_WINE    = enum(R = 0x96, 
                                         G = 0x00, 
                                         B = 0x3C), 
                      VAN_YELLOW  = enum(R = 0xF0, 
                                         G = 0xF0, 
                                         B = 0x00), 
                      CAT_BLUE    = enum(R = 0x20, 
                                         G = 0x39, 
                                         B = 0x64), 
                      PURPLE1     = enum(R = 0x80, 
                                         G = 0x00, 
                                         B = 0x80), 
                      JB_BLUE     = enum(R = 0x3F, 
                                         G = 0x76, 
                                         B = 0x94), 
                      PURPLE2     = enum(R = 0x75, 
                                         G = 0x00, 
                                         B = 0x75), 
                      JACK_PURPLE = enum(R = 0x44, 
                                         G = 0x44, 
                                         B = 0x88), 
                      RED2        = enum(R = 0xFF, 
                                         G = 0x00, 
                                         B = 0x00), 
                      VER_GREEN   = enum(R = 0x50, 
                                         G = 0x64, 
                                         B = 0x00), 
                      CYPRUS      = enum(R = 0x00, 
                                         G = 0x40, 
                                         B = 0x40), 
                      MALACHITE   = enum(R = 0x1B, 
                                         G = 0xED, 
                                         B = 0x59), 
                      MOODY_BLUE  = enum(R = 0x79, 
                                         G = 0x79, 
                                         B = 0xBD), 
                      JAP_LAUREL  = enum(R = 0x2D, 
                                         G = 0x84, 
                                         B = 0x3C), 
                      HIGHBALL    = enum(R = 0x9E, 
                                         G = 0x9C, 
                                         B = 0x38), 
                      AQUA        = enum(R = 0x80, 
                                         G = 0xFF, 
                                         B = 0xD6), 
                      PER_GREEN   = enum(R = 0x00, 
                                         G = 0x9F, 
                                         B = 0x9F), 
                      DARK_CER    = enum(R = 0x00, 
                                         G = 0x41, 
                                         B = 0x82), 
                      CERULEAN    = enum(R = 0x00, 
                                         G = 0x80, 
                                         B = 0xC0), 
                      BLACK       = enum(R = 0x15, 
                                         G = 0x00, 
                                         B = 0x00), 
                      ORACLE      = enum(R = 0x31, 
                                         G = 0x62, 
                                         B = 0x62), 
                      MAROON      = enum(R = 0x62, 
                                         G = 0x00, 
                                         B = 0x00),
                      CHATEAU     = enum(R = 0xA0, 
                                         G = 0xAA, 
                                         B = 0xB4), 
                      ZAMBEZI     = enum(R = 0x5A, 
                                         G = 0x5A, 
                                         B = 0x5A), 
                      MID_BLUE    = enum(R = 0x00, 
                                         G = 0x00, 
                                         B = 0xA0), 
                      SKY_BLUE    = enum(R = 0x00, 
                                         G = 0xB4, 
                                         B = 0xFF), 
                      PAST_GREEN  = enum(R = 0x00, 
                                         G = 0xC8, 
                                         B = 0x32), 
                      PANCHO      = enum(R = 0xDC, 
                                         G = 0xBE, 
                                         B = 0x96), 
                      RE_BLUE     = enum(R = 0x00, 
                                         G = 0xC8, 
                                         B = 0xB4)) 
                     
                      
ALPHA = enum(ON  = 0XFF,
             OFF = 0X00)

					
class CharListing():
	"""Builds the display output of one character."""
	
	def __init__(self, charName, charServerID, charServerModel, charClass,  
				 charRace, charLevel, hairColor, hairLen, hairStyle,
				 faceOpt, robeType, primaryHand, secondaryHand, shieldSlot,
				 charAnimation, vanUnusedValue1, chestSlot, bracerSlot, 
				 gloveSlot, pantsSlot, bootSlot, helmSlot, vanUnusedValue2, vanUnusedValue3, 
				 vanUnusedColor1_R, vanUnusedColor1_B, vanUnusedColor1_G, vanUnusedColor1_A, 
				 vanUnusedColor2_R, vanUnusedColor2_B, vanUnusedColor2_G, vanUnusedColor2_A,
				 vanUnusedColor3_R, vanUnusedColor3_G, vanUnusedColor3_B, vanUnusedColor3_A, 
				 chestColor_R, chestColor_G, chestColor_B, chestColor_A,    
				 bracerColor_R, bracerColor_G, bracerColor_B, bracerColor_A, 
				 gloveColor_R, gloveColor_G, gloveColor_B, gloveColor_A, 
				 pantsColor_R, pantsColor_G, pantsColor_B, pantsColor_A, 
				 bootColor_R, bootColor_G, bootColor_B, bootColor_A, 
				 helmColor_R, helmColor_G, helmColor_B, helmColor_A, 
				 robeColor_R, robeColor_G, robeColor_B, robeColor_A):
				 
		self.charName          = charName
		self.charServerID      = charServerID
		self.charServerModel   = charServerModel
		self.charClass		   = charClass
		self.charRace		   = charRace
		self.charLevel		   = charLevel
		self.hairColor	       = hairColor
		self.hairLen		   = hairLen
		self.hairStyle	       = hairStyle
		self.faceOpt		   = faceOpt
		self.robeType		   = robeType
		self.primaryHand	   = primaryHand
		self.secondaryHand	   = secondaryHand
		self.shieldSlot	 	   = shieldSlot
		self.charAnimation     = charAnimation
		self.vanUnusedValue1   = vanUnusedValue1
		self.chestSlot		   = chestSlot
		self.bracerSlot		   = bracerSlot
		self.gloveSlot		   = gloveSlot
		self.pantsSlot		   = pantsSlot
		self.bootSlot		   = bootSlot
		self.helmSlot	       = helmSlot
		self.vanUnusedValue2   = vanUnusedValue2
		self.vanUnusedValue3   = vanUnusedValue3
		self.vanUnusedColor1_R = vanUnusedColor1_R
		self.vanUnusedColor1_G = vanUnusedColor1_G
		self.vanUnusedColor1_B = vanUnusedColor1_B
		self.vanUnusedColor1_A = vanUnusedColor1_A
		self.vanUnusedColor2_R = vanUnusedColor2_R
		self.vanUnusedColor2_G = vanUnusedColor2_G
		self.vanUnusedColor2_B = vanUnusedColor2_B
		self.vanUnusedColor2_A = vanUnusedColor2_A
		self.vanUnusedColor3_R = vanUnusedColor3_R
		self.vanUnusedColor3_G = vanUnusedColor3_G
		self.vanUnusedColor3_B = vanUnusedColor3_B
		self.vanUnusedColor3_A = vanUnusedColor3_A
		self.chestColor_R	   = chestColor_R
		self.chestColor_G	   = chestColor_G
		self.chestColor_B	   = chestColor_B
		self.chestColor_A	   = chestColor_A
		self.bracerColor_R	   = bracerColor_R
		self.bracerColor_G	   = bracerColor_G
		self.bracerColor_B	   = bracerColor_B
		self.bracerColor_A	   = bracerColor_A
		self.gloveColor_R	   = gloveColor_R
		self.gloveColor_G	   = gloveColor_G
		self.gloveColor_B	   = gloveColor_B
		self.gloveColor_A	   = gloveColor_A
		self.pantsColor_R	   = pantsColor_R
		self.pantsColor_G	   = pantsColor_G
		self.pantsColor_B	   = pantsColor_B
		self.pantsColor_A	   = pantsColor_A
		self.bootColor_R       = bootColor_R
		self.bootColor_G       = bootColor_G
		self.bootColor_B       = bootColor_B
		self.bootColor_A       = bootColor_A
		self.helmColor_R	   = helmColor_R
		self.helmColor_G	   = helmColor_G
		self.helmColor_B	   = helmColor_B
		self.helmColor_A	   = helmColor_A
		self.robeColor_R	   = robeColor_R
		self.robeColor_G	   = robeColor_G
		self.robeColor_B	   = robeColor_B
		self.robeColor_A	   = robeColor_A


		