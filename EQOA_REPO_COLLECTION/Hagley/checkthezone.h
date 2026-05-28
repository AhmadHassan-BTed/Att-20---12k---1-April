#ifndef CHECKTHEZONE_H
#define CHECKTHEZONE_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QStack>
#include <QByteArray>
#include <QtCore>
#include <QPixmap>
#include "packetparsing.h"

class checkthezone : public QObject
{
    Q_OBJECT
public:
    explicit checkthezone(QObject *parent = nullptr);

QString FileName = "C:/PCSX2/" + QString::number(0) + ".txt";


    QString directory = QCoreApplication::applicationDirPath();

    QString Database = "/Master_Database.db";


    static QMap<QString,QMap<QString,QVector<QString>>> ZoneForOBJvectorsmaps;

    static QVector<QMap<QString,QMap<QString,QVector<QString>>>> WorldsVectorOFObjectZonesMaps;

    static QVector <QString> TunariaZoneNames;

    ///////////////////////////////////////

    static QVector <QString> RatheZoneNames;

    ///////////////////////////////////////

    static QVector <QString> OdusZoneNames;

    ///////////////////////////////////////

    static QVector <QString> LSMZoneNames;

    ///////////////////////////////////////

    static QVector <QString> PoSZoneNames;

    ///////////////////////////////////////

    static QVector <QString> SecretsZoneNames;

    ///////////////////////////////////////



    static QVector <QVector<float>> TunariaZoneCentralVectors;

    ///////////////////////////////////////

    static QVector <QVector<float>> RatheZoneCentralVectors;

    ///////////////////////////////////////

    static QVector <QVector<float>> OdusZoneCentralVectors;


    ///////////////////////////////////////

    static QVector <QVector<float>> LSMZoneCentralVectors;

    ///////////////////////////////////////

    static QVector <QVector<float>> PoSZoneCentralVectors;

    ///////////////////////////////////////

    static QVector <QVector<float>> SecretZoneCentralVectors;

    ///////////////////////////////////////

    static QMap <QString,QString> NPCsNames;
    static QMap <QString,int> IDsAndWorlds;

    static QVector <QVector <QVector<float>>> VectorOfWorlds;

    ///////////////////////////////////////

    static QVector<QVector<QString>> WorldZoneNamesVector;
    static QMap <QString,int> IDandZoneNumber;

    ///////////////////////////////////////

    static QMap <QString,QVector <QString>> ZoneLocationsIDsVectorMap;

    static QVector<QMap <QString,QVector <QString>>> VectorOFWorldsZones;

    static QMap <QString,QVector<QStringList>> Zones_IDXZYvectorMap;

signals:

public slots:

    QByteArray CheckTheZone(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort);

private:

    QVector <QString> WorldNameVector =
    {
        "Tunaria","Rathe Mountians","Odus","Lavastorm Mountians","Plane of Sky","Secrets"
    };


};

#endif // CHECKTHEZONE_H
