#ifndef MERCHANTS_H
#define MERCHANTS_H

#include <QObject>
#include<QVector>
#include<QByteArray>
#include<QString>
#include<QDebug>
#include<QMap>

class merchants : public QObject
{
    Q_OBJECT
public:
    explicit merchants(QObject *parent = nullptr);

    static QMap<QString,int> MerchantIDs_PacketIDsMap;
    static QMap <QString,QVector <QString>> MasterItems_Vector_Map;
    static QMap<QString,QString> Items_Map;
    static QMap <QString, QVector <QString>> Merchant_List_Map;
    static QMap <QString,QString> Merchant_Text_Map;

    QByteArray OpcodeAndheader = "B700xxxxxxxxC801C8011E0F000000"; //this is just to compute size of the packets when larger than 8404, or single packet
    QByteArray fa_largeheader = "FAFF8404XXXX";
    QByteArray fb_large_header = "FBFFssssXXXX"; //greater than 255 header
    QByteArray fb_small_header = "FBssXXXX"; //less than 255 smallheader
    QByteArray change_inventory = "";

    QString merch_text_header = "";
    int total_size = 0;
    int working_size = 0;
    int packet_count = 0;
    QByteArray total_packet = "";
    QMap <int,QByteArray> merch_output_map;
    int merch_carat1 = 0;


signals:

public slots:
         QMap <int,QByteArray> mer_packet(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QByteArray merch_id1); //call mer packet with npc's object id and return a map of the packets to send back, keys are 0,1,2 etc
          QString Text_to_Unicode(QString text);
};

#endif // MERCHANTS_H
