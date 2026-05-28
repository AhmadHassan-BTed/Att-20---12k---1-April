#ifndef LOOT_H
#define LOOT_H

#include <QObject>
#include<QVector>
#include<QString>
#include<QString>
#include<QDebug>
#include<QMap>

class Loot : public QObject
{
    Q_OBJECT
public:
    explicit Loot(QObject *parent = nullptr);
    static QVector <QString> Loot_list;
    static QMap <QString, QVector <QString>> Loot_List_Map;
    static QMap<QString,int> LootIDs_PacketIDsMap;

    QString change_inventory = "";
    int total_size = 0;
    QString total_packet = "";

signals:

public slots:
    QString Loot_packet(QString Mob_id);
    bool AddLoot(QByteArray ID_IP_PORT, QString ItemsID, QString Quantity);

};

#endif // LOOT_H
