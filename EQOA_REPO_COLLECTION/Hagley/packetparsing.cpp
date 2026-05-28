#include "packetparsing.h"
#include "increment.h"
#include "crc.h"
#include "loadingcharacter.h"
#include "fortytwentyninedec.h"
#include "opcodes.h"
#include "flipped.h"
#include "worldthread.h"
#include "mainwindow.h"
#include "checkthezone.h"
#include "quests.h"

packetparsing::packetparsing(QObject *parent) : QObject(parent)
{

}

QMap<QByteArray, packetparsing::packetvars> packetparsing::ParseThePacket(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QByteArray ClientsPacket, QMap<QByteArray, packetparsing::packetvars> IPandPort_AllvariablesMap)
{
    packetparsing::packetvars IN_parsing;
    IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    //qDebug() << "";
    //qDebug() << "Inside of ParseThePacket packetparsing.cpp";

    //qDebug() << "IN_parsing.CurrentQuestMenuID_6" << IN_parsing.CurrentQuestMenuID;

    //1C1C0A1FCAE0211D1C5A00200100FB060100000025000000FB390200010000030000000400000045514F4105000000546573743801ED18C727C06E5786C3035A08E5A041E0B71C12AE76C498FDF3CEEB444A0A49B5


    increment sendto;

    ClientsPacket.remove(ClientsPacket.size() -8,8);//remove crc

    //qDebug() << "IN_parsing.UserName1" << IN_parsing.UserName;


    //02500A1F87E0210350FA00230400020001008770BF543213DABC0D23060006000700FF34AF08
    //A0920A1F80E014A1920E00A1920E008770BF543213DABC0D031B001B001C00C75BED8C


    QByteArray PacketCloser = ClientsPacket.mid(12,2);

    QByteArray RemoveFromGamebuf1 = ClientsPacket.toUpper();//remove crc;

    QVector<QByteArray> ReplyVector =
    {
        "00","03","13","20","23","0B","1B","2B","3B","63"
    };


    IN_parsing.gamebuf1 = ClientsPacket;

    IN_parsing.TrimTheGamebuf1 = IN_parsing.gamebuf1;

    //qDebug() << "IN_parsing.PreCharSelect = " << IN_parsing.PreCharSelect;

    if(PacketCloser != "14")
    {
        IN_parsing.PacketReply = "XXXX";

        if(IN_parsing.PreCharSelect == true)
        {


            IN_parsing.PacketReply = ClientsPacket.mid(22,2);

            //B8CE0A1F8B70F1463213D9BC0D63F1463213010001000200
            //CE660A1F8B70F1463213D9BC0D63F1463213010001000200

            QByteArray F1463213 = ClientsPacket.mid(12,8);
            QByteArray Session = ClientsPacket.mid(20,6);
            QByteArray PacketReply = ClientsPacket.mid(26,2);


            //qDebug() << "F1463213 = " << F1463213;
            //qDebug() << "Session = " << Session;
            //qDebug() << "IN_parsing.SessionID = " << IN_parsing.SessionID;
            //qDebug() << "PacketReply = " << PacketReply;

            if(F1463213 == "F1463213" && Session == IN_parsing.SessionID)
            {
                //qDebug() << "Changing values and positions";
                IN_parsing.PacketReply = PacketReply;
                IN_parsing.ReplyConfirm = ClientsPacket.mid(40,8);
                IN_parsing.TrimTheGamebuf1.remove(0,36);
                IN_parsing.Clients_Last_MessageNumber =  IN_parsing.gamebuf1.mid(36,4);
                RemoveFromGamebuf1.remove(0,36);
            }
            else
            {
                if(IN_parsing.PacketReply == "00" || IN_parsing.PacketReply == "03" ||
                        IN_parsing.PacketReply == "20" || IN_parsing.PacketReply == "23")
                {
                    IN_parsing.TrimTheGamebuf1.remove(0,28);
                    IN_parsing.Clients_Last_MessageNumber =  IN_parsing.gamebuf1.mid(24,4);
                    RemoveFromGamebuf1.remove(0,24);

                    if(IN_parsing.PacketReply == "00" || IN_parsing.PacketReply == "20")
                    {
                        //IN_parsing.ReplyConfirm = "";
                    }
                    else
                    {
                        IN_parsing.ReplyConfirm = ClientsPacket.mid(28,8);
                    }
                }

                if(IN_parsing.PacketReply == "13")
                {
                    if(IN_parsing.PacketReply == "13")
                    {
                        IN_parsing.TrimTheGamebuf1.remove(0,28);
                        IN_parsing.Clients_Last_MessageNumber =  IN_parsing.gamebuf1.mid(24,4);
                        RemoveFromGamebuf1.remove(0,24);
                        IN_parsing.ReplyConfirm = ClientsPacket.mid(28,8);
                    }
                }
            }

            //qDebug() << "IN_parsing.PacketReply = " << IN_parsing.PacketReply;
            //qDebug() << "IN_parsing.ReplyConfirm = " << IN_parsing.ReplyConfirm;
        }
        else
        {
            IN_parsing.PacketReply = ClientsPacket.mid(26,2);

            QByteArray ClientsID_inc1 = sendto.count(ClientID);
            QByteArray ClientsID_inc2 = ClientsPacket.mid(14,4);

            //qDebug() << "";
            //qDebug() << "ClientsID_inc1 = " << ClientsID_inc1;
            //qDebug() << "ClientsID_inc2 = " << ClientsID_inc2;

            //02500A1F87E0210350FA00230400020001008770BF543213DABC0D23060006000700FF34AF08


            int skip = 0;

            if(ClientsID_inc1 == ClientsID_inc2 && ClientsPacket.contains("BF543213"))
            {
                int index = ClientsPacket.indexOf("BF543213");
                //qDebug() << "index = " << index;
                IN_parsing.PacketReply = ClientsPacket.mid(index +14,2);
                //qDebug() << "IN_parsing.PacketReply = " << IN_parsing.PacketReply;
                RemoveFromGamebuf1.remove(0,index +16);
                //qDebug() << "RemoveFromGamebuf1 = " << RemoveFromGamebuf1;

                IN_parsing.Clients_Last_MessageNumber = ClientsPacket.mid(index +16,4);
                //qDebug() << "IN_parsing.Clients_Last_MessageNumber = " << IN_parsing.Clients_Last_MessageNumber;
                IN_parsing.TrimTheGamebuf1.remove(0,index +16);
                //qDebug() << "IN_parsing.TrimTheGamebuf1 = " << IN_parsing.TrimTheGamebuf1;
                skip = 1;
                IN_parsing.DoubleSession = true;
                //qDebug() << "IN_parsing.DoubleSession = " << IN_parsing.DoubleSession;
                IN_parsing.ReplyConfirm = ClientsPacket.mid(index +20,8);
                //qDebug() << "IN_parsing.ReplyConfirm = " << IN_parsing.ReplyConfirm;
            }


            if(ClientsID_inc1 == ClientsID_inc2 && skip == 0)
            {

                //54670A1F87E02155672F00230400020001009D70F1463213DBBC0D200300FB1601002A000600000000000000030000000300000005000000E55242EF


                if(ClientsID_inc1 == ClientsID_inc2 && ClientsPacket.contains("F1463213"))
                {
                    QByteArray _1st = ClientsPacket.mid(22,2);
                    QByteArray _2nd = ClientsPacket.mid(40,4);

                    if(_1st == "23" && _2nd == "F1463213")
                    {
                        int index = ClientsPacket.indexOf("F1463213");
                        //qDebug() << "index = " << index;
                        IN_parsing.PacketReply = ClientsPacket.mid(index +14,2);
                        //qDebug() << "IN_parsing.PacketReply = " << IN_parsing.PacketReply;
                        RemoveFromGamebuf1.remove(0,index +16);
                        //qDebug() << "RemoveFromGamebuf1 = " << RemoveFromGamebuf1;

                        IN_parsing.Clients_Last_MessageNumber = ClientsPacket.mid(index +16,4);
                        //qDebug() << "IN_parsing.Clients_Last_MessageNumber = " << IN_parsing.Clients_Last_MessageNumber;
                        IN_parsing.TrimTheGamebuf1.remove(0,index +16);
                        //qDebug() << "IN_parsing.TrimTheGamebuf1 = " << IN_parsing.TrimTheGamebuf1;
                        skip = 1;
                        IN_parsing.DoubleSession = true;
                        //qDebug() << "IN_parsing.DoubleSession = " << IN_parsing.DoubleSession;

                        RemoveFromGamebuf1.remove(0,52);
                    }
                }
                else
                {
                    if(IN_parsing.PacketReply == "00" || IN_parsing.PacketReply == "20")
                    {
                        //IN_parsing.ReplyConfirm = "";
                    }
                    else
                    {
                        IN_parsing.ReplyConfirm = ClientsPacket.mid(28,8);
                    }
                }
            }
            else
            {
                if(skip == 0)
                {

                    if(IN_parsing.PacketReply == "00" || IN_parsing.PacketReply == "03" ||
                            IN_parsing.PacketReply == "20" || IN_parsing.PacketReply == "23")
                    {
                        IN_parsing.TrimTheGamebuf1.remove(0,32);
                        IN_parsing.Clients_Last_MessageNumber =  IN_parsing.gamebuf1.mid(28,4);
                        RemoveFromGamebuf1.remove(0,28);

                        if(IN_parsing.PacketReply == "00" || IN_parsing.PacketReply == "20")
                        {
                            //IN_parsing.ReplyConfirm = "";
                        }
                        else
                        {
                            IN_parsing.ReplyConfirm = ClientsPacket.mid(32,8);
                        }
                    }

                    //52A40A1F9A70BF543213D9BC0D131E001D002300000100420100F8402902000116798119040800
                    //52A40A1F8B70BF543213D9BC0D63BF543213010001000200
                    if(IN_parsing.PacketReply == "13" || IN_parsing.PacketReply == "63")
                    {
                        if(IN_parsing.PacketReply == "13")
                        {
                            IN_parsing.Clients_Last_MessageNumber =  IN_parsing.gamebuf1.mid(28,4);
                            RemoveFromGamebuf1.remove(0,28);
                            IN_parsing.ReplyConfirm = ClientsPacket.mid(32,8);
                        }
                        else
                        {
                            IN_parsing.Clients_Last_MessageNumber =  IN_parsing.gamebuf1.mid(36,4);
                            RemoveFromGamebuf1.remove(0,36);
                            IN_parsing.ReplyConfirm = ClientsPacket.mid(40,8);
                        }

                        IN_parsing.TrimTheGamebuf1.remove(0,40);
                    }
                }
            }
        }
    }
    else
    {
        QByteArray ClientsID_inc1 = sendto.count(ClientID);
        QByteArray ClientsID_inc2 = ClientsPacket.mid(14,4);

        //qDebug() << "";
        //qDebug() << "ClientsID_inc1 = " << ClientsID_inc1;
        //qDebug() << "ClientsID_inc2 = " << ClientsID_inc2;

        //02500A1F87E0210350FA00230400020001008770BF543213DABC0D23060006000700FF34AF08

        if(ClientsID_inc1 == ClientsID_inc2 && ClientsPacket.contains("BF543213"))
        {
            int index = ClientsPacket.indexOf("BF543213");
            //qDebug() << "index = " << index;
            IN_parsing.PacketReply = ClientsPacket.mid(index +14,2);
            //qDebug() << "IN_parsing.PacketReply = " << IN_parsing.PacketReply;
            RemoveFromGamebuf1.remove(0,index +16);
            //qDebug() << "RemoveFromGamebuf1 = " << RemoveFromGamebuf1;

            IN_parsing.Clients_Last_MessageNumber = ClientsPacket.mid(index +16,4);
            //qDebug() << "IN_parsing.Clients_Last_MessageNumber = " << IN_parsing.Clients_Last_MessageNumber;
            IN_parsing.TrimTheGamebuf1.remove(0,index +16);
            //qDebug() << "IN_parsing.TrimTheGamebuf1 = " << IN_parsing.TrimTheGamebuf1;
            IN_parsing.DoubleSession = true;
            //qDebug() << "IN_parsing.DoubleSession = " << IN_parsing.DoubleSession;
            IN_parsing.ReplyConfirm = ClientsPacket.mid(index +20,8);
            //qDebug() << "IN_parsing.ReplyConfirm = " << IN_parsing.ReplyConfirm;
        }


    }

    //qDebug() << "IN_parsing.PacketReply = " << IN_parsing.PacketReply;

    if(ReplyVector.contains(IN_parsing.PacketReply))
    {

        //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber 1st check = " << IN_parsing.Clients_Last_FB_MessageNumber;


        //qDebug() << "RemoveFromGamebuf1_1 = " << RemoveFromGamebuf1;

        if(IN_parsing.PacketReply == "0B" || IN_parsing.PacketReply == "1B" ||
                IN_parsing.PacketReply == "2B" || IN_parsing.PacketReply == "3B")
        {
            //80920A1FA770BF543213DABC0D1B3A003A002900010012000F0200F840291200012201DB12A5210212113B130383156D81020100

            IN_parsing.TrimTheGamebuf1.remove(0,32);
            IN_parsing.Clients_Last_MessageNumber =  IN_parsing.gamebuf1.mid(28,4);
            RemoveFromGamebuf1.remove(0,28);

            IN_parsing.ReplyConfirm = ClientsPacket.mid(32,8);

            QVector<QByteArray> tempVect = Opcodes::CharID_ServerMessageNumbers.value(IN_parsing.CharSelectID);

            QByteArray Servers_Last_MessageNumber = tempVect.at(0);
            QByteArray Servers_Last_FB_MessageNumber = tempVect.at(1);

            //qDebug() << "";
            //qDebug() << "Bad Packet = " << IN_parsing.gamebuf1;
            //qDebug() << "Servers_Last_MessageNumber" << Servers_Last_MessageNumber;
            //qDebug() << "Servers_Last_FB_MessageNumber" << Servers_Last_FB_MessageNumber;
            //qDebug() << "IN_parsing_Client_thinks_Servers_Last_MessageNumber" << IN_parsing.gamebuf1.mid(32,4);
            //qDebug() << "IN_parsing_Client_thinks_Servers_Last_FB_MessageNumber" << IN_parsing.gamebuf1.mid(36,4);
            //qDebug() << "";



            tempVect.replace(0,IN_parsing.gamebuf1.mid(32,4));
            tempVect.replace(1,IN_parsing.gamebuf1.mid(36,4));

            IN_parsing.ResendAllFBs = true;



            Opcodes::CharID_ServerMessageNumbers.insert(IN_parsing.CharSelectID,tempVect);

        }

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_parsing);


        if(IN_parsing.gamebuf1.contains("FB0201001400FB0302000E0000FB040300C9000000FB040400CA000000"))
        {

            IN_parsing.FBMessageNumber = "0400";

            IN_parsing.Clients_Last_FB_MessageNumber = IN_parsing.FBMessageNumber;
            IN_parsing.opcoderesult1 = "";
            RemoveFromGamebuf1.remove(0,62);

            IN_parsing.TrimTheGamebuf1 = RemoveFromGamebuf1;

            RemoveFromGamebuf1.clear();
        }
        else
        {
            int indexFB = RemoveFromGamebuf1.indexOf("FB"); //find FB
            int indexF9 = RemoveFromGamebuf1.indexOf("F901");
            int indexFBtrim = IN_parsing.TrimTheGamebuf1.indexOf("FB"); //find FB
            int indexF9trim = IN_parsing.TrimTheGamebuf1.indexOf("F901");

            //qDebug() << "";


            if(indexFB != -1 || indexF9 != -1)
            {
                do
                {
                    //qDebug() << "";

                    QVector<QByteArray> OpcodeInfoVector;
                    IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

                    indexFB = RemoveFromGamebuf1.indexOf("FB");
                    indexF9 = RemoveFromGamebuf1.indexOf("F901");
                    indexFBtrim = IN_parsing.TrimTheGamebuf1.indexOf("FB"); //find FB
                    indexF9trim = IN_parsing.TrimTheGamebuf1.indexOf("F901");

                    //qDebug() << "indexFB_1 = " << indexFB;
                    //qDebug() << "indexF9_1 = " << indexF9;
                    //qDebug() << "indexFBtrim_1 = " << indexFBtrim;
                    //qDebug() << "indexF9trim_1 = " << indexF9trim;

                    if(indexFB != -1 || indexF9 != -1)
                    {


                        if(indexFB != -1)
                        {

                            int indexSize = indexFB + 2;

                            QString SizeOF = RemoveFromGamebuf1.mid(indexSize,2).toUpper();

                            if(SizeOF == "FF")
                            {
                                SizeOF = RemoveFromGamebuf1.mid(indexSize + 2,4).toUpper();

                                QString Byte1 = SizeOF.mid(0,2);
                                QString Byte2 = SizeOF.mid(2,2);
                                QString RevBytes = Byte2 + Byte1;

                                int RevBytes1 = RevBytes.toInt(nullptr,16);
                                int RevBytes2 = RevBytes1 * 2;

                                QByteArray FullFB = RemoveFromGamebuf1.mid(indexFB,RevBytes2 + 12).toUpper();

                                //qDebug() << "IN_parsing.FullFB = " << FullFB;


                                QByteArray incthis1 = sendto.count(IN_parsing.Clients_Last_FB_MessageNumber).toUpper();//0600
                                QByteArray incthis2 = sendto.count(incthis1).toUpper();//0700
                                QByteArray incthis3 = sendto.count(incthis2).toUpper();//0800
                                QByteArray FromClient = FullFB.mid(8,4).toUpper();

                                QByteArray incthis4 = sendto.count(FromClient).toUpper();

                                if(
                                        FromClient.toUpper() != incthis1.toUpper() &&//not true
                                        FromClient.toUpper() != incthis2.toUpper() &&//not true
                                        FromClient.toUpper() != incthis3.toUpper() &&//not true
                                        FromClient.toUpper() != IN_parsing.Clients_Last_FB_MessageNumber.toUpper()//not true
                                        && incthis4.toUpper() != IN_parsing.Clients_Last_FB_MessageNumber.toUpper())
                                {
                                    IN_parsing.FBMessageNumber =  IN_parsing.FBMessageNumber.toUpper();
                                    IN_parsing.Clients_Last_FB_MessageNumber = IN_parsing.FBMessageNumber;

                                    //qDebug() << "if(SizeOF == FF) then if from client";
                                    //qDebug() << "IN_parsing.FBMessageNumber = " << IN_parsing.FBMessageNumber;
                                    //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber = " << IN_parsing.Clients_Last_FB_MessageNumber;

                                }
                                else
                                {
                                    //qDebug() << "if(SizeOF == FF) then else";

                                    QByteArray opcoderesult1 = FullFB.mid(12,4).toUpper();

                                    if(opvect.contains(opcoderesult1)) //check against the list to see if its a real opcode
                                    {
                                        IN_parsing.FullFB = FullFB;
                                        IN_parsing.opcoderesult1 = opcoderesult1;

                                        if(IN_parsing.OpcodesAndGamebuf1Map.contains("0E00"))
                                        {
                                            IN_parsing.opcoderesult1 = "XXXX";
                                        }

                                        //qDebug() << "IN_parsing.opcoderesult1 = " << IN_parsing.opcoderesult1;

                                        IN_parsing.FBMessageNumber = FromClient.toUpper();

                                        OpcodeInfoVector.append(IN_parsing.Clients_Last_MessageNumber);
                                        OpcodeInfoVector.append(IN_parsing.FBMessageNumber);
                                        OpcodeInfoVector.append(IN_parsing.FullFB);
                                        OpcodeInfoVector.append(IN_parsing.gamebuf1);
                                        IN_parsing.TrimTheGamebuf1.remove(indexFB,IN_parsing.FullFB.size());
                                        IN_parsing.Clients_Last_FB_MessageNumber = IN_parsing.FBMessageNumber;

                                        //qDebug() << "IN_parsing.Clients_Last_MessageNumber = " << IN_parsing.Clients_Last_MessageNumber;
                                        //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber = " << IN_parsing.Clients_Last_FB_MessageNumber;
                                        //qDebug() << "IN_parsing.FBMessageNumber = " << IN_parsing.FBMessageNumber;
                                        //qDebug() << "IN_parsing.FullFB = " << IN_parsing.FullFB;
                                        //qDebug() << "IN_parsing.TrimTheGamebuf1 = " << IN_parsing.TrimTheGamebuf1;
                                        //qDebug() << "IN_parsing.FBMessageNumber = " << IN_parsing.FBMessageNumber;


                                        IN_parsing.OpcodesAndGamebuf1Map.insert(IN_parsing.opcoderesult1,OpcodeInfoVector);
                                    }
                                }
                            }
                            else
                            {
                                int SizeOF1 = SizeOF.toInt(nullptr,16);
                                int SizeOF2 = SizeOF1 * 2;

                                QByteArray FullFB = RemoveFromGamebuf1.mid(indexFB,SizeOF2 + 8).toUpper();


                                //qDebug() << "FullFB = " << FullFB;

                                QByteArray FromClient = FullFB.mid(4,4).toUpper();
                                QByteArray opcoderesult1 = FullFB.mid(8,4).toUpper();

                                if(opvect.contains(opcoderesult1)) //check against the list to see if its a real opcode
                                {

                                    if((opcoderesult1 == "2A00" && IN_parsing.flipped == false && IN_parsing.Clients_Last_FB_MessageNumber == "0200")
                                            || (FullFB == "FB060100000025000000" && IN_parsing.Flip163 == false))
                                    {
                                        IN_parsing.Clients_Last_FB_MessageNumber = "0000";
                                    }


                                    increment sendto;
                                    QByteArray incthis1 = sendto.count(IN_parsing.Clients_Last_FB_MessageNumber).toUpper();//0600
                                    QByteArray incthis2 = sendto.count(incthis1).toUpper();//0700
                                    QByteArray incthis3 = sendto.count(incthis2).toUpper();//0800

                                    QByteArray incthis4 = sendto.count(FromClient).toUpper();

                                    //qDebug() << "IN_parsing.FromClient = " << FromClient.toUpper();
                                    //qDebug() << "IN_parsing.incthis1 = " << incthis1.toUpper();
                                    //qDebug() << "IN_parsing.incthis2 = " << incthis2.toUpper();
                                    //qDebug() << "IN_parsing.incthis3 = " << incthis3.toUpper();
                                    //qDebug() << "IN_parsing.incthis4 = " << incthis4.toUpper();
                                    //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber = " << IN_parsing.Clients_Last_FB_MessageNumber;


                                    if(
                                            FromClient.toUpper() != incthis1.toUpper() &&//not true
                                            FromClient.toUpper() != incthis2.toUpper() &&//not true
                                            FromClient.toUpper() != incthis3.toUpper() &&//not true
                                            FromClient.toUpper() != IN_parsing.Clients_Last_FB_MessageNumber.toUpper()//not true
                                            && incthis4.toUpper() != IN_parsing.Clients_Last_FB_MessageNumber.toUpper())
                                    {
                                        IN_parsing.FBMessageNumber =  IN_parsing.FBMessageNumber.toUpper();
                                        IN_parsing.Clients_Last_FB_MessageNumber = IN_parsing.FBMessageNumber;

                                        //qDebug() << "if(SizeOF != FF) then if from client";
                                        //qDebug() << "IN_parsing.FBMessageNumber = " << IN_parsing.FBMessageNumber;
                                        //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber = " << IN_parsing.Clients_Last_FB_MessageNumber;

                                    }
                                    else
                                    {
                                        //qDebug() << "if(SizeOF != FF) then else";

                                        IN_parsing.FullFB = FullFB;
                                        IN_parsing.opcoderesult1 = opcoderesult1;

                                        if(IN_parsing.OpcodesAndGamebuf1Map.contains("0E00"))
                                        {
                                            IN_parsing.opcoderesult1 = "XXXX";
                                        }

                                        //qDebug() << "IN_parsing.opcoderesult1 = " << IN_parsing.opcoderesult1;

                                        IN_parsing.Clients_Last_FB_MessageNumber = FromClient.toUpper();

                                        IN_parsing.FBMessageNumber = FromClient.toUpper();

                                        OpcodeInfoVector.append(IN_parsing.Clients_Last_MessageNumber);
                                        OpcodeInfoVector.append(FromClient);
                                        OpcodeInfoVector.append(IN_parsing.FullFB);
                                        OpcodeInfoVector.append(IN_parsing.gamebuf1);
                                        IN_parsing.OpcodesAndGamebuf1Map.insert(IN_parsing.opcoderesult1,OpcodeInfoVector);

                                        //qDebug() << "IN_parsing.Clients_Last_MessageNumber = " << IN_parsing.Clients_Last_MessageNumber;
                                        //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber = " << IN_parsing.Clients_Last_FB_MessageNumber;
                                        //qDebug() << "IN_parsing.FBMessageNumber = " << IN_parsing.FBMessageNumber;
                                        //qDebug() << "IN_parsing.FullFB = " << IN_parsing.FullFB;
                                        //qDebug() << "IN_parsing.TrimTheGamebuf1 = " << IN_parsing.TrimTheGamebuf1;
                                        //qDebug() << "IN_parsing.FBMessageNumber = " << IN_parsing.FBMessageNumber;


                                        if(indexFBtrim != -1)
                                        {
                                            IN_parsing.TrimTheGamebuf1.remove(indexFBtrim,IN_parsing.FullFB.size());

                                            //qDebug() << "IN_parsing.TrimTheGamebuf1 = " << IN_parsing.TrimTheGamebuf1;
                                        }

                                        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_parsing);
                                    }
                                }
                            }

                            RemoveFromGamebuf1.remove(indexFB,2);
                        }


                        //F901 reply

                        if(indexF9 != -1)
                        {
                            //qDebug() << "Inside F901's";
                            //F901050012
                            QByteArray FullFB = RemoveFromGamebuf1.mid(indexF9,10).toUpper();

                            //qDebug() << "FullFB = " << FullFB;

                            QByteArray opcoderesult1 = FullFB.mid(8,2).toUpper();

                            //qDebug() << "opcoderesult1 = " << opcoderesult1;

                            //qDebug() << "IN_parsing.Flip163 = " << IN_parsing.Flip163;
                            //qDebug() << "IN_parsing.flipped = " << IN_parsing.flipped;
                            //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber = " << IN_parsing.Clients_Last_FB_MessageNumber;

                            if(opvect.contains(opcoderesult1)) //check against the list to see if its a real opcode
                            {
                                IN_parsing.FullFB = FullFB;
                                IN_parsing.opcoderesult1 = opcoderesult1;

                                QByteArray FromClient = IN_parsing.FullFB.mid(4,4).toUpper();
                                QByteArray Size = IN_parsing.FullFB.mid(2,2).toUpper();
                                QByteArray twelve = IN_parsing.FullFB.mid(8,2).toUpper();

                                increment sendto;
                                QByteArray incthis1 = sendto.count(IN_parsing.Clients_Last_FB_MessageNumber).toUpper();
                                QByteArray incthis2 = sendto.count(incthis1).toUpper();//0700
                                QByteArray incthis3 = sendto.count(incthis2).toUpper();
                                QByteArray incthis4 = sendto.count(FromClient).toUpper();

                                //qDebug() << "IN_parsing.FromClient = " << FromClient.toUpper();
                                //qDebug() << "IN_parsing.incthis1 = " << incthis1.toUpper();
                                //qDebug() << "IN_parsing.incthis2 = " << incthis2.toUpper();
                                //qDebug() << "IN_parsing.incthis3 = " << incthis3.toUpper();
                                //qDebug() << "IN_parsing.incthis4 = " << incthis4.toUpper();
                                //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber = " << IN_parsing.Clients_Last_FB_MessageNumber;


                                //qDebug() << "Size = 01" << Size;
                                //qDebug() << "twelve = 12" << twelve;

                                if(Size == "01" && twelve == "12")
                                {
                                    if(FromClient.toUpper() != incthis1.toUpper() &&//not true
                                            FromClient.toUpper() != incthis2.toUpper() &&//not true
                                            FromClient.toUpper() != incthis3.toUpper() &&//not true
                                            FromClient.toUpper() != IN_parsing.Clients_Last_FB_MessageNumber.toUpper()//not true
                                            && incthis4.toUpper() != IN_parsing.Clients_Last_FB_MessageNumber.toUpper())
                                    {
                                        IN_parsing.FBMessageNumber =  IN_parsing.FBMessageNumber.toUpper();
                                        IN_parsing.Clients_Last_FB_MessageNumber = IN_parsing.FBMessageNumber;

                                        //qDebug() << "if(From client then) ";
                                        //qDebug() << "IN_parsing.Clients_Last_MessageNumber = " << IN_parsing.Clients_Last_MessageNumber;
                                        //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber = " << IN_parsing.Clients_Last_FB_MessageNumber;
                                        //qDebug() << "IN_parsing.FBMessageNumber = " << IN_parsing.FBMessageNumber;
                                        //qDebug() << "IN_parsing.FullFB = " << IN_parsing.FullFB;
                                        //qDebug() << "IN_parsing.TrimTheGamebuf1 = " << IN_parsing.TrimTheGamebuf1;
                                        //qDebug() << "IN_parsing.FBMessageNumber = " << IN_parsing.FBMessageNumber;
                                    }
                                    else
                                    {
                                        //qDebug() << "if(From client else) ";
                                        IN_parsing.Clients_Last_FB_MessageNumber = FromClient.toUpper();

                                        IN_parsing.FBMessageNumber = FromClient.toUpper();

                                        OpcodeInfoVector.append(IN_parsing.Clients_Last_MessageNumber);
                                        OpcodeInfoVector.append(FromClient);
                                        OpcodeInfoVector.append(IN_parsing.FullFB);
                                        OpcodeInfoVector.append(IN_parsing.gamebuf1);
                                        IN_parsing.OpcodesAndGamebuf1Map.insert(IN_parsing.opcoderesult1,OpcodeInfoVector);

                                        //qDebug() << "IN_parsing.Clients_Last_MessageNumber = " << IN_parsing.Clients_Last_MessageNumber;
                                        //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber = " << IN_parsing.Clients_Last_FB_MessageNumber;
                                        //qDebug() << "IN_parsing.FBMessageNumber = " << IN_parsing.FBMessageNumber;
                                        //qDebug() << "IN_parsing.FullFB = " << IN_parsing.FullFB;
                                        //qDebug() << "IN_parsing.TrimTheGamebuf1 = " << IN_parsing.TrimTheGamebuf1;
                                        //qDebug() << "IN_parsing.FBMessageNumber = " << IN_parsing.FBMessageNumber;



                                        if(indexF9trim != -1)
                                        {
                                            IN_parsing.TrimTheGamebuf1.remove(indexF9trim,IN_parsing.FullFB.size());
                                            //qDebug() << "IN_parsing.TrimTheGamebuf1 = " << IN_parsing.TrimTheGamebuf1;
                                        }

                                        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_parsing);
                                    }
                                }
                            }
                            else
                            {
                                IN_parsing.FBMessageNumber =  IN_parsing.Clients_Last_FB_MessageNumber.toUpper();
                            }

                            RemoveFromGamebuf1.remove(indexF9,2);
                        }
                    }

                }while(RemoveFromGamebuf1.contains("FB") || RemoveFromGamebuf1.contains("F901"));

                IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            }
        }

        if(PacketCloser != "14")
        {

            IN_parsing.combine1 = IN_parsing.Clients_Last_MessageNumber + IN_parsing.Clients_Last_FB_MessageNumber + "40" + IN_parsing.fortytwentninenum1 + "F8";

            QByteArray CombineCheck = IN_parsing.combine1.mid(4,4).toUpper();
            QByteArray CB1stByte = CombineCheck.mid(0,2);
            QByteArray CB2ndByte = CombineCheck.mid(2,2);
            QByteArray revCB = CB2ndByte + CB1stByte;

            int CBrevInt = revCB.toInt(nullptr,16);

            QByteArray LB1stByte = IN_parsing.Clients_Last_FB_MessageNumber.mid(0,2);
            QByteArray LB2ndByte = IN_parsing.Clients_Last_FB_MessageNumber.mid(2,2);
            QByteArray revLB = LB2ndByte + LB1stByte;

            int LBrevInt = revLB.toInt(nullptr,16);

            //qDebug() << "";
            //qDebug() << "CBrevInt = " << CBrevInt;
            //qDebug() << "LBrevInt = " << LBrevInt;
            //qDebug() << "CombineCheck = " << CombineCheck;
            //qDebug() << "IN_parsing.combine1 = " << IN_parsing.combine1;
            //qDebug() << "IN_parsing.Clients_Last_MessageNumber = " << IN_parsing.Clients_Last_MessageNumber;
            //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber = " << IN_parsing.Clients_Last_FB_MessageNumber;


            if(CBrevInt > LBrevInt)
            {
                if(CombineCheck.isEmpty() == false)
                {
                    IN_parsing.Clients_Last_FB_MessageNumber = CombineCheck;

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_parsing);
                    IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
                }
            }

            if(LBrevInt > CBrevInt)
            {
                if(IN_parsing.Clients_Last_FB_MessageNumber.isEmpty() == false)
                {
                    IN_parsing.combine1.replace(4,4,IN_parsing.Clients_Last_FB_MessageNumber);

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_parsing);
                    IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
                }
            }
        }

        if(IN_parsing.PacketReply == "13")//if there was a real opcode it will have removed the opholder and header at this point
        {
            IN_parsing.ClientsCheckChannelConfirm = IN_parsing.TrimTheGamebuf1;



            QByteArray Channel = "";
            QByteArray ChannelsCount = "";

            int iter = 0;

            do
            {
                Channel = IN_parsing.ClientsCheckChannelConfirm.mid(iter,2);
                iter += 2;

                if(Channel == "F8")
                {
                    iter = IN_parsing.ClientsCheckChannelConfirm.size();

                }
                else
                {
                    ChannelsCount = IN_parsing.ClientsCheckChannelConfirm.mid(iter,4);
                    iter += 4;

                    IN_parsing.Client_incrementmap.insert(Channel,ChannelsCount);
                }

            }while(iter < IN_parsing.ClientsCheckChannelConfirm.size());
        }

        //qDebug() << "IN_parsing.ReplyConfirm.mid(4,4) = " << IN_parsing.ReplyConfirm.mid(4,4);
        //qDebug() << "IN_parsing.LastReplyConfirm = " << IN_parsing.LastReplyConfirm;

        increment sendto;
        QByteArray incCount = sendto.count(IN_parsing.LastReplyConfirm);

//        if(IN_parsing.ReplyConfirm.mid(4,4) == IN_parsing.LastReplyConfirm && IN_parsing.ResendFBsMap.contains(incCount))
//        {
//            //qDebug() << "";
//            //qDebug() << "Inside_Forced_Resending";

//            IN_parsing.resendingFBCounter++;

//            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_parsing.CharSelectID);
//            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

//            QByteArray ResendingFB = "";

//            ResendingFB = IN_parsing.ResendFBsMap.value(IN_parsing.LastReplyConfirm);

//            int index = ResendingFB.indexOf(IN_parsing.SessionID);
//            int SessionSize = IN_parsing.SessionID.size();


//            increment GrpOut;
//            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
//            ResendingFB.replace(30,4,GrpOut_a);
//            Servers_Last_MessageNumber = GrpOut_a.toUpper();

//            MessNumbVect.replace(0,Servers_Last_MessageNumber);

//            Opcodes::CharID_ServerMessageNumbers.insert(IN_parsing.CharSelectID,MessNumbVect);

//            increment Resending;
//            QByteArray Resending2 = Resending.count(Servers_Last_MessageNumber);
//            ResendingFB.replace(index+SessionSize,4,Resending2);
//            Servers_Last_MessageNumber = Resending2.toUpper();

//            crc sendcrc;
//            QByteArray outcrc =  sendcrc.elcrc(ResendingFB);
//            ResendingFB.append(outcrc);

//            QHostAddress current_client_address;
//            current_client_address.setAddress(QString::fromUtf8(IN_parsing.ClientsIP));
//            quint16 current_client_port = IN_parsing.ClientsPort.toUShort(nullptr,10);
//            QString changeIP = current_client_address.toString();

//            QByteArray resending = ResendingFB.fromHex(ResendingFB);
//            QHostAddress newsender = QHostAddress(changeIP);
//            worldthread::udpsocket3->writeDatagram(resending,newsender,current_client_port);

//            QString MainToon = checkthezone::NPCsNames.value(IN_parsing.CharSelectID);

//            QDateTime dateTime = dateTime.currentDateTime();
//            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

//            //qDebug() << "";
//            //qDebug() << "";
//            //qDebug() << "----------------------------------------------------------";
//            //qDebug() << dateTimeString;
//            //qDebug() << "Forced Resending FBs: " << IN_parsing.resendingFBCounter;
//            //qDebug() << "To Client: " + MainToon;
//            //qDebug() << "IN_parsing.CharSelectID = " << IN_parsing.CharSelectID;
//            //qDebug() << "IP: " + changeIP;
//            //qDebug() << "Port: " + IN_parsing.ClientsPort;
//            //qDebug() << "From Server: ";
//            //qDebug() << ResendingFB;
//            //qDebug() << "----------------------------------------------------------";
//            //qDebug() << "";
//        }
//        else
//        {
//            IN_parsing.LastReplyConfirm = IN_parsing.ReplyConfirm.mid(4,4);
//        }


        if(!IN_parsing.TrimTheGamebuf1.contains("4029"))
        {
            IN_parsing.TrimTheGamebuf1.clear();
            IN_parsing.fortyexist = false;
        }
        else
        {
            //qDebug() << "It does contain 4029" << IN_parsing.TrimTheGamebuf1;
        }


        if(IN_parsing.OpcodesAndGamebuf1Map.isEmpty() == false)
        {
            IN_parsing.opcodeexist = true;

            if(IN_parsing.FBMessageNumber == "0400" && IN_parsing.OpcodesAndGamebuf1Map.isEmpty() == true && IN_parsing.flipped == false)
            {
                IN_parsing.opcodeexist = false;
            }
        }
        else
        {
            IN_parsing.opcodeexist = false;
        }


        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_parsing);
        IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);


        // ///////////////Send to 4029's/////////////////////////////////
        if(IN_parsing.TrimTheGamebuf1.contains("4029"))
        {
            fortytwentyninedec sendto;
            sendto.fortydec_res( ClientID,ClientsIP,ClientsPort,IN_parsing.gamebuf1);
            IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            //qDebug() << "IN_parsing.fortyexist4" << IN_parsing.fortyexist;
        }
        // ///////////////Send to 4029's/////////////////////////////////


        /////////////////Loading Character data///////////////////////////////////////////

        QHostAddress newsender = QHostAddress(MainWindow::PublicMainip);

        if(IN_parsing.LoginStarted == "Started")
        {
            loadingcharacter SendToLoadData;
            QVector <QByteArray> ReturnLoadData = SendToLoadData.LoadData(ClientID,ClientsIP,ClientsPort,ClientsPacket,newsender);

            if(ReturnLoadData.isEmpty() == false)
            {
                QHostAddress current_client_address;
                current_client_address.setAddress(QString::fromUtf8(ClientsIP));
                quint16 current_client_port = ClientsPort.toUShort(nullptr,10);
                QString changeIP = current_client_address.toString();


                for(int rd = 0; rd < ReturnLoadData.size() -1; rd++)
                {

                    QByteArray Data = ReturnLoadData.at(rd);

                    Data.replace(22,6,IN_parsing.SessionID);

                    crc sendcrc;
                    QByteArray outcrc =  sendcrc.elcrc(Data);
                    Data.append(outcrc);

                    //qDebug() << "Server_PacketMap_Load_Data" << Data;



                    QString MainToon = checkthezone::NPCsNames.value(IN_parsing.CharSelectID);

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

                    //qDebug() << "";
                    //qDebug() << "";
                    //qDebug() << "----------------------------------------------------------";
                    //qDebug() << dateTimeString;
                    //qDebug() << "Load_Data";
                    //qDebug() << "To Client: " + MainToon;
                    //qDebug() << "IP: " + ClientsIP;
                    //qDebug() << "Port: " + ClientsPort;
                    //qDebug() << "From Server: ";
                    //qDebug() << Data;
                    //qDebug() << "----------------------------------------------------------";
                    //qDebug() << "";

                    QByteArray gameout2 = Data.fromHex(Data);

                    QHostAddress newsender = QHostAddress(changeIP);
                    worldthread::udpsocket3->writeDatagram(gameout2,newsender,current_client_port);

                    MainWindow::UiOputVector.append("");
                    MainWindow::UiOputVector.append("");
                    MainWindow::UiOputVector.append("----------------------------------------------------------");
                    MainWindow::UiOputVector.append(dateTimeString);
                    MainWindow::UiOputVector.append("Load_Data: " + QString::number(rd));
                    MainWindow::UiOputVector.append("To Client: " + MainToon);
                    MainWindow::UiOputVector.append("IP: " + ClientsIP);
                    MainWindow::UiOputVector.append("Port: " + ClientsPort);
                    MainWindow::UiOputVector.append("From Server: Packet:");
                    MainWindow::UiOputVector.append(Data);
                    MainWindow::UiOputVector.append("----------------------------------------------------------");
                    MainWindow::UiOputVector.append("");

                }
            }

            IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

        }


        if(IN_parsing.opcodeexist == true)
        {
            Opcodes sendto;
            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_parsing);
            sendto.CheckTheOpcode( ClientID,ClientsIP,ClientsPort);
            IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            //qDebug() << "IN_parsing.OpcodesAndGamebuf1Map" << IN_parsing.OpcodesAndGamebuf1Map;

            //qDebug() << "IN_parsing.CurrentQuestMenuID_15" << IN_parsing.CurrentQuestMenuID;

            //qDebug() << "IN_parsing.fortyexist5" << IN_parsing.fortyexist;
        }




        if(IN_parsing.flipped == true)
        {
            flipped sendto;
            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_parsing);
            sendto.if_its_flipped( ClientID,ClientsIP,ClientsPort);
            IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            //qDebug() << "IN_parsing.fortyexist6" << IN_parsing.fortyexist;
        }


        if(IN_parsing.flipped == false)
        {
            flipped SendToflipped;
            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_parsing);
            SendToflipped.if_itsNOT_flipped(ClientID,ClientsIP,ClientsPort);
            IN_parsing = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
        }
    }


    //qDebug() << "IN_parsing.Clients_Last_FB_MessageNumber Last Check = " << IN_parsing.Clients_Last_FB_MessageNumber;

    //qDebug() << "IN_parsing.UserName2" << IN_parsing.UserName;

    //qDebug() << "IN_parsing.fortyexist7" << IN_parsing.fortyexist;

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_parsing);


    //qDebug() << "IN_parsing.CurrentQuestMenuID_19" << IN_parsing.CurrentQuestMenuID;

    return packetparsing::IPandPort_AllvariablesMap;
}
