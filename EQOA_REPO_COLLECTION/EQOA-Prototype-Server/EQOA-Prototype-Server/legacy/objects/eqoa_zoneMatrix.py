'''
Created June 28th, 2017

@author: Ben Turi
'''
#
# For Database Work
#
from sqlalchemy import create_engine,ForeignKey,ForeignKeyConstraint
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, String
from sqlalchemy.orm import sessionmaker

# you can test this by just typing python and then the following commands
# you will need to change the engine to your locaion user/pass, database, and connector type
#
#import eqoa_zoneMatrix
#a = eqoa_zoneMatrix.zoneMatrixList([])
#a.dropTable(eqoa_zoneMatrix.Base)
#a.createTable(eqoa_zoneMatrix.Base,eqoa_zoneMatrix.ses)
#a.loadTable(eqoa_zoneMatrix.ses)
#a.printZoneMatrixList()
#
#b = eqoa_zoneMatrix.continentList([])
#b.dropTable(eqoa_zoneMatrix.Base)
#b.createTable(eqoa_zoneMatrix.Base,eqoa_zoneMatrix.ses)
#b.loadTable(eqoa_zoneMatrix.ses)
#b.printContinentList()


ContinentDict = {0:'Tunaria',1:'Rathe',2:'Odus',3:'Lavastorm',4:'Plane of Sky',5:'Secrets'}                                 

#
# Not really sure where these goes yet. better to pass it in
# Might need to have a DB object that is passed in
#
eng = create_engine('mysql+mysqlconnector://fooUser:fooPass@192.168.1.112/eqoabase')

Base = declarative_base()

Base.metadata.bind = eng        
Session = sessionmaker(bind=eng)
ses = Session()

#
#########################################################################################
#   
# zoneMatrix class holds information on position in Column and Row space for all Continents
# 
#
class zoneMatrix():
    # Define zoneMatrix
    def __init__(self, continent, zonecolumn, zonerow, zonename):
        #
        self.continent     = continent
        self.zonecolumn    = zonecolumn
        self.zonerow       = zonerow
        self.zonename      = zonename
        
    def Center(self):  # return the value
        pass

    def NWCorner(self):  # return the value
        pass

    def SECorner(self):  # return the value
        pass

    def printZoneMatrix(self):
        #
        print ' C: {0:02d}'.format(self.zonecolumn),
        print ' R: {0:02d}'.format(self.zonerow),
        print ' Continent : {:15s}'.format(self.continent),  # might be integer and might lookup in dict
        #print ' Continent : {:15s}'.format(ContinentDict.get(self.continent)),  # might be integer and might lookup in dict
        print ' Name : {:30s}'.format(self.zonename)
#
# zoneMatrixList class holds a list of zoneMatrix Objects
# a database query can be used to initialize this list which should stay in memory all the time
#        
class zoneMatrixList():
    # Generate zoneMatrixList Object
    def __init__(self, myList):
        #
        self.myList = myList
    #
    # prints out the list by calling print method (might switch to __str__ or __repl__) 
    #    
    def printZoneMatrixList(self):
      for entry in self.myList:
        entry.printZoneMatrix()
        
    #    
    # Load zoneMatrixList from Database 
    #
    def loadTable(self,ses): # look to  pass in table name maybe
       # 
       rs = ses.query(ZoneMatrixTable).all()  # this actuall performs the query
       #
       # might want a separate method for this, but for now, just takes query 
       # result and generates a list of zoneMatrix Objects
       #
       for i,r in enumerate(rs):
         self.myList.append(zoneMatrix(r.Continent,r.ZoneColumn,r.ZoneRow,r.Name))
    #
    # Delete zoneMatrix Table from Database 
    #
    def dropTable(self,Base): # Will pass in table name
       #
       ZoneMatrixTable.__table__.drop(Base.metadata.bind)
    #
    # Create zoneMatrix Table in Database 
    #
    def createTable(self,Base,ses): # Will pass in table name
       #
       # should I call drop first?   
       Base.metadata.create_all()  # maybe already there - check to see? print out message?      
       #
       #Read in data from file and insert into table
       #        
       f = open('EQOA_ZONES_v4.csv','r')

       data = f.read().splitlines()

       toInsertList = []
       for entry in data:
         t = entry.split(',')
         toInsertList.append(ZoneMatrixTable(Continent=t[0],ZoneColumn=int(t[1]),ZoneRow=int(t[2]),Name=t[3]))  

       ses.add_all(toInsertList)
       ses.commit()
       #
#
# Class used to specify database table. Sub-class of Base 
#
class ZoneMatrixTable(Base):
   __tablename__ = "ZoneMatrix"

   TableId     = Column(Integer, primary_key=True)
   #Continent   = Column(String(20), ForeignKeyConstraint('Continent','Continents.Id'),nullable=False)
   Continent   = Column(Integer, ForeignKey('Continents.Id'),nullable=True)
   ZoneColumn  = Column(Integer)
   ZoneRow     = Column(Integer)
   Name        = Column(String(30))                
#
#########################################################
#
# Define class for Continent and ContinentList as well with a drop, create, load
# I don't have to read from a file, I can just have a List
#             
#########################################################################################
#
# Continent class holds information about Continent ID mapping
#
#
class continent():
    # Define Continent 
    def __init__(self,conid,name):
        #
        self.conid = conid 
        self.name  = name

    def printContinent(self):
        #
        print ' Id: {0:02d}'.format(self.conid),
        print ' Name : {:15s}'.format(self.name)
#
#
#
class continentList():
    # Generate Continent List Object
    def __init__(self, myList):
        #
        self.myList = myList
    #
    # prints out the list by calling print method (might switch to __str__ or __repl__) 
    #    
    def printContinentList(self):
      for entry in self.myList:
        entry.printContinent()
        
    #    
    # Load zoneMatrixList from Database 
    #
    def loadTable(self,ses): # look to  pass in table name maybe
       # 
       rs = ses.query(ContinentsTable).all()  # this actuall performs the query
       #
       # might want a separate method for this, but for now, just takes query 
       # result and generates a list of Objects
       #
       for i,r in enumerate(rs):
         self.myList.append(continent(r.Id,r.Name))
    #
    # Delete ContinentsTable from Database 
    #
    def dropTable(self,Base): # Will pass in table name
       #
       ContinentsTable.__table__.drop(Base.metadata.bind)
    #
    # Create Continents Table in Database 
    #
    def createTable(self,Base,ses): # Will pass in table name
       #
       # should I call drop first? # which I could only create 1 
       Base.metadata.create_all()  # maybe already there - check to see? print out message?      
       #
       #Read in data from file and insert into table
       #
       CONT_DATA = [[0,'Tunaria'],[1,'Rathe'],[2,'Odus'],[3,'Lavastrom'],[4,'Plane of Sky'],[5,'Secrets']]      
       #
       toInsertList = [] 
       for c in CONT_DATA:
         toInsertList.append(ContinentsTable(Id=c[0],Name=c[1]))  

       ses.add_all(toInsertList)
       ses.commit()
#
#
#
class ContinentsTable(Base):
    __tablename__ = "Continents"
 
    TableId = Column(Integer, primary_key=True)
    Id      = Column(Integer)
    Name    = Column(String(20))  
       
