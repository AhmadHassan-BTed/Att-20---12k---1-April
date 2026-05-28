from tkinter import *
import Creator_functions
from functools import partial
from Creator_DB import * 
 

class Spells:
    def __init__(self):
        self.window      = None
        self.itemOptions = None
        self.Tables      = None
        self.Characters  = None
        self.session     = None
        print('Initializing Spells class')
        
    def setClasses(self, item, table, character, session, window):
        self.window      = window
        self.itemOptions = item
        self.Tables      = table
        self.Characters  = character
        self.session     = session
        print('Classes placed.')
        
    def displaySpells(self):
        self.itemOptions.clearItemClass()
        self.Tables.clear()
        self.Characters.clear()
    
    def clear(self):
        pass
    