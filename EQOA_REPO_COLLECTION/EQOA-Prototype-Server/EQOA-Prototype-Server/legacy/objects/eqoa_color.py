def enum(**enums):
   return type('Enum', (), enums)
   
CHAR_GEARCOLOR = enum(DEFAULT     = enum(R = 0xFF, 
                                         G = 0xFF, 
                                         B = 0xFF),
                      VAN_BLACK   = enum(R = 0x5C, 
                                         G = 0x5C, 
                                         B = 0x5C), 
                      VAN_BLUE    = enum(R = 0x00, 
                                         G = 0x00, 
                                         B = 0xFF),  
                      VAN_BROWN   = enum(R = 0x96, 
                                         G = 0x64, 
                                         B = 0x32), 
                      VAN_COBALT  = enum(R = 0x40, 
                                         G = 0x00, 
                                         B = 0x80), 
                      VAN_GREEN   = enum(R = 0x00, 
                                         G = 0x80, 
                                         B = 0x00), 
                      VAN_ORANGE  = enum(R = 0xFF, 
                                         G = 0x80, 
                                         B = 0x00), 
                      VAN_PUCE    = enum(R = 0x80, 
                                         G = 0x40, 
                                         B = 0x40), 
                      VAN_RED     = enum(R = 0xF0, 
                                         G = 0x3C, 
                                         B = 0x3C), 
                      VAN_SKY     = enum(R = 0xA5, 
                                         G = 0xF5, 
                                         B = 0xFF), 
                      VAN_STEEL   = enum(R = 0x80, 
                                         G = 0x80, 
                                         B = 0x80), 
                      VAN_TAN     = enum(R = 0x80, 
                                         G = 0x40, 
                                         B = 0x40), 
                      VAN_TEAL    = enum(R = 0xFF, 
                                         G = 0x00, 
                                         B = 0xB4), 
                      VAN_WINE    = enum(R = 0x96, 
                                         G = 0x00, 
                                         B = 0x3C), 
                      VAN_YELLOW  = enum(R = 0xF0, 
                                         G = 0xF0, 
                                         B = 0x00), 
                      CAT_BLUE    = enum(R = 0x20, 
                                         G = 0x39, 
                                         B = 0x64), 
                      PURPLE1     = enum(R = 0x80, 
                                         G = 0x00, 
                                         B = 0x80), 
                      JB_BLUE     = enum(R = 0x3F, 
                                         G = 0x76, 
                                         B = 0x94), 
                      PURPLE2     = enum(R = 0x75, 
                                         G = 0x00, 
                                         B = 0x75), 
                      JACK_PURPLE = enum(R = 0x44, 
                                         G = 0x44, 
                                         B = 0x88), 
                      RED2        = enum(R = 0xFF, 
                                         G = 0x00, 
                                         B = 0x00), 
                      VER_GREEN   = enum(R = 0x50, 
                                         G = 0x64, 
                                         B = 0x00), 
                      CYPRUS      = enum(R = 0x00, 
                                         G = 0x40, 
                                         B = 0x40), 
                      MALACHITE   = enum(R = 0x1B, 
                                         G = 0xED, 
                                         B = 0x59), 
                      MOODY_BLUE  = enum(R = 0x79, 
                                         G = 0x79, 
                                         B = 0xBD), 
                      JAP_LAUREL  = enum(R = 0x2D, 
                                         G = 0x84, 
                                         B = 0x3C), 
                      HIGHBALL    = enum(R = 0x9E, 
                                         G = 0x9C, 
                                         B = 0x38), 
                      AQUA        = enum(R = 0x80, 
                                         G = 0xFF, 
                                         B = 0xD6), 
                      PER_GREEN   = enum(R = 0x00, 
                                         G = 0x9F, 
                                         B = 0x9F), 
                      DARK_CER    = enum(R = 0x00, 
                                         G = 0x41, 
                                         B = 0x82), 
                      CERULEAN    = enum(R = 0x00, 
                                         G = 0x80, 
                                         B = 0xC0), 
                      BLACK       = enum(R = 0x15, 
                                         G = 0x00, 
                                         B = 0x00), 
                      ORACLE      = enum(R = 0x31, 
                                         G = 0x62, 
                                         B = 0x62), 
                      MAROON      = enum(R = 0x62, 
                                         G = 0x00, 
                                         B = 0x00),
                      CHATEAU     = enum(R = 0xA0, 
                                         G = 0xAA, 
                                         B = 0xB4), 
                      ZAMBEZI     = enum(R = 0x5A, 
                                         G = 0x5A, 
                                         B = 0x5A), 
                      MID_BLUE    = enum(R = 0x00, 
                                         G = 0x00, 
                                         B = 0xA0), 
                      SKY_BLUE    = enum(R = 0x00, 
                                         G = 0xB4, 
                                         B = 0xFF), 
                      PAST_GREEN  = enum(R = 0x00, 
                                         G = 0xC8, 
                                         B = 0x32), 
                      PANCHO      = enum(R = 0xDC, 
                                         G = 0xBE, 
                                         B = 0x96), 
                      RE_BLUE     = enum(R = 0x00, 
                                         G = 0xC8, 
                                         B = 0xB4)) 
                     
                      
ALPHA = enum(ON = 0XFF,
             OFF = 0X00)

class ColorPrep():
    """Prepares RGB Colors with Alpha for Struct.Pack"""
    
    def __init__(self, color, alpha):
        self.color = color
        self.alpha = alpha
    
    def addAlpha(self):
        color_R = self.color[0]
        color_G = self.color[1]
        color_B = self.color[2]
      
        color = [color_R, color_G, color_B, self.alpha]
        
        sum = 0
        shift  = 0

        for byte in color:
            sum += int(byte<<shift)
            shift += 8

        color = '0x{:X}'.format(sum)   
        
        return color 
   
myColor = ColorPrep(CHAR_GEARCOLOR.VAN_GREEN, ALPHA.ON)



print(myColor.addAlpha())



print (CHAR_GEARCOLOR.VAN_YELLOW.R)


