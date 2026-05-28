from tkinter import *
from functools import partial
from Creator_DB import * 
 

class Tables:
    
    def __init__(self):
        self.window      = None
        self.itemOptions = None
        self.Spells      = None
        self.Characters  = None
        self.session     = None
        print('Initializing Tables class')
    
    def displayTables(self):
        self.itemOptions.clearItemClass()
        self.Spells.clear()
        self.Characters.clear()
    
    def setClasses(self, item, character, spell, session, window):
        self.window     = window
        self.itemOptions = item
        self.Characters = character
        self.Spells     = spell
        self.session    = session
        print('Classes placed.')
    
    def clear(self):
        pass
    