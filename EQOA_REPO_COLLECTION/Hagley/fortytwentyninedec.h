#ifndef FORTYTWENTYNINEDEC_H
#define FORTYTWENTYNINEDEC_H

#include <QObject>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QByteArray>
#include <QVector>
#include <QMap>
#include <QStringList>
#include <QString>
#include <QDebug>
#include "packetparsing.h"

class fortytwentyninedec : public QObject
{
    Q_OBJECT
public:
    explicit fortytwentyninedec(QObject *parent = nullptr);

    QHostAddress mainip;
//    QHostAddress PublicMainip;

//    QByteArray ClientsID = "";
//    QByteArray ClientsIP = "";
//    QByteArray ClientsPort = "";
//    QByteArray ClientsPacket = "";

    static QVector<QByteArray> UpdateMapsVector;

    QString FileName = "C:/PCSX2/" + QString::number(0) + ".txt";


    QString directory = QCoreApplication::applicationDirPath();

    QString Database = "/Master_Database.db";
    QSqlDatabase db = QSqlDatabase::database();

private:




    int where2 = 0;
    int carata = 0;
    int caratb = 0;
    int facedeconumber1 = 0;
    int currentxor = 0;
    QByteArray decom40 = "";
    int high2 = 0;
    int low2 = 0;
    QByteArray decopacket1 = "";
    int packetsize2 = 0;

    int xorresult1 = 0;
    int xorresult2 = 0;
    int xorresult3 = 0;
    int res1 = 0;
    int res2 = 0;
    int res3 = 0;
    int xdeconumber1 = 0;
    int ydeconumber1 = 0;
    int zdeconumber1 = 0;

    int faceresult1 = 0;
    int res4 = 0;

    const QByteArray packet273 = "d37c12349dc001bf543213######002900fb16XXXX9007ffffffffffffffff020000005AE73b1d69776218";

    const QByteArray twoseventwo = "0a1f123485c401bf543213######13xxxxXXXXxxxxXXxxxxF800c9020000";

    const QByteArray twosevenfive = "d37c1234aec301bf543213######002a0000c9030000";

    const QByteArray packetcustom2 = "d37c1234a0c101bf543213######13280022000600400200f8"; //00c9 header

    const QByteArray twosevenone = "0a1f123485c401bf543213######13xxxxXXXXxxxxXXxxxxF800c9010000";

    QByteArray twosevenoneadd1 =
            "42ec"
            "0100"
            "00"
            ;

    QByteArray Correct55550d41 = "0214050100000001000000000000000000000055550d41000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";



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

signals:

public slots:
    QByteArray fortydec_res(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QByteArray gamebuf1_Cut);

protected:


private slots:
};

#endif // FORTYTWENTYNINEDEC_H
