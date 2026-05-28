#ifndef PACKETPARSING_H
#define PACKETPARSING_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QStack>
#include <QByteArray>
#include <QPixmap>
#include <QFile>
#include <QFileDialog>
#include <QHostAddress>

class packetparsing : public QObject
{
    Q_OBJECT
public:
    explicit packetparsing(QObject *parent = nullptr);

    static bool SkipKey;



    struct packetvars
    {
        int MainFlag = -1;
        QString BindLocation = "";
        QString ModelResult = "";
        int ModelTimer = 0;
        int SpellCoolDownTimer = 0;
        int passdown = 9999;
        bool LootWindowChecked = false;
        QMap<QString,QVector<QString>> MobsID_LootVectorMap;
        QVector<QString> FakeLootVector;

        QMap<QString,QVector<QString>> MobsID_LootSlotVectorMap;
        QVector<QString> FakeLootSlotVector;

        QMap<QString,QMap<int,QString>> MobsID_Slot_ItemsID_MapOfMaps;

        QString LootWindowsID = "";

        bool CheckPocketsNow = false;
        QString QuestGiversID = "";
        QString CurrentQuestID = "";
        QString QuestsID = "";
        QMap<QString,QString> MenuID_QuestID;
        QMap<QString,QVector<QString>> QuestsID_MenusID_ItemsID_QuantityVectorMap;
        QMap<QString,QVector<QString>> QuestsID_MenusID_TunarQuantityVectorMap;


        QString tele_x = "";
        QString tele_z = "";
        QString tele_y = "";
        QString tele_world = "";
        QByteArray current_c9 = "";
        QString Face_Color_V = "0";
        QString Con_Color_V = "";
        QByteArray holder_Con = "";

        int MySwingNumber = 0;
        int MobSwingNumber = 0;
        int PrimaryDMG = 0;
        int SecondaryDMG = 0;
        int Randomizer1 = 0;
        int Randomizer2 = 0;
        int ExtraDamage = 0;
        float MyZoneCentralX = 0.0;
        float MyZoneCentralY = 0.0;

        QVector <QString> FBReplyOrNotVector;
        QVector <QString> FBPacketDescriptionVector;

        QVector<int> FactionValueVector;

        bool CheckingTheZone = true;
        QMap<QString,float> Temp_Distance_ID_Map;
        QStringList keptlist1;
        QVector <QString> DeadListVector;
        QMap <QString,QVector <QString>> DeadListVectorMap;
        QMap<QString,QString> sent_ids_map;

        QVector <QByteArray> SortChanelsNumbersVector =
        {
            "00","01","02","03","04","05","06","07","08","09",
            "0A","0B","0C","0D","0E","0F","10","11","12",
            "13","14","15","16","17","40","42","43"
        };

        QVector <QByteArray> SortChanelsValuesVector =
        {
            "0000","0000","0000","0000","0000","0000","0000","0000","0000","0000",
            "0000","0000","0000","0000","0000","0000","0000","0000","0000",
            "0000","0000","0000","0000","0000","0000","0000","0000"
        };

        QMap<QString,QString> channelmap = {{"01","free"},{"02","free"},{"03","free"},{"04","free"},
                                            {"05","free"},{"06","free"},{"07","free"},{"08","free"},
                                            {"09","free"},{"0A","free"},{"0B","free"},{"0C","free"},
                                            {"0D","free"},{"0E","free"},{"0F","free"},{"10","free"},
                                            {"11","free"},{"12","free"},{"13","free"},{"14","free"},
                                            {"15","free"},{"16","free"},{"17","free"}};

        QMap<QString,QByteArray> incrementmap = {{"00","0000"},{"01","0000"},{"02","0000"},{"03","0000"},{"04","0000"},
                                                 {"05","0000"},{"06","0000"},{"07","0000"},{"08","0000"},
                                                 {"09","0000"},{"0A","0000"},{"0B","0000"},{"0C","0000"},
                                                 {"0D","0000"},{"0E","0000"},{"0F","0000"},{"10","0000"},
                                                 {"11","0000"},{"12","0000"},{"13","0000"},{"14","0000"},
                                                 {"15","0000"},{"16","0000"},{"17","0000"},
                                                 {"42","0000"},{"43","0000"}};
        QMap<QString,QString> DespawnDelete_Map;

        QMap<QString,QString> Map_Channel_ID =
        {
            {"01","00000000"},{"02","00000000"},{"03","00000000"},{"04","00000000"},{"05","00000000"},{"06","00000000"},
            {"07","00000000"},{"08","00000000"},{"09","00000000"},{"0A","00000000"},{"0B","00000000"},{"0C","00000000"},
            {"0D","00000000"},{"0E","00000000"},{"0F","00000000"},{"10","00000000"},{"11","00000000"},{"12","00000000"},
            {"13","00000000"},{"14","00000000"},{"15","00000000"},{"16","00000000"},{"17","00000000"}};

        QMap<QString,float> Map_Channel_Distance =
        {{"01",0},{"02",0},{"03",0},{"04",0},{"05",0},{"06",0},
         {"07",0},{"08",0},{"09",0},{"0A",0},{"0B",0},{"0C",0},
         {"0D",0},{"0E",0},{"0F",0},{"10",0},{"11",0},{"12",0},
         {"13",0},{"14",0},{"15",0},{"16",0},{"17",0}};

        QList<float> distancelist1;



        QStringList comparelist1;
        QStringList extrafilteredlist;
        QStringList sendingchannels;
        QVector <float> filteredset1;
        QMap <QString,float>  map1;

        QMap <QByteArray,QByteArray> Master_Map;
        QStringList  holdx;
        QStringList  holdy;
        QStringList  holdz;
        QStringList  holdid1;

        QStringList  holdx2;
        QStringList  holdy2;
        QStringList  holdz2;
        QStringList  holdid2;



        QByteArray ChatMode = "Say";
        QByteArray ReplyConfirm = "00000000";
        QByteArray LastReplyConfirm = "xxxxxxxx";

        QByteArray OLD_Servers_Last_MessageNumber = "0000";
        QByteArray FBMessageNumber = "";
        QByteArray opcoderesult1 = "";
        QByteArray FullFB = "";
        QByteArray TrimTheGamebuf1 = "";
        QMap<QByteArray, QVector<QByteArray>> OpcodesAndGamebuf1Map;
        QByteArray Clients_Last_MessageNumber = "0000";
        QByteArray Clients_Last_FB_MessageNumber = "0000";
        bool DoubleSession = false;
        bool flipped = false;
        bool Flip163 = false;
        bool PreCharSelect = true;
        bool Server_2A00_reply = false;
        QByteArray gamebuf1 = "";
        QByteArray CharID_5bytes = "";
        QByteArray Xor_Mob_movement2 = "";
        QByteArray Mob_ALL_Values2 = "";
        QByteArray LoS_ALL_Values2 = "";
        QByteArray Xor_LoS_movement2 = "";
        qlonglong LoS_X = 0;
        qlonglong LoS_Z = 0;
        qlonglong LoS_Y = 0;
        QString LoS_Counter_X = "";
        QString LoS_Counter_Z = "";
        QString LoS_Counter_Y = "";
        int indexOf4029 = 0;
        bool isitOpen = false;
        QMap<QString,bool> OpenLootWindowsMap;
        QByteArray ClientsCheckChannelConfirm = "";
        QByteArray ServersCheckChannelConfirm = "";
        bool RemoveResendChannels = false;
        bool StopSending = false;
        int resendingChannelCounter = 0;
        bool ConfirmRemove = false;
        int F901_Counter = 0;
        int F707_Counter = 0;
        int F807_Counter = 0;
        int HeartBeat_Counter = 0;
        QByteArray CheckCentral = "YES";
        QByteArray CheckNorth = "YES";
        QByteArray CheckEast = "YES";
        QByteArray CheckWest = "YES";
        QByteArray CheckSouth = "YES";
        float ZoneX = 0;
        float ZoneY = 0;
        int zoneInt = 0;
        float ZoneXh = 0;
        float ZoneYh = 0;
        float Xh = 0;
        float Zh = 0;
        float Yh = 0;
        QString Zone_FileName = "";
        QString Zone_FileNameh = "";
        QString LastZone = "";
        QString MyZone = "CheckTheZone";
        QString MyZoneh = "";
        QString MyZoneCentral = "";
        int LastWorld = 99;
        int LastTeleportWorld = 0;
        bool Opcode4029 = false;
        QByteArray Reply = "";
        float MySwingRadius = 0.0;
        float MobSwingRadius = 0.0;
        bool FullHeath = false;

        bool WorldGreetingFlag = false;
        int FirstLogin = 0;
        int MainQuestFlag = 0;
        bool Quest = false;
        QString QuestID = "";

        QString QuestPopUpID = "";
        QByteArray QuestMenuCounter = "0000";
        QByteArray QuestPopUpCounter = "0000";

        QByteArray HPLowOrPOWLow = "";
        QByteArray LastServerOpcode = "";
        QVector<int> SortServers_Last_MessageNumber1;
        QVector<int> SortServers_Last_MessageNumber2;
        QByteArray Clients42EC = "0200";
        QByteArray ClientsLastFBConfirm = "";
        QByteArray ClientLastFB = "";
        QByteArray Target_Counter_01 = "";
        QString RegenerateHP = "";
        QString RegeneratePOW = "";
        int XPperKill = 0;
        int XPNextLevel = 420;
        int TotalXp = 0;
        int XPBar = 0;
        int Level = 1;
        QString CurrentLevel = "";
        QString currentXP = "00";
        QByteArray Channel42messnum = "0100";
        QByteArray LoginStarted = "";

        QByteArray ringbuffer1 = "";
        QStack <int> X_stack;
        QStack <int> Y_stack;
        QStack <int> Z_stack;
        QStack <int> F_stack;
        QStack <int> T_stack;
        QStack <int> A_stack;
        QStack <int> C_stack;
        QStack <int> WE_stack;
        QStack <int> NS_stack;

        bool loader275 = false;
        QByteArray oldclientval = "";
        int packetid = 0;
        QByteArray CurrentCombat = "00";
        QByteArray Servers_Last_MessageNumber1 = "0200";
        QByteArray Servers_Last_MessageNumber2 = "0200";
        QByteArray masterFBmess = "0300";
        QByteArray Client_Mess = "0100";
        QByteArray Client_FB_Mess = "0100";
        QByteArray Old_session = "";
        QByteArray Next4029 = "0100";
        int Counter4029 = 0;
        QByteArray AtackType = "0000";
        int TrueReply = 0;
        int F8Reply = 0;
        int FC02Counter = 0;
        int MySwing = 4;
        int MySwingType = 4;
        bool allowAggro = false;
        QByteArray returnedcolor = "";
        QString IsTargetGreen = "NO";
        QByteArray InventoryIDCounter = "1B00";
        QByteArray BankIDCounter = "2300";
        int InventoryMAX = 28;
        int BankMAX = 36;
        QByteArray clientid1 = "";

        QByteArray ClientsID = "";
        QByteArray ClientsIP = "";
        QByteArray ClientsPort = "";
        QByteArray teleport_ip = "";




        QVector <QByteArray> OpcodeReturn;
        QVector <QByteArray> Returnflipped;
        QVector<QByteArray> ActivateOP;

        QVector <QByteArray> Return4029;
        QVector <int> XandYvector;

        QByteArray serverid1 = "";
        QByteArray dropped1 = "";
        int UnSpentTPs = 0;
        int SpentTPs = 0;
        QString StartSpentTPs = "";
        QString StartUnSpentTPs = "";
        QString StartXPbar = "";
        QString StartTotalXP = "";
        QString StartLevel = "";
        QString StartCMxp = "";
        QString StartTotalCMxp = "";
        QString StartUnSpentCMs = "";
        QString StartSpentCMs = "";
        QString UnSpentCMpoints = "";
        QString SpentCMpoints = "";
        QString CharSelectID = "";
        QMap <QString, QVector <QString>> CombatAllValuesMap;
        bool boon = false;
        QByteArray Target_ID = "";
        QByteArray OldTarget_ID = "";
        QString coachtext1 = "";
        QByteArray coach_counter = "0000";
        QByteArray coachreply_counter = "0000";
        QByteArray teleport_destination = "";
        QByteArray masterx = "";
        QByteArray mastery = "";
        QByteArray masterz = "";
        QByteArray masterf = "";
        QByteArray masterWE = "";
        QByteArray masterNS = "";
        QString LastCentralZone = "";
        QString CurrentCentralZone = "CheckTheZoneHeight";
        QString HoldOldHP = "";
        QList <QByteArray> mylist;
        QVector <QString> merchant_vector;


        bool teleportmainswitch = false;

        QByteArray Charm_Lock = "00";
        QByteArray Facing_Lock = "00";
        QByteArray Facing_Lock2 = "00";
        QByteArray Dismiss_Lock = "00";
        QByteArray Spellbinder_counter = "0000";
        QByteArray Pet_Trip = "00";
        QByteArray Pet_Trip2 = "00";
        QByteArray Cant_Charm = "00";
        QByteArray Self_Target = "00";
        QByteArray Out_of_DistanceRange = "00";
        QByteArray Out_of_LevelRange = "00";
        QByteArray Pet_Attack = "00";
        QByteArray Charm_recovery = "00";
        QByteArray REcharm = "00";
        QByteArray Out_Matched = "00";
        QByteArray Remove_This_NPC = "";

        QByteArray combine1 = "";

        bool FC02D007 = false;

        QByteArray StatsChange = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        QByteArray fortytwentninenum1 = "0000";
        QByteArray myAnimation = "";
        bool fortyexist = false;
        QByteArray CheckTheAggro = "YES";
        QByteArray decopacket1 = "";

        QString Pet_ID = "";
        QString Pet_Chan = "";

        QByteArray Pet_Follow = "";
        QByteArray PetConOld = "";
        QByteArray PET_Exists = "";



        QVector <QByteArray> SendFBsVector;
        QByteArray zerocounter = "";

        QMap <QByteArray,QByteArray> ResendFBsMap;
        QMap <QByteArray,QByteArray> Resend42ecsMap;
        QMap <QByteArray,QByteArray> ResendAllChannelsMap;

        bool teleporting = false;
        QByteArray C_9007_endcount = "0200";

        bool opcodeexist = false;
        int currentxor = 0;

        QVector <QByteArray> Send42ECsVector;
        QVector <QByteArray> SendChannelsVector;
        QVector <QByteArray> SendZeroChannelVector;
        QVector <QByteArray> Send01ChannelVector;
        QVector <QByteArray> Send02ChannelVector;
        QVector <QByteArray> Send03ChannelVector;
        QVector <QByteArray> Send04ChannelVector;
        QVector <QByteArray> Send05ChannelVector;
        QVector <QByteArray> Send06ChannelVector;
        QVector <QByteArray> Send07ChannelVector;
        QVector <QByteArray> Send08ChannelVector;
        QVector <QByteArray> Send09ChannelVector;
        QVector <QByteArray> Send0AChannelVector;
        QVector <QByteArray> Send0BChannelVector;
        QVector <QByteArray> Send0CChannelVector;
        QVector <QByteArray> Send0DChannelVector;
        QVector <QByteArray> Send0EChannelVector;
        QVector <QByteArray> Send0FChannelVector;
        QVector <QByteArray> Send10ChannelVector;
        QVector <QByteArray> Send11ChannelVector;
        QVector <QByteArray> Send12ChannelVector;
        QVector <QByteArray> Send13ChannelVector;
        QVector <QByteArray> Send14ChannelVector;
        QVector <QByteArray> Send15ChannelVector;
        QVector <QByteArray> Send16ChannelVector;
        QVector <QByteArray> Send17ChannelVector;
        QVector <QByteArray> AllReplyOrNotVector;

        QMap <QString, QString> facing_map =
        {{"00","00000000"},
         {"01","DB0FC93C"},{"02","DB0F493D"},{"03","E4CB963D"},{"04","DB0FC93D"},{"05","D153FB3D"},
         {"06","E4CB163E"},{"07","DFED2F3E"},{"08","DB0F493E"},{"09","D631623E"},{"0A","D1537B3E"},
         {"0B","E63A8A3E"},{"0C","E4CB963E"},{"0D","E15CA33E"},{"0E","DFEDAF3E"},{"0F","DD7EBC3E"},
         {"10","DB0FC93E"},{"11","D8A0D53E"},{"12","D631E23E"},{"13","D4C2EE3E"},{"14","D153FB3E"},
         {"15","67F2033F"},{"16","E63A0A3F"},{"17","6583103F"},{"18","E4CB163F"},{"19","63141D3F"},
         {"1A","E15C233F"},{"1B","60A5293F"},{"1C","DFED2F3F"},{"1D","5E36363F"},{"1E","DD7E3C3F"},
         {"1F","5CC7423F"},{"20","DB0F493F"},{"21","59584F3F"},{"22","D8A0553F"},{"23","57E95B3F"},
         {"24","D631623F"},{"25","557A683F"},{"26","D4C26E3F"},{"27","520B753F"},{"28","D1537B3F"},
         {"29","28CE803F"},{"2A","67F2833F"},{"2B","A716873F"},{"2C","E63A8A3F"},{"2D","255F8D3F"},
         {"2E","6583903F"},{"2F","A4A7933F"},{"30","E4CB963F"},{"31","23F0993F"},{"32","63149D3F"},
         {"33","A238A03F"},{"34","E15CA33F"},{"35","2181A63F"},{"36","60A5A93F"},{"37","A0C9AC3F"},
         {"38","DFEDAF3F"},{"39","1F12B33F"},{"3A","5E36B63F"},{"3B","9D5AB93F"},{"3C","DD7EBC3F"},
         {"3D","1CA3BF3F"},{"3E","5CC7C23F"},{"3F","9BEBC53F"},{"40","DB0FC93F"},{"41","1A34CC3F"},
         {"42","5958CF3F"},{"43","997CD23F"},{"44","D8A0D53F"},{"45","18C5D83F"},{"46","57E9DB3F"},
         {"47","960DDF3F"},{"48","D631E23F"},{"49","1556E53F"},{"4A","557AE83F"},{"4B","949EEB3F"},
         {"4C","D4C2EE3F"},{"4D","13E7F13F"},{"4E","520BF53F"},{"4F","922FF83F"},{"50","D153FB3F"},
         {"51","1178FE3F"},{"52","28CE0040"},{"53","48600240"},{"54","67F20340"},{"55","87840540"},
         {"56","A7160740"},{"57","C6A80840"},{"58","E63A0A40"},{"59","06CD0B40"},{"5A","255F0D40"},
         {"5B","45F10E40"},{"5C","65831040"},{"5D","85151240"},{"5E","A4A71340"},{"5F","C4391540"},
         {"60","E4CB1640"},{"61","035E1840"},{"62","23F01940"},{"63","43821B40"},{"64","63141D40"},
         {"65","82A61E40"},{"66","A2382040"},{"67","C2CA2140"},{"68","E15C2340"},{"69","01EF2440"},
         {"6A","21812640"},{"6B","41132840"},{"6C","60A52940"},{"6D","80372B40"},{"6E","A0C92C40"},
         {"6F","BF5B2E40"},{"70","DFED2F40"},{"71","FF7F3140"},{"72","1F123340"},{"73","3EA43440"},
         {"74","5E363640"},{"75","7EC83740"},{"76","9D5A3940"},{"77","BDEC3A40"},{"78","DD7E3C40"},
         {"79","FD103E40"},{"7A","1CA33F40"},{"7B","3C354140"},{"7C","5CC74240"},{"7D","7B594440"},
         {"7E","9BEB4540"},{"7F","BB7D4740"},{"80","DB0F49C0"},{"81","BB7D47C0"},{"82","9BEB45C0"},
         {"83","7B5944C0"},{"84","5CC742C0"},{"85","3C3541C0"},{"86","1CA33FC0"},{"87","FD103EC0"},
         {"88","DD7E3CC0"},{"89","BDEC3AC0"},{"8A","9D5A39C0"},{"8B","7EC837C0"},{"8C","5E3636C0"},
         {"8D","3EA434C0"},{"8E","1F1233C0"},{"8F","FF7F31C0"},{"90","DFED2FC0"},{"91","BF5B2EC0"},
         {"92","A0C92CC0"},{"93","80372BC0"},{"94","60A529C0"},{"95","411328C0"},{"96","218126C0"},
         {"97","01EF24C0"},{"98","E15C23C0"},{"99","C2CA21C0"},{"9A","A23820C0"},{"9B","82A61EC0"},
         {"9C","63141DC0"},{"9D","43821BC0"},{"9E","23F019C0"},{"9F","035E18C0"},{"A0","E4CB16C0"},
         {"A1","C43915C0"},{"A2","A4A713C0"},{"A3","851512C0"},{"A4","658310C0"},{"A5","45F10EC0"},
         {"A6","255F0DC0"},{"A7","06CD0BC0"},{"A8","E63A0AC0"},{"A9","C6A808C0"},{"AA","A71607C0"},
         {"AB","878405C0"},{"AC","67F203C0"},{"AD","486002C0"},{"AE","28CE00C0"},{"AF","1178FEBF"},
         {"B0","D153FBBF"},{"B1","922FF8BF"},{"B2","520BF5BF"},{"B3","13E7F1BF"},{"B4","D4C2EEBF"},
         {"B5","949EEBBF"},{"B6","557AE8BF"},{"B7","1556E5BF"},{"B8","D631E2BF"},{"B9","960DDFBF"},
         {"BA","57E9DBBF"},{"BB","18C5D8BF"},{"BC","D8A0D5BF"},{"BD","997CD2BF"},{"BE","5958CFBF"},
         {"BF","1A34CCBF"},{"C0","DB0FC9BF"},{"C1","9BEBC5BF"},{"C2","5CC7C2BF"},{"C3","1CA3BFBF"},
         {"C4","DD7EBCBF"},{"C5","9D5AB9BF"},{"C6","5E36B6BF"},{"C7","1F12B3BF"},{"C8","DFEDAFBF"},
         {"C9","A0C9ACBF"},{"CA","60A5A9BF"},{"CB","2181A6BF"},{"CC","E15CA3BF"},{"CD","A238A0BF"},
         {"CE","63149DBF"},{"CF","23F099BF"},{"D0","E4CB96BF"},{"D1","A4A793BF"},{"D2","658390BF"},
         {"D3","255F8DBF"},{"D4","E63A8ABF"},{"D5","A71687BF"},{"D6","67F283BF"},{"D7","28CE80BF"},
         {"D8","D1537BBF"},{"D9","520B75BF"},{"DA","D4C26EBF"},{"DB","557A68BF"},{"DC","D63162BF"},
         {"DD","57E95BBF"},{"DE","D8A055BF"},{"DF","59584FBF"},{"E0","DB0F49BF"},{"E1","5CC742BF"},
         {"E2","DD7E3CBF"},{"E3","5E3636BF"},{"E4","DFED2FBF"},{"E5","60A529BF"},{"E6","E15C23BF"},
         {"E7","63141DBF"},{"E8","E4CB16BF"},{"E9","658310BF"},{"EA","E63A0ABF"},{"EB","67F203BF"},
         {"EC","D153FBBE"},{"ED","D4C2EEBE"},{"EE","D631E2BE"},{"EF","D8A0D5BE"},{"F0","DB0FC9BE"},
         {"F1","DD7EBCBE"},{"F2","DFEDAFBE"},{"F3","E15CA3BE"},{"F4","E4CB96BE"},{"F5","E63A8ABE"},
         {"F6","D1537BBE"},{"F7","D63162BE"},{"F8","DB0F49BE"},{"F9","DFED2FBE"},{"FA","E4CB16BE"},
         {"FB","D153FBBD"},{"FC","DB0FC9BD"},{"FD","E4CB96BD"},{"FE","DB0F49BD"},{"FF","DB0FC9BC"}
        };

        QByteArray Xor_Object_ID = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        QByteArray Dial_ALL_Values = "00000000000000000000";
        QByteArray Xor_Object_ID2 = "";
        QByteArray Dial_ALL_Values2 = "";

        QMap <QByteArray,QVector<QByteArray>> MapOfChannelVectors
        {
            {"01",Send01ChannelVector},{"02",Send02ChannelVector},{"03",Send03ChannelVector},{"04",Send04ChannelVector},
            {"05",Send05ChannelVector},{"06",Send06ChannelVector},{"07",Send07ChannelVector},{"08",Send08ChannelVector},
            {"09",Send09ChannelVector},{"0A",Send0AChannelVector},{"0B",Send0BChannelVector},{"0C",Send0CChannelVector},
            {"0D",Send0DChannelVector},{"0E",Send0EChannelVector},{"0F",Send0FChannelVector},{"10",Send10ChannelVector},
            {"11",Send11ChannelVector},{"12",Send12ChannelVector},{"13",Send13ChannelVector},{"14",Send14ChannelVector},
            {"15",Send15ChannelVector},{"16",Send16ChannelVector},{"17",Send17ChannelVector}
        };


        QMap<QByteArray,QByteArray> Client_incrementmap = {{"00","0000"},{"01","0000"},{"02","0000"},{"03","0000"},{"04","0000"},
                                                           {"05","0000"},{"06","0000"},{"07","0000"},{"08","0000"},
                                                           {"09","0000"},{"0A","0000"},{"0B","0000"},{"0C","0000"},
                                                           {"0D","0000"},{"0E","0000"},{"0F","0000"},{"10","0000"},
                                                           {"11","0000"},{"12","0000"},{"13","0000"},{"14","0000"},
                                                           {"15","0000"},{"16","0000"},{"17","0000"},
                                                           {"42","0000"},{"43","0000"}};





        QVector <QString> Inventory_Vector =
        {
            "00000000","01000000","02000000","03000000",
            "04000000","05000000","06000000","07000000",
            "08000000","09000000","0A000000","0B000000",
            "0C000000","0D000000","0E000000","0F000000",
            "10000000","11000000","12000000","13000000",
            "14000000","15000000","16000000","17000000",
            "18000000","19000000","1A000000","1B000000"
        };

        QVector <QString> Bank_Vector =
        {
            "00000000","01000000","02000000","03000000",
            "04000000","05000000","06000000","07000000",
            "08000000","09000000","0A000000","0B000000",
            "0C000000","0D000000","0E000000","0F000000",
            "10000000","11000000","12000000","13000000",
            "14000000","15000000","16000000","17000000",
            "18000000","19000000","1A000000","1B000000",
            "1C000000","1D000000","1E000000","1F000000",
            "20000000","21000000","22000000","23000000"
        };

        QVector <QString> InventorySlots_ItemIDs =
        {
            "A480808000","A680808000","A880808000","AA80808000",
            "AC80808000","AE80808000","B080808000","B280808000",
            "B480808000","B680808000","B880808000","BA80808000",
            "BA80808000","BC80808000","BE80808000","C080808000",
            "C280808000","C480808000","C680808000","C880808000",
            "CA80808000","CC80808000","CE80808000","D080808000",
            "A280808000","A080808000","9E80808000","9C80808000"
        };

        QVector <QString> BankSlots_ItemIDs =
        {
            "D280808000","D480808000","D680808000","D880808000",
            "9A80808000","DE80808000","8480808000","8280808000",
            "8680808000","8880808000","8A80808000","8C80808000",
            "8E80808000","DC80808000","B680808000","DA80808000",
            "E080808000","E280808000","E280808000","9881808000",
            "E480808000","E680808000","9080808000","E880808000",
            "EA80808000","EC80808000","EE80808000","9280808000",
            "F080808000","F280808000","F480808000","F680808000",
            "F880808000","FA80808000","FC80808000","9480808000"
        };

        QVector <QString> InventoryItem_Quantity =
        {
            "02","02","02","02",
            "02","02","02","02",
            "02","02","02","02",
            "02","02","02","02",
            "02","02","02","02",
            "02","02","02","02",
            "02","06","02","14"
        };

        QVector <QString> BankItem_Type =
        {
            "EQuipment","EQuipment","EQuipment","EQuipment","EQuipment","EQuipment",
            "Other","Other","Other","Other","Other","Other","Other","EQuipment",
            "EQuipment","EQuipment","EQuipment","EQuipment","EQuipment","EQuipment",
            "EQuipment","EQuipment","EQuipment","EQuipment","EQuipment","EQuipment",
            "EQuipment","EQuipment","EQuipment","EQuipment","EQuipment","EQuipment",
            "EQuipment","EQuipment","Other","Other"
        };

        QVector <QString> Inv_HP_vector;
        QVector <QString> Inv_Charges_Vector;
        QVector <QString> Bank_HP_vector;
        QVector <QString> Bank_Charges_Vector;

        QVector <QString> InvItem_Type =
        {
            "EQuipment","EQuipment","EQuipment","EQuipment",
            "EQuipment","EQuipment","EQuipment","EQuipment",
            "EQuipment","EQuipment","EQuipment","EQuipment",
            "EQuipment","EQuipment","EQuipment","EQuipment",
            "EQuipment","EQuipment","EQuipment","EQuipment",
            "EQuipment","EQuipment","EQuipment","EQuipment",
            "Other","Other","Other","Other"
        };

        QVector <QString> ItemIsEquippedVector =
        {
            "NO","NO","NO","NO",
            "NO","NO","NO","NO",
            "NO","NO","NO","NO",
            "NO","NO","NO","NO",
            "NO","NO","NO","NO",
            "NO","NO","NO","NO",
            "NO","NO","NO","NO"
        };

        QVector <QString> BankItem_Quantity =
        {
            "02","02","02","02",
            "02","02","02","02",
            "02","02","02","02",
            "04","02","02","02",
            "02","02","02","02",
            "02","02","02","02",
            "02","02","02","02",
            "02","02","02","02",
            "02","02","04","02"
        };

        QMap <QString,QString> EquipmentSlotsUsed =
        {
            {"00","FREE"},{"01","FREE"},{"02","FREE"},{"03","FREE"},
            {"04","FREE"},{"05","FREE"},{"06","FREE"},{"07","FREE"},
            {"08","FREE"},{"09","FREE"},{"0A","FREE"},{"0B","FREE"},
            {"0C","FREE"},{"0D","FREE"},{"0E","FREE"},{"0F","FREE"},
            {"10","FREE"},{"11","FREE"},{"12","FREE"},{"13","FREE"},
            {"14","FREE"},{"15","FREE"}
        };

        QVector <QString> EQuipSots_InvSlots =
        {
            "XX","XX","XX","XX",
            "XX","XX","XX","XX",
            "XX","XX","XX","XX",
            "XX","XX","XX","XX",
            "XX","XX","XX","XX",
            "XX","XX","XX","XX",
            "XX","XX","XX","XX"
        };


        QMap<QByteArray,QByteArray> earlypacket_map;

        QString spell_idout = "";
        QString spell_str1 = "";
        QString spell_graphic = "";
        int spell_position = 0;
        QByteArray master_spell_count = "";
        QByteArray another_spell_count = "";
        QMap <int,QString> spellslot_copy;
        QList <QTimer*> timers1;
        bool unmemorize = false;

        bool slot_0_trigger = false;
        bool slot_1_trigger = false;
        bool slot_2_trigger = false;
        bool slot_3_trigger = false;
        bool slot_4_trigger = false;
        bool slot_5_trigger = false;
        bool slot_6_trigger = false;
        bool slot_7_trigger = false;
        bool slot_8_trigger = false;
        bool slot_9_trigger = false;

        bool cooldown_0 = false;
        bool cooldown_1 = false;
        bool cooldown_2 = false;
        bool cooldown_3 = false;
        bool cooldown_4 = false;
        bool cooldown_5 = false;
        bool cooldown_6 = false;
        bool cooldown_7 = false;
        bool cooldown_8 = false;
        bool cooldown_9 = false;


        int spell_id = 0x0;
        int spell_bookslot = 0x0;
        bool switch1 = false;
        bool switch2 = false;


        QMap <int,QString> spellbook_copy;

        QByteArray spell_0_counter = "0000";
        QByteArray spell_1_counter = "0000";
        QByteArray spell_2_counter = "0000";
        QByteArray spell_3_counter = "0000";
        QByteArray spell_4_counter = "0000";
        QByteArray spell_5_counter = "0000";
        QByteArray spell_6_counter = "0000";
        QByteArray spell_7_counter = "0000";
        QByteArray spell_8_counter = "0000";
        QByteArray spell_9_counter = "0000";


        QMap <int,int> spellslot_map =
        {
            {0,0x12},
            {1,0x36},
            {2,0x3A},
            {3,0x1B},
            {4,0x2A},
            {5,0x35},
            {6,0x27},
            {7,0x09},
            {8,0x00},
            {9,0x29}
        };

        QMap <int,QString> spellbook_map =
        {
            {0x00,"Kick%%AA##2E6E280A##6000"}, //add graphic casttime duration
            {0x01,"Fortitude%%AA##0892C0F3##12000"},
            {0x02,"Return_Home%%AA##3382B28F##60000"},
            {0x03,"Quick_Strike%%AA##8B77CD66##30000"},
            {0x04,"Taunt%%AA##2A169ECF##5000"},
            {0x05,"Concentration%%AA##0892C0F3##4000"},
            {0x06,"Call_to_Action%%AA##B6B5AE1D##90000"},
            {0x07,"Furious_Defense%%AA##B7B5AE1D##120000"},
            {0x08,"Bellow%%AA##9A40BE64##300000"},
            {0x09,"Stomp%%AA##2E6E280A##6000"},
            {0x0A,"Elemental_Toughness%%AA##9A40BE64##2000"},
            {0X0B,"Attention%%AA##0892C0F3##60000"},
            {0X0C,"Rapid_Strike%%AA##8B77CD66##30000"},
            {0X0D,"Call_to_Arms%%AA##B6B5AE1D##90000"},
            {0X0E,"Critical_Strike%%AA##CD6D63EA##90000"},
            {0X0F,"Iron_Will%%AA##B6B5AE1D##90000"},
            {0X10,"Roar%%AA##9A40BE64##300000"},
            {0X11,"Minotaurs_Persistence%%AA##B8B5AE1D##6000"},
            {0X12,"Enrage%%AA##E3884247##7000"},
            {0X13,"Provoke%%AA##2A169ECF##5000"},
            {0X14,"Critical_Assault%%AA##CD6D63EA##90000"},
            {0X15,"Call_to_Combat%%AA##B6B5AE1D##90000"},
            {0X16,"Iron_Resolve%%AA##B6B5AE1D##90000"},
            {0X17,"Howl%%AA##9A40BE64##300000"},
            {0X18,"Focus%%AA##0892C0F3##90000"},
            {0X19,"Bait%%AA##2A169ECF##5000"},
            {0X1A,"Iron_Conviction%%AA##B6B5AE1D##90000"},
            {0X1B,"Critical_Barrage%%AA##CD6D63EA##90000"},
            {0X1C,"Werewolf%%AA##8503B84D##5000"},
            {0X1D,"War_Cry%%AA##9A40BE64##300000"},
            {0X1E,"Incite%%AA##2A169ECF##5000"},
            {0X1F,"Feral_Wolf%%AA##8503B84D##5000"},
            {0X20,"Dire_Wolf%%AA##8503B84D##5000"},
            {0X21,"Call_to_War%%AA##B6B5AE1D##90000"},
            {0X22,"Dash%%AA##0D5CABA9##240000"},
            {0X23,"Summon_Badger%%AA##8503B84D##5000"},
            {0X24,"Summon_Undead_Mammoth%%AA##8503B84D##5000"},
            {0X25,"Summon_Squishy_Cube%%AA##8503B84D##5000"},
            {0X26,"Summon_Tiny_Jester%%AA##8503B84D##5000"},
            {0X27,"Boot%%AA##2E6E280A##6000"},
            {0X28,"Goad%%AA##2A169ECF##5000"},
            {0X29,"Bite%%AA##D5DE907B##5000" },
            {0X2A,"Slash%%AA##20AF8B76##30000"},
            {0X2B,"Summon_Baby_Sandswiss%%AA##8503B84D##5000"},
            {0X2C,"Summon_White_Rat%%AA##8503B84D##5000"},
            {0X2D,"Illusion_Gelatinous_Cube%%AA##24E6147B##5000"},
            {0X2E,"Illusion_Rat%%AA##8503B84D##5000"},
            {0X2F,"Sand_Blast_Effect%%AA##AE10C42A##30000"},
            {0X30,"Summon_Sand_Giant%%AA##8503B84D##5000"},
            {0X31,"Summon_Moss_Snake%%AA##8503B84D##5000"},
            {0X32,"Illusion_Shark%%AA##8503B84D##5000"},
            {0X33,"Tough_Skin%%AA##B6B5AE1D##4000"},
            {0X34,"Remove_Illusion%%AA##4800030E##5000"},
            {0X35,"Dropkick%%AA##2E6E280A##6000"},
            {0X36,"Critical_Attack%%AA##CD6D63EA##90000"},
            {0X37,"Sprint%%AA##0D5CABA9##120000"},
            {0X38,"Rend%%AA##20AF8B76##30000"},
            {0X39,"Focused_Attention%%AA##0892C0F3##90000"},
            {0X3A,"Critical_Flurry%%AA##CD6D63EA##90000"},
            {0X3B,"Iron_Resolution%%AA##B6B5AE1D##30000"},
            {0X3C,"Mithril_Skin%%AA##0892C0F3##12000"},
            {0X3D,"Call_to_Victory%%AA##B6B5AE1D##90000"},
            {0X3E,"Diligence%%AA##0892C0F3##36000"},
            {0X3F,"Protection_of_Marr%%AA##B8B5AE1D##30000"},
            {0X40,"Growl_of_Battle%%AA##E3884247##20000"},
            {0X41,"Frantic_Defense%%AA##B7B5AE1D##120000"}
        };

        QString Current_Available_balance = "";
        QString Cash_on_hand = "";
        QString Red_Or_White_Text = "";
        int masterworld = 0;

        QByteArray Pass_Word = "";
        QString UserName = "";
        QString PassWord = "";
        QByteArray AccountKeyReply = "";
        QByteArray NewAccount1 = "";

        QString Name = "";
        QString sLevel = "";
        QString Race = "";
        QString Gender = "";
        QString Class = "";
        QString HairColor = "";
        QString HairLength = "";
        QString HairStyle = "";
        QString Face = "";
        QString FPorNot = "";
        QString CurrentQuestMenuID = "";

        int STR1 = 0;
        int STA1 = 0;
        int AGI1 = 0;
        int DEX1 = 0;
        int WIS1 = 0;
        int INT1 = 0;
        int CHA1 = 0;
        int RemainingTPs = 0;
        int TotalTPSpent = 0;

        QByteArray mastertelinc = "0000"; //reset teleport end counter
        QByteArray C_closer9007_endcount = "0000"; //reset closer increment


        bool Death = false;
        bool FullHealth = false;

        bool charselectionPassed = false;
        QByteArray SeverSelectionLastReplyConfirm = "";

        int disconnectCount = 0;
        QByteArray OldConnectionCount  = "";


        QString holdmasterx = "";
        QString holdmastery = "";
        QString holdmasterz = "";
        QByteArray client_order = "03000400";

        QString CoachTimer = "";
        QString TeleportTimer = "";
        QString GroupTimer = "";
        QString Resend42ecTimer = "";
        QString ResendFBsTimer = "";
        QString ResendChannelsTimer = "";
        QString MobDamageTimer = "";
        QString MyDamageTimer = "";
        QString MobRespawnTimer = "";

        QByteArray WeaponTypeSecondary = "";
        QByteArray WeaponTypePrimary = "";

        QString SecondaryLevel = "";
        QString SecondaryDamage = "";
        QString PrimaryLevel = "";
        QString PrimaryDamage = "";

        int Attacker_LVL = 0;
        int Attacker_STR = 0;
        int Attacker_DEX = 0;
        int Attacker_Offmods = 0;

        int Defender_LVL = 0;
        int Defender_AGI = 0;
        int Defender_AC = 0;
        int Defender_Defmods = 0;

        QString AttackersOffMods = "";

        int resendingFBCounter = 0;
        int resending42ecCounter = 0;
        bool ResendAllFBs = false;

        QVector <QString> DamageVector;

        QString coachCharID = "";
        QVector <int> destination;
        QVector <QString> positionlist;
        QVector <QString> ellisto;
        QMap <int,QByteArray>swapnames;

        QByteArray ReplaceID = "595E03";
        float Farthest_distance = 0.0;

        QString targetCharID = "";

        QMap<QString,bool> FollowMap;
        QMap<QByteArray,QByteArray> HPxorMap;
        QMap<QByteArray,QByteArray> LoS_StatusMap;

        bool AllChannelsFull = false;


        QByteArray LastTell = "";
        QString GroupID = "00000000";
        QString GuildID = "";
        QVector<QString> GroupInvitesSentVector;
        QVector<QString> GroupInvitesReceivedVector;

        bool Disconnected = false;
        bool SendToGroupTimer = false;
        bool Packet156 = false;

        QByteArray GroupPacket = "";

        QByteArray OptionFlag = "";
        QByteArray ServerFlag = "";

        QByteArray SessionID = "";
        QString CharSelectID_Main = "";

        QMap<QString,int> NamelistCountMap;
        QByteArray PacketReply = "";

        QString LastCharSelect = "";



    };

    static packetvars pX;


    static QMap<QByteArray,struct packetvars> IPandPort_AllvariablesMap;

private:

    QVector<QByteArray>opvect = {"12","0000","010C","0409","0100","0400","0A00","0B00","0D00","0E00","0F00","1000","1200","1300","1400","1500","1600",
                                 "1800","1900","1A00","1D00","1E00","2000","2100","2B00","2C00","2D00","3200","3300","3400","3500","3A00","3B00",
                                 "3C00","3D00","3E00","3F00","4000","4100","4200","4300","4400","4600","4900","4A00","4B00","4C00","5200","5600",
                                 "5700","6000","6100","6200","6300","6B00","6C00","6D00","7300","7C00","7D00","7E00","7F00","B100",
                                 "B400","B700","C500","C700","C800","C900","CA00","CD00","CE00","D700","D800","D900","DB00",
                                 "EC00","6603","6903","D503","6804","7805","7905","2206","2306","2406","2506","2606","2706","9E06","6007","6107","7207",
                                 "7307","7607","7807","9007","9107","A307","B007","B307","D007","D107",
                                 "F507","F607","F707","F807","FE00","2A00","CB00",
                                 "0108","4008","0409","B009","7A0A","7B0A","A10A","C40A","010D","410D","010E","020E","0010",
                                 "0510","0111","0512","0612","4D12","5012","5112","5312","5412","5512","0114","0214","0115"};


signals:

public slots:
    QMap<QByteArray,struct packetparsing::packetvars> ParseThePacket(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QByteArray ClientsPacket, QMap<QByteArray,struct packetparsing::packetvars> IPandPort_AllvariablesMap);

protected:

};

#endif // PACKETPARSING_H
