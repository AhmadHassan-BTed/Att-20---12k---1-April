00
0A
02/04

02 = Eastern Human
04 = Western Human

QMap <QString,QString> ClassValueToString =
{
    {"00","WAR"},{"01","RAN"},{"02","PAL"},{"03","SK"},
    {"04","MNK"},{"05","BRD"},{"06","RGE"},{"07","DRD"},
    {"08","SHA"},{"09","CL"},{"0A","MAG"},{"0B","NEC"},
    {"0C","ENC"},{"0D","WIZ"},{"0E","ALC"}
};

QMap<QString,QVector<QString>> HumanEasternClassGreetings_VectorMap =
{
    {"ALC",{"alchemist","Alchemist","Academy of Arcane Science","Dilina McNerian","she"}},
    {"BRD",{"bard","Bard","Silken Gauntlet","William Corufost","he"}},
    {"CL",{"cleric","Cleric","Shining Shield Mercenaries","Denouncer Alshea","she"}},
    {"ENC",{"enchanter","Enchanter","Academy of Arcane Science","Azlynn","he"}},
    {"NEC",{"necromancer","Necromancer","House Slaerin","Corious Slaerin","he"}},
    {"RGE",{"rogue","Rogue","Shining Shield Mercenaries","Necorik the Ghost","he"}},
    {"SK",{"shadowknight","Shadowknight","Shining Shield Mercenaries","Malethai Crimsonhand","he"}},
    {"WAR",{"warrior","Warrior","Freeport Militia","Commander Nothard","he"}},
    {"WIZ",{"wizard","Wizard","Academy of Arcane Science","Sivrendesh","he"}},
    {"MAG",{"magician","Magician","Academy of Arcane Science","Malsis","he"}}
};

QMap<QString,QVector<QString>> HumanWesternClassGreetings_VectorMap =
{
    {"ALC",{"alchemist","Alchemist","Academy of Arcane Science","Dilina McNerian","she"}},
    {"BRD",{"bard","Bard","Silken Gauntlet","William Corufost","he"}},
    {"CL",{"cleric","Cleric","Shining Shield Mercenaries","Denouncer Alshea","she"}},
    {"ENC",{"enchanter","Enchanter","Academy of Arcane Science","Azlynn","he"}},
    {"NEC",{"necromancer","Necromancer","House Slaerin","Corious Slaerin","he"}},
    {"RGE",{"rogue","Rogue","Shining Shield Mercenaries","Necorik the Ghost","he"}},
    {"SK",{"shadowknight","Shadowknight","Shining Shield Mercenaries","Malethai Crimsonhand","he"}},
    {"WAR",{"warrior","Warrior","Freeport Militia","Commander Nothard","he"}},
    {"WIZ",{"wizard","Wizard","Academy of Arcane Science","Sivrendesh","he"}},
    {"MAG",{"magician","Magician","Academy of Arcane Science","Malsis","he"}}
};

QMap<QString,QMap<QString,QVector<QString>>> Zones_WorldGreetings_VectorMap =
{
    {"Freeport",{HumanEasternClassGreetings_VectorMap}},{"Qeynos",{HumanWesternClassGreetings_VectorMap}}
};

QMap <QString,QString> RaceAndClassStartingLocationsMap =
{
    //ELF
    {"010E","Fayspires"},//ALC
    {"0105","Tethelin"},//BRD
    {"0109","Fayspires"},//CL
    {"0107","Tethelin"},//DRD
    {"010C","Fayspires"},//ENC
    {"010A","Fayspires"},//MAG
    {"0102","Fayspires"},//PAL
    {"0101","Tethelin"},//RAN
    {"0106","Tethelin"},//RGE
    {"010D","Fayspires"},//WIZ

    //DELF
    {"020E","Neriak"},//ALC
    {"0209","Neriak"},//CL
    {"020C","Neriak"},//ENC
    {"020A","Neriak"},//MAG
    {"020B","Neriak"},//NEC
    {"0206","Neriak"},//RGE
    {"0203","Neriak"},//SK
    {"0200","Neriak"},//WAR
    {"020D","Neriak"},//WIZ

    //GNO
    {"030E","Klick`Anon"},//ALC
    {"0309","Klick`Anon"},//CL
    {"030C","Klick`Anon"},//ENC
    {"030A","Klick`Anon"},//MAG
    {"030B","Klick`Anon"},//NEC
    {"0306","Klick`Anon"},//RGE
    {"0300","Klick`Anon"},//WAR
    {"030D","Klick`Anon"},//WIZ

    //DWF
    {"0409","Moradhim"},//CL
    {"0402","Moradhim"},//PAL
    {"0406","Moradhim"},//RGE
    {"0400","Moradhim"},//WAR

    //TRL
    {"0503","Grobb"},//SK
    {"0508","Grobb"},//SHA
    {"0500","Grobb"},//WAR

    //BAR
    {"0606","Halas"},//RGE
    {"0608","Halas"},//SHA
    {"0600","Halas"},//WAR

    //HLF
    {"0709","Rivervale"},//CL
    {"0707","Rivervale"},//DRD
    {"0706","Rivervale"},//RGE
    {"0700","Rivervale"},//WAR

    //ERU
    {"080E","Highbourne"},//ALC
    {"0809","Highbourne"},//CL
    {"080C","Highbourne"},//ENC
    {"080A","Highbourne"},//MAG
    {"080B","Highbourne"},//NEC
    {"0802","Highbourne"},//PAL
    {"0003","Highbourne"},//SK
    {"080D","Highbourne"},//WIZ

    //OGR
    {"090B","Oggok"},//NEC
    {"0903","Oggok"},//SK
    {"0908","Oggok"},//SHA
    {"0900","Oggok"},//WAR
};
