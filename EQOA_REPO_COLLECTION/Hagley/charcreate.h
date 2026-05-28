#ifndef CHARCREATE_H
#define CHARCREATE_H

#include <QObject>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QByteArray>
#include <QDebug>
#include <QVector>
#include <QMap>
#include <QStringList>
#include <QString>

class CharCreate : public QObject
{
    Q_OBJECT
public:
    explicit CharCreate(QObject *parent = nullptr);

    static QMap<QString,QString> ID_StartingCityMap;
    static QMap<QString,QVector<QByteArray>> CharID_clientID_clientIP_clientPORTvectorMap;
    static QMap<QString,QVector<QString>> GroupID_PlayersID_NamesVectorMap;
    static QMap<QString,QVector<QString>> PendingGroupID_PlayersID_NamesVectorMap;
    static QMap<QString,QString> PlayersID_GroupID_Map;




    QString directory = QCoreApplication::applicationDirPath();

    QString Database = "/Master_Database.db";
    QSqlDatabase db = QSqlDatabase::database();
    QString Table_Name = "";
    QString Column_Name = "";
    QByteArray rowline = "";
    QByteArray Column_value = "";
    int packetid = 0;
    QByteArray PacketOut = "";
    QList<QString> positionlist2;
    int writenum = 0;

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


    QByteArray NPC_Location_XZY_2 = "";
    const QByteArray NPC_Location_XZY	 = "f60700D1E5974505817642AE819745B0184640000000000000000000000000";



    QMap <QByteArray,QByteArray> RaceAndGenderMap =
    {
        {"0600","DF3ECA61"},{"0602","16D54AC4"},{"0200","AA9DD87E"},{"0202","394B9B8C"},
        {"0400","47045BAA"},{"0402","BEFD0B35"},{"0100","F86FD5D8"},{"0102","F71989E1"},
        {"0800","470B0C32"},{"0802","BE3C1A4F"},{"0300","156B9CA9"},{"0302","FC84404C"},
        {"0700","E23FDBA3"},{"0702","31A7278A"},{"0000","C698D870"},{"0002","3573530D"},
        {"0900","90748084"},{"0902","4F0BC0AC"},{"0500","32A56F4C"},{"0502","0127E34D"}
    };

    QMap <QString,QString> StartingXZYandFacingMap =
    {
        //{Race/Class/FPorNot},{X/Z/Y/Facing}
        //HUM EASTERN
        {"000E02","3168F400237B1E602896"},//ALC
        {"000502","31A4150020F41E7AD680"},//BRD
        {"000902","31C25F001AC01E78EF80"},//CL
        {"000C02","3169CD001FDD1E63E5DD"},//ENC
        {"000A02","3164BF001FDD1E604BDC"},//MAG
        {"000B02","31BF52001E1D1EAFD8AB"},//NEC
        {"000602","31C20B001AC01E7F75AD"},//RGE
        {"000302","31C1AB001AC01E79CE26"},//SK
        {"000002","31C7F5001EAE1E6055A5"},//WAR
        {"000D02","31699900237B1E601B17"},//WIZ

        //HUM WESTERN
        {"000E04","08B485001D1D21C5F190"},//ALC
        {"000504","08E4FE001D1B216CE3FE"},//BRD
        {"000904","085FD2001CF221C24080"},//CL
        {"000704","1259B90023F11888237A"},//DRD
        {"000C04","084071001F07215C28C0"},//ENC
        {"000A04","083D1F001F05215D2DAA"},//MAG
        {"000404","0AEBB4001D41226C33AE"},//MNK
        {"000204","083D87001D3921D556C0"},//PAL
        {"000104","119610001F5218DFB0C0"},//RAN
        {"000604","08B9AD0020562157CDFB"},//RGE
        {"000004","084971001D1121892493"},//WAR
        {"000D04","0840F2001F0521608EC8"},//WIZ

        //ELF
        {"010E00","24EEF1001AF00CD3FF00"},//ALC
        {"010500","2369540025C70E27AEEA"},//BRD
        {"010900","257651001FB30C5155D8"},//CL
        {"010700","23D49A0025C70E4AFCC0"},//DRD
        {"010C00","24E42B001FB30CC8F97D"},//ENC
        {"010A00","24E4AE001FB30CD47320"},//MAG
        {"010200","251588001FB30C688BC2"},//PAL
        {"010100","23E3470025C70E689B45"},//RAN
        {"010600","23944D0025C70E401583"},//RGE
        {"010D00","24E513001FB30CD0D358"},//WIZ

        //DELF
        {"020E00","31360E00034C11471800"},//ALC
        {"020900","30634D0003A511374B91"},//CL
        {"020C00","30D6190003C2111BED80"},//ENC
        {"020A00","30D5E70003C21113A828"},//MAG
        {"020B00","30147D0002E21143F56F"},//NEC
        {"020600","3164840000A711591680"},//RGE
        {"020300","3016A40002E211435D50"},//SK
        {"020000","3142050003AD10D80000"},//WAR
        {"020D00","30C42A0003C211130100"},//WIZ

        //GNO
        {"030E00","2E9BE000105E0C31410F"},//ALC
        {"030900","2E13F80010500BEBC75C"},//CL
        {"030C00","2E699B0010500BF5F8DA"},//ENC
        {"030A00","2E704C0010500BEEBF8E"},//MAG
        {"030B00","2E20E90011F40CC7FA9B"},//NEC
        {"030600","2E32F60001A60CA46E39"},//RGE
        {"030000","2DD8A200129E0C20BCEC"},//WAR
        {"030D00","2E7E6200105E0C50284E"},//WIZ

        //DWF
        {"040900","1E2ABF00297C11154240"},//CL
        {"040200","1E27B800297B110C804C"},//PAL
        {"040600","1E09A8002C4810F6A16F"},//RGE
        {"040000","1DA9900031F5111D27F2"},//WAR

        //TRL
        {"050300","320F170025803DB2A9CE"},//SK
        {"050800","3159DE001FE03D603DCF"},//SHA
        {"050000","319F580023013D5907AF"},//WAR

        //BAR
        {"060600","199CB5001B33086DAB77"},//RGE
        {"060800","19F812002094086B63D8"},//SHA
        {"060000","1981B5001B3308614CC0"},//WAR

        //HLF
        {"070900","24F13F001D6015F39E43"},//CL
        {"070700","24E4D0001D4C1535DFC0"},//DRD
        {"070600","24F110001D4C157B0123"},//RGE
        {"070000","24A8A0001F30156FFB5F"},//WAR

        //ERU
        {"080E00","0964B8001B6829DA0B9E"},//ALC
        {"080900","093D660021182A2CC600"},//CL
        {"080C00","08E7B8003C302A65A740"},//ENC
        {"080A00","08AD430054C82AAADC83"},//MAG
        {"080B00","092633001C882A5C8F26"},//NEC
        {"080200","093D6F001B102A48AED1"},//PAL
        {"080300","0925A1001C882A6E8B0A"},//SK
        {"080D00","082C6A0036682A07FE80"},//WIZ

        //OGR
        {"090B00","125016000F300DEAFA3D"},//NEC
        {"090300","126FD1000D300DECDF1C"},//SK
        {"090800","1118520027900E59F3BD"},//SHA
        {"090000","124FC0001A980E6BF2E4"},//WAR
    };

    QMap <QString,QString> StartingStatsMap =
    {
        //BAR
        {"060600","75707080505555"}, //RGE
        {"060800","65806060755560"}, //SHA
        {"060000","85856065555550"}, //WAR

        //DELF
        {"020E00","50655570759050"}, //ALC
        {"020900","60655560806570"}, //CL
        {"020C00","50556070609070"}, //ENC
        {"020A00","50657065659050"}, //MAG
        {"020B00","60755070609050"}, //NEC
        {"020600","60657085556555"}, //RGE
        {"020300","70806070557050"}, //SK
        {"020000","70856070606550"}, //WAR
        {"020D00","60606080559050"}, //WIZ

        //DWF
        {"040900","70705555855070"}, //CL
        {"040200","80856065655050"}, //PAL
        {"040600","70707080605055"}, //RGE
        {"040000","80856065655050"}, //WAR

        //ELF
        {"010E00","50605570808060"}, //ALC
        {"010500","55707570605570"}, //BRD
        {"010900","60605560855580"}, //CL
        {"010700","60505080856070"}, //DRD
        {"010C00","50506070658080"}, //ENC
        {"010A00","50607065708060"}, //MAG
        {"010200","70756070655560"}, //PAL
        {"010100","60607085655560"}, //RAN
        {"010600","60607085605565"}, //RGE
        {"010D00","60556080608060"}, //WIZ

        //ERU
        {"080E00","50605565809550"}, //ALC
        {"080900","60605555857070"}, //CL
        {"080C00","50506065659570"}, //ENC
        {"080A00","50607060709550"}, //MAG
        {"080B00","60705065659550"}, //NEC
        {"080200","70756065657060"}, //PAL
        {"080300","70756065608550"}, //SK
        {"080D00","60556075609550"}, //WIZ

        //GNO
        {"030E00","50606570709050"}, //ALC
        {"030900","60606560756570"}, //CL
        {"030C00","50507070559070"}, //ENC
        {"030A00","50608065609050"}, //MAG
        {"030B00","60706070559050"}, //NEC
        {"030600","60608085506555"}, //RGE
        {"030000","70757070556550"}, //WAR
        {"030D00","60557080509050"}, //WIZ

        //HLF
        {"070900","60706565755070"}, //CL
        {"070700","60606085755560"}, //DRD
        {"070600","60708090505055"}, //RGE
        {"070000","70757075555050"}, //WAR

        //HUM EASTERN
        {"000E02","55656065758055"}, //ALC
        {"000502","60758065555565"}, //BRD
        {"000902","65656055805575"}, //CL
        {"000C02","55556565608075"}, //ENC
        {"000A02","55657560658055"}, //MAG
        {"000B02","65755565608055"}, //NEC
        {"000602","65657580555560"}, //RGE
        {"000302","75806565556055"}, //SK
        {"000002","75806565605555"}, //WAR
        {"000D02","65606575558055"}, //WIZ

        //HUM WESTERN
        {"000E04","455656065758055"}, //ALC
        {"000504","460758065555565"}, //BRD
        {"000904","465656055805575"}, //CL
        {"000704","465555575806065"}, //DRD
        {"000C04","455556565608075"}, //ENC
        {"000A04","455657560658055"}, //MAG
        {"000404","465756565605555"}, //MNK
        {"000204","475806565605555"}, //PAL
        {"000104","465657580605555"}, //RAN
        {"000604","465657580555560"}, //RGE
        {"000004","475806565605555"}, //WAR
        {"000D04","465606575558055"}, //WIZ

        //OGR
        {"090300","90906060505550"}, //SK
        {"090800","70856055755060"}, //SHA
        {"090B00","80856050755550"}, //NEC
        {"090000","90906060555050"}, //WAR

        //TRL
        {"050300","90906060505550"}, //SK
        {"050800","70856055755060"}, //SHA
        {"050000","90906060555050"}  //WAR
    };

    QVector <int> StartingCoachVector =
    {
        0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
        1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
        1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
        1,0,1,0,1,0,1,0,1,0,1,0,1,0,1
    };

    QVector <int> StartingFactionVector =
    {
        10000,9800,9800,10000,10000,9800,9800,
        10000,10000,10000,10200,9800,10200,9800,
        10000,10000,10200,10200,10000,9800,10000,
        10000,10000,10500,10000,10200,9800,9800,
        9800,9800,10000,10200,0,10000,20000,9800
        ,10200,9800
    };

    QMap <QString,QString> ColumnServerIDMap =
    {
        {"0A1F","CLW"},{"013E","DH"},{"1C24","FH"},{"91B9","HOD"},{"9EBD","MF"},
        {"A9D7","PPO"},{"359B","HAG"},{"91B9","SS"}
    };

    QVector <QString> StatsFieldsVector =
    {
        "Name (24 Max)", "ObjectID (4)", "STR", "STA", "AGI", "DEX", "WIS", "INT", "CHA",
        "HP", "HP-MAX", "POW", "POW-MAX", "UNK-01", "HOT", "POT", "AC", "UNK-02", "PR", "DR", "FR", "CR",
        "LR", "AR", "FISHING", "BASE-STR", "BASE-STA", "BASE-AGI", "BASE-DEX", "BASE-WIS", "BASE-INT",
        "BASE-CHA", "HP-2", "BASE-HP", "POW-2", "BASE-POW", "UNK-03", "BASE-HOT", "BASE-POT", "BASE-AC",
        "UNK-04", "BASE-PR", "BASE-DR", "BASE-FR", "BASE-CR", "BASE-LR", "BASE-AR", "BASE-FISHING", "UNK-05"
    };

signals:

public slots:
    QByteArray CharacterCreation(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort);

    QByteArray CharSelectScreen(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort);
    QByteArray CharDelete(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QString DeleteSelectID);
    QByteArray PullCharacter(QString CharSelectID);
};

#endif // CHARCREATE_H
