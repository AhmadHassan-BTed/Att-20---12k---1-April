#ifndef FACINGDIRECTION_H
#define FACINGDIRECTION_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QDebug>
#include <QtSql>
#include <QSqlQuery>
#include <QtMath>
#include <QVector>
#include <QLineEdit>
#include <QStringList>
#include <algorithm>
#include <QMap>
#include <QMapIterator>
#include <QUdpSocket>

class FacingDirection : public QObject
{
    Q_OBJECT
public:
    explicit FacingDirection(QObject *parent = nullptr);


    static QMap<QByteArray,QByteArray> LoS_StatusMap;
    static QMap<QByteArray,QPixmap> CurrentImageMap;
    static QMap<QString,bool> FollowMap;
    static QMap<QByteArray,QByteArray> HPxorMap;
    static QString AggroDistance;
    static float AggroRunDistance;
    static float AggroSpeed1;
    static QString Now_Roaming;

    static QMap <QString,int> RandomIDsMAP;

    static QMap <QString,int> Path_CounterMap;
    static QMap <QString,QMap<int,QString>> Path_Maps;

    static QString UseThisMap;
    static QMap <QString,QMap<int,QString>> TempPathMaps;

    static QMap <QString,QByteArray> Facing_Lock_Map;
    static QMap <QString,QByteArray> Facing_Lock2_Map;
//    static QMap <QString,QString> SnowMan_c9_DM;
//    static QMap <int,QString> SnowMan_Path_DM;
//    static QMap <QString,QString> SnowMan_c9_HP;
//    static QMap <int,QString> SnowMan_Path_HP;
//    static QMap <QString,QString> SnowMan_c9_FP;
//    static QMap <int,QString> SnowMan_Path_FP;
//    static int Roaming_Counter;
//    static int Roaming_Counter2;
//    static int Roaming_Counter3;
//    static int Roaming_Counter4;
//    static QMap <int,QString> Path_Piker_Yeovalech;
//    static QMap <int,QString> RABID_BAT_01;
//    static QMap <int,QString> BAT_01;
//    static QMap <int,QString> HATCHLING_01;
//    static QMap <int,QString> SPIDERLING_01;
//    static QMap <int,QString> MAGPIE_01;
//    static QMap <int,QString> CROW_01;
//    static QMap <int,QString> GRASS_SNAKE_01;
//    static QMap <int,QString> GRASS_VIPER_01;
//    static QMap <int,QString> SPARKFLIES_01;
//    static QMap <int,QString> BAT_02;
//    static QMap <int,QString> GRASS_VIPER_02;
//    static QMap <int,QString> HATCHLING_02;
//    static QMap <int,QString> MAGPIE_02;
//    static QMap <int,QString> SPARKFLIES_02;
//    static QMap <int,QString> RABID_BAT_02;
//    static QMap <int,QString> GRASS_SNAKE_02;
//    static QMap <int,QString> SPIDERLING_02;
//    static QMap <int,QString> CROW_02;
//    static QMap <int,QString> SPARKFLIES_03;
//    static QMap <int,QString> BAT_03;
//    static QMap <int,QString> GRASS_SNAKE_03;
//    static QMap <int,QString> SPIDER_01;
//    static QMap <int,QString> RAVEN_01;
//    static QMap <int,QString> RABID_BAT_03;
//    static QMap <int,QString> BAT_04;
//    static QMap <int,QString> HATCHLING_03;
//    static QMap <int,QString> SPIDERLING_03;
//    static QMap <int,QString> MAGPIE_03;
//    static QMap <int,QString> CROW_03;
//    static QMap <int,QString> GRASS_SNAKE_04;
//    static QMap <int,QString> GRASS_VIPER_03;
//    static QMap <int,QString> SPARKFLIES_04;
//    static QMap <int,QString> BAT_05;
//    static QMap <int,QString> GRASS_VIPER_04;
//    static QMap <int,QString> HATCHLING_04;
//    static QMap <int,QString> MAGPIE_04;
//    static QMap <int,QString> SPARKFLIES_05;
//    static QMap <int,QString> RABID_BAT_04;
//    static QMap <int,QString> GRASS_SNAKE_05;
//    static QMap <int,QString> SPIDERLING_04;
//    static QMap <int,QString> CROW_04;
//    static QMap <int,QString> SPARKFLIES_06;
//    static QMap <int,QString> BAT_06;
//    static QMap <int,QString> GRASS_SNAKE_06;
//    static QMap <int,QString> SPIDER_02;
//    static QMap <int,QString> RAVEN_02;

    const QByteArray Header_Roamer = "XXXXXXXXXXXX01bf543213d9bc0d00xxxx";
    QString MOB_X_MOB_Z_MOB_Y_MOB_Radius = "";
    QByteArray Header_Channel = "xxc9xxxx01";
    QByteArray Combined1 = "";
    QString New_Chan = "";
    QString Output_Xor_F_21 = "";
    float Next_Step_Distance = 0.0;
    QByteArray Animation_New = "";
    QString Output_New_Aimation = "";
    int MOB_Counter_Facing2 = 0;
    QString Output_Xor_F_2 = "";
    QByteArray Facing_Lock = "00";
    QByteArray Facing_Lock2 = "00";
    int Difference = 0;
    int Difference2 = 0;
    int Difference3 = 0;
    int Difference4 = 0;
    int Difference_Best = 0;
    int New_Difference_int = 0;
    int New_Difference4_int = 0;
    QString New_Difference = "";
    QString New_Difference4 = "";
    const QByteArray Xor_Mob_movement = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    QByteArray Xor_Mob_movement2 = "";
    const QByteArray Mob_ALL_Values = "00000000000000000000";
    QByteArray Mob_ALL_Values2 = "";
    QString MOB_Counter_X = "";
    QString MOB_Counter_Z = "";
    QString MOB_Counter_Y = "";
    QString MOB_Master_c9 = "";
    QString AGGRO_Starting_X = "";
    QString AGGRO_Starting_Z = "";
    QString AGGRO_Starting_Y = "";
    QString  Path_XZY = "";

    QString Roamer_ID_key = "";
    float Return_Distance;

    qlonglong NPC_X = 0;
    qlonglong NPC_Z = 0;
    qlonglong NPC_Y = 0;

    qlonglong MOB_X = 0;
    qlonglong MOB_Z = 0;
    qlonglong MOB_Y = 0;
    QString AGGRO_X_AGGRO_Z_AGGRO_Y_AGGRO_Radius = "";
    QString NPC_Counter_X = "";
    QString NPC_Counter_Z = "";
    QString NPC_Counter_Y = "";



    QString Targets_X = "";
    QString Targets_Y = "";
    int mobfacing = 0;
    float X_Float1 = 0.0;
    float Y_Float1 = 0.0;
    float Targets_X_2 = 0.0;
    float Targets_Y_2 = 0.0;
    QString Facing_output = "";
    QString New_NPC_Facing = "00";

    QString AGGRO_put1 = "";
    QString NPC_Counter_X1 = "";
    QString NPC_Counter_Z1 = "";
    QString NPC_Counter_Y1 = "";


    float Main_X = 0.0;
    float Main_Z = 0.0;
    float Main_Y = 0.0;

    QString AGGRO_out1 = "";
    QString MOB_Counter_X1 = "";
    QString MOB_Counter_Z1 = "";
    QString MOB_Counter_Y1 = "";

    float Main_MOB_X = 0.0;
    float Main_MOB_Z = 0.0;
    float Main_MOB_Y = 0.0;
    float tx = 0.0;
    float tz = 0.0;
    float ty = 0.0;
    float tx1 = 0.0;
    float tz1 = 0.0;
    float ty1 = 0.0;

    float xnode = 0.0;
    int Divide_By2;
    float Z_speed;
    QByteArray Hold_Z_Value;
    float Z_Difference;
    QString AGGRO_Movement_Rate;
    QString MOB_Movement_Rate;
    float Divide_By = 0.0;
    float MOB_Speed = 0.0;
    QString NPC_Counter_Facing;
    QString MOB_Counter_Facing;
    QString AGGRO_Facing_Player;
    QString MOB_Facing_Home;
    QByteArray AGGRO_Animation_Old;
    QByteArray MOB_Animation_Old;
    QByteArray Con_Old;
    QByteArray Distance_Lock;

    QByteArray Xor_Object_ID2 = "";
    QByteArray Dial_ALL_Values2 = "";
    QString AGGRO_ID = "";
    QString AGGRO_Chan = "";
    QByteArray outcrc;
    QByteArray byte1 = "";
    QString Combined = "";
    QString Combined2 = "";

    const QByteArray Xor_Object_ID = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    const QByteArray Header_AGGRO = "xxc9xxxx01";
    const QByteArray Dial_ALL_Values = "00000000000000000000";

    bool Follow = true;


    QPixmap currentImage;











signals:

public slots:

    QByteArray SendToFacing(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QByteArray masterx, QByteArray masterz, QByteArray mastery);
    QVector<QByteArray> SendToRoaming(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QByteArray masterx, QByteArray masterz, QByteArray mastery);

    QString Facing_Player(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,
                             QString NPC_Counter_X, QString NPC_Counter_Y);

    QString Facing_Home(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QString AGGRO_Starting_X, QString AGGRO_Starting_Z, QString AGGRO_Starting_Y,
                           QString MOB_Counter_X, QString MOB_Counter_Z, QString MOB_Counter_Y);

    QString AGGRO_Radius_Movement(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QByteArray EndX, QByteArray EndZ, QByteArray EndY,QString NPCID,
                        QString NPC_Counter_X, QString NPC_Counter_Z, QString NPC_Counter_Y);

    QString MOB_Radius_Movement(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QString AGGRO_Starting_X, QString AGGRO_Starting_Z, QString AGGRO_Starting_Y,
                        QString MOB_Counter_X, QString MOB_Counter_Z, QString MOB_Counter_Y);


private slots:

};

#endif // FACINGDIRECTION_H
