#include "opcodes.h"
#include "packetparsing.h"
#include "increment.h"
#include "packetsize.h"
#include "add_or_sub.h"
#include "convert.h"
#include "coaching.h"
#include "targeting.h"
#include "generator.h"
#include "objectpacket.h"
#include "checkthezone.h"
#include "charcreate.h"
#include "login.h"
#include "merchants.h"
#include "xorpacket.h"
#include "combat.h"
#include "facingdirection.h"
#include "worldthread.h"
#include "crc.h"
#include <QRandomGenerator>
#include "mainwindow.h"
#include "quests.h"
#include "loot.h"
#include "flipped.h"



Opcodes::Opcodes(QObject *parent) : QObject(parent)
{

}

void Opcodes::RemoveFromGame(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort)
{

    packetparsing::packetvars IN_Opcodes;
    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QVector<QVector<QTimer*>> VectorOfQtimerVectors =
    {
        MainWindow::OutgoingTimersVector,
        MainWindow::CheckForAggroMovementTimersVector,
        MainWindow::GroupTimersVector,
        MainWindow::TeleportTimersVector,
        MainWindow::CoachTimersVector,
        MainWindow::UpdateMapsTimersVector,
        MainWindow::LoS_TimersVector,
        MainWindow::MySwingTimersVector,
        MainWindow::MobSwingTimersVector,
        MainWindow::SpawningTimersVector,
        MainWindow::reSpawningTimersVector,
        MainWindow::RoamingTimersVector,
        MainWindow::RegenTimersVector,
        MainWindow::reSend42ecTimersVector,
        MainWindow::CheckForAggroTimersVector,
        MainWindow::reSendFBsTimersVector,
        MainWindow::reSendChannelsTimersVector,
    };

    for(int qt = 0; qt < VectorOfQtimerVectors.size(); qt++)
    {
        QVector<QTimer*> CurrentTimerVector = VectorOfQtimerVectors.at(qt);

        for(int cc = 0; cc < CurrentTimerVector.size(); cc++) //remove main toon first
        {
            auto* receivedtimer = CurrentTimerVector.at(cc);

            QString  ObjectID = receivedtimer->objectName();

            if(ObjectID == ClientID + ClientsIP + ClientsPort)
            {
                if(CurrentTimerVector.contains(receivedtimer))
                {
                    receivedtimer->stop();
                    receivedtimer->deleteLater();
                    CurrentTimerVector.removeAt(cc);
                }
            }
        }
    }


    if(db.isOpen())
    {
        db.close();
        db.removeDatabase(QSqlDatabase::defaultConnection);
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(Opcodes::m_FileName);
    db.open();

    /////////////////////////Save Location///////////////////////////

    QString tablesel3 = "MAINTOON";
    QString objecttlpt3 = "ObjectID (4)";
    QString xtlpt3 = "X (3)";
    QString ytlpt3 = "Y (3)";
    QString ztlpt3 = "Z (3)";
    QString facing1 = "Facing (1)";
    QString facing = IN_Opcodes.masterf.toUpper();
    QString tlptid3 = IN_Opcodes.CharSelectID;
    QString x13 = QString::fromUtf8(IN_Opcodes.masterx);
    QString y13 = QString::fromUtf8(IN_Opcodes.mastery);
    QString z13 = QString::fromUtf8(IN_Opcodes.masterz);

    db.transaction();

    QSqlQuery querytlpt3;
    querytlpt3.prepare(QStringLiteral("UPDATE %1 SET [%2] = :val2,[%3] = :val3,[%4] = :val4,[%5] = :val5 WHERE [%6] = :val").arg(tablesel3).arg(xtlpt3).arg(ytlpt3).arg(ztlpt3).arg(facing1).arg(objecttlpt3));

    querytlpt3.bindValue(":val",tlptid3);
    querytlpt3.bindValue(":val2",x13.toUpper());
    querytlpt3.bindValue(":val3",y13.toUpper());
    querytlpt3.bindValue(":val4",z13.toUpper());
    querytlpt3.bindValue(":val5",facing.toUpper());

    querytlpt3.exec();

    db.commit();

    /////////////////////Remove from other players channels/////////////////////////////
    QMapIterator<QByteArray,struct packetparsing::packetvars> StructIter (packetparsing::IPandPort_AllvariablesMap);
    while(StructIter.hasNext())
    {
        packetparsing::packetvars p99;

        StructIter.next();
        //QByteArray ID_IP_PORT = StructIter.key();
        p99 = StructIter.value();//all other players structs

        QString p99_PlayersName = checkthezone::NPCsNames.value(p99.CharSelectID);
        QString DepartingPlayersName = checkthezone::NPCsNames.value(IN_Opcodes.CharSelectID);

        qDebug() << "p99_PlayersName: " + p99_PlayersName;
        qDebug() << "DepartingPlayersName: " + DepartingPlayersName;

        if(p99.CharSelectID.isEmpty() == false && p99.flipped == true && IN_Opcodes.CharSelectID != p99.CharSelectID)
        {
            QString Channel = p99.Map_Channel_ID.key(IN_Opcodes.CharSelectID);

            qDebug() << "Departing_Players_current_channel: " + Channel;

            if(!objectpacket::DeadListVector.contains(IN_Opcodes.CharSelectID))
            {
                qDebug() << "Id was not in the DeadListVector ";
                objectpacket::DeadListVector.append(IN_Opcodes.CharSelectID);
            }
            else
            {
                qDebug() << "Id was alreay in the DeadListVector ";
            }


            if(Channel.isEmpty() == false)
            {
                if(p99.Map_Channel_ID.contains(Channel))//check if i am in their spawned channels.
                {
                    QByteArray Combined1 = Channel.toUtf8() + "c9xxxx01100180c800";

                    QByteArray ChanCount = p99.incrementmap.value(Channel);

                    increment otherChannels;
                    QByteArray ChansNewCount = otherChannels.count(ChanCount);
                    Combined1.replace(4,4,ChansNewCount);

                    p99.incrementmap.insert(Channel,ChansNewCount);

                    QByteArray holderGrpOut = "";

                    holderGrpOut.append(Combined1);

                    p99.ResendAllChannelsMap.insert(p99.ServersCheckChannelConfirm,holderGrpOut);
                    IN_Opcodes.OpcodeReturn.append(holderGrpOut);
                    IN_Opcodes.OpcodeReturn.append(p99.CharSelectID.toUtf8() + " Switch_Clients");

                    int index = p99.SortChanelsNumbersVector.indexOf(Channel.toUtf8());
                    p99.SortChanelsValuesVector.replace(index,ChansNewCount);

                    p99.ServersCheckChannelConfirm.clear();

                    for(int re = 0; re < p99.SortChanelsValuesVector.size(); re++)
                    {
                        QByteArray ChannelsValue = p99.SortChanelsValuesVector.at(re);
                        QByteArray Channel = p99.SortChanelsNumbersVector.at(re);

                        if(ChannelsValue != "0000")
                        {
                            p99.ServersCheckChannelConfirm.append(Channel + ChannelsValue);
                            p99.SortChanelsValuesVector.replace(re,"0000");
                        }
                    }

                    p99.channelmap.insert(Channel,"free");

                    packetparsing::IPandPort_AllvariablesMap.insert(p99.ClientsID + p99.ClientsIP + p99.ClientsPort,p99);

                    objectpacket Spawning;
                    QByteArray Status = Spawning.packetswitch(p99.ClientsID,p99.ClientsIP,p99.ClientsPort);


                    p99 = packetparsing::IPandPort_AllvariablesMap.value(p99.ClientsID + p99.ClientsIP + p99.ClientsPort);

                    qDebug() << "p99.Map_Channel_ID1" << p99.Map_Channel_ID;

                    int VectSize = p99.ActivateOP.size();

                    if(p99.ActivateOP.isEmpty() == false)// if i am in range and need a channel
                    {
                        for(int ld = 0; ld < p99.ActivateOP.size(); ld++)
                        {
                            QByteArray Fullc9 = p99.ActivateOP.at(ld);

                            p99.ResendAllChannelsMap.insert(p99.ServersCheckChannelConfirm,holderGrpOut);

                            IN_Opcodes.OpcodeReturn.append(Fullc9);
                            IN_Opcodes.OpcodeReturn.append(p99.CharSelectID.toUtf8() + "Switch_Clients");
                        }

                        for(int i = 0; i < VectSize; i ++)
                        {
                            p99.ActivateOP.removeAt(0);
                        }
                    }

                    packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);
                    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort);
                    packetparsing::IPandPort_AllvariablesMap.insert(p99.ClientsID + p99.ClientsIP + p99.ClientsPort,p99);
                }
            }
        }



        if(packetparsing::IPandPort_AllvariablesMap.contains(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort))
        {
            if(IN_Opcodes.GroupID != "00000000")
            {
                qDebug() << "Group Logout = true";
            }
            else
            {
                qDebug() << "Normal Logout1 = true";
            }
        }
    }


    packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);
    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort);

    /////////////////////Remove from other players channels/////////////////////////////


    DisbandGroup = false;
    Append_FB_OpcodeVector.clear();

    QString ThisClientsCharSelectID = IN_Opcodes.CharSelectID;

    // check group size

    if(CharCreate::GroupID_PlayersID_NamesVectorMap.contains(IN_Opcodes.GroupID))
    {
        QVector<QString> TempGrpVect = CharCreate::GroupID_PlayersID_NamesVectorMap.value(IN_Opcodes.GroupID);

        int index = TempGrpVect.indexOf(IN_Opcodes.CharSelectID);
        TempGrpVect.removeAt(index);
        TempGrpVect.removeAt(index);

        QVector<QString> TempGrpVect2 = CharCreate::PendingGroupID_PlayersID_NamesVectorMap.value(IN_Opcodes.GroupID);

        if(TempGrpVect2.contains(IN_Opcodes.CharSelectID))
        {
            int index = TempGrpVect2.indexOf(IN_Opcodes.CharSelectID);

            TempGrpVect2.removeAt(index);
            TempGrpVect2.removeAt(index);

            CharCreate::PendingGroupID_PlayersID_NamesVectorMap.insert(IN_Opcodes.GroupID,TempGrpVect2);
        }

        CharCreate::GroupID_PlayersID_NamesVectorMap.insert(IN_Opcodes.GroupID,TempGrpVect);

        if(TempGrpVect.size() >= 4)
        {

            QString NPCsName = checkthezone::NPCsNames.value(ThisClientsCharSelectID);
            QString TextFull5 = NPCsName + " has left the group.";

            Append_FB_OpcodeVector.append(TextFull5);//figure out why no name

            if(index == 0)
            {
                QString NewLeaderID = TempGrpVect.at(0);
                QString NPCsName = checkthezone::NPCsNames.value(NewLeaderID);

                QString TextFull5 = NPCsName + " is the new group leader.";

                Append_FB_OpcodeVector.append(TextFull5);
            }
        }
        else
        {
            DisbandGroup = true;
        }


        ////////////////////////////////Reform the Group////////////////////////////////////

        QVector<QString> TempVect = CharCreate::PendingGroupID_PlayersID_NamesVectorMap.value(IN_Opcodes.GroupID);

        QString GroupIDsString = "";
        QString GroupNamesString = "";
        QString FBsizeOpcode = "";
        QVector<QString> OpcodeVector;
        QVector<QString> GroupMemeberIDsVector;

        TempGrpVect = CharCreate::GroupID_PlayersID_NamesVectorMap.value(IN_Opcodes.GroupID);

        TempGrpVect2.clear();

        for(int grp = 0; grp < TempGrpVect.size(); grp+=2)
        {

            GroupIDsString.append(TempGrpVect.at(grp));
            GroupNamesString.append(TempGrpVect.at(grp + 1));

            TempGrpVect2.append(TempGrpVect.at(grp));
            TempGrpVect2.append(TempGrpVect.at(grp + 1));

            GroupMemeberIDsVector.append(TempGrpVect.at(grp));
        }

        if(TempVect.contains(IN_Opcodes.CharSelectID))
        {
            index = TempVect.indexOf(IN_Opcodes.CharSelectID);

            GroupIDsString.append(TempVect.at(index));
            GroupNamesString.append(TempVect.at(index + 1));

            TempGrpVect2.append(TempVect.at(index));
            TempGrpVect2.append(TempVect.at(index + 1));

            GroupMemeberIDsVector.append(TempVect.at(index));

            TempVect.removeAt(index);
            TempVect.removeAt(index);
        }

        int TempGrpVect2Size = GroupMemeberIDsVector.size();

        QString TempGrpVect2Size1 = QString("%1").arg(TempGrpVect2Size,2,16,QLatin1Char('0')).toUpper();



        FBsizeOpcode = "7207" +
                IN_Opcodes.GroupID + TempGrpVect2Size1 + "000000" +
                GroupIDsString +
                GroupNamesString;

        for(int i = 0; i < GroupMemeberIDsVector.size(); i ++)
        {
            OpcodeVector.append(FBsizeOpcode);
        }

        CharCreate::PendingGroupID_PlayersID_NamesVectorMap.insert(IN_Opcodes.GroupID,TempVect);
        CharCreate::GroupID_PlayersID_NamesVectorMap.insert(IN_Opcodes.GroupID,TempGrpVect2);


        QString DisGroupID = IN_Opcodes.GroupID;

        if(!GroupMemeberIDsVector.contains(IN_Opcodes.CharSelectID))
        {
            IN_Opcodes.GroupID = "00000000";
        }


        packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);
        IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort);//change to other client


        for(int grp = 0; grp < GroupMemeberIDsVector.size(); grp++)

        {
            QString SendToID = GroupMemeberIDsVector.at(grp).toUtf8();

            if(SendToID != IN_Opcodes.CharSelectID)
            {
                QByteArray holderGrpOut = "";
                QByteArray GrpRnk = "";

                QByteArray ClientID_out = "";
                QByteArray ClientIP_out = "";
                QByteArray ClientPort_out = "";

                packetparsing::packetvars pSwitch;

                QVector<QByteArray> TempVectOut = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(SendToID);

                if(TempVectOut.isEmpty() == true)
                {
                    SendToID = Opcodes::ObjectIDsMap.key(SendToID);
                    TempVectOut = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(SendToID);
                }

                if(TempVectOut.isEmpty() == false)
                {
                    ClientID_out = TempVectOut.at(0);
                    ClientIP_out = TempVectOut.at(1);
                    ClientPort_out = TempVectOut.at(2);

                    QMap<int,QByteArray> GroupRanks =
                    {
                        {0,"Group Leader "},{1,"Group Member 1 "},{2,"Group Member 2 "},{3,"Group Member 3 "}
                    };

                    GrpRnk = GroupRanks.value(grp);

                    pSwitch = packetparsing::IPandPort_AllvariablesMap.value(ClientID_out + ClientIP_out + ClientPort_out);//change to other client

                    FBsizeOpcode = OpcodeVector.at(grp);

                    QString DisbandOpcode = "";

                    if(DisbandGroup == true)
                    {
                        pSwitch.GroupID = "";

                        FBsizeOpcode = FBsizeOpcode.mid(0,28);
                        FBsizeOpcode.replace(12,2,"01");
                        DisbandOpcode = "71070377020000";

                        packetparsing::IPandPort_AllvariablesMap.insert(ClientID_out + ClientIP_out + ClientPort_out,pSwitch);

                    }


                    if(FBsizeOpcode.contains(pSwitch.CharSelectID))
                    {
                        int index1 = FBsizeOpcode.indexOf(pSwitch.CharSelectID);

                        FBsizeOpcode.replace(index1,8,IN_Opcodes.CharSelectID_Main);
                    }

                    ADD_OR_SUB CurrentItem;
                    QString NewFB = CurrentItem.FBSize(FBsizeOpcode);

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(pSwitch.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    increment Text;
                    QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                    int index = NewFB.indexOf("XXXX");
                    NewFB.replace(index,4,Text2);
                    Servers_Last_FB_MessageNumber = Text2.toUpper();

                    holderGrpOut.append(NewFB.toUtf8());

                    if(DisbandOpcode.isEmpty() == false)
                    {
                        QString NewFB = CurrentItem.FBSize(DisbandOpcode);

                        increment Text;
                        QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                        int index = NewFB.indexOf("XXXX");
                        NewFB.replace(index,4,Text2);
                        Servers_Last_FB_MessageNumber = Text2.toUpper();

                        holderGrpOut.append(NewFB.toUtf8());
                    }

                    QByteArray GroupUpdate = "";
                    QByteArray Servers_400D = MessNumbVect.at(2);

                    QByteArray IDsXZYs = "";
                    QString FBText5 = "";

                    // ////////////////Send Name Bars/////////////////////
                    if(DisbandGroup == false)
                    {
                        int BarSize = (GroupMemeberIDsVector.size() - 1) * 0x0D;
                        QByteArray BarSize1 = QString("%1").arg(BarSize,2,16,QLatin1Char('0')).toUtf8().toUpper();

                        GroupUpdate = "40" + BarSize1;

                        increment GrpOut;
                        QByteArray GrpOut_a = GrpOut.count(Servers_400D);
                        GroupUpdate.append(GrpOut_a + "00");

                        Servers_400D = GrpOut_a.toUpper();

                        MessNumbVect.replace(2,Servers_400D);

                        for(int grp = 0; grp < GroupMemeberIDsVector.size(); grp++)
                        {
                            QByteArray GroupIDs = GroupMemeberIDsVector.at(grp).toUtf8();

                            if(GroupIDs != pSwitch.CharSelectID.toUtf8())
                            {
                                IDsXZYs.append(GroupIDs + "01FF007A435E00733B");
                            }
                        }

                        if(IDsXZYs.isEmpty() == false)
                        {
                            xorpacket packet2;
                            QByteArray Recompressed2 = packet2.packetencrypt(IDsXZYs).toUpper();

                            GroupUpdate.append(Recompressed2);
                            holderGrpOut.append(GroupUpdate);
                        }
                    }
                    // ////////////////Send Name Bars/////////////////////

                    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                    CharID_ServerMessageNumbers.insert(pSwitch.CharSelectID,MessNumbVect);

                    if(Append_FB_OpcodeVector.isEmpty() == false)
                    {
                        for(int i = 0; i < Append_FB_OpcodeVector.size(); i++)
                        {
                            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(pSwitch.CharSelectID);
                            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                            QString Append_FB_Opcode = Append_FB_OpcodeVector.at(i);

                            QString NPCsName = checkthezone::NPCsNames.value(pSwitch.CharSelectID);

                            if(Append_FB_Opcode.contains(NPCsName))
                            {
                                Append_FB_Opcode.replace(0,NPCsName.size() + 4,"You are ");
                            }

                            ADD_OR_SUB SendToText5;
                            FBText5 =  SendToText5.TextSize(pSwitch.ClientsID,pSwitch.ClientsIP,pSwitch.ClientsPort,Append_FB_Opcode,false,"7A0A",Servers_Last_FB_MessageNumber);

                            Servers_Last_FB_MessageNumber = FBText5.mid(0,4).toUtf8();
                            FBText5.remove(0,4);

                            MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                            CharID_ServerMessageNumbers.insert(pSwitch.CharSelectID,MessNumbVect);

                            holderGrpOut.append(FBText5.toUtf8());
                        }
                    }
                }

                if(holderGrpOut.isEmpty() == false)
                {
                    if(pSwitch.CharSelectID != IN_Opcodes.CharSelectID)
                    {
                        pSwitch.ResendAllChannelsMap.insert(pSwitch.ServersCheckChannelConfirm,holderGrpOut);
                        IN_Opcodes.OpcodeReturn.append(holderGrpOut);
                        IN_Opcodes.OpcodeReturn.append(pSwitch.CharSelectID.toUtf8() + " Switch_Clients " + GrpRnk);


                        packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);
                        IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort);//change to other client
                    }
                }
            }
        }

        if(DisbandGroup == true)
        {
            if(CharCreate::PendingGroupID_PlayersID_NamesVectorMap.contains(DisGroupID))
            {
                CharCreate::PendingGroupID_PlayersID_NamesVectorMap.remove(DisGroupID);
            }

            if(CharCreate::GroupID_PlayersID_NamesVectorMap.contains(DisGroupID))
            {
                CharCreate::GroupID_PlayersID_NamesVectorMap.remove(DisGroupID);
            }
        }

    }


    ////////////////////////////////Reform the Group////////////////////////////////////


    /////////////////////////////Sending out the packets////////////////////////////////

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);


    if(IN_Opcodes.OpcodeReturn.isEmpty() == false)
    {
        do
        {
            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);


            QByteArray Data = IN_Opcodes.OpcodeReturn.takeAt(0);
            QByteArray Description = IN_Opcodes.OpcodeReturn.takeAt(0);

            QString SendToID = Description.mid(0,8);


            if(SendToID != IN_Opcodes.CharSelectID)
            {
                QVector<QByteArray> TempVect = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(SendToID);

                if(TempVect.isEmpty() == true)
                {
                    SendToID = Opcodes::ObjectIDsMap.key(SendToID);
                    TempVect = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(SendToID);
                }

                QByteArray ClientID_out = TempVect.at(0);
                QByteArray ClientIP_out = TempVect.at(1);
                QByteArray ClientPort_out = TempVect.at(2);

                packetparsing::packetvars pG;
                pG = packetparsing::IPandPort_AllvariablesMap.value(ClientID_out + ClientIP_out + ClientPort_out);

                QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(pG.CharSelectID);
                QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                QByteArray holderGrpOut = pG.serverid1 + pG.clientid1 + "92c001bf543213" + pG.SessionID + "00xxxx";

                holderGrpOut.append(Data);

                increment GrpOut;
                QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                holderGrpOut.replace(30,4,GrpOut_a);
                Servers_Last_MessageNumber = GrpOut_a.toUpper();

                packetsize ResizePacket;
                QByteArray ResizePacket2 = ResizePacket.switchsize(holderGrpOut);
                holderGrpOut.replace(8,4,ResizePacket2.toUpper());

                MessNumbVect.replace(0,Servers_Last_MessageNumber);

                Opcodes::CharID_ServerMessageNumbers.insert(pG.CharSelectID,MessNumbVect);

                packetparsing::IPandPort_AllvariablesMap.insert(ClientID_out + ClientIP_out + ClientPort_out,pG);

                QHostAddress current_client_address;
                current_client_address.setAddress(QString::fromUtf8(ClientIP_out));
                quint16 current_client_port = ClientPort_out.toUShort(nullptr,10);
                QString changeIP = current_client_address.toString();

                crc sendcrc;
                QByteArray outcrc =  sendcrc.elcrc(holderGrpOut);
                holderGrpOut.append(outcrc);

                QString MainToon = checkthezone::NPCsNames.value(pG.CharSelectID);


                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

                qDebug() << "";
                qDebug() << "";
                qDebug() << "";
                qDebug() << dateTimeString;

                qDebug() << "Player_Interactions" << Description;
                qDebug() << "To Client: " << MainToon;
                qDebug() << "IP: " << ClientsIP;
                qDebug() << "Port: " << ClientsPort;
                qDebug() << "From Server: ";
                qDebug() << holderGrpOut;
                qDebug() << "";

                QByteArray gameout2 = holderGrpOut.fromHex(holderGrpOut);

                QHostAddress newsender = QHostAddress(changeIP);
                worldthread::udpsocket3->writeDatagram(gameout2,newsender,current_client_port);

                MainWindow::UiOputVector.append("");
                MainWindow::UiOputVector.append("");
                MainWindow::UiOputVector.append("----------------------------------------------------------");
                MainWindow::UiOputVector.append(dateTimeString);
                MainWindow::UiOputVector.append("Player_Interactions: " + Description);
                MainWindow::UiOputVector.append("To Client: " + MainToon);
                MainWindow::UiOputVector.append("IP: " + ClientsIP);
                MainWindow::UiOputVector.append("Port: " + ClientsPort);
                MainWindow::UiOputVector.append("From Server: Packet:");
                MainWindow::UiOputVector.append(holderGrpOut);
                MainWindow::UiOputVector.append("----------------------------------------------------------");
                MainWindow::UiOputVector.append("");

                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

            }

        }while(IN_Opcodes.OpcodeReturn.size() > 0);

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
    }
}

QByteArray Opcodes::CheckTheOpcode(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort)
{
    packetparsing::packetvars IN_Opcodes;
    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QByteArray ID_IP_PORT = ClientID + ClientsIP + ClientsPort;

    qDebug() << "IN_Opcodes.CurrentQuestMenuID_1" << IN_Opcodes.CurrentQuestMenuID;

    if(IN_Opcodes.CharSelectID.isEmpty() == true)
    {
        IN_Opcodes.CharSelectID = ClientsIP;
        QVector<QByteArray> tempVect =
        {"0000","0000","0000","0000"};

        Opcodes::CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,tempVect);
    }

    QVector<QByteArray> tempVect = Opcodes::CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);

    if(tempVect.isEmpty() == true)
    {
        tempVect = {"0000","0000","0000","0000"};
        Opcodes::CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,tempVect);
    }


    if(db.isOpen())
    {
        db.close();
        db.removeDatabase(QSqlDatabase::defaultConnection);
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(Opcodes::m_FileName);
    db.open();

    qDebug() << "";
    qDebug() << "Inside Opcodes.cpp";
    qDebug() << "IN_Opcodes.CharSelectID = " << IN_Opcodes.CharSelectID;

    QMapIterator<QByteArray,QVector<QByteArray>> iterOp(IN_Opcodes.OpcodesAndGamebuf1Map);
    while(iterOp.hasNext())
    {
        iterOp.next();
        IN_Opcodes.opcoderesult1 = iterOp.key();
        QVector<QByteArray> OpcodeInfoVector = iterOp.value();

        QByteArray Clients_Last_MessageNumber = OpcodeInfoVector.at(0);
        QByteArray Clients_Last_FB_MessageNumber = OpcodeInfoVector.at(1);
        QByteArray opholder = OpcodeInfoVector.at(2);
        QByteArray gamebuf1 = OpcodeInfoVector.at(3);

        qDebug() << "Clients_Last_FB_MessageNumber = " << Clients_Last_FB_MessageNumber;
        qDebug() << "opholder = " << opholder;
        qDebug() << "IN_Opcodes.OpcodesAndGamebuf1Map.size() = " << IN_Opcodes.OpcodesAndGamebuf1Map.size();


        if(opholderVector.contains(opholder))
        {
            qDebug() << "Removing_a_dupe_opholder" << opholder;



            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray holderReply = "d37c5ae795c301bf543213" + IN_Opcodes.SessionID + "03xxxxXXXXxxxx";

            holderReply.replace(0,4,IN_Opcodes.serverid1); //swap IDs
            holderReply.replace(4,4,IN_Opcodes.clientid1); //swap IDs

            holderReply.replace(22,6,IN_Opcodes.SessionID);

            holderReply.replace(34,4,Clients_Last_MessageNumber);
            holderReply.replace(38,4,Clients_Last_FB_MessageNumber);

            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

            increment GrpOut;
            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
            holderReply.replace(30,4,GrpOut_a);
            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

            packetsize ResizePacket;
            QByteArray ResizePacket2 = ResizePacket.switchsize(holderReply);
            holderReply.replace(8,4,ResizePacket2);

            IN_Opcodes.OpcodeReturn.append(holderReply);
            IN_Opcodes.OpcodeReturn.append("Dupe_opholder ");
            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }
        else
        {
            //            if(opholderVector.size() > 50)
            //            {
            //                opholderVector.remove(0,45);
            //            }

            opholderVector.append(opholder);

            qDebug() << "opholderVector did not contain opholder" << opholder;

            qDebug() << "opholderVector.size()=" << opholderVector.size();

            qDebug() << "opholderVector" << opholderVector;
        }

        if(
                IN_Opcodes.opcoderesult1 == "6B00" || IN_Opcodes.opcoderesult1 == "6C00" ||
                IN_Opcodes.opcoderesult1 == "0100" || IN_Opcodes.opcoderesult1 == "0E00" ||
                IN_Opcodes.opcoderesult1 == "1400" || IN_Opcodes.opcoderesult1 == "C900" ||
                IN_Opcodes.opcoderesult1 == "CA00")
        {
            if(IN_Opcodes.opcoderesult1 == "0100" || IN_Opcodes.opcoderesult1 == "0E00" || IN_Opcodes.opcoderesult1 == "1400" ||
                    IN_Opcodes.opcoderesult1 == "C900")
            {
                IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

                if(IN_Opcodes.Flip163 != true)
                {
                    IN_Opcodes.Flip163 = true;

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);

                    MessNumbVect.replace(0,"0100");
                    MessNumbVect.replace(1,"0000");

                    CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);


                    IN_Opcodes.OpcodeReturn.append("IN_Opcodes.opcoderesult1");
                    IN_Opcodes.OpcodeReturn.append("Server_Reply_IN_Opcodes.opcoderesult1 = " + IN_Opcodes.opcoderesult1 + " opholder = " + opholder);
                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
                }
            }
            else
            {
                IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

                QByteArray holderReply = "d37c5ae795c301bf543213" + IN_Opcodes.SessionID + "03xxxxXXXXxxxx";

                holderReply.replace(0,4,IN_Opcodes.serverid1); //swap IDs
                holderReply.replace(4,4,IN_Opcodes.clientid1); //swap IDs

                holderReply.replace(22,6,IN_Opcodes.SessionID);

                holderReply.replace(34,4,Clients_Last_MessageNumber);
                holderReply.replace(38,4,Clients_Last_FB_MessageNumber);

                QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                increment GrpOut;
                QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                holderReply.replace(30,4,GrpOut_a);
                Servers_Last_MessageNumber = GrpOut_a.toUpper();

                MessNumbVect.replace(0,Servers_Last_MessageNumber);

                CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                packetsize ResizePacket;
                QByteArray ResizePacket2 = ResizePacket.switchsize(holderReply);
                holderReply.replace(8,4,ResizePacket2);

                IN_Opcodes.OpcodeReturn.append(holderReply);
                IN_Opcodes.OpcodeReturn.append("Server_Reply ");
                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
            }
        }






        if(IN_Opcodes.opcoderesult1 == "0000")
        {
            QByteArray optionflag1 = gamebuf1.mid(18,4);

            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);


            increment clientidC;

            QByteArray ClientsID_inc1 = clientidC.count(IN_Opcodes.clientid1);
            QByteArray ClientsID_inc2 = gamebuf1.mid(14,4);

            qDebug() << "";
            qDebug() << "ClientsID_inc3 = " << ClientsID_inc1;
            qDebug() << "ClientsID_inc4 = " << ClientsID_inc2;

            if(ClientsID_inc1 != ClientsID_inc2)
            {
                ClientsID_inc1 = IN_Opcodes.clientid1;
            }

            if(IN_Opcodes.serverid1.isEmpty() == true)
            {
                IN_Opcodes.serverid1 = gamebuf1.mid(4,4);
            }

            if(IN_Opcodes.clientid1.isEmpty() == true)
            {
                IN_Opcodes.clientid1 = gamebuf1.mid(0,4);
            }



            QByteArray holder163 = packet163;
            holder163.replace(0,4,IN_Opcodes.serverid1); //swap IDs
            holder163.replace(4,4,IN_Opcodes.clientid1); //swap IDs
            holder163.replace(12,4,ClientsID_inc1); //second id
            holder163.replace(22,4,ClientsID_inc1); //third id
            holder163.replace(16,4,optionflag1); //# after id
            holder163.replace(26,4,optionflag1); //# after id


            qDebug() << "IN_Opcodes.serverid1" << IN_Opcodes.serverid1;
            qDebug() << "IN_Opcodes.clientid1" << IN_Opcodes.clientid1;

            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);

            if(MessNumbVect.isEmpty() == true)
            {
                MessNumbVect.append("0000");
                MessNumbVect.append("0000");
                MessNumbVect.append("0000");
                MessNumbVect.append("0000");
            }

            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);



            if(IN_Opcodes.Flip163 != true)
            {
                IN_Opcodes.OLD_Servers_Last_MessageNumber = "0100";

                IN_Opcodes.SeverSelectionLastReplyConfirm = IN_Opcodes.OLD_Servers_Last_MessageNumber + "0100";

                qDebug() << "";
                qDebug() << "Server_163" << holder163;
                qDebug() << "IN_Opcodes.Clients_Last_FB_MessageNumber 163" << IN_Opcodes.Clients_Last_FB_MessageNumber;
            }
            else
            {
                increment GrpOut;
                QByteArray GrpOut_a = GrpOut.count(IN_Opcodes.OLD_Servers_Last_MessageNumber);
                holder163.replace(30,4,GrpOut_a);
                IN_Opcodes.OLD_Servers_Last_MessageNumber = GrpOut_a.toUpper();

                IN_Opcodes.SeverSelectionLastReplyConfirm = IN_Opcodes.OLD_Servers_Last_MessageNumber + "0100";

                QByteArray Clients_Last_MessageNumber = gamebuf1.mid(24,4);

                holder163.replace(34,4,Clients_Last_MessageNumber);

                qDebug() << "";
                qDebug() << "Server_163_Resend" << holder163;
                qDebug() << "IN_Opcodes.Clients_Last_FB_MessageNumber 163" << IN_Opcodes.Clients_Last_FB_MessageNumber;
            }


            IN_Opcodes.OpcodeReturn.append(holder163);
            IN_Opcodes.OpcodeReturn.append("Server_163 opholder = " + opholder + " IN_Opcodes.opcoderesult1 = " + IN_Opcodes.opcoderesult1 + " IN_Opcodes.Servers_Last_MessageNumber = " + Servers_Last_MessageNumber);



            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }




        if(IN_Opcodes.opcoderesult1 == "2A00")// this replies to the 2A00 and sends out 173
        {
            qDebug() << "IN_Opcodes.opcoderesult1 == 2A00";

            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray CharID5Byte = gamebuf1.mid(44,8);

            Convert Sendto;
            IN_Opcodes.CharID_5bytes = Sendto.Bytes4to5(CharID5Byte).toUtf8();

            QByteArray holder172 = "f8c4fa5787c001F1463213" + IN_Opcodes.SessionID + "23030003000100";

            Clients_Last_MessageNumber = gamebuf1.mid(28,4);

            holder172.replace(0,4,IN_Opcodes.serverid1); //swap IDs
            holder172.replace(4,4,IN_Opcodes.clientid1); //swap IDs
            holder172.replace(34,4,Clients_Last_MessageNumber);
            holder172.replace(38,4,Clients_Last_FB_MessageNumber);

            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);

            if(MessNumbVect.isEmpty() == true)
            {
                MessNumbVect.append("0000");
                MessNumbVect.append("0000");
                MessNumbVect.append("0000");
                MessNumbVect.append("0000");
            }

            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

            increment GrpOut;
            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
            holder172.replace(30,4,GrpOut_a);
            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

            packetsize ResizePacket;
            QByteArray ResizePacket2 = ResizePacket.switchsize(holder172);
            holder172.replace(8,4,ResizePacket2);

            IN_Opcodes.OpcodeReturn.append(holder172);
            IN_Opcodes.OpcodeReturn.append("Server_2A00_Reply ");

            IN_Opcodes.Server_2A00_reply = true;
            // //////
            QVector<QByteArray> tempVect = Opcodes::CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);

            Opcodes::CharID_ServerMessageNumbers.remove(IN_Opcodes.CharSelectID);

            IN_Opcodes.CharSelectID = gamebuf1.mid(44,8);

            qDebug() << "IN_Opcodes.CharSelectID_2A00_1 = " << IN_Opcodes.CharSelectID;

            QString Total_Hex1 = QString("%1").arg(SessionsValue,8,16,QLatin1Char('0')).toUpper();
            QString Byte1 = Total_Hex1.mid(0,2);
            QString Byte2 = Total_Hex1.mid(2,2);
            QString Byte3 = Total_Hex1.mid(4,2);
            QString Byte4 = Total_Hex1.mid(6,2);
            QString Total_Hex1_rev = Byte4 + Byte3 + Byte2 + Byte1;
            IN_Opcodes.CharSelectID_Main = Total_Hex1_rev;

            Opcodes::ObjectIDsMap.insert(IN_Opcodes.CharSelectID,IN_Opcodes.CharSelectID_Main);

            SessionsValue++;

            IN_Opcodes.SessionID = ConvertSession(Total_Hex1_rev);

            Opcodes::CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,tempVect);

            IN_Opcodes.coachCharID = IN_Opcodes.CharSelectID;
            IN_Opcodes.targetCharID = IN_Opcodes.CharSelectID;

            QString HairColor  = gamebuf1.mid(76,2);
            QString HairLength = gamebuf1.mid(68,2);
            QString HairStyle  = gamebuf1.mid(60,2);
            QString Face       = gamebuf1.mid(52,2);

            QString PacketNumber = ID_PacketNumber_Map.value(IN_Opcodes.CharSelectID);
            int PacketNumber2 = PacketNumber.toInt(nullptr,10);
            IN_Opcodes.packetid = PacketNumber2;

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            qDebug() << "IN_Opcodes.CharSelectID_2A00_2 = " << IN_Opcodes.CharSelectID;

            db.transaction();

            QSqlQuery query;

            Table_Name = "MAINTOON";

            Column_Name = "Hair Color (1)";
            query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
            query.bindValue(":val",HairColor);
            query.bindValue(":id",IN_Opcodes.packetid);
            query.exec();

            Column_Name = "Hair Length (1)";
            query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
            query.bindValue(":val",HairLength);
            query.bindValue(":id",IN_Opcodes.packetid);
            query.exec();

            Column_Name = "Hair Style (1)";
            query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
            query.bindValue(":val",HairStyle);
            query.bindValue(":id",IN_Opcodes.packetid);
            query.exec();

            Column_Name = "Face (1)";
            query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
            query.bindValue(":val",Face);
            query.bindValue(":id",IN_Opcodes.packetid);
            query.exec();

            QString objectidb1 = "ObjectID (4)";
            QString maintoonb1 = IN_Opcodes.CharSelectID;
            query.prepare(QStringLiteral("SELECT * FROM MAINTOON WHERE [%1] = '%2'").arg(objectidb1).arg(maintoonb1));
            query.exec();
            query.next();

            IN_Opcodes.StartUnSpentTPs = query.value(2).toString();

            QString HexVal = IN_Opcodes.StartUnSpentTPs.toUpper();
            ADD_OR_SUB SendtoDec;
            QString ConvertedValue = SendtoDec.BackToDec(HexVal);
            IN_Opcodes.UnSpentTPs = ConvertedValue.toInt(nullptr,10);


            if(IN_Opcodes.StartUnSpentTPs.size() == 2)
            {
                int TrainingPoints = IN_Opcodes.UnSpentTPs;

                if(TrainingPoints < 128)
                {
                    TrainingPoints = (TrainingPoints * 2) + 128;

                    QString TrainingPoints1 = QString("%1").arg(TrainingPoints,2,16,QLatin1Char('0'));

                    IN_Opcodes.StartUnSpentTPs = TrainingPoints1 + "00";
                }
                else
                {
                    IN_Opcodes.StartUnSpentTPs.append("00");
                }

            }

            IN_Opcodes.StartSpentTPs = query.value(3).toString();

            HexVal = IN_Opcodes.StartSpentTPs.toUpper();
            ConvertedValue = SendtoDec.BackToDec(HexVal);
            IN_Opcodes.SpentTPs = ConvertedValue.toInt(nullptr,10);


            IN_Opcodes.Cash_on_hand = query.value(4).toString();
            IN_Opcodes.Current_Available_balance = query.value(5).toString();

            IN_Opcodes.StartXPbar = query.value(6).toString();

            HexVal = IN_Opcodes.StartXPbar.toUpper();
            ConvertedValue = SendtoDec.BackToDec(HexVal);
            IN_Opcodes.XPBar = ConvertedValue.toInt(nullptr,10);

            IN_Opcodes.StartTotalXP = query.value(7).toString();

            HexVal = IN_Opcodes.StartTotalXP.toUpper();
            ConvertedValue = SendtoDec.BackToDec(HexVal);
            IN_Opcodes.TotalXp = ConvertedValue.toInt(nullptr,10);

            IN_Opcodes.StartCMxp = query.value(8).toString();
            IN_Opcodes.StartTotalCMxp = query.value(9).toString();
            IN_Opcodes.StartTotalCMxp = query.value(10).toString();
            IN_Opcodes.StartSpentCMs = query.value(11).toString();

            QSqlQuery queryFACTION_ID ("SELECT * FROM FACTION_ID");

            if(db.isOpen())
            {
                while(queryFACTION_ID.next())
                {
                    QString FACTION_ID = queryFACTION_ID.value(2).toString();
                    FactionIDVector.append(FACTION_ID);
                }
            }

            if(db.isOpen())
            {

                QString Object_ID = "ObjectID (4)";
                QSqlQuery queryFACTION;
                queryFACTION.prepare (QStringLiteral("SELECT * FROM FACTION WHERE [%1] = :val").arg(Object_ID));
                queryFACTION.bindValue(":val",IN_Opcodes.CharSelectID);
                queryFACTION.exec();

                while(queryFACTION.next())
                {
                    QSqlRecord record5 = queryFACTION.record();

                    for(int k=3; k < record5.count(); k++)
                    {
                        IN_Opcodes.FactionValueVector.append(record5.value(k).toInt());
                    }
                }
            }

            db.commit();

            QByteArray current_c9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8());

            current_c9.replace(290,2,HairColor.toUtf8());
            current_c9.replace(292,2,HairLength.toUtf8());
            current_c9.replace(294,2,HairStyle.toUtf8());
            current_c9.replace(296,2,Face.toUtf8());

            QString xout1 = current_c9.mid(12,6);
            QString zout1 = current_c9.mid(18,6);
            QString yout1 = current_c9.mid(24,6);

            IN_Opcodes.masterx = xout1.toUtf8().toUpper();
            IN_Opcodes.masterz = zout1.toUtf8().toUpper();
            IN_Opcodes.mastery = yout1.toUtf8().toUpper();

            QString facingout1 = current_c9.mid(30,2);
            QString worldout1 = current_c9.mid(34,2);
            IN_Opcodes.masterworld = worldout1.toInt(nullptr,10);
            IN_Opcodes.LastTeleportWorld = IN_Opcodes.masterworld;
            QString StartingRace = current_c9.mid(300,2);//race
            QString StartingLevel = current_c9.mid(358,2);//level
            QString StartingClass = current_c9.mid(368,2);//class

            generator::Players_Map.insert(IN_Opcodes.CharSelectID.toUtf8(),current_c9);

            qlonglong xinter = xout1.toLongLong(nullptr,16); //hex long to divide
            qlonglong yinter = yout1.toLongLong(nullptr,16); //hex long to divide
            qlonglong zinter = zout1.toLongLong(nullptr,16); //hex long to divide

            IN_Opcodes.CheckingTheZone = true;

            QVector<QByteArray> TempVect;

            TempVect.append(ClientID);
            TempVect.append(ClientsIP);
            TempVect.append(ClientsPort);

            CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.insert(IN_Opcodes.CharSelectID,TempVect);

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

            qDebug() << "IN_Opcodes_2A00.ClientID" << ClientID;
            qDebug() << "IN_Opcodes_2A00.ClientsIP" << ClientsIP;
            qDebug() << "IN_Opcodes_2A00.ClientsPort" << ClientsPort;

            qDebug() << "IN_Opcodes_2A00.masterx" << IN_Opcodes.masterx;
            qDebug() << "IN_Opcodes_2A00.mastery" << IN_Opcodes.mastery;

            checkthezone SendTo;
            SendTo.CheckTheZone(ClientID,ClientsIP,ClientsPort);

            qDebug() << "IN_Opcodes_2A00.Got_Back";

            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            float xfloat = xinter / 128.0; //divide by 128.0 to get float to cast for 4 byte
            float yfloat = yinter / 128.0; //divide by 128.0 to get float to cast for 4 byte
            float zfloat = zinter / 128.0; //divide by 128.0 to get float to cast for 4 byte

            if(zfloat > 130000.0) //neg z fix leave this in!
            {
                float smallsub2 = zfloat - 131072.00195193;
                zfloat = smallsub2;
            }

            QByteArray axarray (reinterpret_cast<const char*>(&xfloat), sizeof(xfloat));
            QByteArray ayarray (reinterpret_cast<const char*>(&yfloat), sizeof(yfloat));
            QByteArray azarray (reinterpret_cast<const char*>(&zfloat), sizeof(zfloat));
            QString axstr = axarray.toHex().toUpper();
            QString aystr = ayarray.toHex().toUpper();
            QString azstr = azarray.toHex().toUpper();

            QString Facing_4 = facing_map.value(facingout1);

            QString coorstr = axstr+azstr+aystr+Facing_4; //xzy 4 byte
            QByteArray chngcoor = coorstr.toUtf8();
            int worlddouble = worldout1.toInt(nullptr,16);

            if(worlddouble != 0) //double world byte
            {
                worlddouble = worlddouble * 2;
                QString doubleout = QString("%1").arg(worlddouble,2,16,QLatin1Char('0'));
                worldout1 = doubleout;
            }

            QByteArray chngwrld = worldout1.toUtf8();


            QByteArray holder173 = "";

            if(IN_Opcodes.CharID_5bytes.size() == 6)
            {
                QString Byte3 = IN_Opcodes.CharID_5bytes.mid(4,2);

                int intByte3 = Byte3.toInt(nullptr,16) + 0x80;

                QString Add80 = QString("%1").arg(intByte3,2,16,QLatin1Char('0'));

                IN_Opcodes.CharID_5bytes.replace(4,2,Add80.toUtf8());

                IN_Opcodes.CharID_5bytes.append("00");

            }

            holder173 = packet173;
            holder173.replace(0,4,IN_Opcodes.serverid1); //swap IDs
            holder173.replace(4,4,IN_Opcodes.clientid1); //swap IDs
            holder173.replace(22,6,IN_Opcodes.SessionID);
            holder173.replace(136,8,IN_Opcodes.CharID_5bytes);
            holder173.replace(192,4,IN_Opcodes.StartUnSpentTPs.toUtf8());
            holder173.replace(202,32,chngcoor); //xzy 4 byte hex and facing
            holder173.replace(200,2,chngwrld); //world byte



            int StartingClass1 = StartingClass.toInt(nullptr,16);
            int StartingClass2 = StartingClass1 * 2;
            QString StartingClass3 = QString("%1").arg(StartingClass2,2,16,QLatin1Char('0'));

            IN_Opcodes.Class = StartingClass;

            int StartingRace1 = StartingRace.toInt(nullptr,16);
            int StartingRace2 = StartingRace1 * 2;
            QString StartingRace3 = QString("%1").arg(StartingRace2,2,16,QLatin1Char('0'));

            int StartingLevel1 = StartingLevel.toInt(nullptr,16);

            IN_Opcodes.Level = StartingLevel1;

            int StartingLevel2 = StartingLevel1 * 2;
            QString StartingLevel3 = QString("%1").arg(StartingLevel2,2,16,QLatin1Char('0'));

            IN_Opcodes.CurrentLevel = StartingLevel3;

            IN_Opcodes.MySwingType = ClassSwingCountMap.value(StartingClass);

            if(IN_Opcodes.MySwingType > 1)
            {
                if((IN_Opcodes.MySwingType == 4 && StartingLevel1 >= 20 && StartingLevel1 < 35))//too low to quad
                {
                    IN_Opcodes.MySwingType = 2;//downgrade to 2 hits
                }


                if(StartingLevel1 < 20)// downgrade to 1 hit if less than level 20
                {
                    IN_Opcodes.MySwingType = 1;
                }
            }

            IN_Opcodes.MySwing = IN_Opcodes.MySwingType;
            IN_Opcodes.MySwingNumber = IN_Opcodes.MySwingType;

            holder173.replace(164,2,StartingClass3.toUtf8());
            holder173.replace(166,2,StartingRace3.toUtf8());
            holder173.replace(168,2,StartingLevel3.toUtf8());

            MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
            Servers_Last_MessageNumber = "0000";

            GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
            holder173.replace(30,4,GrpOut_a);
            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);




            IN_Opcodes.LoginStarted = "Started";

            IN_Opcodes.OpcodeReturn.append(holder173);
            IN_Opcodes.OpcodeReturn.append("Server_173 ");


            query.prepare(QStringLiteral("SELECT * FROM MAINTOON_STARTING_LOCATIONS WHERE [%1] = '%2'").arg(objectidb1).arg(maintoonb1));
            query.exec();
            query.next();

            IN_Opcodes.BindLocation = query.value(4).toString();
            IN_Opcodes.MainFlag = query.value(5).toInt();

            qDebug() << "IN_Opcodes.MainFlag1" << IN_Opcodes.MainFlag;

            qDebug() << "IN_Opcodes2A00.BindLocation" << IN_Opcodes.BindLocation;

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }


        if(IN_Opcodes.flipped == true)
        {
            //file.write("IN_Opcodes.flipped == true");
            //file.write("\n");

            IN_Opcodes.Old_session = "BF543213";
        }
        else
        {
            //file.write("IN_Opcodes.flipped == false");
            //file.write("\n");
        }

        //file.write("IN_Opcodes.opcoderesult1 = " + IN_Opcodes.opcoderesult1);
        //file.write("\n");
        //file.write("IN_Opcodes.Old_session = " + IN_Opcodes.Old_session);
        //file.write("\n");

        //file.close();

        if((IN_Opcodes.opcoderesult1 == "F707" || IN_Opcodes.opcoderesult1 == "F807") || (IN_Opcodes.opcoderesult1 == "12" && IN_Opcodes.Old_session == "BF543213")) //F707 Replies
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);


            //DAE40A1F8870BF543213" + IN_Opcodes.SessionID + "008600F901050012F433B6C9

            QByteArray holder2591 = packet259;
            holder2591.replace(4,4,IN_Opcodes.clientid1);
            holder2591.replace(0,4,IN_Opcodes.serverid1);

            holder2591.replace(22,6,IN_Opcodes.SessionID);

            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

            increment GrpOut;
            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
            holder2591.replace(30,4,GrpOut_a);
            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

            holder2591.replace(34,4,Clients_Last_MessageNumber);
            holder2591.replace(38,4,Clients_Last_FB_MessageNumber);

            IN_Opcodes.OpcodeReturn.append(holder2591);
            IN_Opcodes.OpcodeReturn.append("Server_F707_F807_F901_Replies ");
            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }



        if(IN_Opcodes.opcoderesult1 == "12" && IN_Opcodes.Old_session != "BF543213") // this lets you sit at char selection
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);


            QByteArray holder259 = packet001;
            holder259.replace(0,4,IN_Opcodes.serverid1); //swap IDs
            holder259.replace(4,4,IN_Opcodes.clientid1); //swap IDs

            holder259.replace(22,6,IN_Opcodes.SessionID);

            //A2920A1F8870F1463213" + IN_Opcodes.SessionID + "200300F9010100127CCDE3CA

            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

            increment GrpOut;
            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
            holder259.replace(30,4,GrpOut_a);
            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            Opcodes::CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

            holder259.replace(34,4,Clients_Last_MessageNumber);
            holder259.replace(38,4,Clients_Last_FB_MessageNumber);


            packetsize ResizePacket;
            QByteArray ResizePacket2 = ResizePacket.switchsize(holder259);
            holder259.replace(8,4,ResizePacket2);

            IN_Opcodes.OpcodeReturn.append(holder259);
            IN_Opcodes.OpcodeReturn.append("Server_F901_Replies ");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }





        if(IN_Opcodes.opcoderesult1 == "1E00" || IN_Opcodes.opcoderesult1 == "4200"
                || IN_Opcodes.opcoderesult1 == "4300" || IN_Opcodes.opcoderesult1 == "4400")
        {

            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);


            QString FBsizeOpcode = "";
            QByteArray SpellEffect = "";

            QVector<QByteArray> Opcodes =
            {
                "1E00","4200","4300","4400"
            };

            QVector<QByteArray> SpellEffects =
            {
                "A077CB35","578813E3","BD161B0E","A2A4F728"
            };

            int OpcodeIndex = Opcodes.indexOf(IN_Opcodes.opcoderesult1);

            if(OpcodeIndex != -1)
            {
                SpellEffect = SpellEffects.at(OpcodeIndex);

                FBsizeOpcode = "7607" + IN_Opcodes.CharSelectID  + IN_Opcodes.CharSelectID + SpellEffect;
            }


            QMapIterator<QByteArray,struct packetparsing::packetvars> StructIter (packetparsing::IPandPort_AllvariablesMap);
            while(StructIter.hasNext())
            {
                packetparsing::packetvars p99;

                StructIter.next();
                //QByteArray ID_IP_PORT = StructIter.key();
                p99 = StructIter.value();//all other players structs

                qDebug() << "p99.CharSelectID" << p99.CharSelectID;
                qDebug() << "IN_Opcodes.CharSelectID" << IN_Opcodes.CharSelectID;

                if(p99.CharSelectID.isEmpty() == false && p99.flipped == true && IN_Opcodes.CharSelectID != p99.CharSelectID)
                {
                    QString Channel = p99.Map_Channel_ID.key(IN_Opcodes.CharSelectID);

                    if(Channel.isEmpty() == false)
                    {
                        if(p99.Map_Channel_ID.contains(Channel))//check if i am in their spawned channels.
                        {
                            QString NewFB = "";

                            if(IN_Opcodes.CharSelectID != p99.CharSelectID)
                            {
                                QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(p99.CharSelectID);

                                ADD_OR_SUB CurrentItem;
                                NewFB = CurrentItem.FBSize(FBsizeOpcode);

                                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                                increment Text;
                                QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                                int index = NewFB.indexOf("XXXX");
                                NewFB.replace(index,4,Text2);
                                Servers_Last_FB_MessageNumber = Text2.toUpper();

                                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                                CharID_ServerMessageNumbers.insert(p99.CharSelectID,MessNumbVect);

                                IN_Opcodes.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,NewFB.toUtf8());
                            }

                            qDebug() << "Server_Action_OpcodeIndex" + QString::number(OpcodeIndex).toUtf8() + " " + SpellEffect;
                            qDebug() << "NewFB" << NewFB;



                            IN_Opcodes.OpcodeReturn.append(NewFB.toUtf8());
                            IN_Opcodes.OpcodeReturn.append(p99.CharSelectID.toUtf8() + " Switch_Clients");

                            packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);
                        }
                    }
                }

                if(IN_Opcodes.CharSelectID == p99.CharSelectID)
                {
                    QString NewFB = "";

                    qDebug() << "Server_Action_OpcodeIndex" + QString::number(OpcodeIndex).toUtf8() + " " + SpellEffect;
                    qDebug() << "NewFB" << NewFB;

                    IN_Opcodes.OpcodeReturn.append(NewFB.toUtf8());
                    IN_Opcodes.OpcodeReturn.append(p99.CharSelectID.toUtf8() + " Switch_Clients");

                    packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);
                }
            }
        }






        //text chat opcodes



        if(IN_Opcodes.opcoderesult1 == "0B00" || IN_Opcodes.opcoderesult1 == "010C" || IN_Opcodes.opcoderesult1 == "0E00" || IN_Opcodes.opcoderesult1 == "XXXX" || IN_Opcodes.opcoderesult1 == "C800")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray Opcode = "";
            QByteArray TextColor = "";
            QByteArray toPlayersName = "";
            QVector<QString> tempvect2;

            QString RollValue = "";
            QString NewFB = "";
            bool Tell = false;
            bool TalkingToYourself = false;

            bool Rolling = false;

            int Random_Counter = 0;

            QString textout = "";
            QByteArray textascii3 = "";

            //FB0306000E0000 say
            //FB0308000E0003 shout
            //fb3810000b00170000002f0074002000480075006c006b0073006800690074002000680065006c006c006f00200074006f0020006d00650091b91116

            if(IN_Opcodes.opcoderesult1 == "0E00" || IN_Opcodes.opcoderesult1 == "XXXX")
            {
                QMap<QByteArray,QByteArray> ChatModeMap =
                {
                    {"00","Say"},{"02","Group"},{"03","Shout"}
                };

                QByteArray newChatMode = opholder.mid(12,2).toUpper();

                IN_Opcodes.ChatMode = ChatModeMap.value(newChatMode);

                qDebug() << "newChatMode" << newChatMode;

                IN_Opcodes.OpcodeReturn.append(NewFB.toUtf8());
                IN_Opcodes.OpcodeReturn.append(IN_Opcodes.CharSelectID.toUtf8() + " Switch_Clients");
                packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);
            }

            qDebug() << "IN_Opcodes.ChatMode" << IN_Opcodes.ChatMode;



            //FB0F0C00010C02040000005900650073002E00
            if(IN_Opcodes.opcoderesult1 == "0B00" || IN_Opcodes.opcoderesult1 == "010C" || IN_Opcodes.opcoderesult1 == "C800")
            {
                int size2 = opholder.size() -8;

                QByteArray ID = opholder.mid(size2,8);

                if(ID == "FFFFFFFF" || ID == IN_Opcodes.CharSelectID_Main.toUpper()
                        || ID == IN_Opcodes.CharSelectID.toUpper() || ID == IN_Opcodes.Target_ID)
                {
                    QByteArray opholder2 = opholder.remove(size2,8);
                    opholder = opholder2;
                }

                qDebug() << "opholder1" << opholder;

                QString ChatHeader1 = "";
                QString ChatHeader2 = "";
                QByteArray current_C9 = "";
                QVector<QString> tempvect;

                current_C9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8().toUpper());

                QByteArray Name24Hex = current_C9.mid(310,48);

                textout = Name24Hex.fromHex(Name24Hex);

                int opcodeSize = 0;

                if(IN_Opcodes.opcoderesult1 == "0B00")
                {
                    opcodeSize = 20;

                    if(opholder.mid(opcodeSize,8) == "2F007400")
                    {
                        Tell = true;

                        int index = opholder.indexOf("2000");

                        qDebug() << "index1" << index;

                        opholder = opholder.remove(0,index + 4);

                        index = opholder.indexOf("2000");

                        qDebug() << "index2" << index;

                        toPlayersName = opholder.mid(0,index);

                        qDebug() << "toPlayersName1" << toPlayersName;

                        opholder.remove(0,toPlayersName.size() + 4);

                        qDebug() << "Tell_opholder" << opholder;
                    }
                }

                if(IN_Opcodes.opcoderesult1 == "010C")
                {
                    opcodeSize = 22;

                    if(opholder.mid(opcodeSize,8) == "2F007400")
                    {
                        Tell = true;

                        int index = opholder.indexOf("2000");

                        qDebug() << "index1" << index;

                        opholder = opholder.remove(0,index + 4);

                        index = opholder.indexOf("2000");

                        qDebug() << "index2" << index;

                        toPlayersName = opholder.mid(0,index);

                        qDebug() << "toPlayersName2" << toPlayersName;

                        opholder.remove(0,toPlayersName.size() + 4);

                        qDebug() << "Tell_opholder" << opholder;
                    }
                }


                if(IN_Opcodes.opcoderesult1 == "C800")
                {
                    std::uniform_int_distribution <int> distribution2(1, 100);
                    Random_Counter = distribution2(*QRandomGenerator::global());

                    RollValue = QString("%1").arg(Random_Counter,1,10,QLatin1Char('0')).toUpper();

                    Rolling = true;
                }



                qDebug() << "Tell" << Tell;

                if(Tell == false)
                {
                    if(Rolling == true && IN_Opcodes.ChatMode != "Group")
                    {
                        Opcode = "7B0A";
                        TextColor = "FFAA00";

                        ChatHeader1 = textout + " Rolled: " + RollValue;
                        tempvect.append(ChatHeader1);

                        opholder.clear();
                    }
                    else
                    {
                        if(IN_Opcodes.ChatMode == "Group")
                        {
                            qDebug() << "IN_Opcodes.GroupID" << IN_Opcodes.GroupID;

                            if(IN_Opcodes.GroupID != "00000000")
                            {
                                if(Rolling == true)
                                {
                                    Opcode = "7B0A";
                                    TextColor = "FFAA00";

                                    ChatHeader1 = textout + " rolled: " + RollValue;
                                    tempvect.append(ChatHeader1);
                                    opholder.clear();
                                }
                                else
                                {
                                    Opcode = "9E06" + IN_Opcodes.GroupID.toUtf8();
                                    ChatHeader1 = textout + " tells the group: ";
                                    ChatHeader2 = "You tell the group: ";
                                    tempvect.append(ChatHeader1);
                                    tempvect.append(ChatHeader2);
                                }
                            }
                        }

                        if(IN_Opcodes.ChatMode == "Shout")
                        {
                            Opcode = "7B0A";
                            TextColor = "3FFF3F";

                            ChatHeader1 = textout + " shouts: ";
                            tempvect.append(ChatHeader1);
                        }

                        if(IN_Opcodes.ChatMode == "Say")
                        {
                            Opcode = "7A0A";

                            ChatHeader1 = textout + ": ";
                            tempvect.append(ChatHeader1);
                        }

                        if(IN_Opcodes.ChatMode == "Guild")
                        {
                            Opcode = "7B0A";
                            TextColor = "3FFF3F";

                            ChatHeader1 = textout + " tells the guild: ";

                            tempvect.append(ChatHeader1);
                        }
                    }
                }
                else
                {


                    qDebug() << "toPlayersName3" << toPlayersName;

                    QByteArray test1 = toPlayersName;
                    QByteArray textascii = test1;

                    for (int cc = 0; cc < textascii.size() * 2; cc+=2)

                    {
                        textascii.remove(cc + 2,2);
                    }

                    qDebug() << "Text2" << textascii;

                    textascii3 = textascii.fromHex(textascii);

                    QString CharID = checkthezone::NPCsNames.key(textascii3);

                    if(CharID.isEmpty() == true)
                    {
                        Opcode = "7B0A";
                        TextColor = "FF0000";
                        ChatHeader1 = "That player name is invalid. Message not sent.";
                        tempvect.append(ChatHeader1);
                    }
                    else
                    {
                        QVector<QByteArray> TempVect3 = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(CharID);


                        if(TempVect3.isEmpty() == true)
                        {
                            Opcode = "7B0A";
                            TextColor = "FF0000";
                            ChatHeader1 = "That player is not online. Message not sent.";
                            tempvect.append(ChatHeader1);
                        }
                        else
                        {
                            if(textout == textascii3)
                            {
                                TalkingToYourself = true;
                                Opcode = "7B0A";
                                TextColor = "FFFF7F";
                                ChatHeader1 = "Talking to youself is a sign of insanity... ";
                                tempvect.append(ChatHeader1);
                            }
                            else
                            {
                                Opcode = "7B0A";
                                TextColor = "8033C7";
                                ChatHeader1 = "You tell " + textout + ": ";
                                ChatHeader2 = textout + " tells you: ";
                                tempvect.append(ChatHeader1);
                                tempvect.append(ChatHeader2);
                            }
                        }
                    }
                }

                qDebug() << "Opcode" << Opcode;
                qDebug() << "TextColor" << TextColor;

                for(int i = 0; i < tempvect.size(); i ++)
                {
                    QByteArray change1 = tempvect.at(i).toUtf8();
                    QByteArray textascii2 = change1.toHex();
                    QByteArray textascii = textascii2;

                    for (int cc = 0; cc < textascii2.size() * 2; cc+=2)
                    {
                        textascii.insert(cc + 2,"00");
                        cc += 2;
                    }

                    QByteArray NameHex = textascii;

                    qDebug() << "ChatHeader" << ChatHeader1;

                    qDebug() << "NameHex" << NameHex;

                    QString opholder2 = opholder.mid(opcodeSize,-1);

                    QString Text1 = NameHex + opholder2;

                    qDebug() << "Text1" << Text1;


                    int size = Text1.size() / 4;
                    QString text_size1 = QString("%1").arg(size,2,16,QLatin1Char('0')).toUpper();

                    text_size1.append("000000");

                    QString FBsizeOpcode = Opcode + text_size1 + Text1 + TextColor;

                    ADD_OR_SUB CurrentItem;
                    NewFB = CurrentItem.FBSize(FBsizeOpcode).toUpper();

                    tempvect2.append(NewFB);
                }
            }

            if(Tell == true)
            {
                qDebug() << "Text3" << textascii3;

                QString CharID = checkthezone::NPCsNames.key(textascii3);

                qDebug() << "CharID" << CharID;


                QVector<QByteArray> CharIDVector;

                CharIDVector.append(IN_Opcodes.CharSelectID.toUtf8().toUpper());

                if(TalkingToYourself == false && CharID.isEmpty() == false && CharID_ServerMessageNumbers.contains(CharID))
                {
                    IN_Opcodes.LastTell = textascii3;
                    CharIDVector.append(CharID.toUtf8().toUpper());
                }

                for(int ch = 0; ch < CharIDVector.size(); ch++)
                {
                    CharID = CharIDVector.at(ch);

                    NewFB = tempvect2.takeAt(0);

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(CharID);

                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    increment Text;
                    QByteArray Text3 = Text.count(Servers_Last_FB_MessageNumber);
                    int index = NewFB.indexOf("XXXX");
                    NewFB.replace(index,4,Text3);
                    Servers_Last_FB_MessageNumber = Text3.toUpper();

                    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                    CharID_ServerMessageNumbers.insert(CharID,MessNumbVect);

                    qDebug() << "Server_Chat_Tell";
                    qDebug() << "NewFB" << NewFB;

                    QVector<QByteArray> tempVect = Opcodes::CharID_CLientID_IP_PORT_Map.value(CharID);
                    QByteArray ClientsID = tempVect.at(0);
                    QByteArray ClientsIP = tempVect.at(1);
                    QByteArray ClientsPort = tempVect.at(2);

                    packetparsing::packetvars PSWAP = packetparsing::IPandPort_AllvariablesMap.value(ClientsID + ClientsIP + ClientsPort);


                    PSWAP.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,NewFB.toUtf8());
                    packetparsing::IPandPort_AllvariablesMap.insert(ClientsID + ClientsIP + ClientsPort,PSWAP);

                    IN_Opcodes.OpcodeReturn.append(NewFB.toUtf8());
                    IN_Opcodes.OpcodeReturn.append(CharID.toUtf8() + " Switch_Clients");

                    packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);
                }
            }
            else
            {
                packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);

                if(tempvect2.isEmpty() == false)
                {
                    QMapIterator<QByteArray,struct packetparsing::packetvars> StructIter (packetparsing::IPandPort_AllvariablesMap);
                    while(StructIter.hasNext())
                    {
                        packetparsing::packetvars p99;

                        StructIter.next();
                        //QByteArray ID_IP_PORT = StructIter.key();
                        p99 = StructIter.value();//all other players structs

                        bool SendThrough = false;

                        ADD_OR_SUB sendto;
                        float DistanceCheck = sendto.DistanceCheck(IN_Opcodes.masterx,IN_Opcodes.masterz,IN_Opcodes.mastery,p99.masterx,p99.masterz,p99.mastery);

                        qDebug() << "DistanceCheck" << DistanceCheck;

                        if(
                                (IN_Opcodes.ChatMode == "Say" && DistanceCheck <= 300) ||
                                (IN_Opcodes.ChatMode == "Shout") || (IN_Opcodes.ChatMode == "Group") ||
                                (IN_Opcodes.ChatMode == "Guild")
                                )
                        {
                            SendThrough = true;
                        }

                        qDebug() << "p99.GroupID" << p99.GroupID;
                        qDebug() << "IN_Opcodes.GroupID" << IN_Opcodes.GroupID;

                        if(IN_Opcodes.ChatMode == "Group" && IN_Opcodes.GroupID != p99.GroupID)
                        {
                            SendThrough = false;
                        }

                        if(IN_Opcodes.ChatMode == "Guild" && IN_Opcodes.GuildID != p99.GuildID)
                        {
                            SendThrough = false;
                        }

                        qDebug() << "SendThrough" << SendThrough;

                        if(SendThrough == true)
                        {

                            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(p99.CharSelectID);
                            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);
                            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);


                            if(p99.CharSelectID == IN_Opcodes.CharSelectID && tempvect2.size() > 1)
                            {
                                NewFB = tempvect2.at(1);
                            }
                            else
                            {
                                NewFB = tempvect2.at(0);
                            }

                            increment Text;
                            QByteArray Text3 = Text.count(Servers_Last_FB_MessageNumber);
                            int index = NewFB.indexOf("XXXX");
                            NewFB.replace(index,4,Text3);
                            Servers_Last_FB_MessageNumber = Text3.toUpper();

                            MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                            CharID_ServerMessageNumbers.insert(p99.CharSelectID,MessNumbVect);


                            p99.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,NewFB.toUtf8());

                            QByteArray Send_FB = p99.serverid1 + p99.clientid1 + "xxxx01BF543213" + p99.SessionID + "03" +
                                    Servers_Last_MessageNumber + p99.Clients_Last_MessageNumber + p99.Clients_Last_FB_MessageNumber + NewFB.toUtf8();

                            packetsize ResizePacket; //call size class
                            QByteArray ResizePacket2 = ResizePacket.switchsize(Send_FB); //get new size
                            Send_FB.replace(8,4,ResizePacket2);

                            crc sendcrc;
                            QByteArray outcrc =  sendcrc.elcrc(Send_FB);
                            Send_FB.append(outcrc);

                            QByteArray Send_FB_out = Send_FB.fromHex(Send_FB);

                            QHostAddress current_client_address;
                            current_client_address.setAddress(QString::fromUtf8(p99.ClientsIP));
                            quint16 current_client_port = p99.ClientsPort.toUShort(nullptr,10);
                            QString changeIP = current_client_address.toString();

                            QHostAddress newsender = QHostAddress(changeIP);
                            worldthread::udpsocket3->writeDatagram(Send_FB_out,newsender,current_client_port);

                            QString MainToon = checkthezone::NPCsNames.value(p99.CharSelectID);

                            QDateTime dateTime = dateTime.currentDateTime();
                            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

                            MainWindow::UiOputVector.append("");
                            MainWindow::UiOputVector.append("");
                            MainWindow::UiOputVector.append("----------------------------------------------------------");
                            MainWindow::UiOputVector.append(dateTimeString);
                            MainWindow::UiOputVector.append("Sending_Text");
                            MainWindow::UiOputVector.append("To Client: " + MainToon);
                            MainWindow::UiOputVector.append("IP: " + ClientsIP);
                            MainWindow::UiOputVector.append("Port: " + ClientsPort);
                            MainWindow::UiOputVector.append("From Server: Packet:");
                            MainWindow::UiOputVector.append(Send_FB);
                            MainWindow::UiOputVector.append("----------------------------------------------------------");
                            MainWindow::UiOputVector.append("");

                            qDebug() << "";
                            qDebug() << "";
                            qDebug() << "----------------------------------------------------------";
                            qDebug() << dateTimeString;
                            qDebug() << "Sending_Text";
                            qDebug() << "To Client: " + MainToon;
                            qDebug() << "p99.CharSelectID = " << p99.CharSelectID;
                            qDebug() << "IP: " + p99.ClientsIP;
                            qDebug() << "Port: " + p99.ClientsPort;
                            qDebug() << "From Server: ";
                            qDebug() << Send_FB;
                            qDebug() << "----------------------------------------------------------";
                            qDebug() << "";


                            packetparsing::IPandPort_AllvariablesMap.insert(p99.ClientsID + p99.ClientsIP + p99.ClientsPort,p99);
                        }
                    }
                }
            }
        }




        if(IN_Opcodes.opcoderesult1 == "010E")
        {
            //fb040500 010e 0102

            QByteArray PageCount = opholder.mid(12,4);

            QString FBsizeOpcode = "";

            qDebug() << "";

            qDebug() << "packetparsing::IPandPort_AllvariablesMap.size()A" << packetparsing::IPandPort_AllvariablesMap.size();

            QMapIterator<QByteArray,struct packetparsing::packetvars> iter (packetparsing::IPandPort_AllvariablesMap);
            while (iter.hasNext())
            {
                iter.next();
                QByteArray chkey1 = iter.key();
                packetparsing::packetvars p6;
                p6 = iter.value();


                if(chkey1 == p6.ClientsID + p6.ClientsIP + p6.ClientsPort && p6.flipped == true)
                {
                    QByteArray current_C9 = generator::Players_Map.value(p6.CharSelectID.toUtf8()).toUpper();

                    qDebug() << "current_C9.size()" << current_C9.size();

                    QByteArray Name24Hex = current_C9.mid(310,48);

                    qDebug() << "Name24Hex" << Name24Hex;

                    QString textout = Name24Hex.fromHex(Name24Hex);

                    qDebug() << "textout" << textout;

                    p6.Name = textout.toUtf8().toHex();

                    qDebug() << "p6.Name" << p6.Name;

                    int NameSize = p6.Name.size();

                    qDebug() << "NameSize" << NameSize;

                    QString sizeout2 = QString("%1").arg(NameSize /2,2,16,QLatin1Char('0')).toUpper();

                    qDebug() << "sizeout2" << sizeout2;

                    QString Race_OldVal1 = current_C9.mid(300,2);//race
                    QString Level_OldVal1 = current_C9.mid(358,2);//level
                    QString Class_OldVal1 = current_C9.mid(368,2);//class

                    p6.Race = Race_OldVal1;
                    p6.Class = Class_OldVal1;
                    p6.Level = Level_OldVal1.toUInt(nullptr,16);

                    FBsizeOpcode.append(Race_OldVal1 + Class_OldVal1 + Level_OldVal1 + sizeout2 + "000000" + p6.Name + p6.CharSelectID_Main);

                    qDebug() << "FBsizeOpcode" << FBsizeOpcode;
                }
                else
                {
                    packetparsing::IPandPort_AllvariablesMap.remove(chkey1);

                    qDebug() << "packetparsing::IPandPort_AllvariablesMap.size()B" << packetparsing::IPandPort_AllvariablesMap.size();

                }
            }

            int MapSize = packetparsing::IPandPort_AllvariablesMap.size();

            qDebug() << "packetparsing::IPandPort_AllvariablesMap.size()C" << packetparsing::IPandPort_AllvariablesMap.size();


            QString sizeout3 = QString("%1").arg(MapSize *2,1,16,QLatin1Char('0')).toUpper();

            if(sizeout3.size() == 1 || sizeout3.size() == 3)
            {
                sizeout3.insert(0,"0");
            }

            FBsizeOpcode.insert(0,"020E" + PageCount + sizeout3);

            QByteArray Packet = IN_Opcodes.serverid1 + IN_Opcodes.clientid1 + "f5c101bf543213" + IN_Opcodes.SessionID + "03xxxx";

            //020E0102020503010900000048756C6B62616C6C73

            Packet.append(Clients_Last_MessageNumber);
            Packet.append(Clients_Last_FB_MessageNumber);



            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

            qDebug() << "Servers_Last_MessageNumber at whos list 1" << Servers_Last_MessageNumber;

            increment GrpOut;
            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
            Packet.replace(30,4,GrpOut_a);
            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            ADD_OR_SUB CurrentItem;
            QString NewFB = CurrentItem.FBSize(FBsizeOpcode);

            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

            qDebug() << "Servers_Last_MessageNumber at whos list 2" << Servers_Last_MessageNumber;


            increment Text;
            QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
            int index = NewFB.indexOf("XXXX");
            NewFB.replace(index,4,Text2);
            Servers_Last_FB_MessageNumber = Text2.toUpper();

            MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

            CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

            Packet.append(NewFB.toUtf8());

            //fb251900 020e 0104 04030001
            //050000004661786f6e78563412
            //010e01
            //050000004368656d7378563412


            packetsize getnewsize1; //call size class
            QByteArray coachnewsize = getnewsize1.switchsize(Packet); //get new size
            Packet.replace(8,4,coachnewsize);

            IN_Opcodes.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,Packet);

            IN_Opcodes.OpcodeReturn.append(Packet);
            IN_Opcodes.OpcodeReturn.append("Server_Generate_Whos_List ");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }


        //Generate name
        if(IN_Opcodes.opcoderesult1 == "1200")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);
            //fbxx xxxx 1200 0C00

            QString Name = "";
            int index = 0;
            QString Status = "";

            QString RaceGen = opholder.mid(12,4).toUpper();
            QVector<QString> RaceGenVect = Opcodes::RaceGen_NameVectorMap.value(RaceGen);

            do
            {
                if(IN_Opcodes.NamelistCountMap.contains(RaceGen))
                {
                    index =  IN_Opcodes.NamelistCountMap.value(RaceGen);
                    index++;

                    if(index >= RaceGenVect.size())
                    {
                        index = 0;
                    }

                    Name = RaceGenVect.at(index);
                }
                else
                {
                    Name = RaceGenVect.at(index);
                }

                QVector<QString> StatusVect = Opcodes::Name_PacketStatusVectorMap.value(Name);

                Status = StatusVect.at(1);

                qDebug() << "Status" << Status;

                IN_Opcodes.NamelistCountMap.insert(RaceGen,index);

            }
            while(Status == "YES");



            ADD_OR_SUB SendToText;
            QString TextFull = Name;
            QString FBText =  SendToText.TextSizeNo_00s(ClientID,ClientsIP,ClientsPort,TextFull,"1200");



            IN_Opcodes.SendFBsVector.append(FBText.toUtf8());
            IN_Opcodes.FBReplyOrNotVector.append("23");
            IN_Opcodes.FBPacketDescriptionVector.append("Server_Generate_Name, ");

            qDebug() << "";
            qDebug() << "Server_Generate_Name";
            qDebug() << "FBText = " << FBText;

            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);
            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

            flipped sendTo;
            sendTo.SendOut(IN_Opcodes.ClientsID,IN_Opcodes.ClientsIP,IN_Opcodes.ClientsPort);
        }


        qDebug() << "IN_Opcodes.opcoderesult1 = " << IN_Opcodes.opcoderesult1;

        if(IN_Opcodes.opcoderesult1 == "2D00" || IN_Opcodes.opcoderesult1 == "2B00")
        {
            qDebug() << "IN_Opcodes.OpcodesAndGamebuf1Map.size()1 = " << IN_Opcodes.OpcodesAndGamebuf1Map.size();

            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            int index = gamebuf1.indexOf(opholder);
            //FB0604002D00F6C39501
            //359B5ae787c001F1463213" + IN_Opcodes.SessionID + "2320001f000100

            QByteArray holder259 = packet001;
            holder259.replace(0,4,IN_Opcodes.serverid1); //swap IDs
            holder259.replace(4,4,IN_Opcodes.clientid1); //swap IDs

            holder259.replace(22,6,IN_Opcodes.SessionID);


            if(IN_Opcodes.opcoderesult1 == "2D00")
            {

                QString DeleteSelectID = opholder.mid(12,8);

                CharCreate SendTo;
                QByteArray PacketStatus = SendTo.CharDelete(ClientID,ClientsIP,ClientsPort,DeleteSelectID);

                qDebug() << "Delete Character" << DeleteSelectID;
                qDebug() << "PacketStatus" << PacketStatus;

                if(PacketStatus == "Create Packet")
                {
                    QByteArray FBsizeOpcode = SendTo.CharSelectScreen(ClientID,ClientsIP,ClientsPort);



                    ADD_OR_SUB CurrentItem;
                    QString NewFB = CurrentItem.FBSize(FBsizeOpcode);

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);


                    increment Text;
                    QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                    int index = NewFB.indexOf("XXXX");
                    NewFB.replace(index,4,Text2);
                    Servers_Last_FB_MessageNumber = Text2.toUpper();

                    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                    CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                    holder259.append(NewFB.toUtf8().toUpper());

                    qDebug() << "2D00" << DeleteSelectID;
                    qDebug() << "holder259" << holder259;
                }
            }



            if(IN_Opcodes.opcoderesult1 == "2B00")//Create Character
            {
                int packetid1 = 0;
                QString packetid2 = "";
                QString OldID = IN_Opcodes.CharSelectID;
                QString NewFB = "";

                qDebug() << "IN_Opcodes.opcoderesult3 = " << IN_Opcodes.opcoderesult1;

                qDebug() << "Create Character" << holder259;
                qDebug() << "2B00" << OldID;
                qDebug() << "holder259" << holder259;


                QString NameLength = opholder.mid(12,2);

                int NameLength1 = NameLength.toInt(nullptr,16);
                int NameLength2 = NameLength1 * 2;
                int IndexLevel = 20 + NameLength2;

                IN_Opcodes.Name = opholder.mid(20,NameLength2);

                QByteArray NameHex = IN_Opcodes.Name.toUtf8();
                QString textout = NameHex.fromHex(NameHex);

                if(!Opcodes::PlayerNames.contains(textout))
                {

                    QVector<QByteArray> tempvect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);





                    qDebug() << "Opcodes::MaintoonTableSize = " << Opcodes::MaintoonTableSize;

                    Opcodes::MaintoonTableSize++;

                    IN_Opcodes.packetid = Opcodes::MaintoonTableSize;


                    packetid2 = QString("%1").arg(Opcodes::MaintoonTableSize,1,10,QLatin1Char('0')).toUpper();



                    if(IN_Opcodes.packetid == 0)
                    {
                        IN_Opcodes.CharSelectID = "00012000";
                    }


                    IN_Opcodes.CharSelectID = Opcodes::MaintoonsCharID;

                    qDebug() << "IN_Opcodes.CharSelectID_2_z = " << IN_Opcodes.CharSelectID;

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);



                    IN_Opcodes.sLevel = opholder.mid(IndexLevel,2);
                    IN_Opcodes.Race = opholder.mid(IndexLevel + 2,2);
                    IN_Opcodes.Class = opholder.mid(IndexLevel + 4,2);
                    IN_Opcodes.Gender = opholder.mid(IndexLevel + 6,2);
                    IN_Opcodes.HairColor = opholder.mid(IndexLevel + 8,2);
                    IN_Opcodes.HairLength = opholder.mid(IndexLevel + 10,2);
                    IN_Opcodes.HairStyle = opholder.mid(IndexLevel + 12,2);
                    IN_Opcodes.Face = opholder.mid(IndexLevel + 14,2);
                    IN_Opcodes.FPorNot = opholder.mid(IndexLevel + 16,2);

                    //////////////////////////////////////////////////////////

                    QString STR = opholder.mid(IndexLevel + 18,2);
                    IN_Opcodes.STR1 = STR.toInt(nullptr,16);
                    QString STA = opholder.mid(IndexLevel + 26,2);
                    IN_Opcodes.STA1 = STA.toInt(nullptr,16);
                    QString AGI = opholder.mid(IndexLevel + 34,2);
                    IN_Opcodes.AGI1 = AGI.toInt(nullptr,16);
                    QString DEX = opholder.mid(IndexLevel + 42,2);
                    IN_Opcodes.DEX1 = DEX.toInt(nullptr,16);
                    QString WIS = opholder.mid(IndexLevel + 50,2);
                    IN_Opcodes.WIS1 = WIS.toInt(nullptr,16);
                    QString INT = opholder.mid(IndexLevel + 58,2);
                    IN_Opcodes.INT1 = INT.toInt(nullptr,16);
                    QString CHA = opholder.mid(IndexLevel + 66,2);
                    IN_Opcodes.CHA1 = CHA.toInt(nullptr,16);

                    IN_Opcodes.TotalTPSpent = IN_Opcodes.STR1 + IN_Opcodes.STA1 + IN_Opcodes.AGI1 + IN_Opcodes.DEX1 + IN_Opcodes.WIS1 + IN_Opcodes.INT1 + IN_Opcodes.CHA1;
                    IN_Opcodes.SpentTPs = IN_Opcodes.TotalTPSpent;

                    IN_Opcodes.RemainingTPs = 20 - IN_Opcodes.TotalTPSpent;

                    IN_Opcodes.UnSpentTPs = IN_Opcodes.RemainingTPs;

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

                    //////////////////////////////////////////////////////////

                    CharCreate SendTo;

                    qDebug() << "IN_Opcodes.CharSelectID_5 = " << IN_Opcodes.CharSelectID;

                    QByteArray NewC9 = SendTo.CharacterCreation(ClientID,ClientsIP,ClientsPort);

                    IN_Opcodes.CharSelectID = NewC9.mid(2,8);

                    qDebug() << "IN_Opcodes.CharSelectID_2x = " << IN_Opcodes.CharSelectID;

                    qDebug() << "IN_Opcodes.packetid_3 = " << IN_Opcodes.packetid;
                    qDebug() << "NewC9.size()" << NewC9.size();
                    qDebug() << "NewC9" << NewC9;

                    CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,tempvect);

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

                    generator::Players_Map.insert(IN_Opcodes.CharSelectID.toUtf8(),NewC9);

                    QByteArray CharPacketBack = SendTo.PullCharacter(IN_Opcodes.CharSelectID);

                    QVector<QByteArray> tempCharVect = Opcodes::UserName_CharCountVectorMap.value(IN_Opcodes.UserName);


                    QVector<QByteArray> PacketOut_CharVect = tempCharVect;

                    if(CharPacketBack.isEmpty() == false)
                    {
                        int index = PacketOut_CharVect.indexOf(CharPacketBack);

                        if(index == -1)
                        {
                            PacketOut_CharVect.append(CharPacketBack);
                        }

                        if(PacketOut_CharVect.size() <= tempCharVect.size())
                        {
                            Opcodes::UserName_CharCountVectorMap.insert(IN_Opcodes.UserName,tempCharVect);
                        }
                        else
                        {
                            Opcodes::UserName_CharCountVectorMap.insert(IN_Opcodes.UserName,PacketOut_CharVect);
                        }
                    }

                    QByteArray ConfirmID = generator::Players_Map.key(NewC9);
                    qDebug() << "ConfirmID" << ConfirmID;

                    QString packetid3 = QString("%1").arg(packetid1,1,10,QLatin1Char('0')).toUpper(); //pad zeros
                    ID_PacketNumber_Map.insert(IN_Opcodes.CharSelectID,packetid3);



                    //////////////////////////////////////////////////////////
                    QByteArray FBsizeOpcode = SendTo.CharSelectScreen(ClientID,ClientsIP,ClientsPort);


                    ADD_OR_SUB CurrentItem;
                    NewFB = CurrentItem.FBSize(FBsizeOpcode);

                    IN_Opcodes.LastCharSelect = NewFB;
                }
                else
                {
                    NewFB = IN_Opcodes.LastCharSelect;

                    qDebug() << "Repeate Name = " << IN_Opcodes.Name;
                }

                QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                qDebug() << "Servers_Last_FB_MessageNumber = " << Servers_Last_FB_MessageNumber;

                increment Text;
                QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                int index = NewFB.indexOf("XXXX");
                NewFB.replace(index,4,Text2);
                Servers_Last_FB_MessageNumber = Text2.toUpper();

                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                holder259.append(NewFB.toUtf8().toUpper());
                //////////////////////////////////////////////////////////
            }

            packetsize ResizePacket;
            QByteArray ResizePacket2 = ResizePacket.switchsize(holder259);
            holder259.replace(8,4,ResizePacket2);

            Clients_Last_MessageNumber = gamebuf1.mid(28,4);
            Clients_Last_FB_MessageNumber = gamebuf1.mid(index + 4,4);

            holder259.replace(34,4,Clients_Last_MessageNumber);
            holder259.replace(38,4,Clients_Last_FB_MessageNumber);

            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);
            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

            increment recover1; //get class
            QByteArray recovercount2 = recover1.count(Servers_Last_MessageNumber); //packet increment
            holder259.replace(30,4,recovercount2); //replace new count at 30
            Servers_Last_MessageNumber = recovercount2.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

            IN_Opcodes.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,holder259);

            IN_Opcodes.OpcodeReturn.append(holder259);
            IN_Opcodes.OpcodeReturn.append("Server_CharCreation/CharDelete ");


            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }


        ///////////////////////////////////////////////////////////0400/3400////Coaching////////////////////////////////////


        if(IN_Opcodes.opcoderesult1 == "7D00")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            IN_Opcodes.MainFlag--;

            qDebug() << "IN_Opcodes.MainFlag2" << IN_Opcodes.MainFlag;

            if(IN_Opcodes.CheckPocketsNow == true)
            {
                IN_Opcodes.CheckPocketsNow = false;

                QString QuestsID = IN_Opcodes.MenuID_QuestID.value(IN_Opcodes.CurrentQuestMenuID);

                if(QuestsID.isEmpty() == true)
                {
                    QuestsID = IN_Opcodes.QuestsID;
                }

                if(IN_Opcodes.QuestsID_MenusID_ItemsID_QuantityVectorMap.contains(QuestsID))
                {
                    IN_Opcodes.QuestsID_MenusID_ItemsID_QuantityVectorMap.remove(QuestsID);
                }

                if(IN_Opcodes.QuestsID_MenusID_TunarQuantityVectorMap.contains(QuestsID))
                {
                    IN_Opcodes.QuestsID_MenusID_TunarQuantityVectorMap.remove(QuestsID);
                }

                IN_Opcodes.MenuID_QuestID.remove(IN_Opcodes.CurrentQuestMenuID);
                IN_Opcodes.CurrentQuestMenuID.clear();
                IN_Opcodes.QuestID.clear();


            }

            QMap<QString,QString> QuestLogSlot_QuestID_Map = Quests::PlayersID_QuestLogSlot_QuestID_MapOfMaps.value(IN_Opcodes.CharSelectID);

            qDebug() << "QuestLogSlot_QuestID_Map" << QuestLogSlot_QuestID_Map;

            QString Slot = opholder.mid(20,2);

            qDebug() << "Slot" << Slot;

            QuestLogSlot_QuestID_Map.insert(Slot,"");

            Quests::PlayersID_QuestLogSlot_QuestID_MapOfMaps.insert(IN_Opcodes.CharSelectID,QuestLogSlot_QuestID_Map);

            //FB070E00 7D000100000008
            //         7D000000000006

            QString Send_FB1 = opholder.mid(8,opholder.size());

            ADD_OR_SUB sendto;
            QByteArray Send_FB = sendto.FBSize(Send_FB1).toUtf8();

            qDebug() << "Send_FB" << Send_FB;

            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);
            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

            increment GrpOut;
            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            GrpOut_a = GrpOut.count(Servers_Last_FB_MessageNumber);
            Servers_Last_FB_MessageNumber = GrpOut_a.toUpper();

            int index = Send_FB.indexOf("XXXX");
            Send_FB.replace(index,4,Servers_Last_FB_MessageNumber);

            MessNumbVect.replace(0,Servers_Last_MessageNumber);
            MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

            Opcodes::CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

            Send_FB = IN_Opcodes.serverid1 + IN_Opcodes.clientid1 + "xxxx01BF543213" + IN_Opcodes.SessionID + "03" +
                    Servers_Last_MessageNumber + Clients_Last_MessageNumber + Clients_Last_FB_MessageNumber + Send_FB;

            packetsize ResizePacket; //call size class
            QByteArray ResizePacket2 = ResizePacket.switchsize(Send_FB); //get new size
            Send_FB.replace(8,4,ResizePacket2);

            crc sendcrc;
            QByteArray outcrc =  sendcrc.elcrc(Send_FB);
            Send_FB.append(outcrc);

            QByteArray Send_FB_out = Send_FB.fromHex(Send_FB);

            QHostAddress current_client_address;
            current_client_address.setAddress(QString::fromUtf8(IN_Opcodes.ClientsIP));
            quint16 current_client_port = IN_Opcodes.ClientsPort.toUShort(nullptr,10);
            QString changeIP = current_client_address.toString();

            QHostAddress newsender = QHostAddress(changeIP);
            worldthread::udpsocket3->writeDatagram(Send_FB_out,newsender,current_client_port);

            QString MainToon = checkthezone::NPCsNames.value(IN_Opcodes.CharSelectID);

            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append(dateTimeString);
            MainWindow::UiOputVector.append("Removing_Quest_Log");
            MainWindow::UiOputVector.append("To Client: " + MainToon);
            MainWindow::UiOputVector.append("IP: " + ClientsIP);
            MainWindow::UiOputVector.append("Port: " + ClientsPort);
            MainWindow::UiOputVector.append("From Server: Packet:");
            MainWindow::UiOputVector.append(Send_FB);
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append("");

            qDebug() << "";
            qDebug() << "";
            qDebug() << "----------------------------------------------------------";
            qDebug() << dateTimeString;
            qDebug() << "Remove_QuestLog";
            qDebug() << "To Client: " + MainToon;
            qDebug() << "IN_Opcodes.CharSelectID = " << IN_Opcodes.CharSelectID;
            qDebug() << "IP: " + IN_Opcodes.ClientsIP;
            qDebug() << "Port: " + IN_Opcodes.ClientsPort;
            qDebug() << "From Server: ";
            qDebug() << Send_FB;
            qDebug() << "----------------------------------------------------------";
            qDebug() << "";

            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);


            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

        }





        if(IN_Opcodes.opcoderesult1 == "0400")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QString NPCsID = QString::fromUtf8(opholder.mid(12,8).toUpper());

            qDebug() << "IN_Opcodes_0400.NPCsID" << NPCsID;

            IN_Opcodes.current_c9 = objectpacket::Master_Map.value(NPCsID.toUtf8());

            QString X_OldVal1 = IN_Opcodes.current_c9.mid(12,6);
            QString Y_OldVal1 = IN_Opcodes.current_c9.mid(24,6);
            QString Facing_OldVal1 = IN_Opcodes.current_c9.mid(30,2);

            qDebug() << "IN_Opcodes.X_OldVal1" << X_OldVal1;
            qDebug() << "IN_Opcodes.Y_OldVal1" << Y_OldVal1;

            qDebug() << "IN_Opcodes.Facing_OldVal1" << Facing_OldVal1;

            FacingDirection sendto; //get class
            QString Facing_Player = sendto.Facing_Player(ClientID,ClientsIP,ClientsPort,X_OldVal1,Y_OldVal1); //send xyz to FacingDirection

            if(Facing_Player != Facing_OldVal1)
            {
                QByteArray Old_C9 = IN_Opcodes.current_c9;
                QByteArray New_C9 = "";
                qlonglong Old_value = 0;
                QByteArray New_value = "";
                qlonglong New_value2 = 0;
                qlonglong Output_Xor = 0;
                QString Output_Xor_2 = "";

                New_C9 = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

                Old_value = Facing_OldVal1.toLongLong(nullptr,16);
                New_value = Facing_Player.toUtf8();//--------------------------------Facing
                New_value2 = New_value.toLongLong(nullptr,16);
                Output_Xor = Old_value ^ New_value2;
                Output_Xor_2 = QString("%1").arg(Output_Xor,2,16,QLatin1Char('0'));
                New_C9.replace(30,2,Output_Xor_2.toUtf8());//-----------------------Facing

                Old_C9.replace(30,2,Facing_Player.toUtf8());//facing

                xorpacket packet2;
                QByteArray Recompressed2 = packet2.packetencrypt(New_C9);

                QString Current_channel = IN_Opcodes.Map_Channel_ID.key(NPCsID);

                qDebug() << "IN_Opcodes.Current_channel" << Current_channel;

                QByteArray holderDespawn = Current_channel.toUtf8().toUpper() + "c9xxxx01" + Recompressed2;

                qDebug() << "IN_Opcodes.holderDespawn" << holderDespawn;

                QVector <QByteArray> CurrenChannelVector = IN_Opcodes.MapOfChannelVectors.value(Current_channel.toUtf8());
                CurrenChannelVector.append(holderDespawn);
                IN_Opcodes.MapOfChannelVectors.insert(Current_channel.toUtf8(),CurrenChannelVector);


                objectpacket::Master_Map.insert(NPCsID.toUtf8(),Old_C9);
            }


            qDebug() << "";
            qDebug() << "IN_Opcodes_Questing_Menus";
            qDebug() << "IN_Opcodes.opcoderesult1" << IN_Opcodes.opcoderesult1;
            qDebug() << "NPCsID" << NPCsID;
            qDebug() << "Quests::QuestGiversID_NameMap" << Quests::QuestGiversID_NameMap;

            IN_Opcodes.current_c9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8().toUpper());

            QString current_c9 = QString::fromUtf8(IN_Opcodes.current_c9);
            QString Race_OldVal1 = current_c9.mid(300,2);//race
            QString Level_OldVal1 = current_c9.mid(358,2);//level
            QString Class_OldVal1 = current_c9.mid(368,2);//class

            QString StartingCity = CharCreate::ID_StartingCityMap.value(IN_Opcodes.CharSelectID);

            QString Race1 = RaceValueToString.value(Race_OldVal1);
            QString Class1 = ClassValueToString.value(Class_OldVal1);
            QString Level1 = Level_OldVal1;

            QString NPCsName = checkthezone::NPCsNames.value(NPCsID);

            int coachmanFlag = CoachmanIDs_Flags.value(NPCsID);
            QString CoachmansCity = CoachmanIDs_Locations.value(NPCsID);
            int SkipQuest_Coach = 0;

            bool SkipAll = false;

            if(CoachmanIDs_Flags.contains(NPCsID))
            {
                SkipAll = true;

                SkipQuest_Coach++; // = 1

                if(CoachmansCity == StartingCity)
                {
                    SkipQuest_Coach++; // = 2

                    if(coachmanFlag == IN_Opcodes.MainFlag)
                    {
                        SkipQuest_Coach++; // = 3
                    }
                }
            }

            int SpiritmastersFlag = Spiritmasters_Flags.value(NPCsName);
            QString SpiritmastersCity = SpiritmastersIDs_Location.value(NPCsID);
            QString NPCsName2 = ClassRaceLocationSpiritmasters.value(Race1+Class1+StartingCity);

            int SkipQuest_Spiritmaster = 0;

            if(Spiritmasters_Flags.contains(NPCsName))
            {
                SkipAll = true;

                SkipQuest_Spiritmaster++; // = 1

                if(SpiritmastersCity.contains(StartingCity) || SpiritmastersCity == StartingCity)
                {
                    SkipQuest_Spiritmaster++; // = 2

                    if(NPCsName2 == NPCsName)
                    {
                        SkipQuest_Spiritmaster++; // = 3

                        if(SpiritmastersFlag == IN_Opcodes.MainFlag)
                        {
                            SkipQuest_Spiritmaster++; // = 4
                        }
                    }
                }
            }

            qDebug() << "NPCsName" << NPCsName;
            qDebug() << "SkipQuest_Coach" << SkipQuest_Coach;
            qDebug() << "SkipQuest_Spiritmaster" << SkipQuest_Spiritmaster;
            qDebug() << "SpiritmastersCity" << SpiritmastersCity;
            qDebug() << "SkipAll" << SkipAll;


            bool gotoquests = false;

            if(SkipQuest_Coach == 3)
            {
                gotoquests = true;
            }

            if(SkipQuest_Spiritmaster == 4)
            {
                gotoquests = true;
            }

            QMap<int,QString> CurrentFlag_MenuMap;

            bool CorrectClass = false;

            qDebug() << "StartingCity_x" << StartingCity;

            if(City_Class_QGiverVectorMap.contains(StartingCity))
            {
                QMap<QString,QVector<QString>> Class_QGiverVectorMap = City_Class_QGiverVectorMap.value(StartingCity);

                qDebug() << "Class1_x" << Class1;

                if(Class_QGiverVectorMap.contains(Class1))
                {
                    QVector<QString> QGiverVector = Class_QGiverVectorMap.value(Class1);

                    qDebug() << "NPCsName_x" << NPCsName;

                    if(QGiverVector.contains(NPCsName))
                    {
                        CorrectClass = true;
                    }
                }
            }

            if(Quests::QuestGiversID_Flag_MenusID_Map.contains(NPCsID) && SkipQuest_Spiritmaster == 0 && SkipQuest_Coach == 0)
            {
                if(CorrectClass == true)
                {
                    QString GuilMastersName = Class_GuildMasterMap.value(Class1);

                    if(IN_Opcodes.MainFlag == 0 && NPCsName == GuilMastersName)
                    {
                        IN_Opcodes.MainFlag++;

                        QString StartingFaction = Starting_FactionMap.value(StartingCity);
                        QString FBsizeOpcode = "1 faction with " + StartingFaction + " (1).";

                        QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                        QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                        ADD_OR_SUB SendToText;
                        QString FactionText = FBsizeOpcode;
                        QString FBText =  SendToText.TextSize(IN_Opcodes.ClientsID,IN_Opcodes.ClientsIP,IN_Opcodes.ClientsPort,FactionText,true,"7A0A",Servers_Last_FB_MessageNumber);

                        IN_Opcodes.SendFBsVector.append(FBText.toUtf8());
                        IN_Opcodes.FBReplyOrNotVector.append("00");
                        IN_Opcodes.FBPacketDescriptionVector.append("XP, ");

                        packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);
                        IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                    }

                    qDebug() << "IN_Opcodes.MainFlag3_x" << IN_Opcodes.MainFlag;

                    gotoquests = true;


                }
                else
                {
                    gotoquests = false;
                }
            }

            CurrentFlag_MenuMap = Quests::QuestGiversID_Flag_MenusID_Map.value(NPCsID);
            QString CurrentQuestMenuID = CurrentFlag_MenuMap.value(IN_Opcodes.MainFlag);

            if(CurrentQuestMenuID.isEmpty() == false)
            {
                IN_Opcodes.CurrentQuestMenuID = CurrentQuestMenuID;
            }
            else
            {
                gotoquests = false;
            }

            QByteArray coach_packet = "";

            qDebug() << "are we there yet coach gotoquests" << gotoquests;

            if(gotoquests == true)
            {
                qDebug() << "IN_Opcodes.MainFlag4_x" << IN_Opcodes.MainFlag;

                QVector<QString> CheckVector;

                qDebug() << "IN_Opcodes.CurrentQuestMenuID_2_x" << IN_Opcodes.CurrentQuestMenuID;

                qDebug() << "IN_Opcodes.CurrentFlag_MenuMap_x" << CurrentFlag_MenuMap;

                if(IN_Opcodes.CurrentQuestMenuID.contains("P_"))
                {
                    QMap<QString,QMap<int,QVector<QString>>> OP_Flag_CheckVectorMaps = Quests::QGID_OP_Flag_CheckVectorMaps.value(NPCsID);
                    QMap<int,QVector<QString>> Flag_CheckVectorMap;

                    //----------------------------------------------------------------
                    Flag_CheckVectorMap = OP_Flag_CheckVectorMaps.value("8");//IC

                    if(Flag_CheckVectorMap.contains(IN_Opcodes.MainFlag))
                    {
                        CheckVector = Flag_CheckVectorMap.value(IN_Opcodes.MainFlag);
                    }
                    //----------------------------------------------------------------
                    Flag_CheckVectorMap = OP_Flag_CheckVectorMaps.value("32");//TC

                    if(Flag_CheckVectorMap.contains(IN_Opcodes.MainFlag))
                    {
                        QVector<QString> PopupVect = Flag_CheckVectorMap.value(IN_Opcodes.MainFlag);

                        if(PopupVect.isEmpty() == false)
                        {
                            QString Popup = PopupVect.at(0);

                            if(!CheckVector.contains(Popup))
                            {
                                CheckVector.append(Popup);
                            }
                        }
                    }
                }
                //----------------------------------------------------------------
                QVector<QString> PocketCheckVector;

                if(CheckVector.isEmpty() == false)
                {
                    CheckVector.insert(0,NPCsID);

                    qDebug() << "CheckVector_A_x" << CheckVector;

                    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                    Quests sendto;
                    PocketCheckVector = sendto.CheckYoPockets(ID_IP_PORT,CheckVector);

                    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                    qDebug() << "PocketCheckVector_A_x" << PocketCheckVector;

                    if(PocketCheckVector.isEmpty() == false)
                    {
                        PocketCheckVector.removeAt(0);
                        PocketCheckVector.removeAt(0);

                        for(int re = 0; re < PocketCheckVector.size(); re++)
                        {
                            QString CurrentQuestMenuID = PocketCheckVector.at(re);

                            QByteArray PopUpsReturn = sendto.SendPopUps(ID_IP_PORT,CurrentQuestMenuID);
                            SendPacketOut.append(PopUpsReturn);
                        }
                    }
                }

                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);
                IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                if(IN_Opcodes.CurrentQuestMenuID.contains("M_") && !IN_Opcodes.CurrentQuestMenuID.contains("SM_"))
                {
                    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                    Quests sendto;
                    QByteArray MenusReturn = sendto.SendoutMenus(ID_IP_PORT,IN_Opcodes.CurrentQuestMenuID);
                    SendPacketOut.append(MenusReturn);

                    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                }

                if(IN_Opcodes.CurrentQuestMenuID.contains("P_") && PocketCheckVector.isEmpty() == true)
                {
                    QVector<QString> tempPopUpResultVect = Quests::PopUpID_PopUpResltsVectorMap.value(IN_Opcodes.CurrentQuestMenuID);

                    qDebug() << "tempPopUpResultVect_x" << tempPopUpResultVect;

                    for(int re = 0; re < tempPopUpResultVect.size(); re++)
                    {
                        QString ResultID = tempPopUpResultVect.at(re);

                        qDebug() << "ResultID1_x" << ResultID;

                        if(!ResultID.contains("xp_"))
                        {
                            PocketCheckVector.append(ResultID);
                        }
                        else
                        {
                            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                            Quests sendto;
                            QString XP = sendto.GenerateXP(ID_IP_PORT,ResultID);

                            qDebug() << "IN_Opcodes.XP" << XP;

                            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                        }
                    }

                    PocketCheckVector.insert(0,IN_Opcodes.CurrentQuestMenuID);

                    qDebug() << "PocketCheckVector_x" << PocketCheckVector;

                    bool InventoryIsFull = false;

                    for(int re = 0; re < PocketCheckVector.size(); re++)
                    {
                        packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                        IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                        QString ResultID = PocketCheckVector.at(re);

                        if(ResultID.contains("M_") && !ResultID.contains("SM_"))
                        {
                            qDebug() << "ResultID2_x" << ResultID;

                            IN_Opcodes.CurrentQuestMenuID = ResultID;
                            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                            Quests sendto;
                            QByteArray MenusReturn = sendto.SendoutMenus(ID_IP_PORT,ResultID);
                            SendPacketOut.append(MenusReturn);

                            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                        }

                        if(ResultID.contains("P_"))
                        {
                            qDebug() << "ResultID3_x" << ResultID;

                            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                            Quests sendto;
                            QByteArray PopUPsReturn = sendto.SendPopUps(ID_IP_PORT,ResultID);
                            SendPacketOut.append(PopUPsReturn);

                            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                        }

                        if(ResultID.contains("IG_") || ResultID.contains("IR_"))
                        {
                            qDebug() << "ResultID" << ResultID;

                            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                            Quests sendto;
                            InventoryIsFull = sendto.SendtoItems(ID_IP_PORT,ResultID);

                            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                        }

                        if(ResultID.contains("TR_") || ResultID.contains("TG_"))
                        {
                            qDebug() << "ResultID" << ResultID;

                            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                            Quests sendto;
                            QByteArray Tunar = sendto.SendtoTunar(ID_IP_PORT,ResultID);
                            SendPacketOut.append(Tunar);

                            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                        }

                        if(ResultID.contains("sp_"))
                        {

                        }

                        if(ResultID.contains("TS_"))
                        {
                            Quests sendto;
                            QByteArray TargetSpellReturn = sendto.SendoutSpell(ID_IP_PORT,ResultID);
                            SendPacketOut.append(TargetSpellReturn);

                            qDebug() << "TargetSpellReturn" << TargetSpellReturn;
                        }

                        if(ResultID.contains("QLa_"))
                        {
                            qDebug() << "in_QLa_";

                            Quests sendto;
                            ResultID.remove(0,4);
                            QByteArray QuestLog = sendto.AddQuestLog(ID_IP_PORT,ResultID);

                            SendPacketOut.append(QuestLog);

                            IN_Opcodes.MainFlag++;
                            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);
                            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                            qDebug() << "IN_Opcodes.MainFlag5" << IN_Opcodes.MainFlag;
                            qDebug() << "QuestLogID" << ResultID;
                            qDebug() << "QuestLog" << QuestLog;
                            qDebug() << "IN_Opcodes.MainFlag_QLa" << IN_Opcodes.MainFlag;
                        }

                        if(ResultID.contains("QLr_"))
                        {
                            if(InventoryIsFull == false)
                            {
                                Quests sendto;
                                ResultID.remove(0,4);
                                QByteArray QuestLog = sendto.RemoveQuestLog(ID_IP_PORT,ResultID);

                                SendPacketOut.append(QuestLog);

                                IN_Opcodes.MainFlag++;
                                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);
                                IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                                qDebug() << "IN_Opcodes.MainFlag6" << IN_Opcodes.MainFlag;

                                qDebug() << "QuestLogID" << ResultID;
                                qDebug() << "QuestLog" << QuestLog;
                            }

                            qDebug() << "IN_Opcodes.MainFlag_QLr" << IN_Opcodes.MainFlag;
                        }

                        if(ResultID.contains("MC_"))
                        {
                            IN_Opcodes.ModelTimer = 4000;
                            IN_Opcodes.ModelResult = ResultID;
                            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);
                            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                        }
                    }

                    qDebug() << "IN_Opcodes.PocketCheckVector" << PocketCheckVector;
                }
                else
                {
                    PocketCheckVector.clear();
                }
            }
            else
            {
                if(SkipAll == false)
                {
                    QMap<QString,QMap<int,QVector<QString>>>
                            OP_Flag_Popup_36sVectorMaps =
                            Quests::QGID_OP_Flag_Popup_36sVectorMaps.value(NPCsID);

                    QMap<int,QVector<QString>> Flag_Popup_36sVectorMaps =
                            OP_Flag_Popup_36sVectorMaps.value("36");

                    QVector<QString> Popup_36sVector =
                            Flag_Popup_36sVectorMaps.value(-2);

                    for(int re = 2; re < Popup_36sVector.size(); re++)
                    {
                        IN_Opcodes.CurrentQuestMenuID = Popup_36sVector.at(re);
                        QString ResultID = IN_Opcodes.CurrentQuestMenuID;
                        qDebug() << "ResultID_36" << ResultID;

                        packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                        Quests sendto;
                        QByteArray PopUPsReturn = sendto.SendPopUps(ID_IP_PORT,ResultID);
                        SendPacketOut.append(PopUPsReturn);

                        IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                    }
                }
            }

            if(SkipQuest_Coach != 0)
            {
                qDebug() << "are we there yet coach1";

                QByteArray coach1 =  opholder.mid(12,8); //might be the right place?? lol
                IN_Opcodes.coachtext1 = QString::fromUtf8(coach1);
                Coaching maincoach;

                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

                QByteArray coach_packet = maincoach.coach(ClientID,ClientsIP,ClientsPort,IN_Opcodes.coachtext1);

                IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

                if(SkipQuest_Coach != 3)
                {
                    IN_Opcodes.SendFBsVector.append(coach_packet);
                    IN_Opcodes.FBReplyOrNotVector.append("03");
                    IN_Opcodes.FBPacketDescriptionVector.append("Coach_Window1, ");
                }
            }

            if((coach_packet == "NULL") || (SkipQuest_Coach == 0 && SkipQuest_Spiritmaster == 0))
            {
                coach_packet = "d37c5ae7f5c101bf543213" + IN_Opcodes.SessionID + "03xxxx";

                coach_packet.append(Clients_Last_MessageNumber);
                coach_packet.append(Clients_Last_FB_MessageNumber);

                packetsize getnewsize1; //call size class
                QByteArray coachnewsize = getnewsize1.switchsize(coach_packet); //get new size
                coach_packet.replace(8,4,coachnewsize);

                QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);
                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                increment GrpOut;
                QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                coach_packet.replace(30,4,GrpOut_a);
                Servers_Last_MessageNumber = GrpOut_a.toUpper();

                MessNumbVect.replace(0,Servers_Last_MessageNumber);

                CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                IN_Opcodes.OpcodeReturn.append(coach_packet);
                IN_Opcodes.OpcodeReturn.append("Server_NULL_coach_packet ");
            }


            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);



            if(gotoquests == false)
            {
                if(SkipQuest_Coach != 0)
                {
                    qDebug() << "are we there yet coach2";

                    QByteArray coach1 =  opholder.mid(12,8); //might be the right place?? lol
                    IN_Opcodes.coachtext1 = QString::fromUtf8(coach1);
                    Coaching maincoach;

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

                    coach_packet = maincoach.coach(ClientID,ClientsIP,ClientsPort,IN_Opcodes.coachtext1);

                    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

                    qDebug() << "coach_packet" << coach_packet;

                    if(SkipQuest_Coach != 3)
                    {
                        IN_Opcodes.SendFBsVector.append(coach_packet);
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Coach_Window2, ");
                    }
                }



                if((coach_packet == "NULL") || (SkipQuest_Coach == 0 && SkipQuest_Spiritmaster == 0))
                {
                    coach_packet = "d37c5ae7f5c101bf543213" + IN_Opcodes.SessionID + "03xxxx";

                    coach_packet.append(Clients_Last_MessageNumber);
                    coach_packet.append(Clients_Last_FB_MessageNumber);

                    packetsize getnewsize1; //call size class
                    QByteArray coachnewsize = getnewsize1.switchsize(coach_packet); //get new size
                    coach_packet.replace(8,4,coachnewsize);

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    increment GrpOut;
                    QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                    coach_packet.replace(30,4,GrpOut_a);
                    Servers_Last_MessageNumber = GrpOut_a.toUpper();

                    MessNumbVect.replace(0,Servers_Last_MessageNumber);

                    CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);


                    IN_Opcodes.OpcodeReturn.append(coach_packet);
                    IN_Opcodes.OpcodeReturn.append("Server_NULL_coach_packet ");
                }
            }

            if(SkipQuest_Spiritmaster > 0 && SkipQuest_Spiritmaster != 4)
            {
                QString ResultID = "SM_" + NPCsID;

                qDebug() << "ResultID" << ResultID;

                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                Quests sendto;
                QByteArray MenusReturn = sendto.SendoutMenus(ID_IP_PORT,ResultID);
                SendPacketOut.append(MenusReturn);

                IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
            }

            qDebug() << "IN_Opcodes.SendPacketOut1" << SendPacketOut;

            QVector<QByteArray> TempSendVect;

            if(SendPacketOut.isEmpty() == false)
            {
                QByteArray NextPacket = "";
                QByteArray NextOp = "";
                int SizeNP = 0;
                int re = 0;

                for(re = 0; re < SendPacketOut.size(); re++)
                {
                    qDebug() << "A " <<  re << " of " << SendPacketOut.size();

                    NextOp = SendPacketOut.at(re);

                    if(NextPacket.size() + NextOp.size() <= 2374)
                    {
                        NextPacket.append(NextOp);
                        SizeNP = NextPacket.size();

                        qDebug() << "B " <<  re << " of " << SendPacketOut.size();

                        qDebug() << "IN_Opcodes.re_1" << re;
                        qDebug() << "IN_Opcodes.SizeNP_1" << SizeNP;
                        qDebug() << "IN_Opcodes.NextPacket_1" << NextPacket.size();
                        qDebug() << "IN_Opcodes.NextOp_1" << NextOp;
                    }
                    else
                    {
                        TempSendVect.append(NextPacket);

                        SizeNP = 2375;

                        NextPacket = NextOp;

                        qDebug() << "";
                        qDebug() << "IN_Opcodes.Not_Added";
                        qDebug() << "IN_Opcodes.re_2" << re;
                        qDebug() << "IN_Opcodes.SizeNP_2" << SizeNP;
                        qDebug() << "IN_Opcodes.NextPacket_2" << NextPacket.size();
                        qDebug() << "IN_Opcodes.NextOp_2" << NextOp;
                    }

                    qDebug() << "C " << re << " of " << SendPacketOut.size();
                }

                if((TempSendVect.isEmpty() == true && re == SendPacketOut.size())
                        || (!TempSendVect.contains(NextPacket)))
                {
                    TempSendVect.append(NextPacket);
                }

                qDebug() << "D " <<  re << " of " << SendPacketOut.size();

                qDebug() << "IN_Opcodes.TempSendVect" << TempSendVect;

                for(int re = 0; re < TempSendVect.size(); re++)
                {
                    QByteArray NextPacket = TempSendVect.at(re);

                    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                    increment GrpOut;
                    QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);

                    Servers_Last_MessageNumber = GrpOut_a.toUpper();

                    MessNumbVect.replace(0,Servers_Last_MessageNumber);

                    Opcodes::CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);


                    QByteArray Send_FB = IN_Opcodes.serverid1 + IN_Opcodes.clientid1 + "xxxx01BF543213" + IN_Opcodes.SessionID + "03" +
                            Servers_Last_MessageNumber + Clients_Last_MessageNumber + Clients_Last_FB_MessageNumber + NextPacket;

                    packetsize ResizePacket; //call size class
                    QByteArray ResizePacket2 = ResizePacket.switchsize(Send_FB); //get new size
                    Send_FB.replace(8,4,ResizePacket2);

                    crc sendcrc;
                    QByteArray outcrc =  sendcrc.elcrc(Send_FB);
                    Send_FB.append(outcrc);

                    QByteArray Send_FB_out = Send_FB.fromHex(Send_FB);

                    QHostAddress current_client_address;
                    current_client_address.setAddress(QString::fromUtf8(IN_Opcodes.ClientsIP));
                    quint16 current_client_port = IN_Opcodes.ClientsPort.toUShort(nullptr,10);
                    QString changeIP = current_client_address.toString();

                    QHostAddress newsender = QHostAddress(changeIP);
                    worldthread::udpsocket3->writeDatagram(Send_FB_out,newsender,current_client_port);

                    QString MainToon = checkthezone::NPCsNames.value(IN_Opcodes.CharSelectID);

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

                    MainWindow::UiOputVector.append("");
                    MainWindow::UiOputVector.append("");
                    MainWindow::UiOputVector.append("----------------------------------------------------------");
                    MainWindow::UiOputVector.append(dateTimeString);
                    MainWindow::UiOputVector.append("OpcodeReply: 0400 " + QString::number(re));
                    MainWindow::UiOputVector.append("To Client: " + MainToon);
                    MainWindow::UiOputVector.append("IP: " + ClientsIP);
                    MainWindow::UiOputVector.append("Port: " + ClientsPort);
                    MainWindow::UiOputVector.append("From Server: Packet:");
                    MainWindow::UiOputVector.append(Send_FB);
                    MainWindow::UiOputVector.append("----------------------------------------------------------");
                    MainWindow::UiOputVector.append("");

                    qDebug() << "";
                    qDebug() << "";
                    qDebug() << "----------------------------------------------------------";
                    qDebug() << dateTimeString;
                    qDebug() << "Quest_Menu_PopUps " << re;
                    qDebug() << "To Client: " + MainToon;
                    qDebug() << "IN_Opcodes.CharSelectID = " << IN_Opcodes.CharSelectID;
                    qDebug() << "IP: " + IN_Opcodes.ClientsIP;
                    qDebug() << "Port: " + IN_Opcodes.ClientsPort;
                    qDebug() << "From Server: ";
                    qDebug() << Send_FB;
                    qDebug() << "----------------------------------------------------------";
                    qDebug() << "";
                }

                SendPacketOut.clear();
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }


        /////////////////////////////////////////////3200/CD00/CE00//////////////////////////////////////////////////////////////////////////


        if(IN_Opcodes.opcoderesult1 == "3200") //button presses on the spell bar
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray slot1 = opholder.mid(21,1); //slot
            QByteArray target1 = opholder.mid(22,8); //target


            if((slot1 == "0" && IN_Opcodes.slot_0_trigger == true) ||
                    (slot1 == "1" && IN_Opcodes.slot_1_trigger == true) || //only send if spell has been cast
                    (slot1 == "2" && IN_Opcodes.slot_2_trigger == true) ||
                    (slot1 == "3" && IN_Opcodes.slot_3_trigger == true) ||
                    (slot1 == "4" && IN_Opcodes.slot_4_trigger == true) ||
                    (slot1 == "5" && IN_Opcodes.slot_5_trigger == true) ||
                    (slot1 == "6" && IN_Opcodes.slot_6_trigger == true) ||
                    (slot1 == "7" && IN_Opcodes.slot_7_trigger == true) ||
                    (slot1 == "8" && IN_Opcodes.slot_8_trigger == true) ||
                    (slot1 == "9" && IN_Opcodes.slot_9_trigger == true))
            {

                //////////////////////Spell Recovery//////////////////////////////


                QByteArray spell_recovery = "ce00300000005200650063006F00760065007200790020007400"
                                            "69006D006500200066006F00720020007400680061007400200061006200"
                                            "69006C00690074007900200068006100730020006E006F00740020006200"
                                            "650065006E0020006D00650074002E00";

                IN_Opcodes.SendFBsVector.append(spell_recovery);
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Spell_Recovery, ");

                spell_recovery = "b100" + IN_Opcodes.CharSelectID_Main.toUtf8() + IN_Opcodes.CharSelectID_Main.toUtf8() +"00000000004a85c79e";

                IN_Opcodes.SendFBsVector.append(spell_recovery);
                IN_Opcodes.FBReplyOrNotVector.append("00");
                IN_Opcodes.FBPacketDescriptionVector.append("Spell_not_ready, ");
            }

            //////////////////////////////////////Main Spells//////////////////////////////////////////

            IN_Opcodes.cooldown_0 = false;
            IN_Opcodes.cooldown_1 = false;
            IN_Opcodes.cooldown_2 = false;
            IN_Opcodes.cooldown_3 = false;
            IN_Opcodes.cooldown_4 = false;
            IN_Opcodes.cooldown_5 = false;
            IN_Opcodes.cooldown_6 = false;
            IN_Opcodes.cooldown_7 = false;
            IN_Opcodes.cooldown_8 = false;
            IN_Opcodes.cooldown_9 = false;

            if(slot1 == "0" && IN_Opcodes.slot_0_trigger == false)
            {
                qDebug() << "Got here 0";

                IN_Opcodes.spell_position =  IN_Opcodes.spellslot_map.value(0);
                IN_Opcodes.spell_str1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_position);
                IN_Opcodes.spell_idout = QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'));
                IN_Opcodes.slot_0_trigger = true;
                IN_Opcodes.cooldown_0 = true;
                increment kick4; //get class
                QByteArray kickcount5 = kick4.count(IN_Opcodes.spell_0_counter); //spell counter
                IN_Opcodes.spell_0_counter = kickcount5.toUpper(); //replace new count
                IN_Opcodes.another_spell_count = IN_Opcodes.spell_0_counter;
            }

            if(slot1 == "1" && IN_Opcodes.slot_1_trigger == false)
            {
                qDebug() << "Got here 1";

                IN_Opcodes.spell_position =  IN_Opcodes.spellslot_map.value(1);
                IN_Opcodes.spell_str1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_position);
                IN_Opcodes.spell_idout = QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'));
                IN_Opcodes.slot_1_trigger = true;
                IN_Opcodes.cooldown_1 = true;
                increment kick4; //get class
                QByteArray kickcount5 = kick4.count(IN_Opcodes.spell_1_counter); //spell counter
                IN_Opcodes.spell_1_counter = kickcount5.toUpper(); //replace new count
                IN_Opcodes.another_spell_count = IN_Opcodes.spell_1_counter;
            }

            if(slot1 == "2" && IN_Opcodes.slot_2_trigger == false)
            {
                qDebug() << "Got here 2";

                IN_Opcodes.spell_position =  IN_Opcodes.spellslot_map.value(2);
                IN_Opcodes.spell_str1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_position);
                IN_Opcodes.spell_idout = QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'));
                IN_Opcodes.slot_2_trigger = true;
                IN_Opcodes.cooldown_2 = true;
                increment kick4; //get class
                QByteArray kickcount5 = kick4.count(IN_Opcodes.spell_2_counter); //spell counter
                IN_Opcodes.spell_2_counter = kickcount5.toUpper(); //replace new count
                IN_Opcodes.another_spell_count = IN_Opcodes.spell_2_counter;
            }

            if(slot1 == "3" && IN_Opcodes.slot_3_trigger == false)
            {
                qDebug() << "Got here 3";

                IN_Opcodes.spell_position =  IN_Opcodes.spellslot_map.value(3);
                IN_Opcodes.spell_str1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_position);
                IN_Opcodes.spell_idout = QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'));
                IN_Opcodes.slot_3_trigger = true;
                IN_Opcodes.cooldown_3 = true;
                increment kick4; //get class
                QByteArray kickcount5 = kick4.count(IN_Opcodes.spell_3_counter); //spell counter
                IN_Opcodes.spell_3_counter = kickcount5.toUpper(); //replace new count
                IN_Opcodes.another_spell_count = IN_Opcodes.spell_3_counter;
            }

            if(slot1 == "4" && IN_Opcodes.slot_4_trigger == false)
            {
                qDebug() << "Got here 4";

                IN_Opcodes.spell_position =  IN_Opcodes.spellslot_map.value(4);
                IN_Opcodes.spell_str1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_position);
                IN_Opcodes.spell_idout = QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'));
                IN_Opcodes.slot_4_trigger = true;
                IN_Opcodes.cooldown_4 = true;
                increment kick4; //get class
                QByteArray kickcount5 = kick4.count(IN_Opcodes.spell_4_counter); //spell counter
                IN_Opcodes.spell_4_counter = kickcount5.toUpper(); //replace new count
                IN_Opcodes.another_spell_count = IN_Opcodes.spell_4_counter;
            }

            if(slot1 == "5" && IN_Opcodes.slot_5_trigger == false)
            {
                qDebug() << "Got here 5";

                IN_Opcodes.spell_position =  IN_Opcodes.spellslot_map.value(5);
                IN_Opcodes.spell_str1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_position);
                IN_Opcodes.spell_idout = QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'));
                IN_Opcodes.slot_5_trigger = true;
                IN_Opcodes.cooldown_5 = true;
                increment kick4; //get class
                QByteArray kickcount5 = kick4.count(IN_Opcodes.spell_5_counter); //spell counter
                IN_Opcodes.spell_5_counter = kickcount5.toUpper(); //replace new count
                IN_Opcodes.another_spell_count = IN_Opcodes.spell_5_counter;
            }

            if(slot1 == "6" && IN_Opcodes.slot_6_trigger == false)
            {
                qDebug() << "Got here 6";

                IN_Opcodes.spell_position =  IN_Opcodes.spellslot_map.value(6);
                IN_Opcodes.spell_str1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_position);
                IN_Opcodes.spell_idout = QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'));
                IN_Opcodes.slot_6_trigger = true;
                IN_Opcodes.cooldown_6 = true;
                increment kick4; //get class
                QByteArray kickcount5 = kick4.count(IN_Opcodes.spell_6_counter); //spell counter
                IN_Opcodes.spell_6_counter = kickcount5.toUpper(); //replace new count
                IN_Opcodes.another_spell_count = IN_Opcodes.spell_6_counter;
            }

            if(slot1 == "7"  && IN_Opcodes.slot_7_trigger == false)
            {
                qDebug() << "Got here 7";

                IN_Opcodes.spell_position =  IN_Opcodes.spellslot_map.value(7);
                IN_Opcodes.spell_str1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_position);
                IN_Opcodes.spell_idout = QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'));
                IN_Opcodes.slot_7_trigger = true;
                IN_Opcodes.cooldown_7 = true;
                increment kick4; //get class
                QByteArray kickcount5 = kick4.count(IN_Opcodes.spell_7_counter); //spell counter
                IN_Opcodes.spell_7_counter = kickcount5.toUpper(); //replace new count
                IN_Opcodes.another_spell_count = IN_Opcodes.spell_7_counter;
            }

            if(slot1 == "8"  && IN_Opcodes.slot_8_trigger == false)
            {
                qDebug() << "Got here 8";

                IN_Opcodes.spell_position =  IN_Opcodes.spellslot_map.value(8);
                IN_Opcodes.spell_str1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_position);
                IN_Opcodes.spell_idout = QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'));
                IN_Opcodes.slot_8_trigger = true;
                IN_Opcodes.cooldown_8 = true;
                increment kick4; //get class
                QByteArray kickcount5 = kick4.count(IN_Opcodes.spell_8_counter); //spell counter
                IN_Opcodes.spell_8_counter = kickcount5.toUpper(); //replace new count
                IN_Opcodes.another_spell_count = IN_Opcodes.spell_8_counter;
            }

            if(slot1 == "9"  && IN_Opcodes.slot_9_trigger == false)
            {
                qDebug() << "Got here 9";

                IN_Opcodes.spell_position =  IN_Opcodes.spellslot_map.value(9);
                IN_Opcodes.spell_str1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_position);
                IN_Opcodes.spell_idout = QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'));
                IN_Opcodes.slot_9_trigger = true;
                IN_Opcodes.cooldown_9 = true;
                increment kick4; //get class
                QByteArray kickcount5 = kick4.count(IN_Opcodes.spell_9_counter); //spell counter
                IN_Opcodes.spell_9_counter = kickcount5.toUpper(); //replace new count
                IN_Opcodes.another_spell_count = IN_Opcodes.spell_9_counter;
            }


            if((slot1 == "0" && IN_Opcodes.cooldown_0 == true) ||
                    (slot1 == "1" &&  IN_Opcodes.cooldown_1 == true) || //only send if spell has been cast
                    (slot1 == "2" &&  IN_Opcodes.cooldown_2 == true) ||
                    (slot1 == "3" && IN_Opcodes.cooldown_3 ==   true) ||
                    (slot1 == "4" && IN_Opcodes.cooldown_4 ==  true) ||
                    (slot1 == "5" && IN_Opcodes.cooldown_5 ==   true) ||
                    (slot1 == "6" && IN_Opcodes.cooldown_6 ==    true) ||
                    (slot1 == "7" && IN_Opcodes.cooldown_7 ==   true) ||
                    (slot1 == "8" && IN_Opcodes.cooldown_8 == true) ||
                    (slot1 == "9" && IN_Opcodes.cooldown_9 ==  true))
            {

                int currsize = IN_Opcodes.spell_str1.size();
                int timer_pos1 = IN_Opcodes.spell_str1.lastIndexOf("##",-1);
                int spell_graphic1 = IN_Opcodes.spell_str1.indexOf("##",0);
                QString spell_graph2 = IN_Opcodes.spell_str1.mid(spell_graphic1 + 2, 8); //graphic
                QString timer_times1 = IN_Opcodes.spell_str1.mid(timer_pos1 + 2,currsize - timer_pos1); //time
                int book_pos = IN_Opcodes.spell_str1.indexOf("%%",0);
                IN_Opcodes.spell_str1.replace(book_pos + 2,2,QString("%1").arg(IN_Opcodes.spell_position,2,16,QLatin1Char('0'))); //current book id

                qDebug() << "currsize" << currsize;
                qDebug() << "timer_pos1" << timer_pos1;
                qDebug() << "spell_graphic1" << spell_graphic1;
                qDebug() << "spell_graph2" << spell_graph2;
                qDebug() << "timer_times1" << timer_times1;
                qDebug() << "book_pos" << book_pos;
                qDebug() << "IN_Opcodes.spell_str1" << IN_Opcodes.spell_str1;

                QByteArray main_spell = "CD000000000000D007";
                main_spell.replace(4,2,IN_Opcodes.spell_idout.toUtf8()); //spell id out
                main_spell.replace(6,4,IN_Opcodes.another_spell_count); //replace new count at 48  56

                IN_Opcodes.SendFBsVector.append(main_spell);
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Grey_Spell, ");

                int CastTime = timer_times1.toInt(nullptr,10);

                IN_Opcodes.SpellCoolDownTimer = CastTime + 3000;

                QString CastTime1 = QString("%1").arg(CastTime,4,16,QLatin1Char('0')).toUpper();

                qDebug() << "CastTime1" << CastTime1;

                QString CastTime1stByte = CastTime1.mid(0,2);
                QString CastTime2ndByte = CastTime1.mid(2,2);
                QString timer_times2 = CastTime2ndByte + CastTime1stByte;

                qDebug() << "timer_times2" << timer_times2;

                main_spell = "b100" + IN_Opcodes.CharSelectID_Main.toUtf8() + "5a18060000" + timer_times2.toUtf8() + "00002E6E280A";

                if(target1 == "FFFFFFFF")
                {
                    target1 = IN_Opcodes.CharSelectID_Main.toUtf8();
                }

                main_spell.replace(12,8,target1); //replace target
                main_spell.replace(30,8,spell_graph2.toUtf8()); //graphic

                IN_Opcodes.SendFBsVector.append(main_spell);
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Spell_Graphic, ");

            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }



        //////////////////////////////////////////////////////////3300 Spell_Bar_Memorize////////////////////////////////////////////////////////////////////////////////////////////////

        //7CF8D37C8F30" + IN_Opcodes.SessionID + "00A504 FB081A00 3300 00000000[01 00][BCCBF90E]

        if(IN_Opcodes.opcoderesult1 == "3300") //memorize spell
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray Old_ID = opholder.mid(20,2);
            QByteArray New_Slot = opholder.mid(22,2);
            QByteArray Inc_Memorize = opholder.mid(12,4);

            int set_idmap1 = Old_ID.toInt(nullptr,16); //spell id
            int set_posmap1 = New_Slot.toInt(nullptr,10); //slot

            IN_Opcodes.spellslot_map.insert(set_posmap1,set_idmap1);

            QByteArray bar_swap = "B40001000000xxXX";

            increment bar_inc;
            QByteArray Memorize2 = bar_inc.count(Inc_Memorize); //packet increment
            bar_swap.replace(4,4);

            bar_swap.replace(12,2,Old_ID);
            bar_swap.replace(14,2,New_Slot);

            IN_Opcodes.SendFBsVector.append(bar_swap);
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("Spell_Memorize, ");


            //spellunmemorize(); //jumping to spellunmemorize

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

        }




        ///////////////////////////////////////////////////3500/4600//////////////////////////////////////////////////////////////////////////////

        if(IN_Opcodes.opcoderesult1 == "3500") //Menu result
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            qDebug() << "";
            qDebug() << "IN_Opcodes.opcoderesult1" << IN_Opcodes.opcoderesult1;
            qDebug() << "IN_Opcodes.Quest" << IN_Opcodes.Quest;
            qDebug() << "IN_Opcodes.WorldGreetingFlag" << IN_Opcodes.WorldGreetingFlag;
            qDebug() << "IN_Opcodes.OpcodesAndGamebuf1Map" << IN_Opcodes.OpcodesAndGamebuf1Map;

            QString NPCsID = IN_Opcodes.Target_ID;// fix this

            IN_Opcodes.current_c9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8().toUpper());

            QString current_c9 = QString::fromUtf8(IN_Opcodes.current_c9);
            QString Race_OldVal1 = current_c9.mid(300,2);//race
            QString Class_OldVal1 = current_c9.mid(368,2);//class

            QString StartingCity = CharCreate::ID_StartingCityMap.value(IN_Opcodes.CharSelectID);

            QString Race1 = RaceValueToString.value(Race_OldVal1);
            QString Class1 = ClassValueToString.value(Class_OldVal1);

            int coachmanFlag = CoachmanIDs_Flags.value(NPCsID);
            QString CoachmansCity = CoachmanIDs_Locations.value(NPCsID);
            int SkipQuest_Coach = 0;

            if(CoachmanIDs_Flags.contains(NPCsID))
            {
                SkipQuest_Coach++; // = 1

                if(CoachmansCity == StartingCity)
                {
                    SkipQuest_Coach++; // = 2

                    if(coachmanFlag == IN_Opcodes.MainFlag)
                    {
                        SkipQuest_Coach++; // = 3
                    }
                }
            }

            QString NPCsName = checkthezone::NPCsNames.value(NPCsID);
            int SpiritmastersFlag = Spiritmasters_Flags.value(NPCsName);
            QString SpiritmastersCity = SpiritmastersIDs_Location.value(NPCsID);
            QString NPCsName2 = ClassRaceLocationSpiritmasters.value(Race1+Class1+StartingCity);

            qDebug() << "IN_Opcodes.NPCsName" << NPCsName;


            int SkipQuest_Spiritmaster = 0;

            if(Spiritmasters_Flags.contains(NPCsName))
            {
                qDebug() << "IN_Opcodes.SpiritmastersFlag" << SpiritmastersFlag;
                qDebug() << "IN_Opcodes.SpiritmastersCity" << SpiritmastersCity;
                qDebug() << "IN_Opcodes.NPCsName2" << NPCsName2;

                SkipQuest_Spiritmaster++; // = 1

                if(SpiritmastersCity.contains(StartingCity) || SpiritmastersCity == StartingCity)
                {
                    SkipQuest_Spiritmaster++; // = 2

                    if(NPCsName2 == NPCsName)
                    {
                        SkipQuest_Spiritmaster++; // = 3

                        if(SpiritmastersFlag == IN_Opcodes.MainFlag)
                        {
                            SkipQuest_Spiritmaster++; // = 4
                        }
                    }
                }
            }

            //FB072100350008000000FF
            if(opholder.mid(20,2) == "FF")
            {
                SkipQuest_Coach = -1;
                SkipQuest_Spiritmaster = -1;
            }

            qDebug() << "IN_Opcodes.SkipQuest_Coach" << SkipQuest_Coach;
            qDebug() << "IN_Opcodes.SkipQuest_Spiritmaster" << SkipQuest_Spiritmaster;

            if(SkipQuest_Coach > 0)
            {
                QByteArray client_return1 = opholder.mid(21,1); //client return number think this is where??
                int  client_return_input = client_return1.toInt(nullptr,16);
                int dest_count = IN_Opcodes.destination.value(client_return_input);

                if(dest_count == 0 && client_return_input != 0xF)
                {
                    QByteArray coachout = IN_Opcodes.swapnames.value(dest_count); //get the number out of the map
                    QString ledger_name = IN_Opcodes.positionlist.value(coachout.toInt(nullptr,10)); //get the number in the record
                    QString coach_name = IN_Opcodes.positionlist.value(coachout.toInt(nullptr,10) + 1);

                    IN_Opcodes.ellisto.replace(coachout.toInt(nullptr,10),"0");
                    IN_Opcodes.ellisto.replace(coachout.toInt(nullptr,10) + 1,"1");

                    QVector<QVector<QString>> ChoachMainVector;

                    ChoachMainVector.append(IN_Opcodes.ellisto);
                    ChoachMainVector.append(IN_Opcodes.positionlist);

                    Opcodes::ID_VectorOfCoachesVectorMap.insert(IN_Opcodes.CharSelectID,ChoachMainVector);

                    int  indexfirst1 =  ledger_name.indexOf("\"",0);
                    int indexsecond1 = ledger_name.indexOf(")",0);
                    int posi_1 = indexsecond1 - indexfirst1;
                    QString coachname_out1 = ledger_name.mid(indexfirst1 + 1,posi_1);

                    qDebug() << "IN_Opcodes.positionlist" << IN_Opcodes.positionlist;
                    qDebug() << "coach_name" << coach_name;
                    qDebug() << "ledger_name" << ledger_name;
                    qDebug() << "coachout" << coachout;
                    qDebug() << "coachname_out1" << coachname_out1;

                    int  indexfirst2 =  coach_name.indexOf("\"",0);
                    int indexsecond2 = coach_name.indexOf(")",0);
                    int posi_2 = indexsecond2 - indexfirst2;
                    QString coachname_out2 = coach_name.mid(indexfirst2 + 1,posi_2);

                    db.transaction();

                    QString objid1 = IN_Opcodes.CharSelectID;
                    QString objtext = "ObjectID (4)";
                    QSqlQuery querya4;
                    querya4.prepare(QString("UPDATE COACHES SET [%1] = :val WHERE [%2] = :id").arg(coachname_out1).arg(objtext));
                    querya4.bindValue(":val",0); //this is ledger will always be zero
                    querya4.bindValue(":id",objid1);
                    querya4.exec();

                    QSqlQuery querya5;
                    querya5.prepare(QString("UPDATE COACHES SET [%1] = :val WHERE [%2] = :id").arg(coachname_out2).arg(objtext));
                    querya5.bindValue(":val",1); //this is the coach, will always be 1
                    querya5.bindValue(":id",objid1);
                    querya5.exec();

                    db.commit();

                    //send signed ledger text here

                    Coaching reply2;
                    QByteArray signed_coach1 =  reply2.leaving(IN_Opcodes.coachtext1,0);

                    if(signed_coach1 != "NULL" && SkipQuest_Coach != 3)
                    {
                        increment leave3; //get class first opcode
                        QByteArray leavecoach4 = leave3.count(IN_Opcodes.coachreply_counter); //first opcode increment
                        signed_coach1.replace(4,4,leavecoach4); //replace new count at 54
                        IN_Opcodes.coachreply_counter = leavecoach4.toUpper(); //replace new  count

                        IN_Opcodes.SendFBsVector.append(signed_coach1);
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Coach_Option1, ");
                    }
                }

                if(dest_count != 0 && client_return_input != 0xF)
                {
                    Coaching reply3;
                    QByteArray signed_coach2 =  reply3.leaving(IN_Opcodes.coachtext1,5);

                    if(signed_coach2 != "NULL" && SkipQuest_Coach != 3)
                    {
                        increment leave8; //get class first opcode
                        QByteArray leavecoach10 = leave8.count(IN_Opcodes.coachreply_counter); //first opcode increment
                        signed_coach2.replace(4,4,leavecoach10); //replace new count at 54
                        IN_Opcodes.coachreply_counter = leavecoach10.toUpper(); //replace new  count

                        IN_Opcodes.SendFBsVector.append(signed_coach2);
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Coach_Option2, ");

                        IN_Opcodes.coachtext1 = ""; //clear the id result out

                        IN_Opcodes.CoachTimer = "1000";

                        QByteArray teleportcoachout2 = IN_Opcodes.swapnames.value(dest_count);
                        IN_Opcodes.teleport_destination = coach_destination_map.value(teleportcoachout2.toInt(nullptr,10));

                        qDebug() << "IN_Opcodes.teleport_destination" << IN_Opcodes.teleport_destination;

                        int oggokport1 = teleportcoachout2.toInt(nullptr,10);

                        IN_Opcodes.LastTeleportWorld = IN_Opcodes.masterworld;

                        if(oggokport1 == 52)
                        {
                            IN_Opcodes.masterworld = 01;
                        }
                        else
                        {
                            IN_Opcodes.masterworld = 00;
                        }
                        //coachtimer.start(1000); //1 second timer for end packet
                    }
                }
            }

            if(SkipQuest_Coach == 3 || SkipQuest_Coach == 0 || SkipQuest_Spiritmaster > 0)
            {
                IN_Opcodes.Quest = false;

                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                qDebug() << "IN_Opcodes.CurrentQuestMenuID_3" << IN_Opcodes.CurrentQuestMenuID;

                QString QuestsID = "";

                if((SkipQuest_Coach == 0 && SkipQuest_Spiritmaster == 0) || SkipQuest_Coach == 3 || SkipQuest_Spiritmaster == 4)
                {
                    QuestsID = IN_Opcodes.CurrentQuestMenuID;
                }

                if(SkipQuest_Spiritmaster > 0 && SkipQuest_Spiritmaster != 4)
                {
                    QuestsID = "SM_" + NPCsID;
                }

                qDebug() << "IN_Opcodes.QuestsID" << QuestsID;

                Quests sendto;
                sendto.QuestMenus(ID_IP_PORT,Clients_Last_MessageNumber,Clients_Last_FB_MessageNumber,opholder,QuestsID,NPCsID);

                IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                qDebug() << "IN_Opcodes.CurrentQuestMenuID_4" << IN_Opcodes.CurrentQuestMenuID;
            }
            else
            {
                QByteArray holderReply = "d37c5ae795c301bf543213" + IN_Opcodes.SessionID + "03xxxxXXXXxxxx";

                holderReply.replace(0,4,IN_Opcodes.serverid1); //swap IDs
                holderReply.replace(4,4,IN_Opcodes.clientid1); //swap IDs

                holderReply.replace(22,6,IN_Opcodes.SessionID);

                holderReply.replace(34,4,Clients_Last_MessageNumber);
                holderReply.replace(38,4,Clients_Last_FB_MessageNumber);

                QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                increment GrpOut;
                QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                holderReply.replace(30,4,GrpOut_a);
                Servers_Last_MessageNumber = GrpOut_a.toUpper();

                MessNumbVect.replace(0,Servers_Last_MessageNumber);

                CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                packetsize ResizePacket;
                QByteArray ResizePacket2 = ResizePacket.switchsize(holderReply);
                holderReply.replace(8,4,ResizePacket2);

                IN_Opcodes.OpcodeReturn.append(holderReply);
                IN_Opcodes.OpcodeReturn.append("Dupe_opholder ");
                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
            }

        }

        ///////////////////////////////////////////////////3A00 Spell Book Moves//////////////////////////////////////////////////////////////////////////////////
        if(IN_Opcodes.opcoderesult1 == "3A00")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            //FB08 2900 3A00 00000000 0305
            QByteArray Spell_arr = opholder.mid(20,2);
            QByteArray New_Slot = opholder.mid(22,2);
            IN_Opcodes.spell_id = Spell_arr.toInt(nullptr,16);
            IN_Opcodes.spell_bookslot = New_Slot.toInt(nullptr,16);
            ////////////////////////////////////SLOT //////////////////////////////////////////
            for(int yy = 0; yy < IN_Opcodes.spellslot_map.size(); yy++) //create history of which spell is currently in which slot do this before sorting
            {
                int temp_spell1 = IN_Opcodes.spellslot_map.value(yy); //get spell ids
                QString new_spell_res = IN_Opcodes.spellbook_map.value(temp_spell1); //get the new id (position)
                IN_Opcodes.spellslot_copy.insert(yy,new_spell_res); //this is slot, spell string

            }

            ////////////////////////////////////////////Arranging

            int result_sp = IN_Opcodes.spell_id - IN_Opcodes.spell_bookslot; //sub

            if(result_sp < 0) //going down
            {

                QString row1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_id); //get spell at position
                IN_Opcodes.spellbook_copy.insert(IN_Opcodes.spell_bookslot,row1);

                for(int i = IN_Opcodes.spell_bookslot; i > -1; i--)
                {
                    if(i == IN_Opcodes.spell_id)
                    {

                        QString row5 = IN_Opcodes.spellbook_map.value(i - 1);
                        IN_Opcodes.switch2 = true;
                        IN_Opcodes.spellbook_copy.insert(i - 1,row5);
                        i--;
                    }

                    if(IN_Opcodes.switch2 == true)
                    {
                        QString row1 = IN_Opcodes.spellbook_map.value(i);
                        IN_Opcodes.spellbook_copy.insert(i,row1);
                    }
                    else
                    {
                        QString row1 = IN_Opcodes.spellbook_map.value(i);
                        IN_Opcodes.spellbook_copy.insert(i - 1,row1);
                    }
                }

                IN_Opcodes.switch2 = false;

                for(int xx = IN_Opcodes.spell_bookslot + 1; xx < IN_Opcodes.spellbook_map.size(); xx++) //complete the map copy
                {
                    QString frr = IN_Opcodes.spellbook_map.value(xx);
                    IN_Opcodes.spellbook_copy.insert(xx,frr);
                }

                IN_Opcodes.spellbook_map.clear();
                IN_Opcodes.spellbook_map = IN_Opcodes.spellbook_copy; //transfer our new map
                IN_Opcodes.spellbook_copy.clear(); //clear this we are done with it
            }
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            else
            {
                for(int i = IN_Opcodes.spell_id; i > -1; i--)
                {
                    if(i == IN_Opcodes.spell_bookslot)
                    {
                        QString row1 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_id);
                        IN_Opcodes.spellbook_copy.insert(i,row1);
                        IN_Opcodes.switch1 = true;
                        QString row2 = IN_Opcodes.spellbook_map.value(IN_Opcodes.spell_bookslot);
                        IN_Opcodes.spellbook_copy.insert(i - 1,row2);
                    }
                    else
                    {
                        if(IN_Opcodes.switch1 == true)
                        {
                            QString row1 = IN_Opcodes.spellbook_map.value(i);
                            IN_Opcodes.spellbook_copy.insert(i,row1);
                        }
                        else
                        {
                            QString row1 = IN_Opcodes.spellbook_map.value(i - 1);
                            IN_Opcodes.spellbook_copy.insert(i,row1);
                        }
                    }
                }

                IN_Opcodes.switch1 = false;

                for(int xx = IN_Opcodes.spell_id + 1; xx < IN_Opcodes.spellbook_map.size(); xx++) //complete the map copy
                {
                    QString frr = IN_Opcodes.spellbook_map.value(xx);
                    IN_Opcodes.spellbook_copy.insert(xx,frr);
                }

                IN_Opcodes.spellbook_map.clear();
                IN_Opcodes.spellbook_map = IN_Opcodes.spellbook_copy; //transfer our new map
                IN_Opcodes.spellbook_copy.clear(); //clear this we are done with it
            }

            ///////////////////////////////////////////Slot ///////////////////////////////////////
            for(int zz = 0; zz < IN_Opcodes.spellslot_copy.size(); zz++)
            {
                QString new_spell_res2 = IN_Opcodes.spellslot_copy.value(zz); //get spell description
                int new_val_spell = IN_Opcodes.spellbook_map.key(new_spell_res2); //get the new position since we rearranged the book ids
                IN_Opcodes.spellslot_map.insert(zz,new_val_spell); //insert new spell id for the spell description
            }

            IN_Opcodes.spellslot_copy.clear();

            QByteArray Inc_Arrange = opholder.mid(12,4);
            QByteArray Arrange_Spells = "B600010000000102";

            Arrange_Spells.replace(12,2,Spell_arr);
            Arrange_Spells.replace(14,2,New_Slot);

            IN_Opcodes.SendFBsVector.append(Arrange_Spells);
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("Arrange_Spells, ");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }



        if(IN_Opcodes.opcoderesult1 == "5512")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray Currency = opholder.mid(22,-1);

            QByteArray Deposit_Withdraw = opholder.mid(20,2);

            ////////////////////////////////////////////////////////////////////////

            int StartingBalance = 0;
            int WithdrawAmt = 0;
            int NewBalance = 0;
            int StartingWallet = 0;
            int NewAmtInWallet = 0;

            QString BackToDec = "";
            ADD_OR_SUB ConvertBack;
            ADD_OR_SUB AddtoPocket;
            Convert Send;

            if(Deposit_Withdraw == "01")
            {
                //Withdraw tunar from bank
                BackToDec = ConvertBack.BackToDec(IN_Opcodes.Current_Available_balance);
                StartingBalance = BackToDec.toInt(nullptr,10);

                BackToDec = ConvertBack.BackToDec(Currency);
                WithdrawAmt = BackToDec.toInt(nullptr,10);

                NewBalance = StartingBalance - WithdrawAmt;

                QString NewBalance1 = QString("%1").arg(NewBalance,2,10,QLatin1Char('0'));

                IN_Opcodes.Current_Available_balance = Send.ConvertTo(NewBalance1);


                QString FBsizeOpcode = "5312" + IN_Opcodes.Current_Available_balance.toUtf8();

                IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Update_Tunar_in_Bank, ");

                //Add tunar to wallet
                BackToDec = AddtoPocket.BackToDec(IN_Opcodes.Cash_on_hand);
                StartingWallet = BackToDec.toInt(nullptr,10);

                NewAmtInWallet = StartingWallet + WithdrawAmt;

                QString NewAmtInWallet1 = QString("%1").arg(NewAmtInWallet,2,10,QLatin1Char('0'));

                IN_Opcodes.Cash_on_hand = Send.ConvertTo(NewAmtInWallet1);

                FBsizeOpcode = "5200" + IN_Opcodes.Cash_on_hand.toUtf8();

                IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Update_Tunar_on_Toon, ");
            }


            if(Deposit_Withdraw == "00")
            {
                //ADD tunar to bank
                BackToDec = ConvertBack.BackToDec(IN_Opcodes.Cash_on_hand);
                StartingBalance = BackToDec.toInt(nullptr,10);

                BackToDec = ConvertBack.BackToDec(Currency);
                WithdrawAmt = BackToDec.toInt(nullptr,10);

                NewBalance = StartingBalance - WithdrawAmt;

                QString NewBalance1 = QString("%1").arg(NewBalance,2,10,QLatin1Char('0'));

                IN_Opcodes.Cash_on_hand = Send.ConvertTo(NewBalance1);

                QString FBsizeOpcode = "5200" + IN_Opcodes.Cash_on_hand;

                IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Update_Tunar_on_Toon, ");


                //sub tunar from wallet
                BackToDec = AddtoPocket.BackToDec(IN_Opcodes.Current_Available_balance);
                StartingWallet = BackToDec.toInt(nullptr,10);

                NewAmtInWallet = StartingWallet + WithdrawAmt;

                QString NewAmtInWallet1 = QString("%1").arg(NewAmtInWallet,2,10,QLatin1Char('0'));

                IN_Opcodes.Current_Available_balance = Send.ConvertTo(NewAmtInWallet1);

                FBsizeOpcode = "5312" + IN_Opcodes.Current_Available_balance.toUtf8();

                IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Update_Tunar_in_Bank, ");


            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }


        // FB0D0F000512080204F6FD940DF8FD940D
        // FB06 0F00 4D12 90E70000

        //            if(IN_Opcodes.opcoderesult1 == "0512")// not working yet
        //            {
        //                QByteArray auction1 = opholder.mid(14,2);

        //                QByteArray Holder_My_Auctions = "d37c5ae7a0c101bf543213" + IN_Opcodes.SessionID + "03280022000600FB0D0F000512080204F6FD940DF8FD940D";

        //                packetsize ResizePacket; //call size class
        //                QByteArray ResizePacket2 = ResizePacket.switchsize(Holder_My_Auctions); //get new size
        //                Holder_My_Auctions.replace(8,4,ResizePacket2);

        //                Holder_My_Auctions.replace(0,4,IN_Opcodes.serverid1);
        //                Holder_My_Auctions.replace(4,4,IN_Opcodes.clientid1);
        // holder2591.replace(22,6,IN_Opcodes.SessionID);
        //                Holder_My_Auctions.replace(34,4,Clients_Last_MessageNumber);
        //                Holder_My_Auctions.replace(38,4,Clients_Last_FB_MessageNumber);
        //                Holder_My_Auctions.replace(56,2,auction1);


        //                increment My_Auctions;
        //                QByteArray My_Auctions2 = My_Auctions.count(IN_Opcodes.Servers_Last_MessageNumber); //packet increment //packet increment
        //                Holder_My_Auctions.replace(30,4,My_Auctions2); //replace new count at 30
        //                IN_Opcodes.Servers_Last_MessageNumber = My_Auctions2.toUpper(); //replace new count
        //                ui->lineEdit_5->setText(My_Auctions2.toUpper());

        //                My_Auctions2 = My_Auctions.count(IN_Opcodes.Servers_Last_FB_MessageNumber); //first opcode increment
        //                Holder_My_Auctions.replace(46,4,My_Auctions2); //replace new count at 46
        //                IN_Opcodes.Servers_Last_FB_MessageNumber = My_Auctions2.toUpper(); //replace new count
        //                ui->lineEdit_10->setText(My_Auctions2.toUpper());

        //                byte1 = Holder_My_Auctions;
        //                elcrc(byte1);
        //                Holder_My_Auctions.append(outcrc);

        //                qDebug() << "Holder_My_Auctions" << Holder_My_Auctions;

        //                QByteArray My_Auctions_out = Holder_My_Auctions.fromHex(Holder_My_Auctions);
        //                udpsocket3.writeDatagram(My_Auctions_out, sender3, senderPort3);

        //                QDateTime dateTimes_2 = dateTime.currentDateTime();
        //                QString dateTimeStrings_2 = dateTimes_2.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
        //                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        //                ui->plainTextEdit->appendPlainText("Server-Holder_My_Auctions- " + dateTimeStrings_2);
        //                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        //                ui->plainTextEdit->appendPlainText(Holder_My_Auctions.toUpper());
        //                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        //                ui->plainTextEdit->appendPlainText("Server-Holder_My_Auctions- " + dateTimeStrings_2);
        //                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        //                ui->plainTextEdit->moveCursor(QTextCursor::End);
        //            }


        if(IN_Opcodes.opcoderesult1 == "4D12")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray Holder_Bank = "4D1200";

            IN_Opcodes.SendFBsVector.append(Holder_Bank);
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("Open_Bank_Menu, ");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

        }





        if(IN_Opcodes.opcoderesult1 == "3D00")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray Old_Slot = opholder.mid(12,8);
            QByteArray New_Slot = opholder.mid(20,8);


            int Old_Slot2 = IN_Opcodes.Inventory_Vector.indexOf(Old_Slot);
            int New_Slot2 = IN_Opcodes.Inventory_Vector.indexOf(New_Slot);


            QString take_item = IN_Opcodes.Inventory_Vector.takeAt(Old_Slot2);
            IN_Opcodes.Inventory_Vector.insert(New_Slot2,take_item.toUpper());

            take_item = IN_Opcodes.InventorySlots_ItemIDs.takeAt(Old_Slot2);
            IN_Opcodes.InventorySlots_ItemIDs.insert(New_Slot2,take_item.toUpper());

            take_item = IN_Opcodes.InventoryItem_Quantity.takeAt(Old_Slot2);
            IN_Opcodes.InventoryItem_Quantity.insert(New_Slot2,take_item.toUpper());

            take_item = IN_Opcodes.InvItem_Type.takeAt(Old_Slot2);
            IN_Opcodes.InvItem_Type.insert(New_Slot2,take_item);

            take_item = IN_Opcodes.ItemIsEquippedVector.takeAt(Old_Slot2);
            IN_Opcodes.ItemIsEquippedVector.insert(New_Slot2,take_item);

            take_item = IN_Opcodes.EQuipSots_InvSlots.takeAt(Old_Slot2);
            IN_Opcodes.EQuipSots_InvSlots.insert(New_Slot2,take_item);


            QString Old_Slot3 = QString("%1").arg(Old_Slot2,2,16,QLatin1Char('0')).toUpper();
            QString New_Slot3 = QString("%1").arg(New_Slot2,2,16,QLatin1Char('0')).toUpper();

            QByteArray Arrange_Items = "3D000406";

            Arrange_Items.replace(04,2,Old_Slot3.toUtf8().toUpper());
            Arrange_Items.replace(06,2,New_Slot3.toUtf8().toUpper());

            IN_Opcodes.SendFBsVector.append(Arrange_Items);
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("Arrange_Items, ");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }

        //FB08 0900 4A00 00 02   36D40C00 //buy 01/02   items in slot 00/00
        //FB08 0D00 4A00 06 02   36D40C00 //buy 01/02   items in slot 03/06
        //FB09 1200 4A00 36 C601 36D40C00 //buy 99/c601 items in slot 1B/36

        if(IN_Opcodes.opcoderesult1 == "4A00")//buy
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray Item_Slot = opholder.mid(12,2);

            //FB0807004A00060235D40C00

            int Item_Slot1 = Item_Slot.toInt(nullptr,16);
            Item_Slot1 = Item_Slot1/2;
            QString ItemsID = IN_Opcodes.merchant_vector.at(Item_Slot1);
            QString Item_ID2 = ItemsID;

            QVector <QString> ItemsFields = merchants::MasterItems_Vector_Map.value(Item_ID2);

            QString ItemsMaxSize = ItemsFields.at(23);
            QString CostPerItem = ItemsFields.at(10);
            QString ArmorColor = ItemsFields.at(0);
            QString WeaponGraphic = ItemsFields.at(1);
            QString ArmorGraphic = ItemsFields.at(2);
            QString Charges = ItemsFields.at(5);
            QString EQuipmentsType = ItemsFields.at(14);
            QString DoesItHaveCharges = "";

            if(Charges != "00")
            {
                DoesItHaveCharges = "YES";
            }
            else
            {
                DoesItHaveCharges = "NO";
            }

            if(ItemsMaxSize == "02")
            {
                NotStackable = "True";
            }
            else
            {
                NotStackable = "False";
            }

            ADD_OR_SUB ConvertBack;
            QString BackToDec = "";
            int QuantityLocation = 0;
            int InventoryID = 0;
            QString Inventoryidxxxxxxxx = "";
            QString CurrentInventoryQuantity = "";
            int SubTotal = 0;
            Convert Send;
            ADD_OR_SUB SendToText;
            QString FBText = "";
            QString TextFull = "";

            if(IN_Opcodes.InventorySlots_ItemIDs.contains(Item_ID2))
            {
                if(NotStackable == "False") // if it is a stack and it is in the inventory already
                {
                    NewItem = "False";
                    QuantityLocation = IN_Opcodes.InventorySlots_ItemIDs.indexOf(Item_ID2);
                    Inventoryidxxxxxxxx = IN_Opcodes.Inventory_Vector.at(QuantityLocation);
                    CurrentInventoryQuantity = IN_Opcodes.InventoryItem_Quantity.at(QuantityLocation);
                    qDebug() << "item is in here ";
                }
                else // if it is not a stack and there is one in the inventory already
                {    // needs to be treated as a new item

                    CurrentInventoryQuantity = "00";
                    NewItem = "True";
                }
            }
            else // if it is not in the inventory
            {
                CurrentInventoryQuantity = "00";
                NewItem = "True";
            }

            qDebug() << "NotStackable" << NotStackable;
            qDebug() << "CurrentInventoryQuantity" << CurrentInventoryQuantity;
            qDebug() << "NewItem" << NewItem;

            QString Item_Quantity1 = opholder.mid(14,2);
            QString Item_Quantity2 = opholder.mid(16,2);
            QString Item_Quantity3 = opholder.mid(18,2);
            int Item_Quantity4 = Item_Quantity1.toInt(nullptr,16);
            int Item_Quantity5 = Item_Quantity2.toInt(nullptr,16);

            QString Item_Quantity6 = "";

            if(Item_Quantity4 >= 0x80)
            {
                if(Item_Quantity5 >= 0x80)
                {
                    Item_Quantity6 = Item_Quantity1 + Item_Quantity2 + Item_Quantity3;
                }
                else
                {
                    Item_Quantity6 = Item_Quantity1 + Item_Quantity2;
                }
            }
            else
            {
                Item_Quantity6 = Item_Quantity1;
            }

            BackToDec = ConvertBack.BackToDec(Item_Quantity6);
            int BuyQuantity = BackToDec.toInt(nullptr,10);
            BackToDec = ConvertBack.BackToDec(CurrentInventoryQuantity);
            int InvQuantity = BackToDec.toInt(nullptr,10);
            BackToDec = ConvertBack.BackToDec(ItemsMaxSize);
            int ItemsMaxSize2 = BackToDec.toInt(nullptr,10);
            BackToDec = ConvertBack.BackToDec(CostPerItem);
            int CostPerItem2 = BackToDec.toInt(nullptr,10);
            BackToDec = ConvertBack.BackToDec(IN_Opcodes.Cash_on_hand);
            int Cash_on_hand1 = BackToDec.toInt(nullptr,10);
            int total = InvQuantity + BuyQuantity;

            if(total <= ItemsMaxSize2 || NotStackable == "True")//if the stack can fit or its not stackable
            {
                // //// do you have enough tunar on you. //////

                if(SubTotal > Cash_on_hand1)
                {
                    CantAffordIt = "True"; // if you cant afford what you picked
                }
                else// if you cant afford it you dont want this part to go off
                {
                    CantAffordIt = "False";// if you can afford what you picked
                    ItemsFull = "False"; // false = the item is not full

                    Buy_ItemTotal = QString("%1").arg(BuyQuantity,2,10,QLatin1Char('0')).toUpper();
                    Buy_ItemTotal = Send.ConvertTo(Buy_ItemTotal);

                    QString Buy_Item2 = QString("%1").arg(total,2,10,QLatin1Char('0')).toUpper();

                    ConvertedValue = Send.ConvertTo(Buy_Item2);
                }
            }



            // if the stack amount you picked does not fit
            if(total > ItemsMaxSize2)// if we try to buy more than we can fit
            {
                int total2 = total - ItemsMaxSize2;// how much we cant fit
                int total3 = BuyQuantity - total2;// subtract what we cant fit

                if(total3 > 0)//if what we can fit is less then the max stack size
                {
                    ////// do you have enough tunar on you. //////

                    SubTotal = total3 * CostPerItem2;

                    if(SubTotal > Cash_on_hand1)
                    {
                        CantAffordIt = "True"; // if you cant afford how many will fit
                    }
                    else
                    {
                        CantAffordIt = "False"; // if you can afford how many will fit

                        // if you can fit more than 0 // we can probably make it do this part only once
                        ItemsFull = "False";//then Item is not full

                        Buy_ItemTotal = QString("%1").arg(total3,2,10,QLatin1Char('0')).toUpper();
                        Buy_ItemTotal = Send.ConvertTo(Buy_ItemTotal);

                        QString Buy_Item2 = QString("%1").arg(total3 + InvQuantity,2,10,QLatin1Char('0')).toUpper();

                        ConvertedValue = Send.ConvertTo(Buy_Item2);
                    }
                }
                else
                {
                    ItemsFull = "True";
                    qDebug() << "ItemsFull4 =" << ItemsFull;
                }
            }

            qDebug() << "CantAffordIt" << CantAffordIt;
            qDebug() << "ItemsFull" << ItemsFull;

            qDebug() << "IN_Opcodes.InventoryMAX" << IN_Opcodes.InventoryMAX;

            if(
                    (IN_Opcodes.InventoryMAX < 40 && ItemsFull == "False" && CantAffordIt == "False") ||
                    (IN_Opcodes.InventoryMAX == 40 && ItemsFull == "False" && NotStackable == "False"
                     && NewItem == "False" && CantAffordIt == "False"))
            {
                Cash_on_hand1 = Cash_on_hand1 - SubTotal;

                QString Cash_on_hand2 = QString("%1").arg(Cash_on_hand1,2,10,QLatin1Char('0')).toUpper();
                IN_Opcodes.Cash_on_hand = Send.ConvertTo(Cash_on_hand2);

                QString FBsizeOpcode = "5200" + IN_Opcodes.Cash_on_hand.toUtf8();

                IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Update_Tunar_on_Toon, ");

                QString Buy_Item = merchants::Items_Map.value(ItemsID);

                Buy_Item.replace(0,2,Buy_ItemTotal.toUtf8().toUpper());//change the number of items to what we are buying

                if(NewItem == "True")
                {
                    InventoryID = Buy_Item.indexOf("XXXXXXXX");
                    increment Buy;
                    QByteArray Buy2 = Buy.count(IN_Opcodes.InventoryIDCounter); //InventoryID increment
                    Buy_Item.replace(InventoryID,8,IN_Opcodes.InventoryIDCounter + "0000");
                    IN_Opcodes.InventoryIDCounter = Buy2.toUpper();

                    IN_Opcodes.InventorySlots_ItemIDs.append(ItemsID);
                    IN_Opcodes.Inventory_Vector.append(IN_Opcodes.InventoryIDCounter + "0000");

                    IN_Opcodes.InventoryMAX = IN_Opcodes.InventoryMAX + 1;

                    IN_Opcodes.ItemIsEquippedVector.append("NO");
                    IN_Opcodes.EQuipSots_InvSlots.append("XX");

                    if(DoesItHaveCharges == "NO")
                    {
                        IN_Opcodes.InventoryItem_Quantity.append(ConvertedValue);
                        qDebug() << "IN_Opcodes.InventoryItem_Quantity =" << IN_Opcodes.InventoryItem_Quantity;
                    }
                    else
                    {
                        IN_Opcodes.InventoryItem_Quantity.append(Charges);
                        qDebug() << "IN_Opcodes.InventoryItem_Quantity =" << IN_Opcodes.InventoryItem_Quantity;
                    }



                    if(EQuipmentsType == "01")
                    {
                        IN_Opcodes.InvItem_Type.append("Other");
                    }
                    else
                    {
                        IN_Opcodes.InvItem_Type.append("EQuipment");
                    }
                }

                if(NotStackable == "False" && NewItem == "False")
                {
                    InventoryID = Buy_Item.indexOf("XXXXXXXX");
                    Buy_Item.replace(InventoryID,8,Inventoryidxxxxxxxx.toUtf8().toUpper());
                    IN_Opcodes.InventoryItem_Quantity.replace(QuantityLocation,ConvertedValue);
                }

                FBsizeOpcode = "3B00" + Buy_Item; // Add opcode

                IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Buy_Item1, ");
            }
            else
            {
                // there is one option if you cant afford it
                // there are 3 ways your inventory can be full
                if(CantAffordIt == "False") // if you can afford it check the next condition or go to the else with the red text
                {
                    if(NotStackable == "False") // if you can afford it and it is a stack item check the next condition or go to the else with red text
                    {
                        if(NewItem == "False") // if you can afford it and it is a stack item and it is not a new item send this red text or go to the else with the other red text
                        {
                            //this part is for invtory is full opcode
                            IN_Opcodes.SendFBsVector.append("D700");
                            IN_Opcodes.FBReplyOrNotVector.append("03");
                            IN_Opcodes.FBPacketDescriptionVector.append("Fake_Opcode, ");
                            //this part is for invtory is full opcode


                            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(0);


                            TextFull = "Your inventory for that stack item is full.";
                            FBText =  SendToText.TextSize(ClientID,ClientsIP,ClientsPort,TextFull,true,"CE00",Servers_Last_FB_MessageNumber);

                            IN_Opcodes.SendFBsVector.append(FBText.toUtf8());
                            IN_Opcodes.FBReplyOrNotVector.append("03");
                            IN_Opcodes.FBPacketDescriptionVector.append("Inventory_Full, ");

                        }
                        else
                        {
                            //this part is for invtory is full opcode
                            IN_Opcodes.SendFBsVector.append("D700");
                            //this part is for invtory is full opcode
                            IN_Opcodes.FBReplyOrNotVector.append("03");
                            IN_Opcodes.FBPacketDescriptionVector.append("Fake_Opcode, ");

                            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(0);

                            TextFull = "Your inventory is full.";
                            FBText =  SendToText.TextSize(ClientID,ClientsIP,ClientsPort,TextFull,true,"CE00",Servers_Last_FB_MessageNumber);

                            IN_Opcodes.SendFBsVector.append(FBText.toUtf8());
                            IN_Opcodes.FBReplyOrNotVector.append("03");
                            IN_Opcodes.FBPacketDescriptionVector.append("Inventory_Full, ");
                        }
                    }
                    else
                    {
                        //this part is for invtory is full opcode
                        IN_Opcodes.SendFBsVector.append("D700");
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Fake_Opcode, ");
                        //this part is for invtory is full opcode

                        QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                        QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(0);

                        TextFull = "Your inventory is full.";
                        FBText =  SendToText.TextSize(ClientID,ClientsIP,ClientsPort,TextFull,true,"CE00",Servers_Last_FB_MessageNumber);

                        IN_Opcodes.SendFBsVector.append(FBText.toUtf8());
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Inventory_Full, ");
                    }
                }
                else
                {

                    //this part is for invtory is full opcode
                    IN_Opcodes.SendFBsVector.append("D700");
                    //this part is for invtory is full opcode
                    IN_Opcodes.FBReplyOrNotVector.append("03");
                    IN_Opcodes.FBPacketDescriptionVector.append("Fake_Opcode, ");


                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(0);

                    TextFull = "You can't afford that.";
                    FBText =  SendToText.TextSize(ClientID,ClientsIP,ClientsPort,TextFull,true,"CE00",Servers_Last_FB_MessageNumber);

                    IN_Opcodes.SendFBsVector.append(FBText.toUtf8());
                    IN_Opcodes.FBReplyOrNotVector.append("03");
                    IN_Opcodes.FBPacketDescriptionVector.append("Inventory_Full, ");

                }
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }



        //FB0B 1800 4B00 20000000 02 DCE90000
        //FB0B 1C00 4B00 38000000 3C DCE90000

        //fb05 xxxx 3c00 1900 06 // server

        if(IN_Opcodes.opcoderesult1 == "4B00")//sell items
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);


            QByteArray Item_Slot = opholder.mid(12,8);
            QByteArray Item_Slot2 = Item_Slot.mid(0,2);
            QByteArray Item_amt1 = opholder.mid(20,2);
            QByteArray Item_amt2 = opholder.mid(22,2);
            ADD_OR_SUB ConvertBack;
            Convert Send;
            QString BackToDec = "";
            QString ItemsID = "";
            QString Inventoryidxxxxxxxx = "";
            QString CurrentInventoryQuantity = "";
            QString Item_Quantity4 = "";

            int Item_Slot1 = Item_Slot2.toInt(nullptr,16);
            Item_Slot1 = Item_Slot1/2;
            qDebug() << "";
            qDebug() << "";
            qDebug() << "";
            qDebug() << "";

            int Inventoryid = IN_Opcodes.Inventory_Vector.indexOf(Item_Slot);
            ItemsID = IN_Opcodes.InventorySlots_ItemIDs.at(Inventoryid);
            CurrentInventoryQuantity = IN_Opcodes.InventoryItem_Quantity.at(Inventoryid);
            qDebug() << "IN_Opcodes.InventoryItem_Quantity =" << IN_Opcodes.InventoryItem_Quantity;

            QVector <QString> ItemsFields = merchants::MasterItems_Vector_Map.value(ItemsID);
            QString ItemsMaxSize = ItemsFields.at(23);
            QString ArmorColor = ItemsFields.at(0);
            QString WeaponGraphic = ItemsFields.at(1);
            QString ArmorGraphic = ItemsFields.at(2);
            QString Charges = ItemsFields.at(5);
            QString CostPerItem = ItemsFields.at(10);
            QString EQuipmentsType = ItemsFields.at(14);
            QString DoesItHaveCharges = "";

            if(Charges != "00")
            {
                DoesItHaveCharges = "YES";
                CurrentInventoryQuantity = "02";
            }
            else
            {
                DoesItHaveCharges = "NO";
            }

            qDebug() << "ItemsID" << ItemsID;
            QString Item_ID2 = ItemsID;

            int Item_Quantity3 = Item_amt1.toInt(nullptr,16);

            if(Item_Quantity3 >= 0x80)
            {
                Item_Quantity4 = Item_amt1 + Item_amt2;
            }
            else
            {
                Item_Quantity4 = Item_amt1;
            }


            BackToDec = ConvertBack.BackToDec(Item_Quantity4);
            int sellQuantity = BackToDec.toInt(nullptr,10);

            BackToDec = ConvertBack.BackToDec(CurrentInventoryQuantity);
            int InvQuantity = BackToDec.toInt(nullptr,10);

            int SellSubTotal = InvQuantity - sellQuantity;

            if(SellSubTotal == 0)
            {
                IN_Opcodes.InventorySlots_ItemIDs.removeAt(Inventoryid);
                IN_Opcodes.Inventory_Vector.removeAt(Inventoryid);
                IN_Opcodes.InventoryItem_Quantity.removeAt(Inventoryid);
                IN_Opcodes.InventoryMAX = IN_Opcodes.InventoryMAX - 1;

                QString IsItemEQupped = IN_Opcodes.ItemIsEquippedVector.at(Inventoryid);

                if(IsItemEQupped == "YES")
                {
                    QString ReplaceLocation;
                    int Position = 0;
                    int Length = 0;
                    QString ReplaceLocationC;
                    int PositionC = 0;
                    int LengthC = 0;
                    QString EQuipSlot = "";
                    QString Condition = "";
                    QString FreeUsed = "";

                    EQuipSlot = IN_Opcodes.EQuipSots_InvSlots.at(Inventoryid);

                    if(C9replaceGear.contains(EQuipSlot))
                    {
                        ReplaceLocation = C9replaceGear.value(EQuipSlot);
                        int index1 = C9GearLocations.indexOf(ReplaceLocation);

                        QString PositionS1 = C9GearLocations.at(index1 + 1);
                        QString LengthS1 = C9GearLocations.at(index1 + 2);
                        Position = PositionS1.toInt(nullptr,10);
                        Length = LengthS1.toInt(nullptr,10);

                        QString PositionS2 = C9GearLocationsC.at(index1 + 1);
                        QString LengthS2 = C9GearLocationsC.at(index1 + 2);
                        PositionC = PositionS2.toInt(nullptr,10);
                        LengthC = LengthS2.toInt(nullptr,10);
                    }

                    if(C9replaceWeapon.contains(EQuipSlot))
                    {
                        ReplaceLocation = C9replaceWeapon.value(EQuipSlot);
                        int index1 = C9WeaponLocations.indexOf(ReplaceLocation);

                        QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                        QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                        Position = PositionS1.toInt(nullptr,10);
                        Length = LengthS1.toInt(nullptr,10);
                    }

                    if(Position != 0)
                    {

                        QByteArray current_c9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8());
                        if(WeaponGraphic == "00000000")//this means it is Armor/Shields....
                        {
                            QString ArmorGraphic2 = "";

                            if(EQuipSlot == "15")// if its a robe
                            {
                                ArmorGraphic2 = "FF";
                            }
                            else
                            {
                                ArmorGraphic2 = "00";
                            }


                            // Armor position and length
                            current_c9.replace(Position,Length,ArmorGraphic2.toUtf8());

                            // Color position and length
                            current_c9.replace(PositionC,LengthC,"FFFFFFFF");

                            qDebug() << "ReplaceLocation3" << ReplaceLocation;
                            qDebug() << "ReplaceLocationC3" << ReplaceLocationC;
                        }

                        if(ArmorGraphic == "00" && EQuipSlot != "15")//this means it is Weapons/Held...
                        {
                            // Weapon position and length
                            current_c9.replace(Position,Length,"00000000");
                            qDebug() << "ReplaceLocation4" << ReplaceLocation;
                        }

                        generator::Players_Map.insert(IN_Opcodes.CharSelectID.toUtf8(),current_c9);

                        generator packet4;
                        QByteArray holdergen1 = packet4.maintoonswitch(IN_Opcodes.CharSelectID,"",IN_Opcodes.CharSelectID_Main);
                        QByteArray holdergen2 = "00C9030000" + holdergen1;

                        IN_Opcodes.SendZeroChannelVector.append(holdergen2);

                    }

                    QString ItemsSlot = IN_Opcodes.EQuipSots_InvSlots.at(Inventoryid);

                    IN_Opcodes.EquipmentSlotsUsed.remove(ItemsSlot);
                    IN_Opcodes.EquipmentSlotsUsed.insert(ItemsSlot,"FREE");
                }

                IN_Opcodes.ItemIsEquippedVector.removeAt(Inventoryid);
                IN_Opcodes.InvItem_Type.removeAt(Inventoryid);
                IN_Opcodes.EQuipSots_InvSlots.removeAt(Inventoryid);
            }
            else
            {
                QString Sell_Item2 = QString("%1").arg(SellSubTotal,2,10,QLatin1Char('0')).toUpper();
                Sell_Item2 = Send.ConvertTo(Sell_Item2);
                IN_Opcodes.InventoryItem_Quantity.replace(Inventoryid,Sell_Item2);
                qDebug() << "IN_Opcodes.InventoryItem_Quantity =" << IN_Opcodes.InventoryItem_Quantity;
            }



            BackToDec = ConvertBack.BackToDec(CostPerItem);
            int CostPerItem2 = BackToDec.toInt(nullptr,10);
            qDebug() << "int_CostPerItem2 " << CostPerItem2;

            BackToDec = ConvertBack.BackToDec(IN_Opcodes.Cash_on_hand);
            int Cash_on_hand1 = BackToDec.toInt(nullptr,10);

            int NewWalletValue = sellQuantity * CostPerItem2;
            qDebug() << "NewWalletValue" << NewWalletValue;

            Cash_on_hand1 = Cash_on_hand1 + NewWalletValue;
            QString Cash_on_hand2 = QString("%1").arg(Cash_on_hand1,2,10,QLatin1Char('0')).toUpper();
            IN_Opcodes.Cash_on_hand = Send.ConvertTo(Cash_on_hand2);



            QString FBsizeOpcode = "5200" + IN_Opcodes.Cash_on_hand.toUtf8();

            IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("Update_Tunar_on_Toon, ");

            qDebug() << "IN_Opcodes.Cash_on_hand =" << IN_Opcodes.Cash_on_hand;

            QString slotValue1 = QString("%1").arg(Inventoryid,2,16,QLatin1Char('0')).toUpper();

            FBsizeOpcode = "3C00" + slotValue1.toUtf8() + "01" + Item_Quantity4.toUtf8();

            IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("Sell_Item, ");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }

        // FB0B 3300 3E00 14000000 F3E90000 00
        // FB0B 3600 3E00 1B000000 F3E90000 00
        // FB0B 0801 3E00 00000000  00
        // FB0B 0F00 3E00 1C000000 DCE90000 00
        // FB04 xxxx 3e00 1c 09 // remove charges // server

        // Prepare
        // FB0F 1B01 2100 00 04000000 70006F0074007300

        if(IN_Opcodes.opcoderesult1 == "3E00")//EQuip/use
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray Item_Slot = opholder.mid(12,8);
            QByteArray Item_Slot2 = Item_Slot.mid(0,2);

            qDebug() << "";
            qDebug() << "";
            qDebug() << "";
            qDebug() << "";
            qDebug() << "opholder" << opholder;


            QByteArray current_c9 = "";
            ADD_OR_SUB ConvertBack;
            Convert Send;
            QString BackToDec = "";
            QString ItemsID = "";
            QString CurrentInventoryQuantity = "";
            QString Items_Type = "";
            QString IsItemEquipped = "";
            bool DoesItHaveCharges = false;
            QString ReplaceLocation = "";
            int Position = 0;
            int Length = 0;
            QString ReplaceLocationC = "";
            int PositionC = 0;
            int LengthC = 0;
            QString EQuipSlot = "";
            QString FreeUsed = "";
            QString FreeUsed1 = "";
            int index1 = 0;
            QString LevelStatusIs = "";
            QString ClassStatusIs = "";
            QString RaceStatusIs = "";
            QString FBsizeOpcode = "";

            int Item_Slot1 = Item_Slot2.toInt(nullptr,16);
            Item_Slot1 = Item_Slot1/2;

            int Inventoryid = IN_Opcodes.Inventory_Vector.indexOf(Item_Slot);
            ItemsID = IN_Opcodes.InventorySlots_ItemIDs.at(Inventoryid);
            CurrentInventoryQuantity = IN_Opcodes.InventoryItem_Quantity.at(Inventoryid);
            Items_Type = IN_Opcodes.InvItem_Type.at(Inventoryid);
            IsItemEquipped = IN_Opcodes.ItemIsEquippedVector.at(Inventoryid);

            QString Item_ID2 = ItemsID;

            QVector <QString> ItemsFields = merchants::MasterItems_Vector_Map.value(Item_ID2);
            QString ArmorColor = ItemsFields.at(0);
            QString WeaponGraphic = ItemsFields.at(1);
            QString ArmorGraphic = ItemsFields.at(2);
            QString Charges = ItemsFields.at(5);
            QString EQuipmentsType = ItemsFields.at(14);
            QString ItemsDMG = ItemsFields.at(21);

            if(Charges == "00")
            {
                DoesItHaveCharges = false;
            }
            else
            {
                DoesItHaveCharges = true;
            }

            current_c9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8());

            // get levels /////////////////////////////////////
            QString ItemsLevel1 = ItemsFields.at(22);

            BackToDec = ConvertBack.BackToDec(ItemsLevel1);
            int ItemsLevel = BackToDec.toInt(nullptr,10);


            QString PlayersLevel1 = current_c9.mid(358,2);
            int PlayersLevel2 = PlayersLevel1.toInt(nullptr,16);


            if(PlayersLevel2 >= ItemsLevel)
            {
                LevelStatusIs = "GOOD";
            }
            else
            {
                LevelStatusIs = "BAD";
            }

            qDebug() << "LevelStatusIs" << LevelStatusIs;


            QString ItemsClass = ItemsFields.at(26);

            qDebug() << "ItemsClass" << ItemsClass;

            QVector <QString> ClassVector = objectpacket::MonsterClassVectorMap.value(ItemsClass);

            qDebug() << "ClassVector" << ClassVector;

            QString PlayersClass = current_c9.mid(368,2);

            qDebug() << "PlayersClass" << PlayersClass;

            QString ClassAbreviation = objectpacket::ClassValueToString.value(PlayersClass);

            qDebug() << "ClassAbreviation" << ClassAbreviation;

            if(ClassVector.contains(ClassAbreviation))
            {
                ClassStatusIs = "GOOD";
            }
            else
            {
                ClassStatusIs = "BAD";
            }



            QString ItemsRace = ItemsFields.at(27);

            qDebug() << "ItemsRace" << ItemsRace;

            QVector <QString> RaceVector = objectpacket::MonsterRaceVectorMap.value(ItemsRace);

            qDebug() << "RaceVector" << RaceVector;

            QString PlayersRace = current_c9.mid(366,2);

            qDebug() << "PlayersRace" << PlayersRace;

            QString RaceAbreviation = objectpacket::RaceValueToString.value(PlayersRace);

            qDebug() << "RaceAbreviation" << RaceAbreviation;

            if(RaceVector.contains(RaceAbreviation))
            {
                RaceStatusIs = "GOOD";
            }
            else
            {
                RaceStatusIs = "BAD";
            }



            if(LevelStatusIs == "GOOD" && ClassStatusIs == "GOOD" && RaceStatusIs == "GOOD")
            {
                QString ATT_Type1 = ItemsFields.at(19);

                if(ATT_Type1 == "10")
                {
                    ATT_Type1 = "12";
                }

                int ATT_Type2 = ATT_Type1.toInt(nullptr,16);
                int ATT_Type3 = ATT_Type2 / 2;
                QString ATT_Type4 = QString("%1").arg(ATT_Type3,2,16,QLatin1Char('0')).toUpper();

                IN_Opcodes.MySwingType = ClassSwingCountMap.value(PlayersClass);

                if(IN_Opcodes.MySwingType > 1 )
                {
                    if((IN_Opcodes.MySwingType == 4 && PlayersLevel2 >= 20 && PlayersLevel2 < 35))//too low to quad
                    {
                        IN_Opcodes.MySwingType = 2;//downgrade to 2 hits
                    }


                    if(PlayersLevel2 < 20)// downgrade to 1 hit if less than level 20
                    {
                        IN_Opcodes.MySwingType = 1;
                    }
                }


                if(C9replaceWeaponEQ.contains(EQuipmentsType))
                {
                    EQuipSlot = EQuipTypeToSlot.value(EQuipmentsType);

                    ReplaceLocation = C9replaceWeaponEQ.value(EQuipmentsType);

                    index1 = C9WeaponLocations.indexOf(ReplaceLocation);

                    QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                    QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                    Position = PositionS1.toInt(nullptr,10);
                    Length = LengthS1.toInt(nullptr,10);

                    if(EQuipmentsType == "1C")//secondary as primary if empty
                    {

                        FreeUsed = IN_Opcodes.EquipmentSlotsUsed.value("0E");//primary weapon

                        if(IN_Opcodes.MySwingType == 1 || (IN_Opcodes.MySwingType == 2 && PlayersLevel2 < 20))
                        {
                            EQuipSlot = "0E";
                            ReplaceLocation = "Primary";
                            index1 = C9WeaponLocations.indexOf(ReplaceLocation);
                            QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                            QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                            Position = PositionS1.toInt(nullptr,10);
                            Length = LengthS1.toInt(nullptr,10);
                        }
                        else
                        {
                            if(FreeUsed == "FREE")
                            {
                                EQuipSlot = "0E";
                                ReplaceLocation = "Primary";
                                index1 = C9WeaponLocations.indexOf(ReplaceLocation);
                                QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                                QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                                Position = PositionS1.toInt(nullptr,10);
                                Length = LengthS1.toInt(nullptr,10);
                            }
                            else
                            {
                                EQuipSlot = "0F";
                                ReplaceLocation = "Secondary";
                                index1 = C9WeaponLocations.indexOf(ReplaceLocation);
                                QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                                QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                                Position = PositionS1.toInt(nullptr,10);
                                Length = LengthS1.toInt(nullptr,10);
                            }
                        }
                    }

                    if(EQuipmentsType == "24")//secondary held as primary held if empty
                    {
                        FreeUsed = IN_Opcodes.EquipmentSlotsUsed.value("0E");
                        FreeUsed1 = IN_Opcodes.EquipmentSlotsUsed.value("13");

                        if(FreeUsed == "FREE" && FreeUsed1 == "FREE")
                        {
                            EQuipSlot = "13";
                            ReplaceLocation = "Primary";
                            index1 = C9WeaponLocations.indexOf(ReplaceLocation);
                            QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                            QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                            Position = PositionS1.toInt(nullptr,10);
                            Length = LengthS1.toInt(nullptr,10);
                        }
                        else
                        {
                            EQuipSlot = "14";
                            ReplaceLocation = "Secondary";
                            index1 = C9WeaponLocations.indexOf(ReplaceLocation);
                            QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                            QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                            Position = PositionS1.toInt(nullptr,10);
                            Length = LengthS1.toInt(nullptr,10);
                        }
                    }
                }
                else
                {
                    if(EQuipmentsType == "08")//Neck
                    {
                        EQuipSlot = "04";
                    }

                    if(EQuipmentsType == "12")//Waist
                    {
                        EQuipSlot = "0B";
                    }

                    if(EQuipmentsType == "06")//Right Ear, Left Ear
                    {
                        EQuipSlot = "02";
                        FreeUsed = IN_Opcodes.EquipmentSlotsUsed.value(EQuipSlot);

                        if(FreeUsed == "USED")
                        {
                            EQuipSlot = "03";
                        }
                    }

                    if(EQuipmentsType == "0C")//Right Forearm, Left Forearm
                    {
                        EQuipSlot = "06";
                        FreeUsed = IN_Opcodes.EquipmentSlotsUsed.value(EQuipSlot);

                        if(FreeUsed == "USED")
                        {
                            EQuipSlot = "07";
                        }
                    }

                    if(EQuipmentsType == "10")//Right Ring, Left Ring
                    {
                        EQuipSlot = "09";
                        FreeUsed = IN_Opcodes.EquipmentSlotsUsed.value(EQuipSlot);

                        if(FreeUsed == "USED")
                        {
                            EQuipSlot = "0A";
                        }
                    }
                }

                if(C9replaceArmor.contains(EQuipmentsType))
                {
                    EQuipSlot = EQuipTypeToSlot.value(EQuipmentsType);

                    ReplaceLocation = C9replaceArmor.value(EQuipmentsType);
                    ReplaceLocationC = C9replaceColor.value(EQuipmentsType);

                    // Armor position and length
                    index1 = C9GearLocations.indexOf(ReplaceLocation);

                    QString PositionS1 = C9GearLocations.at(index1 + 1);
                    QString LengthS1 = C9GearLocations.at(index1 + 2);
                    Position = PositionS1.toInt(nullptr,10);
                    Length = LengthS1.toInt(nullptr,10);

                    QString PositionS2 = C9GearLocationsC.at(index1 + 1);
                    QString LengthS2 = C9GearLocationsC.at(index1 + 2);
                    PositionC = PositionS2.toInt(nullptr,10);
                    LengthC = LengthS2.toInt(nullptr,10);
                }
                else
                {
                    //                        EQuipSlot = EQuipTypeToSlot.value(EQuipmentsType);
                    //                        qDebug() << "EQuipSlot1" << EQuipSlot;
                    //                        qDebug() << "Somthing went wrong";
                }



                if(Items_Type == "EQuipment")
                {
                    if(IsItemEquipped == "NO")// EQuipt Item
                    {
                        QString isItFree = IN_Opcodes.EquipmentSlotsUsed.value(EQuipSlot);
                        int OldID = IN_Opcodes.EQuipSots_InvSlots.indexOf(EQuipSlot);

                        if(isItFree == "USED")
                        {
                            IN_Opcodes.ItemIsEquippedVector.replace(OldID,"NO");
                            IN_Opcodes.EQuipSots_InvSlots.replace(OldID,"XX");
                        }

                        IN_Opcodes.ItemIsEquippedVector.replace(Inventoryid,"YES");
                        IN_Opcodes.EquipmentSlotsUsed.insert(EQuipSlot,"USED");
                        IN_Opcodes.EQuipSots_InvSlots.replace(Inventoryid,EQuipSlot);

                        if((EQuipTypeToSlot.contains(EQuipmentsType) && WeaponGraphic != "00000000") ||
                                (EQuipTypeToSlot.contains(EQuipmentsType) && ArmorGraphic != "00") ||
                                (EQuipSlot == "15") || (EQuipSlot == "06") || (EQuipSlot == "07"))
                        {
                            current_c9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8());

                            if(WeaponGraphic == "00000000"  && EQuipSlot != "06" && EQuipSlot != "07")//this means it is Armor
                            {
                                // Armor position and length
                                current_c9.replace(Position,Length,ArmorGraphic.toUtf8());

                                // Color position and length
                                current_c9.replace(PositionC,LengthC,ArmorColor.toUtf8());

                                if(EQuipSlot == "08") // if it is in 2forearms slot
                                {
                                    FreeUsed = IN_Opcodes.EquipmentSlotsUsed.value("06");

                                    if(FreeUsed == "USED")// check if right forearm is used
                                    {
                                        IN_Opcodes.EquipmentSlotsUsed.insert("06","FREE");
                                        int index = IN_Opcodes.EQuipSots_InvSlots.indexOf("06");
                                        IN_Opcodes.ItemIsEquippedVector.replace(index,"NO");
                                        IN_Opcodes.EQuipSots_InvSlots.replace(index,"XX");
                                    }

                                    FreeUsed = IN_Opcodes.EquipmentSlotsUsed.value("07");

                                    if(FreeUsed == "USED")// check if left forearm is used
                                    {
                                        IN_Opcodes.EquipmentSlotsUsed.insert("07","FREE");
                                        int index = IN_Opcodes.EQuipSots_InvSlots.indexOf("07");
                                        IN_Opcodes.ItemIsEquippedVector.replace(index,"NO");
                                        IN_Opcodes.EQuipSots_InvSlots.replace(index,"XX");
                                    }
                                }
                            }

                            if(ArmorGraphic == "00" && EQuipSlot != "15" && EQuipSlot != "06" && EQuipSlot != "07")//this means it is Weapons/Held...
                            {
                                //and its not a robe

                                //check which equip slots this slot will replace..
                                //reset all that are USED..
                                //Clear those locations in the c9 with 00000000...

                                QVector <QString> WeaponCloseIfOpen = WeaponCheckIfOpen.value(EQuipSlot);

                                int test = 0;
                                for(int i = 0; i < WeaponCloseIfOpen.size(); i++)
                                {
                                    QString CheckSlot = WeaponCloseIfOpen.at(i);

                                    QString FreeUsed = IN_Opcodes.EquipmentSlotsUsed.value(CheckSlot);

                                    test = test + 1;

                                    if(CheckSlot == "10" && FreeUsed == "USED")
                                    {
                                        current_c9.replace(182,2,"00");
                                    }

                                    if(FreeUsed == "USED")// check if slots are in use
                                    {
                                        QString ReplaceSlot = C9replaceWeapon.value(CheckSlot);

                                        int index1 = C9WeaponLocations.indexOf(ReplaceSlot);
                                        QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                                        QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                                        int Position = PositionS1.toInt(nullptr,10);
                                        int Length = LengthS1.toInt(nullptr,10);

                                        current_c9.replace(Position,Length,"00000000");
                                        IN_Opcodes.EquipmentSlotsUsed.insert(CheckSlot,"FREE");
                                        int index = IN_Opcodes.EQuipSots_InvSlots.indexOf(CheckSlot);
                                        IN_Opcodes.ItemIsEquippedVector.replace(index,"NO");
                                        IN_Opcodes.EQuipSots_InvSlots.replace(index,"XX");
                                    }
                                }

                                // Write new graphic into the C9
                                // Weapon position and length
                                current_c9.replace(Position,Length,WeaponGraphic.toUtf8());

                                QString ReplaceSlot = C9replaceWeapon.value(EQuipSlot);

                                if(ReplaceSlot == "Primary")
                                {
                                    if(EQuipSlot == "10")
                                    {
                                        current_c9.replace(182,2,ATT_Type4.toUtf8());
                                        current_c9.replace(184,2,"00");

                                        QString PrimaryDMG1 = ConvertBack.BackToDec(ItemsDMG);
                                        IN_Opcodes.PrimaryDMG = PrimaryDMG1.toInt(nullptr,16);

                                        QString SecondaryDMG1 = ConvertBack.BackToDec("00");
                                        IN_Opcodes.SecondaryDMG = SecondaryDMG1.toInt(nullptr,16);
                                    }
                                    else
                                    {
                                        current_c9.replace(182,2,ATT_Type4.toUtf8());

                                        if(EQuipSlot == "0E")
                                        {
                                            QString PrimaryDMG1 = ConvertBack.BackToDec(ItemsDMG);
                                            IN_Opcodes.PrimaryDMG = PrimaryDMG1.toInt(nullptr,16);
                                        }
                                        else
                                        {
                                            QString PrimaryDMG1 = ConvertBack.BackToDec("00");
                                            IN_Opcodes.PrimaryDMG = PrimaryDMG1.toInt(nullptr,16);
                                        }
                                    }
                                }
                                if(ReplaceSlot == "Secondary")
                                {
                                    if(EQuipSlot == "12")
                                    {
                                        current_c9.replace(182,2,"00");
                                        current_c9.replace(184,2,ATT_Type4.toUtf8());

                                        QString PrimaryDMG1 = ConvertBack.BackToDec("00");
                                        IN_Opcodes.PrimaryDMG = PrimaryDMG1.toInt(nullptr,16);

                                        QString SecondaryDMG1 = ConvertBack.BackToDec(ItemsDMG);
                                        IN_Opcodes.SecondaryDMG = SecondaryDMG1.toInt(nullptr,16);
                                    }
                                    else
                                    {
                                        current_c9.replace(184,2,ATT_Type4.toUtf8());

                                        if(EQuipSlot == "0F")
                                        {
                                            QString SecondaryDMG1 = ConvertBack.BackToDec(ItemsDMG);
                                            IN_Opcodes.SecondaryDMG = SecondaryDMG1.toInt(nullptr,16);
                                        }
                                        else
                                        {
                                            QString SecondaryDMG1 = ConvertBack.BackToDec("00");
                                            IN_Opcodes.SecondaryDMG = SecondaryDMG1.toInt(nullptr,16);
                                        }
                                    }
                                }
                                if(ReplaceSlot == "Shield")
                                {
                                    current_c9.replace(184,2,"00");

                                    QString SecondaryDMG1 = ConvertBack.BackToDec("00");
                                    IN_Opcodes.SecondaryDMG = SecondaryDMG1.toInt(nullptr,16);
                                }
                            }


                            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
                            if(EQuipSlot == "06")//if its in the right forearm slot
                            {
                                FreeUsed = IN_Opcodes.EquipmentSlotsUsed.value("08");

                                if(FreeUsed == "USED")//check if 2forearms is used
                                {
                                    current_c9.replace(188,2,"00");
                                    current_c9.replace(232,8,"FFFFFFFF");
                                    IN_Opcodes.EquipmentSlotsUsed.insert("08","FREE");
                                    int index = IN_Opcodes.EQuipSots_InvSlots.indexOf("08");
                                    IN_Opcodes.ItemIsEquippedVector.replace(index,"NO");
                                    IN_Opcodes.EQuipSots_InvSlots.replace(index,"XX");
                                }
                            }

                            if(EQuipSlot == "07")//if its is in left forearm slot
                            {
                                FreeUsed = IN_Opcodes.EquipmentSlotsUsed.value("08");

                                if(FreeUsed == "USED")//check if 2 forearms is used
                                {
                                    current_c9.replace(188,2,"00");
                                    current_c9.replace(232,8,"FFFFFFFF");
                                    IN_Opcodes.EquipmentSlotsUsed.insert("08","FREE");
                                    int index = IN_Opcodes.EQuipSots_InvSlots.indexOf("08");
                                    IN_Opcodes.ItemIsEquippedVector.replace(index,"NO");
                                    IN_Opcodes.EQuipSots_InvSlots.replace(index,"XX");
                                }
                            }

                            generator::Players_Map.insert(IN_Opcodes.CharSelectID.toUtf8(),current_c9);

                            generator packet4;
                            QByteArray holdergen1 = packet4.maintoonswitch(IN_Opcodes.CharSelectID,"",IN_Opcodes.CharSelectID_Main);
                            QByteArray holdergen2 = "00C9030000" + holdergen1;


                            IN_Opcodes.SendZeroChannelVector.append(holdergen2);

                        }

                        QString InvSlot2 = QString("%1").arg(Inventoryid,2,16,QLatin1Char('0')).toUpper();
                        FBsizeOpcode = "3F00" + InvSlot2 + EQuipSlot;

                        IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Equip_Item, ");
                    }


                    //////////////////////////////////unEQuip/////////////////////////////////////////////////////////////////

                    if(IsItemEquipped == "YES")// unEQuipt Item
                    {

                        EQuipSlot = IN_Opcodes.EQuipSots_InvSlots.at(Inventoryid);

                        FBsizeOpcode = "4000" + EQuipSlot;

                        IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("UnEquip_Item, ");

                        if(EQuipSlot == "0E" || EQuipSlot == "13" || EQuipSlot == "10")
                        {
                            ReplaceLocation = "Primary";
                            int index1 = C9WeaponLocations.indexOf(ReplaceLocation);

                            QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                            QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                            Position = PositionS1.toInt(nullptr,10);
                            Length = LengthS1.toInt(nullptr,10);

                            if(EQuipSlot == "10")
                            {
                                current_c9.replace(182,2,"00");
                                current_c9.replace(184,2,"00");
                            }
                            else
                            {
                                current_c9.replace(182,2,"00");
                            }

                        }

                        if(EQuipSlot == "0F" || EQuipSlot == "12" || EQuipSlot == "14")
                        {
                            ReplaceLocation = "Secondary";
                            int index1 = C9WeaponLocations.indexOf(ReplaceLocation);

                            QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                            QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                            Position = PositionS1.toInt(nullptr,10);
                            Length = LengthS1.toInt(nullptr,10);

                            if(EQuipSlot == "12")
                            {
                                current_c9.replace(182,2,"00");
                                current_c9.replace(184,2,"00");
                            }
                            else
                            {
                                current_c9.replace(182,2,"00");
                            }

                        }

                        if(EQuipSlot == "11")
                        {
                            ReplaceLocation = "Shield";
                            int index1 = C9WeaponLocations.indexOf(ReplaceLocation);

                            QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                            QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                            Position = PositionS1.toInt(nullptr,10);
                            Length = LengthS1.toInt(nullptr,10);
                        }

                        if(Position != 0)
                        {
                            current_c9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8());

                            if(WeaponGraphic == "00000000")//this means it is Armor/Shields....
                            {
                                QString ArmorGraphic2 = "";

                                if(EQuipSlot == "15")// if its a robe
                                {
                                    ArmorGraphic2 = "FF";
                                }
                                else
                                {
                                    ArmorGraphic2 = "00";
                                }


                                // Armor position and length
                                current_c9.replace(Position,Length,ArmorGraphic2.toUtf8());

                                // Color position and length
                                current_c9.replace(PositionC,LengthC,"FFFFFFFF");

                            }

                            if(ArmorGraphic == "00" && EQuipSlot != "15")//this means it is Weapons/Held...
                            {
                                // Weapon position and length
                                current_c9.replace(Position,Length,"00000000");

                                QString ReplaceSlot = C9replaceWeapon.value(EQuipSlot);

                                if(ReplaceSlot == "Primary")
                                {
                                    current_c9.replace(182,2,"00");
                                }
                                if(ReplaceSlot == "Secondary")
                                {
                                    current_c9.replace(184,2,"00");
                                }
                            }

                            generator::Players_Map.insert(IN_Opcodes.CharSelectID.toUtf8(),current_c9);

                            generator packet4;
                            QByteArray holdergen1 = packet4.maintoonswitch(IN_Opcodes.CharSelectID,"",IN_Opcodes.CharSelectID_Main);
                            QByteArray holdergen2 = "00C9030000" + holdergen1;


                            IN_Opcodes.SendZeroChannelVector.append(holdergen2);
                        }

                        IN_Opcodes.EquipmentSlotsUsed.insert(EQuipSlot,"FREE");

                        IN_Opcodes.EQuipSots_InvSlots.replace(Inventoryid,"XX");
                        IN_Opcodes.ItemIsEquippedVector.replace(Inventoryid,"NO");

                    }
                }
            }

            qDebug() << "Items_Type" << Items_Type;
            qDebug() << "ClassStatusIs" << ClassStatusIs;
            qDebug() << "RaceStatusIs" << RaceStatusIs;

            if(Items_Type == "EQuipment")
            {
                if(ClassStatusIs == "BAD" || RaceStatusIs == "BAD")//check Class/Race first
                {

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    ADD_OR_SUB SendToText;
                    QString EQuipText = "You cannot equip that item.";
                    QString FBText =  SendToText.TextSize(ClientID,ClientsIP,ClientsPort,EQuipText,true,"CE00",Servers_Last_FB_MessageNumber);

                    IN_Opcodes.SendFBsVector.append(FBText.toUtf8());
                    IN_Opcodes.FBReplyOrNotVector.append("03");
                    IN_Opcodes.FBPacketDescriptionVector.append("Equip_Failed_Text1, ");
                }
                else
                {
                    if(LevelStatusIs == "BAD")
                    {
                        QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                        QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                        ADD_OR_SUB SendToText;
                        QString EQuipText = "Your level is too low to equip that item.";
                        QString FBText =  SendToText.TextSize(ClientID,ClientsIP,ClientsPort,EQuipText,true,"CE00",Servers_Last_FB_MessageNumber);

                        IN_Opcodes.SendFBsVector.append(FBText.toUtf8());
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Equip_Failed_Text2, ");
                    }
                }
            }

            if(Items_Type == "Other")// useable code will go here
            {
                if(DoesItHaveCharges == false)
                {

                }

                if(DoesItHaveCharges == true)
                {

                }
            }


            int NormalMySwingType = ClassSwingCountMap.value(PlayersClass);

            qDebug() << "ClassSwingCountMap" << ClassSwingCountMap;

            QVector <QString> IsItUsedVector;

            QMapIterator<QString,QString> iter (IN_Opcodes.EquipmentSlotsUsed);
            while (iter.hasNext())
            {
                iter.next();
                QString chkey1 = iter.key();
                QString mapval1 = iter.value();


                if(mapval1 == "USED")
                {
                    IsItUsedVector.append(chkey1);
                }
            }

            qDebug() << "IsItUsedVector" << IsItUsedVector;

            if(NormalMySwingType == 4)//if it is 4 hits normally
            {
                if(
                        (IsItUsedVector.contains("0E") && IsItUsedVector.contains("11")) ||//Primary,Shield = 2 hits
                        (IsItUsedVector.contains("0E") && IsItUsedVector.contains("13")) ||//Primary,SecondaryHeld = 2 hits
                        (!IsItUsedVector.contains("0E") && IsItUsedVector.contains("11"))||//Nothing,Shield = 2 hits
                        (IsItUsedVector.contains("13") && IsItUsedVector.contains("0F")) ||//PrimaryHeld,Nothing = 2 hits
                        (IsItUsedVector.contains("10")) ||                                 //2hnd = 2 hits
                        (IsItUsedVector.contains("12"))                                    //1 hit
                        )
                {

                    if(IN_Opcodes.MySwingType == 2)// too low to quad and was downgraded to 2 hits for level
                    {
                        IN_Opcodes.MySwingType = 1;//downgrad to 1 hit for equipment used
                    }
                    if(IN_Opcodes.MySwingType == 4)//Can quad
                    {
                        IN_Opcodes.MySwingType = 2;//downgrad to 2 hits for equipment used

                        if(EQuipSlot == "12")
                        {
                            IN_Opcodes.MySwingType = 1;//downgrad to 2 hits for equipment used
                        }

                    }
                }
            }



            if(NormalMySwingType == 2)//if it is 2 hits normally
            {
                if(PlayersLevel2 >= 1 && PlayersLevel2 < 34)
                {
                    if(
                            (IsItUsedVector.contains("0E") && IsItUsedVector.contains("11")) ||//Primary,Shield = 1 hit
                            (IsItUsedVector.contains("0E") && IsItUsedVector.contains("13")) ||//Primary,SecondaryHeld = 1 hit
                            (!IsItUsedVector.contains("0E") && IsItUsedVector.contains("11"))||//Nothing,Shield = 1 hit
                            (IsItUsedVector.contains("13") && IsItUsedVector.contains("0F")) ||//PrimaryHeld,Nothing = 1 hit
                            (IsItUsedVector.contains("10")) ||                                 //2hnd = 1 hit
                            (IsItUsedVector.contains("12"))
                            )
                    {
                        IN_Opcodes.MySwingType = 1;//downgrad to 1 hit for equipment used
                    }
                }
            }


            if(PlayersLevel2 >= 1 && PlayersLevel2 <= 60)//all the ways to not hit
            {
                if(
                        (IsItUsedVector.contains("13") && IsItUsedVector.contains("14")) ||//PrimaryHeld,SecondaryHeld = 0 hits
                        (IsItUsedVector.contains("13") && IsItUsedVector.contains("11"))   //PrimaryHeld,Shield = 0 hits
                        )
                {
                    IN_Opcodes.MySwingType = 0;//downgrad to 0 hits for equipment used
                }
            }

            IN_Opcodes.MySwing = IN_Opcodes.MySwingType;
            IN_Opcodes.MySwingNumber = IN_Opcodes.MySwingType;

            qDebug() << "IN_Opcodes.MySwingType bottom of equip/use" << IN_Opcodes.MySwingType;

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
            qDebug() << "";
            qDebug() << "";
            qDebug() << "";
            qDebug() << "";
        }

        // Delete Item
        // FB0A 1800 3C00 1B000000 05000000
        // FB0A 0C00 3C00 1C000000 E7030000
        if(IN_Opcodes.opcoderesult1 == "3C00")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray Item_Slot = opholder.mid(12,8);
            QByteArray Item_Slot2 = Item_Slot.mid(0,2);
            QByteArray Item_amt1 = opholder.mid(20,2);
            QByteArray Item_amt2 = opholder.mid(22,2);
            QByteArray Item_amt3 = opholder.mid(24,2);

            ADD_OR_SUB ConvertBack;
            Convert Send;
            QString BackToDec = "";
            QString ItemsID = "";
            QString CurrentInventoryQuantity = "";
            QString Item_Quantity4 = "";

            int Inventoryid = IN_Opcodes.Inventory_Vector.indexOf(Item_Slot);
            ItemsID = IN_Opcodes.InventorySlots_ItemIDs.at(Inventoryid);
            CurrentInventoryQuantity = IN_Opcodes.InventoryItem_Quantity.at(Inventoryid);

            QVector <QString> ItemsFields = merchants::MasterItems_Vector_Map.value(ItemsID);
            QString WeaponGraphic = ItemsFields.at(1);
            QString ArmorGraphic = ItemsFields.at(2);
            QString Charges = ItemsFields.at(5);

            if(Charges != "00")
            {
                CurrentInventoryQuantity = "02";
            }


            int Item_Quantity2 = Item_amt2.toInt(nullptr,16);
            int Item_Quantity3 = Item_amt3.toInt(nullptr,16);

            if(Item_Quantity2 > 0)// if 2nd byte is > 0 it is part of the amount
            {
                if(Item_Quantity3 > 0)// if 3rd byte is > 0 it is part of the amount
                {
                    Item_Quantity4 = Item_amt3 + Item_amt2 + Item_amt1;
                }
                else
                {
                    Item_Quantity4 = Item_amt2 + Item_amt1;
                }
            }
            else
            {
                Item_Quantity4 = Item_amt1;
            }


            int DestroyQuantity = Item_Quantity4.toInt(nullptr,16);

            BackToDec = ConvertBack.BackToDec(CurrentInventoryQuantity);
            int InvQuantity = BackToDec.toInt(nullptr,10);

            int DestroySubTotal = InvQuantity - DestroyQuantity;

            if(DestroySubTotal == 0)
            {
                IN_Opcodes.InventorySlots_ItemIDs.removeAt(Inventoryid);
                IN_Opcodes.Inventory_Vector.removeAt(Inventoryid);
                IN_Opcodes.InventoryItem_Quantity.removeAt(Inventoryid);

                QString IsItemEQupped = IN_Opcodes.ItemIsEquippedVector.at(Inventoryid);


                if(IsItemEQupped == "YES")
                {
                    QString ReplaceLocation = "";
                    int Position = 0;
                    int Length = 0;
                    int PositionC = 0;
                    int LengthC = 0;
                    QString EQuipSlot = "";

                    EQuipSlot = IN_Opcodes.EQuipSots_InvSlots.at(Inventoryid);


                    if(C9replaceGear.contains(EQuipSlot))
                    {
                        ReplaceLocation = C9replaceGear.value(EQuipSlot);
                        int index1 = C9GearLocations.indexOf(ReplaceLocation);


                        QString PositionS1 = C9GearLocations.at(index1 + 1);
                        QString LengthS1 = C9GearLocations.at(index1 + 2);
                        Position = PositionS1.toInt(nullptr,10);
                        Length = LengthS1.toInt(nullptr,10);


                        QString PositionS2 = C9GearLocationsC.at(index1 + 1);
                        QString LengthS2 = C9GearLocationsC.at(index1 + 2);
                        PositionC = PositionS2.toInt(nullptr,10);
                        LengthC = LengthS2.toInt(nullptr,10);

                    }

                    if(C9replaceWeapon.contains(EQuipSlot))
                    {
                        ReplaceLocation = C9replaceWeapon.value(EQuipSlot);
                        int index1 = C9WeaponLocations.indexOf(ReplaceLocation);

                        QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                        QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                        Position = PositionS1.toInt(nullptr,10);
                        Length = LengthS1.toInt(nullptr,10);
                    }

                    if(Position != 0)
                    {
                        QByteArray current_c9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8());
                        if(WeaponGraphic == "00000000")//this means it is Armor/Shields....
                        {
                            QString ArmorGraphic2 = "";

                            if(EQuipSlot == "15")// if its a robe
                            {
                                ArmorGraphic2 = "FF";
                            }
                            else
                            {
                                ArmorGraphic2 = "00";
                            }

                            // Armor position and length
                            current_c9.replace(Position,Length,ArmorGraphic2.toUtf8());

                            // Color position and length
                            current_c9.replace(PositionC,LengthC,"FFFFFFFF");

                        }

                        if(ArmorGraphic == "00" && EQuipSlot != "15")//this means it is Weapons/Held...
                        {
                            // Weapon position and length
                            current_c9.replace(Position,Length,"00000000");
                        }

                        generator::Players_Map.insert(IN_Opcodes.CharSelectID.toUtf8(),current_c9);

                        generator packet4;
                        QByteArray holdergen1 = packet4.maintoonswitch(IN_Opcodes.CharSelectID,"",IN_Opcodes.CharSelectID_Main);
                        QByteArray holdergen2 = "00C9030000" + holdergen1;

                        IN_Opcodes.SendZeroChannelVector.append(holdergen2);
                    }

                    QString ItemsSlot = IN_Opcodes.EQuipSots_InvSlots.at(Inventoryid);

                    IN_Opcodes.EquipmentSlotsUsed.insert(ItemsSlot,"FREE");
                }

                QString ItemsSlot = IN_Opcodes.EQuipSots_InvSlots.at(Inventoryid);

                IN_Opcodes.ItemIsEquippedVector.removeAt(Inventoryid);
                IN_Opcodes.InvItem_Type.removeAt(Inventoryid);
                IN_Opcodes.EQuipSots_InvSlots.removeAt(Inventoryid);

                IN_Opcodes.InventoryMAX = IN_Opcodes.InventoryMAX - 1;
            }
            else
            {
                QString Destroy_Item2 = QString("%1").arg(DestroySubTotal,2,10,QLatin1Char('0')).toUpper();
                Destroy_Item2 = Send.ConvertTo(Destroy_Item2);
                IN_Opcodes.InventoryItem_Quantity.replace(Inventoryid,Destroy_Item2);
            }


            QString slotValue1 = QString("%1").arg(Inventoryid,2,16,QLatin1Char('0')).toUpper();

            QString Destroy_Item2 = QString("%1").arg(DestroyQuantity,2,10,QLatin1Char('0')).toUpper();
            Destroy_Item2 = Send.ConvertTo(Destroy_Item2);

            QString FBsizeOpcode = "3C00" + slotValue1.toUtf8() + "01" + Destroy_Item2.toUtf8();

            IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("Delete_Item, ");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }

        ////////////////////Bank Items/////////////////////////////////////////////////////////

        if(IN_Opcodes.opcoderesult1 == "5412")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray RemoveOrAddItems = opholder.mid(20,2);
            QByteArray Item_Slot = opholder.mid(22,8);
            QByteArray Item_amt = opholder.mid(30,-1);


            QString CurrentItem0 = "";
            ADD_OR_SUB ConvertBack;
            Convert Send;
            increment CurrentItem1;
            QByteArray CurrentItem2 = "";
            QString BackToDec = "";
            QString ItemsID = "";
            QString Inventoryidxxxxxxxx = "";
            int QuantityLocation = 0;
            QString InventoryQuantityTo = "";
            QString CurrentQuantityFrom = "";
            QString ItemAmount = Item_amt;
            int InvSlotFrom = 0;
            int AddInvQuantityTo = 0;
            QString ItemStatus = "";
            int NewQuantityFrom = 0;

            QVector <QString> SlotsFromVector;
            QVector <QString> ItemIDsFromVector;
            QVector <QString> ItemsQuantityFromVector;
            QByteArray IDFromCounter = "";
            int TotalSlotsUsedFrom = 0;

            QVector <QString> SlotsToVector;
            QVector <QString> ItemIDsToVector;
            QVector <QString> ItemsQuantityToVector;
            QByteArray IDToCounter = "";
            int TotalSlotsUsedTo = 0;
            QByteArray RemoveOpcode = "";
            QByteArray AddOpcode = "";
            QString TextFull1 = "";
            QString TextFull2 = "";
            QString TextFull3 = "";

            if(RemoveOrAddItems == "01")// from bank to inventory
            {
                SlotsFromVector = IN_Opcodes.Bank_Vector;
                ItemIDsFromVector = IN_Opcodes.BankSlots_ItemIDs;
                ItemsQuantityFromVector = IN_Opcodes.BankItem_Quantity;
                IDFromCounter = IN_Opcodes.BankIDCounter;
                TotalSlotsUsedFrom = IN_Opcodes.BankMAX;

                SlotsToVector = IN_Opcodes.Inventory_Vector;
                ItemIDsToVector = IN_Opcodes.InventorySlots_ItemIDs;
                ItemsQuantityToVector = IN_Opcodes.InventoryItem_Quantity;
                IDToCounter = IN_Opcodes.InventoryIDCounter;
                TotalSlotsUsedTo = IN_Opcodes.InventoryMAX;

                TextFull1 = "Your inventory is full.";
                TextFull2 = "Your inventory for that stack item is full";
                TextFull3 = "You could not carry all that.";

                RemoveOpcode = "5012";
                AddOpcode = "3B00";
            }

            if(RemoveOrAddItems == "00")// from inventory to bank
            {
                SlotsFromVector = IN_Opcodes.Inventory_Vector;
                ItemIDsFromVector = IN_Opcodes.InventorySlots_ItemIDs;
                ItemsQuantityFromVector = IN_Opcodes.InventoryItem_Quantity;
                IDFromCounter = IN_Opcodes.InventoryIDCounter;
                TotalSlotsUsedFrom = IN_Opcodes.InventoryMAX;

                SlotsToVector = IN_Opcodes.Bank_Vector;
                ItemIDsToVector = IN_Opcodes.BankSlots_ItemIDs;
                ItemsQuantityToVector = IN_Opcodes.BankItem_Quantity;
                IDToCounter = IN_Opcodes.BankIDCounter;
                TotalSlotsUsedTo = IN_Opcodes.BankMAX;

                TextFull1 = "Your banks inventory is full.";
                TextFull2 = "Your banks inventory for that stack item is full";
                TextFull3 = "You could not bank all that.";

                RemoveOpcode = "3C00";
                AddOpcode = "5112";
            }

            InvSlotFrom = SlotsFromVector.indexOf(Item_Slot.toUpper());

            ItemsID = ItemIDsFromVector.at(InvSlotFrom);
            CurrentQuantityFrom = ItemsQuantityFromVector.at(InvSlotFrom);

            QVector <QString> ItemsFields = merchants::MasterItems_Vector_Map.value(ItemsID);
            QString ItemsMaxSize = ItemsFields.at(23);
            QString WeaponGraphic = ItemsFields.at(1);
            QString ArmorGraphic = ItemsFields.at(2);
            QString Charges = ItemsFields.at(5);
            QString EQuipmentsType = ItemsFields.at(14);

            bool DoesItHaveCharges = false;

            if(Charges != "00")
            {
                DoesItHaveCharges = true;
                CurrentQuantityFrom = "02";
            }

            BackToDec = ConvertBack.BackToDec(ItemAmount);
            int RemoveQuantity = BackToDec.toInt(nullptr,10);

            BackToDec = ConvertBack.BackToDec(CurrentQuantityFrom);
            int InvQuantityFrom = BackToDec.toInt(nullptr,10);

            int RemoveSubTotal = InvQuantityFrom - RemoveQuantity;

            BackToDec = ConvertBack.BackToDec(ItemsMaxSize);
            int ItemsMaxSize2 = BackToDec.toInt(nullptr,10);

            if(ItemIDsToVector.contains(ItemsID))
            {
                QuantityLocation = ItemIDsToVector.indexOf(ItemsID);
                Inventoryidxxxxxxxx = SlotsToVector.at(QuantityLocation);
                InventoryQuantityTo = ItemsQuantityToVector.at(QuantityLocation);

                if(DoesItHaveCharges == true)
                {
                    InventoryQuantityTo = "02";
                }

                BackToDec = ConvertBack.BackToDec(InventoryQuantityTo);
                int InvQuantityTo = BackToDec.toInt(nullptr,10);

                AddInvQuantityTo = InvQuantityTo + RemoveQuantity;

                int PartialInvQuantityTo = 0;
                int AmountOver = 0;

                if(AddInvQuantityTo > ItemsMaxSize2)
                {
                    AmountOver = AddInvQuantityTo - ItemsMaxSize2;
                    PartialInvQuantityTo = RemoveQuantity - AmountOver;

                    if(PartialInvQuantityTo != 0)
                    {
                        ItemStatus = "Change_ItemQuantityFrom";
                    }
                }
                else
                {
                    PartialInvQuantityTo = RemoveQuantity;
                }

                if(ItemsMaxSize2 == 1 && TotalSlotsUsedTo == 40)
                {
                    //this part is for invtory is full opcode
                    IN_Opcodes.SendFBsVector.append("D700");
                    IN_Opcodes.FBReplyOrNotVector.append("03");
                    IN_Opcodes.FBPacketDescriptionVector.append("Fake_Opcode, ");
                    //this part is for invtory is full opcode

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    ADD_OR_SUB SendToText;
                    QString FBText =  SendToText.TextSize(ClientID,ClientsIP,ClientsPort,TextFull1,true,"CE00",Servers_Last_FB_MessageNumber);

                    IN_Opcodes.SendFBsVector.append(InventoryFullHeader + FBText.toUtf8().toUpper());
                    IN_Opcodes.FBReplyOrNotVector.append("03");
                    IN_Opcodes.FBPacketDescriptionVector.append("Inventory_Full_Text, ");
                }

                if(ItemsMaxSize2 == 1 && TotalSlotsUsedTo < 40)
                {
                    ItemStatus = "Delete_item";

                    CurrentItem0 = merchants::Items_Map.value(ItemsID.toUtf8());

                    int InventoryID = CurrentItem0.indexOf("XXXXXXXX");

                    CurrentItem2 = CurrentItem1.count(IDToCounter); //InventoryID increment
                    CurrentItem0.replace(InventoryID,8,CurrentItem2 + "0000");
                    IDToCounter = CurrentItem2.toUpper();

                    SlotsToVector.append(IDToCounter + "0000");

                    TotalSlotsUsedTo = TotalSlotsUsedTo + 1;

                    RemoveADDQuantity = ItemAmount;

                    if(RemoveOrAddItems == "01")
                    {
                        IN_Opcodes.ItemIsEquippedVector.append("NO");
                        IN_Opcodes.EQuipSots_InvSlots.append("XX");

                        if(EQuipmentsType == "01")
                        {
                            IN_Opcodes.InvItem_Type.append("Other");
                        }
                        else
                        {
                            IN_Opcodes.InvItem_Type.append("EQuipment");
                        }
                    }
                }

                if(ItemsMaxSize2 > 1 && TotalSlotsUsedTo <= 40 && PartialInvQuantityTo != 0)
                {
                    NewQuantityFrom = InvQuantityFrom - PartialInvQuantityTo;

                    if(NewQuantityFrom == 0)
                    {
                        ItemStatus = "Delete_item";
                    }

                    if(NewQuantityFrom > 0)
                    {
                        ItemStatus = "Change_ItemQuantityFrom";
                    }

                    CurrentItem0 = merchants::Items_Map.value(ItemsID.toUtf8());

                    int InventoryID = CurrentItem0.indexOf("XXXXXXXX");
                    CurrentItem0.replace(InventoryID,8,Inventoryidxxxxxxxx.toUtf8().toUpper());

                    QString PartialInvQuantityTo1 = QString("%1").arg(PartialInvQuantityTo,2,10,QLatin1Char('0')).toUpper();
                    RemoveADDQuantity = Send.ConvertTo(PartialInvQuantityTo1);

                    int PartialInvQuantityTo2 = InvQuantityTo + PartialInvQuantityTo;

                    QString PartialInvQuantityTo3 = QString("%1").arg(PartialInvQuantityTo2,2,10,QLatin1Char('0')).toUpper();
                    PartialInvQuantityTo3 = Send.ConvertTo(PartialInvQuantityTo3);

                    int Inventoryid = SlotsToVector.indexOf(Inventoryidxxxxxxxx);
                    ItemsQuantityToVector.replace(Inventoryid,PartialInvQuantityTo3);

                    if(AmountOver > 0)
                    {
                        QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                        QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                        ADD_OR_SUB SendToText;
                        QString FBText =  SendToText.TextSize(ClientID,ClientsIP,ClientsPort,TextFull3,true,"CE00",Servers_Last_FB_MessageNumber);

                        IN_Opcodes.SendFBsVector.append(FBText.toUtf8().toUpper());
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Bank_Item_Text, ");
                    }
                }

                if(ItemsMaxSize2 > 1 && TotalSlotsUsedTo <= 40 && PartialInvQuantityTo == 0)
                {
                    //this part is for invtory is full opcode
                    IN_Opcodes.SendFBsVector.append("D700");
                    IN_Opcodes.FBReplyOrNotVector.append("03");
                    IN_Opcodes.FBPacketDescriptionVector.append("Fake_Opcode, ");
                    //this part is for invtory is full opcode

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    ADD_OR_SUB SendToText;
                    QString FBText =  SendToText.TextSize(ClientID,ClientsIP,ClientsPort,TextFull2,true,"CE00",Servers_Last_FB_MessageNumber);

                    IN_Opcodes.SendFBsVector.append(FBText.toUtf8().toUpper());
                    IN_Opcodes.FBReplyOrNotVector.append("03");
                    IN_Opcodes.FBPacketDescriptionVector.append("Bank_Item_Failed_Text, ");
                }
            }

            else
            {
                if(RemoveSubTotal == 0 && TotalSlotsUsedTo != 40)
                {
                    ItemStatus = "Delete_item";
                }
                if(RemoveSubTotal != 0 && TotalSlotsUsedTo != 40)
                {
                    ItemStatus = "Change_ItemQuantityFrom";
                }

                if(TotalSlotsUsedTo < 40)
                {
                    NewQuantityFrom = RemoveSubTotal;

                    CurrentItem0 = merchants::Items_Map.value(ItemsID.toUtf8());

                    int InventoryID = CurrentItem0.indexOf("XXXXXXXX");

                    RemoveADDQuantity = ItemAmount;

                    CurrentItem2 = CurrentItem1.count(IDToCounter); //InventoryID increment
                    CurrentItem0.replace(InventoryID,8,CurrentItem2 + "0000");
                    IDToCounter = CurrentItem2.toUpper();

                    SlotsToVector.append(IDToCounter + "0000");

                    ItemsQuantityToVector.append(RemoveADDQuantity);
                    ItemIDsToVector.append(ItemsID);

                    TotalSlotsUsedTo = TotalSlotsUsedTo + 1;

                    if(RemoveOrAddItems == "01")
                    {
                        IN_Opcodes.ItemIsEquippedVector.append("NO");
                        IN_Opcodes.EQuipSots_InvSlots.append("XX");

                        if(EQuipmentsType == "01")
                        {
                            IN_Opcodes.InvItem_Type.append("Other");
                        }
                        else
                        {
                            IN_Opcodes.InvItem_Type.append("EQuipment");
                        }
                    }
                }

                else
                {
                    //this part is for invtory is full opcode
                    IN_Opcodes.SendFBsVector.append("D700");
                    IN_Opcodes.FBReplyOrNotVector.append("03");
                    IN_Opcodes.FBPacketDescriptionVector.append("Fake_Opcode, ");
                    //this part is for invtory is full opcode

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    ADD_OR_SUB SendToText;
                    QString FBText =  SendToText.TextSize(ClientID,ClientsIP,ClientsPort,TextFull1,true,"CE00",Servers_Last_FB_MessageNumber);

                    IN_Opcodes.SendFBsVector.append(FBText.toUtf8().toUpper());
                    IN_Opcodes.FBReplyOrNotVector.append("03");
                    IN_Opcodes.FBPacketDescriptionVector.append("Bank_Item_Failed_Text, ");
                }
            }

            QString FBsizeOpcode = "";
            if(ItemStatus == "Delete_item")
            {
                ItemIDsFromVector.removeAt(InvSlotFrom);
                SlotsFromVector.removeAt(InvSlotFrom);
                ItemsQuantityFromVector.removeAt(InvSlotFrom);

                TotalSlotsUsedFrom = TotalSlotsUsedFrom - 1;

                QString InvSlotFrom1 = QString("%1").arg(InvSlotFrom,2,16,QLatin1Char('0')).toUpper();


                if(RemoveOrAddItems == "01")
                {
                    FBsizeOpcode = RemoveOpcode + InvSlotFrom1.toUtf8().toUpper() + RemoveADDQuantity.toUtf8().toUpper();
                }

                if(RemoveOrAddItems == "00")//FB0C 0A00 5412 FAF20400 01 02000000 02
                {
                    FBsizeOpcode = RemoveOpcode + InvSlotFrom1.toUtf8().toUpper() + "01" + RemoveADDQuantity.toUtf8().toUpper();


                    QString IsItemEQupped = IN_Opcodes.ItemIsEquippedVector.at(InvSlotFrom);

                    if(IsItemEQupped == "YES")
                    {
                        QString ReplaceLocation;
                        int Position = 0;
                        int Length = 0;
                        QString ReplaceLocationC;
                        int PositionC = 0;
                        int LengthC = 0;
                        QString EQuipSlot = "";
                        QString Condition = "";
                        QString FreeUsed = "";

                        EQuipSlot = IN_Opcodes.EQuipSots_InvSlots.at(InvSlotFrom);

                        if(C9replaceGear.contains(EQuipSlot))
                        {
                            ReplaceLocation = C9replaceGear.value(EQuipSlot);
                            int index1 = C9GearLocations.indexOf(ReplaceLocation);

                            QString PositionS1 = C9GearLocations.at(index1 + 1);
                            QString LengthS1 = C9GearLocations.at(index1 + 2);
                            Position = PositionS1.toInt(nullptr,10);
                            Length = LengthS1.toInt(nullptr,10);

                            QString PositionS2 = C9GearLocationsC.at(index1 + 1);
                            QString LengthS2 = C9GearLocationsC.at(index1 + 2);
                            PositionC = PositionS2.toInt(nullptr,10);
                            LengthC = LengthS2.toInt(nullptr,10);
                        }

                        if(C9replaceWeapon.contains(EQuipSlot))
                        {
                            ReplaceLocation = C9replaceWeapon.value(EQuipSlot);
                            int index1 = C9WeaponLocations.indexOf(ReplaceLocation);

                            QString PositionS1 = C9WeaponLocations.at(index1 + 1);
                            QString LengthS1 = C9WeaponLocations.at(index1 + 2);
                            Position = PositionS1.toInt(nullptr,10);
                            Length = LengthS1.toInt(nullptr,10);
                        }

                        if(Position != 0)
                        {

                            QByteArray current_c9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8());
                            if(WeaponGraphic == "00000000")//this means it is Armor/Shields....
                            {
                                QString ArmorGraphic2 = "";

                                if(EQuipSlot == "15")// if its a robe
                                {
                                    ArmorGraphic2 = "FF";
                                }
                                else
                                {
                                    ArmorGraphic2 = "00";
                                }


                                // Armor position and length
                                current_c9.replace(Position,Length,ArmorGraphic2.toUtf8());

                                // Color position and length
                                current_c9.replace(PositionC,LengthC,"FFFFFFFF");

                            }

                            if(ArmorGraphic == "00" && EQuipSlot != "15")//this means it is Weapons/Held...
                            {
                                // Weapon position and length
                                current_c9.replace(Position,Length,"00000000");
                            }

                            generator::Players_Map.insert(IN_Opcodes.CharSelectID.toUtf8(),current_c9);

                            generator packet4;
                            QByteArray holdergen1 = packet4.maintoonswitch(IN_Opcodes.CharSelectID,"",IN_Opcodes.CharSelectID_Main);
                            QByteArray holdergen2 = "00C9030000" + holdergen1;


                            IN_Opcodes.SendZeroChannelVector.append(holdergen2);
                        }

                        QString ItemsSlot = IN_Opcodes.EQuipSots_InvSlots.at(InvSlotFrom);

                        IN_Opcodes.EquipmentSlotsUsed.remove(ItemsSlot);
                        IN_Opcodes.EquipmentSlotsUsed.insert(ItemsSlot,"FREE");
                    }

                    IN_Opcodes.ItemIsEquippedVector.removeAt(InvSlotFrom);
                    IN_Opcodes.InvItem_Type.removeAt(InvSlotFrom);
                    IN_Opcodes.EQuipSots_InvSlots.removeAt(InvSlotFrom);


                }

                IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Delete_Item, ");
            }

            if(ItemStatus == "Change_ItemQuantityFrom")
            {
                QString Remove_Item2 = QString("%1").arg(NewQuantityFrom,2,10,QLatin1Char('0')).toUpper();
                Remove_Item2 = Send.ConvertTo(Remove_Item2);
                ItemsQuantityFromVector.replace(InvSlotFrom,Remove_Item2);

                QString InvSlotFrom1 = QString("%1").arg(InvSlotFrom,2,16,QLatin1Char('0')).toUpper();

                if(RemoveOrAddItems == "01")
                {
                    FBsizeOpcode = RemoveOpcode + InvSlotFrom1.toUtf8().toUpper() + RemoveADDQuantity.toUtf8().toUpper();
                }

                if(RemoveOrAddItems == "00")
                {
                    FBsizeOpcode = RemoveOpcode + InvSlotFrom1.toUtf8().toUpper() + "01" + RemoveADDQuantity.toUtf8().toUpper();
                }

                IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Change_Quantity, ");
            }


            if(CurrentItem0.isEmpty() == false)
            {
                CurrentItem0.replace(0,2,RemoveADDQuantity.toUtf8());

                FBsizeOpcode = AddOpcode + CurrentItem0; // Add opcode

                IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Add_Item, ");
            }

            if(RemoveOrAddItems == "01")
            {
                IN_Opcodes.Bank_Vector = SlotsFromVector;
                IN_Opcodes.BankSlots_ItemIDs = ItemIDsFromVector;
                IN_Opcodes.BankItem_Quantity = ItemsQuantityFromVector;
                IN_Opcodes.BankMAX = TotalSlotsUsedFrom;
                IN_Opcodes.BankIDCounter = IDFromCounter;

                IN_Opcodes.Inventory_Vector = SlotsToVector;
                IN_Opcodes.InventorySlots_ItemIDs = ItemIDsToVector;
                IN_Opcodes.InventoryItem_Quantity = ItemsQuantityToVector;
                IN_Opcodes.InventoryIDCounter = IDToCounter;
                IN_Opcodes.InventoryMAX = TotalSlotsUsedTo;
            }
            if(RemoveOrAddItems == "00")
            {
                IN_Opcodes.Inventory_Vector = SlotsFromVector;
                IN_Opcodes.InventorySlots_ItemIDs = ItemIDsFromVector;
                IN_Opcodes.InventoryItem_Quantity = ItemsQuantityFromVector;
                IN_Opcodes.InventoryMAX = TotalSlotsUsedFrom;
                IN_Opcodes.InventoryIDCounter = IDFromCounter;

                IN_Opcodes.Bank_Vector = SlotsToVector;
                IN_Opcodes.BankSlots_ItemIDs = ItemIDsToVector;
                IN_Opcodes.BankItem_Quantity = ItemsQuantityToVector;
                IN_Opcodes.BankIDCounter = IDToCounter;
                IN_Opcodes.BankMAX = TotalSlotsUsedTo;
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

        }


        QByteArray Charm_Slot = opholder.mid(20,2);






        //        if(IN_Opcodes.opcoderesult1 == "3200")
        //        {
        //            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);



        //            // //////////////// Distance //////////////////////////////////////////////////


        //            QString PET_Master_ID = IN_Opcodes.Target_ID;
        //            QByteArray PET_Master_c9 = objectpacket::Master_Map.value(PET_Master_ID.toUtf8());

        //            //                        if(Roamers_List.contains(PET_Master_ID))
        //            //                        {
        //            //                            Roamers_List.remove(PET_Master_ID);
        //            //                        }

        //            QString holdx1 = PET_Master_c9.mid(12,6);
        //            QString holdz1 = PET_Master_c9.mid(18,6);
        //            QString holdy1 = PET_Master_c9.mid(24,6);
        //            QString NPC_face_val = PET_Master_c9.mid(30,2);
        //            QString NPC_Animation_val = PET_Master_c9.mid(116,2);
        //            int NPC_Level1 = PET_Master_c9.mid(358,2).toInt(nullptr,10);
        //            QString NPC_Type1 = PET_Master_c9.mid(370,4);

        //            // /////// Pet Radius //////////

        //            QByteArray  myx1 = IN_Opcodes.masterx; //send as 3 byte hex
        //            int xfromhex1 = myx1.toInt(nullptr,16);
        //            double maintoonx1 = xfromhex1 / 128.0;

        //            QByteArray  myz1 = IN_Opcodes.masterz;
        //            int zfromhex1 = myz1.toInt(nullptr,16);
        //            double maintoonz1 = zfromhex1 / 128.0;

        //            if(maintoonz1 > 130000)
        //            {
        //                maintoonz1 = maintoonz1 - 131072.00195193;
        //            }

        //            QByteArray  myy1 = IN_Opcodes.mastery;
        //            int yfromhex1 = myy1.toInt(nullptr,16);
        //            double maintoony1 = yfromhex1 / 128.0;



        //            //Pet xzy

        //            QString tempstrx = holdx1;
        //            int tempint1 = tempstrx.toInt(nullptr,16);
        //            double xinterlude = tempint1 / 128.0;

        //            QString tempstrz = holdz1;
        //            int tempint3 = tempstrz.toInt(nullptr,16);
        //            double  zinterlude = tempint3 / 128.0;

        //            if(zinterlude > 130000)
        //            {
        //                zinterlude = zinterlude - 131072.00195193;
        //            }

        //            QString tempstry = holdy1;
        //            int tempint2 = tempstry.toInt(nullptr,16);
        //            double yinterlude = tempint2 / 128.0;



        //            double sub1 = maintoonx1 - xinterlude;
        //            double sub2 = maintoonz1 - zinterlude;
        //            double sub3 = maintoony1 - yinterlude;

        //            double power1 = qPow(sub1,2);
        //            double power2 = qPow(sub2,2);
        //            double power3 = qPow(sub3,2);

        //            double firstresult = power1 + power2 + power3;

        //            double Targets_result = qSqrt(firstresult);

        //            QString Targets_Distance = QString("%1").arg(Targets_result,0,'g',10,QLatin1Char('0')); //float result to string

        //            if(IN_Opcodes.REcharm == "00")
        //            {
        //                if(NPC_Type1 == "0000" && PET_Master_ID != IN_Opcodes.CharSelectID_Main && Targets_result >= 20.0
        //                        && NPC_Level1 < IN_Opcodes.CurrentLevel.toInt(nullptr,10) && IN_Opcodes.Pet_Trip2 != "01")
        //                {
        //                    IN_Opcodes.Out_of_DistanceRange = "01";
        //                }
        //            }
        //            if(IN_Opcodes.REcharm == "01")
        //            {
        //                IN_Opcodes.Out_of_DistanceRange = "00";
        //            }

        //            if(IN_Opcodes.PET_Exists == "01" && IN_Opcodes.REcharm == "00")
        //            {
        //                IN_Opcodes.Pet_Trip2 = "01";
        //            }

        //            if(NPC_Type1 != "0000" && PET_Master_ID != IN_Opcodes.CharSelectID_Main && IN_Opcodes.Pet_Trip2 != "01")
        //            {
        //                IN_Opcodes.Cant_Charm = "01";
        //            }

        //            if(PET_Master_ID == IN_Opcodes.CharSelectID_Main && IN_Opcodes.Pet_Trip2 != "01")
        //            {
        //                IN_Opcodes.Self_Target = "01";
        //            }

        //            if(NPC_Type1 == "0000" && PET_Master_ID != IN_Opcodes.CharSelectID_Main && Targets_result <= 20.0 && NPC_Level1 >= IN_Opcodes.CurrentLevel.toInt(nullptr,10) && IN_Opcodes.Pet_Trip2 != "01")
        //            {
        //                IN_Opcodes.Out_of_LevelRange = "01";
        //            }

        //            if((IN_Opcodes.Self_Target != "01" && IN_Opcodes.Out_of_DistanceRange != "01" && IN_Opcodes.Out_of_LevelRange != "01" && IN_Opcodes.Cant_Charm != "01"
        //                && IN_Opcodes.Pet_Trip2 != "01" && IN_Opcodes.Charm_Lock == "00") || IN_Opcodes.REcharm == "01")
        //            {
        //                IN_Opcodes.Charm_Lock = "01";
        //                IN_Opcodes.Pet_Trip = "00";
        //            }






        //            if(IN_Opcodes.Pet_Trip2 == "01" || IN_Opcodes.Cant_Charm == "01" || IN_Opcodes.Self_Target == "01" || IN_Opcodes.Out_of_DistanceRange == "01" || IN_Opcodes.Out_of_LevelRange == "01" || IN_Opcodes.Out_Matched == "01")
        //            {
        //                if(IN_Opcodes.Pet_Trip2 == "01")//-----------------------------------/////////////
        //                {
        //                    IN_Opcodes.SendFBsVector.append("b100595E0300595E030000000000004a85c79e");
        //                    IN_Opcodes.SendFBsVector.append("CE001700000059006F007500200061006C0072006500610064007900200068006100760065002000610020007000650074002E00");
        //                }

        //                if(IN_Opcodes.Self_Target == "01")//---------------------------------------
        //                {
        //                    IN_Opcodes.SendFBsVector.append("b100595E0300595E030000000000004a85c79e");
        //                    IN_Opcodes.SendFBsVector.append("CE002D00000059006F0075002000630061006E006E006F0074002000740061007200670065007400200079006F0075007200730065006C006600200077006900740068002000740068006100740020006100620069006C006900740079002E00");
        //                }

        //                if(IN_Opcodes.Out_of_DistanceRange == "01")//--------------------------
        //                {
        //                    IN_Opcodes.SendFBsVector.append("b100595E0300595E030000000000004a85c79e");
        //                    IN_Opcodes.SendFBsVector.append("CE002D00000059006F0075007200200074006100720067006500740020006900730020006F007500740020006F0066002000720061006E0067006500200066006F0072002000740068006100740020006100620069006C006900740079002E00");
        //                }

        //                if(IN_Opcodes.Out_of_LevelRange == "01")//-----------------------------
        //                {
        //                    IN_Opcodes.SendFBsVector.append("b100595E0300595E030000000000004a85c79e");
        //                    IN_Opcodes.SendFBsVector.append("CE003300000059006F0075007200200074006100720067006500740020006900730020006F007500740020006F00660020006C006500760065006C002000720061006E0067006500200066006F0072002000740068006100740020006100620069006C006900740079002E00");
        //                }

        //                if(IN_Opcodes.Cant_Charm == "01")
        //                {
        //                    IN_Opcodes.SendFBsVector.append("b100595E0300595E030000000000004a85c79e");
        //                    IN_Opcodes.SendFBsVector.append("CE002D00000059006F00750020006D007500730074002000740061007200670065007400200061006E00200065006E0065006D007900200074006F0020007500730065002000740068006100740020006100620069006C006900740079002E00");
        //                }

        //                if(IN_Opcodes.Out_Matched == "01")
        //                {
        //                    IN_Opcodes.SendFBsVector.append("b100595E0300595E030000000000004a85c79e");
        //                    IN_Opcodes.SendFBsVector.append("CE003300000059006f007500200061007200650020006f00750074006d006100740063006800650064002e002000410074007400610063006b0069006e006700200061006700610069006e0020006900730020006e006f007400200061006400760069007300650064002e00");
        //                }

        //                IN_Opcodes.FBReplyOrNotVector.append("03");
        //                IN_Opcodes.FBPacketDescriptionVector.append("Spell_not_ready, ");
        //                IN_Opcodes.FBReplyOrNotVector.append("03");
        //                IN_Opcodes.FBPacketDescriptionVector.append("Failed_Spell_Text, ");

        //                IN_Opcodes.Charm_recovery = "";
        //                IN_Opcodes.Cant_Charm = "00";
        //                IN_Opcodes.Self_Target = "00";
        //                IN_Opcodes.Out_of_DistanceRange = "00";
        //                IN_Opcodes.Out_of_LevelRange = "00";
        //                IN_Opcodes.Out_Matched = "00";
        //                IN_Opcodes.Pet_Trip2 = "00";
        //            }

        //            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        //        }



        //Charm Step 1
        if(IN_Opcodes.opcoderesult1 == "4100")
        {

            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);


            QByteArray Dis_pet = opholder.toUpper();

            //Dismiss pet
            //FB030C00410005

            QByteArray Dis_pet2 = Dis_pet.mid(8,6);

            if(Dis_pet2 == "410008")//pet att
            {

            }

            if(Dis_pet2 == "410005")
            {
                if(IN_Opcodes.Pet_Follow == "01" && IN_Opcodes.Dismiss_Lock != "01")
                {

                    IN_Opcodes.Pet_Follow = "00";
                    IN_Opcodes.Dismiss_Lock = "01";

                    QByteArray Con_New = "00";
                    int Con_Old1 = IN_Opcodes.PetConOld.toInt(nullptr,16);
                    int Con_New1 = Con_New.toInt(nullptr,16);
                    int Con_New2 = Con_Old1 ^ Con_New1;

                    QString Output_New = QString("%1").arg(Con_New2,2,16,QLatin1Char('0'));
                    IN_Opcodes.PetConOld = Con_New.toUpper();


                    IN_Opcodes.Xor_Object_ID2 = Xor_Object_ID;
                    IN_Opcodes.Xor_Object_ID2.replace(362,2,Output_New.toUtf8());

                    xorpacket packet2;
                    QByteArray Recompressed2 = packet2.packetencrypt(IN_Opcodes.Xor_Object_ID2);


                    QByteArray Header_Pet = "D37CFA0A97C001bf543213" + IN_Opcodes.SessionID + "00cf07fb068f007F0070E50000xxc9xxxx01";

                    QByteArray Combined1 = Header_Pet + Recompressed2;
                    QByteArray holderinject2 = Combined1;



                    increment count;
                    QByteArray pull20 = IN_Opcodes.incrementmap.value(IN_Opcodes.Pet_Chan);
                    increment change1;
                    QByteArray result20 = change1.count(pull20);
                    IN_Opcodes.incrementmap.insert(IN_Opcodes.Pet_Chan,result20);
                    holderinject2.replace(0,4,IN_Opcodes.serverid1);
                    holderinject2.replace(4,4,IN_Opcodes.clientid1);

                    holderinject2.replace(22,6,IN_Opcodes.SessionID);

                    packetsize newsize6;
                    QByteArray sizeresult6 = newsize6.switchsize(holderinject2); //send to get new size
                    holderinject2.replace(8,4,sizeresult6);

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                    increment GrpOut;
                    QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                    holderinject2.replace(30,4,GrpOut_a);
                    Servers_Last_MessageNumber = GrpOut_a.toUpper();

                    MessNumbVect.replace(0,Servers_Last_MessageNumber);


                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    //fb1
                    increment opp1; //get class first opcode
                    QByteArray opp2 = opp1.count(Servers_Last_FB_MessageNumber); //first opcode increment
                    holderinject2.replace(38,4,opp2); //replace new count at 38 /46
                    Servers_Last_FB_MessageNumber = opp2.toUpper(); //replace new count

                    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                    CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                    holderinject2.replace(46,8,IN_Opcodes.Pet_ID.toUtf8().toUpper());
                    holderinject2.replace(54,2,IN_Opcodes.Pet_Chan.toUtf8().toUpper());
                    holderinject2.replace(58,4,result20);

                    objectpacket::AGGRO_List.clear();
                    IN_Opcodes.PET_Exists = "00";
                    objectpacket::Roamers_List.insert(IN_Opcodes.Pet_ID,IN_Opcodes.Pet_Chan);
                }
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }



        if(IN_Opcodes.opcoderesult1 == "B009") // disconnect thru in game menu
        {
            IN_Opcodes.Disconnected = true;

            if(MainWindow::MastersClientsID+MainWindow::MastersClientsIP+MainWindow::MastersClientsPort ==
                    IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort)
            {
                MainWindow::MastersClientsID = "";
                MainWindow::MastersClientsIP = "";
                MainWindow::MastersClientsPort = "";
            }

            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);



            packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);

            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort);

            qDebug() << "IN_Opcodes.Disconnected = " << IN_Opcodes.Disconnected;

            QVector<QVector<QTimer*>> VectorOfQtimerVectors =
            {
                MainWindow::OutgoingTimersVector,
                MainWindow::CheckForAggroMovementTimersVector,
                MainWindow::GroupTimersVector,
                MainWindow::TeleportTimersVector,
                MainWindow::CoachTimersVector,
                MainWindow::UpdateMapsTimersVector,
                MainWindow::LoS_TimersVector,
                MainWindow::MySwingTimersVector,
                MainWindow::MobSwingTimersVector,
                MainWindow::SpawningTimersVector,
                MainWindow::reSpawningTimersVector,
                MainWindow::RoamingTimersVector,
                MainWindow::RegenTimersVector,
                MainWindow::reSend42ecTimersVector,
                MainWindow::CheckForAggroTimersVector,
                MainWindow::reSendFBsTimersVector,
                MainWindow::reSendChannelsTimersVector,
            };

            for(int qt = 0; qt < VectorOfQtimerVectors.size(); qt++)
            {
                QVector<QTimer*> CurrentTimerVector = VectorOfQtimerVectors.at(qt);

                for(int cc = 0; cc < CurrentTimerVector.size(); cc++) //remove main toon first
                {
                    auto* receivedtimer = CurrentTimerVector.at(cc);

                    QString  ObjectID = receivedtimer->objectName();

                    if(ObjectID == ClientID + ClientsIP + ClientsPort)
                    {
                        if(CurrentTimerVector.contains(receivedtimer))
                        {
                            receivedtimer->stop();
                            receivedtimer->deleteLater();
                            CurrentTimerVector.removeAt(cc);
                        }
                    }
                }
            }

            /////////////////////////Save Location///////////////////////////


            QSqlQuery querytlpt2;
            QString tablesel2 = "MAINTOON";
            QString objecttlpt2 = "ObjectID (4)";
            QString worldtlpt = "World (1)";
            QString xtlpt = "X (3)";
            QString ztlpt = "Z (3)";
            QString ytlpt = "Y (3)";
            QString facing1 = "Facing (1)";

            QString tlptid2 = IN_Opcodes.CharSelectID;
            QString chosenworld = QString("%1").arg(IN_Opcodes.masterworld,2,16,QLatin1Char('0')).toUpper();
            QString facing = IN_Opcodes.masterf;
            QString x1 = QString::fromUtf8(IN_Opcodes.masterx);
            QString z1 = QString::fromUtf8(IN_Opcodes.masterz);
            QString y1 = QString::fromUtf8(IN_Opcodes.mastery);



            if(chosenworld.isEmpty() == false && facing.isEmpty() == false && x1.isEmpty() == false &&
                    z1.isEmpty() == false && y1.isEmpty() == false)
            {
                db.transaction();

                querytlpt2.prepare(QStringLiteral("UPDATE %1 SET [%2] = :val2,[%3] = :val3,[%4] = :val4,[%5] = :val5,[%6] = :val6 WHERE [%7] = :val").arg(tablesel2).arg(xtlpt).arg(ytlpt).arg(ztlpt).arg(worldtlpt).arg(facing1).arg(objecttlpt2));

                querytlpt2.bindValue(":val",tlptid2);
                querytlpt2.bindValue(":val2",x1.toUpper());
                querytlpt2.bindValue(":val3",y1.toUpper());
                querytlpt2.bindValue(":val4",z1.toUpper());
                querytlpt2.bindValue(":val5",chosenworld.toUpper());
                querytlpt2.bindValue(":val6",facing.toUpper());

                querytlpt2.exec();
                db.commit();
            }

            ///////////////////////////////////////////////////////////





            QMapIterator<QByteArray,struct packetparsing::packetvars> StructIter (packetparsing::IPandPort_AllvariablesMap);
            while(StructIter.hasNext())
            {
                packetparsing::packetvars p99;

                StructIter.next();
                //QByteArray ID_IP_PORT = StructIter.key();
                p99 = StructIter.value();//all other players structs

                if(p99.CharSelectID.isEmpty() == false && p99.flipped == true && IN_Opcodes.CharSelectID != p99.CharSelectID)
                {
                    QString Channel = p99.Map_Channel_ID.key(IN_Opcodes.CharSelectID);

                    if(!objectpacket::DeadListVector.contains(IN_Opcodes.CharSelectID))
                    {
                        objectpacket::DeadListVector.append(IN_Opcodes.CharSelectID);
                    }

                    if(Channel.isEmpty() == false)
                    {
                        if(p99.Map_Channel_ID.contains(Channel))//check if i am in their spawned channels.
                        {
                            QByteArray Combined1 = Channel.toUtf8() + "c9xxxx01100180c800";

                            QByteArray ChanCount = p99.incrementmap.value(Channel);

                            increment otherChannels;
                            QByteArray ChansNewCount = otherChannels.count(ChanCount);
                            Combined1.replace(4,4,ChansNewCount);

                            p99.incrementmap.insert(Channel,ChansNewCount);

                            QByteArray holderGrpOut = "";

                            holderGrpOut.append(Combined1);

                            p99.ResendAllChannelsMap.insert(IN_Opcodes.ServersCheckChannelConfirm,holderGrpOut);

                            IN_Opcodes.OpcodeReturn.append(holderGrpOut);
                            IN_Opcodes.OpcodeReturn.append(p99.CharSelectID.toUtf8() + " Switch_Clients");

                            int index = p99.SortChanelsNumbersVector.indexOf(Channel.toUtf8());
                            p99.SortChanelsValuesVector.replace(index,ChansNewCount);

                            p99.ServersCheckChannelConfirm.clear();

                            for(int re = 0; re < p99.SortChanelsValuesVector.size(); re++)
                            {
                                QByteArray ChannelsValue = p99.SortChanelsValuesVector.at(re);
                                QByteArray Channel = p99.SortChanelsNumbersVector.at(re);

                                if(ChannelsValue != "0000")
                                {
                                    p99.ServersCheckChannelConfirm.append(Channel + ChannelsValue);
                                    p99.SortChanelsValuesVector.replace(re,"0000");
                                }
                            }

                            p99.channelmap.insert(Channel,"free");

                            packetparsing::IPandPort_AllvariablesMap.insert(p99.ClientsID + p99.ClientsIP + p99.ClientsPort,p99);

                            objectpacket Spawning;
                            QByteArray Status = Spawning.packetswitch(p99.ClientsID,p99.ClientsIP,p99.ClientsPort);


                            p99 = packetparsing::IPandPort_AllvariablesMap.value(p99.ClientsID + p99.ClientsIP + p99.ClientsPort);

                            qDebug() << "p99.Map_Channel_ID2" << p99.Map_Channel_ID;

                            int VectSize = p99.ActivateOP.size();

                            if(p99.ActivateOP.isEmpty() == false)// if i am in range and need a channel
                            {
                                for(int ld = 0; ld < p99.ActivateOP.size(); ld++)
                                {
                                    QByteArray Fullc9 = p99.ActivateOP.at(ld);

                                    p99.ResendAllChannelsMap.insert(p99.ServersCheckChannelConfirm,Fullc9);

                                    IN_Opcodes.OpcodeReturn.append(Fullc9);
                                    IN_Opcodes.OpcodeReturn.append(p99.CharSelectID.toUtf8() + "Switch_Clients");
                                }

                                for(int i = 0; i < VectSize; i ++)
                                {
                                    p99.ActivateOP.removeAt(0);
                                }
                            }

                            packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);
                            packetparsing::IPandPort_AllvariablesMap.insert(p99.ClientsID + p99.ClientsIP + p99.ClientsPort,p99);
                        }
                    }
                }

                if(packetparsing::IPandPort_AllvariablesMap.contains(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort))
                {
                    if(IN_Opcodes.GroupID != "00000000")
                    {
                        qDebug() << "Group Logout = true";
                        Logout = true;
                    }
                    else
                    {
                        qDebug() << "Normal Logout1 = true";
                    }
                }
            }



            packetparsing::IPandPort_AllvariablesMap.insert(IN_Opcodes.ClientsID + IN_Opcodes.ClientsIP + IN_Opcodes.ClientsPort,IN_Opcodes);
        }







        if(IN_Opcodes.opcoderesult1 == "2706")//disband group.
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);
            DisbandGroup = true;
            ReformGroup = true;
        }
        //group invite

        if(IN_Opcodes.opcoderesult1 == "2506")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QString SendToID = opholder.mid(12,8).toUpper();

            QString FBsizeOpcode = "";

            QString GroupIsFull = "71070379020000";

            //check if in a group first

            if(IN_Opcodes.GroupID == "00000000")
            {
                IN_Opcodes.GroupID = GroupID;
            }

            QVector<QString> TempGrpVect;

            if(CharCreate::GroupID_PlayersID_NamesVectorMap.contains(IN_Opcodes.GroupID))
            {
                TempGrpVect = CharCreate::GroupID_PlayersID_NamesVectorMap.value(IN_Opcodes.GroupID);
            }

            if(TempGrpVect.size() == 8)//if group is full
            {
                FBsizeOpcode = GroupIsFull;

                QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                ADD_OR_SUB CurrentItem;
                QString NewFB = CurrentItem.FBSize(FBsizeOpcode);

                increment Text;
                QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                int index = NewFB.indexOf("XXXX");
                NewFB.replace(index,4,Text2);
                Servers_Last_FB_MessageNumber = Text2.toUpper();



                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                IN_Opcodes.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,NewFB.toUtf8());

                IN_Opcodes.OpcodeReturn.append(NewFB.toUtf8().toUpper());
                IN_Opcodes.OpcodeReturn.append(IN_Opcodes.CharSelectID.toUtf8() + " Switch_Clients");
            }
            else// group is not full
            {
                QVector<QByteArray> TempVect2 = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(SendToID);

                if(TempVect2.isEmpty() == true)
                {
                    SendToID = Opcodes::ObjectIDsMap.key(SendToID);
                    TempVect2 = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(SendToID);
                }

                QByteArray ClientID_out = TempVect2.at(0);
                QByteArray ClientIP_out = TempVect2.at(1);
                QByteArray ClientPort_out = TempVect2.at(2);


                packetparsing::packetvars pSwitch;

                pSwitch = packetparsing::IPandPort_AllvariablesMap.value(ClientID_out + ClientIP_out + ClientPort_out);//change to other client

                if(pSwitch.GroupID != "00000000")//check if target is already in a group
                {

                    QString NPCsName = checkthezone::NPCsNames.value(SendToID);

                    FBsizeOpcode = NPCsName + " is already in a group.";

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    ADD_OR_SUB SendToText5;
                    QString FBText5 =  SendToText5.TextSize(ClientID,ClientsIP,ClientsPort,FBsizeOpcode,false,"7A0A",Servers_Last_FB_MessageNumber);

                    Servers_Last_FB_MessageNumber = FBText5.mid(0,4).toUtf8();
                    FBText5.remove(0,4);

                    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                    CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                    IN_Opcodes.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,FBText5.toUtf8());

                    IN_Opcodes.OpcodeReturn.append(FBText5.toUtf8().toUpper());
                    IN_Opcodes.OpcodeReturn.append(IN_Opcodes.CharSelectID.toUtf8() + " Switch_Clients");
                }
                else
                {

                    QByteArray Currentc9 = objectpacket::Master_Map.value(IN_Opcodes.CharSelectID.toUtf8());


                    QByteArray GroupLeadersName1 = Currentc9.mid(310,48);
                    QString textout = GroupLeadersName1.fromHex(GroupLeadersName1);
                    QByteArray GroupLeadersName = textout.toUtf8().toHex().toUpper();

                    int NameSize = GroupLeadersName.size() / 2;

                    QString NameSize1 = QString("%1").arg(NameSize,2,16,QLatin1Char('0')).toUpper();

                    FBsizeOpcode = "7307" + IN_Opcodes.GroupID + IN_Opcodes.CharSelectID + NameSize1 + "000000" + GroupLeadersName;

                    //                                        48756C6B73686974
                    //            73079D030C0091B911170800000048756C6B73686974

                    NameSize = opholder.mid(20,2).toInt(nullptr,16) * 2 + 8;



                    QVector<QString> TempVect;

                    if(CharCreate::PendingGroupID_PlayersID_NamesVectorMap.contains(IN_Opcodes.GroupID))
                    {
                        TempVect = CharCreate::PendingGroupID_PlayersID_NamesVectorMap.value(IN_Opcodes.GroupID);
                    }


                    if(!TempVect.contains(IN_Opcodes.CharSelectID))
                    {
                        TempVect.append(IN_Opcodes.CharSelectID);
                        TempVect.append(NameSize1 + "000000" + GroupLeadersName);
                    }

                    TempVect.append(SendToID);
                    TempVect.append(opholder.mid(20,NameSize));

                    CharCreate::PendingGroupID_PlayersID_NamesVectorMap.insert(GroupID,TempVect);

                    QByteArray holderGrpOut = "";

                    IN_Opcodes.OpcodeReturn.append(holderGrpOut);
                    IN_Opcodes.OpcodeReturn.append(IN_Opcodes.CharSelectID.toUtf8() + " Switch_Clients");

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
                    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);



                    holderGrpOut = "";

                    ADD_OR_SUB CurrentItem;
                    QString NewFB = CurrentItem.FBSize(FBsizeOpcode);

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(pSwitch.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    increment Text;
                    QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                    int index = NewFB.indexOf("XXXX");
                    NewFB.replace(index,4,Text2);
                    Servers_Last_FB_MessageNumber = Text2.toUpper();

                    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                    CharID_ServerMessageNumbers.insert(pSwitch.CharSelectID,MessNumbVect);


                    holderGrpOut.append(NewFB.toUtf8());

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID_out + ClientIP_out + ClientPort_out,pSwitch);

                    QVector<QByteArray> tempVect = Opcodes::CharID_CLientID_IP_PORT_Map.value(SendToID);
                    QByteArray ClientsID = tempVect.at(0);
                    QByteArray ClientsIP = tempVect.at(1);
                    QByteArray ClientsPort = tempVect.at(2);

                    packetparsing::packetvars PSWAP = packetparsing::IPandPort_AllvariablesMap.value(ClientsID + ClientsIP + ClientsPort);


                    PSWAP.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,NewFB.toUtf8());
                    packetparsing::IPandPort_AllvariablesMap.insert(ClientsID + ClientsIP + ClientsPort,PSWAP);

                    IN_Opcodes.OpcodeReturn.append(holderGrpOut);
                    IN_Opcodes.OpcodeReturn.append(SendToID.toUtf8() + " Switch_Clients");
                }
            }


            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }



        if(IN_Opcodes.opcoderesult1 == "2206" || IN_Opcodes.opcoderesult1 == "2306" || Logout == true)//Accept invite/Leave group
        {
            if(Logout == false)
            {
                IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);
            }

            QByteArray holderGrpOut = "";

            bool FullGroup = false;
            bool GroupInviteExpired = false;

            if(IN_Opcodes.opcoderesult1 == "2206")//Accept group
            {
                QString GroupID = opholder.mid(12,8);

                QVector<QString> TempGrpVect;

                qDebug() << "IN_Opcodes.GroupID_1" << GroupID;

                if(CharCreate::GroupID_PlayersID_NamesVectorMap.contains(GroupID))
                {
                    TempGrpVect = CharCreate::GroupID_PlayersID_NamesVectorMap.value(GroupID);

                    qDebug() << "IN_Opcodes.TempGrpVect.size()" << TempGrpVect.size();

                    if(TempGrpVect.size() < 8)
                    {
                        IN_Opcodes.GroupID = GroupID;
                        CharCreate::PlayersID_GroupID_Map.insert(IN_Opcodes.CharSelectID,IN_Opcodes.GroupID);
                    }
                    else
                    {
                        QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                        QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                        QString Append_FB_Opcode = "The group is already full.";

                        ADD_OR_SUB SendToText5;
                        QString FBText5 =  SendToText5.TextSize(IN_Opcodes.ClientsID,IN_Opcodes.ClientsIP,IN_Opcodes.ClientsPort,Append_FB_Opcode,false,"7A0A",Servers_Last_FB_MessageNumber);


                        Servers_Last_FB_MessageNumber = FBText5.mid(0,4).toUtf8();
                        FBText5.remove(0,4);

                        MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                        CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                        holderGrpOut.append(FBText5.toUtf8());

                        FullGroup = true;
                    }
                }
                else
                {
                    if(CharCreate::PendingGroupID_PlayersID_NamesVectorMap.contains(GroupID))
                    {
                        IN_Opcodes.GroupID = GroupID;

                        qDebug() << "IN_Opcodes.GroupID_2" << GroupID;

                    }
                    else
                    {
                        QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);

                        qDebug() << "IN_Opcodes.MessNumbVect_1.size()" << MessNumbVect.size();

                        QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                        QString Append_FB_Opcode = "The group invitation has expired.";

                        ADD_OR_SUB SendToText5;
                        QString FBText5 =  SendToText5.TextSize(IN_Opcodes.ClientsID,IN_Opcodes.ClientsIP,IN_Opcodes.ClientsPort,Append_FB_Opcode,false,"7A0A",Servers_Last_FB_MessageNumber);


                        Servers_Last_FB_MessageNumber = FBText5.mid(0,4).toUtf8();
                        FBText5.remove(0,4);

                        MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                        CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                        holderGrpOut.append(FBText5.toUtf8());

                        GroupInviteExpired = true;
                    }
                }
            }


            if(IN_Opcodes.opcoderesult1 == "2306" || Logout == true)//Leave Group
            {
                DisbandGroup = false;
                Append_FB_OpcodeVector.clear();

                QString ThisClientsCharSelectID = IN_Opcodes.CharSelectID;


                QString GroupID = CharCreate::PlayersID_GroupID_Map.value(ThisClientsCharSelectID);

                QVector<QString> TempGrpVect = CharCreate::GroupID_PlayersID_NamesVectorMap.value(GroupID);

                int index = TempGrpVect.indexOf(IN_Opcodes.CharSelectID);

                if(index != -1)
                {
                    TempGrpVect.removeAt(index);
                    TempGrpVect.removeAt(index);
                }

                QVector<QString> TempGrpVect2 = CharCreate::PendingGroupID_PlayersID_NamesVectorMap.value(GroupID);

                if(TempGrpVect2.contains(IN_Opcodes.CharSelectID))
                {
                    int index = TempGrpVect2.indexOf(IN_Opcodes.CharSelectID);

                    if(index != -1)
                    {
                        TempGrpVect2.removeAt(index);
                        TempGrpVect2.removeAt(index);
                    }

                    CharCreate::PendingGroupID_PlayersID_NamesVectorMap.insert(GroupID,TempGrpVect2);
                }

                CharCreate::GroupID_PlayersID_NamesVectorMap.insert(GroupID,TempGrpVect);

                if(TempGrpVect.size() >= 4)
                {

                    QString NPCsName = checkthezone::NPCsNames.value(ThisClientsCharSelectID);
                    QString TextFull5 = NPCsName + " has left the group.";

                    Append_FB_OpcodeVector.append(TextFull5);//figure out why no name

                    if(index == 0)
                    {
                        QString NewLeaderID = TempGrpVect.at(0);
                        QString NPCsName = checkthezone::NPCsNames.value(NewLeaderID);

                        QString TextFull5 = NPCsName + " is the new group leader.";

                        Append_FB_OpcodeVector.append(TextFull5);
                    }
                }
                else
                {
                    DisbandGroup = true;
                }

                if(Logout == false)
                {
                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    QString Append_FB_Opcode = "You have left the group.";

                    ADD_OR_SUB SendToText5;
                    QString FBText5 =  SendToText5.TextSize(IN_Opcodes.ClientsID,IN_Opcodes.ClientsIP,IN_Opcodes.ClientsPort,Append_FB_Opcode,false,"7A0A",Servers_Last_FB_MessageNumber);


                    Servers_Last_FB_MessageNumber = FBText5.mid(0,4).toUtf8();
                    FBText5.remove(0,4);

                    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                    CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                    holderGrpOut.append(FBText5.toUtf8());

                }
            }

            if(Logout == false && holderGrpOut.isEmpty() == false)
            {
                QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);

                qDebug() << "IN_Opcodes.MessNumbVect_2.size()" << MessNumbVect.size();

                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                IN_Opcodes.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,holderGrpOut);

                IN_Opcodes.OpcodeReturn.append(holderGrpOut);
                IN_Opcodes.OpcodeReturn.append(IN_Opcodes.CharSelectID.toUtf8() + " Switch_Clients");
            }

            if(Logout == true)
            {
                Logout = false;
            }

            if(FullGroup == false && GroupInviteExpired == false)
            {
                ReformGroup = true;

                qDebug() << "IN_Opcodes.ReformGroup = " << ReformGroup;
            }

            //file.close();
            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
        }

        //leave group








        if(ReformGroup == true)
        {
            ReformGroup = false;

            QVector<QString> TempVect = CharCreate::PendingGroupID_PlayersID_NamesVectorMap.value(IN_Opcodes.GroupID);

            qDebug() << "IN_Opcodes.ReformGroup.TempVect.size() = " << TempVect.size();

            int index = 0;
            QString GroupLeader = "";
            QString GroupLeadersName = "";
            QString GroupIDsString = "";
            QString NextGroupMember = "";
            QString NextGroupMembersName = "";
            QString GroupNamesString = "";
            QString FBsizeOpcode = "";
            QVector<QString> TempGrpVect;
            QVector<QString> OpcodeVector;
            QVector<QString> GroupMemeberIDsVector;

            if(!CharCreate::GroupID_PlayersID_NamesVectorMap.contains(IN_Opcodes.GroupID))//new group started
            {
                if(TempVect.isEmpty() == false)//new group started
                {
                    GroupLeader = TempVect.at(0);
                    GroupLeadersName = TempVect.at(1);

                    TempGrpVect.append(GroupLeader);
                    TempGrpVect.append(GroupLeadersName);

                    GroupMemeberIDsVector.append(GroupLeader);

                    TempVect.removeAt(0);
                    TempVect.removeAt(0);

                    if(TempVect.contains(IN_Opcodes.CharSelectID))
                    {
                        index = TempVect.indexOf(IN_Opcodes.CharSelectID);

                        NextGroupMember = TempVect.at(index);
                        NextGroupMembersName = TempVect.at(index + 1);

                        TempGrpVect.append(NextGroupMember);
                        TempGrpVect.append(NextGroupMembersName);

                        GroupMemeberIDsVector.append(NextGroupMember);

                        TempVect.removeAt(index);
                        TempVect.removeAt(index);
                    }
                }

                qDebug() << "IN_Opcodes.ReformGroup.GroupMemeberIDsVector_1.size() = " << GroupMemeberIDsVector.size();

                int TempGrpVect2Size = GroupMemeberIDsVector.size();
                QString TempGrpVect2Size1 = QString("%1").arg(TempGrpVect2Size,2,16,QLatin1Char('0')).toUpper();

                FBsizeOpcode = "7207" +
                        IN_Opcodes.GroupID + TempGrpVect2Size1 + "000000" +
                        GroupLeader + NextGroupMember +
                        GroupLeadersName + NextGroupMembersName;

                //                         4D697261 0000000000000000000000000000000000000000
                // 72079D030C000200000091B9121791B90B170A0000004D69737477616C6B6572040000004D697261
                for(int i = 0; i < GroupMemeberIDsVector.size(); i ++)
                {
                    OpcodeVector.append(FBsizeOpcode);
                    CharCreate::PendingGroupID_PlayersID_NamesVectorMap.insert(IN_Opcodes.GroupID,TempVect);
                    CharCreate::GroupID_PlayersID_NamesVectorMap.insert(IN_Opcodes.GroupID,TempGrpVect);
                }


                qDebug() << "IN_Opcodes.ReformGroup.OpcodeVector_1.size() = " << OpcodeVector.size();
            }
            else
            {
                QVector<QString> TempGrpVect = CharCreate::GroupID_PlayersID_NamesVectorMap.value(IN_Opcodes.GroupID);

                qDebug() << "IN_Opcodes.ReformGroup.TempGrpVect.size() = " << TempGrpVect.size();

                QVector<QString> TempGrpVect2;

                for(int grp = 0; grp < TempGrpVect.size(); grp+=2)
                {
                    GroupIDsString.append(TempGrpVect.at(grp));
                    GroupNamesString.append(TempGrpVect.at(grp + 1));

                    TempGrpVect2.append(TempGrpVect.at(grp));
                    TempGrpVect2.append(TempGrpVect.at(grp + 1));

                    GroupMemeberIDsVector.append(TempGrpVect.at(grp));
                }


                if(TempVect.contains(IN_Opcodes.CharSelectID))
                {
                    index = TempVect.indexOf(IN_Opcodes.CharSelectID);

                    GroupIDsString.append(TempVect.at(index));
                    GroupNamesString.append(TempVect.at(index + 1));

                    TempGrpVect2.append(TempVect.at(index));
                    TempGrpVect2.append(TempVect.at(index + 1));

                    GroupMemeberIDsVector.append(TempVect.at(index));

                    TempVect.removeAt(index);
                    TempVect.removeAt(index);
                }

                int TempGrpVect2Size = GroupMemeberIDsVector.size();
                qDebug() << "IN_Opcodes.ReformGroup.GroupMemeberIDsVector_2.size() = " << GroupMemeberIDsVector.size();

                QString TempGrpVect2Size1 = QString("%1").arg(TempGrpVect2Size,2,16,QLatin1Char('0')).toUpper();

                FBsizeOpcode = "7207" +
                        IN_Opcodes.GroupID + TempGrpVect2Size1 + "000000" +
                        GroupIDsString +
                        GroupNamesString;

                for(int i = 0; i < GroupMemeberIDsVector.size(); i ++)
                {
                    OpcodeVector.append(FBsizeOpcode);
                }

                qDebug() << "IN_Opcodes.ReformGroup.OpcodeVector_2.size() = " << OpcodeVector.size();

                CharCreate::PendingGroupID_PlayersID_NamesVectorMap.insert(IN_Opcodes.GroupID,TempVect);
                CharCreate::GroupID_PlayersID_NamesVectorMap.insert(IN_Opcodes.GroupID,TempGrpVect2);
            }

            QString DisGroupID = IN_Opcodes.GroupID;

            if(!GroupMemeberIDsVector.contains(IN_Opcodes.CharSelectID))
            {
                IN_Opcodes.GroupID = "00000000";
            }

            for(int grp = 0; grp < GroupMemeberIDsVector.size(); grp++)
            {
                QString SendToID = GroupMemeberIDsVector.at(grp).toUtf8();

                QByteArray holderGrpOut = "";
                QByteArray GrpRnk = "";

                QByteArray ClientID_out = "";
                QByteArray ClientIP_out = "";
                QByteArray ClientPort_out = "";

                packetparsing::packetvars pSwitch;

                QVector<QByteArray> TempVectOut = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(SendToID);

                qDebug() << "IN_Opcodes.ReformGroup.TempVectOut.size() = " << TempVectOut.size();


                if(TempVectOut.isEmpty() == true)
                {
                    SendToID = Opcodes::ObjectIDsMap.key(SendToID);
                    TempVectOut = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(SendToID);
                }

                if(TempVectOut.isEmpty() == false)
                {
                    ClientID_out = TempVectOut.at(0);
                    ClientIP_out = TempVectOut.at(1);
                    ClientPort_out = TempVectOut.at(2);

                    QMap<int,QByteArray> GroupRanks =
                    {
                        {0,"Group Leader "},{1,"Group Member 1 "},{2,"Group Member 2 "},{3,"Group Member 3 "}
                    };

                    GrpRnk = GroupRanks.value(grp);

                    pSwitch = packetparsing::IPandPort_AllvariablesMap.value(ClientID_out + ClientIP_out + ClientPort_out);//change to other client

                    qDebug() << "IN_Opcodes.ReformGroup.OpcodeVector_3.size() = " << OpcodeVector.size();

                    FBsizeOpcode = OpcodeVector.at(grp);

                    QString DisbandOpcode = "";

                    if(DisbandGroup == true)
                    {
                        FBsizeOpcode = FBsizeOpcode.mid(0,28);
                        FBsizeOpcode.replace(12,2,"01");
                        DisbandOpcode = "71070377020000";
                    }

                    if(FBsizeOpcode.contains(pSwitch.CharSelectID))
                    {
                        int index1 = FBsizeOpcode.indexOf(pSwitch.CharSelectID);

                        FBsizeOpcode.replace(index1,8,IN_Opcodes.CharSelectID_Main);
                    }

                    ADD_OR_SUB CurrentItem;
                    QString NewFB = CurrentItem.FBSize(FBsizeOpcode);

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(pSwitch.CharSelectID);

                    qDebug() << "IN_Opcodes.ReformGroup.MessNumbVect_3.size() = " << MessNumbVect.size();

                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    increment Text;
                    QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                    int index = NewFB.indexOf("XXXX");
                    NewFB.replace(index,4,Text2);
                    Servers_Last_FB_MessageNumber = Text2.toUpper();

                    holderGrpOut.append(NewFB.toUtf8());

                    if(DisbandOpcode.isEmpty() == false)
                    {
                        QString NewFB = CurrentItem.FBSize(DisbandOpcode);

                        increment Text;
                        QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                        int index = NewFB.indexOf("XXXX");
                        NewFB.replace(index,4,Text2);
                        Servers_Last_FB_MessageNumber = Text2.toUpper();

                        holderGrpOut.append(NewFB.toUtf8());
                    }

                    QByteArray GroupUpdate = "";
                    QByteArray Servers_400D = MessNumbVect.at(2);

                    QByteArray IDsXZYs = "";
                    QString FBText5 = "";

                    // ////////////////Send Name Bars/////////////////////

                    int BarSize = (GroupMemeberIDsVector.size() - 1) * 0x0D;
                    QByteArray BarSize1 = QString("%1").arg(BarSize,2,16,QLatin1Char('0')).toUtf8().toUpper();

                    QString out = "00";
                    QString End40 = "01FF007A435E00733B";


                    if(DisbandGroup == true)
                    {
                        BarSize1 = "00";
                        out = "01";
                        End40 = "000000000000000000";
                    }

                    GroupUpdate = "40" + BarSize1;

                    increment GrpOut;
                    QByteArray GrpOut_a = GrpOut.count(Servers_400D);

                    GroupUpdate.append(GrpOut_a + out);

                    Servers_400D = GrpOut_a.toUpper();

                    MessNumbVect.replace(2,Servers_400D);

                    for(int grp = 0; grp < GroupMemeberIDsVector.size(); grp++)
                    {
                        QByteArray GroupIDs = GroupMemeberIDsVector.at(grp).toUtf8();

                        if(GroupIDs != pSwitch.CharSelectID.toUtf8())
                        {
                            IDsXZYs.append(GroupIDs + End40);
                        }
                    }

                    if(IDsXZYs.isEmpty() == false)
                    {
                        xorpacket packet2;
                        QByteArray Recompressed2 = packet2.packetencrypt(IDsXZYs).toUpper();

                        GroupUpdate.append(Recompressed2);
                        holderGrpOut.append(GroupUpdate);
                    }

                    // ////////////////Send Name Bars/////////////////////

                    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                    CharID_ServerMessageNumbers.insert(pSwitch.CharSelectID,MessNumbVect);

                    if(Append_FB_OpcodeVector.isEmpty() == false)
                    {
                        for(int i = 0; i < Append_FB_OpcodeVector.size(); i++)
                        {
                            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(pSwitch.CharSelectID);

                            qDebug() << "IN_Opcodes.ReformGroup.MessNumbVect_4.size() = " << MessNumbVect.size();

                            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                            QString Append_FB_Opcode = Append_FB_OpcodeVector.at(i);

                            QString NPCsName = checkthezone::NPCsNames.value(pSwitch.CharSelectID);

                            if(Append_FB_Opcode.contains(NPCsName))
                            {
                                Append_FB_Opcode.replace(0,NPCsName.size() + 4,"You are ");
                            }

                            ADD_OR_SUB SendToText5;
                            FBText5 =  SendToText5.TextSize(pSwitch.ClientsID,pSwitch.ClientsIP,pSwitch.ClientsPort,Append_FB_Opcode,false,"7A0A",Servers_Last_FB_MessageNumber);

                            Servers_Last_FB_MessageNumber = FBText5.mid(0,4).toUtf8();
                            FBText5.remove(0,4);

                            MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                            CharID_ServerMessageNumbers.insert(pSwitch.CharSelectID,MessNumbVect);

                            holderGrpOut.append(FBText5.toUtf8());
                        }
                    }
                }

                if(holderGrpOut.isEmpty() == false)
                {
                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(SendToID);

                    qDebug() << "IN_Opcodes.ReformGroup.MessNumbVect_5.size() = " << MessNumbVect.size();

                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    pSwitch.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,holderGrpOut);

                    IN_Opcodes.OpcodeReturn.append(holderGrpOut);
                    IN_Opcodes.OpcodeReturn.append(pSwitch.CharSelectID.toUtf8() + " Switch_Clients " + GrpRnk);

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID_out + ClientIP_out + ClientPort_out,pSwitch);
                }
            }


            if(DisbandGroup == true)
            {
                if(CharCreate::PendingGroupID_PlayersID_NamesVectorMap.contains(DisGroupID))
                {
                    CharCreate::PendingGroupID_PlayersID_NamesVectorMap.remove(DisGroupID);
                }

                if(CharCreate::GroupID_PlayersID_NamesVectorMap.contains(DisGroupID))
                {
                    CharCreate::GroupID_PlayersID_NamesVectorMap.remove(DisGroupID);
                }
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }














        if(IN_Opcodes.opcoderesult1 == "0F00" || IN_Opcodes.opcoderesult1 == "D800")
        {


            qDebug() << "IN_Opcodes.OldTarget_ID1" << IN_Opcodes.OldTarget_ID;

            if(IN_Opcodes.opcoderesult1 == "0F00" && IN_Opcodes.OpcodesAndGamebuf1Map.contains("D800"))
            {
                IN_Opcodes.OpcodesAndGamebuf1Map.remove("0F00");
            }
            else
            {

                if(IN_Opcodes.opcoderesult1 == "D800")
                {
                    IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

                    QByteArray holderD800 = "d37c5ae792c001bf543213" + IN_Opcodes.SessionID + "03xxxxXXXXxxxx";

                    holderD800.replace(0,4,IN_Opcodes.serverid1);
                    holderD800.replace(4,4,IN_Opcodes.clientid1);


                    holderD800.replace(34,4,Clients_Last_MessageNumber);
                    holderD800.replace(38,4,Clients_Last_FB_MessageNumber);

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    increment GrpOut;
                    QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                    holderD800.replace(30,4,GrpOut_a);
                    Servers_Last_MessageNumber = GrpOut_a.toUpper();

                    MessNumbVect.replace(0,Servers_Last_MessageNumber);

                    CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                    packetsize ResizePacket;
                    QByteArray ResizePacket2 = ResizePacket.switchsize(holderD800);
                    holderD800.replace(8,4,ResizePacket2);

                    IN_Opcodes.ResendFBsMap.insert(Servers_Last_FB_MessageNumber,holderD800);

                    IN_Opcodes.OpcodeReturn.append(holderD800);
                    IN_Opcodes.OpcodeReturn.append("Target_Out_Of_Range ");
                }
                else
                {

                    if(!generator::Players_Map.contains(IN_Opcodes.OldTarget_ID))
                    {

                        if(IN_Opcodes.opcoderesult1 == "0F00")
                        {
                            IN_Opcodes.OldTarget_ID = opholder.mid(12,8);

                            IN_Opcodes.Reply = "03";
                            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);
                        }

                        QVector <QString> MobValuesVector;
                        QVector <QString> MyValuesVector;



                        if(IN_Opcodes.returnedcolor == "05")
                        {
                            IN_Opcodes.IsTargetGreen = "YES";
                        }
                        else
                        {
                            IN_Opcodes.IsTargetGreen = "NO";
                        }

                        //////////////////////////////////////////////////////////////////////
                        QByteArray current_c9 = generator::Players_Map.value(IN_Opcodes.CharSelectID.toUtf8());
                        QString MyLevel1 = current_c9.mid(358,2);//level

                        if(!IN_Opcodes.CombatAllValuesMap.contains(IN_Opcodes.CharSelectID_Main))
                        {


                            QString MyObjectID = current_c9.mid(2,8);
                            QString MyC9HP = current_c9.mid(54,2);//class
                            int MyC9HP1 = MyC9HP.toInt(nullptr,16);
                            MyC9HP = QString("%1").arg(MyC9HP1,1,10,QLatin1Char('0')).toUpper();

                            IN_Opcodes.AtackType = current_c9.mid(182,4);

                            QString MyClass1 = current_c9.mid(368,2);//class


                            QString MySwingType1 = QString("%1").arg(IN_Opcodes.MySwingType,1,10,QLatin1Char('0')).toUpper();
                            MyValuesVector.append(MySwingType1);

                            QString MySwing1 = QString("%1").arg(IN_Opcodes.MySwingType,1,10,QLatin1Char('0')).toUpper();
                            MyValuesVector.append(MySwing1);

                            MyValuesVector.append(MyLevel1);
                            MyValuesVector.append(MyClass1);

                            QString MyOldHP0 = IN_Opcodes.StatsChange.mid(56,4);
                            QString Byte1 = MyOldHP0.mid(0,2);
                            QString Byte2 = MyOldHP0.mid(2,2);
                            QString MyOldHP2 = Byte2 + Byte1;
                            int MyOldHP = MyOldHP2.toInt(nullptr,16);
                            QString MyOldHP1 = QString("%1").arg(MyOldHP,1,10,QLatin1Char('0')).toUpper();

                            MyValuesVector.append(MyOldHP1);//MyHP

                            QString MyMaxHP0 = IN_Opcodes.StatsChange.mid(64,4);
                            Byte1 = MyMaxHP0.mid(0,2);
                            Byte2 = MyMaxHP0.mid(2,2);
                            QString MyMaxHP2 = Byte2 + Byte1;
                            int MyMaxHP = MyMaxHP2.toInt(nullptr,16);
                            QString MyMaxHP1 = QString("%1").arg(MyMaxHP,1,10,QLatin1Char('0')).toUpper();

                            MyValuesVector.append(MyMaxHP1);//MyMaxHP

                            MyValuesVector.append(MyC9HP);//MyFFhp

                            IN_Opcodes.CombatAllValuesMap.insert(IN_Opcodes.CharSelectID_Main,MyValuesVector);
                        }
                        else
                        {
                            for(int cc = 0; cc < MainWindow::MySwingTimersVector.size(); cc++) //remove main toon first
                            {
                                auto* receivedtimer2 = MainWindow::MySwingTimersVector.at(cc);

                                QString  ObjectID = receivedtimer2->objectName();

                                if(ObjectID == IN_Opcodes.CharSelectID_Main)
                                {
                                    if(MainWindow::MySwingTimersVector.contains(receivedtimer2))
                                    {
                                        receivedtimer2->stop();
                                        receivedtimer2->deleteLater();
                                        MainWindow::MySwingTimersVector.removeAt(cc);
                                    }
                                }
                            }
                        }

                        IN_Opcodes.MyDamageTimer = "250";

                        //////////////////////////////////////////////////////////////////////

                        //////////////////////////////////////////////////////////////////////

                        MainWindow::MobsAggroTargetMap.insert(IN_Opcodes.OldTarget_ID,IN_Opcodes.CharSelectID);

                        current_c9 = objectpacket::Master_Map.value(IN_Opcodes.OldTarget_ID);

                        QString MobObjectID = current_c9.mid(2,8);

                        if(!IN_Opcodes.CombatAllValuesMap.contains(MobObjectID))
                        {
                            QByteArray  myx1 = IN_Opcodes.masterx; //send as 3 byte hex
                            int xfromhex1 = myx1.toInt(nullptr,16);
                            //      qDebug() <<"MainToon-X-Decimal" << xfromhex1;
                            float maintoonx1 = xfromhex1 / 128.0;

                            QByteArray  myy1 = IN_Opcodes.mastery;
                            int yfromhex1 = myy1.toInt(nullptr,16);
                            //        qDebug() <<"MainToon-Y-Decimal" << yfromhex1;
                            float maintoony1 = yfromhex1 / 128.0;



                            QByteArray  myz1 = IN_Opcodes.masterz;
                            int zfromhex1 = myz1.toInt(nullptr,16);
                            //        qDebug() <<"MainToon-Z-Decimal" << zfromhex1;
                            float maintoonz1 = zfromhex1 / 128.0;

                            if(maintoonz1 > 130000)
                            {
                                maintoonz1 = maintoonz1 - 131072.00195193;
                            }

                            float result = 0.0;

                            QStringList holdid;
                            QStringList holdx;
                            QStringList holdz;
                            QStringList holdy;

                            if(IN_Opcodes.holdid2.contains(IN_Opcodes.OldTarget_ID))
                            {
                                holdid = IN_Opcodes.holdid2;
                                holdx = IN_Opcodes.holdx2;
                                holdz = IN_Opcodes.holdz2;
                                holdy = IN_Opcodes.holdy2;
                            }
                            else
                            {
                                holdid = IN_Opcodes.holdid1;
                                holdx = IN_Opcodes.holdx;
                                holdz = IN_Opcodes.holdz;
                                holdy = IN_Opcodes.holdy;
                            }

                            if(holdid.contains(IN_Opcodes.OldTarget_ID))
                            {
                                int Index = holdid.indexOf(IN_Opcodes.OldTarget_ID);

                                QString tempstrx = holdx.at(Index);
                                int tempint1 = tempstrx.toInt(nullptr,16);
                                float xinterlude = tempint1 / 128.0;

                                QString tempstry = holdy.at(Index);
                                int tempint2 = tempstry.toInt(nullptr,16);
                                float yinterlude = tempint2 / 128.0;

                                QString tempstrz = holdz.at(Index);
                                int tempint3 = tempstrz.toInt(nullptr,16);
                                float  zinterlude = tempint3 / 128.0;

                                if(zinterlude > 130000)
                                {
                                    zinterlude = zinterlude - 131072.00195193;
                                }

                                float sub1 = maintoonx1 - xinterlude;
                                float sub2 = maintoony1 - yinterlude;
                                float sub3 = maintoonz1 - zinterlude;

                                float power1 = qPow(sub1,2);
                                float power2 = qPow(sub2,2);
                                float power3 = qPow(sub3,2);

                                float firstresult = power1 + power2 + power3;
                                result = qSqrt(firstresult);
                            }

                            IN_Opcodes.MySwingRadius = result;

                            qDebug() << "MySwingRadius1" << IN_Opcodes.MySwingRadius;

                            if(IN_Opcodes.MySwingRadius <= 4.5)
                            {
                                QString MobX = current_c9.mid(12,6);
                                QString MobZ = current_c9.mid(18,6);
                                QString MobY = current_c9.mid(24,6);
                                QString MobFacing = current_c9.mid(30,2);
                                QString MobAnimation = current_c9.mid(116,2);
                                QString MobAtackType = current_c9.mid(182,4);
                                QString MobLevel1 = current_c9.mid(358,2);//level
                                int MobLevel2 = MobLevel1.toInt(nullptr,16);
                                QString MobClass1 = current_c9.mid(368,2);//class

                                //add MobAnimation here

                                QString MobStr1 = "";
                                QString MobSta1 = "";
                                QString MobDex1 = "";

                                int SwingType = ClassSwingCountMap.value(MobClass1);

                                if(SwingType > 1 )
                                {
                                    if((SwingType == 4 && MobLevel2 >= 20 && MobLevel2 < 35))//too low to quad
                                    {
                                        SwingType = 2;//downgrade to 2 hits
                                    }

                                    if(MobLevel2 < 20)// downgrade to 1 hit if less than level 20
                                    {
                                        SwingType = 1;
                                    }
                                }


                                QString MobSwingType1 = QString("%1").arg(SwingType,1,10,QLatin1Char('0')).toUpper();
                                MobValuesVector.append(MobSwingType1);

                                QString MobSwing1 = QString("%1").arg(SwingType,1,10,QLatin1Char('0')).toUpper();
                                MobValuesVector.append(MobSwing1);

                                MobValuesVector.append(MobLevel1);
                                MobValuesVector.append(MobClass1);

                                Combat AllValues;

                                QString MobHP = AllValues.SendToMobHP(ClientID,ClientsIP,ClientsPort,MobLevel1, MobClass1);
                                MobValuesVector.append(MobHP);//MobHP
                                MobValuesVector.append(MobHP);//MobMaxHP
                                MobValuesVector.append("255");//MobFFhp
                                QString MobXP = AllValues.SendToXP(ClientID,ClientsIP,ClientsPort,MyLevel1, MobLevel1, IN_Opcodes.IsTargetGreen);
                                MobValuesVector.append(MobXP);

                                MobValuesVector.append(MobAtackType);
                                MobValuesVector.append(MobX);
                                MobValuesVector.append(MobZ);
                                MobValuesVector.append(MobY);
                                MobValuesVector.append(MobFacing);


                                //////////////////////////////////////////////////////////////////////


                                IN_Opcodes.MobDamageTimer = "880" + IN_Opcodes.OldTarget_ID;

                                IN_Opcodes.CombatAllValuesMap.insert(MobObjectID,MobValuesVector);

                                if(objectpacket::Roamers_List.contains(IN_Opcodes.OldTarget_ID))
                                {
                                    objectpacket::Roamers_List.insert(IN_Opcodes.OldTarget_ID,"Combat");
                                    FacingDirection::Path_CounterMap.remove(IN_Opcodes.OldTarget_ID);
                                    FacingDirection::Facing_Lock2_Map.remove(IN_Opcodes.OldTarget_ID);
                                }
                            }
                        }
                    }
                }
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);


            qDebug() << "IN_Opcodes.OldTarget_ID2" << IN_Opcodes.OldTarget_ID;
        }





        //loot window FB060B0015008BED0000
        if(IN_Opcodes.opcoderesult1 == "1500" || IN_Opcodes.opcoderesult1 == "1600"  || IN_Opcodes.opcoderesult1 == "1900"  || IN_Opcodes.opcoderesult1 == "1A00")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            //FB0A0C001900 00000000 02000000
            QString MobsID = opholder.mid(12,8);
            QString LootSlot = opholder.mid(12,4);
            QString SlotNumber = opholder.mid(12,8);

            QString Quantity = opholder.mid(20,2);
            QString ItemsID = "";

            qDebug() << "IN_Opcodes.opcoderesult1" << IN_Opcodes.opcoderesult1;
            qDebug() << "opholder" << opholder;

            QByteArray HolderLootWindow = "";
            QString FBsizeOpcode = "";
            QString openOrnot = "";

            QString Current_channel = IN_Opcodes.Map_Channel_ID.key(IN_Opcodes.Target_ID);

            qDebug() << "Current_channel" << Current_channel;

            QVector<QString> LootVect;
            QVector<QString> LootSlotVect;


            if(Current_channel.isEmpty() == false)
            {


                if(IN_Opcodes.opcoderesult1 == "1500")
                {
                    if(IN_Opcodes.isitOpen == true)
                    {
                        openOrnot = "true";
                    }
                    else
                    {
                        openOrnot = "false";
                    }


                    if(IN_Opcodes.isitOpen == false)
                    {
                        qDebug() << "IN_Opcodes.isitOpen" << IN_Opcodes.isitOpen;

                        IN_Opcodes.isitOpen = true;

                        QString NumberOfItems = "00";
                        QString ItemsInfo = "";

                        //Send MobsID to Loot Class and return the items to ItemsInfo
                        // and NumberOfItems

                        QVector<QString> GlobalLootVector =
                        {
                            "DE81808000","EA81808000","8082808000","C882808000",
                            "D282808000","EA82808000","C483808000","B684808000",
                            "C684808000","EE84808000","9685808000","92BD808000",
                            "94BD808000","96BD808000","B2F2808000","B4F2808000",
                            "B6F2808000","8CF4808000"
                        };


                        std::uniform_int_distribution <int> distribution2(0, 17);
                        int RandomLootNumber1 = distribution2(*QRandomGenerator::global());

                        QVector<QString> Items_IDvect;
                        int slotCounter = 0;
                        QMap<int,QString> tempMap;

                        if(MobIDs_ItemIDs_Map.contains(MobsID))
                        {
                            if(!IN_Opcodes.MobsID_LootVectorMap.contains(MobsID))
                            {
                                Items_IDvect = MobIDs_ItemIDs_Map.value(MobsID);

                                qDebug() << "IN_Opcodes.Items_IDvect" << Items_IDvect;

                                for(int add = 0; add < Items_IDvect.size(); add++)
                                {
                                    QString Items_ID = Items_IDvect.at(add);

                                    qDebug() << "IN_Opcodes.Items_ID" << Items_ID;

                                    QVector<QString> AntVect =
                                    {
                                        "43321000",
                                        "1E341000",
                                        "D8361000",
                                        "DE361000"
                                    };

                                    Quantity = "02";

                                    if(AntVect.contains(MobsID))
                                    {
                                        Quantity = "04";
                                    }

                                    if(Items_ID == "D4F2808000")
                                    {
                                        Quantity = "06";
                                    }


                                    LootVect.append(Items_ID);
                                    LootVect.append(Quantity);
                                    LootSlotVect.append(Items_ID);

                                    tempMap.insert(slotCounter,Items_ID);
                                    slotCounter++;
                                }

                                QString Items_ID = GlobalLootVector.at(RandomLootNumber1);

                                Quantity = "02";
                                LootVect.append(Items_ID);
                                LootVect.append(Quantity);
                                LootSlotVect.append(Items_ID);

                                tempMap.insert(slotCounter,Items_ID);
                                slotCounter++;

                                std::uniform_int_distribution <int> distribution2(0, 17);
                                int RandomLootNumber2 = distribution2(*QRandomGenerator::global());

                                if(RandomLootNumber1 == RandomLootNumber2)
                                {
                                    do
                                    {
                                        std::uniform_int_distribution <int> distribution2(0, 17);
                                        RandomLootNumber2 = distribution2(*QRandomGenerator::global());

                                    }while(RandomLootNumber1 == RandomLootNumber2);

                                }

                                Items_ID = GlobalLootVector.at(RandomLootNumber2);

                                Quantity = "02";
                                LootVect.append(Items_ID);
                                LootVect.append(Quantity);
                                LootSlotVect.append(Items_ID);

                                tempMap.insert(slotCounter,Items_ID);

                                IN_Opcodes.MobsID_LootVectorMap.insert(MobsID,LootVect);
                                IN_Opcodes.MobsID_LootSlotVectorMap.insert(MobsID,LootSlotVect);

                                IN_Opcodes.MobsID_Slot_ItemsID_MapOfMaps.insert(MobsID,tempMap);
                            }
                        }



                        IN_Opcodes.FakeLootVector = IN_Opcodes.MobsID_LootVectorMap.value(MobsID);
                        IN_Opcodes.FakeLootSlotVector = IN_Opcodes.MobsID_LootSlotVectorMap.value(MobsID);

                        qDebug() << "IN_Opcodes.FakeLootVector1" << IN_Opcodes.FakeLootVector;

                        IN_Opcodes.LootWindowsID = MobsID;

                        if(IN_Opcodes.FakeLootVector.isEmpty() == false)
                        {
                            QString ItemsList = "";

                            int counter = 0;

                            for(int re = 0; re < IN_Opcodes.FakeLootVector.size(); re+=2)
                            {
                                QString Items_ID = IN_Opcodes.FakeLootVector.at(re);
                                Quantity = IN_Opcodes.FakeLootVector.at(re+1);

                                qDebug() << "Items_ID" << Items_ID;
                                qDebug() << "Quantity" << Quantity;

                                ItemsInfo = merchants::Items_Map.value(Items_ID);

                                int index = ItemsInfo.indexOf("XXXXXXXX");
                                qDebug() << "index" << index;

                                QString LootBoxSlot = QString("%1").arg(counter,2,16,QLatin1Char('0')).toUpper();

                                ItemsInfo.replace(index,8,LootBoxSlot + "000000");

                                ItemsInfo.replace(0,2,Quantity);

                                ItemsList.append(ItemsInfo);

                                qDebug() << "ItemsInfo" << ItemsInfo;

                                counter++;
                            }

                            NumberOfItems = QString("%1").arg(IN_Opcodes.FakeLootVector.size(),2,16,QLatin1Char('0')).toUpper();

                            int Num1 = NumberOfItems.toInt(nullptr,16);

                            QString Num2 = QString("%1").arg(Num1/2,2,16,QLatin1Char('0')).toUpper();

                            qDebug() << "NumberOfItems" << NumberOfItems;
                            qDebug() << "Num2" << Num2;

                            FBsizeOpcode = "1800" + NumberOfItems + Num2 + "000000" + ItemsList;
                        }
                        else
                        {
                            FBsizeOpcode = "180000";
                        }



                        qDebug() << "FBsizeOpcode" << FBsizeOpcode;

                        IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Open_Loot_Window, ");
                    }
                }

                //Make a vector to keep track of where the slots are after looting.

                QString ItemsID = "";



                if(IN_Opcodes.opcoderesult1 == "1900")
                {
                    QMap<int,QString> Slot_ItemsID_Map = IN_Opcodes.MobsID_Slot_ItemsID_MapOfMaps.value(IN_Opcodes.Target_ID);

                    IN_Opcodes.FakeLootVector = IN_Opcodes.MobsID_LootVectorMap.value(IN_Opcodes.Target_ID);
                    IN_Opcodes.FakeLootSlotVector = IN_Opcodes.MobsID_LootSlotVectorMap.value(IN_Opcodes.Target_ID);

                    QString slot1 = SlotNumber.mid(0,2);
                    int slot2 = slot1.toInt(nullptr,16);

                    if(IN_Opcodes.LootWindowChecked == true)
                    {
                        IN_Opcodes.LootWindowChecked = false;

                        ItemsID = Slot_ItemsID_Map.value(slot2);

                        qDebug() << "Slot_ItemsID_Map" << ItemsID;
                        qDebug() << "IN_Opcodes.Slot_ItemsID_MapA" << Slot_ItemsID_Map;


                    }
                    else
                    {
                        ItemsID = IN_Opcodes.FakeLootSlotVector.at(slot2);

                        qDebug() << "FakeLootSlotVector" << ItemsID;
                        qDebug() << "IN_Opcodes.FakeLootSlotVectorB" << IN_Opcodes.FakeLootSlotVector;
                    }

                    IN_Opcodes.passdown = slot2;

                    qDebug() << "IN_Opcodes.passdown1" << IN_Opcodes.passdown;

                    //FB0A100019000100000001000000
                    // slot2 = 1

                    int index = IN_Opcodes.FakeLootVector.indexOf(ItemsID);

                    qDebug() << "index" << index;

                    Quantity = IN_Opcodes.FakeLootVector.at(index+1);

                    qDebug() << "Quantity" << Quantity;

                    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                    Loot sendto;
                    bool SendOrNot = sendto.AddLoot(ID_IP_PORT,ItemsID,Quantity);

                    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                    qDebug() << "IN_Opcodes.FakeLootVector" << IN_Opcodes.FakeLootVector;

                    if(SendOrNot == true)
                    {
                        IN_Opcodes.FakeLootVector.takeAt(index);
                        IN_Opcodes.FakeLootVector.takeAt(index);

                        QString SlotNumber1 = QString("%1").arg(index/2,2,16,QLatin1Char('0'));

                        FBsizeOpcode = "1900" + SlotNumber1 + "000000";

                        qDebug() << "FBsizeOpcode" << FBsizeOpcode;

                        IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Take_One_Item, ");
                    }


                    qDebug() << "IN_Opcodes.FakeLootVectorC" << IN_Opcodes.FakeLootVector;

                    if(IN_Opcodes.FakeLootVector.isEmpty() == true)
                    {
                        IN_Opcodes.MobsID_LootVectorMap.remove(IN_Opcodes.Target_ID);
                        IN_Opcodes.MobsID_LootSlotVectorMap.remove(IN_Opcodes.Target_ID);
                        IN_Opcodes.MobsID_Slot_ItemsID_MapOfMaps.remove(IN_Opcodes.Target_ID);
                    }
                    else
                    {
                        IN_Opcodes.MobsID_LootVectorMap.insert(IN_Opcodes.Target_ID,IN_Opcodes.FakeLootVector);
                        IN_Opcodes.MobsID_LootSlotVectorMap.insert(IN_Opcodes.Target_ID,IN_Opcodes.FakeLootSlotVector);
                    }
                }

                if(IN_Opcodes.opcoderesult1 == "1A00" || IN_Opcodes.opcoderesult1 == "1600")
                {
                    if(IN_Opcodes.opcoderesult1 == "1A00")
                    {
                        FBsizeOpcode = "1A00";

                        qDebug() << "FBsizeOpcode" << FBsizeOpcode;

                        packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_Opcodes);

                        IN_Opcodes.FakeLootVector = IN_Opcodes.MobsID_LootVectorMap.value(IN_Opcodes.Target_ID);

                        for(int re = 0; re < IN_Opcodes.FakeLootVector.size(); re+=2)
                        {
                            ItemsID = IN_Opcodes.FakeLootVector.at(re);
                            Quantity = IN_Opcodes.FakeLootVector.at(re+1);

                            Loot sendto;
                            bool SendOrNot = sendto.AddLoot(ID_IP_PORT,ItemsID,Quantity);

                            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                            if(SendOrNot == true)
                            {
                                IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                                IN_Opcodes.FBReplyOrNotVector.append("03");
                                IN_Opcodes.FBPacketDescriptionVector.append("Take_All_Items, ");

                                IN_Opcodes.FakeLootVector.takeAt(re);
                                IN_Opcodes.FakeLootVector.takeAt(re);
                            }
                        }

                        if(IN_Opcodes.FakeLootVector.isEmpty() == true)
                        {
                            IN_Opcodes.MobsID_LootVectorMap.remove(IN_Opcodes.Target_ID);
                            IN_Opcodes.MobsID_LootSlotVectorMap.remove(IN_Opcodes.Target_ID);
                            IN_Opcodes.MobsID_Slot_ItemsID_MapOfMaps.remove(IN_Opcodes.Target_ID);
                        }
                        else
                        {
                            IN_Opcodes.MobsID_LootVectorMap.insert(IN_Opcodes.Target_ID,IN_Opcodes.FakeLootVector);
                            IN_Opcodes.MobsID_LootSlotVectorMap.insert(IN_Opcodes.Target_ID,IN_Opcodes.FakeLootSlotVector);
                        }
                    }

                    //Closes loot window



                    if(IN_Opcodes.isitOpen == true)
                    {
                        openOrnot = "true";
                    }
                    else
                    {
                        openOrnot = "false";
                    }

                    if(IN_Opcodes.isitOpen == true)
                    {
                        IN_Opcodes.isitOpen = false;

                        FBsizeOpcode = "1600";

                        qDebug() << "FBsizeOpcode" << FBsizeOpcode;

                        IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
                        IN_Opcodes.FBReplyOrNotVector.append("03");
                        IN_Opcodes.FBPacketDescriptionVector.append("Close_Loot_Window, ");



                        if(IN_Opcodes.FakeLootVector.isEmpty() == true)
                        {
                            //Despawn
                            QByteArray holderXorC9 = "01c9xxxx01100180c800";

                            holderXorC9.replace(0,2,Current_channel.toUtf8().toUpper());

                            QByteArray Channel = holderXorC9.mid(0,2);
                            QVector <QByteArray> CurrenChannelVector = IN_Opcodes.MapOfChannelVectors.value(Channel);
                            CurrenChannelVector.append(holderXorC9);
                            IN_Opcodes.MapOfChannelVectors.insert(Channel,CurrenChannelVector);

                            QByteArray MobC9 = objectpacket::Master_Map.value(IN_Opcodes.Target_ID);
                            MobC9.replace(52,2,"01");
                            MobC9.replace(54,2,"FF");
                            MobC9.replace(116,2,"00");

                            objectpacket::Master_Map.insert(IN_Opcodes.Target_ID,MobC9);

                            objectpacket::DeadListVector.append(IN_Opcodes.Target_ID);

                            IN_Opcodes.channelmap.insert(Current_channel,"free");

                            IN_Opcodes.MobRespawnTimer = "20000";

                            IN_Opcodes.LootWindowChecked = false;
                        }
                        else
                        {
                            IN_Opcodes.LootWindowChecked = true;

                            qDebug() << "IN_Opcodes.passdown2" << IN_Opcodes.passdown;

                            if(IN_Opcodes.passdown != 9999)
                            {
                                QMap<int,QString> Slot_ItemsID_Map = IN_Opcodes.MobsID_Slot_ItemsID_MapOfMaps.value(IN_Opcodes.Target_ID);
                                Slot_ItemsID_Map.remove(IN_Opcodes.passdown);
                                int counter = 0;
                                QMap<int,QString> tempMap;

                                qDebug() << "IN_Opcodes.Slot_ItemsID_MapB" << Slot_ItemsID_Map;

                                QMapIterator<int,QString> iter(Slot_ItemsID_Map);
                                while(iter.hasNext())
                                {
                                    iter.next();
                                    QString ItemsID = iter.value();

                                    tempMap.insert(counter,ItemsID);

                                    counter++;
                                }

                                IN_Opcodes.FakeLootSlotVector.removeAt(IN_Opcodes.passdown);
                                qDebug() << "IN_Opcodes.FakeLootSlotVectorA" << IN_Opcodes.FakeLootSlotVector;

                                IN_Opcodes.MobsID_LootSlotVectorMap.insert(IN_Opcodes.Target_ID,IN_Opcodes.FakeLootSlotVector);

                                IN_Opcodes.passdown = 9999;

                                qDebug() << "IN_Opcodes.Slot_ItemsID_MapC" << tempMap;

                                IN_Opcodes.MobsID_Slot_ItemsID_MapOfMaps.insert(IN_Opcodes.Target_ID,tempMap);
                            }
                        }
                    }
                }
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }



        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if(IN_Opcodes.opcoderesult1 == "4900" && IN_Opcodes.teleporting == true)
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);
            /////////////////////////////////////////////////////Stack Fix/////////////////////////////////////////////////////////////////////////////
            QByteArray didwedrop = gamebuf1.mid(36,4).toUpper(); //find the client opcode

            if(IN_Opcodes.dropped1 ==  didwedrop)
            {
                IN_Opcodes.dropped1 = ""; //set it back
                //     ui->textEdit_4->append("C700-dropped or doubled" + QString::fromUtf8(IN_Opcodes.dropped1) +"..." + didwedrop);

                int newstackcountx = IN_Opcodes.X_stack.size();
                int laststackx = IN_Opcodes.X_stack.at(newstackcountx - 2); //-1 for last 4029

                IN_Opcodes.X_stack.push(laststackx);


                if(IN_Opcodes.X_stack.size()  >  255)
                {
                    IN_Opcodes.X_stack.removeAt(0);
                }

                int newstackcountz = IN_Opcodes.Z_stack.size();
                int laststackz = IN_Opcodes.Z_stack.at(newstackcountz - 2);

                IN_Opcodes.Z_stack.push(laststackz);


                if(IN_Opcodes.Z_stack.size()  >  255)
                {
                    IN_Opcodes.Z_stack.removeAt(0);
                }
                int newstackcounty = IN_Opcodes.Y_stack.size();
                int laststacky = IN_Opcodes.Y_stack.at(newstackcounty - 2);

                IN_Opcodes.Y_stack.push(laststacky);


                if(IN_Opcodes.Y_stack.size()  >  255)
                {
                    IN_Opcodes.Y_stack.removeAt(0);
                }
            }

            IN_Opcodes.dropped1 = gamebuf1.mid(36,4).toUpper(); //find client opcode, keep til the next go around
            ///////////////////////////////////////////////////////////////C700/4900///////////////////////////////////////////////////////////

            IN_Opcodes.SendFBsVector.append("C70000");
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("4900_IN_Opcodes.Reply, ");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

            objectpacket sendto;
            QByteArray Status = sendto.packetswitch(ClientID,ClientsIP,ClientsPort);

            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            qDebug() << "IN_Opcodes.Map_Channel_ID1" << IN_Opcodes.Map_Channel_ID;

            if(IN_Opcodes.ActivateOP.isEmpty() == false)
            {
                for(int ld = 0; ld < IN_Opcodes.ActivateOP.size(); ld++)
                {
                    QByteArray Fullc9 = IN_Opcodes.ActivateOP.at(ld);

                    QByteArray Channel = Fullc9.mid(0,2);
                    QVector <QByteArray> CurrenChannelVector = IN_Opcodes.MapOfChannelVectors.value(Channel);

                    CurrenChannelVector.append(Fullc9);
                    IN_Opcodes.MapOfChannelVectors.insert(Channel,CurrenChannelVector);
                }

                IN_Opcodes.ActivateOP.clear();
                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

                flipped sento;
                sento.SendOut(IN_Opcodes.ClientsID, IN_Opcodes.ClientsIP, IN_Opcodes.ClientsPort);

                IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
            }



            QString masterxhold1 = QString::fromUtf8(IN_Opcodes.masterx);
            QString masterzhold1 = QString::fromUtf8(IN_Opcodes.masterz);
            QString masteryhold1 = QString::fromUtf8(IN_Opcodes.mastery);


            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }

        if(IN_Opcodes.opcoderesult1 == "1D00")//tps spent
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            ADD_OR_SUB SendToDec;


            int holdposition = 0;
            QList <QByteArray> mylist;
            QByteArray myarray = "";
            QByteArray main_array = "";
            int caratc = 0;
            int help1 = 0;
            int help2 = 0;





            do
            {
                main_array = opholder.mid(12,-1);

                QString hold_str1 = main_array.mid(caratc,2);
                help1 = hold_str1.toInt(nullptr,16);

                if(help1 < 0x80)
                {
                    mylist.append(hold_str1.toUtf8().toUpper());
                }

                if(help1 >= 0x80)
                {
                    holdposition = caratc;
                    myarray.append(main_array.mid(caratc,2));

                    do
                    {
                        caratc+=2;
                        QString hold_strpeek = main_array.mid(caratc,2);
                        help2= hold_strpeek.toInt(nullptr,16);
                        myarray.append(hold_strpeek.toUtf8());
                    }

                    while (help2 >= 0x80);

                    caratc +=2;
                    mylist.insert(holdposition,myarray);
                    myarray = "";
                }
                else
                {
                    caratc+=2;
                }
            }

            while(caratc < main_array.size());

            int tpsSpent = 0;

            for(int tp = 0; tp < mylist.size(); tp++)
            {
                QString Value = mylist.at(tp);
                QString DecVal = SendToDec.BackToDec(Value);
                int DecVal1 = DecVal.toInt(nullptr,10);
                tpsSpent = tpsSpent + DecVal1;
            }

            ////////////////////////////////////////////////////////////

            int NewCurrentHP = 0;
            int NewCurrentPOW = 0;
            int MyNewHP = -1;
            xorpacket Xor;
            QByteArray New42 = Xor.Recompress42EC(ClientID,ClientsIP,ClientsPort,mylist,IN_Opcodes.CharSelectID,IN_Opcodes.StatsChange,IN_Opcodes.HoldOldHP,NewCurrentHP,NewCurrentPOW,MyNewHP);


            QByteArray StatHeader = "42ECxxxx01";

            IN_Opcodes.Send42ECsVector.append(StatHeader + New42);

            QString tpsSpent1 = QString("%1").arg(tpsSpent,1,10,QLatin1Char('0')).toUpper();

            IN_Opcodes.UnSpentTPs = IN_Opcodes.UnSpentTPs - tpsSpent;
            IN_Opcodes.SpentTPs = IN_Opcodes.SpentTPs + tpsSpent;


            db.transaction();

            QSqlQuery query;

            Table_Name = "MAINTOON";

            QString RemainingTPs1 = QString("%1").arg(IN_Opcodes.UnSpentTPs,1,10,QLatin1Char('0')).toUpper();
            Convert SendToHex;
            QString RemainingTPs2 = SendToHex.ConvertTo(RemainingTPs1);

            Column_Name = "RemainingTPs";
            query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
            query.bindValue(":val",RemainingTPs2);
            query.bindValue(":id",IN_Opcodes.packetid);
            query.exec();

            if(!query.exec())
            {
                qDebug() << "RemainingTPs query failed" << IN_Opcodes.packetid;
            }

            IN_Opcodes.StartUnSpentTPs = RemainingTPs2;

            ////////////////////////////////////////////////////////////////////////////////////////////

            QString TotalTPSpent1 = QString("%1").arg(IN_Opcodes.SpentTPs,1,10,QLatin1Char('0')).toUpper();
            QString TotalTPSpent2 = SendToHex.ConvertTo(TotalTPSpent1);

            Column_Name = "TotalTPSpent";
            query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
            query.bindValue(":val",TotalTPSpent2);
            query.bindValue(":id",IN_Opcodes.packetid);
            query.exec();

            if(!query.exec())
            {
                qDebug() << "TotalTPSpent query failed" << IN_Opcodes.packetid;
            }

            IN_Opcodes.StartSpentTPs = TotalTPSpent2;

            db.commit();


            Convert SendTo;
            QString TPVal = SendTo.ConvertTo(tpsSpent1);

            QString FBsizeOpcode = "1D00" + TPVal;

            IN_Opcodes.SendFBsVector.append(FBsizeOpcode.toUtf8());
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("Spend_Training_Points, ");

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }


        /////////////////////////////////////////////////////4C00 Merchants/////////////////////////////////////////////////////////////////////
        if(IN_Opcodes.opcoderesult1 == "4C00")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);


            QByteArray new_merch_id = opholder.mid(12,8).toUpper(); //get last 8 pretty sure crc is gone

            merchants sending1;
            QMap <int,QByteArray> merch_output_map;

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
            merch_output_map = sending1.mer_packet(ClientID,ClientsIP,ClientsPort,new_merch_id);
            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            QByteArray no_match = merch_output_map.value(0).toUpper();

            if(no_match == "NULL")
            {
                QByteArray MerchantPacket = "";

                QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                QString TextFull5 = "This store is closed due to covid-19.";
                ADD_OR_SUB SendToText5;
                QString FBText5 =  SendToText5.TextSize(ClientID,ClientsIP,ClientsPort,TextFull5,false,"CE00",Servers_Last_FB_MessageNumber);

                Servers_Last_FB_MessageNumber = FBText5.mid(0,4).toUtf8();
                FBText5.remove(0,4);

                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                increment GrpOut;
                QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                Servers_Last_MessageNumber = GrpOut_a.toUpper();

                MessNumbVect.replace(0,Servers_Last_MessageNumber);

                CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                MerchantPacket.insert(0,IN_Opcodes.serverid1 + IN_Opcodes.clientid1 + "xxxx01bf543213" + IN_Opcodes.SessionID +
                                      + "03" + Servers_Last_MessageNumber + Clients_Last_MessageNumber + Clients_Last_FB_MessageNumber + FBText5);

                packetsize ResizePacket; //call size class
                QByteArray work_merch2 = ResizePacket.switchsize(MerchantPacket); //get new size
                MerchantPacket.replace(8,4,work_merch2);

                crc sendcrc;
                QByteArray outcrc =  sendcrc.elcrc(MerchantPacket);
                MerchantPacket.append(outcrc);

                QByteArray Arrange_out = MerchantPacket.fromHex(MerchantPacket);

                QHostAddress current_client_address;
                current_client_address.setAddress(QString::fromUtf8(IN_Opcodes.ClientsIP));
                quint16 current_client_port = IN_Opcodes.ClientsPort.toUShort(nullptr,10);
                QString changeIP = current_client_address.toString();

                QHostAddress newsender = QHostAddress(changeIP);
                worldthread::udpsocket3->writeDatagram(Arrange_out,newsender,current_client_port);

                QString MainToon = checkthezone::NPCsNames.value(IN_Opcodes.CharSelectID);

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

                MainWindow::UiOputVector.append("");
                MainWindow::UiOputVector.append("");
                MainWindow::UiOputVector.append("----------------------------------------------------------");
                MainWindow::UiOputVector.append(dateTimeString);
                MainWindow::UiOputVector.append("Merchant_Window: 4C00");
                MainWindow::UiOputVector.append("To Client: " + MainToon);
                MainWindow::UiOputVector.append("IP: " + ClientsIP);
                MainWindow::UiOputVector.append("Port: " + ClientsPort);
                MainWindow::UiOputVector.append("From Server: Packet:");
                MainWindow::UiOputVector.append(MerchantPacket);
                MainWindow::UiOputVector.append("----------------------------------------------------------");
                MainWindow::UiOputVector.append("");

                qDebug() << "";
                qDebug() << "";
                qDebug() << "----------------------------------------------------------";
                qDebug() << dateTimeString;
                qDebug() << "Merchant_Window: ";
                qDebug() << "To Client: " + MainToon;
                qDebug() << "IN_MainWindow.CharSelectID = " << IN_Opcodes.CharSelectID;
                qDebug() << "IP: " + IN_Opcodes.ClientsIP;
                qDebug() << "Port: " + IN_Opcodes.ClientsPort;
                qDebug() << "From Server: ";
                qDebug() << MerchantPacket;
                qDebug() << "----------------------------------------------------------";
                qDebug() << "";
            }

            if(no_match != "NULL")
            {
                qDebug() << "no_match" << no_match;

                for(int re = 0; re < merch_output_map.size(); re++)
                {
                    QByteArray MerchantPacket = merch_output_map.value(re).toUpper(); //change to upper to find correct indexes



                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                    increment GrpOut;
                    QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                    Servers_Last_MessageNumber = GrpOut_a.toUpper();

                    MessNumbVect.replace(0,Servers_Last_MessageNumber);

                    CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                    MerchantPacket.insert(0,IN_Opcodes.serverid1 + IN_Opcodes.clientid1 + "xxxx01bf543213" + IN_Opcodes.SessionID +
                                          + "03" + Servers_Last_MessageNumber);

                    QString fb_position = MerchantPacket.mid(34,2);
                    QString fa_position = MerchantPacket.mid(34,2);


                    qDebug() << "fa_position" << fa_position;
                    qDebug() << "fb_position" << fb_position;
                    qDebug() << "work1" << MerchantPacket;


                    if(fa_position == "FA")
                    {


                        if(re == 0)
                        {
                            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                            MerchantPacket.insert(34,Clients_Last_MessageNumber + Clients_Last_FB_MessageNumber);

                            qDebug() << "Clients_Last_MessageNumber" << Clients_Last_MessageNumber;
                            qDebug() << "Clients_Last_FB_MessageNumber" << Clients_Last_FB_MessageNumber;
                            qDebug() << "work2" << MerchantPacket;

                            increment merch_opc;
                            QByteArray merch_serv1 = merch_opc.count(Servers_Last_FB_MessageNumber); //server opcode increment
                            MerchantPacket.replace(50,4,merch_serv1); //replace new count at fa + 8
                            Servers_Last_FB_MessageNumber = merch_serv1.toUpper(); //replace new count

                            MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                            CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);
                        }
                        else
                        {
                            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                            MerchantPacket.replace(28,2,"00");

                            increment merch_opc;
                            QByteArray merch_serv1 = merch_opc.count(Servers_Last_FB_MessageNumber); //server opcode increment
                            MerchantPacket.replace(42,4,merch_serv1); //replace new count at fa + 8
                            Servers_Last_FB_MessageNumber = merch_serv1.toUpper(); //replace new count

                            MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                            CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);
                        }

                        packetsize ResizePacket; //call size class
                        QByteArray work_merch2 = ResizePacket.switchsize(MerchantPacket); //get new size
                        MerchantPacket.replace(8,4,work_merch2);

                        crc sendcrc;
                        QByteArray outcrc =  sendcrc.elcrc(MerchantPacket);
                        MerchantPacket.append(outcrc);

                        QByteArray Arrange_out = MerchantPacket.fromHex(MerchantPacket);

                        QHostAddress current_client_address;
                        current_client_address.setAddress(QString::fromUtf8(IN_Opcodes.ClientsIP));
                        quint16 current_client_port = IN_Opcodes.ClientsPort.toUShort(nullptr,10);
                        QString changeIP = current_client_address.toString();

                        QHostAddress newsender = QHostAddress(changeIP);
                        worldthread::udpsocket3->writeDatagram(Arrange_out,newsender,current_client_port);

                        QString MainToon = checkthezone::NPCsNames.value(IN_Opcodes.CharSelectID);

                        QDateTime dateTime = dateTime.currentDateTime();
                        QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

                        MainWindow::UiOputVector.append("");
                        MainWindow::UiOputVector.append("");
                        MainWindow::UiOputVector.append("----------------------------------------------------------");
                        MainWindow::UiOputVector.append(dateTimeString);
                        MainWindow::UiOputVector.append("Merchant_Window: 4C00");
                        MainWindow::UiOputVector.append("To Client: " + MainToon);
                        MainWindow::UiOputVector.append("IP: " + ClientsIP);
                        MainWindow::UiOputVector.append("Port: " + ClientsPort);
                        MainWindow::UiOputVector.append("From Server: Packet:");
                        MainWindow::UiOputVector.append(MerchantPacket);
                        MainWindow::UiOputVector.append("----------------------------------------------------------");
                        MainWindow::UiOputVector.append("");

                        qDebug() << "";
                        qDebug() << "";
                        qDebug() << "----------------------------------------------------------";
                        qDebug() << dateTimeString;
                        qDebug() << "Merchant_Window: ";
                        qDebug() << "To Client: " + MainToon;
                        qDebug() << "IN_MainWindow.CharSelectID = " << IN_Opcodes.CharSelectID;
                        qDebug() << "IP: " + IN_Opcodes.ClientsIP;
                        qDebug() << "Port: " + IN_Opcodes.ClientsPort;
                        qDebug() << "From Server: ";
                        qDebug() << MerchantPacket;
                        qDebug() << "----------------------------------------------------------";
                        qDebug() << "";


                    }

                    if(fb_position == "FB")
                    {
                        QByteArray find_ff = MerchantPacket.mid(36,2).toUpper();
                        qDebug() << "find ff" << find_ff;
                        if(find_ff == "FF") //ff large header
                        {
                            if(re == 0)
                            {
                                QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                                MerchantPacket.insert(34,Clients_Last_MessageNumber + Clients_Last_FB_MessageNumber);

                                qDebug() << "Clients_Last_MessageNumber" << Clients_Last_MessageNumber;
                                qDebug() << "Clients_Last_FB_MessageNumber" << Clients_Last_FB_MessageNumber;
                                qDebug() << "work2" << MerchantPacket;

                                increment merch_opc;
                                QByteArray merch_serv1 = merch_opc.count(Servers_Last_FB_MessageNumber); //server opcode increment
                                MerchantPacket.replace(50,4,merch_serv1); //replace new count at fa + 8
                                Servers_Last_FB_MessageNumber = merch_serv1.toUpper(); //replace new count

                                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                                CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);
                            }
                            else
                            {
                                QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                                MerchantPacket.replace(28,2,"00");

                                increment merch_opc;
                                QByteArray merch_serv1 = merch_opc.count(Servers_Last_FB_MessageNumber); //server opcode increment
                                MerchantPacket.replace(42,4,merch_serv1); //replace new count at fa + 8
                                Servers_Last_FB_MessageNumber = merch_serv1.toUpper(); //replace new count

                                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                                CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);
                            }

                            packetsize ResizePacket; //call size class
                            QByteArray work_merch2 = ResizePacket.switchsize(MerchantPacket); //get new size
                            MerchantPacket.replace(8,4,work_merch2);

                            crc sendcrc;
                            QByteArray outcrc =  sendcrc.elcrc(MerchantPacket);
                            MerchantPacket.append(outcrc);

                            QByteArray Arrange_out = MerchantPacket.fromHex(MerchantPacket);

                            QHostAddress current_client_address;
                            current_client_address.setAddress(QString::fromUtf8(IN_Opcodes.ClientsIP));
                            quint16 current_client_port = IN_Opcodes.ClientsPort.toUShort(nullptr,10);
                            QString changeIP = current_client_address.toString();

                            QHostAddress newsender = QHostAddress(changeIP);
                            worldthread::udpsocket3->writeDatagram(Arrange_out,newsender,current_client_port);

                            QString MainToon = checkthezone::NPCsNames.value(IN_Opcodes.CharSelectID);

                            QDateTime dateTime = dateTime.currentDateTime();
                            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

                            MainWindow::UiOputVector.append("");
                            MainWindow::UiOputVector.append("");
                            MainWindow::UiOputVector.append("----------------------------------------------------------");
                            MainWindow::UiOputVector.append(dateTimeString);
                            MainWindow::UiOputVector.append("Merchant_Window: 4C00");
                            MainWindow::UiOputVector.append("To Client: " + MainToon);
                            MainWindow::UiOputVector.append("IP: " + ClientsIP);
                            MainWindow::UiOputVector.append("Port: " + ClientsPort);
                            MainWindow::UiOputVector.append("From Server: Packet:");
                            MainWindow::UiOputVector.append(MerchantPacket);
                            MainWindow::UiOputVector.append("----------------------------------------------------------");
                            MainWindow::UiOputVector.append("");

                            qDebug() << "";
                            qDebug() << "";
                            qDebug() << "----------------------------------------------------------";
                            qDebug() << dateTimeString;
                            qDebug() << "Merchant_Window: ";
                            qDebug() << "To Client: " + MainToon;
                            qDebug() << "IN_MainWindow.CharSelectID = " << IN_Opcodes.CharSelectID;
                            qDebug() << "IP: " + IN_Opcodes.ClientsIP;
                            qDebug() << "Port: " + IN_Opcodes.ClientsPort;
                            qDebug() << "From Server: ";
                            qDebug() << MerchantPacket;
                            qDebug() << "----------------------------------------------------------";
                            qDebug() << "";

                            //IN_Opcodes.ResendFBsMap.insert(IN_Opcodes.Servers_Last_FB_MessageNumber,MerchantPacket);

                            //IN_Opcodes.ResendFBsMap.insert(IN_Opcodes.Servers_Last_FB_MessageNumber,MerchantPacket);

                        }
                        else
                        { //small header

                            if(re != 0)
                            {
                                MerchantPacket.replace(28,2,"00");
                            }

                            QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);



                            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                            increment merch_opc; //get class
                            QByteArray merch_serv1 = merch_opc.count(Servers_Last_FB_MessageNumber); //server opcode increment
                            MerchantPacket.replace(38,4,merch_serv1); //replace new count at fb + 4
                            Servers_Last_FB_MessageNumber = merch_serv1.toUpper(); //replace new count

                            MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                            CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);

                            packetsize ResizePacket; //call size class
                            QByteArray work_merch2 = ResizePacket.switchsize(MerchantPacket); //get new size
                            MerchantPacket.replace(8,4,work_merch2);

                            crc sendcrc;
                            QByteArray outcrc =  sendcrc.elcrc(MerchantPacket);
                            MerchantPacket.append(outcrc);

                            QByteArray Arrange_out = MerchantPacket.fromHex(MerchantPacket);

                            QHostAddress current_client_address;
                            current_client_address.setAddress(QString::fromUtf8(IN_Opcodes.ClientsIP));
                            quint16 current_client_port = IN_Opcodes.ClientsPort.toUShort(nullptr,10);
                            QString changeIP = current_client_address.toString();

                            QHostAddress newsender = QHostAddress(changeIP);
                            worldthread::udpsocket3->writeDatagram(Arrange_out,newsender,current_client_port);

                            QString MainToon = checkthezone::NPCsNames.value(IN_Opcodes.CharSelectID);

                            QDateTime dateTime = dateTime.currentDateTime();
                            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

                            MainWindow::UiOputVector.append("");
                            MainWindow::UiOputVector.append("");
                            MainWindow::UiOputVector.append("----------------------------------------------------------");
                            MainWindow::UiOputVector.append(dateTimeString);
                            MainWindow::UiOputVector.append("Merchant_Window: 4C00");
                            MainWindow::UiOputVector.append("To Client: " + MainToon);
                            MainWindow::UiOputVector.append("IP: " + ClientsIP);
                            MainWindow::UiOputVector.append("Port: " + ClientsPort);
                            MainWindow::UiOputVector.append("From Server: Packet:");
                            MainWindow::UiOputVector.append(MerchantPacket);
                            MainWindow::UiOputVector.append("----------------------------------------------------------");
                            MainWindow::UiOputVector.append("");

                            qDebug() << "";
                            qDebug() << "";
                            qDebug() << "----------------------------------------------------------";
                            qDebug() << dateTimeString;
                            qDebug() << "Merchant_Window: ";
                            qDebug() << "To Client: " + MainToon;
                            qDebug() << "IN_MainWindow.CharSelectID = " << IN_Opcodes.CharSelectID;
                            qDebug() << "IP: " + IN_Opcodes.ClientsIP;
                            qDebug() << "Port: " + IN_Opcodes.ClientsPort;
                            qDebug() << "From Server: ";
                            qDebug() << MerchantPacket;
                            qDebug() << "----------------------------------------------------------";
                            qDebug() << "";
                        }
                    }
                }
            }
            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }

        ////////////////////////////////////////////6007/6107//////////////////////////////////////////////////////////////////////////////////////
        if(IN_Opcodes.opcoderesult1 == "6007")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);


            IN_Opcodes.Target_ID = opholder.mid(20,8).toUpper();

            IN_Opcodes.Con_Color_V = IN_Opcodes.returnedcolor;

            IN_Opcodes.current_c9 = objectpacket::Master_Map.value(IN_Opcodes.Target_ID);
            QString npc_value = IN_Opcodes.current_c9.mid(374,8);


            qDebug() << "npc_value" << npc_value;

            targeting changecolor1;

            if(IN_Opcodes.Target_ID == IN_Opcodes.CharSelectID_Main)// if self target
            {
                IN_Opcodes.returnedcolor = changecolor1.target(ClientID,ClientsIP,ClientsPort,IN_Opcodes.CharSelectID.toUtf8());

                IN_Opcodes.Con_Color_V = IN_Opcodes.returnedcolor;

                IN_Opcodes.Face_Color_V = "04";
                IN_Opcodes.Con_Color_V = "02";
                IN_Opcodes.holder_Con = Con_Packet; //send self packet

                IN_Opcodes.holder_Con.replace(296,6,IN_Opcodes.Target_ID);
            }

            if(IN_Opcodes.Target_ID != IN_Opcodes.CharSelectID_Main)// if not self target
            {
                IN_Opcodes.returnedcolor = changecolor1.target(ClientID,ClientsIP,ClientsPort,IN_Opcodes.Target_ID);

                IN_Opcodes.Con_Color_V = IN_Opcodes.returnedcolor;

                QByteArray current_c9 = objectpacket::Master_Map.value(IN_Opcodes.Target_ID);
                QString npc_value = current_c9.mid(374,8);

                if(FactionIDVector.contains(npc_value))
                {
                    int Faction = FactionIDVector.indexOf(npc_value);
                    int factionnumber1 = IN_Opcodes.FactionValueVector.at(Faction);

                    if(factionnumber1 <= 9800)
                    {
                        IN_Opcodes.Face_Color_V = "00";
                    }

                    if(factionnumber1 >= 9801 && factionnumber1 <= 10199)
                    {
                        IN_Opcodes.Face_Color_V = "02";
                    }

                    if(factionnumber1 >= 10199)
                    {
                        IN_Opcodes.Face_Color_V = "04";
                    }

                    IN_Opcodes.holder_Con = Con_Packet; //send the con packet
                }
                else
                {
                    IN_Opcodes.Face_Color_V = "02";
                    IN_Opcodes.holder_Con = Con_Packet;
                }
            }

            IN_Opcodes.holder_Con.replace(4,2,IN_Opcodes.Face_Color_V.toUtf8());
            IN_Opcodes.holder_Con.replace(6,2,IN_Opcodes.Con_Color_V.toUtf8());

            IN_Opcodes.Target_Counter_01 = opholder.mid(12,8);
            IN_Opcodes.holder_Con.replace(522,8,IN_Opcodes.Target_Counter_01);

            IN_Opcodes.SendFBsVector.append(IN_Opcodes.holder_Con);
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("Target_NPC, ");

            IN_Opcodes.Con_Color_V = "";
            IN_Opcodes.Face_Color_V = "";
            IN_Opcodes.holder_Con = "";

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }


        if(IN_Opcodes.opcoderesult1 == "0114")//add to vector
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray SubOp = opholder.mid(12,2);

            if(SubOp == "04")
            {
                for(int cm = 0; cm < CMvector.size(); cm++)
                {
                    QByteArray holderCM = CMvector.at(cm);

                    if(cm == 0)
                    {
                        holderCM.replace(34,4,Clients_Last_MessageNumber);
                        holderCM.replace(38,4,Clients_Last_FB_MessageNumber);
                    }

                    holderCM.replace(0,4,IN_Opcodes.serverid1);
                    holderCM.replace(4,4,IN_Opcodes.clientid1);

                    holderCM.replace(22,6,IN_Opcodes.SessionID);

                    packetsize getnewsize1;
                    QByteArray resize = getnewsize1.switchsize(holderCM);
                    holderCM.replace(8,4,resize);

                    QVector <QByteArray> MessNumbVect = CharID_ServerMessageNumbers.value(IN_Opcodes.CharSelectID);
                    QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                    increment GrpOut;
                    QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                    holderCM.replace(30,4,GrpOut_a);
                    Servers_Last_MessageNumber = GrpOut_a.toUpper();

                    MessNumbVect.replace(0,Servers_Last_MessageNumber);



                    if(cm != 0)
                    {
                        QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                        increment SendTo;
                        QByteArray ServerCM = SendTo.count(Servers_Last_FB_MessageNumber);
                        holderCM.replace(42,4,ServerCM);
                        Servers_Last_FB_MessageNumber = ServerCM.toUpper();

                        MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                    }

                    CharID_ServerMessageNumbers.insert(IN_Opcodes.CharSelectID,MessNumbVect);
                }
            }


            if(SubOp == "05")
            {
                IN_Opcodes.SendFBsVector.append(CM_Packet_10);
                IN_Opcodes.FBReplyOrNotVector.append("03");
                IN_Opcodes.FBPacketDescriptionVector.append("Shut_up_CM_Check, ");
            }
            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }

        ///////////////////////////////////////////////////9107/9007/////////////////////////////////////////////////////////////////




        if(IN_Opcodes.opcoderesult1 == "9107")
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            QByteArray closer9007 = "9007ffffffffffffffff0x000000" + IN_Opcodes.serverid1.toUpper() + "3b1d69776218";

            increment packetcount7; //get class
            QByteArray newcount7 = packetcount7.count(IN_Opcodes.C_9007_endcount); //use old count from first 9007
            closer9007.replace(20,4,newcount7); //replace new end count at 20

            IN_Opcodes.SendFBsVector.append(closer9007);
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("9007_Closer, ");

            if(IN_Opcodes.teleporting == true)
            {
                IN_Opcodes.TeleportTimer = "50";
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);


        }

        /////////////////////////////////////////B009 Disconnect//////////////////////////////////////////////////////////////





        /////////////////////////////////////////FE00 Unmemorize Spells//////////////////////////////////////////////////////////////////////////
        if(IN_Opcodes.opcoderesult1 == "FE00")     //FB060F00FE0002000000
        {
            IN_Opcodes.OpcodesAndGamebuf1Map.remove(IN_Opcodes.opcoderesult1);

            IN_Opcodes.unmemorize = true; //set this bool so we can trick ungrey to ungrey all of our slots

            QByteArray Current_Slot = opholder.mid(12,8);
            QByteArray UnMemorize_All = "FF0001000000";

            UnMemorize_All.replace(4,8,Current_Slot);

            IN_Opcodes.SendFBsVector.append(UnMemorize_All);
            IN_Opcodes.FBReplyOrNotVector.append("03");
            IN_Opcodes.FBPacketDescriptionVector.append("UnMemorize_All, ");

            //spellunmemorize();//jump to spellcooldown

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        }


        ///////////////////////////////////////////////////////////Packet 173//////////////////////////////////////////////////////////////







    }


    IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);


    QHostAddress current_client_address;
    current_client_address.setAddress(QString::fromUtf8(ClientsIP));
    quint16 current_client_port = ClientsPort.toUShort(nullptr,10);
    QString changeIP = current_client_address.toString();



    if(IN_Opcodes.OpcodeReturn.isEmpty() == false)
    {
        do
        {
            IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            qDebug() << "IN_Opcodes.OpcodeReturn.size()4 = " << IN_Opcodes.OpcodeReturn.size();

            QByteArray Data = IN_Opcodes.OpcodeReturn.takeAt(0);
            QByteArray Description = IN_Opcodes.OpcodeReturn.takeAt(0);



            if(Data != "IN_Opcodes.opcoderesult1")
            {
                if(!Description.contains("Switch_Clients"))
                {
                    QByteArray Reply_or_Not = Data.mid(28,2);

                    if(IN_Opcodes.PacketReply == "00")
                    {
                        if(Reply_or_Not != "00")
                        {
                            Reply_or_Not = "03";
                        }
                    }

                    if(IN_Opcodes.PacketReply == "20")
                    {
                        if(Reply_or_Not != "20")
                        {
                            Reply_or_Not = "23";
                        }
                    }

                    int index = Data.indexOf("xxXXxxXX");

                    if(index != -1)
                    {
                        if(Reply_or_Not == "23")
                        {
                            if(IN_Opcodes.Server_2A00_reply == true)
                            {
                                Data.replace(index,8,"BF543213");
                            }
                            else
                            {
                                Data.replace(index,8,"F1463213");
                            }
                        }

                        if(Reply_or_Not == "03" || Reply_or_Not == "13"
                                || Reply_or_Not == "00" || Reply_or_Not == "20")
                        {
                            if(IN_Opcodes.Server_2A00_reply == true)
                            {
                                Data.replace(index,8,"BF543213");
                            }
                            else
                            {
                                Data.replace(index,8,"F1463213");
                            }
                        }
                    }

                    qDebug() << "IN_Opcodes.Server_2A00_reply" << IN_Opcodes.Server_2A00_reply;

                    qDebug() << "IN_Opcodes.PacketReply" << IN_Opcodes.PacketReply;
                    qDebug() << "Reply_or_Not" << Reply_or_Not;

                    Data.replace(28,2,Reply_or_Not);

                    IN_Opcodes.ResendFBsTimer = "400";

                    crc sendcrc;
                    QByteArray outcrc =  sendcrc.elcrc(Data);
                    Data.append(outcrc);


                    QString MainToon = checkthezone::NPCsNames.value(IN_Opcodes.CharSelectID);

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

                    qDebug() << "";
                    qDebug() << "";
                    qDebug() << "";
                    qDebug() << dateTimeString;

                    qDebug() << "Opcodes";
                    qDebug() << "To Client: " + MainToon;
                    qDebug() << "IP: " + ClientsIP;
                    qDebug() << "Port: " + ClientsPort;
                    qDebug() << "From Server: ";
                    qDebug() << Data;
                    qDebug() << "";


                    QByteArray gameout2 = Data.fromHex(Data);

                    QHostAddress newsender = QHostAddress(changeIP);
                    worldthread::udpsocket3->writeDatagram(gameout2,newsender,current_client_port);

                    MainWindow::UiOputVector.append("");
                    MainWindow::UiOputVector.append("");
                    MainWindow::UiOputVector.append("----------------------------------------------------------");
                    MainWindow::UiOputVector.append(dateTimeString);
                    MainWindow::UiOputVector.append("Opcodes: " + Description);
                    MainWindow::UiOputVector.append("To Client: " + MainToon);
                    MainWindow::UiOputVector.append("IP: " + ClientsIP);
                    MainWindow::UiOputVector.append("Port: " + ClientsPort);
                    MainWindow::UiOputVector.append("From Server: Packet:");
                    MainWindow::UiOputVector.append(Data);
                    MainWindow::UiOputVector.append("----------------------------------------------------------");
                    MainWindow::UiOputVector.append("");

                }

                if(Description.contains("Switch_Clients"))
                {

                    QString SendToID = Description.mid(0,8);


                    QVector<QByteArray> TempVect = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(SendToID);

                    if(TempVect.isEmpty() == true)
                    {
                        SendToID = Opcodes::ObjectIDsMap.key(SendToID);
                        TempVect = CharCreate::CharID_clientID_clientIP_clientPORTvectorMap.value(SendToID);
                    }

                    QByteArray ClientID_out = TempVect.at(0);
                    QByteArray ClientIP_out = TempVect.at(1);
                    QByteArray ClientPort_out = TempVect.at(2);

                    packetparsing::packetvars pG;
                    pG = packetparsing::IPandPort_AllvariablesMap.value(ClientID_out + ClientIP_out + ClientPort_out);

                    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(pG.CharSelectID);
                    QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                    QByteArray holderGrpOut = pG.serverid1 + pG.clientid1 + "92c001bf543213" + pG.SessionID + "00xxxx";

                    qDebug() << "pG.CharSelectID" << pG.CharSelectID;
                    qDebug() << "IN_Opcodes.CharSelectID" << IN_Opcodes.CharSelectID;

                    if(IN_Opcodes.CharSelectID == pG.CharSelectID)
                    {
                        holderGrpOut.replace(28,2,"03");
                        holderGrpOut.append(pG.Clients_Last_MessageNumber);
                        holderGrpOut.append(pG.Clients_Last_FB_MessageNumber);
                    }

                    holderGrpOut.append(Data);

                    increment GrpOut;
                    QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                    holderGrpOut.replace(30,4,GrpOut_a);
                    Servers_Last_MessageNumber = GrpOut_a.toUpper();

                    packetsize ResizePacket;
                    QByteArray ResizePacket2 = ResizePacket.switchsize(holderGrpOut);
                    holderGrpOut.replace(8,4,ResizePacket2.toUpper());

                    MessNumbVect.replace(0,Servers_Last_MessageNumber);

                    Opcodes::CharID_ServerMessageNumbers.insert(pG.CharSelectID,MessNumbVect);

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID_out + ClientIP_out + ClientPort_out,pG);




                    QHostAddress current_client_address;
                    current_client_address.setAddress(QString::fromUtf8(ClientIP_out));
                    quint16 current_client_port = ClientPort_out.toUShort(nullptr,10);
                    QString changeIP = current_client_address.toString();

                    crc sendcrc;
                    QByteArray outcrc =  sendcrc.elcrc(holderGrpOut);
                    holderGrpOut.append(outcrc);

                    QString MainToon = checkthezone::NPCsNames.value(pG.CharSelectID);


                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

                    qDebug() << "";
                    qDebug() << "";
                    qDebug() << "";
                    qDebug() << dateTimeString;

                    qDebug() << "Player_Interactions" << Description;
                    qDebug() << "To Client: " << MainToon;
                    qDebug() << "IP: " << ClientsIP;
                    qDebug() << "Port: " << ClientsPort;
                    qDebug() << "From Server: ";
                    qDebug() << holderGrpOut;
                    qDebug() << "";

                    QByteArray gameout2 = holderGrpOut.fromHex(holderGrpOut);

                    QHostAddress newsender = QHostAddress(changeIP);
                    worldthread::udpsocket3->writeDatagram(gameout2,newsender,current_client_port);

                    MainWindow::UiOputVector.append("");
                    MainWindow::UiOputVector.append("");
                    MainWindow::UiOputVector.append("----------------------------------------------------------");
                    MainWindow::UiOputVector.append(dateTimeString);
                    MainWindow::UiOputVector.append("Player_Interactions" + Description);
                    MainWindow::UiOputVector.append("To Client: " + MainToon);
                    MainWindow::UiOputVector.append("IP: " + ClientsIP);
                    MainWindow::UiOputVector.append("Port: " + ClientsPort);
                    MainWindow::UiOputVector.append("From Server: Packet:");
                    MainWindow::UiOputVector.append(Data);
                    MainWindow::UiOputVector.append("----------------------------------------------------------");
                    MainWindow::UiOputVector.append("");


                }
            }



            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);

            qDebug() << "IN_Opcodes.OpcodeReturn.size()5 = " << IN_Opcodes.OpcodeReturn.size();

        }while(IN_Opcodes.OpcodeReturn.size() > 0);

        IN_Opcodes.OpcodesAndGamebuf1Map.clear();
        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_Opcodes);
        IN_Opcodes = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

        qDebug() << "IN_Opcodes.CharSelectID check = " << IN_Opcodes.CharSelectID;

    }


    qDebug() << "IN_Opcodes.CurrentQuestMenuID_5" << IN_Opcodes.CurrentQuestMenuID;


    return "IN_Opcodes";

}

QByteArray Opcodes::DisconnectCheck(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort)
{

    packetparsing::packetvars  IN_DisconnectCheck;
    IN_DisconnectCheck = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    qDebug() << "IN_DisconnectCheck.ReplyConfirm " << IN_DisconnectCheck.ReplyConfirm.mid(4,4);
    QByteArray ReplyCheck = IN_DisconnectCheck.ReplyConfirm.mid(4,4).toUpper();
    QByteArray RC1stByte = ReplyCheck.mid(0,2);
    QByteArray RC2ndByte = ReplyCheck.mid(2,2);
    QByteArray revRC = RC2ndByte + RC1stByte;

    int RCrevInt = revRC.toInt(nullptr,16);

    qDebug() << "IN_DisconnectCheck.OldConnectionCount " << IN_DisconnectCheck.OldConnectionCount;
    QByteArray OC1stByte = IN_DisconnectCheck.OldConnectionCount.mid(0,2);
    QByteArray OC2ndByte = IN_DisconnectCheck.OldConnectionCount.mid(2,2);
    QByteArray revOC = OC2ndByte + OC1stByte;

    int OCrevInt = revOC.toInt(nullptr,16);

    qDebug() << "RCrevInt " << RCrevInt;
    qDebug() << "OCrevInt " << OCrevInt;

    if(RCrevInt >= OCrevInt)
    {
        return "KeepAlive";
    }
    else
    {
        return "Disconnect";
    }

    return "";
}

QByteArray Opcodes::ConvertSession(QString SessionID1)
{
    qDebug() << "";
    qDebug() << "ObjectID to SessionID";
    qDebug() << "SessionID1" << SessionID1;


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
        qDebug() << "object1" << object1;

        uchar lower7bits = (uchar)(object1 & 0x7f);
        object1 >>= 7;

        if (object1 > 0)

            lower7bits |= 128;
        uchar add(lower7bits);
        test2.append(add);

        qDebug() << "add" << add;
        qDebug() << "Val" << test2.toHex().toUpper();
        qDebug() << "lower7bits" << lower7bits;


        test3.append(lower7bits);


    } while (object1 > 0);

    qDebug() << "test3" << test3.toHex().toUpper();


    qDebug() << "SessionID2" << test2;

    return test2.toHex().toUpper();
}

