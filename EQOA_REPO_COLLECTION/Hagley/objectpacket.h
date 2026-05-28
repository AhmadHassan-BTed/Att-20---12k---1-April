#ifndef OBJECTPACKET_H
#define OBJECTPACKET_H

#include <QObject>
#include<QString>
#include<QByteArray>
#include<QList>
#include<QDebug>
#include <QtSql>
#include <QSqlQuery>
#include<QtMath>
#include<QVector>
#include<QLineEdit>
#include<QStringList>
#include<algorithm>
#include<QMap>
#include<QMapIterator>
#include "packetparsing.h"

class objectpacket : public QObject
{
    Q_OBJECT
public:
    explicit objectpacket(QObject *parent = nullptr);


    QString FileName = "C:/PCSX2/" + QString::number(0) + ".txt";

    static QStringList holdx;
    static QStringList holdz;
    static QStringList holdy;
    static QStringList holdid1;

    static float distancecheck1;

    static QVector <QByteArray> OutGoingVector;

    static QMap<QString,float> Temp_Distance_ID_Map;
    static QMap<QString,QString> AGGRO_List;
    static QMap<QString,QString> Roamers_List;
    static float AGGRO_Radius;
    static float Roamers_Radius;
    static QString Current_Chan;
    static QString AGGRO_Chan;
    static QString Roamers_Chan;
    static QByteArray Path_Maker;
    static float Farthest_distance;
    static QString FarthestID;
    static QByteArray PET_Exists;
    static QString Resend_AGGRO_ID;
    static QByteArray Resend_AGGRO_Chan;
    static QString Resend_AGGRO_c9;
    static QString AGGRO_New_Chan;
    static QVector <QString> DeadListVector;
    static QMap <QString,QVector <QString>> DeadListVectorMap;



    QSqlDatabase db = QSqlDatabase::database();
    QByteArray byte2;
    QByteArray workpacket;
    QString output1 = "";

    static QMap<QString,QString> DespawnDelete_Map;
    static QMap<QByteArray,QByteArray> Master_Map;
    static QMap<QByteArray,QByteArray> MasterMap2;

    static QMap <QString,QVector<QString>> IDs_DamageVectorMap;
    static QMap <QString,QVector <QString>> MonsterRaceVectorMap;
    static QMap <QString,QVector <QString>> MonsterClassVectorMap;
    static QMap <QString,QString> ClassValueToString;
    static QMap <QString,QString> RaceValueToString;

    int index1 = 0;
    int index2 = 0;
    int encarat1 = 0;
    int encarat2 = 0;
    int enhigh = 0;
    int enlow = 0;
    int zerocounter = 0;
    int holdit = 0;

    QByteArray encryptpacket = "";
    QByteArray encom1 = "";
    QByteArray encom2 = "";
    QByteArray encom3 = "";
    QByteArray hold2 = "";
    QByteArray hold4 = "";





signals:

public slots:
    QByteArray packetswitch(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort);

private:

    int numberofresults1 = 0;
    int maincount1 = 0;
    int mainsub = 0;
    int mainremain = 0;
    int supercounter = 0;
    QString pullidout1 = "";



};

#endif // OBJECTPACKET_H
