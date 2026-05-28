#!/usr/bin/env python3

from tkinter import * 
import Creator_functions
from sqlalchemy import func   # func.now()
from sqlalchemy import *
from functools import partial
from Creator_DB import *
from Creator_items import itemOptions
from Creator_spells import Spells
from Creator_character import Characters
from Creator_tables import Tables


#main tk functions/Variables
window = Tk()  

class introduction():
    
    def __init__(self):
        self.window       = window
        self.session      = None
        self.login        = None
        self.pwd          = None
        self.db           = None
        self.error        = None
        self.itemOptions  = None
        self.tables       = Tables()
        self.spells       = Spells()
        self.characters   = Characters()
        self.database     = None
        self.labels       = []
        self.buttons      = []
        self.fail         = None
        self.access       = None

        
        
    def selectButton(self):
        self.but = Button(self.window, text = "Tables", width = 10, command = self.tables.displayTables)
        self.but.place(x = 10, y = 5)
        self.buttons.append(self.but)
        self.but2 = Button(self.window, text = "Items", width = 10, command = self.itemOptions.selectOption)
        self.but2.place(x = 120, y = 5)
        self.buttons.append(self.but2)
        self.but3 = Button(self.window, text = "Spells", width = 10, command = self.spells.displaySpells)
        self.but3.place(x = 230, y = 5)
        self.buttons.append(self.but3)
        self.but4 = Button(self.window, text = "Characters", width = 10, command = self.characters.displaycharacters)
        self.but4.place(x = 340, y = 5)
        self.buttons.append(self.but4)
        self.but6 = Button(self.window, text = "Logout", width = 10, command = self.logout)
        self.but6.place(x = 1400, y = 725)
        self.buttons.append(self.but6)
        if self.access == 0:
            self.but9 = Button(self.window, text = "Reset Database", width = 10, command = self.resetDatabase)
            self.but9.place(x = 1400, y = 755)
            self.buttons.append(self.but9)
        
    def addButton(self, button):
        self.labels.append(button)
        
    def addUser(self, login, pwd, db, labels):
        self.login        = login
        self.pwd          = pwd
        self.db           = db
        for label in labels:
            self.labels.append(label)

         
    def clear(self):
        self.login.destroy()
        self.pwd.destroy()
        self.db.destroy()
        self.itemOptions.clearItemClass()
        self.tables.clear()
        self.spells.clear()
        self.characters.clear()
        for label in self.labels:
            label.destroy()
    
    def pushInfo(self):
        self.characters.setClasses(self.itemOptions, self.tables, self.spells, self.session, self.window)
        self.itemOptions.setClasses(self.characters, self.tables, self.spells, self.session, self.access)
        self.spells.setClasses(self.itemOptions, self.tables, self.characters, self.session, self.window)
        self.tables.setClasses(self.itemOptions, self.characters, self.spells, self.session, self.window)
    
    def processCredentials(self):
        #Acquires EquipmentTypes
        self.session, self.database, self.access = sessionCreate(self.login.get(), self.pwd.get(), self.db.get())
        if self.access != None:
            if self.fail:
                self.fail.destroy()
            self.itemOptions = itemOptions(self.window, self.login.get())
            self.clear()
            self.pushInfo()
            self.selectButton()
            
        else:
            if self.fail:
                self.fail.destroy()
            self.fail = Label(self.window, text="Error, Incorrect username?", bg = "black", fg = "white", font = "none 12 bold")
            self.fail.place(x = 50, y = 575)
            self.clear()
            logging(self)
            
    def logout(self):
        self.clear()
        for button in self.buttons:
            button.destroy()
        logging(self)
        
    def resetDatabase(self): 
        string = "DROP DATABASE {};".format(self.database)
        result = self.session.execute(string)
        for button in self.buttons:
            button.destroy()
        if self.fail:
            self.fail.destroy()
        self.clear()
        logging(self)
        
   
#Basic window settings
window.geometry("1600x900+30+30") 
window.title("EQOA Object creator")
window.configure(background = "black")

#EQOA Photo

photo1 = PhotoImage(file="EQOA.gif")
eqoaPhoto = Label(window, image = photo1, bg = "black")
eqoaPhoto.place(x = 600, y = 50)
eqoaPhoto.image = photo1 

def logging(intro):
    #Credential Labels
    labels = []
    boo = Label(window, text="Login Name:   ", bg = "black", fg = "white", font = "none 12 bold")
    boo.place(x = 575, y = 400)
    labels.append(boo)
    foo = Label(window, text="Password:     ", bg = "black", fg = "white", font = "none 12 bold")
    foo.place(x = 825, y = 400)
    labels.append(foo)
    foo = Label(window, text="Database IP:  ", bg = "black", fg = "white", font = "none 12 bold")
    foo.place(x = 625, y = 425)
    labels.append(foo)

    #Credential Entry boxs
    login = Entry(window, width = 15, bg = "white")
    login.place(x = 690, y = 400)
    pwd = Entry(window, width = 15, show = '*', bg = "white")
    pwd.place(x = 925, y = 400)
    db = Entry(window, width = 15, bg = "white")
    db.place(x = 740, y = 425)
    if intro == None:
        print('Instantiating.')
        intro = introduction()
    intro.addUser(login, pwd, db, labels)

    #Credential Button
    but = Button(window, text = "SUBMIT", width = 6, command = intro.processCredentials)
    but.place(x = 750, y = 450)
    intro.addButton(but)
    
    
intro = None    
logging(intro)
window.mainloop()
