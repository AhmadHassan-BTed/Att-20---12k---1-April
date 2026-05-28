'''
Created on Apr 30, 2016
@author: Stefan McDaniel
'''

from eqoa_messages import LANGUAGE, RECOMMENDED


# A ServerListing that 
class ServerListing():
   
   # serverName - name of the server to be presented
   # recommended - whether the server should be marked as recommended
   # endPointId - end point identifier of the server
   # ipAddress - IP Address of the server
   # portNumber - port number to use on the server
   # language - language of the server
   def __init__(self, serverName, recommended, endPointId, ipAddress, portNumber, language):
      self.serverName = serverName
      self.recommended = recommended
      self.endPointId = endPointId
      self.ipAddress = ipAddress
      self.portNumber = portNumber
      self.language = language

#class to represent a world server
class LoginServer():
   
   def __init__(self, serverList, accounts):
      self.serverList = serverList
      self.accounts = accounts
   
   def authenticateAccount(self, username, password):
      if username in self.accounts and self.accounts[username] == password:
         return True
      else:
         return False
      
   def getServerList(self):
      return self.serverList

#
# These are used for tests
#   
      
#build a list of servers
_server1 = ServerListing("Castle Lightwolf", RECOMMENDED.NO , 0x1F0A, "192.168.1.100", 10070, LANGUAGE.US_ENGLISH)
_server2 = ServerListing("Test Server 2", RECOMMENDED.YES, 0x1F0A, "192.168.1.100", 10080, LANGUAGE.US_ENGLISH)

_serverList = [_server1, _server2]

#build a couple test user accounts
_userAccounts = { "user1": "password1", "user2":"password2" }

#create the login server instance
loginServer = LoginServer(_serverList, _userAccounts)
