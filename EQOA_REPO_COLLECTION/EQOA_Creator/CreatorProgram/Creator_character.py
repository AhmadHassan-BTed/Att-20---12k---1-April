from tkinter import *
import Creator_functions
from functools import partial
from Creator_DB import * 
 

class Characters:
    def __init__(self):
        self.window      = None
        self.itemOptions = None
        self.Tables      = None
        self.Spells      = None
        self.session     = None
        print('Initializing characters class')
        
    def displaycharacters(self):
        self.itemOptions.clearItemClass()
        self.Spells.clear()
        self.Tables.clear()
    
    def setClasses(self, item, table, spell, session, window):
        self.window      = window
        self.itemOptions = item
        self.Tables      = table
        self.Spells      = spell
        self.session     = session
        print('Classes placed.')
        
    
    def clear(self):
        pass
    