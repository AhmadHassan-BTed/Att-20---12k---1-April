#include "myrunnable.h"
#include "mainwindow.h"
#include "flipped.h"
#include "add_or_sub.h"
#include "convert.h"
#include "generator.h"
#include "combat.h"
#include "increment.h"
#include "coaching.h"
#include "xorpacket.h"
#include "merchants.h"
#include "objectpacket.h"
#include "targeting.h"
#include "charcreate.h"
#include "facingdirection.h"
#include "packetparsing.h"
#include "opcodes.h"
#include "fortytwentyninedec.h"
#include "crc.h"
#include "loadingcharacter.h"
#include "packetsize.h"
#include "worldthread.h"


MyRunnable::MyRunnable(QObject *parent) : QObject(parent)
{
    poolPacketParse = new QThreadPool(this);
    poolPacketParse->setMaxThreadCount(50);
}

void MyRunnable::run()
{

    QByteArray clientffcall = ClientsPacket.mid(0,8).toUpper();

    if(ClientsID != "FFFF" && ClientsPacket.size() > 24)
    {

        // //////////////Adding the port to the key///////////////////////////////////////////
        if(packetparsing::IPandPort_AllvariablesMap.contains(ClientsID + ClientsIP))
        {
            //adding port to key
            //if the key doesnt contain the port
            packetparsing::packetvars pSWAP;

            pSWAP = packetparsing::IPandPort_AllvariablesMap.value(ClientsID + ClientsIP);

            pSWAP.clientid1 = ClientsPacket.mid(0,4);
            pSWAP.serverid1 = ClientsPacket.mid(4,4);

            qDebug() << "";
            qDebug() << "pSWAP.SessionID = " << pSWAP.SessionID;
            qDebug() << "pSWAP.PreCharSelect = " << pSWAP.PreCharSelect;
            qDebug() << "";

            packetparsing::IPandPort_AllvariablesMap.remove(ClientsID + ClientsIP);
            packetparsing::IPandPort_AllvariablesMap.insert(ClientsID + ClientsIP + ClientsPort,pSWAP);
        }

        packetparsing::packetvars IN_MyRunnable;
        IN_MyRunnable = packetparsing::IPandPort_AllvariablesMap.value(ClientsID + ClientsIP + ClientsPort);


        ///////////////////Packet parsing///////////////////////////////////////////////
//        packetparsing *task = new packetparsing();
//        task->setAutoDelete(true);
//        task->mainip = mainip;
//        task->PublicMainip = PublicMainip;
//        task->ClientsID = ClientsID;
//        task->ClientsIP = ClientsIP;
//        task->ClientsPort = ClientsPort;
//        task->ClientsPacket = ClientsPacket;
//        poolPacketParse->start(task);
        packetparsing sendto;

        packetparsing::IPandPort_AllvariablesMap = sendto.ParseThePacket( ClientsID,  ClientsIP,  ClientsPort,  ClientsPacket, packetparsing::IPandPort_AllvariablesMap);
        IN_MyRunnable = packetparsing::IPandPort_AllvariablesMap.value(ClientsID + ClientsIP + ClientsPort);

        ///////////////////Packet parsing///////////////////////////////////////////////


    }
    else
    {
        if(clientffcall == "FFFF9209") //FFFF reply
        {
            QByteArray ffsendpacket = packetffff;
            QByteArray clientvalue1 = ClientsPacket.mid(8,4); //get the count from the client
            ffsendpacket.replace(8,4,clientvalue1);
            QByteArray ffffmessage = ffsendpacket.fromHex(ffsendpacket);

            QHostAddress current_client_address;
            current_client_address.setAddress(QString::fromUtf8(ClientsIP));
            quint16 current_client_port = ClientsPort.toUShort(nullptr,10);
            QString changeIP = current_client_address.toString();



            packetparsing::packetvars IN_MyRunnable;

            if(packetparsing::IPandPort_AllvariablesMap.contains(ClientsID + ClientsIP + ClientsPort))
            {
                IN_MyRunnable = packetparsing::IPandPort_AllvariablesMap.value(ClientsID + ClientsIP + ClientsPort);
            }

            QString MainToon = Opcodes::NPCsNames.value(IN_MyRunnable.CharSelectID);


            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

            qDebug() << "";
            qDebug() << "";
            qDebug() << "";
            qDebug() << dateTimeString;

            qDebug() << "FFFF_reply: " << ClientsPacket;
            qDebug() << "To Client: " << MainToon;
            qDebug() << "IP: " << ClientsIP;
            qDebug() << "Port: " << ClientsPort;
            qDebug() << "From Server: ";
            qDebug() << ffsendpacket;
            qDebug() << "";

            QHostAddress newsender = QHostAddress(changeIP);
            worldthread::udpsocket3->writeDatagram(ffffmessage,newsender,current_client_port);
        }
    }
}
