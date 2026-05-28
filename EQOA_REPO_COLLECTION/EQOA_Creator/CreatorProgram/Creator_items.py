from tkinter import *
from tkinter import ttk
from tkinter import colorchooser
import base64
from Creator_functions import Gear
from functools import partial
from sqlalchemy import func   # func.now()
from sqlalchemy import *
from Creator_DB import *
 

class equipEnum:
    helm        = 'HELM'
    robe        = 'ROBE'
    earring     = 'EARRING'
    neck        = 'NECK'
    chest       = 'CHEST'
    bracers     = 'BRACERS'
    bracelet    = 'BRACELET'
    ring        = 'RING'
    belt        = 'BELT'
    legs        = 'LEGS'
    feet        = 'FEET'
    primary     = 'PRIMARY'
    shield      = 'SHIELD'
    secondary   = 'SECONDARY'
    thand       = '2 HAND'
    bow         = 'BOW'
    thrown      = 'THROWN'
    held        = 'HELD'
    gloves      = 'GLOVES'
    fishing     = 'FISHING'
    bait        = 'BAIT'
    weaponcraft = 'WEAPON CRAFT'
    armorcraft  = 'ARMOR CRAFT'
    tailoring   = 'TAILORING'
    jewelcraft  = 'JEWEL CRAFT'
    carpentry   = 'CARPENTRY'
    alchemy     = 'ALCHEMY'
    armor       = [helm, feet, robe, earring, neck, chest, bracers, bracelet, ring, belt, legs,
                   shield, held, gloves, bait, weaponcraft, armorcraft, tailoring, jewelcraft,
                   carpentry, alchemy]
    weapon      = [primary, secondary, bow, thand, fishing, thrown]
    
    items       = ['NONE']

class classSummary:
    
    ALC = 14
    BRD = 5
    CL  = 9
    DRD = 7
    ENC = 12
    MAG = 10
    MNK = 4
    NEC = 11
    PAL = 2
    RAN = 1
    RGE = 6
    SHA = 8
    SK  = 3
    WAR = 0
    WIZ = 13
    
class raceSummary:
    
    HUM  = 0
    ELF  = 1
    DELF = 2
    GNO  = 3
    DWF  = 4
    TRL  = 5
    BAR  = 6
    HLF  = 7
    ERU  = 8
    OGR  = 9

#Objects
class createItem(Gear):
    
    def __init__(self, window, login):
        #print('Initializing Item Creation class')
        self.window      = window
        self.login       = login
        self.error       = None
        self.itemObject  = []
        self.stats       = []
        self.session     = None
        self.color       = None
        self.vart        = IntVar()
        self.varr        = IntVar()
        self.varl        = IntVar()
        self.varc        = IntVar()
        self.warint      = IntVar()
        self.brdint      = IntVar()
        self.alcint      = IntVar()
        self.clint       = IntVar()
        self.drdint      = IntVar()
        self.encint      = IntVar()
        self.magint      = IntVar()
        self.mnkint      = IntVar()
        self.necint      = IntVar()
        self.palint      = IntVar()
        self.ranint      = IntVar()
        self.rgeint      = IntVar()
        self.skint       = IntVar()
        self.shaint      = IntVar()
        self.wizint      = IntVar()
        self.allint      = IntVar()
        self.tankint     = IntVar()
        self.healint     = IntVar()
        self.melint      = IntVar()
        self.castint     = IntVar()
        self.humint      = IntVar()
        self.elfint      = IntVar()
        self.delfint     = IntVar()
        self.gnoint      = IntVar()
        self.dwfint      = IntVar()
        self.trlint      = IntVar()
        self.barint      = IntVar()
        self.hlfint      = IntVar()
        self.eruint      = IntVar()
        self.ogrint      = IntVar()
        self.rallint     = IntVar()
        self.varreg      = StringVar()
        self.varitem     = StringVar()
        self.vari        = StringVar(self.window)
        self.vare        = StringVar(self.window)
        self.vara        = StringVar(self.window)
        self.varrare     = StringVar()
        self.editItem    = None
        self.viewItem    = None
        self.qcItem      = None
        self.varimagetype= StringVar()
        self.varimagename= StringVar()
        self.imageNames  = []
        self.iconNames   = []
        self.labels      = []
        self.picture     = []
        self.entry       = []
        self.checkbox    = [self.warint,
                            self.brdint,
                            self.alcint,
                            self.clint ,
                            self.drdint,
                            self.encint,
                            self.magint,
                            self.mnkint,
                            self.necint,
                            self.palint,
                            self.ranint,
                            self.rgeint,
                            self.skint ,
                            self.shaint,
                            self.wizint,
                            self.allint,
                            self.tankint,
                            self.healint,
                            self.melint,
                            self.castint,
                            self.humint,
                            self.elfint,
                            self.delfint,
                            self.gnoint,
                            self.dwfint,
                            self.trlint,
                            self.barint,
                            self.hlfint,
                            self.eruint,
                            self.ogrint,
                            self.rallint,
                            self.vart,
                            self.varr,
                            self.varl,
                            self.varc]
                            
        self.combobox    = [self.vari,
                            self.varitem,
                            self.vare,
                            self.vara,
                            self.varreg,
                            self.varimagetype,
                            self.varimagename,
                            self.varrare]
        self.pict        = None
        self.gear        = Gear()
        
    def setSession(self, session, viewItem, editItem, qcItem):
        self.session   = session
        self.viewItem  = viewItem
        self.editItem  = editItem
        self.qcItem    = qcItem
    
    def click(self):
        
        self.itemObject.clear()

        #clears error incase of new one
        if self.error:
            self.error.destroy()
        
        #Makes sure ItemID is a digit or generates an error    
        if self.itemid.get().isdigit():
            self.itemObject.append(self.itemid.get())
            
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with ItemID", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        #Makes sure item Family is a digit or generates an error       
        if self.itemfam.get().isdigit():
            self.itemObject.append(self.itemfam.get())
            
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with ItemFam", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        #Checks if a return for self.icon exists, if not we generate an error   
        if self.icon.get():
            
            icon = self.icon.get()
            for row in self.session.query(icons).filter(icons.iconname == icon):
                self.itemObject.append(row.iconhex)
            
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="No Icon selected", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        #Checks if a return for equipmenttype exists, if not generate an error 
        if self.vare.get():
            self.itemObject.append(self.vare.get())

        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text= "No Equipment Type selected", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        self.itemObject.append(self.vart.get())
        self.itemObject.append(self.varr.get())
        
        #checks is attype is disabled, if not, gets value
        if self.atktype['state'] == 'disabled':
            self.itemObject.append(0)
        
        elif self.vara.get():
            for row in self.session.query(attackTypes).filter(attackTypes.attacktype == self.vara.get()):
                self.itemObject.append(row.hexid)
        
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text= "Error with Attack Type", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        #Weapon damage state check, then makes sure int > 0 
        if self.wpndmg['state'] == 'disabled':
            self.itemObject.append(0)

        elif self.wpndmg.get().isdigit() > 0:
            self.itemObject.append(self.wpndmg.get())
        
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text= "Error with Weapon Damage", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return

        #Level catch for non-digits, digits < 1 and state check
        if self.lvlreq['state'] == 'disabled':
            self.itemObject.append(0)
            
        elif self.lvlreq.get().isdigit() > 0:
            self.itemObject.append(self.lvlreq.get())
            
        else:
            
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with Level", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
  
        #Maxstack catch for state and digits
        if self.maxstack['state'] == 'disabled':
            self.itemObject.append(0)

        elif self.maxstack.get().isdigit():
            self.itemObject.append(self.maxstack.get())
            
        else:
            
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with max stack", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
            
        #Max HP catch for non-equipment
        if self.maxhp['state'] == 'disabled':
            self.itemObject.append(0)
            
        else:
            if self.maxhp.get().isdigit() > 0:
                self.itemObject.append(self.maxhp.get())
                
            else:
            
                if self.error:
                    self.error.destroy()
                self.error = Label(self.window, text="Error with max HP", bg = "black", fg = "white", font = "none 12 bold")
                self.error.place(x = 700, y = 700)
                return
            
        #Catch for durability
        if self.dur['state'] == 'disabled':
            self.itemObject.append(0) 
            
        else:
            if self.dur.get().isdigit() > 0:
                self.itemObject.append(self.dur.get())
                
            else:
            
                if self.error:
                    self.error.destroy()
                self.error = Label(self.window, text="Error with Durability", bg = "black", fg = "white", font = "none 12 bold")
                self.error.place(x = 700, y = 700)
                return
            
        itemClass = 0   
        if self.allint.get():
            itemClass += self.allint.get()
        
        else:
            if self.tankint.get():
                itemClass += self.tankint.get()
                
            else:
                itemClass += self.palint.get()
                itemClass += self.warint.get()
                itemClass += self.skint.get()
            
            if self.healint.get():
                itemClass += self.healint.get()
                
            else:
                itemClass += self.clint.get()
                itemClass += self.shaint.get()
                itemClass += self.drdint.get()
                
            if self.melint.get():
                itemClass += self.melint.get()
                
            else:
                itemClass += self.ranint.get()
                itemClass += self.rgeint.get()
                itemClass += self.brdint.get()
                itemClass += self.mnkint.get()
                
            if self.castint.get():
                itemClass += self.castint.get()
                
            else:
                itemClass += self.encint.get()
                itemClass += self.alcint.get()
                itemClass += self.magint.get()
                itemClass += self.necint.get()
        self.itemObject.append(itemClass)
         
        itemRace = 0
        if self.rallint.get():
            itemRace += self.rallint.get()            
            
        else:
            itemRace += self.humint.get()
            itemRace += self.elfint.get()
            itemRace += self.delfint.get()
            itemRace += self.gnoint.get()
            itemRace += self.dwfint.get()
            itemRace += self.trlint.get()
            itemRace += self.barint.get()
            itemRace += self.hlfint.get()
            itemRace += self.eruint.get()
            itemRace += self.ogrint.get()
            
        self.itemObject.append(itemRace)
            
        self.itemObject.append(self.wepproc.get())
        self.itemObject.append(self.armproc.get())
        self.itemObject.append(self.procname.get())
        self.itemObject.append(self.varl.get())
        self.itemObject.append(self.varc.get())
        self.itemObject.append(self.itemname.get())
        self.itemObject.append(self.itemdesc.get())
        
        totStats = [self.strength,
                    self.sta,
                    self.agi,
                    self.dex,
                    self.wis,
                    self.intel,
                    self.cha,
                    self.HPM,
                    self.POWM,
                    self.PoT,
                    self.HoT,
                    self.AC,
                    self.PR,
                    self.DR,
                    self.FR,
                    self.CR,
                    self.LR,
                    self.AR]
                    
        #Catch for Stats
        for stat in totStats:
            if stat['state'] == 'disabled':
                self.stats.append(0)
            
            else:
                if stat.get().isdigit() >= 0:
                    self.stats.append(stat.get())
                
                else:
            
                    if self.error:
                        self.error.destroy()
                    self.error = Label(self.window, text="Error with Stats", bg = "black", fg = "white", font = "none 12 bold")
                    self.error.place(x = 700, y = 700)
                    return
                
        self.itemObject.append(self.varreg.get())
        eqptype = self.vare.get()
        if self.varimagename.get():
            if eqptype == 'HELM' or eqptype == 'GLOVES' or eqptype == 'LEGS' or eqptype == 'CHEST' or eqptype == 'FEET' or eqptype == 'ROBE' or eqptype == 'BRACELET' or eqptype == 'BRACERS':
                if eqptype == 'GLOVES' or eqptype == 'LEGS' or eqptype == 'CHEST' or eqptype == 'FEET' or eqptype == 'BRACELET' or eqptype == 'BRACERS':
                    for row in self.session.query(Armor).filter(Armor.itemname == self.varimagename.get()):
                        items = ['Armor', row.itemhex]
                        self.itemObject.append(items)
                                 
                elif eqptype == 'ROBE':
                    for row in self.session.query(Robe).filter(Robe.itemname == self.varimagename.get()):
                        items = ['Robe', row.itemhex]
                        self.itemObject.append(items)
                        
                elif eqptype == 'HELM':                    
                    for row in self.session.query(Helm).filter(Helm.itemname == self.varimagename.get()):
                        items = ['Helm', row.itemhex]
                        self.itemObject.append(items)
                        
            else:
                for row in self.session.query(item).filter(item.itemname == self.varimagename.get()):
                    items = ['Other', row.itemhex]
                    self.itemObject.append(items)
        else:
            items = [None, None]    
            self.itemObject.append(items)
        
        if self.color != None:
            print('Received color {}'.format(self.color))
            self.itemObject.append(self.color)
                
        else:
            self.itemObject.append(0)
            
        self.itemObject.append(self.varrare.get())
                        
        self.itemObject.append(self.login)
        self.itemObject.append(self.note.get())
        self.gear.addItem(self.itemObject, self.stats, self)
        self.gear.processSubmit()
        self.gear.writeGear()
        
    
    def createArmor(self):
        self.wepproc.config(state = 'disabled')
        self.maxstack.config(state = 'disabled')
        self.atktype.config(state = 'disabled')
        self.wpndmg.config(state = 'disabled')
        self.armproc.config(state = 'normal')
        self.strength.config(state = 'normal')
        self.sta.config(state = 'normal')
        self.agi.config(state = 'normal')
        self.dex.config(state = 'normal')
        self.wis.config(state = 'normal')
        self.intel.config(state = 'normal')
        self.cha.config(state = 'normal')
        self.HPM.config(state = 'normal')
        self.POWM.config(state = 'normal')
        self.PoT.config(state = 'normal')
        self.HoT.config(state = 'normal')
        self.AC.config(state = 'normal')
        self.PR.config(state = 'normal')
        self.DR.config(state = 'normal')
        self.FR.config(state = 'normal')
        self.CR.config(state = 'normal')
        self.LR.config(state = 'normal')
        self.AR.config(state = 'normal')
        self.maxhp.config(state = 'normal')
        self.dur.config(state = 'normal')

    def createWeapon(self):
        self.maxstack.config(state = 'disabled')
        self.armproc.config(state = 'disabled')
        self.wepproc.config(state = 'normal')
        self.strength.config(state = 'normal')
        self.sta.config(state = 'normal')
        self.agi.config(state = 'normal')
        self.dex.config(state = 'normal')
        self.wis.config(state = 'normal')
        self.intel.config(state = 'normal')
        self.cha.config(state = 'normal')
        self.HPM.config(state = 'normal')
        self.POWM.config(state = 'normal')
        self.PoT.config(state = 'normal')
        self.HoT.config(state = 'normal')
        self.AC.config(state = 'normal')
        self.PR.config(state = 'normal')
        self.DR.config(state = 'normal')
        self.FR.config(state = 'normal')
        self.CR.config(state = 'normal')
        self.LR.config(state = 'normal')
        self.AR.config(state = 'normal')
        self.atktype.config(state = 'normal')
        self.wpndmg.config(state = 'normal')
        self.maxhp.config(state = 'normal')
        self.dur.config(state = 'normal')        
    
    def createItems(self):
        self.strength.config(state = 'disabled')
        self.sta.config(state = 'disabled')
        self.agi.config(state = 'disabled')
        self.dex.config(state = 'disabled')
        self.wis.config(state = 'disabled')
        self.intel.config(state = 'disabled')
        self.cha.config(state = 'disabled')
        self.HPM.config(state = 'disabled')
        self.POWM.config(state = 'disabled')
        self.PoT.config(state = 'disabled')
        self.HoT.config(state = 'disabled')
        self.AC.config(state = 'disabled')
        self.PR.config(state = 'disabled')
        self.DR.config(state = 'disabled')
        self.FR.config(state = 'disabled')
        self.CR.config(state = 'disabled')
        self.LR.config(state = 'disabled')
        self.AR.config(state = 'disabled')
        self.atktype.config(state = 'disabled')
        self.wpndmg.config(state = 'disabled')
        self.maxhp.config(state = 'disabled')
        self.dur.config(state = 'disabled')

        
    
    def processEquip(self, *args):

        [self.createArmor() for x in equipEnum.armor if x == self.vare.get()]
        [self.createWeapon() for x in equipEnum.weapon if x == self.vare.get()]
        [self.createItems() for x in equipEnum.items if x == self.vare.get()]
        
        choice = self.vare.get()
        if choice:
            if choice == 'ROBE':
                self.imagename.config(values = 'Robe')
                self.varimagetype.set('Robe')
 
            elif choice == 'CHEST' or choice == 'GLOVES' or choice == 'FEET' or choice == 'BRACERS' or choice == 'BRACELET' or choice == 'LEGS':
                self.imagename.config(values = 'Armor')
                self.varimagetype.set('Armor')
            
            elif choice == 'HELM':
                self.imagename.config(values = 'Helm')
                self.varimagetype.set('Helm')
        
    def displayItems(self):
        self.gear.addSession(self.session, self.window)
        self.viewItem.clear()
        self.editItem.clear()
        self.qcItem.clear()
        
        #Acquires EquipmentTypes
        self.eqpTypes = [] 
        for row in self.session.query(equipTypes):
            self.eqpTypes.append(row.equipmenttype)

        #Acquires attack types 
        self.atkTypes = []
        for row in self.session.query(attackTypes):
            self.atkTypes.append(row.attacktype)
             
        #Acquires Icons
        self.icons = []
        for row in self.session.query(icons):
            self.icons.append(row.image)

        #Acquire IconTypes
        self.iconTypes = []
        for row in self.session.query(icontype):
            self.iconTypes.append(row.icontype)
        self.iconTypes.sort()
        self.iconTypes.insert(0, None)

        #Acquire regions
        self.regions = []
        for row in self.session.query(regions):
            self.regions.append(row.region)
        self.regions.sort()
        self.regions.insert(0, None)
            
        #Acquires ItemTypes
        self.itemTypes = []
        for row in self.session.query(itemtype):
                self.itemTypes.append(row.itemtype)
        self.itemTypes.append('Helm')
        self.itemTypes.append('Armor')
        self.itemTypes.append('Robe')
        self.itemTypes.sort()
        self.itemTypes.insert(0, None)
        
        #Rarity types
        self.rarity = []
        for row in self.session.query(Rarity):
            self.rarity.append(row.rarity)
        self.rarity.sort()
                
        
        #Function for majority of Labels
        newLabels = labelFunc(self.window)
        self.labels.append(newLabels)
            
        self.NoT = Checkbutton(self.window, text="No Trade: ", width = 9, variable = self.vart)
        self.NoT.place(x = 930, y = 320)
        self.labels.append(self.NoT)
        self.rent = Checkbutton(self.window, text="Rent:        ", width = 9, variable = self.varr)
        self.rent.place(x = 930, y = 340)
        self.labels.append(self.rent)
        self.lore = Checkbutton(self.window, text="Lore:        ", width = 9, variable = self.varl)
        self.lore.place(x = 930, y = 360)
        self.labels.append(self.lore)
        self.craft = Checkbutton(self.window, text="Craft Mat: ", width = 9, variable = self.varc)
        self.craft.place(x = 930, y = 380)
        self.labels.append(self.craft)

        #textboxs
        self.itemname = Entry(self.window, width = 30, bg = "white")
        self.itemname.place(x = 240, y = 320)
        self.entry.append(self.itemname)
        self.itemid = Entry(self.window, width = 30, bg = "white")
        self.itemid.place(x = 240, y = 340)
        self.entry.append(self.itemid) 
        self.itemfam = Entry(self.window, width = 30, bg = "white")
        self.itemfam.place(x = 240, y = 360)
        self.entry.append(self.itemfam)
        self.icont = ttk.Combobox(self.window, textvariable = self.varitem, values = self.iconTypes, state = "readonly", width = 30)
        self.icont.place(x = 240, y = 380)
        self.labels.append(self.icont)
        self.icon = ttk.Combobox(self.window, textvariable = self.vari, state = "readonly", width = 30)
        self.icon.place(x = 240, y = 400)
        self.labels.append(self.icon)

        #Detects changes for Icon Types
        self.varitem.trace("w", self.createIconNames)
        
        #Detects changes with Icon name choice for image
        self.vari.trace("w", self.change_image)
        
        self.eqpslot = OptionMenu(self.window, self.vare, *self.eqpTypes, command = self.processEquip)
        self.eqpslot.place(x = 240, y = 420)
        self.labels.append(self.eqpslot)
        self.atktype = OptionMenu(self.window, self.vara, *self.atkTypes)
        self.atktype.place(x = 240, y = 450)
        self.labels.append(self.atktype)
        self.wpndmg = Entry(self.window, width = 10, bg = "white")
        self.wpndmg.place(x = 240, y = 480)
        self.entry.append(self.wpndmg)
        self.lvlreq = Entry(self.window, width = 10, bg = "white")
        self.lvlreq.place(x = 240, y = 500)
        self.entry.append(self.lvlreq)
        self.region = ttk.Combobox(self.window, textvariable = self.varreg, values = self.regions, width = 30, state = "readonly")
        self.region.place(x = 240, y = 520)
        self.labels.append(self.region)
        self.rare = ttk.Combobox(self.window, textvariable = self.varrare, values = self.rarity, width = 30, state = "readonly")
        self.rare.place(x = 240, y = 540)
        self.labels.append(self.rare)
        self.maxstack = Entry(self.window, width = 30, bg = "white")
        self.maxstack.place(x = 680, y = 320)
        self.entry.append(self.maxstack)
        self.itemdesc = Entry(self.window, width = 30, bg = "white")
        self.itemdesc.place(x = 680, y = 340)
        self.entry.append(self.itemdesc)
        self.maxhp = Entry(self.window, width = 30, bg = "white")
        self.maxhp.place(x = 680, y = 360)
        self.entry.append(self.maxhp)
        self.dur = Entry(self.window, width = 30, bg = "white")
        self.dur.place(x = 680, y = 380)
        self.entry.append(self.dur)
        self.wepproc = Entry(self.window, width = 30, bg = "white")
        self.wepproc.place(x = 680, y = 400)
        self.entry.append(self.wepproc)
        self.armproc = Entry(self.window, width = 30, bg = "white")
        self.armproc.place(x = 680, y = 420)
        self.entry.append(self.armproc)
        self.procname = Entry(self.window, width = 30, bg = "white")
        self.procname.place(x = 680, y = 440)
        self.entry.append(self.procname)

        #Classes
        self.alc = Checkbutton(self.window, variable = self.alcint, text = "ALC", onvalue = 2**classSummary.ALC, width = 3, bg = "white")
        self.alc.place(x = 680, y = 465)
        self.labels.append(self.alc)
        self.brd = Checkbutton(self.window, variable = self.brdint, text = "BRD", onvalue = 2**classSummary.BRD, width = 3, bg = "white")
        self.brd.place(x = 730, y = 465)
        self.labels.append(self.brd)
        self.cl = Checkbutton(self.window, variable = self.clint, text = "CL", onvalue = 2**classSummary.CL, width = 3, bg = "white")
        self.cl.place(x = 780, y = 465)
        self.labels.append(self.cl)
        self.drd = Checkbutton(self.window, variable = self.drdint, text = "DRD", onvalue = 2**classSummary.DRD, width = 3, bg = "white")
        self.drd.place(x = 830, y = 465)
        self.labels.append(self.drd)
        self.enc = Checkbutton(self.window, variable = self.encint,  text = "ENC", onvalue = 2**classSummary.ENC, width = 3, bg = "white")
        self.enc.place(x = 880, y = 465)
        self.labels.append(self.enc)
        self.mag = Checkbutton(self.window, variable = self.magint, text = "MAG", onvalue = 2**classSummary.MAG, width = 3, bg = "white")
        self.mag.place(x = 680, y = 485)
        self.labels.append(self.mag)
        self.mnk = Checkbutton(self.window, variable = self.mnkint, text = "MNK", onvalue = 2**classSummary.MNK, width = 3, bg = "white")
        self.mnk.place(x = 730, y = 485)
        self.labels.append(self.mnk)
        self.nec = Checkbutton(self.window, variable = self.necint, text = "NEC", onvalue = 2**classSummary.NEC, width = 3, bg = "white")
        self.nec.place(x = 780, y = 485)
        self.labels.append(self.nec)
        self.pal = Checkbutton(self.window, variable = self.palint, text = "PAL", onvalue = 2**classSummary.PAL, width = 3, bg = "white")
        self.pal.place(x = 830, y = 485)
        self.labels.append(self.pal)
        self.ran = Checkbutton(self.window, variable = self.ranint, text = "RAN", onvalue = 2**classSummary.RAN, width = 3, bg = "white")
        self.ran.place(x = 880, y = 485)
        self.labels.append(self.ran)
        self.rge = Checkbutton(self.window, variable = self.rgeint, text = "RGE", onvalue = 2**classSummary.RGE, width = 3, bg = "white")
        self.rge.place(x = 650, y = 505)
        self.labels.append(self.rge)
        self.sk = Checkbutton(self.window, variable = self.skint, text = "SK", onvalue = 2**classSummary.SK, width = 3, bg = "white")
        self.sk.place(x = 700, y = 505)
        self.labels.append(self.sk)
        self.sha = Checkbutton(self.window, variable = self.shaint, text = "SHA", onvalue = 2**classSummary.SHA, width = 3, bg = "white")
        self.sha.place(x = 750, y = 505)
        self.labels.append(self.sha) 
        self.war = Checkbutton(self.window, variable = self.warint, text = "WAR", onvalue = 2**classSummary.WAR, width = 3, bg = "white")
        self.war.place(x = 800, y = 505)
        self.labels.append(self.war)
        self.wiz = Checkbutton(self.window, variable = self.wizint, text = "WIZ", onvalue = 2**classSummary.WIZ, width = 3, bg = "white")
        self.wiz.place(x = 850, y = 505)
        self.labels.append(self.wiz)
        self.all = Checkbutton(self.window, variable = self.allint, text = "ALL", onvalue = 32767, width = 3, bg = "white")
        self.all.place(x = 900, y = 505)
        self.labels.append(self.all)
        self.tank = Checkbutton(self.window, variable = self.tankint, text = "TANK", onvalue = 13, width = 6, bg = "white")
        self.tank.place(x = 660, y = 525)
        self.labels.append(self.tank)
        self.heal = Checkbutton(self.window, variable =  self.healint, text = "HEAL", onvalue = 896, width = 6, bg = "white")
        self.heal.place(x = 730, y = 525)
        self.labels.append(self.heal)
        self.mel = Checkbutton(self.window, variable = self.melint, text = "MELEE", onvalue = 114, width = 6, bg = "white")
        self.mel.place(x = 800, y = 525)
        self.labels.append(self.mel) 
        self.cast = Checkbutton(self.window, variable = self.castint, text = "CASTER", onvalue = 31744, width = 6, bg = "white")
        self.cast.place(x = 870, y = 525)
        self.labels.append(self.cast)
        
        #Races
        self.hum = Checkbutton(self.window, variable = self.humint, text = "HUM", onvalue = 2**raceSummary.HUM, width = 3, bg = "white")
        self.hum.place(x = 680, y = 560)
        self.labels.append(self.hum)
        self.elf = Checkbutton(self.window, variable = self.elfint, text = "ELF", onvalue = 2**raceSummary.ELF, width = 3, bg = "white")
        self.elf.place(x = 730, y = 560)
        self.labels.append(self.elf)
        self.delf = Checkbutton(self.window, variable = self.delfint, text = "DELF", onvalue = 2**raceSummary.DELF, width = 3, bg = "white")
        self.delf.place(x = 780, y = 560)
        self.labels.append(self.delf)
        self.gno = Checkbutton(self.window, variable = self.gnoint, text = "GNO", onvalue = 2**raceSummary.GNO, width = 3, bg = "white")
        self.gno.place(x = 830, y = 560)
        self.labels.append(self.gno)
        self.dwf = Checkbutton(self.window, variable = self.dwfint, text = "DWF", onvalue = 2**raceSummary.DWF, width = 3, bg = "white")
        self.dwf.place(x = 880, y = 560)
        self.labels.append(self.dwf)
        self.trl = Checkbutton(self.window, variable = self.trlint, text = "TRL", onvalue = 2**raceSummary.TRL, width = 3, bg = "white")
        self.trl.place(x = 680, y = 580)
        self.labels.append(self.trl)
        self.bar = Checkbutton(self.window, variable = self.barint, text = "BAR", onvalue = 2**raceSummary.BAR, width = 3, bg = "white")
        self.bar.place(x = 730, y = 580)
        self.labels.append(self.bar)
        self.hlf = Checkbutton(self.window, variable = self.hlfint, text = "HLF", onvalue = 2**raceSummary.HLF, width = 3, bg = "white")
        self.hlf.place(x = 780, y = 580)
        self.labels.append(self.hlf)
        self.eru = Checkbutton(self.window, variable = self.eruint, text = "ERU", onvalue = 2**raceSummary.ERU, width = 3, bg = "white")
        self.eru.place(x = 830, y = 580)
        self.labels.append(self.eru)
        self.ogr = Checkbutton(self.window, variable = self.ogrint, text = "OGR", onvalue = 2**raceSummary.OGR, width = 3, bg = "white")
        self.ogr.place(x = 880, y = 580)
        self.labels.append(self.ogr)
        self.rall = Checkbutton(self.window, variable = self.rallint, text = "ALL", onvalue = 1023, width = 3, bg = "white")
        self.rall.place(x = 680, y = 600)
        self.labels.append(self.rall)

        #stats
        self.strength = Entry(self.window, width = 5, bg = "white")
        self.strength.place(x = 1180, y = 320)
        self.entry.append(self.strength)
        self.sta = Entry(self.window, width = 5, bg = "white")
        self.sta.place(x = 1180, y = 340)
        self.entry.append(self.sta)
        self.agi = Entry(self.window, width = 5, bg = "white")
        self.agi.place(x = 1180, y = 360)
        self.entry.append(self.agi)
        self.dex = Entry(self.window, width = 5, bg = "white")
        self.dex.place(x = 1180, y = 380)
        self.entry.append(self.dex)
        self.wis = Entry(self.window, width = 5, bg = "white")
        self.wis.place(x = 1180, y = 400)
        self.entry.append(self.wis)
        self.intel = Entry(self.window, width = 5, bg = "white")
        self.intel.place(x = 1180, y = 420)
        self.entry.append(self.intel)
        self.cha = Entry(self.window, width = 5, bg = "white")
        self.cha.place(x = 1180, y = 440)
        self.entry.append(self.cha)
        self.HPM = Entry(self.window, width = 5, bg = "white")
        self.HPM.place(x = 1180, y = 460)
        self.entry.append(self.HPM)
        self.POWM = Entry(self.window, width = 5, bg = "white")
        self.POWM.place(x = 1180, y = 480)
        self.entry.append(self.POWM)
        self.PoT = Entry(self.window, width = 5, bg = "white")
        self.PoT.place(x = 1380, y = 320)
        self.entry.append(self.PoT)
        self.HoT = Entry(self.window, width = 5, bg = "white")
        self.HoT.place(x = 1380, y = 340)
        self.entry.append(self.HoT)
        self.AC = Entry(self.window, width = 5, bg = "white")
        self.AC.place(x = 1380, y = 360)
        self.entry.append(self.AC)
        self.PR = Entry(self.window, width = 5, bg = "white")
        self.PR.place(x = 1380, y = 380)
        self.entry.append(self.PR)
        self.DR = Entry(self.window, width = 5, bg = "white")
        self.DR.place(x = 1380, y = 400)
        self.entry.append(self.DR)
        self.FR = Entry(self.window, width = 5, bg = "white")
        self.FR.place(x = 1380, y = 420)
        self.entry.append(self.FR)
        self.CR = Entry(self.window, width = 5, bg = "white")
        self.CR.place(x = 1380, y = 440)
        self.entry.append(self.CR)
        self.LR = Entry(self.window, width = 5, bg = "white")
        self.LR.place(x = 1380, y = 460)
        self.entry.append(self.LR)
        self.AR = Entry(self.window, width = 5, bg = "white")
        self.AR.place(x = 1380, y = 480)
        self.entry.append(self.AR)
        
        #3d Selector portion
        self.imagetype = ttk.Combobox(self.window, textvariable = self.varimagetype, values = self.itemTypes, state = "readonly", width = 30)
        self.imagetype.place(x = 1180, y = 500)
        self.labels.append(self.imagetype)
        self.imagename = ttk.Combobox(self.window, textvariable = self.varimagename, state = "readonly", width = 30)
        self.imagename.place(x = 1180, y = 520)
        self.labels.append(self.imagename)
        
        #Detects changes for Image Type
        self.varimagetype.trace("w", self.createImageNames)
        
        #Detects changes for Image Name
        self.varimagename.trace("w", self.create3dImage)
        
        #notes
        self.note = Entry(self.window, width = 100, bg = "white")
        self.note.place(x = 150, y = 850)
        self.entry.append(self.note)

        #Buttons
        self.submit = Label(self.window, text="Submit to Database", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 50, y = 575)
        self.labels.append(self.submit)
        sub = Button(self.window, text = "SUBMIT", width = 6, command = self.click)
        sub.place(x = 50, y = 600)
        self.labels.append(sub) 
        
        self.colors = Button(text = 'Select Color for item', command = self.getColor)
        self.colors.place(x = 1300, y = 100)
        self.labels.append(self.colors)
        
        
    def clear(self):
        self.removeEntry()
        if self.entry:
            for entry in self.entry:
                entry.destroy()        
            self.entry.clear()
        if self.labels:
            for i in self.labels:
                if type(i) == list:
                    for a in i:
                        a.destroy()
                else:
                    i.destroy()
        if self.picture:
            for pic in self.picture:
                pic.destroy()
        if self.error:
            self.error.destroy()
        self.gear.destroyError()
        #print('Page de-construction completed')
        
    def change_image(self, *args):
        pic = self.vari.get()
        if pic == None:
            return
        for row in self.session.query(icons).filter(icons.iconname == pic):  
            # Change image of label accordingly
            self.pict = (PhotoImage(data = row.image))

        self.label = Label(self.window, image = self.pict, bg = "black")
        self.label.place(x = 390, y = 420)
        self.picture.append(self.label)
        
    def createIconNames(self, *args):
        self.iconNames.clear()
        iconType = self.varitem.get()
        if iconType == None or iconType == '':
            return
   
        row = self.session.query(icons.iconname).join(icontype).filter(icontype.icontype == iconType)
        for a in row.all():
            self.iconNames.append(a.iconname)
            
        self.iconNames.sort()
        self.iconNames.insert(0, None)
        self.icon.config(values = self.iconNames)

    def getColor(self):
        thisColor = None
        if self.color == None:
            pass
        else:
            print('Pre-defined color')
            thisColor = '0x' + hex(self.color)[2:].zfill(8)
            thisColor = thisColor.replace('0x', '#')[:-2]
            
        color = colorchooser.askcolor(color = thisColor, title = "Select color for this item, if any")
        if color == (None, None):
            return
        color = (color[1].strip('#'))+'FF'
        self.color = int(color, 16)
        
    def createImageNames(self, *args):
        self.imageNames.clear()
        itemType = self.varimagetype.get()
        if itemType == None or itemType == '':
            return
  
        if itemType == 'Robe' or itemType == 'Armor' or itemType == 'Helm':
            if itemType == 'Robe':
                for row in self.session.query(Robe):
                    self.imageNames.append(row.itemname)
                    
            elif itemType == 'Helm':
                for row in self.session.query(Helm):
                    self.imageNames.append(row.itemname)
                    
            elif itemType == 'Armor':
                for row in self.session.query(Armor):
                    self.imageNames.append(row.itemname)
        else:            
            row = self.session.query(item.itemname).join(itemtype).filter(itemtype.itemtype == itemType)
            for a in row.all():
                self.imageNames.append(a.itemname)
        
        self.imageNames.sort()
        self.imagename.config(values = self.imageNames)

    def create3dImage(self, *args):
        name = self.varimagename.get()
        self.image = None
        if name  == 'None':
            return
            
        if self.varimagetype.get() == 'Robe' or self.varimagetype.get() == 'Armor' or self.varimagetype.get() == 'Helm':
            if self.varimagetype.get() == 'Robe':
                for row in self.session.query(Robe).filter(Robe.itemname == name):
                    self.image = PhotoImage(data = row.image)

            elif self.varimagetype.get() == 'Helm':
                for row in self.session.query(Helm).filter(Helm.itemname == name):
                    self.image = PhotoImage(data = row.image)

            elif self.varimagetype.get() == 'Armor':
                for row in self.session.query(Armor).filter(Armor.itemname == name):                  
                    self.image = PhotoImage(data = row.image)

        else:
            for row in self.session.query(item).filter(item.itemname == name):
                self.image = PhotoImage(data = row.image)
        
        self.label = Label(self.window, image = self.image, bg = "black")
        self.label.place(x = 1030, y = 600)
        self.picture.append(self.label)
        
    def removeEntry(self):
        for entry in self.entry:
            if entry:
                entry.delete(0, END)
        
        for checkbox in self.checkbox:
            if checkbox:
                checkbox.set(0)
       
        for combobox in self.combobox:
            if combobox:
                combobox.set('')

        
#Function to trim file down abit
def labelFunc(window):
        labels = []
        
        #labels
        label = Label (window, text="Item name:        ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 320)
        labels.append(label)
        label = Label (window, text="Pattern ID:       ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 340)
        labels.append(label)
        label = Label (window, text="Pattern Family:   ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 360)
        labels.append(label)
        label = Label (window, text="Item Type:   ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 380)
        labels.append(label)
        label = Label (window, text="Item Icon:        ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 400)
        labels.append(label)
        label = Label (window, text="Equipment Slot:   ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 420)
        labels.append(label)
        label = Label (window, text="AttackType:       ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 450)
        labels.append(label)
        label = Label (window, text="Weapon Damage:    ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 480)
        labels.append(label)
        label = Label (window, text="Level Requirement:", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 500)
        labels.append(label)
        label = Label (window, text="Regions:", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 520)
        labels.append(label)   
        label = Label (window, text="Rarity:", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 540)
        labels.append(label)      

        label = Label (window, text="Max Stack:        ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 520, y = 320)
        labels.append(label)
        label = Label (window, text="Item Description: ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 520, y = 340)
        labels.append(label)
        label = Label (window, text="Max HP:           ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 520, y = 360)
        labels.append(label)
        label = Label (window, text="Durability:       ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 520, y = 380)
        labels.append(label) 
        label = Label (window, text="Weapon Proc:     ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 520, y = 400)
        labels.append(label)
        label = Label (window, text="Armor Proc:      ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 520, y = 420)
        labels.append(label)
        label = Label (window, text="Proc Name:        ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 520, y = 440)
        labels.append(label)
        label = Label (window, text="Class:            ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 520, y = 465)
        labels.append(label)
        label = Label (window, text="Race:             ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 520, y = 560)
        labels.append(label)

        
        label = Label(window, text="Status:          ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 600, y = 700)
        labels.append(label)

        label = Label(window, text="Strength:     ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 320)
        labels.append(label)
        label = Label(window, text="Stamina:      ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 340)
        labels.append(label)
        label = Label(window, text="Agility:      ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 360)
        labels.append(label)
        label = Label(window, text="Dexterity:    ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 380)
        labels.append(label)
        label = Label(window, text="Wisdom:       ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 400)
        labels.append(label)
        label = Label(window, text="Intelligence: ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 420)
        labels.append(label)
        label = Label(window, text="Charisma:     ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 440)
        labels.append(label)
        label = Label(window, text="HP Max:       ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 460)
        labels.append(label)
        label = Label(window, text="POW MAX:      ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 480)
        labels.append(label)
        label = Label(window, text="3D Image Type:", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 500)
        labels.append(label)
        label = Label(window, text="3D Image Name:", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1030, y = 520)
        labels.append(label)       
    
        label = Label(window, text="PoT:             ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1230, y = 320)
        labels.append(label)
        label = Label(window, text="HoT:             ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1230, y = 340)
        labels.append(label)
        label = Label(window, text="AC:              ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1230, y = 360)
        labels.append(label)
        label = Label(window, text="Poison Resist:   ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1230, y = 380)
        labels.append(label)
        label = Label(window, text="Disease Resist:  ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1230, y = 400)
        labels.append(label)
        label = Label(window, text="Fire Resist:     ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1230, y = 420)
        labels.append(label)
        label = Label(window, text="Cold Resist:     ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1230, y = 440)
        labels.append(label)
        label = Label(window, text="Lightning Resist:", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1230, y = 460)
        labels.append(label)
        label = Label(window, text="Arcane Resist:   ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 1230, y = 480)
        labels.append(label)
        
        label = Label(window, text="Notes:   ", bg = "black", fg = "white", font = "none 12 bold")
        label.place(x = 50, y = 850)
        labels.append(label)
        
        return labels        

class itemOptions:
    
    def __init__(self, window, login):
        self.bun        = []
        self.window     = window
        self.Characters = None
        self.Spells     = None
        self.session    = None
        self.Tables     = None
        self.login      = login
        self.viewItem   = viewItem(self.window)       
        self.qcItem     = qcItem(self.window, self.login)
        self.editItem   = editItem(self.window, self.login)
        self.createItem = createItem(self.window, self.login)
        self.access     = None 
        
    
    def selectOption(self):
        
        #Admin    
        if self.access == 0:
            but5 = Button(self.window, text = "View", width = 10, command = self.viewItem.displayItems)
            but5.place(x = 120, y = 35)
            self.bun.append(but5)
            but6 = Button(self.window, text = "Create", width = 10, command = self.createItem.displayItems)
            but6.place(x = 120, y = 65)
            self.bun.append(but6)
            but7 = Button(self.window, text = "Edit", width = 10, command = self.editItem.displayItems)
            but7.place(x = 120, y = 95)
            self.bun.append(but7)
            but7 = Button(self.window, text = "QC", width = 10, command = self.qcItem.displayItems)
            but7.place(x = 120, y = 125)
            self.bun.append(but7)
        
        #Creator    
        elif self.access == 1:
            but5 = Button(self.window, text = "View", width = 10, command = self.viewItem.displayItems)
            but5.place(x = 120, y = 35)
            self.bun.append(but5)
            but6 = Button(self.window, text = "Create", width = 10, command = self.createItem.displayItems)
            but6.place(x = 120, y = 65)
            self.bun.append(but6)
        
        #Editor    
        elif self.access == 2:
            but5 = Button(self.window, text = "View", width = 10, command = self.viewItem.displayItems)
            but5.place(x = 120, y = 35)
            self.bun.append(but5)
            but7 = Button(self.window, text = "Edit", width = 10, command = self.editItem.displayItems)
            but7.place(x = 120, y = 65)
            self.bun.append(but7)
        
        #QC    
        elif self.access == 3:
            but5 = Button(self.window, text = "View", width = 10, command = self.viewItem.displayItems)
            but5.place(x = 120, y = 35)
            self.bun.append(but5)
            but7 = Button(self.window, text = "Edit", width = 10, command = self.editItem.displayItems)
            but7.place(x = 120, y = 65)
            self.bun.append(but7)
            but7 = Button(self.window, text = "QC", width = 10, command = self.qcItem.displayItems)
            but7.place(x = 120, y = 95)
            self.bun.append(but7)  
            
        #Viewer    
        elif self.access == 4:
            but5 = Button(self.window, text = "View", width = 10, command = self.viewItem.displayItems)
            but5.place(x = 120, y = 35)
            self.bun.append(but5)
        
        else:
            print('How did you get here?')
    
    def setClasses(self, character, table, spell, session, access):
        self.Characters = character
        self.Tables     = table
        self.Spells     = spell
        self.session    = session
        self.access     = access 
        self.viewItem.setSession(self.session, self.createItem, self.editItem, self.qcItem, self.access)
        self.editItem.setSession(self.session, self.createItem, self.viewItem, self.qcItem)
        self.createItem.setSession(self.session, self.viewItem, self.editItem, self.qcItem)
        self.qcItem.setSession(self.session, self.viewItem, self.editItem, self.createItem)
        #print('Classes placed')
    
        
    def clearItemClass(self):
        if self.bun:
            for bun in self.bun:
                bun.destroy()
                
        self.viewItem.clear()
        self.editItem.clear()
        self.createItem.clear()
        self.qcItem.clear()
        #print('Page de-construction completed, item classes removed')
        
class viewItem:
    
    def __init__(self, window):
        self.access       = None
        self.session      = None
        self.window       = window
        self.labels       = []
        self.picture      = []
        self.varreg       = StringVar()
        self.vari         = StringVar(self.window)
        self.varp         = StringVar(self.window)
        self.varitem      = StringVar()
        self.varimagetype = StringVar()
        self.varimagename = StringVar()
        self.createstr    = StringVar()
        self.modifystr    = StringVar()
        self.auditstr     = StringVar()
        self.notes        = StringVar()
        self.varrare      = StringVar()
        self.lev          = StringVar()
        self.name         = StringVar()
        self.vardel       = IntVar()
        self.auditapprv   = IntVar()
        self.equip        = StringVar()
        self.iconNames    = []
        self.imageNames   = []
        self.photo        = []
        self.offvalue     = 0
        self.pict         = None
        self.qcItem       = None
        self.createItem   = None
        self.editItem     = None
        
        #For writing to entry boxes
        self.warint      = IntVar()
        self.brdint      = IntVar()
        self.alcint      = IntVar()
        self.clint       = IntVar()
        self.drdint      = IntVar()
        self.encint      = IntVar()
        self.magint      = IntVar()
        self.mnkint      = IntVar()
        self.necint      = IntVar()
        self.palint      = IntVar()
        self.ranint      = IntVar()
        self.rgeint      = IntVar()
        self.skint       = IntVar()
        self.shaint      = IntVar()
        self.wizint      = IntVar()
        self.allint      = IntVar()
        self.tankint     = IntVar()
        self.healint     = IntVar()
        self.melint      = IntVar()
        self.castint     = IntVar()
        self.humint      = IntVar()
        self.elfint      = IntVar()
        self.delfint     = IntVar()
        self.gnoint      = IntVar()
        self.dwfint      = IntVar()
        self.trlint      = IntVar()
        self.barint      = IntVar()
        self.hlfint      = IntVar()
        self.eruint      = IntVar()
        self.ogrint      = IntVar()
        self.rallint     = IntVar()
        self.itemstr     = StringVar()
        self.idint       = StringVar()
        self.famint      = StringVar()
        self.equipstr    = StringVar()
        self.atktypestr  = StringVar()
        self.wpnint      = StringVar()
        self.levelint    = StringVar()
        self.maxint      = StringVar()
        self.itemdescstr = StringVar()
        self.maxhpint    = StringVar()
        self.durint      = StringVar()
        self.classint    = StringVar()
        self.raceint     = StringVar()
        self.wepprocint  = StringVar()
        self.armprocint  = StringVar()
        self.procnamestr = StringVar()
        self.strint      = StringVar()
        self.staint      = StringVar()
        self.agiint      = StringVar()
        self.dexint      = StringVar()
        self.wisint      = StringVar()
        self.intint      = StringVar()
        self.chaint      = StringVar()
        self.hpmint      = StringVar()
        self.powint      = StringVar()
        self.PoTint      = StringVar()
        self.HoTint      = StringVar()
        self.ACint       = StringVar()
        self.PRint       = StringVar()
        self.DRint       = StringVar()
        self.FRint       = StringVar()
        self.CRint       = StringVar()
        self.LRint       = StringVar()
        self.ARint       = StringVar()
        self.tvalue      = IntVar()
        self.rvalue      = IntVar()
        self.lvalue      = IntVar()
        self.cvalue      = IntVar()        
        self.entry       = []
        self.checkbox    = [self.warint,
                            self.brdint,
                            self.alcint,
                            self.clint ,
                            self.drdint,
                            self.encint,
                            self.magint,
                            self.mnkint,
                            self.necint,
                            self.palint,
                            self.ranint,
                            self.rgeint,
                            self.skint ,
                            self.shaint,
                            self.wizint,
                            self.allint,
                            self.tankint,
                            self.healint,
                            self.melint,
                            self.castint,
                            self.humint,
                            self.elfint,
                            self.delfint,
                            self.gnoint,
                            self.dwfint,
                            self.trlint,
                            self.barint,
                            self.hlfint,
                            self.eruint,
                            self.ogrint,
                            self.rallint,
                            self.tvalue,
                            self.rvalue,
                            self.lvalue,
                            self.cvalue]
                            
        self.combobox    = [self.varitem,
                            self.varreg,
                            self.varp,
                            self.vari,
                            self.varimagetype,
                            self.varimagename,
                            self.equipstr,
                            self.atktypestr,
                            self.varrare]
        
        

    def setSession(self, session, createItem, editItem, qcItem, access):
        self.session    = session
        self.createItem = createItem
        self.editItem   = editItem
        self.qcItem     = qcItem
        self.access     = access
    
    def clear(self):
        for entry in self.entry:
            if entry:
                entry.config(state = 'normal')
                
        self.removeEntry()
        
        if self.entry:
            for entry in self.entry:
                entry.destroy()   
            self.entry.clear()
            
        if self.labels:
            for i in self.labels:
                if type(i) == list:
                    for a in i:
                        a.destroy()
                else:
                    i.destroy()
        if self.picture:
            for pic in self.picture:
                pic.destroy()
        #print('Page de-construction completed')
                
    def displayItems(self):
        self.createItem.clear()
        self.editItem.clear()
        self.qcItem.clear()
        
        #Acquires EquipmentTypes
        self.equipTypes = [] 
        for row in self.session.query(equipTypes):
            self.equipTypes.append(row.equipmenttype)

        #Acquires attack types
        self.atkTypes = []
        for row in self.session.query(attackTypes):
            self.atkTypes.append(row.attacktype)
             
        #Acquires Icons
        self.icons = []
        for row in self.session.query(icons):
            self.icons.append(row.image)

        
        #Acquires Items
        self.items = []
        for row in self.session.query(items):
            self.items.append(row.Itemname)
        
        #Acquires ItemTypes
        self.itemTypes = []
        for row in self.session.query(itemtype):
                self.itemTypes.append(row.itemtype)
        self.itemTypes.append('Helm')
        self.itemTypes.append('Armor')
        self.itemTypes.append('Robe')
        self.itemTypes.sort()
            
        #Function for majority of Labels
        newLabels = labelFunc(self.window)
        self.labels.append(newLabels)

        self.label = Checkbutton(self.window, text="No Trade: ", width = 9, variable = self.tvalue, state = "disabled")
        self.label.place(x = 930, y = 320)
        self.labels.append(self.label)
        self.label = Checkbutton(self.window, text="Rent:        ", width = 9, variable = self.rvalue, state = "disabled")
        self.label.place(x = 930, y = 340)
        self.labels.append(self.label)
        self.label = Checkbutton(self.window, text="Lore:        ", width = 9, variable = self.lvalue, state = "disabled")
        self.label.place(x = 930, y = 360)
        self.labels.append(self.label)
        self.label = Checkbutton(self.window, text="Craft Mat: ", width = 9, variable = self.cvalue, state = "disabled")
        self.label.place(x = 930, y = 380)
        self.labels.append(self.label)

        #textboxs
        self.itemname = Entry(self.window, width = 30, textvariable = self.itemstr, bg = "white", state = "readonly")
        self.itemname.place(x = 240, y = 320)
        self.entry.append(self.itemname)
        self.itemid = Entry(self.window, width = 30, textvariable = self.idint, bg = "white", state = "readonly")
        self.itemid.place(x = 240, y = 340)
        self.entry.append(self.itemid) 
        self.itemfam = Entry(self.window, width = 30, textvariable = self.famint, bg = "white", state = "readonly")
        self.itemfam.place(x = 240, y = 360)
        self.entry.append(self.itemfam)
        self.icont = ttk.Combobox(self.window, textvariable = self.varitem, value = '', state = "disabled")
        self.icont.place(x = 240, y = 380)
        self.labels.append(self.icont)
        self.icon = ttk.Combobox(self.window, textvariable = self.varp, value = '', state = "disabled")
        self.icon.place(x = 240, y = 400)
        self.labels.append(self.icon)
        
        self.varp.trace("w", self.change_image)
              
        self.eqpslot = OptionMenu(self.window, self.equipstr, *self.equipTypes)
        self.eqpslot.place(x = 240, y = 420)
        self.eqpslot.config(state = 'disabled')
        self.labels.append(self.eqpslot)
        self.atktype = OptionMenu(self.window, self.atktypestr, *self.atkTypes)
        self.atktype.place(x = 240, y = 450)
        self.atktype.config(state = 'disabled')
        self.labels.append(self.atktype)
        self.wpndmg = Entry(self.window, width = 10, textvariable = self.wpnint, bg = "white", state = "readonly")
        self.wpndmg.place(x = 240, y = 480)
        self.entry.append(self.wpndmg)
        self.lvlreq = Entry(self.window, width = 10, textvariable = self.levelint, bg = "white", state = "readonly")
        self.lvlreq.place(x = 240, y = 500)
        self.entry.append(self.lvlreq)
        self.region = ttk.Combobox(self.window, textvariable = self.varreg, width = 30, state = "disabled")
        self.region.place(x = 240, y = 520)
        self.labels.append(self.region)
        self.rare = ttk.Combobox(self.window, textvariable = self.varrare, width = 30, state = "disabled")
        self.rare.place(x = 240, y = 540)
        self.labels.append(self.rare)
        self.maxstack = Entry(self.window, width = 30, textvariable = self.maxint, bg = "white", state = "readonly")
        self.maxstack.place(x = 680, y = 320)
        self.entry.append(self.maxstack)
        self.itemdesc = Entry(self.window, width = 30, textvariable = self.itemdescstr, bg = "white", state = "readonly")
        self.itemdesc.place(x = 680, y = 340)
        self.entry.append(self.itemdesc)
        self.maxhp = Entry(self.window, width = 30, textvariable = self.maxhpint, bg = "white", state = "readonly")
        self.maxhp.place(x = 680, y = 360)
        self.entry.append(self.maxhp)
        self.dur = Entry(self.window, width = 30, textvariable = self.durint, bg = "white", state = "readonly")
        self.dur.place(x = 680, y = 380)
        self.entry.append(self.dur)
        self.wepproc = Entry(self.window, width = 30, textvariable = self.wepprocint, bg = "white", state = "readonly")
        self.wepproc.place(x = 680, y = 400)
        self.entry.append(self.wepproc)
        self.armproc = Entry(self.window, width = 30, textvariable = self.armprocint, bg = "white", state = "readonly")
        self.armproc.place(x = 680, y = 420)
        self.entry.append(self.armproc)
        self.procname = Entry(self.window, width = 30, textvariable = self.procnamestr, bg = "white", state = "readonly")
        self.procname.place(x = 680, y = 440)
        self.entry.append(self.procname)
                
        
        
        #Classes
        self.alc = Checkbutton(self.window, variable = self.alcint, text = "ALC", onvalue = 2**classSummary.ALC, width = 3, bg = "white", state = "disabled")
        self.alc.place(x = 680, y = 465)
        self.labels.append(self.alc)
        self.brd = Checkbutton(self.window, variable = self.brdint, text = "BRD", onvalue = 2**classSummary.BRD, width = 3, bg = "white", state = "disabled")
        self.brd.place(x = 730, y = 465)
        self.labels.append(self.brd)
        self.cl = Checkbutton(self.window, variable = self.clint, text = "CL", onvalue = 2**classSummary.CL, width = 3, bg = "white", state = "disabled")
        self.cl.place(x = 780, y = 465)
        self.labels.append(self.cl)
        self.drd = Checkbutton(self.window, variable = self.drdint, text = "DRD", onvalue = 2**classSummary.DRD, width = 3, bg = "white", state = "disabled")
        self.drd.place(x = 830, y = 465)
        self.labels.append(self.drd)
        self.enc = Checkbutton(self.window, variable = self.encint,  text = "ENC", onvalue = 2**classSummary.ENC, width = 3, bg = "white", state = "disabled")
        self.enc.place(x = 880, y = 465)
        self.labels.append(self.enc)
        self.mag = Checkbutton(self.window, variable = self.magint, text = "MAG", onvalue = 2**classSummary.MAG, width = 3, bg = "white", state = "disabled")
        self.mag.place(x = 680, y = 485)
        self.labels.append(self.mag)
        self.mnk = Checkbutton(self.window, variable = self.mnkint, text = "MNK", onvalue = 2**classSummary.MNK, width = 3, bg = "white", state = "disabled")
        self.mnk.place(x = 730, y = 485)
        self.labels.append(self.mnk)
        self.nec = Checkbutton(self.window, variable = self.necint, text = "NEC", onvalue = 2**classSummary.NEC, width = 3, bg = "white", state = "disabled")
        self.nec.place(x = 780, y = 485)
        self.labels.append(self.nec)
        self.pal = Checkbutton(self.window, variable = self.palint, text = "PAL", onvalue = 2**classSummary.PAL, width = 3, bg = "white", state = "disabled")
        self.pal.place(x = 830, y = 485)
        self.labels.append(self.pal)
        self.ran = Checkbutton(self.window, variable = self.ranint, text = "RAN", onvalue = 2**classSummary.RAN, width = 3, bg = "white", state = "disabled")
        self.ran.place(x = 880, y = 485)
        self.labels.append(self.ran)
        self.rge = Checkbutton(self.window, variable = self.rgeint, text = "RGE", onvalue = 2**classSummary.RGE, width = 3, bg = "white", state = "disabled")
        self.rge.place(x = 650, y = 505)
        self.labels.append(self.rge)
        self.sk = Checkbutton(self.window, variable = self.skint, text = "SK", onvalue = 2**classSummary.SK, width = 3, bg = "white", state = "disabled")
        self.sk.place(x = 700, y = 505)
        self.labels.append(self.sk)
        self.sha = Checkbutton(self.window, variable = self.shaint, text = "SHA", onvalue = 2**classSummary.SHA, width = 3, bg = "white", state = "disabled")
        self.sha.place(x = 750, y = 505)
        self.labels.append(self.sha) 
        self.war = Checkbutton(self.window, variable = self.warint, text = "WAR", onvalue = 2**classSummary.WAR, width = 3, bg = "white", state = "disabled")
        self.war.place(x = 800, y = 505)
        self.labels.append(self.war)
        self.wiz = Checkbutton(self.window, variable = self.wizint, text = "WIZ", onvalue = 2**classSummary.WIZ, width = 3, bg = "white", state = "disabled")
        self.wiz.place(x = 850, y = 505)
        self.labels.append(self.wiz)
        self.all = Checkbutton(self.window, variable = self.allint, text = "ALL", onvalue = 32767, width = 3, bg = "white", state = "disabled")
        self.all.place(x = 900, y = 505)
        self.labels.append(self.all)
        self.tank = Checkbutton(self.window, variable = self.tankint, text = "TANK", onvalue = 13, width = 6, bg = "white", state = "disabled")
        self.tank.place(x = 660, y = 525)
        self.labels.append(self.tank)
        self.heal = Checkbutton(self.window, variable =  self.healint, text = "HEAL", onvalue = 896, width = 6, bg = "white", state = "disabled")
        self.heal.place(x = 730, y = 525)
        self.labels.append(self.heal)
        self.mel = Checkbutton(self.window, variable = self.melint, text = "MELEE", onvalue = 114, width = 6, bg = "white", state = "disabled")
        self.mel.place(x = 800, y = 525)
        self.labels.append(self.mel) 
        self.cast = Checkbutton(self.window, variable = self.castint, text = "CASTER", onvalue = 31744, width = 6, bg = "white", state = "disabled")
        self.cast.place(x = 870, y = 525)
        self.labels.append(self.cast)
        
        #Races
        self.hum = Checkbutton(self.window, variable = self.humint, text = "HUM", onvalue = 2**raceSummary.HUM, width = 3, bg = "white", state = "disabled")
        self.hum.place(x = 680, y = 560)
        self.labels.append(self.hum)
        self.elf = Checkbutton(self.window, variable = self.elfint, text = "ELF", onvalue = 2**raceSummary.ELF, width = 3, bg = "white", state = "disabled")
        self.elf.place(x = 730, y = 560)
        self.labels.append(self.elf)
        self.delf = Checkbutton(self.window, variable = self.delfint, text = "DELF", onvalue = 2**raceSummary.DELF, width = 3, bg = "white", state = "disabled")
        self.delf.place(x = 780, y = 560)
        self.labels.append(self.delf)
        self.gno = Checkbutton(self.window, variable = self.gnoint, text = "GNO", onvalue = 2**raceSummary.GNO, width = 3, bg = "white", state = "disabled")
        self.gno.place(x = 830, y = 560)
        self.labels.append(self.gno)
        self.dwf = Checkbutton(self.window, variable = self.dwfint, text = "DWF", onvalue = 2**raceSummary.DWF, width = 3, bg = "white", state = "disabled")
        self.dwf.place(x = 880, y = 560)
        self.labels.append(self.dwf)
        self.trl = Checkbutton(self.window, variable = self.trlint, text = "TRL", onvalue = 2**raceSummary.TRL, width = 3, bg = "white", state = "disabled")
        self.trl.place(x = 680, y = 580)
        self.labels.append(self.trl)
        self.bar = Checkbutton(self.window, variable = self.barint, text = "BAR", onvalue = 2**raceSummary.BAR, width = 3, bg = "white", state = "disabled")
        self.bar.place(x = 730, y = 580)
        self.labels.append(self.bar)
        self.hlf = Checkbutton(self.window, variable = self.hlfint, text = "HLF", onvalue = 2**raceSummary.HLF, width = 3, bg = "white", state = "disabled")
        self.hlf.place(x = 780, y = 580)
        self.labels.append(self.hlf)
        self.eru = Checkbutton(self.window, variable = self.eruint, text = "ERU", onvalue = 2**raceSummary.ERU, width = 3, bg = "white", state = "disabled")
        self.eru.place(x = 830, y = 580)
        self.labels.append(self.eru)
        self.ogr = Checkbutton(self.window, variable = self.ogrint, text = "OGR", onvalue = 2**raceSummary.OGR, width = 3, bg = "white", state = "disabled")
        self.ogr.place(x = 880, y = 580)
        self.labels.append(self.ogr)
        self.rall = Checkbutton(self.window, variable = self.rallint, text = "ALL", onvalue = 1023, width = 3, bg = "white", state = "disabled")
        self.rall.place(x = 680, y = 600)
        self.labels.append(self.rall)
        
        self.strength = Entry(self.window, width = 5, textvariable = self.strint, bg = "white", state = "readonly")
        self.strength.place(x = 1180, y = 320)
        self.entry.append(self.strength)
        self.sta = Entry(self.window, width = 5, textvariable = self.staint, bg = "white", state = "readonly")
        self.sta.place(x = 1180, y = 340)
        self.entry.append(self.sta)
        self.agi = Entry(self.window, width = 5, textvariable = self.agiint, bg = "white", state = "readonly")
        self.agi.place(x = 1180, y = 360)
        self.entry.append(self.agi)
        self.dex = Entry(self.window, width = 5, textvariable = self.dexint, bg = "white", state = "readonly")
        self.dex.place(x = 1180, y = 380)
        self.entry.append(self.dex)
        self.wis = Entry(self.window, width = 5, textvariable = self.wisint, bg = "white", state = "readonly")
        self.wis.place(x = 1180, y = 400)
        self.entry.append(self.wis)
        self.intel = Entry(self.window, width = 5, textvariable = self.intint, bg = "white", state = "readonly")
        self.intel.place(x = 1180, y = 420)
        self.entry.append(self.intel)
        self.cha = Entry(self.window, width = 5, textvariable = self.chaint, bg = "white", state = "readonly")
        self.cha.place(x = 1180, y = 440)
        self.entry.append(self.cha)
        self.HPM = Entry(self.window, width = 5, textvariable = self.hpmint, bg = "white", state = "readonly")
        self.HPM.place(x = 1180, y = 460)
        self.entry.append(self.HPM)
        self.POWM = Entry(self.window, width = 5, textvariable = self.powint, bg = "white", state = "readonly")
        self.POWM.place(x = 1180, y = 480)
        self.entry.append(self.POWM)
        self.PoT = Entry(self.window, width = 5, textvariable = self.PoTint, bg = "white", state = "readonly")
        self.PoT.place(x = 1380, y = 320)
        self.entry.append(self.PoT)
        self.HoT = Entry(self.window, width = 5, textvariable = self.HoTint, bg = "white", state = "readonly")
        self.HoT.place(x = 1380, y = 340)
        self.entry.append(self.HoT)
        self.AC = Entry(self.window, width = 5, textvariable = self.ACint, bg = "white", state = "readonly")
        self.AC.place(x = 1380, y = 360)
        self.entry.append(self.AC)
        self.PR = Entry(self.window, width = 5, textvariable = self.PRint,  bg = "white", state = "readonly")
        self.PR.place(x = 1380, y = 380)
        self.entry.append(self.PR)
        self.DR = Entry(self.window, width = 5, textvariable = self.DRint,  bg = "white", state = "readonly")
        self.DR.place(x = 1380, y = 400)
        self.entry.append(self.DR)
        self.FR = Entry(self.window, width = 5, textvariable = self.FRint, bg = "white", state = "readonly")
        self.FR.place(x = 1380, y = 420)
        self.entry.append(self.FR)
        self.CR = Entry(self.window, width = 5, textvariable = self.CRint, bg = "white", state = "readonly")
        self.CR.place(x = 1380, y = 440)
        self.entry.append(self.CR)
        self.LR = Entry(self.window, width = 5, textvariable = self.LRint,bg = "white", state = "readonly")
        self.LR.place(x = 1380, y = 460)
        self.entry.append(self.LR)
        self.AR = Entry(self.window, width = 5, textvariable = self.ARint, bg = "white", state = "readonly")
        self.AR.place(x = 1380, y = 480)
        self.entry.append(self.AR)

        
        self.vari.trace("w", self.generateItem)

        #Search Parameters
        self.submit = Label(self.window, text="Search Parameters:", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 125, y = 560)
        self.labels.append(self.submit) 
        subm = Button(self.window, text = "SUBMIT", width = 6, command = self.generateItemList)
        subm.place(x = 360, y = 560)
        self.labels.append(subm)
        self.submit = Label(self.window, text="Has item been Audited?", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 50, y = 595)
        self.labels.append(self.submit) 
        self.auditcheck = Checkbutton(self.window, variable = self.auditapprv, text = "Yes", width = 10, bg = "white")
        self.auditcheck.place(x = 50, y = 620)
        self.labels.append(self.auditcheck)
        self.eqpbox = Label(self.window, text="Equip Type", bg = "black", fg = "white", font = "none 12 bold")
        self.eqpbox.place(x = 275, y = 595)
        self.labels.append(self.eqpbox)
        self.eqslot = OptionMenu(self.window, self.equip, *self.equipTypes)
        self.eqslot.place(x = 255, y = 620)
        self.equip.set(None)
        self.labels.append(self.eqslot)
        self.submit = Label(self.window, text="Level", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 390, y = 595)
        self.labels.append(self.submit)
        self.level = Entry(self.window, width = 2, textvariable = self.lev, bg = "white",)
        self.level.place(x = 400, y = 620)
        self.entry.append(self.level)
        self.submit = Label(self.window, text="Name/words in name", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 450, y = 595)
        self.labels.append(self.submit) 
        self.names = Entry(self.window, width = 20, textvariable = self.name, bg = "white")
        self.names.place(x = 450, y = 620)
        self.entry.append(self.names)
        
        #3d Selector portion
        self.imagetype = ttk.Combobox(self.window, textvariable = self.varimagetype, values = self.itemTypes, state = "disabled", width = 30)
        self.imagetype.place(x = 1180, y = 500)
        self.labels.append(self.imagetype)
        self.imagename = ttk.Combobox(self.window, textvariable = self.varimagename, state = "disabled", width = 30)
        self.imagename.place(x = 1180, y = 520)
        self.labels.append(self.imagename)
        
        #Detects changes for Image Type
        self.varimagetype.trace("w", self.createImageNames)
        
        #Detects changes for Image Name
        self.varimagename.trace("w", self.create3dImage)
        
        #notes
        self.note = Entry(self.window, textvariable = self.notes, width = 100, bg = "white", state = "readonly")
        self.note.place(x = 150, y = 850)
        self.entry.append(self.note)
        
        #Buttons
        self.submit = Label(self.window, text="Change to View:", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 50, y = 675)
        self.labels.append(self.submit)
        self.item = ttk.Combobox(self.window, textvariable = self.vari, state = "readonly", width = 30)
        self.item.place(x = 240, y = 675)
        self.labels.append(self.item)
        self.deletecheck = Checkbutton(self.window, variable = self.vardel, text = "Yes", width = 10, bg = "white")
        self.deletecheck.place(x = 290, y = 652)
        self.labels.append(self.deletecheck)

        
        #Creator/Modifier/Auditor
        self.create = Label(self.window, text="Creator:", bg = "black", fg = "white", font = "none 12 bold")
        self.create.place(x = 50, y = 760)
        self.labels.append(self.create)        
        self.mod = Label(self.window, text="Last Modifier:", bg = "black", fg = "white", font = "none 12 bold")
        self.mod.place(x = 50, y = 780)
        self.labels.append(self.mod)
        self.audit = Label(self.window, text="Audit Approved:", bg = "black", fg = "white", font = "none 12 bold")
        self.audit.place(x = 50, y = 740)
        self.labels.append(self.audit)
        self.auditor = Entry(self.window, width = 30, textvariable = self.auditstr, bg = "white", state = "readonly")
        self.auditor.place(x = 240, y = 740)
        self.entry.append(self.auditor)
        self.cret = Entry(self.window, width = 30, textvariable = self.createstr, bg = "white", state = "readonly")
        self.cret.place(x = 240, y = 760)
        self.entry.append(self.cret)
        self.modify = Entry(self.window, width = 30, textvariable = self.modifystr, bg = "white", state = "readonly")
        self.modify.place(x = 240, y = 780)
        self.entry.append(self.modify)
        
        #Admin privileges for deleting items from Database
        if self.access == 0:
            self.delete = Label(self.window, text="Look for Delete requests?", bg = "black", fg = "white", font = "none 12 bold")
            self.delete.place(x = 50, y = 652)
            self.labels.append(self.delete)
            self.deletecheck = Checkbutton(self.window, variable = self.vardel, text = "Yes", width = 10, bg = "white")
            self.deletecheck.place(x = 290, y = 652)
            self.labels.append(self.deletecheck)
            delitem = Button(self.window, text = "Delete Item", width = 12, command = self.deleteItem)
            delitem.place(x = 100, y = 700)
            self.labels.append(delitem)
            
            

    def deleteItem(self):
        self.session.query(items).filter(items.Itemid == self.itemid.get()).delete()
        self.session.commit()
        print('To be... deleted.')
        
    def generateItem(self, *args):
        
        itemunit = self.vari.get()
        if itemunit == None or itemunit == '':
            return
        row = self.session.query(items).filter(items.Itemname == itemunit).first()
        
        self.itemstr.set(row.Itemname)
        self.idint.set(row.Itemid)
        self.famint.set(row.Itemfamily)
        
        self.varrare.set(row.Rarity)
        
        #acquires icon/name

        icon = self.session.query(icons, icontype).join(icontype).filter(icons.iconhex == row.Itemicon)
        for a in icon:
            self.varp.set(a[0].iconname)
            self.varitem.set(a[1].icontype)
        
        #Acquires EquipmentTypes
        eqp = self.session.query(equipTypes)[row.Slot]
        eqptype = eqp.equipmenttype
        self.equipstr.set(eqptype)

        #Acquires attack types
        atk = self.session.query(attackTypes)[row.Attacktype]
        atktype = atk.attacktype
        self.atktypestr.set(atktype)
        
        #Checkboxes
        self.tvalue.set(row.Trade)
        self.rvalue.set(row.Rent)
        self.lvalue.set(row.Lore)
        self.cvalue.set(row.Craft)
               
        self.wpnint.set(row.Damage)
        self.levelint.set(row.Level)
        self.maxint.set(row.Maxstack)
        self.itemdescstr.set(row.Itemdesc)
        self.maxhpint.set(row.HPMAX)
        self.durint.set(row.Durability)
        self.createstr.set(row.Creator)
        self.modifystr.set(row.Modifier)
        
        classes = row.Class
        
        if (classes>>classSummary.ALC)&1:
            self.alcint.set(2**classSummary.ALC)
        else:
            self.alcint.set(0)
            
        if (classes>>classSummary.WAR)&1:
            self.warint.set(2**classSummary.WAR)
        else:
            self.warint.set(0)
            
        if (classes>>classSummary.BRD)&1:
            self.brdint.set(2**classSummary.BRD)
        else:
            self.brdint.set(0)
            
        if (classes>>classSummary.CL)&1:
            self.clint.set(2**classSummary.CL)
        else:
            self.clint.set(0)
            
        if (classes>>classSummary.DRD)&1:
            self.drdint.set(2**classSummary.DRD)
        else:
            self.drdint.set(0)
            
        if (classes>>classSummary.ENC)&1:
            self.encint.set(2**classSummary.ENC)
        else:
            self.encint.set(0)
            
        if (classes>>classSummary.MAG)&1:
            self.magint.set(2**classSummary.MAG)
        else:
            self.magint.set(0)
            
        if (classes>>classSummary.MNK)&1:
            self.mnkint.set(2**classSummary.MNK)
        else:
            self.mnkint.set(0)
            
        if (classes>>classSummary.NEC)&1:
            self.necint.set(2**classSummary.NEC)
        else:
            self.necint.set(0)
            
        if (classes>>classSummary.PAL)&1:
            self.palint.set(2**classSummary.PAL)
        else:
            self.palint.set(0)
            
        if (classes>>classSummary.RAN)&1:
            self.ranint.set(2**classSummary.RAN)
        else:
            self.ranint.set(0)
            
        if (classes>>classSummary.RGE)&1:
            self.rgeint.set(2**classSummary.RGE)
        else:
            self.rgeint.set(0)
            
        if (classes>>classSummary.SK)&1:
            self.skint.set(2**classSummary.SK)
        else:
            self.skint.set(0)
            
        if (classes>>classSummary.SHA)&1:
            self.shaint.set(2**classSummary.SHA)
        else:
            self.shaint.set(0)
            
        if (classes>>classSummary.WIZ)&1:
            self.wizint.set(2**classSummary.WIZ)
        else:
            self.wizint.set(0)
        
        races = row.Race
        
        if (races>>raceSummary.HUM)&1:
            self.humint.set(2**raceSummary.HUM)
        else:
            self.humint.set(0)
        
        if (races>>raceSummary.ELF)&1:
            self.elfint.set(2**raceSummary.ELF)
        else:
            self.elfint.set(0)
            
        if (races>>raceSummary.DELF)&1:
            self.delfint.set(2**raceSummary.DELF)
        else:
            self.delfint.set(0)
            
        if (races>>raceSummary.GNO)&1:
            self.gnoint.set(2**raceSummary.GNO)
        else:
            self.gnoint.set(0)
            
        if (races>>raceSummary.DWF)&1:
            self.dwfint.set(2**raceSummary.DWF)
        else:
            self.dwfint.set(0)
            
        if (races>>raceSummary.TRL)&1:
            self.trlint.set(2**raceSummary.TRL)
        else:
            self.trlint.set(0)
            
        if (races>>raceSummary.BAR)&1:
            self.barint.set(2**raceSummary.BAR)
        else:
            self.barint.set(0)
            
        if (races>>raceSummary.HLF)&1:
            self.hlfint.set(2**raceSummary.HLF)
        else:
            self.hlfint.set(0)
            
        if (races>>raceSummary.ERU)&1:
            self.eruint.set(2**raceSummary.ERU)
        else:
            self.eruint.set(0)
            
        if (races>>raceSummary.OGR)&1:
            self.ogrint.set(2**raceSummary.OGR)
        else:
            self.ogrint.set(0)

        self.wepprocint.set(row.Wepproc)
        self.armprocint.set(row.Armproc)
        self.procnamestr.set(row.Procname)
        self.strint.set(row.Strength)
        self.staint.set(row.Stamina)
        self.agiint.set(row.Agility)
        self.dexint.set(row.Dexterity)
        self.wisint.set(row.Wisdom)
        self.intint.set(row.Intelligence)
        self.chaint.set(row.Charisma)
        self.hpmint.set(row.HP_MAX)
        self.powint.set(row.PWR_MAX)
        self.PoTint.set(row.PoT)
        self.HoTint.set(row.HoT)
        self.ACint.set(row.AC)
        self.PRint.set(row.PR)
        self.DRint.set(row.DR)
        self.FRint.set(row.FR)
        self.CRint.set(row.CR)
        self.LRint.set(row.LR)
        self.ARint.set(row.AR)
        
        self.varreg.set(row.region)
        
        self.auditstr.set(row.Verifier)
        
        self.color = row.Color
        
        if eqptype:
                    
            if eqptype == 'ROBE':
                for rows in self.session.query(Robe).filter(row.Robe == Robe.itemhex):
                    self.varimagetype.set('Robe')
                    self.varimagename.set(rows.itemname)

            elif eqptype == 'HELM':
                for rows in self.session.query(Helm).filter(row.Helm == Helm.itemhex):
                    self.varimagetype.set('Helm')                      
                    self.varimagename.set(rows.itemname)

            elif eqptype == 'CHEST' or eqptype == 'LEGS' or eqptype == 'GLOVES' or eqptype == 'FEET' or eqptype == 'BRACERS' or eqptype == 'BRACELETS':
                for rows in self.session.query(Armor).filter(row.Armor == Armor.itemhex):
                    self.varimagetype.set('Armor')
                    self.varimagename.set(rows.itemname)
                    
            else:
                for a in self.session.query(item, itemtype).join(itemtype).filter(item.itemhex == row.Imagegraphic):
                    self.varimagetype.set(a[1].itemtype)
                    self.varimagename.set(a[0].itemname)

                    
        else: 
            self.varimagetype.set('')
            self.varimagename.set('')
            for picture in self.picture:
                picture.destroy()
                    
        self.notes.set(row.Notes)

    def generateItemList(self):
        self.items.clear()    
        if self.auditapprv.get():
            #print('Searching for pre-audited items')
            if (self.equip.get() != 'None') and self.lev.get():
                for row in self.session.query(items).join(equipTypes).filter(equipTypes.equipmenttype == self.equip.get()).filter(items.Level == self.lev.get()).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
                    
            elif self.equip.get() != 'None':
                for row in self.session.query(items).join(equipTypes).filter(equipTypes.equipmenttype == self.equip.get()).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
        
            elif self.lev.get():
                for row in self.session.query(items).filter(items.Level == self.lev.get()).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
        
            elif self.name.get():
                for row in self.session.query(items).filter(items.Itemname.like('%'+self.name.get()+'%')).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
            
            elif self.vardel.get():
                for row in self.session.query(items).filter(items.Notes.like('%'+'delete'+'%')).filter(items.Verifier != None):
                    self.items.append(row.Itemname)                
            else:
                for row in self.session.query(items).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
            
        else:
            #print('Searching for non-audited items')
            if (self.equip.get() != 'None') and self.lev.get():
                for row in self.session.query(items).join(equipTypes).filter(equipTypes.equipmenttype == self.equip.get()).filter(items.Level == self.lev.get()).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
                    
            elif self.equip.get() != 'None':
                for row in self.session.query(items).join(equipTypes).filter(equipTypes.equipmenttype == self.equip.get()).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
        
            elif self.lev.get():
                for row in self.session.query(items).filter(items.Level == self.lev.get()).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
            
            elif self.vardel.get():
                for row in self.session.query(items).filter(items.Notes.like('%'+'delete'+'%')).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
          
            elif self.name.get():
                for row in self.session.query(items).filter(items.Itemname.like('%'+self.name.get()+'%')).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
                    
                
            else:
                for row in self.session.query(items).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
                    
        self.item.configure(values = self.items)

    def change_image(self, *args):
        pic = self.varp.get()
        if pic == None:
            return
        for row in self.session.query(icons).filter(icons.iconname == pic):  
            # Change image of label accordingly
            self.pict = (PhotoImage(data = row.image))

        self.label = Label(self.window ,image = self.pict, bg = "black")
        self.label.place(x = 390, y = 420)
        self.picture.append(self.label)

    def createImageNames(self, *args):
        self.imageNames.clear()
        itemType = self.varimagetype.get()
        if itemType == None or itemType == '':
            return
              
        if itemType == 'Robe' or itemType == 'Armor' or itemType == 'Helm':
            if itemType == 'Robe':
                for row in self.session.query(Robe):
                    self.imageNames.append(row.itemname)
                    
            elif itemType == 'Helm':
                for row in self.session.query(Helm):
                    self.imageNames.append(row.itemname)
                    
            elif itemType == 'Armor':
                for row in self.session.query(Armor):
                    self.imageNames.append(row.itemname)
        else:            
            row = self.session.query(item.itemname).join(itemtype).filter(itemtype.itemtype == itemType)
            for a in row.all():
                self.imageNames.append(a.itemname)
        
        self.imageNames.sort()
        self.imagename.config(values = self.imageNames)

    def create3dImage(self, *args):
        name = self.varimagename.get()
        self.image = None
        if name  == 'None':
            return
            
        if self.varimagetype.get() == 'Robe' or self.varimagetype.get() == 'Armor' or self.varimagetype.get() == 'Helm':
            if self.varimagetype.get() == 'Robe':
                for row in self.session.query(Robe).filter(Robe.itemname == name):
                    self.image = PhotoImage(data = row.image)

            elif self.varimagetype.get() == 'Helm':
                for row in self.session.query(Helm).filter(Helm.itemname == name):
                    self.image = PhotoImage(data = row.image)

            elif self.varimagetype.get() == 'Armor':
                for row in self.session.query(Armor).filter(Armor.itemname == name):                  
                    self.image = PhotoImage(data = row.image)

        else:
            for row in self.session.query(item).filter(item.itemname == name):
                self.image = PhotoImage(data = row.image)
        
        self.label = Label(self.window, image = self.image, bg = "black")
        self.label.place(x = 1030, y = 600)
        self.picture.append(self.label)
        
    def removeEntry(self):
        for entry in self.entry:
            if entry:
                entry.delete(0, END)

        for checkbox in self.checkbox:
            if checkbox:
                checkbox.set(0)

        for combobox in self.combobox:
            if combobox:
                combobox.set('')

        
class editItem:
    
    def __init__(self, window, login):
        self.window       = window
        self.login        = login
        self.session      = None
        self.createItem   = None
        self.viewItem     = None
        self.qcItem       = None
        self.picture      = []
        self.varreg       = StringVar()
        self.varp         = StringVar(self.window)
        self.vari         = StringVar()
        self.varitem      = StringVar()
        self.createstr    = StringVar()
        self.modifystr    = StringVar()
        self.auditstr     = StringVar()
        self.varimagetype = StringVar()
        self.varimagename = StringVar()
        self.notes        = StringVar()
        self.auditstr     = StringVar()
        self.lev          = StringVar()
        self.name         = StringVar()
        self.auditapprv   = IntVar()
        self.equip        = StringVar()
        self.pict         = None
        self.labels       = []
        self.iconNames    = []
        self.itemObject   = []
        self.stats        = []
        self.items        = []
        self.gear         = Gear()       
        self.error        = []
        self.imageNames   = []
    
        #For writing to entry boxes
        self.warint      = IntVar()
        self.brdint      = IntVar()
        self.alcint      = IntVar()
        self.clint       = IntVar()
        self.drdint      = IntVar()
        self.encint      = IntVar()
        self.magint      = IntVar()
        self.mnkint      = IntVar()
        self.necint      = IntVar()
        self.palint      = IntVar()
        self.ranint      = IntVar()
        self.rgeint      = IntVar()
        self.skint       = IntVar()
        self.shaint      = IntVar()
        self.wizint      = IntVar()
        self.allint      = IntVar()
        self.tankint     = IntVar()
        self.healint     = IntVar()
        self.melint      = IntVar()
        self.castint     = IntVar()
        self.humint      = IntVar()
        self.elfint      = IntVar()
        self.delfint     = IntVar()
        self.gnoint      = IntVar()
        self.dwfint      = IntVar()
        self.trlint      = IntVar()
        self.barint      = IntVar()
        self.hlfint      = IntVar()
        self.eruint      = IntVar()
        self.ogrint      = IntVar()
        self.rallint     = IntVar()
        self.itemstr     = StringVar()
        self.idint       = IntVar()
        self.famint      = IntVar()
        self.equipstr    = StringVar()
        self.atktypestr  = StringVar()
        self.wpnint      = IntVar()
        self.levelint    = IntVar()
        self.maxint      = IntVar()
        self.itemdescstr = StringVar()
        self.maxhpint    = IntVar()
        self.durint      = IntVar()
        self.classint    = StringVar()
        self.raceint     = StringVar()
        self.wepprocint  = StringVar()
        self.armprocint  = StringVar()
        self.procnamestr = StringVar()
        self.varrare     = StringVar()
        self.strint      = IntVar()
        self.staint      = IntVar()
        self.agiint      = IntVar()
        self.dexint      = IntVar()
        self.wisint      = IntVar()
        self.intint      = IntVar()
        self.chaint      = IntVar()
        self.hpmint      = IntVar()
        self.powint      = IntVar()
        self.PoTint      = IntVar()
        self.HoTint      = IntVar()
        self.ACint       = IntVar()
        self.PRint       = IntVar()
        self.DRint       = IntVar()
        self.FRint       = IntVar()
        self.CRint       = IntVar()
        self.LRint       = IntVar()
        self.ARint       = IntVar()
        self.tvalue      = IntVar()
        self.rvalue      = IntVar()
        self.lvalue      = IntVar()
        self.cvalue      = IntVar()
        self.entry       = []
        self.checkbox    = [self.warint,
                            self.brdint,
                            self.alcint,
                            self.clint ,
                            self.drdint,
                            self.encint,
                            self.magint,
                            self.mnkint,
                            self.necint,
                            self.palint,
                            self.ranint,
                            self.rgeint,
                            self.skint ,
                            self.shaint,
                            self.wizint,
                            self.allint,
                            self.tankint,
                            self.healint,
                            self.melint,
                            self.castint,
                            self.humint,
                            self.elfint,
                            self.delfint,
                            self.gnoint,
                            self.dwfint,
                            self.trlint,
                            self.barint,
                            self.hlfint,
                            self.eruint,
                            self.ogrint,
                            self.rallint,
                            self.tvalue,
                            self.rvalue,
                            self.lvalue,
                            self.cvalue]
                            
        self.combobox    = [self.varitem,
                            self.varreg,
                            self.varp,
                            self.vari,
                            self.varimagetype,
                            self.varimagename,
                            self.equipstr,
                            self.atktypestr,
                            self.equip,
                            self.varrare]
         
        

    def setSession(self, session, createItem, viewItem, qcItem):
        self.session    = session
        self.createItem = createItem
        self.viewItem   = viewItem
        self.qcItem     = qcItem
    
    def clear(self):
        self.removeEntry()
        if self.entry:
            for entry in self.entry:
                entry.destroy()        
            self.entry.clear()
        if self.labels:
            for i in self.labels:
                if type(i) == list:
                    for a in i:
                        a.destroy()
                else:
                    i.destroy()
        if self.picture:
            for pic in self.picture:
                pic.destroy()
        if self.error:
            self.error.destroy()
        self.gear.destroyError()
        #print('Page de-construction completed')
        
    def displayItems(self):
        self.gear.addSession(self.session, self.window)
        self.createItem.clear()
        self.viewItem.clear()
        self.qcItem.clear()
        
        #Acquires EquipmentTypes
        self.equipTypes = [] 
        for row in self.session.query(equipTypes):
            self.equipTypes.append(row.equipmenttype)

        #Acquires attack types
        self.atkTypes = []
        for row in self.session.query(attackTypes):
            self.atkTypes.append(row.attacktype)
             
        #Acquires Icons
        self.icons = []
        for row in self.session.query(icons):
            self.icons.append(row.image)


        #Acquire IconTypes
        self.iconTypes = []
        for row in self.session.query(icontype):
            self.iconTypes.append(row.icontype)
        self.iconTypes.sort()
        self.iconTypes.insert(0, None)
            
        #Acquire regions
        self.regions = []
        for row in self.session.query(regions):
            self.regions.append(row.region)
        self.regions.sort()
        self.regions.insert(0, None)

        #Acquires ItemTypes
        self.itemTypes = []
        for row in self.session.query(itemtype):
                self.itemTypes.append(row.itemtype)
        self.itemTypes.append('Helm')
        self.itemTypes.append('Armor')
        self.itemTypes.append('Robe')
        self.itemTypes.sort()
        
        #Rarity types
        self.rarity = []
        for row in self.session.query(Rarity):
            self.rarity.append(row.rarity)
        self.rarity.sort()
        
        #Function for majority of Labels
        newLabels = labelFunc(self.window)
        self.labels.append(newLabels)
        
        self.label = Checkbutton(self.window, text="No Trade: ", width = 9, variable = self.tvalue)
        self.label.place(x = 930, y = 320)
        self.labels.append(self.label)
        self.label = Checkbutton(self.window, text="Rent:        ", width = 9, variable = self.rvalue)
        self.label.place(x = 930, y = 340)
        self.labels.append(self.label)
        self.label = Checkbutton(self.window, text="Lore:        ", width = 9, variable = self.lvalue)
        self.label.place(x = 930, y = 360)
        self.labels.append(self.label)
        self.label = Checkbutton(self.window, text="Craft Mat: ", width = 9, variable = self.cvalue)
        self.label.place(x = 930, y = 380)
        self.labels.append(self.label)

        #textboxs
        self.itemname = Entry(self.window, width = 30, textvariable = self.itemstr, bg = "white")
        self.itemname.place(x = 240, y = 320)
        self.entry.append(self.itemname)
        self.itemid = Entry(self.window, width = 30, textvariable = self.idint, bg = "white")
        self.itemid.place(x = 240, y = 340)
        self.entry.append(self.itemid) 
        self.itemfam = Entry(self.window, width = 30, textvariable = self.famint, bg = "white")
        self.itemfam.place(x = 240, y = 360)
        self.entry.append(self.itemfam)
        self.icont = ttk.Combobox(self.window, textvariable = self.varitem, values = self.iconTypes, width = 30, state = "readonly")
        self.icont.place(x = 240, y = 380)
        self.labels.append(self.icont)
        self.icon = ttk.Combobox(self.window, textvariable = self.varp, width = 30, state = "readonly")
        self.icon.place(x = 240, y = 400)
        self.labels.append(self.icon)
         
        #Detects changes for Icon Types
        self.varitem.trace("w", self.createIconNames)
         
        #Changes icon image based on iconname chosen
        self.varp.trace("w", self.change_image)
            
        #Changes editable options based on Equipment type
        self.equipstr.trace("w", self.processEquip)
        
        self.eqpslot = OptionMenu(self.window, self.equipstr, *self.equipTypes)
        self.eqpslot.place(x = 240, y = 420)
        self.labels.append(self.eqpslot)
        self.atktype = OptionMenu(self.window, self.atktypestr, *self.atkTypes)
        self.atktype.place(x = 240, y = 450)
        self.labels.append(self.atktype)
        self.wpndmg = Entry(self.window, width = 10, textvariable = self.wpnint, bg = "white")
        self.wpndmg.place(x = 240, y = 480)
        self.entry.append(self.wpndmg)
        self.lvlreq = Entry(self.window, width = 10, textvariable = self.levelint, bg = "white")
        self.lvlreq.place(x = 240, y = 500)
        self.entry.append(self.lvlreq)
        self.region = ttk.Combobox(self.window, textvariable = self.varreg, values = self.regions, width = 30, state = "readonly")
        self.region.place(x = 240, y = 520)
        self.labels.append(self.region)
        self.rare = ttk.Combobox(self.window, textvariable = self.varrare, values = self.rarity, width = 30, state = "readonly")
        self.rare.place(x = 240, y = 540)
        self.labels.append(self.rare)
        self.maxstack = Entry(self.window, width = 30, textvariable = self.maxint, bg = "white")
        self.maxstack.place(x = 680, y = 320)
        self.entry.append(self.maxstack)
        self.itemdesc = Entry(self.window, width = 30, textvariable = self.itemdescstr, bg = "white")
        self.itemdesc.place(x = 680, y = 340)
        self.entry.append(self.itemdesc)
        self.maxhp = Entry(self.window, width = 30, textvariable = self.maxhpint, bg = "white")
        self.maxhp.place(x = 680, y = 360)
        self.entry.append(self.maxhp)
        self.dur = Entry(self.window, width = 30, textvariable = self.durint, bg = "white")
        self.dur.place(x = 680, y = 380)
        self.entry.append(self.dur)
        self.wepproc = Entry(self.window, width = 30, textvariable = self.wepprocint, bg = "white")
        self.wepproc.place(x = 680, y = 400)
        self.entry.append(self.wepproc)
        self.armproc = Entry(self.window, width = 30, textvariable = self.armprocint, bg = "white")
        self.armproc.place(x = 680, y = 420)
        self.entry.append(self.armproc)
        self.procname = Entry(self.window, width = 30, textvariable = self.procnamestr, bg = "white")
        self.procname.place(x = 680, y = 440)
        self.entry.append(self.procname)
        
        #Classes        
        self.alc = Checkbutton(self.window, variable = self.alcint, text = "ALC", onvalue = 2**classSummary.ALC, width = 3, bg = "white")
        self.alc.place(x = 680, y = 465)
        self.labels.append(self.alc)
        self.brd = Checkbutton(self.window, variable = self.brdint, text = "BRD", onvalue = 2**classSummary.BRD, width = 3, bg = "white")
        self.brd.place(x = 730, y = 465)
        self.labels.append(self.brd)
        self.cl = Checkbutton(self.window, variable = self.clint, text = "CL", onvalue = 2**classSummary.CL, width = 3, bg = "white")
        self.cl.place(x = 780, y = 465)
        self.labels.append(self.cl)
        self.drd = Checkbutton(self.window, variable = self.drdint, text = "DRD", onvalue = 2**classSummary.DRD, width = 3, bg = "white")
        self.drd.place(x = 830, y = 465)
        self.labels.append(self.drd)
        self.enc = Checkbutton(self.window, variable = self.encint,  text = "ENC", onvalue = 2**classSummary.ENC, width = 3, bg = "white")
        self.enc.place(x = 880, y = 465)
        self.labels.append(self.enc)
        self.mag = Checkbutton(self.window, variable = self.magint, text = "MAG", onvalue = 2**classSummary.MAG, width = 3, bg = "white")
        self.mag.place(x = 680, y = 485)
        self.labels.append(self.mag)
        self.mnk = Checkbutton(self.window, variable = self.mnkint, text = "MNK", onvalue = 2**classSummary.MNK, width = 3, bg = "white")
        self.mnk.place(x = 730, y = 485)
        self.labels.append(self.mnk)
        self.nec = Checkbutton(self.window, variable = self.necint, text = "NEC", onvalue = 2**classSummary.NEC, width = 3, bg = "white")
        self.nec.place(x = 780, y = 485)
        self.labels.append(self.nec)
        self.pal = Checkbutton(self.window, variable = self.palint, text = "PAL", onvalue = 2**classSummary.PAL, width = 3, bg = "white")
        self.pal.place(x = 830, y = 485)
        self.labels.append(self.pal)
        self.ran = Checkbutton(self.window, variable = self.ranint, text = "RAN", onvalue = 2**classSummary.RAN, width = 3, bg = "white")
        self.ran.place(x = 880, y = 485)
        self.labels.append(self.ran)
        self.rge = Checkbutton(self.window, variable = self.rgeint, text = "RGE", onvalue = 2**classSummary.RGE, width = 3, bg = "white")
        self.rge.place(x = 650, y = 505)
        self.labels.append(self.rge)
        self.sk = Checkbutton(self.window, variable = self.skint, text = "SK", onvalue = 2**classSummary.SK, width = 3, bg = "white")
        self.sk.place(x = 700, y = 505)
        self.labels.append(self.sk)
        self.sha = Checkbutton(self.window, variable = self.shaint, text = "SHA", onvalue = 2**classSummary.SHA, width = 3, bg = "white")
        self.sha.place(x = 750, y = 505)
        self.labels.append(self.sha) 
        self.war = Checkbutton(self.window, variable = self.warint, text = "WAR", onvalue = 2**classSummary.WAR, width = 3, bg = "white")
        self.war.place(x = 800, y = 505)
        self.labels.append(self.war)
        self.wiz = Checkbutton(self.window, variable = self.wizint, text = "WIZ", onvalue = 2**classSummary.WIZ, width = 3, bg = "white")
        self.wiz.place(x = 850, y = 505)
        self.labels.append(self.wiz)
        self.all = Checkbutton(self.window, variable = self.allint, text = "ALL", onvalue = 32767, width = 3, bg = "white")
        self.all.place(x = 900, y = 505)
        self.labels.append(self.all)
        self.tank = Checkbutton(self.window, variable = self.tankint, text = "TANK", onvalue = 13, width = 6, bg = "white")
        self.tank.place(x = 660, y = 525)
        self.labels.append(self.tank)
        self.heal = Checkbutton(self.window, variable =  self.healint, text = "HEAL", onvalue = 896, width = 6, bg = "white")
        self.heal.place(x = 730, y = 525)
        self.labels.append(self.heal)
        self.mel = Checkbutton(self.window, variable = self.melint, text = "MELEE", onvalue = 114, width = 6, bg = "white")
        self.mel.place(x = 800, y = 525)
        self.labels.append(self.mel) 
        self.cast = Checkbutton(self.window, variable = self.castint, text = "CASTER", onvalue = 31744, width = 6, bg = "white")
        self.cast.place(x = 870, y = 525)
        self.labels.append(self.cast)
        
        #Races
        self.hum = Checkbutton(self.window, variable = self.humint, text = "HUM", onvalue = 2**raceSummary.HUM, width = 3, bg = "white")
        self.hum.place(x = 680, y = 560)
        self.labels.append(self.hum)
        self.elf = Checkbutton(self.window, variable = self.elfint, text = "ELF", onvalue = 2**raceSummary.ELF, width = 3, bg = "white")
        self.elf.place(x = 730, y = 560)
        self.labels.append(self.elf)
        self.delf = Checkbutton(self.window, variable = self.delfint, text = "DELF", onvalue = 2**raceSummary.DELF, width = 3, bg = "white")
        self.delf.place(x = 780, y = 560)
        self.labels.append(self.delf)
        self.gno = Checkbutton(self.window, variable = self.gnoint, text = "GNO", onvalue = 2**raceSummary.GNO, width = 3, bg = "white")
        self.gno.place(x = 830, y = 560)
        self.labels.append(self.gno)
        self.dwf = Checkbutton(self.window, variable = self.dwfint, text = "DWF", onvalue = 2**raceSummary.DWF, width = 3, bg = "white")
        self.dwf.place(x = 880, y = 560)
        self.labels.append(self.dwf)
        self.trl = Checkbutton(self.window, variable = self.trlint, text = "TRL", onvalue = 2**raceSummary.TRL, width = 3, bg = "white")
        self.trl.place(x = 680, y = 580)
        self.labels.append(self.trl)
        self.bar = Checkbutton(self.window, variable = self.barint, text = "BAR", onvalue = 2**raceSummary.BAR, width = 3, bg = "white")
        self.bar.place(x = 730, y = 580)
        self.labels.append(self.bar)
        self.hlf = Checkbutton(self.window, variable = self.hlfint, text = "HLF", onvalue = 2**raceSummary.HLF, width = 3, bg = "white")
        self.hlf.place(x = 780, y = 580)
        self.labels.append(self.hlf)
        self.eru = Checkbutton(self.window, variable = self.eruint, text = "ERU", onvalue = 2**raceSummary.ERU, width = 3, bg = "white")
        self.eru.place(x = 830, y = 580)
        self.labels.append(self.eru)
        self.ogr = Checkbutton(self.window, variable = self.ogrint, text = "OGR", onvalue = 2**raceSummary.OGR, width = 3, bg = "white")
        self.ogr.place(x = 880, y = 580)
        self.labels.append(self.ogr)
        self.rall = Checkbutton(self.window, variable = self.rallint, text = "ALL", onvalue = 1023, width = 3, bg = "white")
        self.rall.place(x = 680, y = 600)
        self.labels.append(self.rall)
        
        #Stats
        self.strength = Entry(self.window, width = 5, textvariable = self.strint, bg = "white")
        self.strength.place(x = 1180, y = 320)
        self.entry.append(self.strength)
        self.sta = Entry(self.window, width = 5, textvariable = self.staint, bg = "white")
        self.sta.place(x = 1180, y = 340)
        self.entry.append(self.sta)
        self.agi = Entry(self.window, width = 5, textvariable = self.agiint, bg = "white")
        self.agi.place(x = 1180, y = 360)
        self.entry.append(self.agi)
        self.dex = Entry(self.window, width = 5, textvariable = self.dexint, bg = "white")
        self.dex.place(x = 1180, y = 380)
        self.entry.append(self.dex)
        self.wis = Entry(self.window, width = 5, textvariable = self.wisint, bg = "white")
        self.wis.place(x = 1180, y = 400)
        self.entry.append(self.wis)
        self.intel = Entry(self.window, width = 5, textvariable = self.intint, bg = "white")
        self.intel.place(x = 1180, y = 420)
        self.entry.append(self.intel)
        self.cha = Entry(self.window, width = 5, textvariable = self.chaint, bg = "white")
        self.cha.place(x = 1180, y = 440)
        self.entry.append(self.cha)
        self.HPM = Entry(self.window, width = 5, textvariable = self.hpmint, bg = "white")
        self.HPM.place(x = 1180, y = 460)
        self.entry.append(self.HPM)
        self.POWM = Entry(self.window, width = 5, textvariable = self.powint, bg = "white")
        self.POWM.place(x = 1180, y = 480)
        self.entry.append(self.POWM)
        self.PoT = Entry(self.window, width = 5, textvariable = self.PoTint, bg = "white")
        self.PoT.place(x = 1380, y = 320)
        self.entry.append(self.PoT)
        self.HoT = Entry(self.window, width = 5, textvariable = self.HoTint, bg = "white")
        self.HoT.place(x = 1380, y = 340)
        self.entry.append(self.HoT)
        self.AC = Entry(self.window, width = 5, textvariable = self.ACint, bg = "white")
        self.AC.place(x = 1380, y = 360)
        self.entry.append(self.AC)
        self.PR = Entry(self.window, width = 5, textvariable = self.PRint,  bg = "white")
        self.PR.place(x = 1380, y = 380)
        self.entry.append(self.PR)
        self.DR = Entry(self.window, width = 5, textvariable = self.DRint,  bg = "white")
        self.DR.place(x = 1380, y = 400)
        self.entry.append(self.DR)
        self.FR = Entry(self.window, width = 5, textvariable = self.FRint, bg = "white")
        self.FR.place(x = 1380, y = 420)
        self.entry.append(self.FR)
        self.CR = Entry(self.window, width = 5, textvariable = self.CRint, bg = "white")
        self.CR.place(x = 1380, y = 440)
        self.entry.append(self.CR)
        self.LR = Entry(self.window, width = 5, textvariable = self.LRint,bg = "white")
        self.LR.place(x = 1380, y = 460)
        self.entry.append(self.LR)
        self.AR = Entry(self.window, width = 5, textvariable = self.ARint, bg = "white",)
        self.AR.place(x = 1380, y = 480)
        self.entry.append(self.AR)

        
        self.vari.trace("w", self.generateItem)

        #Search Parameters
        self.submit = Label(self.window, text="Search Parameters:", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 125, y = 560)
        self.labels.append(self.submit) 
        subm = Button(self.window, text = "SUBMIT", width = 6, command = self.generateItemList)
        subm.place(x = 360, y = 560)
        self.labels.append(subm)
        self.submit = Label(self.window, text="Has item been Audited?", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 50, y = 595)
        self.labels.append(self.submit) 
        self.auditcheck = Checkbutton(self.window, variable = self.auditapprv, text = "Yes", width = 10, bg = "white")
        self.auditcheck.place(x = 50, y = 620)
        self.labels.append(self.auditcheck)
        self.eqpbox = Label(self.window, text="Equip Type", bg = "black", fg = "white", font = "none 12 bold")
        self.eqpbox.place(x = 275, y = 595)
        self.labels.append(self.eqpbox)
        self.eqslot = OptionMenu(self.window, self.equip, *self.equipTypes)
        self.eqslot.place(x = 255, y = 620)
        self.equip.set(None)
        self.labels.append(self.eqslot)
        self.submit = Label(self.window, text="Level", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 390, y = 595)
        self.labels.append(self.submit)
        self.level = Entry(self.window, width = 2, textvariable = self.lev, bg = "white",)
        self.level.place(x = 400, y = 620)
        self.entry.append(self.level)
        self.submit = Label(self.window, text="Name/words in name", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 450, y = 595)
        self.labels.append(self.submit) 
        self.name = Entry(self.window, width = 20, textvariable = self.name, bg = "white")
        self.name.place(x = 450, y = 620)
        self.entry.append(self.name)

        #3d Selector portion
        self.imagetype = ttk.Combobox(self.window, textvariable = self.varimagetype, values = self.itemTypes, state = "readonly", width = 30)
        self.imagetype.place(x = 1180, y = 500)
        self.labels.append(self.imagetype)
        self.imagename = ttk.Combobox(self.window, textvariable = self.varimagename, state = "readonly", width = 30)
        self.imagename.place(x = 1180, y = 520)
        self.labels.append(self.imagename)
        
        #Detects changes for Image Type
        self.varimagetype.trace("w", self.createImageNames)
        
        #Detects changes for Image Name
        self.varimagename.trace("w", self.create3dImage)

        #Buttons
        self.submit = Label(self.window, text="Change to View/Edit:", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 50, y = 655)
        self.labels.append(self.submit)
        self.item = ttk.Combobox(self.window, textvariable = self.vari, state = "readonly", width = 30)
        self.item.place(x = 240, y = 655)
        self.labels.append(self.item)
        
        self.submit = Label(self.window, text="Submit to Update", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 50, y = 680)
        self.labels.append(self.submit)
        sub = Button(self.window, text = "SUBMIT", width = 6, command = self.click)
        sub.place(x = 50, y = 705)
        self.labels.append(sub)
        
        #Color
        self.colors = Button(text = 'Select Color for item', command = self.getColor)
        self.colors.place(x = 1300, y = 100)
        self.labels.append(self.colors)
        
        #notes
        self.note = Entry(self.window, textvariable = self.notes, width = 100, bg = "white")
        self.note.place(x = 150, y = 850)
        self.entry.append(self.note)
        
        #Creator/Modifier/Auditor
        self.create = Label(self.window, text="Creator:", bg = "black", fg = "white", font = "none 12 bold")
        self.create.place(x = 50, y = 760)
        self.labels.append(self.create)        
        self.mod = Label(self.window, text="Last Modifier:", bg = "black", fg = "white", font = "none 12 bold")
        self.mod.place(x = 50, y = 780)
        self.labels.append(self.mod)
        self.audit = Label(self.window, text="Audit Approved:", bg = "black", fg = "white", font = "none 12 bold")
        self.audit.place(x = 50, y = 740)
        self.labels.append(self.audit)
        self.auditor = Entry(self.window, width = 30, textvariable = self.auditstr, bg = "white", state = "readonly")
        self.auditor.place(x = 240, y = 740)
        self.entry.append(self.auditor)
        self.cret = Entry(self.window, width = 30, textvariable = self.createstr, bg = "white", state = "readonly")
        self.cret.place(x = 240, y = 760)
        self.entry.append(self.cret)
        self.modify = Entry(self.window, width = 30, textvariable = self.modifystr, bg = "white", state = "readonly")
        self.modify.place(x = 240, y = 780)
        self.entry.append(self.modify) 
        
    def generateItem(self, *args):
        
        itemunit = self.vari.get()
        if itemunit == None or itemunit == '':
            return
        row = self.session.query(items).filter(items.Itemname == itemunit).first()
        
        self.itemstr.set(row.Itemname)
        self.idint.set(row.Itemid)
        self.famint.set(row.Itemfamily)
        self.varrare.set(row.Rarity)
        #acquires icon/name

        icon = self.session.query(icons, icontype).join(icontype).filter(icons.iconhex == row.Itemicon)
        for a in icon:
            self.varp.set(a[0].iconname)
            self.varitem.set(a[1].icontype)
        
        #Acquires EquipmentTypes
        eqp = self.session.query(equipTypes)[row.Slot]
        eqptype = eqp.equipmenttype
        self.equipstr.set(eqptype)

        #Acquires attack types
        atk = self.session.query(attackTypes)[row.Attacktype]
        atktype = atk.attacktype
        self.atktypestr.set(atktype)
        
        #Checkboxes
        self.tvalue.set(row.Trade)
        self.rvalue.set(row.Rent)
        self.lvalue.set(row.Lore)
        self.cvalue.set(row.Craft)
               
        self.wpnint.set(row.Damage)
        self.levelint.set(row.Level)
        self.maxint.set(row.Maxstack)
        self.itemdescstr.set(row.Itemdesc)
        self.maxhpint.set(row.HPMAX)
        self.durint.set(row.Durability)
        self.createstr.set(row.Creator)
        self.modifystr.set(row.Modifier)
        
        classes = row.Class
        
        if (classes>>classSummary.ALC)&1:
            self.alcint.set(2**classSummary.ALC)
        else:
            self.alcint.set(0)
            
        if (classes>>classSummary.WAR)&1:
            self.warint.set(2**classSummary.WAR)
        else:
            self.warint.set(0)
            
        if (classes>>classSummary.BRD)&1:
            self.brdint.set(2**classSummary.BRD)
        else:
            self.brdint.set(0)
            
        if (classes>>classSummary.CL)&1:
            self.clint.set(2**classSummary.CL)
        else:
            self.clint.set(0)
            
        if (classes>>classSummary.DRD)&1:
            self.drdint.set(2**classSummary.DRD)
        else:
            self.drdint.set(0)
            
        if (classes>>classSummary.ENC)&1:
            self.encint.set(2**classSummary.ENC)
        else:
            self.encint.set(0)
            
        if (classes>>classSummary.MAG)&1:
            self.magint.set(2**classSummary.MAG)
        else:
            self.magint.set(0)
            
        if (classes>>classSummary.MNK)&1:
            self.mnkint.set(2**classSummary.MNK)
        else:
            self.mnkint.set(0)
            
        if (classes>>classSummary.NEC)&1:
            self.necint.set(2**classSummary.NEC)
        else:
            self.necint.set(0)
            
        if (classes>>classSummary.PAL)&1:
            self.palint.set(2**classSummary.PAL)
        else:
            self.palint.set(0)
            
        if (classes>>classSummary.RAN)&1:
            self.ranint.set(2**classSummary.RAN)
        else:
            self.ranint.set(0)
            
        if (classes>>classSummary.RGE)&1:
            self.rgeint.set(2**classSummary.RGE)
        else:
            self.rgeint.set(0)
            
        if (classes>>classSummary.SK)&1:
            self.skint.set(2**classSummary.SK)
        else:
            self.skint.set(0)
            
        if (classes>>classSummary.SHA)&1:
            self.shaint.set(2**classSummary.SHA)
        else:
            self.shaint.set(0)
            
        if (classes>>classSummary.WIZ)&1:
            self.wizint.set(2**classSummary.WIZ)
        else:
            self.wizint.set(0)
        
        races = row.Race
        
        if (races>>raceSummary.HUM)&1:
            self.humint.set(2**raceSummary.HUM)
        else:
            self.humint.set(0)
        
        if (races>>raceSummary.ELF)&1:
            self.elfint.set(2**raceSummary.ELF)
        else:
            self.elfint.set(0)
            
        if (races>>raceSummary.DELF)&1:
            self.delfint.set(2**raceSummary.DELF)
        else:
            self.delfint.set(0)
            
        if (races>>raceSummary.GNO)&1:
            self.gnoint.set(2**raceSummary.GNO)
        else:
            self.gnoint.set(0)
            
        if (races>>raceSummary.DWF)&1:
            self.dwfint.set(2**raceSummary.DWF)
        else:
            self.dwfint.set(0)
            
        if (races>>raceSummary.TRL)&1:
            self.trlint.set(2**raceSummary.TRL)
        else:
            self.trlint.set(0)
            
        if (races>>raceSummary.BAR)&1:
            self.barint.set(2**raceSummary.BAR)
        else:
            self.barint.set(0)
            
        if (races>>raceSummary.HLF)&1:
            self.hlfint.set(2**raceSummary.HLF)
        else:
            self.hlfint.set(0)
            
        if (races>>raceSummary.ERU)&1:
            self.eruint.set(2**raceSummary.ERU)
        else:
            self.eruint.set(0)
            
        if (races>>raceSummary.OGR)&1:
            self.ogrint.set(2**raceSummary.OGR)
        else:
            self.ogrint.set(0)

        self.wepprocint.set(row.Wepproc)
        self.armprocint.set(row.Armproc)
        self.procnamestr.set(row.Procname)
        self.strint.set(row.Strength)
        self.staint.set(row.Stamina)
        self.agiint.set(row.Agility)
        self.dexint.set(row.Dexterity)
        self.wisint.set(row.Wisdom)
        self.intint.set(row.Intelligence)
        self.chaint.set(row.Charisma)
        self.hpmint.set(row.HP_MAX)
        self.powint.set(row.PWR_MAX)
        self.PoTint.set(row.PoT)
        self.HoTint.set(row.HoT)
        self.ACint.set(row.AC)
        self.PRint.set(row.PR)
        self.DRint.set(row.DR)
        self.FRint.set(row.FR)
        self.CRint.set(row.CR)
        self.LRint.set(row.LR)
        self.ARint.set(row.AR)
        
        self.varreg.set(row.region)
        
        self.auditstr.set(row.Verifier)
        
        self.color = row.Color
        
        if eqptype:
            if eqptype == 'PRIMARY' or eqptype == 'SECONDARY' or eqptype == 'HELD':
                for a in self.session.query(item, itemtype).join(itemtype).filter(item.itemhex == row.Imagegraphic):
                    self.varimagename.set(a[0].itemname)
                    self.varimagetype.set(a[1].itemtype)
                    
            elif eqptype == 'ROBE':
                for rows in self.session.query(Robe).filter(row.Robe == Robe.itemhex):
                    self.varimagename.set(rows.itemname)
                    self.varimagetype.set('Robe')

            elif eqptype == 'HELM':
                for rows in self.session.query(Helm).filter(row.Helm == Helm.itemhex):
                    self.varimagename.set(rows.itemname)
                    self.varimagetype.set('Helm')     

            elif eqptype == 'CHEST' or eqptype == 'LEGS' or eqptype == 'GLOVES' or eqptype == 'FEET' or eqptype == 'BRACERS' or eqptype == 'BRACELETS':
                for rows in self.session.query(Armor).filter(row.Armor == Armor.itemhex):
                    self.varimagename.set(rows.itemname)
                    self.varimagetype.set('Armor')
                    
        else: 
            self.varimagetype.set('')
            self.varimagename.set('')
            for picture in self.picture:
                picture.destroy()
                    
        self.notes.set(row.Notes)
            
        
    def click(self):
        
        self.itemObject.clear()

        #clears error incase of new one
        if self.error:
            self.error.destroy()
        
        #Makes sure ItemID is a digit or generates an error    
        if self.itemid.get().isdigit():
            self.itemObject.append(self.itemid.get())
            
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with ItemID", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        #Makes sure item Family is a digit or generates an error       
        if self.itemfam.get().isdigit():
            self.itemObject.append(self.itemfam.get())
            
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with ItemFam", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
           
        #Checks if a return for self.icon exists, if not we generate an error   
        if self.icon.get():
            
            icon = self.icon.get()
            for row in self.session.query(icons).filter(icons.iconname == icon):
                self.itemObject.append(row.iconhex)
            
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="No Icon selected", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        #Checks if a return for equipmenttype exists, if not generate an error 
        if self.equipstr.get():
            self.itemObject.append(self.equipstr.get())

        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text= "No Equipment Type selected", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        self.itemObject.append(self.tvalue.get())
        self.itemObject.append(self.rvalue.get())
        
        #checks is attype is disabled, if not, gets value
        if self.atktype['state'] == 'disabled':
            self.itemObject.append(0)
        
        elif self.atktypestr.get():
            self.itemObject.append(self.atktypestr.get)
        
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text= "Error with Attack Type", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        #self.wpndmg = self.convert(self.wpndmg.get())
        #Weapon damage state check, then makes sure int > 0 
        if self.wpndmg['state'] == 'disabled':
            self.itemObject.append(0)

        elif self.wpndmg.get().isdigit() > 0:
            self.itemObject.append(self.wpndmg)
        
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text= "Error with Weapon Damage", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return

        #Level catch for non-digits, digits < 1 and state check
        if self.lvlreq['state'] == 'disabled':
            self.itemObject.append(0)
            
        elif self.lvlreq.get().isdigit() > 0:
            self.itemObject.append(self.lvlreq.get())
            
        else:
            
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with Level", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
  
        #Maxstack catch for state and digits
        if self.maxstack['state'] == 'disabled':
            self.itemObject.append(0)

        elif self.maxstack.get().isdigit():
            self.itemObject.append(self.maxstack.get())
            
        else:
            
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with max stack", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
            
        #Max HP catch for non-equipment
        if self.maxhp['state'] == 'disabled':
            self.itemObject.append(0)
            
        else:
            if self.maxhp.get().isdigit() > 0:
                self.itemObject.append(self.maxhp.get())
                
            else:
            
                if self.error:
                    self.error.destroy()
                self.error = Label(self.window, text="Error with max HP", bg = "black", fg = "white", font = "none 12 bold")
                self.error.place(x = 700, y = 700)
                return
            
        #Catch for durability
        if self.dur['state'] == 'disabled':
            self.itemObject.append(0)
            
        else:
            if self.dur.get().isdigit() > 0:
                self.itemObject.append(self.dur.get())
                
            else:
            
                if self.error:
                    self.error.destroy()
                self.error = Label(self.window, text="Error with Durability", bg = "black", fg = "white", font = "none 12 bold")
                self.error.place(x = 700, y = 700)
                return

        itemClass = 0
        
        if self.allint.get():
            itemClass += self.allint.get()
        
        else:
            if self.tankint.get():
                itemClass += self.tankint.get()
                
            else:
                itemClass += self.palint.get()
                itemClass += self.warint.get()
                itemClass += self.skint.get()
            
            if self.healint.get():
                itemClass += self.healint.get()
                
            else:
                itemClass += self.clint.get()
                itemClass += self.shaint.get()
                itemClass += self.drdint.get()
                
            if self.melint.get():
                itemClass += self.melint.get()
                
            else:
                itemClass += self.ranint.get()
                itemClass += self.rgeint.get()
                itemClass += self.brdint.get()
                itemClass += self.mnkint.get()
                
            if self.castint.get():
                itemClass += self.castint.get()
                
            else:
                itemClass += self.encint.get()
                itemClass += self.alcint.get()
                itemClass += self.magint.get()
                itemClass += self.necint.get()
                itemClass += self.wizint.get()
        
        self.itemObject.append(itemClass)
         
        itemRace = 0
        
        if self.rallint.get():
            itemRace += self.rallint.get()           
            
        else:
            itemRace += self.humint.get()
            itemRace += self.elfint.get()
            itemRace += self.delfint.get()
            itemRace += self.gnoint.get()
            itemRace += self.dwfint.get()
            itemRace += self.trlint.get()
            itemRace += self.barint.get()
            itemRace += self.hlfint.get()
            itemRace += self.eruint.get()
            itemRace += self.ogrint.get()
            
        self.itemObject.append(itemRace)
          

        self.itemObject.append(self.wepproc.get())
        self.itemObject.append(self.armproc.get())
        self.itemObject.append(self.procname.get())
        self.itemObject.append(self.lvalue.get())
        self.itemObject.append(self.cvalue.get())
        self.itemObject.append(self.itemname.get())
        self.itemObject.append(self.itemdesc.get())
        
        totStats = [self.strength,
                    self.sta,
                    self.agi,
                    self.dex,
                    self.wis,
                    self.intel,
                    self.cha,
                    self.HPM,
                    self.POWM,
                    self.PoT,
                    self.HoT,
                    self.AC,
                    self.PR,
                    self.DR,
                    self.FR,
                    self.CR,
                    self.LR,
                    self.AR]
                    
        #Catch for Stats
        for stat in totStats:
            if stat['state'] == 'disabled':
                self.stats.append(0)
            
            else:
                if stat.get().isdigit() >= 0:
                    self.stats.append(stat.get())
                
                else:
            
                    if self.error:
                        self.error.destroy()
                    self.error = Label(self.window, text="Error with Stats", bg = "black", fg = "white", font = "none 12 bold")
                    self.error.place(x = 700, y = 700)
                    return
        self.itemObject.append(self.varreg.get())
        
        eqptype = self.equipstr.get()
        if self.varimagename.get():
            if eqptype == 'HELM' or eqptype == 'GLOVES' or eqptype == 'LEGS' or eqptype == 'CHEST' or eqptype == 'FEET' or eqptype == 'ROBE' or eqptype == 'BRACELET' or eqptype == 'BRACERS':
                if eqptype == 'GLOVES' or eqptype == 'LEGS' or eqptype == 'CHEST' or eqptype == 'FEET' or eqptype == 'BRACELET' or eqptype == 'BRACERS':
                    for row in self.session.query(Armor).filter(Armor.itemname == self.varimagename.get()):
                        items = ['Armor', row.itemhex]
                        self.itemObject.append(items)
                                 
                elif eqptype == 'ROBE':
                    for row in self.session.query(Robe).filter(Robe.itemname == self.varimagename.get()):
                        items = ['Robe', row.itemhex]
                        self.itemObject.append(items)
                        
                elif eqptype == 'HELM':                    
                    for row in self.session.query(Helm).filter(Helm.itemname == self.varimagename.get()):
                        items = ['Helm', row.itemhex]
                        self.itemObject.append(items)
                        
            else:
                for row in self.session.query(item).filter(item.itemname == self.varimagename.get()):
                    items = ['Other', row.itemhex]
                    self.itemObject.append(items)
        else:
            items = [None, None]    
            self.itemObject.append(items)
        
        if self.color != None or self.color != 0:
            print('Received color {}'.format(self.color))
            self.itemObject.append(self.color)
                
        else:
            self.itemObject.append(0)
            
        self.itemObject.append(self.varrare.get())
                 
        self.itemObject.append(self.login)
        self.itemObject.append(self.note.get())
        self.gear.addItem(self.itemObject, self.stats, self)
        self.gear.processSubmit()
        self.gear.updateGear()
        
    
    def createArmor(self):
        self.wepproc.config(state = 'disabled')
        self.armproc.config(state = 'normal')
        self.maxstack.config(state = 'disabled')
        self.atktype.config(state = 'disabled')
        self.wpndmg.config(state = 'disabled')
        self.strength.config(state = 'normal')
        self.sta.config(state = 'normal')
        self.agi.config(state = 'normal')
        self.dex.config(state = 'normal')
        self.wis.config(state = 'normal')
        self.intel.config(state = 'normal')
        self.cha.config(state = 'normal')
        self.HPM.config(state = 'normal')
        self.POWM.config(state = 'normal')
        self.PoT.config(state = 'normal')
        self.HoT.config(state = 'normal')
        self.AC.config(state = 'normal')
        self.PR.config(state = 'normal')
        self.DR.config(state = 'normal')
        self.FR.config(state = 'normal')
        self.CR.config(state = 'normal')
        self.LR.config(state = 'normal')
        self.AR.config(state = 'normal')
    
    def createWeapon(self):
        self.armproc.config(state = 'disabled')
        self.wepproc.config(state = 'normal')
        self.strength.config(state = 'normal')
        self.sta.config(state = 'normal')
        self.agi.config(state = 'normal')
        self.dex.config(state = 'normal')
        self.wis.config(state = 'normal')
        self.intel.config(state = 'normal')
        self.cha.config(state = 'normal')
        self.HPM.config(state = 'normal')
        self.POWM.config(state = 'normal')
        self.PoT.config(state = 'normal')
        self.HoT.config(state = 'normal')
        self.AC.config(state = 'normal')
        self.PR.config(state = 'normal')
        self.DR.config(state = 'normal')
        self.FR.config(state = 'normal')
        self.CR.config(state = 'normal')
        self.LR.config(state = 'normal')
        self.AR.config(state = 'normal')
        self.atktype.config(state = 'normal')
        self.wpndmg.config(state = 'normal')
        self.maxhp.config(state = 'normal')
        self.dur.config(state = 'normal') 
        self.maxstack.config(state = 'disabled')
    
    def createItems(self):
        self.strength.config(state = 'disabled')
        self.sta.config(state = 'disabled')
        self.agi.config(state = 'disabled')
        self.dex.config(state = 'disabled')
        self.wis.config(state = 'disabled')
        self.intel.config(state = 'disabled')
        self.cha.config(state = 'disabled')
        self.HPM.config(state = 'disabled')
        self.POWM.config(state = 'disabled')
        self.PoT.config(state = 'disabled')
        self.HoT.config(state = 'disabled')
        self.AC.config(state = 'disabled')
        self.PR.config(state = 'disabled')
        self.DR.config(state = 'disabled')
        self.FR.config(state = 'disabled')
        self.CR.config(state = 'disabled')
        self.LR.config(state = 'disabled')
        self.AR.config(state = 'disabled')
        self.atktype.config(state = 'disabled')
        self.wpndmg.config(state = 'disabled')
        self.maxhp.config(state = 'disabled')
        self.dur.config(state = 'disabled')
        self.maxstack.config(state = 'normal')
           
    def processEquip(self, *args):

        [self.createArmor() for x in equipEnum.armor if x == self.equipstr.get()]
        [self.createWeapon() for x in equipEnum.weapon if x == self.equipstr.get()]
        [self.createItems() for x in equipEnum.items if x == self.equipstr.get()]
        
        choice = self.equipstr.get()
        if choice:
            if choice == 'ROBE':
                self.imagename.config(values = 'Robe')
                self.varimagetype.set('Robe')
 
            elif choice == 'CHEST' or choice == 'GLOVES' or choice == 'FEET' or choice == 'BRACERS' or choice == 'BRACELET' or choice == 'LEGS':
                self.imagename.config(values = 'Armor')
                self.varimagetype.set('Armor')
            
            elif choice == 'HELM':
                self.imagename.config(values = 'Helm')
                self.varimagetype.set('Helm')
     
    def change_image(self, *args):
        pic = self.varp.get()
        if pic == None:
            return
        for row in self.session.query(icons).filter(icons.iconname == pic):  
            # Change image of label accordingly
            self.pict = (PhotoImage(data = row.image))

        self.label = Label(self.window ,image = self.pict, bg = "black")
        self.label.place(x = 390, y = 420)
        self.picture.append(self.label)
    
    def createIconNames(self, *args):
        self.iconNames.clear()
        iconType = self.varitem.get()
        if iconType == None or iconType == '':
            return
   
        row = self.session.query(icons.iconname).join(icontype).filter(icontype.icontype == iconType)
        for a in row.all():
            self.iconNames.append(a.iconname)
            
        self.iconNames.sort()
        self.iconNames.insert(0, None)
        self.icon.config(values = self.iconNames)

    def getColor(self):
        thisColor = None
        if self.color == None:
            pass
        else:
            print('Pre-defined color')
            thisColor = '0x' + hex(self.color)[2:].zfill(8)
            thisColor = thisColor.replace('0x', '#')[:-2]
            
        color = colorchooser.askcolor(color = thisColor, title = "Select color for this item, if any")
        if color == (None, None):
            return
        color = (color[1].strip('#'))+'FF'
        self.color = int(color, 16)
        
    def createImageNames(self, *args):
        self.imageNames.clear()
        itemType = self.varimagetype.get()
        if itemType == None or itemType == '':
            return
            
        if itemType == 'Robe' or itemType == 'Armor' or itemType == 'Helm':
            if itemType == 'Robe':
                for row in self.session.query(Robe):
                    self.imageNames.append(row.itemname)
                    
            elif itemType == 'Helm':
                for row in self.session.query(Helm):
                    self.imageNames.append(row.itemname)
                    
            elif itemType == 'Armor':
                for row in self.session.query(Armor):
                    self.imageNames.append(row.itemname)
        else:            
            row = self.session.query(item.itemname).join(itemtype).filter(itemtype.itemtype == itemType)
            for a in row.all():
                self.imageNames.append(a.itemname)
        
        self.imageNames.sort()
        self.imagename.config(values = self.imageNames)

    def create3dImage(self, *args):
        name = self.varimagename.get()
        self.image = None
        if name  == 'None':
            return
            
        if self.varimagetype.get() == 'Robe' or self.varimagetype.get() == 'Armor' or self.varimagetype.get() == 'Helm':
            if self.varimagetype.get() == 'Robe':
                for row in self.session.query(Robe).filter(Robe.itemname == name):
                    self.image = PhotoImage(data = row.image)

            elif self.varimagetype.get() == 'Helm':
                for row in self.session.query(Helm).filter(Helm.itemname == name):
                    self.image = PhotoImage(data = row.image)

            elif self.varimagetype.get() == 'Armor':
                for row in self.session.query(Armor).filter(Armor.itemname == name):                  
                    self.image = PhotoImage(data = row.image)

        else:
            for row in self.session.query(item).filter(item.itemname == name):
                self.image = PhotoImage(data = row.image)
        
        self.label = Label(self.window, image = self.image, bg = "black")
        self.label.place(x = 1030, y = 600)
        self.picture.append(self.label)
        
    def generateItemList(self):
        self.items.clear()    
        if self.auditapprv.get():
            #print('Searching for pre-audited items')
            if (self.equip.get() != 'None') and self.lev.get():
                for row in self.session.query(items).join(equipTypes).filter(equipTypes.equipmenttype == self.equip.get()).filter(items.Level == self.lev.get()).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
                    
            elif self.equip.get() != 'None':
                for row in self.session.query(items).join(equipTypes).filter(equipTypes.equipmenttype == self.equip.get()).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
        
            elif self.lev.get():
                for row in self.session.query(items).filter(items.Level == self.lev.get()).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
        
            elif self.name.get():
                for row in self.session.query(items).filter(items.Itemname.like('%'+self.name.get()+'%')).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
                
            else:
                for row in self.session.query(items).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
            
        else:
            #print('Searching for non-audited items')
            if (self.equip.get() != 'None') and self.lev.get():
                for row in self.session.query(items).join(equipTypes).filter(equipTypes.equipmenttype == self.equip.get()).filter(items.Level == self.lev.get()).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
                    
            elif self.equip.get() != 'None':
                for row in self.session.query(items).join(equipTypes).filter(equipTypes.equipmenttype == self.equip.get()).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
        
            elif self.lev.get():
                for row in self.session.query(items).filter(items.Level == self.lev.get()).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
        
            elif self.name.get():
                for row in self.session.query(items).filter(items.Itemname.like('%'+self.name.get()+'%')).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
                    
                
            else:
                for row in self.session.query(items).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
                    
        self.item.configure(values = self.items)
        
    def removeEntry(self):
        for entry in self.entry:
            if entry:
                entry.delete(0, END)
        
        for checkbox in self.checkbox:
            if checkbox:
                checkbox.set(0)
       
        for combobox in self.combobox:
            if combobox:
                combobox.set('')

class qcItem:
     
    def __init__(self, window, login):
        self.window       = window
        self.login        = login
        self.session      = None
        self.createItem   = None
        self.viewItem     = None
        self.editItem     = None
        self.image        = None
        self.picture      = []
        self.varreg       = StringVar()
        self.varp         = StringVar(self.window)
        self.vari         = StringVar()
        self.createstr    = StringVar()
        self.modifystr    = StringVar()
        self.varitem      = StringVar()
        self.auditstr     = StringVar()
        self.lev          = StringVar()
        self.name         = StringVar()
        self.varimagetype = StringVar()
        self.varimagename = StringVar()
        self.notes        = StringVar()
        self.varrare      = StringVar()
        self.auditapprv   = IntVar()
        self.equip        = StringVar()
        self.labels       = []
        self.imageNames   = []
        self.iconNames    = []
        self.itemObject   = []
        self.items        = []
        self.stats        = []
        self.gear         = Gear()       
        self.error        = []
    
        #For writing to entry boxes
        self.warint      = IntVar()
        self.brdint      = IntVar()
        self.alcint      = IntVar()
        self.clint       = IntVar()
        self.drdint      = IntVar()
        self.encint      = IntVar()
        self.magint      = IntVar()
        self.mnkint      = IntVar()
        self.necint      = IntVar()
        self.palint      = IntVar()
        self.ranint      = IntVar()
        self.rgeint      = IntVar()
        self.skint       = IntVar()
        self.shaint      = IntVar()
        self.wizint      = IntVar()
        self.allint      = IntVar()
        self.tankint     = IntVar()
        self.healint     = IntVar()
        self.melint      = IntVar()
        self.castint     = IntVar()
        self.humint      = IntVar()
        self.elfint      = IntVar()
        self.delfint     = IntVar()
        self.gnoint      = IntVar()
        self.dwfint      = IntVar()
        self.trlint      = IntVar()
        self.barint      = IntVar()
        self.hlfint      = IntVar()
        self.eruint      = IntVar()
        self.ogrint      = IntVar()
        self.rallint     = IntVar()
        self.itemstr     = StringVar()
        self.idint       = IntVar()
        self.famint      = IntVar()
        self.equipstr    = StringVar()
        self.atktypestr  = StringVar()
        self.wpnint      = IntVar()
        self.levelint    = IntVar()
        self.maxint      = IntVar()
        self.itemdescstr = StringVar()
        self.maxhpint    = IntVar()
        self.durint      = IntVar()
        self.classint    = StringVar()
        self.raceint     = StringVar()
        self.wepprocint  = StringVar()
        self.armprocint  = StringVar()
        self.procnamestr = StringVar()
        self.rarity      = StringVar()
        self.strint      = IntVar()
        self.staint      = IntVar()
        self.agiint      = IntVar()
        self.dexint      = IntVar()
        self.wisint      = IntVar()
        self.intint      = IntVar()
        self.chaint      = IntVar()
        self.hpmint      = IntVar()
        self.powint      = IntVar()
        self.PoTint      = IntVar()
        self.HoTint      = IntVar()
        self.ACint       = IntVar()
        self.PRint       = IntVar()
        self.DRint       = IntVar()
        self.FRint       = IntVar()
        self.CRint       = IntVar()
        self.LRint       = IntVar()
        self.ARint       = IntVar()
        self.tvalue      = IntVar()
        self.rvalue      = IntVar()
        self.lvalue      = IntVar()
        self.cvalue      = IntVar()
        self.checkint    = IntVar()
        self.entry       = []
        self.checkbox    = [self.warint,
                            self.brdint,
                            self.alcint,
                            self.clint ,
                            self.drdint,
                            self.encint,
                            self.magint,
                            self.mnkint,
                            self.necint,
                            self.palint,
                            self.ranint,
                            self.rgeint,
                            self.skint ,
                            self.shaint,
                            self.wizint,
                            self.allint,
                            self.tankint,
                            self.healint,
                            self.melint,
                            self.castint,
                            self.humint,
                            self.elfint,
                            self.delfint,
                            self.gnoint,
                            self.dwfint,
                            self.trlint,
                            self.barint,
                            self.hlfint,
                            self.eruint,
                            self.ogrint,
                            self.rallint,
                            self.tvalue,
                            self.rvalue,
                            self.lvalue,
                            self.cvalue,
                            self.auditapprv, 
                            self.checkint]
                            
        self.combobox    = [self.varitem,
                            self.varreg,
                            self.varp,
                            self.vari,
                            self.varimagetype,
                            self.varimagename,
                            self.equipstr,
                            self.atktypestr,
                            self.equip,
                            self.varrare]
    
    def setSession(self, session, viewItem, editItem, createItem):
        self.session    = session
        self.createItem = createItem
        self.viewItem   = viewItem
        self.editItem   = editItem
    
    def clear(self):
        self.removeEntry()
        if self.entry:
            for entry in self.entry:
                entry.destroy()        
            self.entry.clear()
        if self.labels:
            for i in self.labels:
                if type(i) == list:
                    for a in i:
                        a.destroy()
                else:
                    i.destroy()
        if self.picture:
            for pic in self.picture:
                pic.destroy()
        if self.error:
            self.error.destroy()
        self.gear.destroyError()
        #print('Page de-construction completed')
        
    def displayItems(self):
        self.gear.addSession(self.session, self.window)
        self.createItem.clear()
        self.viewItem.clear()
        self.editItem.clear()
        
        #Acquires EquipmentTypes
        self.equipTypes = [None] 
        for row in self.session.query(equipTypes):
            self.equipTypes.append(row.equipmenttype)

        #Acquires attack types
        self.atkTypes = []
        for row in self.session.query(attackTypes):
            self.atkTypes.append(row.attacktype)
             
        #Acquires Icons
        self.icons = []
        for row in self.session.query(icons):
            self.icons.append(row.image)


        #Acquire IconTypes
        self.iconTypes = []
        for row in self.session.query(icontype):
            self.iconTypes.append(row.icontype)
        self.iconTypes.sort()
        self.iconTypes.insert(0, None)
            
        #Acquire regions
        self.regions = []
        for row in self.session.query(regions):
            self.regions.append(row.region)
        self.regions.sort()
        self.regions.insert(0, None)
            
        #Acquires ItemTypes
        self.itemTypes = []
        for row in self.session.query(itemtype):
                self.itemTypes.append(row.itemtype)
        self.itemTypes.append('Helm')
        self.itemTypes.append('Armor')
        self.itemTypes.append('Robe')
        self.itemTypes.sort()
        
        #Rarity types
        self.rarity = []
        for row in self.session.query(Rarity):
            self.rarity.append(row.rarity)
        self.rarity.sort()
            
        #Function for majority of Labels
        newLabels = labelFunc(self.window)
        self.labels.append(newLabels)
        
        self.label = Checkbutton(self.window, text="No Trade: ", width = 9, variable = self.tvalue)
        self.label.place(x = 930, y = 320)
        self.labels.append(self.label)
        self.label = Checkbutton(self.window, text="Rent:        ", width = 9, variable = self.rvalue)
        self.label.place(x = 930, y = 340)
        self.labels.append(self.label)
        self.label = Checkbutton(self.window, text="Lore:        ", width = 9, variable = self.lvalue)
        self.label.place(x = 930, y = 360)
        self.labels.append(self.label)
        self.label = Checkbutton(self.window, text="Craft Mat: ", width = 9, variable = self.cvalue)
        self.label.place(x = 930, y = 380)
        self.labels.append(self.label)

        #textboxs
        self.itemname = Entry(self.window, width = 30, textvariable = self.itemstr, bg = "white")
        self.itemname.place(x = 240, y = 320)
        self.entry.append(self.itemname)
        self.itemid = Entry(self.window, width = 30, textvariable = self.idint, bg = "white")
        self.itemid.place(x = 240, y = 340)
        self.entry.append(self.itemid) 
        self.itemfam = Entry(self.window, width = 30, textvariable = self.famint, bg = "white")
        self.itemfam.place(x = 240, y = 360)
        self.entry.append(self.itemfam)
        self.icont = ttk.Combobox(self.window, textvariable = self.varitem, values = self.iconTypes, width = 30, state = "readonly")
        self.icont.place(x = 240, y = 380)
        self.labels.append(self.icont)
        self.icon = ttk.Combobox(self.window, textvariable = self.varp, values = self.iconNames, width = 30, state = "readonly")
        self.icon.place(x = 240, y = 400)
        self.labels.append(self.icon)
         
        #Detects changes for Icon Types
        self.varitem.trace("w", self.createIconNames)
         
        #Changes icon image based on iconname chosen
        self.varp.trace("w", self.change_image)
        
        #Changes editable options based on Equipment type
        self.equipstr.trace("w", self.processEquip)
        
        self.eqpslot = OptionMenu(self.window, self.equipstr, *self.equipTypes)
        self.eqpslot.place(x = 240, y = 420)
        self.labels.append(self.eqpslot)
        self.atktype = OptionMenu(self.window, self.atktypestr, *self.atkTypes)
        self.atktype.place(x = 240, y = 450)
        self.labels.append(self.atktype)
        self.wpndmg = Entry(self.window, width = 10, textvariable = self.wpnint, bg = "white")
        self.wpndmg.place(x = 240, y = 480)
        self.entry.append(self.wpndmg)
        self.lvlreq = Entry(self.window, width = 10, textvariable = self.levelint, bg = "white")
        self.lvlreq.place(x = 240, y = 500)
        self.entry.append(self.lvlreq)
        self.region = ttk.Combobox(self.window, textvariable = self.varreg, values = self.regions, width = 30, state = "readonly")
        self.region.place(x = 240, y = 520)
        self.labels.append(self.region)
        self.rare = ttk.Combobox(self.window, textvariable = self.varrare, values = self.rarity, width = 30, state = "readonly")
        self.rare.place(x = 240, y = 540)
        self.labels.append(self.rare)
        self.maxstack = Entry(self.window, width = 30, textvariable = self.maxint, bg = "white")
        self.maxstack.place(x = 680, y = 320)
        self.entry.append(self.maxstack)
        self.itemdesc = Entry(self.window, width = 30, textvariable = self.itemdescstr, bg = "white")
        self.itemdesc.place(x = 680, y = 340)
        self.entry.append(self.itemdesc)
        self.maxhp = Entry(self.window, width = 30, textvariable = self.maxhpint, bg = "white")
        self.maxhp.place(x = 680, y = 360)
        self.entry.append(self.maxhp)
        self.dur = Entry(self.window, width = 30, textvariable = self.durint, bg = "white")
        self.dur.place(x = 680, y = 380)
        self.entry.append(self.dur)
        self.wepproc = Entry(self.window, width = 30, textvariable = self.wepprocint, bg = "white")
        self.wepproc.place(x = 680, y = 400)
        self.entry.append(self.wepproc)
        self.armproc = Entry(self.window, width = 30, textvariable = self.armprocint, bg = "white")
        self.armproc.place(x = 680, y = 420)
        self.entry.append(self.armproc)
        self.procname = Entry(self.window, width = 30, textvariable = self.procnamestr, bg = "white")
        self.procname.place(x = 680, y = 440)
        self.entry.append(self.procname)
                
        
        
        #Classes
        self.alc = Checkbutton(self.window, variable = self.alcint, text = "ALC", onvalue = 2**classSummary.ALC, width = 3, bg = "white")
        self.alc.place(x = 680, y = 465)
        self.labels.append(self.alc)
        self.brd = Checkbutton(self.window, variable = self.brdint, text = "BRD", onvalue = 2**classSummary.BRD, width = 3, bg = "white")
        self.brd.place(x = 730, y = 465)
        self.labels.append(self.brd)
        self.cl = Checkbutton(self.window, variable = self.clint, text = "CL", onvalue = 2**classSummary.CL, width = 3, bg = "white")
        self.cl.place(x = 780, y = 465)
        self.labels.append(self.cl)
        self.drd = Checkbutton(self.window, variable = self.drdint, text = "DRD", onvalue = 2**classSummary.DRD, width = 3, bg = "white")
        self.drd.place(x = 830, y = 465)
        self.labels.append(self.drd)
        self.enc = Checkbutton(self.window, variable = self.encint,  text = "ENC", onvalue = 2**classSummary.ENC, width = 3, bg = "white")
        self.enc.place(x = 880, y = 465)
        self.labels.append(self.enc)
        self.mag = Checkbutton(self.window, variable = self.magint, text = "MAG", onvalue = 2**classSummary.MAG, width = 3, bg = "white")
        self.mag.place(x = 680, y = 485)
        self.labels.append(self.mag)
        self.mnk = Checkbutton(self.window, variable = self.mnkint, text = "MNK", onvalue = 2**classSummary.MNK, width = 3, bg = "white")
        self.mnk.place(x = 730, y = 485)
        self.labels.append(self.mnk)
        self.nec = Checkbutton(self.window, variable = self.necint, text = "NEC", onvalue = 2**classSummary.NEC, width = 3, bg = "white")
        self.nec.place(x = 780, y = 485)
        self.labels.append(self.nec)
        self.pal = Checkbutton(self.window, variable = self.palint, text = "PAL", onvalue = 2**classSummary.PAL, width = 3, bg = "white")
        self.pal.place(x = 830, y = 485)
        self.labels.append(self.pal)
        self.ran = Checkbutton(self.window, variable = self.ranint, text = "RAN", onvalue = 2**classSummary.RAN, width = 3, bg = "white")
        self.ran.place(x = 880, y = 485)
        self.labels.append(self.ran)
        self.rge = Checkbutton(self.window, variable = self.rgeint, text = "RGE", onvalue = 2**classSummary.RGE, width = 3, bg = "white")
        self.rge.place(x = 650, y = 505)
        self.labels.append(self.rge)
        self.sk = Checkbutton(self.window, variable = self.skint, text = "SK", onvalue = 2**classSummary.SK, width = 3, bg = "white")
        self.sk.place(x = 700, y = 505)
        self.labels.append(self.sk)
        self.sha = Checkbutton(self.window, variable = self.shaint, text = "SHA", onvalue = 2**classSummary.SHA, width = 3, bg = "white")
        self.sha.place(x = 750, y = 505)
        self.labels.append(self.sha) 
        self.war = Checkbutton(self.window, variable = self.warint, text = "WAR", onvalue = 2**classSummary.WAR, width = 3, bg = "white")
        self.war.place(x = 800, y = 505)
        self.labels.append(self.war)
        self.wiz = Checkbutton(self.window, variable = self.wizint, text = "WIZ", onvalue = 2**classSummary.WIZ, width = 3, bg = "white")
        self.wiz.place(x = 850, y = 505)
        self.labels.append(self.wiz)
        self.all = Checkbutton(self.window, variable = self.allint, text = "ALL", onvalue = 32767, width = 3, bg = "white")
        self.all.place(x = 900, y = 505)
        self.labels.append(self.all)
        self.tank = Checkbutton(self.window, variable = self.tankint, text = "TANK", onvalue = 13, width = 6, bg = "white")
        self.tank.place(x = 660, y = 525)
        self.labels.append(self.tank)
        self.heal = Checkbutton(self.window, variable =  self.healint, text = "HEAL", onvalue = 896, width = 6, bg = "white")
        self.heal.place(x = 730, y = 525)
        self.labels.append(self.heal)
        self.mel = Checkbutton(self.window, variable = self.melint, text = "MELEE", onvalue = 114, width = 6, bg = "white")
        self.mel.place(x = 800, y = 525)
        self.labels.append(self.mel) 
        self.cast = Checkbutton(self.window, variable = self.castint, text = "CASTER", onvalue = 31744, width = 6, bg = "white")
        self.cast.place(x = 870, y = 525)
        self.labels.append(self.cast)
        
        #Races
        self.hum = Checkbutton(self.window, variable = self.humint, text = "HUM", onvalue = 2**raceSummary.HUM, width = 3, bg = "white")
        self.hum.place(x = 680, y = 560)
        self.labels.append(self.hum)
        self.elf = Checkbutton(self.window, variable = self.elfint, text = "ELF", onvalue = 2**raceSummary.ELF, width = 3, bg = "white")
        self.elf.place(x = 730, y = 560)
        self.labels.append(self.elf)
        self.delf = Checkbutton(self.window, variable = self.delfint, text = "DELF", onvalue = 2**raceSummary.DELF, width = 3, bg = "white")
        self.delf.place(x = 780, y = 560)
        self.labels.append(self.delf)
        self.gno = Checkbutton(self.window, variable = self.gnoint, text = "GNO", onvalue = 2**raceSummary.GNO, width = 3, bg = "white")
        self.gno.place(x = 830, y = 560)
        self.labels.append(self.gno)
        self.dwf = Checkbutton(self.window, variable = self.dwfint, text = "DWF", onvalue = 2**raceSummary.DWF, width = 3, bg = "white")
        self.dwf.place(x = 880, y = 560)
        self.labels.append(self.dwf)
        self.trl = Checkbutton(self.window, variable = self.trlint, text = "TRL", onvalue = 2**raceSummary.TRL, width = 3, bg = "white")
        self.trl.place(x = 680, y = 580)
        self.labels.append(self.trl)
        self.bar = Checkbutton(self.window, variable = self.barint, text = "BAR", onvalue = 2**raceSummary.BAR, width = 3, bg = "white")
        self.bar.place(x = 730, y = 580)
        self.labels.append(self.bar)
        self.hlf = Checkbutton(self.window, variable = self.hlfint, text = "HLF", onvalue = 2**raceSummary.HLF, width = 3, bg = "white")
        self.hlf.place(x = 780, y = 580)
        self.labels.append(self.hlf)
        self.eru = Checkbutton(self.window, variable = self.eruint, text = "ERU", onvalue = 2**raceSummary.ERU, width = 3, bg = "white")
        self.eru.place(x = 830, y = 580)
        self.labels.append(self.eru)
        self.ogr = Checkbutton(self.window, variable = self.ogrint, text = "OGR", onvalue = 2**raceSummary.OGR, width = 3, bg = "white")
        self.ogr.place(x = 880, y = 580)
        self.labels.append(self.ogr)
        self.rall = Checkbutton(self.window, variable = self.rallint, text = "ALL", onvalue = 1023, width = 3, bg = "white")
        self.rall.place(x = 680, y = 600)
        self.labels.append(self.rall)
        
        #stats
        self.strength = Entry(self.window, width = 5, textvariable = self.strint, bg = "white")
        self.strength.place(x = 1180, y = 320)
        self.entry.append(self.strength)
        self.sta = Entry(self.window, width = 5, textvariable = self.staint, bg = "white")
        self.sta.place(x = 1180, y = 340)
        self.entry.append(self.sta)
        self.agi = Entry(self.window, width = 5, textvariable = self.agiint, bg = "white")
        self.agi.place(x = 1180, y = 360)
        self.entry.append(self.agi)
        self.dex = Entry(self.window, width = 5, textvariable = self.dexint, bg = "white")
        self.dex.place(x = 1180, y = 380)
        self.entry.append(self.dex)
        self.wis = Entry(self.window, width = 5, textvariable = self.wisint, bg = "white")
        self.wis.place(x = 1180, y = 400)
        self.entry.append(self.wis)
        self.intel = Entry(self.window, width = 5, textvariable = self.intint, bg = "white")
        self.intel.place(x = 1180, y = 420)
        self.entry.append(self.intel)
        self.cha = Entry(self.window, width = 5, textvariable = self.chaint, bg = "white")
        self.cha.place(x = 1180, y = 440)
        self.entry.append(self.cha)
        self.HPM = Entry(self.window, width = 5, textvariable = self.hpmint, bg = "white")
        self.HPM.place(x = 1180, y = 460)
        self.entry.append(self.HPM)
        self.POWM = Entry(self.window, width = 5, textvariable = self.powint, bg = "white")
        self.POWM.place(x = 1180, y = 480)
        self.entry.append(self.POWM)
        self.PoT = Entry(self.window, width = 5, textvariable = self.PoTint, bg = "white")
        self.PoT.place(x = 1380, y = 320)
        self.entry.append(self.PoT)
        self.HoT = Entry(self.window, width = 5, textvariable = self.HoTint, bg = "white")
        self.HoT.place(x = 1380, y = 340)
        self.entry.append(self.HoT)
        self.AC = Entry(self.window, width = 5, textvariable = self.ACint, bg = "white")
        self.AC.place(x = 1380, y = 360)
        self.entry.append(self.AC)
        self.PR = Entry(self.window, width = 5, textvariable = self.PRint,  bg = "white")
        self.PR.place(x = 1380, y = 380)
        self.entry.append(self.PR)
        self.DR = Entry(self.window, width = 5, textvariable = self.DRint,  bg = "white")
        self.DR.place(x = 1380, y = 400)
        self.entry.append(self.DR)
        self.FR = Entry(self.window, width = 5, textvariable = self.FRint, bg = "white")
        self.FR.place(x = 1380, y = 420)
        self.entry.append(self.FR)
        self.CR = Entry(self.window, width = 5, textvariable = self.CRint, bg = "white")
        self.CR.place(x = 1380, y = 440)
        self.entry.append(self.CR)
        self.LR = Entry(self.window, width = 5, textvariable = self.LRint,bg = "white")
        self.LR.place(x = 1380, y = 460)
        self.entry.append(self.LR)
        self.AR = Entry(self.window, width = 5, textvariable = self.ARint, bg = "white",)
        self.AR.place(x = 1380, y = 480)
        self.entry.append(self.AR)

        
        self.vari.trace("w", self.generateItem)
        
        #Search Parameters
        self.submit = Label(self.window, text="Search Parameters:", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 125, y = 560)
        self.labels.append(self.submit) 
        subm = Button(self.window, text = "SUBMIT", width = 6, command = self.generateItemList)
        subm.place(x = 360, y = 560)
        self.labels.append(subm)
        self.submit = Label(self.window, text="Has item been Audited?", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 50, y = 595)
        self.labels.append(self.submit) 
        self.auditcheck = Checkbutton(self.window, variable = self.auditapprv, text = "Yes", width = 10, bg = "white")
        self.auditcheck.place(x = 50, y = 620)
        self.labels.append(self.auditcheck)
        self.eqpbox = Label(self.window, text="Equip Type", bg = "black", fg = "white", font = "none 12 bold")
        self.eqpbox.place(x = 275, y = 595)
        self.labels.append(self.eqpbox)
        self.eqslot = OptionMenu(self.window, self.equip, *self.equipTypes)
        self.eqslot.place(x = 255, y = 620)
        self.equip.set(None)
        self.labels.append(self.eqslot)
        self.submit = Label(self.window, text="Level", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 390, y = 595)
        self.labels.append(self.submit)
        self.level = Entry(self.window, width = 2, textvariable = self.lev, bg = "white",)
        self.level.place(x = 400, y = 620)
        self.entry.append(self.level)
        self.submit = Label(self.window, text="Name/words in name", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 450, y = 595)
        self.labels.append(self.submit) 
        self.names = Entry(self.window, width = 20, textvariable = self.name, bg = "white")
        self.names.place(x = 450, y = 620)
        self.entry.append(self.names)

        #3d Selector portion
        self.imagetype = ttk.Combobox(self.window, textvariable = self.varimagetype, values = self.itemTypes, state = "readonly", width = 30)
        self.imagetype.place(x = 1180, y = 500)
        self.labels.append(self.imagetype)
        self.imagename = ttk.Combobox(self.window, textvariable = self.varimagename, state = "readonly", width = 30)
        self.imagename.place(x = 1180, y = 520)
        self.labels.append(self.imagename)
        
        #Detects changes for Image Type
        self.varimagetype.trace("w", self.createImageNames)
        
        #Detects changes for Image Name
        self.varimagename.trace("w", self.create3dImage)
        
        #Color
        self.colors = Button(text = 'Select Color for item', command = self.getColor)
        self.colors.place(x = 1300, y = 100)
        self.labels.append(self.colors)
        
        #notes
        self.note = Entry(self.window, textvariable = self.notes, width = 100, bg = "white")
        self.note.place(x = 150, y = 850)
        self.entry.append(self.note)

        #Buttons
        self.submit = Label(self.window, text="Change to View/Edit:", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 50, y = 655)
        self.labels.append(self.submit)
        self.item = ttk.Combobox(self.window, textvariable = self.vari, state = "readonly", width = 30)
        self.item.place(x = 240, y = 655)
        self.labels.append(self.item)
        
        self.submit = Label(self.window, text="Submit Audit, if changes are made please do not click approve", bg = "black", fg = "white", font = "none 12 bold")
        self.submit.place(x = 50, y = 680)
        self.labels.append(self.submit)
        sub = Button(self.window, text = "SUBMIT", width = 6, command = self.click)
        sub.place(x = 50, y = 705)
        self.labels.append(sub)
        self.audi = Checkbutton(self.window, variable = self.checkint, text = "Approval", width = 10, bg = "white", command = self.pushAudit)
        self.audi.place(x = 200, y = 705)
        self.labels.append(self.audi)
        
        #Creator/Modifier/Auditor
        self.create = Label(self.window, text="Creator:", bg = "black", fg = "white", font = "none 12 bold")
        self.create.place(x = 50, y = 760)
        self.labels.append(self.create)        
        self.mod = Label(self.window, text="Last Modifier:", bg = "black", fg = "white", font = "none 12 bold")
        self.mod.place(x = 50, y = 780)
        self.labels.append(self.mod)
        self.audit = Label(self.window, text="Audit Approved:", bg = "black", fg = "white", font = "none 12 bold")
        self.audit.place(x = 50, y = 740)
        self.labels.append(self.audit)
        self.auditor = Entry(self.window, width = 30, textvariable = self.auditstr, bg = "white", state = "readonly")
        self.auditor.place(x = 240, y = 740)
        self.entry.append(self.auditor)
        self.cret = Entry(self.window, width = 30, textvariable = self.createstr, bg = "white", state = "readonly")
        self.cret.place(x = 240, y = 760)
        self.entry.append(self.cret)
        self.modify = Entry(self.window, width = 30, textvariable = self.modifystr, bg = "white", state = "readonly")
        self.modify.place(x = 240, y = 780)
        self.entry.append(self.modify) 
        
    def generateItem(self, *args):
        itemunit = self.vari.get()
        if itemunit == '' or itemunit == None:
            return
        row = self.session.query(items).filter(items.Itemname == itemunit).first()
                    
        self.notes.set(row.Notes)
        
        self.itemstr.set(row.Itemname)
        self.idint.set(row.Itemid)
        self.famint.set(row.Itemfamily)
        
        self.varrare.set(row.Rarity)
        
        #acquires icon/name

        icon = self.session.query(icons, icontype).join(icontype).filter(icons.iconhex == row.Itemicon)
        for a in icon:
            self.varp.set(a[0].iconname)
            self.varitem.set(a[1].icontype)
        
        #Acquires EquipmentTypes
        eqp = self.session.query(equipTypes)[row.Slot]
        eqptype = eqp.equipmenttype
        self.equipstr.set(eqptype)

        #Acquires attack types
        atk = self.session.query(attackTypes)[row.Attacktype]
        atktype = atk.attacktype
        self.atktypestr.set(atktype)
        
        #Checkboxes
        self.tvalue.set(row.Trade)
        self.rvalue.set(row.Rent)
        self.lvalue.set(row.Lore)
        self.cvalue.set(row.Craft)
               
        self.wpnint.set(row.Damage)
        self.levelint.set(row.Level)
        self.maxint.set(row.Maxstack)
        self.itemdescstr.set(row.Itemdesc)
        self.maxhpint.set(row.HPMAX)
        self.durint.set(row.Durability)
        self.createstr.set(row.Creator)
        self.modifystr.set(row.Modifier)
        
        classes = row.Class
        
        if (classes>>classSummary.ALC)&1:
            self.alcint.set(2**classSummary.ALC)
        else:
            self.alcint.set(0)
            
        if (classes>>classSummary.WAR)&1:
            self.warint.set(2**classSummary.WAR)
        else:
            self.warint.set(0)
            
        if (classes>>classSummary.BRD)&1:
            self.brdint.set(2**classSummary.BRD)
        else:
            self.brdint.set(0)
            
        if (classes>>classSummary.CL)&1:
            self.clint.set(2**classSummary.CL)
        else:
            self.clint.set(0)
            
        if (classes>>classSummary.DRD)&1:
            self.drdint.set(2**classSummary.DRD)
        else:
            self.drdint.set(0)
            
        if (classes>>classSummary.ENC)&1:
            self.encint.set(2**classSummary.ENC)
        else:
            self.encint.set(0)
            
        if (classes>>classSummary.MAG)&1:
            self.magint.set(2**classSummary.MAG)
        else:
            self.magint.set(0)
            
        if (classes>>classSummary.MNK)&1:
            self.mnkint.set(2**classSummary.MNK)
        else:
            self.mnkint.set(0)
            
        if (classes>>classSummary.NEC)&1:
            self.necint.set(2**classSummary.NEC)
        else:
            self.necint.set(0)
            
        if (classes>>classSummary.PAL)&1:
            self.palint.set(2**classSummary.PAL)
        else:
            self.palint.set(0)
            
        if (classes>>classSummary.RAN)&1:
            self.ranint.set(2**classSummary.RAN)
        else:
            self.ranint.set(0)
            
        if (classes>>classSummary.RGE)&1:
            self.rgeint.set(2**classSummary.RGE)
        else:
            self.rgeint.set(0)
            
        if (classes>>classSummary.SK)&1:
            self.skint.set(2**classSummary.SK)
        else:
            self.skint.set(0)
            
        if (classes>>classSummary.SHA)&1:
            self.shaint.set(2**classSummary.SHA)
        else:
            self.shaint.set(0)
            
        if (classes>>classSummary.WIZ)&1:
            self.wizint.set(2**classSummary.WIZ)
        else:
            self.wizint.set(0)
        
        races = row.Race
        
        if (races>>raceSummary.HUM)&1:
            self.humint.set(2**raceSummary.HUM)
        else:
            self.humint.set(0)
        
        if (races>>raceSummary.ELF)&1:
            self.elfint.set(2**raceSummary.ELF)
        else:
            self.elfint.set(0)
            
        if (races>>raceSummary.DELF)&1:
            self.delfint.set(2**raceSummary.DELF)
        else:
            self.delfint.set(0)
            
        if (races>>raceSummary.GNO)&1:
            self.gnoint.set(2**raceSummary.GNO)
        else:
            self.gnoint.set(0)
            
        if (races>>raceSummary.DWF)&1:
            self.dwfint.set(2**raceSummary.DWF)
        else:
            self.dwfint.set(0)
            
        if (races>>raceSummary.TRL)&1:
            self.trlint.set(2**raceSummary.TRL)
        else:
            self.trlint.set(0)
            
        if (races>>raceSummary.BAR)&1:
            self.barint.set(2**raceSummary.BAR)
        else:
            self.barint.set(0)
            
        if (races>>raceSummary.HLF)&1:
            self.hlfint.set(2**raceSummary.HLF)
        else:
            self.hlfint.set(0)
            
        if (races>>raceSummary.ERU)&1:
            self.eruint.set(2**raceSummary.ERU)
        else:
            self.eruint.set(0)
            
        if (races>>raceSummary.OGR)&1:
            self.ogrint.set(2**raceSummary.OGR)
        else:
            self.ogrint.set(0)

        self.wepprocint.set(row.Wepproc)
        self.armprocint.set(row.Armproc)
        self.procnamestr.set(row.Procname)
        self.strint.set(row.Strength)
        self.staint.set(row.Stamina)
        self.agiint.set(row.Agility)
        self.dexint.set(row.Dexterity)
        self.wisint.set(row.Wisdom)
        self.intint.set(row.Intelligence)
        self.chaint.set(row.Charisma)
        self.hpmint.set(row.HP_MAX)
        self.powint.set(row.PWR_MAX)
        self.PoTint.set(row.PoT)
        self.HoTint.set(row.HoT)
        self.ACint.set(row.AC)
        self.PRint.set(row.PR)
        self.DRint.set(row.DR)
        self.FRint.set(row.FR)
        self.CRint.set(row.CR)
        self.LRint.set(row.LR)
        self.ARint.set(row.AR)
        
        self.varreg.set(row.region)
        
        self.auditstr.set(row.Verifier)
        
        self.color = row.Color
        
        if eqptype:
            if eqptype == 'PRIMARY' or eqptype == 'SECONDARY' or eqptype == 'HELD':
                for a in self.session.query(item, itemtype).join(itemtype).filter(item.itemhex == row.Imagegraphic):
                    self.varimagename.set(a[0].itemname)
                    self.varimagetype.set(a[1].itemtype)
                    
            elif eqptype == 'ROBE':
                for rows in self.session.query(Robe).filter(row.Robe == Robe.itemhex):
                    self.varimagename.set(rows.itemname)
                    self.varimagetype.set('Robe')

            elif eqptype == 'HELM':
                for rows in self.session.query(Helm).filter(row.Helm == Helm.itemhex):
                    self.varimagename.set(rows.itemname)
                    self.varimagetype.set('Helm')     

            elif eqptype == 'CHEST' or eqptype == 'LEGS' or eqptype == 'GLOVES' or eqptype == 'FEET' or eqptype == 'BRACERS' or eqptype == 'BRACELETS':
                for rows in self.session.query(Armor).filter(row.Armor == Armor.itemhex):
                    self.varimagename.set(rows.itemname)
                    self.varimagetype.set('Armor')
                    
        else: 
            self.varimagetype.set('')
            self.varimagename.set('')
            for picture in self.picture:
                picture.destroy()
        
    def click(self):
        
        self.itemObject.clear()

        #clears error incase of new one
        if self.error:
            self.error.destroy()
        
        #Makes sure ItemID is a digit or generates an error    
        if self.itemid.get().isdigit():
            self.itemObject.append(self.itemid.get())
            
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with ItemID", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        #Makes sure item Family is a digit or generates an error       
        if self.itemfam.get().isdigit():
            self.itemObject.append(self.itemfam.get())
            
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with ItemFam", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        #Checks if a return for self.icon exists, if not we generate an error   
        if self.icon.get():
            
            icon = self.icon.get()
            for row in self.session.query(icons).filter(icons.iconname == icon):
                self.itemObject.append(row.iconhex)
            
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="No Icon selected", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        
        #Checks if a return for equipmenttype exists, if not generate an error 
        if self.equipstr.get():
            self.itemObject.append(self.equipstr.get())

        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text= "No Equipment Type selected", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        self.itemObject.append(self.tvalue.get())
        self.itemObject.append(self.rvalue.get())
        
        #checks is attype is disabled, if not, gets value
        if self.atktype['state'] == 'disabled':
            self.itemObject.append(0)
        
        elif self.atktypestr.get():
            self.itemObject.append(self.atktypestr.get)
        
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text= "Error with Attack Type", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
        
        #self.wpndmg = self.convert(self.wpndmg.get())
        #Weapon damage state check, then makes sure int > 0 
        if self.wpndmg['state'] == 'disabled':
            self.itemObject.append(0)

        elif self.wpndmg.get().isdigit() > 0:
            self.itemObject.append(self.wpndmg)
        
        else:
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text= "Error with Weapon Damage", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return

        #Level catch for non-digits, digits < 1 and state check
        if self.lvlreq['state'] == 'disabled':
            self.itemObject.append(0)
            
        elif self.lvlreq.get().isdigit() > 0:
            self.itemObject.append(self.lvlreq.get())
            
        else:
            
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with Level", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
  
        #Maxstack catch for state and digits
        if self.maxstack['state'] == 'disabled':
            self.itemObject.append(0)

        elif self.maxstack.get().isdigit():
            self.itemObject.append(self.maxstack.get())
            
        else:
            
            if self.error:
                self.error.destroy()
            self.error = Label(self.window, text="Error with max stack", bg = "black", fg = "white", font = "none 12 bold")
            self.error.place(x = 700, y = 700)
            return
            
        #Max HP catch for non-equipment
        if self.maxhp['state'] == 'disabled':
            self.itemObject.append(0)
            
        else:
            if self.maxhp.get().isdigit() > 0:
                self.itemObject.append(self.maxhp.get())
                
            else:
            
                if self.error:
                    self.error.destroy()
                self.error = Label(self.window, text="Error with max HP", bg = "black", fg = "white", font = "none 12 bold")
                self.error.place(x = 700, y = 700)
                return
            
        #Catch for durability
        if self.dur['state'] == 'disabled':
            self.itemObject.append(0)
            
        else:
            if self.dur.get().isdigit() > 0:
                self.itemObject.append(self.dur.get())
                
            else:
            
                if self.error:
                    self.error.destroy()
                self.error = Label(self.window, text="Error with Durability", bg = "black", fg = "white", font = "none 12 bold")
                self.error.place(x = 700, y = 700)
                return

        itemClass = 0
        
        if self.allint.get():
            itemClass += self.allint.get()
        
        else:
            if self.tankint.get():
                itemClass += self.tankint.get()
                
            else:
                itemClass += self.palint.get()
                itemClass += self.warint.get()
                itemClass += self.skint.get()
            
            if self.healint.get():
                itemClass += self.healint.get()
                
            else:
                itemClass += self.clint.get()
                itemClass += self.shaint.get()
                itemClass += self.drdint.get()
                
            if self.melint.get():
                itemClass += self.melint.get()
                
            else:
                itemClass += self.ranint.get()
                itemClass += self.rgeint.get()
                itemClass += self.brdint.get()
                itemClass += self.mnkint.get()
                
            if self.castint.get():
                itemClass += self.castint.get()
                
            else:
                itemClass += self.encint.get()
                itemClass += self.alcint.get()
                itemClass += self.magint.get()
                itemClass += self.necint.get()
                itemClass += self.wizint.get()
        
        self.itemObject.append(itemClass)
         
        itemRace = 0
        
        if self.rallint.get():
            itemRace += self.rallint.get()           
            
        else:
            itemRace += self.humint.get()
            itemRace += self.elfint.get()
            itemRace += self.delfint.get()
            itemRace += self.gnoint.get()
            itemRace += self.dwfint.get()
            itemRace += self.trlint.get()
            itemRace += self.barint.get()
            itemRace += self.hlfint.get()
            itemRace += self.eruint.get()
            itemRace += self.ogrint.get()
            
        self.itemObject.append(itemRace)
          

        self.itemObject.append(self.wepproc.get())
        self.itemObject.append(self.armproc.get())
        self.itemObject.append(self.procname.get())
        self.itemObject.append(self.lvalue.get())
        self.itemObject.append(self.cvalue.get())
        self.itemObject.append(self.itemname.get())
        self.itemObject.append(self.itemdesc.get())
        
        totStats = [self.strength,
                    self.sta,
                    self.agi,
                    self.dex,
                    self.wis,
                    self.intel,
                    self.cha,
                    self.HPM,
                    self.POWM,
                    self.PoT,
                    self.HoT,
                    self.AC,
                    self.PR,
                    self.DR,
                    self.FR,
                    self.CR,
                    self.LR,
                    self.AR]
                    
        #Catch for Stats
        for stat in totStats:
            if stat['state'] == 'disabled':
                self.stats.append(0)
            
            else:
                if stat.get().isdigit() >= 0:
                    self.stats.append(stat.get())
                
                else:
            
                    if self.error:
                        self.error.destroy()
                    self.error = Label(self.window, text="Error with Stats", bg = "black", fg = "white", font = "none 12 bold")
                    self.error.place(x = 700, y = 700)
                    return
                    
        self.itemObject.append(self.varreg.get())
        
        eqptype = self.equipstr.get()
        if self.varimagename.get():
            if eqptype == 'HELM' or eqptype == 'GLOVES' or eqptype == 'LEGS' or eqptype == 'CHEST' or eqptype == 'FEET' or eqptype == 'ROBE' or eqptype == 'BRACELET' or eqptype == 'BRACERS':
                if eqptype == 'GLOVES' or eqptype == 'LEGS' or eqptype == 'CHEST' or eqptype == 'FEET' or eqptype == 'BRACELET' or eqptype == 'BRACERS':
                    for row in self.session.query(Armor).filter(Armor.itemname == self.varimagename.get()):
                        items = ['Armor', row.itemhex]
                        self.itemObject.append(items)
                                 
                elif eqptype == 'ROBE':
                    for row in self.session.query(Robe).filter(Robe.itemname == self.varimagename.get()):
                        items = ['Robe', row.itemhex]
                        self.itemObject.append(items)
                        
                elif eqptype == 'HELM':                    
                    for row in self.session.query(Helm).filter(Helm.itemname == self.varimagename.get()):
                        items = ['Helm', row.itemhex]
                        self.itemObject.append(items)
                        
            else:
                for row in self.session.query(item).filter(item.itemname == self.varimagename.get()):
                    items = ['Other', row.itemhex]
                    self.itemObject.append(items)
        else:
            items = [None, None]    
            self.itemObject.append(items)
        
        if self.color != None:
            print('Received color {}'.format(self.color))
            self.itemObject.append(self.color)
                
        else:
            self.itemObject.append(0)
             
        self.itemObject.append(self.varrare.get())
                
        self.itemObject.append(self.login)
        self.itemObject.append(self.note.get())
        self.gear.addItem(self.itemObject, self.stats, self)
        self.gear.processSubmit()
        self.gear.updateGear()
        
    
    def createArmor(self):
        self.wepproc.config(state = 'disabled')
        self.armproc.config(state = 'normal')
        self.maxstack.config(state = 'disabled')
        self.atktype.config(state = 'disabled')
        self.wpndmg.config(state = 'disabled')
        self.strength.config(state = 'normal')
        self.sta.config(state = 'normal')
        self.agi.config(state = 'normal')
        self.dex.config(state = 'normal')
        self.wis.config(state = 'normal')
        self.intel.config(state = 'normal')
        self.cha.config(state = 'normal')
        self.HPM.config(state = 'normal')
        self.POWM.config(state = 'normal')
        self.PoT.config(state = 'normal')
        self.HoT.config(state = 'normal')
        self.AC.config(state = 'normal')
        self.PR.config(state = 'normal')
        self.DR.config(state = 'normal')
        self.FR.config(state = 'normal')
        self.CR.config(state = 'normal')
        self.LR.config(state = 'normal')
        self.AR.config(state = 'normal')
    
    def createWeapon(self):
        self.armproc.config(state = 'disabled')
        self.wepproc.config(state = 'normal')
        self.strength.config(state = 'normal')
        self.sta.config(state = 'normal')
        self.agi.config(state = 'normal')
        self.dex.config(state = 'normal')
        self.wis.config(state = 'normal')
        self.intel.config(state = 'normal')
        self.cha.config(state = 'normal')
        self.HPM.config(state = 'normal')
        self.POWM.config(state = 'normal')
        self.PoT.config(state = 'normal')
        self.HoT.config(state = 'normal')
        self.AC.config(state = 'normal')
        self.PR.config(state = 'normal')
        self.DR.config(state = 'normal')
        self.FR.config(state = 'normal')
        self.CR.config(state = 'normal')
        self.LR.config(state = 'normal')
        self.AR.config(state = 'normal')
        self.atktype.config(state = 'normal')
        self.wpndmg.config(state = 'normal')
        self.maxhp.config(state = 'normal')
        self.dur.config(state = 'normal') 
        self.maxstack.config(state = 'disabled')
    
    def createItems(self):
        self.strength.config(state = 'disabled')
        self.sta.config(state = 'disabled')
        self.agi.config(state = 'disabled')
        self.dex.config(state = 'disabled')
        self.wis.config(state = 'disabled')
        self.intel.config(state = 'disabled')
        self.cha.config(state = 'disabled')
        self.HPM.config(state = 'disabled')
        self.POWM.config(state = 'disabled')
        self.PoT.config(state = 'disabled')
        self.HoT.config(state = 'disabled')
        self.AC.config(state = 'disabled')
        self.PR.config(state = 'disabled')
        self.DR.config(state = 'disabled')
        self.FR.config(state = 'disabled')
        self.CR.config(state = 'disabled')
        self.LR.config(state = 'disabled')
        self.AR.config(state = 'disabled')
        self.atktype.config(state = 'disabled')
        self.wpndmg.config(state = 'disabled')
        self.maxhp.config(state = 'disabled')
        self.dur.config(state = 'disabled')
        self.maxstack.config(state = 'normal')
        
    def processEquip(self, *args):

        [self.createArmor() for x in equipEnum.armor if x == self.equipstr.get()]
        [self.createWeapon() for x in equipEnum.weapon if x == self.equipstr.get()]
        [self.createItems() for x in equipEnum.items if x == self.equipstr.get()]
        
        choice = self.equipstr.get()
        if choice:
            if choice == 'ROBE':
                self.imagename.config(values = 'Robe')
                self.varimagetype.set('Robe')
 
            elif choice == 'CHEST' or choice == 'GLOVES' or choice == 'FEET' or choice == 'BRACERS' or choice == 'BRACELET' or choice == 'LEGS':
                self.imagename.config(values = 'Armor')
                self.varimagetype.set('Armor')
            
            elif choice == 'HELM':
                self.imagename.config(values = 'Helm')
                self.varimagetype.set('Helm')
     
    def change_image(self, *args):
        pic = self.varp.get()
        self.pict = None
        if pic == None:
            return
        for row in self.session.query(icons).filter(icons.iconname == pic):  
            # Change image of label accordingly
            self.pict = (PhotoImage(data = row.image))

        self.label = Label(self.window ,image = self.pict, bg = "black")
        self.label.place(x = 390, y = 420)
        self.picture.append(self.label)
    
    def createIconNames(self, *args):
        self.iconNames.clear()
        iconType = self.varitem.get()
        if iconType == None or iconType == '':
            return
   
        row = self.session.query(icons.iconname).join(icontype).filter(icontype.icontype == iconType)
        for a in row.all():
            self.iconNames.append(a.iconname)
            
        self.iconNames.sort()
        self.iconNames.insert(0, None)
        self.icon.config(values = self.iconNames)

    def pushAudit(self):
        self.gear.audit(self.login, self.itemid.get())
        
    def generateItemList(self):
        self.items.clear()  
        if self.auditapprv.get():
            #print('Searching for pre-audited items')
            if (self.equip.get() != 'None') and self.lev.get():
                for rows in self.session.query(equipType).filter(equipType.equipmenttype == self.equip.get()):
                    for row in self.session.query(items).filter(items.Slot == rows.hexid).filter(items.Level == self.lev.get()).filter(items.Verifier != None):
                        self.items.append(row.Itemname)
                    
            elif self.equip.get() != 'None':
                for rows in self.session.query(equipType).filter(equipType.equipmenttype == self.equip.get()):
                    for row in self.session.query(items).filter(items.Slot == rows.hexid).filter(items.Verifier != None):
                        self.items.append(row.Itemname)
        
            elif self.lev.get():
                for row in self.session.query(items).filter(items.Level == self.lev.get()).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
        
            elif self.name.get():
                for row in self.session.query(items).filter(items.Itemname.like('%'+self.name.get()+'%')).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
                
            else:
                for row in self.session.query(items).filter(items.Verifier != None):
                    self.items.append(row.Itemname)
            
        else:
            #print('Searching for non-audited items')
            if (self.equip.get() != 'None') and self.lev.get():
                for rows in self.session.query(equipType).filter(equipType.equipmenttype == self.equip.get()):
                    for row in self.session.query(items).filter(items.Slot == rows.hexid).filter(items.Level == self.lev.get()).filter(items.Verifier == None):
                        self.items.append(row.Itemname)
                    
            elif self.equip.get() != 'None':
                for rows in self.session.query(equipType).filter(equipType.equipmenttype == self.equip.get()):
                    for row in self.session.query(items).filter(items.Slot == rows.hexid).filter(items.Verifier == None):
                        self.items.append(row.Itemname)
        
            elif self.lev.get():
                for row in self.session.query(items).filter(items.Level == self.lev.get()).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
        
            elif self.name.get():
                for row in self.session.query(items).filter(items.Itemname.like('%'+self.name.get()+'%')).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
                    
                
            else:
                for row in self.session.query(items).filter(items.Verifier == None):
                    self.items.append(row.Itemname)
                    
        self.item.configure(values = self.items)
    
    def createImageNames(self, *args):
        self.imageNames.clear()
        itemType = self.varimagetype.get()
        if itemType == None or itemType == '':
            return
             
        if itemType == 'Robe' or itemType == 'Armor' or itemType == 'Helm':
            if itemType == 'Robe':
                for row in self.session.query(Robe):
                    self.imageNames.append(row.itemname)
                    
            elif itemType == 'Helm':
                for row in self.session.query(Helm):
                    self.imageNames.append(row.itemname)
                    
            elif itemType == 'Armor':
                for row in self.session.query(Armor):
                    self.imageNames.append(row.itemname)
        else:            
            row = self.session.query(item.itemname).join(itemtype).filter(itemtype.itemtype == itemType)
            for a in row.all():
                self.imageNames.append(a.itemname)
        
        self.imageNames.sort()
        self.imagename.config(values = self.imageNames)

    def create3dImage(self, *args):
        name = self.varimagename.get()
        self.image = None
        if name  == 'None':
            return
            
        if self.varimagetype.get() == 'Robe' or self.varimagetype.get() == 'Armor' or self.varimagetype.get() == 'Helm':
            if self.varimagetype.get() == 'Robe':
                for row in self.session.query(Robe).filter(Robe.itemname == name):
                    self.image = PhotoImage(data = row.image)

            elif self.varimagetype.get() == 'Helm':
                for row in self.session.query(Helm).filter(Helm.itemname == name):
                    self.image = PhotoImage(data = row.image)

            elif self.varimagetype.get() == 'Armor':
                for row in self.session.query(Armor).filter(Armor.itemname == name):                  
                    self.image = PhotoImage(data = row.image)

        else:
            for row in self.session.query(item).filter(item.itemname == name):
                self.image = PhotoImage(data = row.image)
        
        self.label = Label(self.window, image = self.image, bg = "black")
        self.label.place(x = 1030, y = 600)
        self.picture.append(self.label)
        
    def getColor(self):
        thisColor = None
        if self.color == None:
            pass
        else:
            print('Pre-defined color')
            thisColor = '0x' + hex(self.color)[2:].zfill(8)
            thisColor = thisColor.replace('0x', '#')[:-2]
            
        color = colorchooser.askcolor(color = thisColor, title = "Select color for this item, if any")
        if color == (None, None):
            return
        color = (color[1].strip('#'))+'FF'
        self.color = int(color, 16)
        
    def removeEntry(self):
        for entry in self.entry:
            if entry:
                entry.delete(0, END)
        
        for checkbox in self.checkbox:
            if checkbox:
                checkbox.set(0)
       
        for combobox in self.combobox:
            if combobox:
                combobox.set('')
