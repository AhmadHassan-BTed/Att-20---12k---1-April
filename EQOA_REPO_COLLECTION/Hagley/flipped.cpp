#include "flipped.h"
#include "packetparsing.h"
#include "packetsize.h"
#include "increment.h"
#include "objectpacket.h"
#include "generator.h"
#include "charcreate.h"
#include "add_or_sub.h"
#include "opcodes.h"
#include "crc.h"
#include "worldthread.h"
#include "checkthezone.h"
#include "mainwindow.h"


flipped::flipped(QObject *parent) : QObject(parent)
{

}

QMap<QByteArray,struct packetparsing::packetvars> flipped::if_itsNOT_flipped(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort)
{

    packetparsing::packetvars NOT_flipped;
    NOT_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    //qDebug() << "IN_if_itsNOT_flipped";

    NOT_flipped.Old_session = NOT_flipped.gamebuf1.mid(12,8);

    //qDebug() << "NOT_flipped.Clients_Last_FB_MessageNumber =" << NOT_flipped.Clients_Last_FB_MessageNumber;

    //qDebug() << "NOT_flipped.PacketReply" << NOT_flipped.PacketReply;
    //qDebug() << "NOT_flipped.ReplyConfirm" << NOT_flipped.ReplyConfirm;

    if(NOT_flipped.ReplyConfirm == "01000200" && NOT_flipped.Old_session != "BF543213" && NOT_flipped.charselectionPassed == false)
    {
        NOT_flipped.charselectionPassed = true;

        //CharSelectScreen
        CharCreate SendTo;

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,NOT_flipped);

        QByteArray holder169 = SendTo.CharSelectScreen(ClientID, ClientsIP, ClientsPort);

        NOT_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

        QByteArray FBsizeOpcode = holder169.toUpper();

        //qDebug() << "NOT_flipped.CharSelectID" << NOT_flipped.CharSelectID;
        //qDebug() << "NOT_flipped.Opcodes::CharID_ServerMessageNumbers" << Opcodes::CharID_ServerMessageNumbers;

        QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(NOT_flipped.CharSelectID);

        QByteArray Servers_Last_FB_MessageNumber = "0200";

        ADD_OR_SUB CurrentItem;

        QString NewFB = CurrentItem.FBSize(FBsizeOpcode);

        increment Text;
        QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
        int index = NewFB.indexOf("XXXX");
        NewFB.replace(index,4,Text2);
        Servers_Last_FB_MessageNumber = Text2.toUpper();

        MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

        //qDebug() << "NOT_flipped.SessionID Char select screen" << NOT_flipped.SessionID;

        holder169 = NewFB.toUtf8().toUpper();
        holder169.insert(0,"0a1f5ae7a0c601f1463213" + NOT_flipped.SessionID + "200200");

        QByteArray Servers_Last_MessageNumber = "0100";
        increment recover1; //get class
        QByteArray recovercount2 = recover1.count(Servers_Last_MessageNumber); //packet increment
        holder169.replace(30,4,recovercount2); //replace new count at 30
        Servers_Last_MessageNumber = recovercount2.toUpper();
        MessNumbVect.replace(0,Servers_Last_MessageNumber);

        Opcodes::CharID_ServerMessageNumbers.insert(NOT_flipped.CharSelectID,MessNumbVect);

        holder169.replace(0,4,NOT_flipped.serverid1); //swap IDs
        holder169.replace(4,4,NOT_flipped.clientid1); //swap IDs

        packetsize ResizePacket; //call size class
        QByteArray ResizePacket2 = ResizePacket.switchsize(holder169); //get new size
        holder169.replace(8,4,ResizePacket2);

        NOT_flipped.Returnflipped.append(holder169.toUpper());
        NOT_flipped.Returnflipped.append("Server_Character_Selection_ ");

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,NOT_flipped);
        NOT_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
    }

    QByteArray Reply1 = NOT_flipped.gamebuf1.mid(22,2);
    QByteArray Reply2 = NOT_flipped.gamebuf1.mid(28,8);

    if(NOT_flipped.LoginStarted != "Started" && Reply1 == "23" && Reply2 == "01000100" && NOT_flipped.PreCharSelect == true)
    {
        //qDebug() << "";
        //qDebug() << "";
        //qDebug() << "Reply1" << Reply1;
        //qDebug() << "";
        //qDebug() << "";

        NOT_flipped.PreCharSelect = false;

        QString Total_Hex1 = QString("%1").arg(Opcodes::SessionsValue,8,16,QLatin1Char('0')).toUpper();
        QString Byte1 = Total_Hex1.mid(0,2);
        QString Byte2 = Total_Hex1.mid(2,2);
        QString Byte3 = Total_Hex1.mid(4,2);
        QString Byte4 = Total_Hex1.mid(6,2);
        QString Total_Hex1_rev = Byte4 + Byte3 + Byte2 + Byte1;

        NOT_flipped.CharSelectID_Main = Total_Hex1_rev;

        Opcodes::ObjectIDsMap.insert(NOT_flipped.CharSelectID,NOT_flipped.CharSelectID_Main);

        Opcodes::SessionsValue++;

        NOT_flipped.SessionID = ConvertSession(Total_Hex1_rev);

        //qDebug() << "NOT_flipped.SessionID" << NOT_flipped.SessionID;

        QByteArray holder166 = packet166;
        holder166.replace(0,4,NOT_flipped.serverid1); //swap IDs
        holder166.replace(4,4,NOT_flipped.clientid1); //swap IDs

        holder166.replace(22,6,NOT_flipped.SessionID);

        NOT_flipped.Returnflipped.append(holder166);
        NOT_flipped.Returnflipped.append("Server_166 ");

        //qDebug() << "Server_166" << holder166;

        NOT_flipped.Servers_Last_MessageNumber1 = "0100";

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,NOT_flipped);
        NOT_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
    }

    if(NOT_flipped.Returnflipped.isEmpty() == false)
    {
        QByteArray Packet = NOT_flipped.Returnflipped.at(0);
        QByteArray Description = NOT_flipped.Returnflipped.at(1);

        crc sendcrc;
        QByteArray outcrc =  sendcrc.elcrc(Packet);
        Packet.append(outcrc);

        QHostAddress current_client_address;
        current_client_address.setAddress(QString::fromUtf8(ClientsIP));
        quint16 current_client_port = ClientsPort.toUShort(nullptr,10);
        QString changeIP = current_client_address.toString();

        QString MainToon = checkthezone::NPCsNames.value(NOT_flipped.CharSelectID);

        QDateTime dateTime = dateTime.currentDateTime();
        QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

        //qDebug() << "";
        //qDebug() << "";
        //qDebug() << "";
        //qDebug() << dateTimeString;

        //qDebug() << "Flipped = false: " + Description;
        //qDebug() << "To Client: " + MainToon;
        //qDebug() << "IP: " + ClientsIP;
        //qDebug() << "Port: " + ClientsPort;
        //qDebug() << "From Server: ";
        //qDebug() << Packet;
        //qDebug() << "";

        QByteArray gameout2 = Packet.fromHex(Packet);

        QHostAddress newsender = QHostAddress(changeIP);
        worldthread::udpsocket3->writeDatagram(gameout2,newsender,current_client_port);

        MainWindow::UiOputVector.append("");
        MainWindow::UiOputVector.append("");
        MainWindow::UiOputVector.append("----------------------------------------------------------");
        MainWindow::UiOputVector.append(dateTimeString);
        MainWindow::UiOputVector.append("Flipped = false: " + Description);
        MainWindow::UiOputVector.append("To Client: " + MainToon);
        MainWindow::UiOputVector.append("IP: " + ClientsIP);
        MainWindow::UiOputVector.append("Port: " + ClientsPort);
        MainWindow::UiOputVector.append("From Server: Packet:");
        MainWindow::UiOputVector.append(Packet);
        MainWindow::UiOputVector.append("----------------------------------------------------------");
        MainWindow::UiOputVector.append("");

        NOT_flipped.Returnflipped.clear();

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,NOT_flipped);
    }

    return packetparsing::IPandPort_AllvariablesMap;
}

void flipped::if_its_flipped(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort)
{

    //qDebug() << "IN_if_its_flipped";

    packetparsing::packetvars IS_flipped;
    IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    //qDebug() << "IS_flipped.CurrentQuestMenuID_17" << IS_flipped.CurrentQuestMenuID;

    QByteArray clientffcall = IS_flipped.gamebuf1.mid(0,8);

    if(clientffcall == "FFFF9209" && IS_flipped.teleporting == true) //FFFF reply
    {
        IS_flipped.teleportmainswitch = false; //turn off fc02 packets
        QByteArray ffsendpacket = packetffff;
        QByteArray clientvalue1 = IS_flipped.gamebuf1.mid(8,4); //get the count from the client
        ffsendpacket.replace(8,4,clientvalue1);
        QByteArray ffffmessage = ffsendpacket.fromHex(ffsendpacket);

        IS_flipped.Returnflipped.append(ffffmessage);
        IS_flipped.Returnflipped.append("Server_ffffmessage");

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);
        IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    }



    if(IS_flipped.teleporting == true && IS_flipped.fortyexist == true) //testing sending  00c9s on leaving teleport/coach if client asks for it
    {

        QByteArray holdergen2 = "";

        holdergen2.insert(0,"d37c5ae7a0c101bf543213" + IS_flipped.SessionID + "13xxxx" + IS_flipped.combine1); //add the header


        packetsize newsize5;
        QByteArray sizeresult2 = newsize5.switchsize(holdergen2); //send to get new size
        holdergen2.replace(8,4,sizeresult2);

        holdergen2.replace(4,4,IS_flipped.clientid1);
        holdergen2.replace(0,4,IS_flipped.serverid1);

        QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IS_flipped.CharSelectID);
        QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

        increment GrpOut;
        QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
        holdergen2.replace(30,4,GrpOut_a);
        Servers_Last_MessageNumber = GrpOut_a.toUpper();

        MessNumbVect.replace(0,Servers_Last_MessageNumber);

        Opcodes::CharID_ServerMessageNumbers.insert(IS_flipped.CharSelectID,MessNumbVect);

        IS_flipped.Returnflipped.append(holdergen2);
        IS_flipped.Returnflipped.append("Server_Teleport_Reply");

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);
        IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    }




    ////////////////////////////Custom Packets////////////////////////////////////////////



    if(IS_flipped.fortyexist == false && IS_flipped.teleporting == false)
    {
        //Normal AutoSpawn

        objectpacket Spawning;




        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);

        QByteArray Status = Spawning.packetswitch(ClientID,ClientsIP,ClientsPort);

        IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

        //qDebug() << "IS_flipped.Map_Channel_ID1" << IS_flipped.Map_Channel_ID;

        if(IS_flipped.ActivateOP.isEmpty() == false)
        {
            for(int ld = 0; ld < IS_flipped.ActivateOP.size(); ld++)
            {
                QByteArray Fullc9 = IS_flipped.ActivateOP.at(ld);

                QByteArray Channel = Fullc9.mid(0,2);
                QVector <QByteArray> CurrenChannelVector = IS_flipped.MapOfChannelVectors.value(Channel);

                CurrenChannelVector.append(Fullc9);
                IS_flipped.MapOfChannelVectors.insert(Channel,CurrenChannelVector);
            }

            IS_flipped.ActivateOP.clear();
            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);
            IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
        }
    }

    if(IS_flipped.fortyexist == true)
    {

        IS_flipped.F8Reply++;

        //qDebug() << "IS_flipped.F8Reply0.0" << IS_flipped.F8Reply;
        //qDebug() << "IS_flipped.opcodeexist" << IS_flipped.opcodeexist;

        if(IS_flipped.opcodeexist == false)
        {
            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);

            IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            QString mainid4 = IS_flipped.CharSelectID;
            generator packet4;
            QByteArray holdergen = packet4.maintoonswitch(mainid4,"",IS_flipped.CharSelectID_Main); //send the id we want to get a 00c9
            holdergen.insert(0,"00c9xxxx00"); //add the header

            IS_flipped.SendZeroChannelVector.append(holdergen);








            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);

            objectpacket Spawning;
            QByteArray Status = Spawning.packetswitch(ClientID,ClientsIP,ClientsPort);

            IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            //qDebug() << "IS_flipped.Map_Channel_ID2" << IS_flipped.Map_Channel_ID;

            if(IS_flipped.ActivateOP.isEmpty() == false)
            {
                for(int ld = 0; ld < IS_flipped.ActivateOP.size(); ld++)
                {
                    QByteArray Fullc9 = IS_flipped.ActivateOP.at(ld);

                    QByteArray Channel = Fullc9.mid(0,2);
                    QVector <QByteArray> CurrenChannelVector = IS_flipped.MapOfChannelVectors.value(Channel);

                    CurrenChannelVector.append(Fullc9);
                    IS_flipped.MapOfChannelVectors.insert(Channel,CurrenChannelVector);
                }

                IS_flipped.ActivateOP.clear();

            }
        }

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);
        IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    }


    //qDebug() << "IS_flipped.F8Reply0.1" << IS_flipped.F8Reply;




    if(IS_flipped.Returnflipped.isEmpty() == false)
    {
        QHostAddress current_client_address;
        current_client_address.setAddress(QString::fromUtf8(ClientsIP));
        quint16 current_client_port = ClientsPort.toUShort(nullptr,10);
        QString changeIP = current_client_address.toString();

        QByteArray Packet = IS_flipped.Returnflipped.at(0);
        QByteArray Description = IS_flipped.Returnflipped.at(1);

        if(Description == "Server_ffffmessage")
        {
            QString MainToon = checkthezone::NPCsNames.value(IS_flipped.CharSelectID);


            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

            //qDebug() << "";
            //qDebug() << "";
            //qDebug() << "";
            //qDebug() << dateTimeString;

            //qDebug() << "Server_ffffmessage: " + Description;
            //qDebug() << "To Client: " + MainToon;
            //qDebug() << "IP: " + ClientsIP;
            //qDebug() << "Port: " + ClientsPort;
            //qDebug() << "From Server: ";
            //qDebug() << Packet;
            //qDebug() << "";

            crc sendcrc;
            QByteArray outcrc =  sendcrc.elcrc(Packet);
            Packet.append(outcrc);

            QByteArray gameout2 = Packet.fromHex(Packet);

            QHostAddress newsender = QHostAddress(changeIP);
            worldthread::udpsocket3->writeDatagram(gameout2,newsender,current_client_port);

            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append(dateTimeString);
            MainWindow::UiOputVector.append("Server_ffffmessage " + Description);
            MainWindow::UiOputVector.append("To Client: " + MainToon);
            MainWindow::UiOputVector.append("IP: " + ClientsIP);
            MainWindow::UiOputVector.append("Port: " + ClientsPort);
            MainWindow::UiOputVector.append("From Server: Packet:");
            MainWindow::UiOputVector.append(Packet);
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append("");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);


        }
        else
        {
            crc sendcrc;
            QByteArray outcrc =  sendcrc.elcrc(Packet);
            Packet.append(outcrc);

            QString MainToon = checkthezone::NPCsNames.value(IS_flipped.CharSelectID);


            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

            //qDebug() << "";
            //qDebug() << "";
            //qDebug() << "";
            //qDebug() << dateTimeString;

            //qDebug() << "Server: " + Description;
            //qDebug() << "To Client: " + MainToon;
            //qDebug() << "IP: " + ClientsIP;
            //qDebug() << "Port: " + ClientsPort;
            //qDebug() << "From Server: ";
            //qDebug() << Packet;
            //qDebug() << "";

            QByteArray gameout2 = Packet.fromHex(Packet);

            QHostAddress newsender = QHostAddress(changeIP);
            worldthread::udpsocket3->writeDatagram(gameout2,newsender,current_client_port);

            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append(dateTimeString);
            MainWindow::UiOputVector.append("Server: " + Description);
            MainWindow::UiOputVector.append("To Client: " + MainToon);
            MainWindow::UiOputVector.append("IP: " + ClientsIP);
            MainWindow::UiOputVector.append("Port: " + ClientsPort);
            MainWindow::UiOputVector.append("From Server: Packet:");
            MainWindow::UiOputVector.append(Packet);
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append("");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);

        }

        IS_flipped.Returnflipped.clear();

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);
    }

    SendOut(ClientID, ClientsIP, ClientsPort);

}

void flipped::SendOut(QByteArray ClientsID,QByteArray ClientsIP,QByteArray ClientsPort)
{
    packetparsing::packetvars IS_flipped;
    IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientsID + ClientsIP + ClientsPort);

    //qDebug() << "IS_flipped.F8Reply1" << IS_flipped.F8Reply;

    QVector <QByteArray> CurrenChannelVector;
    QByteArray Chan = "";

    QMapIterator<QByteArray,QVector<QByteArray>> ChanIter (IS_flipped.MapOfChannelVectors);
    while(ChanIter.hasNext())
    {
        ChanIter.next();
        Chan = ChanIter.key();
        CurrenChannelVector = ChanIter.value();

        if(CurrenChannelVector.isEmpty() == false)
        {
            QByteArray Xor_C9 = CurrenChannelVector.takeAt(0);

            int Chan1 = Chan.toInt(nullptr,16);

            if(Chan1 <= 23)
            {
                IS_flipped.SendChannelsVector.append(Xor_C9);
            }

            IS_flipped.MapOfChannelVectors.insert(Chan,CurrenChannelVector);

            //qDebug() << "IS_flipped.Chan" << Chan;
            //qDebug() << "IS_flipped.CurrenChannelVector" << CurrenChannelVector;
            //qDebug() << "IS_flipped.SendChannelsVector" << IS_flipped.SendChannelsVector;
        }
    }






    if(IS_flipped.SendZeroChannelVector.isEmpty() == false || IS_flipped.Send42ECsVector.isEmpty() == false || IS_flipped.SendChannelsVector.isEmpty() == false)
    {
        QByteArray Send_Channels = "";

        if(IS_flipped.SendZeroChannelVector.isEmpty() == false)
        {
            QByteArray holdergen = IS_flipped.SendZeroChannelVector.takeAt(0);

            increment main_chan0;
            QByteArray maintoonchan0 = main_chan0.count(IS_flipped.zerocounter);
            holdergen.replace(4,4,maintoonchan0);
            IS_flipped.zerocounter = maintoonchan0.toUpper();

            Send_Channels.append(holdergen);

            IS_flipped.SortChanelsValuesVector.replace(0,IS_flipped.zerocounter);

            IS_flipped.incrementmap.insert("00",IS_flipped.zerocounter);
        }

        if(IS_flipped.Send42ECsVector.isEmpty() == false)
        {
            QByteArray StatHeader = "";
            StatHeader.append(IS_flipped.Send42ECsVector.takeAt(0));

            increment Text;
            QByteArray Text2 = Text.count(IS_flipped.Channel42messnum);
            StatHeader.replace(4,4,Text2);
            IS_flipped.Channel42messnum = Text2.toUpper();

            IS_flipped.incrementmap.insert("42",IS_flipped.Channel42messnum);

            Send_Channels.append(StatHeader);

            int index = IS_flipped.SortChanelsNumbersVector.indexOf("42");
            IS_flipped.SortChanelsValuesVector.replace(index,IS_flipped.Channel42messnum);
        }



        if(IS_flipped.SendChannelsVector.isEmpty() == false)
        {
            int sizeOf = IS_flipped.SendChannelsVector.size();

            for(int sc = 0; sc < IS_flipped.SendChannelsVector.size(); sc++)
            {
                QByteArray ChannelUpdate = IS_flipped.SendChannelsVector.at(sc);

                QString Current_Channel = ChannelUpdate.mid(0,2).toUpper();

                QByteArray ChanCount = IS_flipped.incrementmap.value(Current_Channel);

                increment otherChannels;
                QByteArray ChansNewCount = otherChannels.count(ChanCount);
                ChannelUpdate.replace(4,4,ChansNewCount);

                IS_flipped.incrementmap.insert(Current_Channel,ChansNewCount);

                Send_Channels.append(ChannelUpdate);

                int index = IS_flipped.SortChanelsNumbersVector.indexOf(Current_Channel.toUtf8());
                IS_flipped.SortChanelsValuesVector.replace(index,ChansNewCount);
            }

            for(int i = 0; i < sizeOf; i++)
            {
                if(IS_flipped.SendChannelsVector.isEmpty() == false)
                {
                    IS_flipped.SendChannelsVector.removeAt(0);
                }
            }
        }




        IS_flipped.ServersCheckChannelConfirm.clear();

        for(int re = 0; re < IS_flipped.SortChanelsValuesVector.size(); re++)
        {
            QByteArray ChannelsValue = IS_flipped.SortChanelsValuesVector.at(re);
            QByteArray Channel = IS_flipped.SortChanelsNumbersVector.at(re);



            if(ChannelsValue != "0000")
            {
                IS_flipped.ServersCheckChannelConfirm.append(Channel + ChannelsValue);
                IS_flipped.SortChanelsValuesVector.replace(re,"0000");
            }
        }


        if(Send_Channels.isEmpty() == false)
        {
            Send_Channels.insert(0,"D37C5AE7A0C101BF543213" + IS_flipped.SessionID + "00xxxx");
            Send_Channels.replace(0,4,IS_flipped.serverid1);
            Send_Channels.replace(4,4,IS_flipped.clientid1);

            //qDebug() << "IS_flipped.F8Reply2" << IS_flipped.F8Reply;

            if(IS_flipped.F8Reply > 0)
            {
                IS_flipped.F8Reply--;
                Send_Channels.replace(28,2,"13");
                Send_Channels.insert(34,IS_flipped.combine1);


            }

            //qDebug() << "IS_flipped.F8Reply3" << IS_flipped.F8Reply;

            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IS_flipped.CharSelectID);
            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

            increment GrpOut;
            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
            Send_Channels.replace(30,4,GrpOut_a);
            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            Opcodes::CharID_ServerMessageNumbers.insert(IS_flipped.CharSelectID,MessNumbVect);

            packetsize ResizePacket;
            QByteArray ResizePacket2 = ResizePacket.switchsize(Send_Channels);
            Send_Channels.replace(8,4,ResizePacket2.toUpper());

            QHostAddress current_client_address;
            current_client_address.setAddress(QString::fromUtf8(ClientsIP));
            quint16 current_client_port = ClientsPort.toUShort(nullptr,10);
            QString changeIP = current_client_address.toString();

            crc sendcrc;
            QByteArray outcrc =  sendcrc.elcrc(Send_Channels);
            Send_Channels.append(outcrc);


            QString MainToon = checkthezone::NPCsNames.value(IS_flipped.CharSelectID);


            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

            //qDebug() << "";
            //qDebug() << "";
            //qDebug() << "";
            //qDebug() << dateTimeString;

            //qDebug() << "Send_Channels: " << IS_flipped.ServersCheckChannelConfirm;
            //qDebug() << "To Client: " << MainToon;
            //qDebug() << "IP: " << ClientsIP;
            //qDebug() << "Port: " << ClientsPort;
            //qDebug() << "From Server: ";
            //qDebug() << Send_Channels;
            //qDebug() << "";

            QByteArray gameout2 = Send_Channels.fromHex(Send_Channels);

            QHostAddress newsender = QHostAddress(changeIP);
            worldthread::udpsocket3->writeDatagram(gameout2,newsender,current_client_port);

            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append(dateTimeString);
            MainWindow::UiOputVector.append("Send_Channels: " + IS_flipped.ServersCheckChannelConfirm);
            MainWindow::UiOputVector.append("To Client: " + MainToon);
            MainWindow::UiOputVector.append("IP: " + ClientsIP);
            MainWindow::UiOputVector.append("Port: " + ClientsPort);
            MainWindow::UiOputVector.append("From Server: Packet:");
            MainWindow::UiOputVector.append(Send_Channels);
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append("");
        }

        packetparsing::IPandPort_AllvariablesMap.insert(ClientsID + ClientsIP + ClientsPort,IS_flipped);
        IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientsID + ClientsIP + ClientsPort);
    }

    if(IS_flipped.Disconnected == false)
    {


        ////////////////Send FBs////////////////////////////////////////////////////////////////

        if(IS_flipped.SendFBsVector.isEmpty() == false)
        {
            QString Description = "";
            QByteArray Send_FB = "D37C5AE7A0C101xxXXxxXX" + IS_flipped.SessionID + "00xxxx";
            QByteArray Reply_or_Not = "";
            QByteArray PacketDescription = "";

            QHostAddress current_client_address;
            current_client_address.setAddress(QString::fromUtf8(ClientsIP));
            quint16 current_client_port = ClientsPort.toUShort(nullptr,10);
            QString changeIP = current_client_address.toString();

            int sizeOf = IS_flipped.SendFBsVector.size();

            //qDebug() << "SendFBsVector.size() = " << sizeOf;

            for(int st = 0; st < IS_flipped.SendFBsVector.size(); st++)
            {

                Description = IS_flipped.FBPacketDescriptionVector.at(st);
                PacketDescription.append(Description.toUtf8());

                //qDebug() << "IN_SendFBsVector.Description = " << Description;

                QString Reply = IS_flipped.FBReplyOrNotVector.at(st);
                Reply_or_Not.append(Reply.toUtf8());

                if(Reply_or_Not.contains("03"))
                {
                    Reply_or_Not = "03";
                }

                if(Reply_or_Not.contains("13"))
                {
                    Reply_or_Not = "13";
                }
                else
                {
                    if(Reply_or_Not.contains("00"))
                    {
                        Reply_or_Not = "00";
                    }
                }



                QString SendAllFBs = IS_flipped.SendFBsVector.at(st);



                ADD_OR_SUB CurrentItem;
                QString NewFB = CurrentItem.FBSize(SendAllFBs);

                QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IS_flipped.CharSelectID);
                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                increment Text;
                QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                int index = NewFB.indexOf("XXXX");
                NewFB.replace(index,4,Text2);
                Servers_Last_FB_MessageNumber = Text2.toUpper();

                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);



                Opcodes::CharID_ServerMessageNumbers.insert(IS_flipped.CharSelectID,MessNumbVect);


                //qDebug() << "IN_SendFBsVector.NewFB = " << NewFB;

                Send_FB.append(NewFB.toUtf8());//merge all FBs
            }



            for(int i = 0; i < sizeOf; i++)
            {
                IS_flipped.FBPacketDescriptionVector.removeAt(0);
                IS_flipped.FBReplyOrNotVector.removeAt(0);
                IS_flipped.SendFBsVector.removeAt(0);
            }

            Send_FB.replace(0,4,IS_flipped.serverid1);
            Send_FB.replace(4,4,IS_flipped.clientid1);

            Send_FB.replace(22,6,IS_flipped.SessionID);

            if(IS_flipped.PacketReply == "00")
            {
                Reply_or_Not = "03";
            }

            if(IS_flipped.PacketReply == "20")
            {
                Reply_or_Not = "23";
            }

            QVector<QByteArray> ReplyVector =
            {
                "03","13","23","63"
            };

            int index = 1234567890;

            if(Reply_or_Not == "23")
            {
                index = Send_FB.indexOf("xxXXxxXX");
                Send_FB.replace(index,8,"F1463213");
            }

            if(Reply_or_Not == "03" || Reply_or_Not == "13"
                    || Reply_or_Not == "00" || Reply_or_Not == "20")
            {
                index = Send_FB.indexOf("xxXXxxXX");

                if(IS_flipped.Server_2A00_reply == true)
                {
                    Send_FB.replace(index,8,"BF543213");
                }
                else
                {
                    Send_FB.replace(index,8,"F1463213");
                }
            }

            //qDebug() << "Reply_or_Not" << Reply_or_Not;
            //qDebug() << "index" << index;

            //qDebug() << "IS_flipped.F8Reply4" << IS_flipped.F8Reply;

            if(IS_flipped.F8Reply > 0)
            {
                IS_flipped.F8Reply--;
                Reply_or_Not = "13";
            }

            //qDebug() << "IS_flipped.F8Reply5" << IS_flipped.F8Reply;

            if(ReplyVector.contains(Reply_or_Not))
            {
                Send_FB.replace(28,2,Reply_or_Not);

                if(Reply_or_Not == "13")
                {
                    Send_FB.insert(34,IS_flipped.combine1);
                }
                else
                {
                    Send_FB.insert(34,IS_flipped.Clients_Last_MessageNumber);
                    Send_FB.insert(38,IS_flipped.Clients_Last_FB_MessageNumber);
                }
            }




            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IS_flipped.CharSelectID);
            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

            increment GrpOut;
            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
            Send_FB.replace(30,4,GrpOut_a);
            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            Opcodes::CharID_ServerMessageNumbers.insert(IS_flipped.CharSelectID,MessNumbVect);

            packetsize ResizePacket;
            QByteArray ResizePacket2 = ResizePacket.switchsize(Send_FB);
            Send_FB.replace(8,4,ResizePacket2.toUpper());

            IS_flipped.ResendFBsMap.insert(IS_flipped.opcoderesult1,Send_FB);


            crc sendcrc;
            QByteArray outcrc =  sendcrc.elcrc(Send_FB);
            Send_FB.append(outcrc);

            //qDebug() << "IS_flipped.Send_FB" << Send_FB;



            QByteArray ClientID = IS_flipped.gamebuf1.mid(0,4);
            packetparsing::packetvars p9;

            if(packetparsing::IPandPort_AllvariablesMap.contains(ClientID + ClientsIP + ClientsPort))
            {
                p9 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
            }

            QString MainToon = checkthezone::NPCsNames.value(p9.CharSelectID);


            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

            qDebug() << "";
            qDebug() << "";
            qDebug() << "";
            qDebug() << dateTimeString;

            qDebug() << "IS_flipped.Send_FB: " << Description;
            qDebug() << "To Client: " << MainToon;
            qDebug() << "IP: " << ClientsIP;
            qDebug() << "Port: " << ClientsPort;
            qDebug() << "From Server: ";
            qDebug() << Send_FB;
            qDebug() << "";

            IS_flipped.opcodeexist = false;

            QByteArray gameout2 = Send_FB.fromHex(Send_FB);

            QHostAddress newsender = QHostAddress(changeIP);
            worldthread::udpsocket3->writeDatagram(gameout2,newsender,current_client_port);

            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append(dateTimeString);
            MainWindow::UiOputVector.append("Send_FB: " + Description);
            MainWindow::UiOputVector.append("To Client: " + MainToon);
            MainWindow::UiOputVector.append("IP: " + ClientsIP);
            MainWindow::UiOputVector.append("Port: " + ClientsPort);
            MainWindow::UiOputVector.append("From Server: Packet:");
            MainWindow::UiOputVector.append(Send_FB);
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append("");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IS_flipped);
            IS_flipped = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

        }
    }
    else
    {
        packetparsing::IPandPort_AllvariablesMap.remove(ClientsID + ClientsIP + ClientsPort);

    }


}

QByteArray flipped::ConvertSession(QString SessionID1)
{
    //qDebug() << "";
    //qDebug() << "ObjectID to SessionID";
    //qDebug() << "SessionID1" << SessionID1;


    QString byte1 = SessionID1.mid(0,2);
    QString byte2 = SessionID1.mid(2,2);
    QString byte3 = SessionID1.mid(4,2);
    QString byte4 = SessionID1.mid(6,2);

    QString CharSelectID_rev = byte4 + byte3 + byte2 + byte1;

    QByteArray objectid = CharSelectID_rev.toUtf8();

    int object1;

    object1 = objectid.toInt(nullptr,16); //this needs to be input as reversed byte order ( dudderz 035e59)

    QByteArray test2;
    QByteArray test3;

    do
    {
        //qDebug() << "object1" << object1;

        uchar lower7bits = (uchar)(object1 & 0x7f);
        object1 >>= 7;

        if (object1 > 0)

            lower7bits |= 128;
        uchar add(lower7bits);
        test2.append(add);

        //qDebug() << "add" << add;
        //qDebug() << "Val" << test2.toHex().toUpper();
        //qDebug() << "lower7bits" << lower7bits;


        test3.append(lower7bits);


    } while (object1 > 0);

    //qDebug() << "test3" << test3.toHex().toUpper();


    //qDebug() << "SessionID2" << test2;

    return test2.toHex().toUpper();
}
