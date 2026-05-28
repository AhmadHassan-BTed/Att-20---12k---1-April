#!/usr/bin/env python
#
#
# Devin and Ben
# June 28, 2018 
#

from sqlalchemy import *
from sqlalchemy import Column, Integer, String
from sqlalchemy import create_engine
from sqlalchemy.exc import SQLAlchemyError
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy_utils import database_exists, create_database
from sqlalchemy.orm import sessionmaker

import configparser
#
#####################################################################################################
#
configfile_name = "../config/eqoa_revival_config.ini"
config = configparser.ConfigParser()
config.read(configfile_name)
try:
    print('Trying to connect to Database')
    engine_string  = config.get('database','db_connector')
    engine_string += config.get('database','db_user_rw')   + ':'
    engine_string += config.get('database','db_passwd_rw') + '@'
    engine_string += config.get('database','db_host_ip')   + '/'
    engine_string += config.get('database','db_eqoa_base')
    
finally:
    print('Last method failed, trying another')
    engine_string  = config.get('database2','db_connector')
    engine_string += config.get('database2','db_user_rw')   + ':'
    engine_string += config.get('database2','db_passwd_rw') + '@'
    engine_string += config.get('database2','db_host_ip')   + ':'
    engine_string += config.get('database2','db_host_port') + '/'
    engine_string += config.get('database2','db_eqoa_base')    
#
print('Using this to attach to db: ',engine_string)
engine = create_engine(engine_string, pool_pre_ping=True)
if not database_exists(engine.url):
  create_database(engine.url)
print('Does the database exist?: ',database_exists(engine.url))

Base = declarative_base()
Base.metadata.bind = engine

def get_db_session():
  #Generates a single use session for the client, which is closed after session is over.
  Session = sessionmaker(bind=engine)
  baseDBsession = Session()
  baseDBsession.autocommit = True
  return baseDBsession


#
#####################################################################################################
#

class AccountInfo(Base):
    __tablename__ = "AccountInfo"

    accountid   = Column(INT, nullable = False, autoincrement = True, primary_key = True)
    username    = Column(VARCHAR(32), nullable = False, primary_key = True)
    pwhash      = Column(BINARY(60), nullable = False)
    acctstatus  = Column(INT)
    acctlevel   = Column(INT)
    acctcreation= Column(DATETIME)
    lastlogin   = Column(DATETIME)
    ipaddress   = Column(VARCHAR(16))
    firstname   = Column(VARCHAR(32))
    unknown1    = Column(VARCHAR(32))
    midinitial  = Column(VARCHAR(16))
    lastname    = Column(VARCHAR(32))
    unknown2    = Column(VARCHAR(32))
    countryAB   = Column(VARCHAR(16))
    zip         = Column(VARCHAR(16))
    birthday    = Column(VARCHAR(16))
    birthyear   = Column(VARCHAR(16))
    birthmon    = Column(VARCHAR(16))
    sex         = Column(VARCHAR(16))
    email       = Column(VARCHAR(128))
    result      = Column(INT)
    subtime     = Column(INT)
    partime     = Column(INT)
    subfeatures = Column(INT)
    gamefeatures= Column(INT)
    

#
###################################################################################
#
# Create all Tables 
#
Base.metadata.create_all(engine)
#
