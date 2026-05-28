#ifndef QUESTS_H
#define QUESTS_H

#include <QObject>
#include <QVector>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <QMap>
#include<QtSql>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlRecord>

class Quests : public QObject
{
    Q_OBJECT
public:
    explicit Quests(QObject *parent = nullptr);

    QSqlDatabase db = QSqlDatabase::database();


    static int PacketID_Check;
    static QMap<QString,QMap<QString,QMap<int,QVector<QString>>>> QGID_OP_Flag_Popup_9sVectorMaps;
    static QMap<QString,QMap<QString,QMap<int,QVector<QString>>>> QGID_OP_Flag_Popup_36sVectorMaps;
    static QMap<QString,QMap<QString,QMap<int,QVector<QString>>>> QGID_OP_Flag_CheckVectorMaps;
    static QMap<QString,QMap<int,QString>> QuestGiversID_Flag_MenusID_Map;
    static QMap<QString,QVector<QString>> SpiritmastersID_MenuMap;
    static int SpiritMastersPacketID;
    static QMap<QString,QMap<int,QString>> SpiritMastersID_PacketNumber_LocationMaps;
    static QMap<QString,QMap<int,QString>> LocationFlag_Menu_PopUpIDsMap;
    static QMap<QString,QVector<QString>> NPCsID_QuestIDsVectorMap;
    static QMap<QString,QMap<QVector<int>,QString>> LocationQuestID_QuestVectorsMap;
    static QMap<QString,QMap<QString,QVector<QString>>> QuestGiversID_Level_LocationClassRaceMap;
    static QMap<QString,QVector<QString>> PopUpID_PopUpResltsVectorMap;
    static QString QuestsID;
    static int PacketID_Quests;
    static QVector<QString> MenuTextCheckVector;
    static int FakeFlag;
    static QMap<QString,QVector<QString>> QuestsIDs_ResultsVectorMap;
    static QMap<QString,QMap<QString,QString>> PlayersID_QuestLogSlot_QuestID_MapOfMaps;
    static QMap<int,QVector<QString>> QuestValuesVectorMap;
    static QMap <QString,int> QuestGiversNames_IDS_Map;
    static QMap<QString,QString> QuestGiversID_NameMap;
    static QString Decription_ID;
    static int PacketID_Decription;
    static QMap<QString,QString> NPCsName_ZoneMap;
    static QString Logs_ID;
    static int PacketID_Logs;
    static QMap<QString,QString> LogTextID_Text;
    static QMap<QString,QString> DescriptionTextID_Text;
    static QString Text_ID;
    static QString Menu_ID;
    static QString Menu_Option_ID;
    static QString Popup_ID;
    static int PacketID_Text;
    static int PacketID_Menu;
    static int PacketID_Menu_Option;
    static int PacketID_Popup;
    static int PacketID_FlagMenuPopUpIDs;
    static int Quest_Givers_PacketID;
    static int PRE_REQS_PacketID;
    static QMap<QString,QString> MenuID_MenuText_Map;
    static QMap<QString,QString> OptionID_OptionText_Map;
    static QMap<QString,QString> PopUpID_PopUpText_Map;
    static QMap<QString,QVector<QString>> InGame_MenuesVectorMap;
    static QMap<QString,QMap<QString,QVector<QString>>> MenuID_OptionsIDs_ResultsVectorMap;
    static bool Quest;
    static QMap<QByteArray,QVector <QByteArray>> NPC_Quests_IDs;
    static QMap <int,QVector <QString>> light_blue_map;




    QMap <QString,QString> ClassValueToString =
    {
        {"00","WAR"},{"01","RAN"},{"02","PAL"},{"03","SK"},
        {"04","MNK"},{"05","BRD"},{"06","RGE"},{"07","DRD"},
        {"08","SHA"},{"09","CL"},{"0A","MAG"},{"0B","NEC"},
        {"0C","ENC"},{"0D","WIZ"},{"0E","ALC"}
    };

    QMap <QString,QString> RaceValueToString =
    {
        {"00","HUM"},{"01","ELF"},{"02","DELF"},
        {"03","GNO"},{"04","DWF"},{"05","TRL"},
        {"06","BAR"},{"07","HLF"},{"08","ERU"},
        {"09","OGR"}
    };

    QMap<QString,QVector<QString>> FreeportHumanClassGreetings_VectorMap =
    {
        {"ALC",{"alchemist","Academy of Arcane Science","Dilina McNerian","Ashlan","she"}},
        {"BRD",{"bard","Silken Gauntlet","William Corufost","Imaryn","he"}},
        {"CL",{"cleric","Shining Shield Mercenaries","Denouncer Alshea","Keika","she"}},
        {"ENC",{"enchanter","Academy of Arcane Science","Azlynn","Ashlan","he"}},
        {"MAG",{"magician","Academy of Arcane Science","Malsis","he","Alshan"}},
        {"NEC",{"necromancer","House Slaerin","Corious Slaerin","Keika","he"}},
        {"RGE",{"rogue","Shining Shield Mercenaries","Necorik the Ghost","Keika","he"}},
        {"SK",{"shadowknight","Shining Shield Mercenaries","Malethai Crimsonhand","Keika","he"}},
        {"WAR",{"warrior","Freeport Militia","Commander Nothard","Zole","he"}},
        {"WIZ",{"wizard","Academy of Arcane Science","Sivrendesh","Ashlan","he"}}
    };

    QMap<QString,QVector<QString>> QeynosHumanClassGreetings_VectorMap =
    {
        {"ALC",{"alchemist","Anagogical Order","Jergish Anaebarum","Decia","he"}},
        {"BRD",{"bard","Qeynos Troupe","Thrush Baird","Jolan","he"}},
        {"CL",{"cleric","Defenders of Erollisi Marr","Vedilion Brithstar","Kloe","he"}},
        {"ENC",{"enchanter","Anagogical Order","Shiassa Radian","Jall","she"}},
        {"MAG",{"magician","Anagogical Order","Ammathor Lithkin","Jall","he"}},
        {"MNK",{"monk","Order of the Silent Fist","Master T'an Chen","Lo","he"}},
        {"PAL",{"paladin","Temple of Mithaniel Marr","Sir Hanst Breach","Kloe","he"}},
        {"RGE",{"rogue","Beggar's Quarter","Snyde Cragsmear","Jolan","he"}},
        {"WAR",{"warrior","Qeynos Guard House","Roger Stoutheart","Ferd","he"}},
        {"WIZ",{"wizard","Anagogical Order","Gadenon Flamefist","Jall","he"}}
    };

    QMap<QString,QVector<QString>> SurefallGladeHumanClassGreetings_VectorMap =
    {
        {"DRD",{"druid","Jaggedpine Treefolk","Shiol'Anara","Terent","she"}},
        {"RAN",{"ranger","Protectors of the Pine","Sigmor Fallbourne","Barkwillow","he"}}
    };

    QMap<QString,QVector<QString>> FayspiresELFClassGreetings_VectorMap =
    {
        {"ALC",{"alchemist","College of High Magic","Silnea Aesiowe","Elinos","she"}},
        {"CL",{"cleric","Church of Tunare","Tessarina Starshimmer","Muselender","she"}},
        {"ENC",{"enchanter","College of High Magic","Casalandria Lyssia","Elinos","she"}},
        {"MAG",{"magician","College of High Magic","Fethinar Silspin","Elinos","he"}},
        {"PAL",{"paladin","Paladins of Tunare","Sir Lothwin Galiel","Tintal","he"}},
        {"WIZ",{"wizard","College of High Magic","Lyriam Kaelean","Elinos","he"}}
    };

    QMap<QString,QVector<QString>> TethelinELFClassGreetings_VectorMap =
    {
        {"BRD",{"bard","Songweavers of Tunare","Torenia Eaglesong","Tiladinya","she"}},
        {"DRD",{"druid","Keepers of the Glade","Dawnseer Mistwelder","Alyrik","he"}},
        {"RAN",{"ranger","Scouts of Tunare","Lythen Trueshot","Perfinoth","he"}},
        {"RGE",{"rogue","Scouts of Tunare","Eterin Nitegazer","Marinal","he"}}
    };

    QMap<QString,QVector<QString>> NeriakDarkElfClassGreetings_VectorMap =
    {
        {"ALC",{"alchemist","Spurned","Raemiss" "D'Mariji","Z'Tottl","he"}},
        {"CL",{"cleric","Church of Innoruuk","Theologist S'Tai","Sharun","he"}},
        {"ENC",{"enchanter","Spurned","Dominary K'Jartan","Rendor","he"}},
        {"MAG",{"magician","Spurned","Elementalist R'Virr","Rendor","he"}},
        {"NEC",{"necromancer","Spurned","Grand Defiler J'Narus","Irios","he"}},
        {"RGE",{"rogue","Ebon Mask","Kriyn","Xoln","she"}},
        {"SK",{"shadowknight","Lodge of the Dead","Zethril Do'Vexis","Irios","he"}},
        {"WAR",{"warrior","Indigo Brotherhood","Scornmaster U'Dedne","Malka","he"}},
        {"WIZ",{"wizard","Spurned","Sorceress X'Lottl","Rendor","she"}},
    };

    QMap<QString,QVector<QString>> KlickAnonGnomeGreetings_VectorMap =
    {
        {"ALC",{"alchemist","Mechanamagical College","Lanlin Ogledmaggen","Savi","he"}},
        {"CL",{"cleric","Temple of Brell","Teka Harnswoof","Bage","she"}},
        {"ENC",{"enchanter","Mechanamagical College","Professor Temwiddle","Savi","she"}},
        {"MAG",{"magician","Mechanamagical College","Werlib Quackook","Lacy","he"}},
        {"NEC",{"necromancer","Mechanamagical College","Grear Hosrottle","Zake","he"}},
        {"RGE",{"rogue","Junk Mongers","Mistress Briva","Ula","she"}},
        {"WAR",{"warrior","Klick'Anon Watch","Captain Buntattle","Kettlemaw","he"}},
        {"WIZ",{"wizard","Mechanamagical College","Pazelfun Pansoof","Hansel","he"}},
    };

    QMap<QString,QVector<QString>> MoradhimDwarfGreetings_VectorMap =
    {
        {"CL",{"cleric","Church of Below","Bishop Gundar","Bico","he"}},
        {"PAL",{"paladin","Doomseekers","Ungrist the Prophet","Bico","he"}},
        {"RGE",{"rogue","Miners Guild 231","Foreman Druza","Lesha","she"}},
        {"WAR",{"warrior","Stone Guard","Field General Oxfist","Codwin","he"}},
    };

    QMap<QString,QVector<QString>> GrobbTrollGreetings_VectorMap =
    {
        {"SK",{"shadowknight","Shadowknights of Nightkeep","Underlord Solthe","Griffnik","he"}},
        {"SHA",{"shaman","Dark Ones","Hierophant Koligo","Grueldor","he"}},
        {"WAR",{"warrior","Da Bashers","Warlord Jurglash","Gugzug","he"}},
    };

    QMap<QString,QVector<QString>> HalasBarbarianGreetings_VectorMap =
    {
        {"RGE",{"rogue","Eyes of the Tribunal","Juno Felligan","Dean","she"}},
        {"SHA",{"shaman","Seers of the Tribunal","Beril O'Leary","Lera","he"}},
        {"WAR",{"warrior","Wolves of the North","Marik McPherson","Kerr","he"}},
    };

    QMap<QString,QVector<QString>> RivervaleHalflingGreetings_VectorMap =
    {
        {"CL",{"cleric","Temple of Bristlebane","Pora Meepup","Lily","she"}},
        {"DRD",{"druid","Stormreapers","Deke Gabbins","Thom","he"}},
        {"RGE",{"rogue","Deep Pockets","Walt Deeppockets","Willa","he"}},
        {"WAR",{"warrior","Guardians of the Vale","Sheriff Hopper","Rory","he"}},
    };

    QMap<QString,QVector<QString>> HighbourneEruditeGreetings_VectorMap =
    {
        {"ALC",{"alchemist","Alchemist's Guild","Melina Quiscellin","Alcadia","she"}},
        {"CL",{"cleric","Hand of Quellious","Arch Bishop Erah","Helshah","he"}},
        {"ENC",{"enchanter","Craft Keepers","Master Delar","Keyston","he"}},
        {"MAG",{"magician","Gate Callers","Master Veljhan","Anja","he"}},
        {"NEC",{"necromancer","Hidden","Retainer Alishai","Phendar","he"}},
        {"PAL",{"paladin","Peacekeepers","Zulan Sunshield","Helshah","she"}},
        {"SK",{"shadowknight","Hidden","Desh the Harvester","Phendar","he"}},
        {"WIZ",{"wizard","Crimson Hand","Master Rayne","Kaolo","she"}},
    };

    QMap<QString,QVector<QString>> OggokOgreGreetings_VectorMap =
    {
        {"NEC",{"necromancer","Temple of Greenblood","Asogwe Greth","Korg","she"}},
        {"SK",{"shadowknight","Temple of Greenblood","Greenblood Yurgat","Korg","he"}},
        {"SHA",{"shaman","Chosen of Gunthak","Gunthak","Zarg","he"}},
        {"WAR",{"warrior","Craknek Warriors","Warlord Brogar","Yerf","he"}},
    };

    QMap<QString,QMap<QString,QVector<QString>>> StartingCity_WorldGreetings_VectorMap =
    {
        {"Freeport",{FreeportHumanClassGreetings_VectorMap}},
        {"Qeynos",{QeynosHumanClassGreetings_VectorMap}},
        {"Surefall Glade",{SurefallGladeHumanClassGreetings_VectorMap}},
        {"Fayspires",{FayspiresELFClassGreetings_VectorMap}},
        {"Tethelin",{TethelinELFClassGreetings_VectorMap}},
        {"Neriak",{NeriakDarkElfClassGreetings_VectorMap}},
        {"Klick'Anon",{KlickAnonGnomeGreetings_VectorMap}},
        {"Moradhim",{MoradhimDwarfGreetings_VectorMap}},
        {"Grobb",{GrobbTrollGreetings_VectorMap}},
        {"Halas",{HalasBarbarianGreetings_VectorMap}},
        {"Rivervale",{RivervaleHalflingGreetings_VectorMap}},
        {"Highbourne",{HighbourneEruditeGreetings_VectorMap}},
        {"Oggok",{OggokOgreGreetings_VectorMap}}
    };

    QVector<QByteArray> SendPacketOut;

    QMap <int,int> XPtoNextLevelMap =
    {
        {1,420},{2,2185},{3,6975},{4,17125},{5,35640},
        {6,66195},{7,113150},{8,181520},{9,277000},{10,405955},
        {11,575420},{12,793115},{13,1067415},{14,1407375},{15,1822720},
        {16,2323845},{17,2921830},{18,3628410},{19,4456000},{20,5417685},
        {21,6527220},{22,7799045},{23,9248255},{24,10890625},{25,12742600},
        {26,14821295},{27,17144510},{28,19730700},{29,22599000},{30,25769215},
        {31,29261820},{32,33097975},{33,37299495},{34,41888875},{35,46889280},
        {36,52324545},{37,58219190},{38,64598390},{39,71488000},{40,78914545},
        {41,86905220},{42,95487905},{43,104691135},{44,114544125},{45,125076760},
        {46,136319595},{47,148303870},{48,161061480},{49,174625000},{50,189027675},
        {51,204303420},{52,220486835},{53,237613175},{54,255718375},{55,274839040},
        {56,295012445},{57,316276550},{58,338669970},{59,362232000}
    };

    QVector <QString> ToltalXPperTPsVector =
    {

        "140","280","420",
        "1148","1876","2605",
        "4930","7255","9580",
        "15288","20996","26705",
        "38585","50465","62345",
        "84410","106475","128540",
        "166256","203972","241690",
        "302196","362702","423210",
        "515543","607876","700210",
        "835528","970846","1106165",
        "1297971","1489777","1681585",//check level 11 to see when you ding from 10
        "1945956","2210327","2474700",
        "2830505","3186310","3542115",
        "4011240","4480365","4949490",

        //5 tps per level
        "5314034","5678578","6043122","6407666","6772210",
        "7236979","7701748","8166517","8631286","9096055",
        "9680421","10264787","10849153","11433519","12017885",
        "12743567","13469249","14194931","14920613","15646295",
        "16537495","17428695","18319895","19211095","20102295",
        "21185832","22269369","23352906","24436443","25519980",
        "26825424","28130868","29436312","30741756","32047200",
        "33607009","35166818","36726627","38286436","39846245",
        "41695896","43545547","45395198","47244849","49094500",
        "51272625","53450750","55628875","57807000","59985125",
        "62533645","65082165","67630685","70179205","72727725",
        "75691984","78656243","81620502","84584761","87549020",
        "90977922","94406824","97835726","101264628","104693530",
        "108639670","112585810","116531950","120478090","124424230",
        "128944030","133463830","137983630","142503430","147023230",

        //9 tps per level
        "149886476","152749722","155612968","158476214","161339460","164202706","167065952","169929198","172792445",
        "176043758","179295071","182546384","185797697","189049010","192300323","195551636","198802949","202054265",
        "205731817","209409369","213086921","216764473","220442025","224119577","227797129","231474681","235152240",
        "239296628","243441016","247585404","251729792","255874180","260018568","264162956","268307344","272451735",
        "277106054","281760373","286414692","291069011","295723330","300377649","305031968","309686287","314340610",
        "319550530","324760450","329970370","335180290","340390210","345600130","350810050","356019970","361229890",
        "367043728","372857566","378671404","384485242","390299080","396112918","401926756","407740594","413554435",
        "420023233","426492031","432960829","439429627","445898425","452367223","458836021","465304819","471773625",
        "478951223","486128821","493306419","500484017","507661615","514839213","522016811","529194409","536372015",
        "544315126","552258237","560201348","568144459","576087570","584030681","591973792","599916903","607860015",
        "616628297","625396579","634164861","642933143","651701425","660469707","669237989","678006271","686774560",
        "696430695","706086830","715742965","725399100","735055235","744711370","754367505","764023640","773679780",
        "784289547","794899314","805509081","816118848","826728615","837338382","847948149","858557916","869167685",
        "880800033","892432381","904064729","915697077","927329425","938961773","950594121","962226469","973858820",
        "986585945","999313070","1012040195","1024767320","1037494445","1050221570","1062948695","1075675820","1088402945",

        //14 tps per level
        "1097336999","1106271053","1115205107","1124139161","1133073215","1142007269","1150941323","1159875377","1168809431","1177743485","1186677539","1195611593","1204545647","1213479705",
        "1223216818","1232953931","1242691044","1252428157","1262165270","1271902383","1281639496","1291376609","1301113722","1310850835","1320587948","1330325061","1340062174","1349799300",
        "1360392433","1370985566","1381578699","1392171832","1402764965","1413358098","1423951231","1434544364","1445137497","1455730630","1466323763","1476916896","1487510029","1498103170",
        "1509607561","1521111952","1532616343","1544120734","1555625125","1567129516","1578633907","1590138298","1601642689","1613147080","1624651471","1636155862","1647660253","1659164650",
        "1671637864","1684111078","1696584292","1709057506","1721530720","1734003934","1746477148","1758950362","1771423576","1783896790","1796370004","1808843218","1821316432","1833789650",
        "1847291626","1860793602","1874295578","1887797554","1901299530","1914801506","1928303482","1941805458","1955307434","1968809410","1982311386","1995813362","2009315338","2022817325",
        "2037410426","2052003527","2066596628","2081189729","2095782830","2110375931","2124969032","2139562133","2154155234","2168748335","2183341436","2197934537","2212527638","2227120745",
        "2242869804","2258618863","2274367922","2290116981","2305866040","2321615099","2337364158","2353113217","2368862276","2384611335","2400360394","2416109453","2431858512","2447607580",
        "2464579949","2481552318","2498524687","2515497056","2532469425","2549441794","2566414163","2583386532","2600358901","2617331270","2634303639","2651276008","2668248377","2685220755",
        "2703486353","2721751951","2740017549","2758283147","2776548745","2794814343","2813079941","2831345539","2849611137","2867876735","2886142333","2904407931","2922673529","2940939130",
        "2960570490","2980201850","2999833210","3019464570","3039095930","3058727290","3078358650","3097990010","3117621370","3137252730","3156884090","3176515450","3196146810","3215778170",
        "3236850487","3257922804","3278995121","3300067438","3321139755","3342212072","3363284389","3384356706","3405429023","3426501340","3447573657","3468645974","3489718291","3510790615",
        "3533381797","3555972979","3578564161","3601155343","3623746525","3646337707","3668928889","3691520071","3714111253","3736702435","3759293617","3781884799","3804475981","3827067165",
        "3851257877","3875448589","3899639301","3923830013","3948020725","3972211437","3996402149","4020592861","4044783573","4068974285","4093164997","4117355709","4141546421","4165737135",
        "4191610849","4217484563","4243358277","4269231991","4295105705","4320979419","4346853133","4372726847","4398600561","4424474275","4450347989","4476221703","4502095417","4527969135"

    };

    QVector <int> TPsPerToltalXPVector =
    {
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35,
        36, 37, 38,
        39, 40, 41,
        42, 43, 44,
        45, 46, 47,
        48, 49, 50,
        51, 52, 53,
        54, 55, 56,
        57, 58, 59,
        60, 61, 62,
        63, 64, 65, 66, 67,
        68, 69, 70, 71, 72,
        73, 74, 75, 76, 77,
        78, 79, 80, 81, 82,
        83, 84, 85, 86, 87,
        88, 89, 90, 91, 92,
        93, 94, 95, 96, 97,
        98, 99, 100, 101, 102,
        103, 104, 105, 106, 107,
        108, 109, 110, 111, 112,
        113, 114, 115, 116, 117,
        118, 119, 120, 121, 122,
        123, 124, 125, 126, 127,
        128, 129, 130, 131, 132,
        133, 134, 135, 136, 137,
        138, 139, 140, 141, 142, 143, 144, 145, 146,
        147, 148, 149, 150, 151, 152, 153, 154, 155,
        156, 157, 158, 159, 160, 161, 162, 163, 164,
        165, 166, 167, 168, 169, 170, 171, 172, 173,
        174, 175, 176, 177, 178, 179, 180, 181, 182,
        183, 184, 185, 186, 187, 188, 189, 190, 191,
        192, 193, 194, 195, 196, 197, 198, 199, 200,
        201, 202, 203, 204, 205, 206, 207, 208, 209,
        210, 211, 212, 213, 214, 215, 216, 217, 218,
        219, 220, 221, 222, 223, 224, 225, 226, 227,
        228, 229, 230, 231, 232, 233, 234, 235, 236,
        237, 238, 239, 240, 241, 242, 243, 244, 245,
        246, 247, 248, 249, 250, 251, 252, 253, 254,
        255, 256, 257, 258, 259, 260, 261, 262, 263,
        264, 265, 266, 267, 268, 269, 270, 271, 272,
        273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286,
        287, 288, 269, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300,
        301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314,
        315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328,
        329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342,
        343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356,
        357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370,
        371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384,
        385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398,
        399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412,
        413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426,
        427, 428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439, 440,
        441, 442, 443, 444, 445, 446, 447, 448, 449, 450, 451, 452, 453, 454,
        455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468,
        469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 481, 482
    };

    QVector<QString> PlaceHolderStuffVector =
    {
        "#Class#","#GuildsName#","#Guildmaster#","#SpiritMaster#","#Gender#","#StartingCity#"
    };

    const QByteArray New_NPC_c9 = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";



signals:

public slots:

    QByteArray SendoutSpell(QByteArray ID_IP_PORT,QString ResultID);
    QByteArray SendoutMenus(QByteArray ID_IP_PORT,QString ResultID);
    QByteArray SendPopUps(QByteArray ID_IP_PORT,QString ResultID);
    bool SendtoItems(QByteArray ID_IP_PORT,QString ResultID);
    QByteArray SendtoTunar(QByteArray ID_IP_PORT,QString ResultID);
    QByteArray GenerateXP(QByteArray ID_IP_PORT,QString ResultID);
    QByteArray RemoveQuestLog(QByteArray ID_IP_PORT,QString ResultID);
    QByteArray AddQuestLog(QByteArray ID_IP_PORT,QString ResultID);
    QVector<QString> CheckYoPockets(QByteArray ID_IP_PORT, QVector<QString> CheckVector);

    QByteArray QuestMenus(QByteArray ID_IP_PORT, QByteArray Clients_Last_MessageNumber, QByteArray Clients_Last_FB_MessageNumber, QByteArray opholder, QString QuestsID, QString NPCsID);
    QByteArray WelcomeGreetings(QByteArray ID_IP_PORT);
    QByteArray TestingOptions(QString Opcode, QString Text, QByteArray Servers_Last_FB_MessageNumber, QByteArray ID_IP_PORT);
    QByteArray MenuHeaderConverter(QString Text);
    QByteArray TestingMenus(QString Text);

private slots:
    void SendoutPacket(QByteArray ID_IP_PORT,QByteArray FullPacket);

};

#endif // QUESTS_H
