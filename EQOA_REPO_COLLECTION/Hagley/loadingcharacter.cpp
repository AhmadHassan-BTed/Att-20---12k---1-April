#include "loadingcharacter.h"
#include "packetparsing.h"
#include "increment.h"
#include "packetsize.h"
#include "opcodes.h"
#include "worldthread.h"
#include "crc.h"
#include "login.h"
#include "add_or_sub.h"
#include "generator.h"
#include "charcreate.h"
#include "checkthezone.h"
#include "merchants.h"
#include "convert.h"

QUdpSocket *worldthread::udpsocket3;

loadingcharacter::loadingcharacter(QObject *parent) : QObject(parent)
{

}

QVector<QByteArray> loadingcharacter::LoadData(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QByteArray ClientsPacket, QHostAddress mainip)
{
    packetparsing::packetvars IN_loadingcharacter;
    IN_loadingcharacter = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QVector <QByteArray> LoadingVector;

    IN_loadingcharacter.gamebuf1 = ClientsPacket;

    QByteArray ReplyConfirm = "";

    if(IN_loadingcharacter.DoubleSession == true)
    {
        IN_loadingcharacter.DoubleSession = false;
        ReplyConfirm = IN_loadingcharacter.ReplyConfirm.toUpper();
    }
    else
    {
        if(IN_loadingcharacter.PacketReply == "63")
        {
            ReplyConfirm = IN_loadingcharacter.gamebuf1.mid(40,8).toUpper();
        }
        if(IN_loadingcharacter.PacketReply == "23" || IN_loadingcharacter.PacketReply == "03" ||
                IN_loadingcharacter.PacketReply == "13"  || IN_loadingcharacter.PacketReply == "0B" || IN_loadingcharacter.PacketReply == "1B")
        {
            ReplyConfirm = IN_loadingcharacter.gamebuf1.mid(32,8).toUpper();
        }
    }


    qDebug() << "";

    if(earlypacket_map.contains(ReplyConfirm))
    {
        qDebug() << "";
        qDebug() << "ReplyConfirm was in the map" << ReplyConfirm;

        QByteArray holderpmap = earlypacket_map.value(ReplyConfirm);

        holderpmap.replace(0,4,IN_loadingcharacter.serverid1);
        holderpmap.replace(4,4,IN_loadingcharacter.clientid1);

        if(ReplyConfirm == "1B001C00")
        {
            if(IN_loadingcharacter.StartUnSpentCMs.size() == 2)
            {
                int StartUnSpentCMs1 = IN_loadingcharacter.StartUnSpentCMs.toInt(nullptr,16);
                StartUnSpentCMs1 = StartUnSpentCMs1 + 0x80;
                QString StartUnSpentCMs2 = QString("%1").arg(StartUnSpentCMs1,2,16,QLatin1Char('0'));
                IN_loadingcharacter.StartUnSpentCMs = StartUnSpentCMs2 + "00";
                holderpmap.replace(1992,4,IN_loadingcharacter.StartUnSpentCMs.toUtf8());
            }

            if(IN_loadingcharacter.StartSpentCMs.size() == 2)
            {
                int StartSpentCMs1 = IN_loadingcharacter.StartSpentCMs.toInt(nullptr,16);
                StartSpentCMs1 = StartSpentCMs1 + 0x80;
                QString StartSpentCMs2 = QString("%1").arg(StartSpentCMs1,2,16,QLatin1Char('0'));
                IN_loadingcharacter.StartSpentCMs = StartSpentCMs2 + "00";
                holderpmap.replace(1996,4,IN_loadingcharacter.StartSpentCMs.toUtf8());
            }

//            QString hexip = QString("%1").arg(QHostAddress(mainip).toIPv4Address(), 8, 16, QLatin1Char( '0' ));
//            QByteArray outip = hexip.toUtf8();

//            QByteArray holdip1 = outip;
//            QByteArray first3 =  holdip1.mid(0,2);
//            QByteArray second3 = holdip1.mid(2,2);
//            QByteArray third3 =  holdip1.mid(4,2);
//            QByteArray fourth3 = holdip1.mid(6,2);
//            QByteArray bigendianip2 = fourth3+= third3 += second3+= first3;
//            holderpmap.replace(2270,8,bigendianip2);
        }

        if(ReplyConfirm == "13001400")
        {
            holderpmap.replace(46,8,IN_loadingcharacter.CharID_5bytes);
        }


        QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_loadingcharacter.CharSelectID);
        QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

        increment GrpOut;
        QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
        holderpmap.replace(30,4,GrpOut_a);
        Servers_Last_MessageNumber = GrpOut_a.toUpper();

        MessNumbVect.replace(0,Servers_Last_MessageNumber);

        Opcodes::CharID_ServerMessageNumbers.insert(IN_loadingcharacter.CharSelectID,MessNumbVect);

        packetsize ResizePacket; //call size class
        QByteArray ResizePacket2 = ResizePacket.switchsize(holderpmap); //get new size
        holderpmap.replace(8,4,ResizePacket2);

        QByteArray Byte1 =  Servers_Last_MessageNumber.mid(0,2);
        QByteArray Byte2 = Servers_Last_MessageNumber.mid(2,2);
        QByteArray revNumber = Byte2 + Byte1;

        int PacketNumber = revNumber.toInt(nullptr,16);

        LoadingVector.append(holderpmap);
        LoadingVector.append("Server_PacketMap_" + QString::number(PacketNumber).toUtf8());


        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_loadingcharacter);
    }
    else
    {
        qDebug() << "";
        qDebug() << "ReplyConfirm did not contain" << ReplyConfirm;
    }


    return LoadingVector;
}
