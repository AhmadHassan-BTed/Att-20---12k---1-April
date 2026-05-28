#ifndef FLIPPED_H
#define FLIPPED_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QStack>
#include <QByteArray>
#include "packetparsing.h"

class flipped : public QObject
{
    Q_OBJECT
public:
    explicit flipped(QObject *parent = nullptr);

    QHostAddress mainip;
//    QHostAddress PublicMainip;
//    QByteArray ClientsID = "";
//    QByteArray ClientsIP = "";
//    QByteArray ClientsPort = "";
//    QByteArray ClientsPacket = "";
//    QByteArray ReplyConfirm = "";

    QString FileName = "C:/PCSX2/" + QString::number(0) + ".txt";

signals:

public slots:

    QMap<QByteArray,struct packetparsing::packetvars> if_itsNOT_flipped(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort);
    QByteArray ConvertSession(QString SessionID1);
    void if_its_flipped(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort);
    void SendOut(QByteArray ClientsID,QByteArray ClientsIP,QByteArray ClientsPort);

private:
    const QByteArray packetfc1 = "d37c123487c001bf543213######002100fc02d007";
    const QByteArray packetffff = "FFFF930901000000";

    const QByteArray packet166 = "0a1f123497c021f1463213######200100"
                                 "fb060100d10703000000"
                                 "fb060200f5071b000000";

protected:

private slots:

};

#endif // FLIPPED_H
