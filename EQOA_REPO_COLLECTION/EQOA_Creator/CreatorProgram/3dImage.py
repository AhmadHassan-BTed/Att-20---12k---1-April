item_key_dict = {2:'Axes',4:'Bows',6:'Clubs',8:'Crossbows',10:'Daggers',12:'God Gear',14:'Epics',16:'Foils',18:'Great Axes',20:'Great Clubs',22:'Great Hammers',24:'Great Swords',26:'Hammers',28:'Knuckles',30:'Long Swords',32:'Maces',34:'Martial Rods',36:'Pole Arms',38:'Short Swords',40:'Spears',42:'Staves', 50: 'Armor', 52: 'Helm', 54: 'Robe',62:'Ammunition',64:'Books',66:'Totems',68:'Wands',80:'Shields'}
#
# Read and Sort
#
image = 0
f = open('./3dImages.txt', 'r')
all_icons = f.readlines()
all_icons.sort()
#
item = []
armor = []
helm = []
robe = []
count = 0
for key in item_key_dict:
    for line in all_icons:
        codes = [];
        icons = line.split("-", 3)
        icontype = int(icons[0])
        if icontype == key:
            count = count +1 
            giffile = icons[3].lstrip(" ").rstrip(" \r\n")
            desc = icons[2]
            icon_id = "".join(icons[1]) 
            big_endian = 0
            cr = icons[1]
            big_endian = '0x'+''.join(cr)
            try:
                with open('./3dgifs/'+giffile, 'rb') as f:
                    image = f.read()
                    
                #image = '../Downloads/ICONS/'+giffile
            except:
                
                if giffile == None or giffile == '':
                    print('No Gif for None type (Armor or Helm)')
                else:
                    print('Error importing {}.gif'.format(giffile))
                    image = 0
                            
            if icontype == 50:
                graphic = [int(icon_id, 16), image, desc.rstrip('\r\n')]
                armor.append(graphic)
            elif icontype == 54:
                graphic = [int(icon_id, 16), image, desc.rstrip('\r\n')]
                robe.append(graphic)
                
            elif icontype == 52:
                graphic = [int(icon_id, 16), image, desc.rstrip('\r\n')]
                helm.append(graphic)
                
            else:
                icon = [int(icon_id, 16), image, desc.rstrip('\r\n'), icontype]
                item.append(icon)
for i in range(len(armor)):                
    print('We have received {}'.format(armor[i][2]))
    
for i in range(len(helm)):                
    print('We have received {}'.format(helm[i][2]))
    
for i in range(len(robe)):                
    print('We have received {}'.format(robe[i][2]))
        
