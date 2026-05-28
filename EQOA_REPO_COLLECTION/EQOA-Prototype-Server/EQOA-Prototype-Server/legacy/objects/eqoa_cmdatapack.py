'''

created december 25th, 2016
@author: Devin Dallas
'''

import eqoa_utilities

class cmDataPack():
    
    def __init__(self, cmdatapack, unspentcm, spentcm, xptocm, unknown1, moverate, STRbase, STAbase, AGIbase, DEXbase, WISbase, INTbase, CHAbase, STRmax, STAmax, AGImax, DEXmax, WISmax, INTmax, CHAmax, HPmax, PWRmax, HoT, PoT, defmod, offmod, ACbase, HPfactor, FR, LR, CR, AR, PR, DR, unknown2, STRmax2, STAmax2, AGImax2, DEXmax2, WISmax2, INTmax2, CHAmax2):
        
        self.cmdatapack = cmdatapack
        self.unspentcm  = unspentcm
        self.spentcm    = spentcm
        self.xptocm     = xptocm
        self.unknown1   = unknown1
        self.moverate   = moverate
        self.STRbase    = STRbase
        self.STAbase    = STAbase
        self.AGIbase    = AGIbase
        self.DEXbase    = DEXbase
        self.WISbase    = WISbase
        self.INTbase    = INTbase
        self.CHAbase    = CHAbase
        self.STRmax     = STRmax
        self.STAmax     = STAmax
        self.AGImax     = AGImax
        self.DEXmax     = DEXmax
        self.WISmax     = WISmax
        self.INTmax     = INTmax
        self.CHAmax     = CHAmax
        self.HPmax      = HPmax
        self.PWRmax     = PWRmax
        self.HoT        = HoT
        self.PoT        = PoT
        self.defmod     = defmod
        self.offmod     = offmod
        self.ACbase     = ACbase
        self.HPfactor   = HPfactor
        self.FR         = FR
        self.LR         = LR
        self.CR         = CR
        self.AR         = AR
        self.PR         = PR
        self.DR         = DR
        self.unknown2   = unknown2
        self.STRmax2    = STRmax2
        self.STAmax2    = STAmax2
        self.AGImax2    = AGImax2
        self.DEXmax2    = DEXmax2
        self.WISmax2    = WISmax2
        self.INTmax2    = INTmax2
        self.CHAmax2    = CHAmax2
        
        
    def encodecmDataPack(self):
        
        packingList = [];
        encode_fmt = 'f'; packingList.append(self.cmdatapack)          # 4 byte value
        
        encodedcmDataString = eqoa_utilities.packFixed(encode_fmt,packingList)
        
        packingList = [ self.unspentcm,
                        self.spentcm,
                        self.xptocm,
                        self.unknown1]
        
        encodedcmDataString += eqoa_utilities.packVariable(packingList)

                
        packingList = [];
        encode_fmt = '<f'; packingList.append(self.moverate)          # 4 byte float for movement rate

        encodedcmDataString += eqoa_utilities.packFixed(encode_fmt,packingList)
  
  
        packingList = [ self.STRbase,
                        self.STAbase,
                        self.AGIbase,
                        self.DEXbase,
                        self.WISbase,
                        self.INTbase,
                        self.CHAbase,
                        self.STRmax,
                        self.STAmax,
                        self.AGImax,
                        self.DEXmax,
                        self.WISmax,
                        self.INTmax,
                        self.CHAmax,
                        self.HPmax,
                        self.PWRmax,
                        self.HoT,
                        self.PoT,
                        self.defmod,
                        self.offmod,
                        self.ACbase,
                        self.HPfactor,
                        self.FR,
                        self.LR,
                        self.CR,
                        self.AR,
                        self.PR,
                        self.DR,
                        self.unknown2,
                        self.STRmax2,
                        self.STAmax2,
                        self.AGImax2,
                        self.DEXmax2,
                        self.WISmax2,
                        self.INTmax2,
                        self.CHAmax2]
                        
        encodedcmDataString += eqoa_utilities.packVariable(packingList)
        
        return encodedcmDataString;
        
    def printcmDataPack(self):
    
        print
        print ' CMDATAPACK      :    {:8q}'.format(self.cmdatapack)
        print ' UNSPENTCM       :    {:8x}'.format(self.unspentcm)
        print ' SPENTCM         :    {:8x}'.format(self.spentcm)
        print ' XPTOCM          :    {:8x}'.format(self.xptocm)
        print ' UNKNOWN1        :    {:8x}'.format(self.unknown1)
        print ' MOVERATE        :    {:8f}'.format(self.moverate)
        print ' STRBASE         :    {:8x}'.format(self.STRbase)
        print ' STABASE         :    {:8x}'.format(self.STAbase)
        print ' AGIBASE         :    {:8x}'.format(self.AGIbase)
        print ' DEXBASE         :    {:8x}'.format(self.DEXbase)
        print ' WISBASE         :    {:8x}'.format(self.WISbase)
        print ' INTBASE         :    {:8x}'.format(self.INTbase)
        print ' CHABASE         :    {:8x}'.format(self.CHAbase)
        print ' HPMAX           :    {:8x}'.format(self.HPmax)
        print ' PWRMAX          :    {:8x}'.format(self.PWRmax)
        print ' HOT             :    {:8x}'.format(self.HoT)
        print ' POT             :    {:8x}'.format(self.PoT)
        print ' DEFMOD          :    {:8x}'.format(self.defmod)
        print ' OFFMOD          :    {:8x}'.format(self.offmod)
        print ' ACBASE          :    {:8x}'.format(self.ACbase)
        print ' HPFACTOR        :    {:8x}'.format(self.HPfactor)
        print ' FR              :    {:8x}'.format(self.FR)
        print ' LR              :    {:8x}'.format(self.LR)
        print ' CR              :    {:8x}'.format(self.CR)
        print ' AR              :    {:8x}'.format(self.AR)
        print ' PR              :    {:8x}'.format(self.PR)
        print ' DR              :    {:8x}'.format(self.DR)
        print ' UNKNOWN2        :    {:8x}'.format(self.unknown2)
        print ' STRMAX2         :    {:8x}'.format(self.STRmax2)
        print ' STAMAX2         :    {:8x}'.format(self.STAmax2)
        print ' AGIMAX2         :    {:8x}'.format(self.AGImax2)
        print ' DEXMAX2         :    {:8x}'.format(self.DEXmax2)
        print ' WISMAX2         :    {:8x}'.format(self.WISmax2)
        print ' INTMAX2         :    {:8x}'.format(self.INTmax2)
        print ' CHAMAX2         :    {:8x}'.format(self.CHAmax2)
        print
