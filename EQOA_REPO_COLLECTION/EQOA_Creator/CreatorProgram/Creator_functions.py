from Creator_DB import *
from tkinter import *
  
class Gear:
    
    def __init__(self):
        self.session     = None
        self.error       = None
        self.itemid      = None 
        self.itemfam     = None
        self.itemicon    = None
        self.slot        = None
        self.trade       = None
        self.rent        = None
        self.atkType     = None
        self.damage      = None
        self.level       = None
        self.maxStack    = None
        self.HPMAX       = None
        self.durability  = None
        self.tclass      = None
        self.race        = None
        self.wepproc     = None
        self.armproc     = None
        self.procname    = None
        self.lore        = None
        self.craft       = None
        self.itemName    = None
        self.itemDesc    = None
        self.stats       = None
        self.window      = None
        self.region      = None
        self.graphic     = None
        self.armor       = None
        self.helm        = None
        self.robe        = None
        self.color       = None
        self.rarity      = None
        self.login       = None
        self.notes       = None
        self.itemCreator = None
              
    def processSubmit(self):
        
        #acquires equiptype id
        if self.slot != 0:
            for row in self.session.query(equipTypes):
                if row.equipmenttype == self.slot:
                    self.slot = row.hexid

        #Acquires attack types id
        if self.atkType != 0:
            for row in self.session.query(attackTypes):
                if row.attacktype == self.atkType:
                    self.atkType = row.hexid

        
        #Acquires Icon id
        for row in self.session.query(icons):
            if row.iconname == self.itemicon:
                self.itemicon = row.iconid
                
    
    def addItem(self, item, stats, itemCreator):
        
        self.itemid      = item[0] 
        self.itemfam     = item[1]
        self.itemicon    = item[2]
        self.slot        = item[3]
        self.trade       = item[4]
        self.rent        = item[5]
        self.atkType     = item[6]
        self.damage      = item[7]
        self.level       = item[8]
        self.maxStack    = item[9]
        self.HPMAX       = item[10]
        self.durability  = item[11]
        self.tclass      = item[12]
        self.race        = item[13]
        self.wepproc     = item[14]
        self.armproc     = item[15]
        self.procname    = item[16]
        self.lore        = item[17]
        self.craft       = item[18]
        self.itemName    = item[19]
        self.itemDesc    = item[20]
        self.region      = item[21]
        if item[22][0] == None:
            self.graphic = None
        elif item[22][0] == 'Armor':
            self.armor = item[22][1]
        elif item[22][0] == 'Robe':
            self.robe = item[22][1]
        elif item[22][0] == 'Helm':
            self.helm = item[22][1]
        elif item[22][0] == 'Other':
            self.graphic = item[22][1]
        self.color       = item[23]
        self.rarity      = item[24]
        self.login       = item[25]
        self.notes       = item[26]
        self.stats       = stats
        self.itemCreator = itemCreator
        
    def addSession(self, session, window):
        self.session    = session
        self.window     = window
        
    def writeGear(self):
        
        if self.graphic != None:
            obj = items(Itemid         = self.itemid,
                        Itemfamily     = self.itemfam,
                        Itemicon       = self.itemicon,
                        Slot           = self.slot,
                        Trade          = self.trade,
                        Rent           = self.rent,
                        Attacktype     = self.atkType,
                        Damage         = self.damage,
                        Level          = self.level,
                        Maxstack       = self.maxStack,
                        HPMAX          = self.HPMAX,
                        Durability     = self.durability,
                        Class          = self.tclass,
                        Race           = self.race,
                        Wepproc        = self.wepproc,
                        Armproc        = self.armproc,
                        Procname       = self.procname,
                        Lore           = self.lore,
                        Craft          = self.craft,
                        Itemname       = self.itemName,
                        Itemdesc       = self.itemDesc,
                        Strength       = self.stats[0],
                        Stamina        = self.stats[1],
                        Agility        = self.stats[2],
                        Dexterity      = self.stats[3],
                        Wisdom         = self.stats[4],
                        Intelligence   = self.stats[5],
                        Charisma       = self.stats[6],
                        HP_MAX         = self.stats[7],
                        PWR_MAX        = self.stats[8],
                        PoT            = self.stats[9],
                        HoT            = self.stats[10],
                        AC             = self.stats[11],
                        PR             = self.stats[12],
                        DR             = self.stats[13],
                        FR             = self.stats[14],
                        CR             = self.stats[15],
                        LR             = self.stats[16],
                        AR             = self.stats[17],
                        region         = self.region,
                        Rarity         = self.rarity,
                        Creator        = self.login,
                        Creatortime    = func.now(),
                        Imagegraphic   = self.graphic,
                        Color          = self.color,
                        Notes          = self.notes)
                        
        elif self.armor:            
            obj = items(Itemid         = self.itemid,
                        Itemfamily     = self.itemfam,
                        Itemicon       = self.itemicon,
                        Slot           = self.slot,
                        Trade          = self.trade,
                        Rent           = self.rent,
                        Attacktype     = self.atkType,
                        Damage         = self.damage,
                        Level          = self.level,
                        Maxstack       = self.maxStack,
                        HPMAX          = self.HPMAX,
                        Durability     = self.durability,
                        Class          = self.tclass,
                        Race           = self.race,
                        Wepproc        = self.wepproc,
                        Armproc        = self.armproc,
                        Procname       = self.procname,
                        Lore           = self.lore,
                        Craft          = self.craft,
                        Itemname       = self.itemName,
                        Itemdesc       = self.itemDesc,
                        Strength       = self.stats[0],
                        Stamina        = self.stats[1],
                        Agility        = self.stats[2],
                        Dexterity      = self.stats[3],
                        Wisdom         = self.stats[4],
                        Intelligence   = self.stats[5],
                        Charisma       = self.stats[6],
                        HP_MAX         = self.stats[7],
                        PWR_MAX        = self.stats[8],
                        PoT            = self.stats[9],
                        HoT            = self.stats[10],
                        AC             = self.stats[11],
                        PR             = self.stats[12],
                        DR             = self.stats[13],
                        FR             = self.stats[14],
                        CR             = self.stats[15],
                        LR             = self.stats[16],
                        AR             = self.stats[17],
                        region         = self.region,
                        Rarity         = self.rarity,
                        Creator        = self.login,
                        Creatortime    = func.now(),
                        Armor          = self.armor,
                        Color          = self.color,
                        Notes          = self.notes)
             
        elif self.robe:                
            obj = items(Itemid         = self.itemid,
                        Itemfamily     = self.itemfam,
                        Itemicon       = self.itemicon,
                        Slot           = self.slot,
                        Trade          = self.trade,
                        Rent           = self.rent,
                        Attacktype     = self.atkType,
                        Damage         = self.damage,
                        Level          = self.level,
                        Maxstack       = self.maxStack,
                        HPMAX          = self.HPMAX,
                        Durability     = self.durability,
                        Class          = self.tclass,
                        Race           = self.race,
                        Wepproc        = self.wepproc,
                        Armproc        = self.armproc,
                        Procname       = self.procname,
                        Lore           = self.lore,
                        Craft          = self.craft,
                        Itemname       = self.itemName,
                        Itemdesc       = self.itemDesc,
                        Strength       = self.stats[0],
                        Stamina        = self.stats[1],
                        Agility        = self.stats[2],
                        Dexterity      = self.stats[3],
                        Wisdom         = self.stats[4],
                        Intelligence   = self.stats[5],
                        Charisma       = self.stats[6],
                        HP_MAX         = self.stats[7],
                        PWR_MAX        = self.stats[8],
                        PoT            = self.stats[9],
                        HoT            = self.stats[10],
                        AC             = self.stats[11],
                        PR             = self.stats[12],
                        DR             = self.stats[13],
                        FR             = self.stats[14],
                        CR             = self.stats[15],
                        LR             = self.stats[16],
                        AR             = self.stats[17],
                        region         = self.region,
                        Rarity         = self.rarity,
                        Creator        = self.login,
                        Creatortime    = func.now(),
                        Robe           = self.robe,
                        Color          = self.color,
                        Notes          = self.notes)
            
        elif self.helm:                
            obj = items(Itemid         = self.itemid,
                        Itemfamily     = self.itemfam,
                        Itemicon       = self.itemicon,
                        Slot           = self.slot,
                        Trade          = self.trade,
                        Rent           = self.rent,
                        Attacktype     = self.atkType,
                        Damage         = self.damage,
                        Level          = self.level,
                        Maxstack       = self.maxStack,
                        HPMAX          = self.HPMAX,
                        Durability     = self.durability,
                        Class          = self.tclass,
                        Race           = self.race,
                        Wepproc        = self.wepproc,
                        Armproc        = self.armproc,
                        Procname       = self.procname,
                        Lore           = self.lore,
                        Craft          = self.craft,
                        Itemname       = self.itemName,
                        Itemdesc       = self.itemDesc,
                        Strength       = self.stats[0],
                        Stamina        = self.stats[1],
                        Agility        = self.stats[2],
                        Dexterity      = self.stats[3],
                        Wisdom         = self.stats[4],
                        Intelligence   = self.stats[5],
                        Charisma       = self.stats[6],
                        HP_MAX         = self.stats[7],
                        PWR_MAX        = self.stats[8],
                        PoT            = self.stats[9],
                        HoT            = self.stats[10],
                        AC             = self.stats[11],
                        PR             = self.stats[12],
                        DR             = self.stats[13],
                        FR             = self.stats[14],
                        CR             = self.stats[15],
                        LR             = self.stats[16],
                        AR             = self.stats[17],
                        region         = self.region,
                        Rarity         = self.rarity,
                        Creator        = self.login,
                        Creatortime    = func.now(),
                        Helm           = self.helm,
                        Color          = self.color,
                        Notes          = self.notes) 
                        
        else:                
            obj = items(Itemid         = self.itemid,
                        Itemfamily     = self.itemfam,
                        Itemicon       = self.itemicon,
                        Slot           = self.slot,
                        Trade          = self.trade,
                        Rent           = self.rent,
                        Attacktype     = self.atkType,
                        Damage         = self.damage,
                        Level          = self.level,
                        Maxstack       = self.maxStack,
                        HPMAX          = self.HPMAX,
                        Durability     = self.durability,
                        Class          = self.tclass,
                        Race           = self.race,
                        Wepproc        = self.wepproc,
                        Armproc        = self.armproc,
                        Procname       = self.procname,
                        Lore           = self.lore,
                        Craft          = self.craft,
                        Itemname       = self.itemName,
                        Itemdesc       = self.itemDesc,
                        Strength       = self.stats[0],
                        Stamina        = self.stats[1],
                        Agility        = self.stats[2],
                        Dexterity      = self.stats[3],
                        Wisdom         = self.stats[4],
                        Intelligence   = self.stats[5],
                        Charisma       = self.stats[6],
                        HP_MAX         = self.stats[7],
                        PWR_MAX        = self.stats[8],
                        PoT            = self.stats[9],
                        HoT            = self.stats[10],
                        AC             = self.stats[11],
                        PR             = self.stats[12],
                        DR             = self.stats[13],
                        FR             = self.stats[14],
                        CR             = self.stats[15],
                        LR             = self.stats[16],
                        AR             = self.stats[17],
                        region         = self.region,
                        Rarity         = self.rarity,
                        Creator        = self.login,
                        Creatortime    = func.now(),
                        Color          = self.color,
                        Notes          = self.notes)
                    
        try:
            self.session.add(obj)
            self.session.flush()
            self.session.commit()
            if self.error:
                self.destroyError()
            self.error = Label(self.window, text="Successfully saved", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            self.itemCreator.removeEntry()
            
        except SQLAlchemyError as e:
            print(str(e))
            if self.error:
                self.destroyError()
            self.error = Label(self.window, text="Error. Duplicate?", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            
        self.reset()
            
    def destroyError(self):
        if self.error:
            self.error.destroy()
            
    def updateGear(self):
                        
        if self.armor:
            for row in self.session.query(items).filter(items.Itemid == self.itemid):
                row.Itemid       = self.itemid
                row.Itemfamily   = self.itemfam
                row.Itemicon     = self.itemicon
                row.Slot         = self.slot
                row.Trade        = self.trade
                row.Rent         = self.rent
                row.Attacktype   = self.atkType
                row.Damage       = self.damage
                row.Level        = self.level
                row.Maxstack     = self.maxStack
                row.HPMAX        = self.HPMAX
                row.Durability   = self.durability
                row.Class        = self.tclass
                row.Race         = self.race
                row.Weproc       = self.wepproc
                row.Armroc       = self.armproc
                row.Procname     = self.procname
                row.Lore         = self.lore
                row.Craft        = self.craft
                row.Itemname     = self.itemName
                row.Itemdesc     = self.itemDesc
                row.Strength     = self.stats[0]
                row.Stamina      = self.stats[1]
                row.Agility      = self.stats[2]
                row.Dexterity    = self.stats[3]
                row.Wisdom       = self.stats[4]
                row.Intelligence = self.stats[5]
                row.Charisma     = self.stats[6]
                row.HP_MAX       = self.stats[7]
                row.PWR_MAX      = self.stats[8]
                row.PoT          = self.stats[9]
                row.HoT          = self.stats[10]
                row.AC           = self.stats[11]
                row.PR           = self.stats[12]
                row.DR           = self.stats[13]
                row.FR           = self.stats[14]
                row.CR           = self.stats[15]
                row.LR           = self.stats[16]
                row.AR           = self.stats[17]
                row.region       = self.region
                row.Rarity       = self.rarity
                row.Modifier     = self.login
                row.Modifiertime = func.now()
                row.Notes        = self.notes
                row.Armor        = self.Armor
                
                self.session.flush()
                self.session.commit()
                        
        elif self.robe:
            for row in self.session.query(items).filter(items.Itemid == self.itemid):
                row.Itemid       = self.itemid
                row.Itemfamily   = self.itemfam
                row.Itemicon     = self.itemicon
                row.Slot         = self.slot
                row.Trade        = self.trade
                row.Rent         = self.rent
                row.Attacktype   = self.atkType
                row.Damage       = self.damage
                row.Level        = self.level
                row.Maxstack     = self.maxStack
                row.HPMAX        = self.HPMAX
                row.Durability   = self.durability
                row.Class        = self.tclass
                row.Race         = self.race
                row.Weproc       = self.wepproc
                row.Armroc       = self.armproc
                row.Procname     = self.procname
                row.Lore         = self.lore
                row.Craft        = self.craft
                row.Itemname     = self.itemName
                row.Itemdesc     = self.itemDesc
                row.Strength     = self.stats[0]
                row.Stamina      = self.stats[1]
                row.Agility      = self.stats[2]
                row.Dexterity    = self.stats[3]
                row.Wisdom       = self.stats[4]
                row.Intelligence = self.stats[5]
                row.Charisma     = self.stats[6]
                row.HP_MAX       = self.stats[7]
                row.PWR_MAX      = self.stats[8]
                row.PoT          = self.stats[9]
                row.HoT          = self.stats[10]
                row.AC           = self.stats[11]
                row.PR           = self.stats[12]
                row.DR           = self.stats[13]
                row.FR           = self.stats[14]
                row.CR           = self.stats[15]
                row.LR           = self.stats[16]
                row.AR           = self.stats[17]
                row.region       = self.region
                row.Rarity       = self.rarity
                row.Modifier     = self.login
                row.Modifiertime = func.now()
                row.Notes        = self.notes
                row.Robe         = self.robe
                self.session.flush()
                self.session.commit()
                        
        elif self.helm:
            for row in self.session.query(items).filter(items.Itemid == self.itemid):
                row.Itemid       = self.itemid
                row.Itemfamily   = self.itemfam
                row.Itemicon     = self.itemicon
                row.Slot         = self.slot
                row.Trade        = self.trade
                row.Rent         = self.rent
                row.Attacktype   = self.atkType
                row.Damage       = self.damage
                row.Level        = self.level
                row.Maxstack     = self.maxStack
                row.HPMAX        = self.HPMAX
                row.Durability   = self.durability
                row.Class        = self.tclass
                row.Race         = self.race
                row.Weproc       = self.wepproc
                row.Armroc       = self.armproc
                row.Procname     = self.procname
                row.Lore         = self.lore
                row.Craft        = self.craft
                row.Itemname     = self.itemName
                row.Itemdesc     = self.itemDesc
                row.Strength     = self.stats[0]
                row.Stamina      = self.stats[1]
                row.Agility      = self.stats[2]
                row.Dexterity    = self.stats[3]
                row.Wisdom       = self.stats[4]
                row.Intelligence = self.stats[5]
                row.Charisma     = self.stats[6]
                row.HP_MAX       = self.stats[7]
                row.PWR_MAX      = self.stats[8]
                row.PoT          = self.stats[9]
                row.HoT          = self.stats[10]
                row.AC           = self.stats[11]
                row.PR           = self.stats[12]
                row.DR           = self.stats[13]
                row.FR           = self.stats[14]
                row.CR           = self.stats[15]
                row.LR           = self.stats[16]
                row.AR           = self.stats[17]
                row.region       = self.region
                row.Rarity       = self.rarity
                row.Modifier     = self.login
                row.Modifiertime = func.now()
                row.Notes        = self.notes
                row.Helm         = self.helm
                self.session.flush()
                self.session.commit()

        else:
            for row in self.session.query(items).filter(items.Itemid == self.itemid):
                row.Itemid       = self.itemid
                row.Itemfamily   = self.itemfam
                row.Itemicon     = self.itemicon
                row.Slot         = self.slot
                row.Trade        = self.trade
                row.Rent         = self.rent
                row.Attacktype   = self.atkType
                row.Damage       = self.damage
                row.Level        = self.level
                row.Maxstack     = self.maxStack
                row.HPMAX        = self.HPMAX
                row.Durability   = self.durability
                row.Class        = self.tclass
                row.Race         = self.race
                row.Weproc       = self.wepproc
                row.Armroc       = self.armproc
                row.Procname     = self.procname
                row.Lore         = self.lore
                row.Craft        = self.craft
                row.Itemname     = self.itemName
                row.Itemdesc     = self.itemDesc
                row.Strength     = self.stats[0]
                row.Stamina      = self.stats[1]
                row.Agility      = self.stats[2]
                row.Dexterity    = self.stats[3]
                row.Wisdom       = self.stats[4]
                row.Intelligence = self.stats[5]
                row.Charisma     = self.stats[6]
                row.HP_MAX       = self.stats[7]
                row.PWR_MAX      = self.stats[8]
                row.PoT          = self.stats[9]
                row.HoT          = self.stats[10]
                row.AC           = self.stats[11]
                row.PR           = self.stats[12]
                row.DR           = self.stats[13]
                row.FR           = self.stats[14]
                row.CR           = self.stats[15]
                row.LR           = self.stats[16]
                row.AR           = self.stats[17]
                row.region       = self.region
                row.Rarity       = self.rarity
                row.Modifier     = self.login
                row.Modifiertime = func.now()
                row.Notes        = self.notes
                row.Imagegraphic = self.graphic
                
                self.session.flush()
                self.session.commit()
        
        try:
            if self.error:
                 self.destroyError()
            self.error = Label(self.window, text="Successfully saved", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            
        except SQLAlchemyError as e:
            print(str(e))
            if self.error:
                self.destroyError()
            self.error = Label(self.window, text="Error. Duplicate?", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            
        self.reset()
        
    def reset(self):
        self.itemid      = None 
        self.itemfam     = None
        self.itemicon    = None
        self.slot        = None
        self.trade       = None
        self.rent        = None
        self.atkType     = None
        self.damage      = None
        self.level       = None
        self.maxStack    = None
        self.HPMAX       = None
        self.durability  = None
        self.tclass      = None
        self.race        = None
        self.wepproc     = None
        self.armproc     = None
        self.procname    = None
        self.lore        = None
        self.craft       = None
        self.itemName    = None
        self.itemDesc    = None
        self.window      = None
        self.region      = None
        self.graphic     = None
        self.armor       = None
        self.helm        = None
        self.robe        = None
        self.color       = None
        self.rarity      = None
        self.login       = None
        self.notes       = None
        self.itemCreator = None
        self.stats.clear()
            
    def audit(self, login, itemid):
        
        obj =      {'Verifier'       : login,
                    'Verifiertime'   : func.now()}
        try:
            thisSession = self.session.query(items).get(itemid)
            for key, value in obj.items():
                setattr(thisSession, key, value)
                
            self.session.flush()
            self.session.commit()
            if self.error:
                self.destroyError()
            self.error = Label(self.window, text="Successfully saved", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            
        except SQLAlchemyError as e:
            print(str(e))
            if self.error:
                self.destroyError()
            self.error = Label(self.window, text="Error. Duplicate?", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            
        self.reset()
