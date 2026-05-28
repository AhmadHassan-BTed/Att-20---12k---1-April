#include "fortytwentyninedec.h"
#include "packetparsing.h"
#include "generator.h"
#include "packetsize.h"
#include "increment.h"
#include "xorpacket.h"
#include "checkthezone.h"
#include "add_or_sub.h"
#include "login.h"
#include "objectpacket.h"
#include "opcodes.h"
#include "mainwindow.h"
#include "crc.h"
#include "worldthread.h"
#include "facingdirection.h"

fortytwentyninedec::fortytwentyninedec(QObject *parent) : QObject(parent)
{

}

QByteArray fortytwentyninedec::fortydec_res(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QByteArray gamebuf1_Cut)
{
    packetparsing::packetvars IN_4029s;
    IN_4029s = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    ////////////////////////////////////////////////////////4029 Decryption///////////////////////////////////



    packetsize2 = gamebuf1_Cut.size();
    where2 = gamebuf1_Cut.indexOf("4029"); //find 4029
    IN_4029s.indexOf4029 = where2;
    carata = where2 + 10;





    if(where2 == -1)
    {
        carata = gamebuf1_Cut.size(); //set to end if no 4029
        IN_4029s.fortyexist = false;
    }
    else
    {
        IN_4029s.indexOf4029 = where2;
        QByteArray Check4029 = gamebuf1_Cut.mid(where2 + 4,4); //4029 number
        QByteArray Byte1 = Check4029.mid(0,2);
        QByteArray Byte2 = Check4029.mid(2,2);
        QByteArray revCheck4029 = Byte2 + Byte1;
        int Check40291 = revCheck4029.toInt(nullptr,16);

        if(IN_4029s.Counter4029 < 3)// Stops it at 2 so its not counting for nothing
        {
            IN_4029s.Next4029 = Check4029;
            IN_4029s.Counter4029++;
        }

        Byte1 = IN_4029s.Next4029.mid(0,2);
        Byte2 = IN_4029s.Next4029.mid(2,2);
        QByteArray revNext4029 = Byte2 + Byte1;
        int Next40291 = revNext4029.toInt(nullptr,16);

        if((IN_4029s.Next4029 == Check4029 || Check40291 == Next40291 - 1 || Check40291 == Next40291 + 1) || (Check40291 == 0 && IN_4029s.Next4029 == "0000" && IN_4029s.flipped == true))
        {
            //do nothing 4029 is fake
        }
        else
        {
            // recheck for real 4029
            where2 = gamebuf1_Cut.indexOf("4029",where2 + 1);



            carata = where2 + 10;

            if(where2 == -1)
            {
                // no real 4029 found
                carata = gamebuf1_Cut.size(); //set to end if no 4029
                IN_4029s.fortyexist = false;
            }
            else
            {
                IN_4029s.indexOf4029 = where2;
                //found a real 4029
                Check4029 = gamebuf1_Cut.mid(where2 + 4,4); //4029 number
                Byte1 = Check4029.mid(0,2);
                Byte2 = Check4029.mid(2,2);
                revCheck4029 = Byte2 + Byte1;
                Check40291 = revCheck4029.toInt(nullptr,16);

                Byte1 = IN_4029s.Next4029.mid(0,2);
                Byte2 = IN_4029s.Next4029.mid(2,2);
                revNext4029 = Byte2 + Byte1;
                Next40291 = revNext4029.toInt(nullptr,16);
            }
        }


        //only real 4029s can go through
        if((IN_4029s.Next4029 == Check4029 || Check40291 == Next40291 - 1 || Check40291 == Next40291 + 1) || (Check40291 == 0 && IN_4029s.Next4029 == "0000" && IN_4029s.flipped == true))// I put this around the whole 4029 section
        {
            IN_4029s.fortyexist = true;



            if(IN_4029s.PacketReply == "13")
            {
                IN_4029s.ClientsCheckChannelConfirm = IN_4029s.ClientsCheckChannelConfirm.mid(0,IN_4029s.indexOf4029);

                QByteArray Channel = "";
                QByteArray ChannelsCount = "";

                int iter = 0;

                do
                {
                    Channel = IN_4029s.ClientsCheckChannelConfirm.mid(iter,2);
                    iter += 2;

                    if(Channel == "F8")
                    {
                        iter = IN_4029s.ClientsCheckChannelConfirm.size();

                    }
                    else
                    {
                        ChannelsCount = IN_4029s.ClientsCheckChannelConfirm.mid(iter,4);
                        iter += 4;

                        IN_4029s.Client_incrementmap.insert(Channel,ChannelsCount);
                    }

                }while(iter < IN_4029s.ClientsCheckChannelConfirm.size());
            }


            if(Check40291 == Next40291 + 1)// this should fix it if a 4029 drops
            {
                IN_4029s.Next4029 = Check4029;
            }

            if(Check40291 == Next40291 - 1)// this should fix it if a 4029 drops
            {
                IN_4029s.fortyexist = false;

                if(IN_4029s.opcodeexist == false)
                {

                    QString mainid4 = IN_4029s.CharSelectID;
                    generator packet4;
                    QByteArray holdergen = packet4.maintoonswitch(mainid4,"",IN_4029s.CharSelectID_Main); //send the id we want to get a 00c9

                    IN_4029s.SendZeroChannelVector.append("00c9xxxx00" + holdergen);

                    holdergen = packetcustom2;

                    packetsize newsize4;
                    QByteArray sizeresult = newsize4.switchsize(holdergen); //send to get new size
                    holdergen.replace(8,4,sizeresult);

                    holdergen.replace(4,4,IN_4029s.clientid1);
                    holdergen.replace(0,4,IN_4029s.serverid1);
                    holdergen.replace(34,16,IN_4029s.combine1); //add the F8 response

                    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_4029s.CharSelectID);
                    QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                    increment GrpOut;
                    QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                    holdergen.replace(30,4,GrpOut_a);
                    Servers_Last_MessageNumber = GrpOut_a.toUpper();

                    MessNumbVect.replace(0,Servers_Last_MessageNumber);

                    Opcodes::CharID_ServerMessageNumbers.insert(IN_4029s.CharSelectID,MessNumbVect);

                    IN_4029s.Return4029.append(holdergen);
                    IN_4029s.Return4029.append("Shutting up the 4029s");
                }
            }

            if(Check40291 == 0)
            {
                IN_4029s.Next4029 = Check4029;
            }
        }


        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_4029s);
        IN_4029s = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);


        qDebug() << "IN_4029s.fortyexist1" << IN_4029s.fortyexist;

        if(IN_4029s.fortyexist == true && IN_4029s.opcodeexist == true && IN_4029s.flipped == true)
        {
            IN_4029s.Opcode4029 = true;
        }
        else
        {
            IN_4029s.Opcode4029 = false;
        }


        IN_4029s.decopacket1 = ""; //clear the packet

        if(IN_4029s.fortyexist == true)
        {
            increment Next4029Number;
            IN_4029s.Next4029 = Next4029Number.count(IN_4029s.Next4029).toUpper();

            IN_4029s.fortytwentninenum1 = gamebuf1_Cut.mid(where2 + 4,4); //4029 number


            IN_4029s.combine1 = IN_4029s.Clients_Last_MessageNumber + IN_4029s.Clients_Last_FB_MessageNumber + "40" + IN_4029s.fortytwentninenum1 + "F8";

            QByteArray holdMyxor = gamebuf1_Cut.mid(where2 + 8,2); //Xor
            IN_4029s.currentxor = holdMyxor.toInt(nullptr,16);

            do
            {
                decom40 = gamebuf1_Cut.mid(carata,2); //get first compressed
                int over1 = decom40.toInt(nullptr,16); //change to int

                if(over1 > 0x79) //is it over 79h?
                {
                    decom40 = gamebuf1_Cut.mid(carata,4); //get two bytes
                    int hold1 = decom40.mid(0,2).toInt(nullptr,16); //get highbyte
                    high2 = hold1 - 0x80;  //subtract 80h
                    //        qDebug() << "high2" << high2;
                    low2 = decom40.mid(2,2).toInt(nullptr,16); //get lowbyte
                    carata += 4; //increment by what was read
                }
                else
                {
                    high2 = decom40.mid(0,1).toInt(nullptr,16); //get one byte
                    low2 = decom40.mid(1,1).toInt(nullptr,16); //get one byte

                    if(high2 == 0 && low2 == 0) //breakout when done
                    {
                        where2 = gamebuf1_Cut.indexOf("4029",carata); //find 4029

                        if(where2 == -1)
                        {
                            break;
                        }

                        carata = where2 + 8;
                    }

                    carata += 2; //increment by what was read
                }

                QByteArray zeros = "00"; //zero byte
                QByteArray zeroholder = ""; //hold number of zeros
                zeroholder = zeros.repeated(low2); //repeat #
                IN_4029s.decopacket1.insert(caratb,zeroholder); //insert zeros
                caratb += low2 * 2; //set new carat position
                QByteArray decom2 = gamebuf1_Cut.mid(carata,high2 * 2); //get bytes to keep
                IN_4029s.decopacket1.insert(caratb,decom2); //insert kept bytes
                carata += high2 *2 ; //set new carat position
                caratb += high2 *2; //set new carat position

            }while (carata  <  packetsize2 -2);


            //////////////////////////////4029 xyz///////////////////////////////////////

            QByteArray CurrentCombat1 = "";
            QString CurrentCombatclientnumber2 = "";

            if(IN_4029s.fortytwentninenum1 == "0100") //set the buffers to get the first 3 4029s
            {
                IN_4029s.ringbuffer1 = IN_4029s.decopacket1;
                QByteArray Xsnip1 = IN_4029s.ringbuffer1.mid(2,6); //get X
                QByteArray Zsnip1 = IN_4029s.ringbuffer1.mid(8,6); //get Z
                QByteArray Ysnip1 = IN_4029s.ringbuffer1.mid(14,6); //get Y
                QByteArray WEsnip1 = IN_4029s.ringbuffer1.mid(20,4); //get W/E
                QByteArray NSsnip1 = IN_4029s.ringbuffer1.mid(28,4); //get N/S
                QByteArray Fsnip1 = IN_4029s.ringbuffer1.mid(44,2); //get Facing
                QByteArray Asnip1 = IN_4029s.ringbuffer1.mid(70,2); //get Animation
                QByteArray Csnip1 = IN_4029s.ringbuffer1.mid(72,2); //get IN_4029s.CurrentCombat
                QByteArray Tsnip1 = IN_4029s.ringbuffer1.mid(74,8); //get Target

                int Xnum1 = Xsnip1.toInt(nullptr,16);
                int Znum1 = Zsnip1.toInt(nullptr,16);
                int Ynum1 = Ysnip1.toInt(nullptr,16);
                int WEnum1 = WEsnip1.toInt(nullptr,16);
                int NSnum1 = NSsnip1.toInt(nullptr,16);
                int Fnum1 = Fsnip1.toInt(nullptr,16);
                int Anum1 = Asnip1.toInt(nullptr,16);
                int Cnum1 = Csnip1.toInt(nullptr,16);
                qlonglong Tnum1 = Tsnip1.toLongLong(nullptr,16);

                IN_4029s.X_stack.push(Xnum1); //this will go off twice
                IN_4029s.Z_stack.push(Znum1);
                IN_4029s.Y_stack.push(Ynum1);
                IN_4029s.WE_stack.push(WEnum1);
                IN_4029s.NS_stack.push(NSnum1);
                IN_4029s.F_stack.push(Fnum1);
                IN_4029s.A_stack.push(Anum1);
                IN_4029s.C_stack.push(Cnum1);
                IN_4029s.T_stack.push(Tnum1);

                IN_4029s.LoginStarted = "";

                qDebug() << "";
                qDebug() << "IN_4029s.CharSelectID" << IN_4029s.CharSelectID;

                QString mainid = IN_4029s.CharSelectID;
                generator packet;
                QByteArray holder271 = packet.maintoonswitch(mainid,"1F",IN_4029s.CharSelectID_Main); //send the id we want to get a 00c9
                holder271.insert(0,twosevenone); //add the header



                if(IN_4029s.packetid != 0)
                {

                    if(db.isOpen())
                    {
                        db.close();
                        db.removeDatabase(QSqlDatabase::defaultConnection);
                    }

                    db = QSqlDatabase::addDatabase("QSQLITE");
                    db.setDatabaseName(Opcodes::m_FileName);
                    db.open();

                    db.transaction();


                    QString TableName = "STATS42EC";

                    QSqlQuery query;
                    QSqlRecord record;
                    query.prepare(QString ("SELECT * FROM STATS42EC WHERE ROWID = %1").arg(IN_4029s.packetid));
                    query.exec();

                    record = query.record();

                    qDebug() << "IN_4029s.packetid" << IN_4029s.packetid;

                    QByteArray TestStatsChange = "";

                    while(query.next())
                    {
                        for(int rr = 3; rr < record.count(); rr++)
                        {
                            IN_4029s.StatsChange.append(query.value(rr).toString().toUtf8());

                            TestStatsChange.append(query.value(rr).toString().toUtf8());

                        }
                    }

                    qDebug() << "IN_4029s.StatsChange.size()1" << IN_4029s.StatsChange.size();
                    qDebug() << "IN_4029s.StatsChange" << IN_4029s.StatsChange;

                    db.commit();
                }

                qDebug() << "got here 1";

                int NewCurrentHP = 0;
                int NewCurrentPOW = 0;
                int MyNewHP = -2;


                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_4029s);

                xorpacket Xor;
                QByteArray New42 = Xor.Recompress42EC(ClientID ,ClientsIP,ClientsPort,IN_4029s.mylist,IN_4029s.CharSelectID,IN_4029s.StatsChange,IN_4029s.HoldOldHP,NewCurrentHP,NewCurrentPOW,MyNewHP);

                qDebug() << "did we come out?";

                IN_4029s = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

                twosevenoneadd1.append(New42);
                holder271.append(twosevenoneadd1); //add the stats



                qDebug() << "got here 2";

                IN_4029s.incrementmap.insert("00","0100");
                IN_4029s.zerocounter = "0100";
                IN_4029s.incrementmap.insert("42","0100");


                holder271.replace(0,4,IN_4029s.serverid1);
                holder271.replace(4,4,IN_4029s.clientid1);

                IN_4029s.combine1.replace(4,4,"0400");
                IN_4029s.Clients_Last_FB_MessageNumber = "0400";
                holder271.replace(34,16,IN_4029s.combine1);

                qDebug() << "got here 3";

                packetsize newsize;
                QByteArray sizeresult = newsize.switchsize(holder271); //send to get new size
                holder271.replace(8,4,sizeresult);

                QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_4029s.CharSelectID);
                QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                increment GrpOut;
                QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                holder271.replace(30,4,GrpOut_a);
                Servers_Last_MessageNumber = GrpOut_a.toUpper();

                MessNumbVect.replace(0,Servers_Last_MessageNumber);

                QByteArray Servers_Last_FB_MessageNumber = "2200";
                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
                Opcodes::CharID_ServerMessageNumbers.insert(IN_4029s.CharSelectID,MessNumbVect);

                MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_4029s.CharSelectID);
                Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                qDebug() << "holder271" << holder271;
                qDebug() << "Servers_Last_FB_MessageNumber in 271 " << Servers_Last_FB_MessageNumber;

                IN_4029s.Return4029.append(holder271);
                IN_4029s.Return4029.append("Server_271_");

                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_4029s);

                IN_4029s = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
            }



            if(IN_4029s.fortytwentninenum1 == "0200")
            {
                QByteArray Xsnip1 = IN_4029s.ringbuffer1.mid(2,6); //get X
                QByteArray Zsnip1 = IN_4029s.ringbuffer1.mid(8,6); //get Z
                QByteArray Ysnip1 = IN_4029s.ringbuffer1.mid(14,6); //get Y
                QByteArray WEsnip1 = IN_4029s.ringbuffer1.mid(20,4); //get W/E
                QByteArray NSsnip1 = IN_4029s.ringbuffer1.mid(28,4); //get N/S
                QByteArray Fsnip1 = IN_4029s.ringbuffer1.mid(44,2); //get Facing
                QByteArray Asnip1 = IN_4029s.ringbuffer1.mid(70,2); //get Animation
                QByteArray Csnip1 = IN_4029s.ringbuffer1.mid(72,2); //get IN_4029s.CurrentCombat
                QByteArray Tsnip1 = IN_4029s.ringbuffer1.mid(74,8); //get Target

                int Xnum1 = Xsnip1.toInt(nullptr,16);
                int Znum1 = Zsnip1.toInt(nullptr,16);
                int Ynum1 = Ysnip1.toInt(nullptr,16);
                int WEnum1 = WEsnip1.toInt(nullptr,16);
                int NSnum1 = NSsnip1.toInt(nullptr,16);
                int Fnum1 = Fsnip1.toInt(nullptr,16);
                int Anum1 = Asnip1.toInt(nullptr,16);
                int Cnum1 = Csnip1.toInt(nullptr,16);
                qlonglong Tnum1 = Tsnip1.toLongLong(nullptr,16);

                IN_4029s.X_stack.push(Xnum1); //this will go off twice
                IN_4029s.Z_stack.push(Znum1);
                IN_4029s.Y_stack.push(Ynum1);
                IN_4029s.WE_stack.push(WEnum1);
                IN_4029s.NS_stack.push(NSnum1);
                IN_4029s.F_stack.push(Fnum1);
                IN_4029s.A_stack.push(Anum1);
                IN_4029s.C_stack.push(Cnum1);
                IN_4029s.T_stack.push(Tnum1);

                QString mainid2 = IN_4029s.CharSelectID;
                generator packet2;
                QByteArray holder272 = packet2.maintoonswitch(mainid2,"1F",IN_4029s.CharSelectID_Main); //send the id we want to get a 00c9
                holder272.insert(0,twoseventwo); //add the header

                IN_4029s.incrementmap.insert("00","0200");
                IN_4029s.zerocounter = "0300";

                holder272.replace(0,4,IN_4029s.serverid1);
                holder272.replace(4,4,IN_4029s.clientid1);


                holder272.replace(34,16,IN_4029s.combine1);

                packetsize newsize2;
                QByteArray sizeresult2 = newsize2.switchsize(holder272);
                holder272.replace(8,4,sizeresult2);

                QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_4029s.CharSelectID);
                QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                increment GrpOut;
                QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                holder272.replace(30,4,GrpOut_a);
                Servers_Last_MessageNumber = GrpOut_a.toUpper();

                MessNumbVect.replace(0,Servers_Last_MessageNumber);

                Opcodes::CharID_ServerMessageNumbers.insert(IN_4029s.CharSelectID,MessNumbVect);

                IN_4029s.Return4029.append(holder272);
                IN_4029s.Return4029.append("Server_272_");

                IN_4029s.loader275 = true;
            }

            if(IN_4029s.loader275 == true)
            {


                //packet 274  - load packet 275

                QString mainid3 = IN_4029s.CharSelectID;
                generator packet3;
                QByteArray holder275 = packet3.maintoonswitch(mainid3,"15",IN_4029s.CharSelectID_Main); //send the id we want to get a 00c9
                holder275.insert(0,twosevenfive); //add the header


                IN_4029s.incrementmap.insert("00","0300");
                IN_4029s.zerocounter = "0300";

                QString FBsizeOpcode = "5200" + IN_4029s.Cash_on_hand;



                ADD_OR_SUB CurrentItem;
                QString NewFB = CurrentItem.FBSize(FBsizeOpcode);

                qDebug() << "NewFB1" << NewFB;

                QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_4029s.CharSelectID);
                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                increment Text;
                QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
                int index = NewFB.indexOf("XXXX");
                NewFB.replace(index,4,Text2);
                Servers_Last_FB_MessageNumber = Text2.toUpper();

                qDebug() << "NewFB2" << NewFB;

                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                Opcodes::CharID_ServerMessageNumbers.insert(IN_4029s.CharSelectID,MessNumbVect);

                holder275.append(NewFB.toUtf8());

                FBsizeOpcode = "5312" + IN_4029s.Current_Available_balance.toUtf8();

                NewFB = CurrentItem.FBSize(FBsizeOpcode);

                qDebug() << "NewFB3" << NewFB;

                MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_4029s.CharSelectID);
                Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                Text2 = Text.count(Servers_Last_FB_MessageNumber);
                index = NewFB.indexOf("XXXX");
                NewFB.replace(index,4,Text2);
                Servers_Last_FB_MessageNumber = Text2.toUpper();

                qDebug() << "NewFB4" << NewFB;

                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                Opcodes::CharID_ServerMessageNumbers.insert(IN_4029s.CharSelectID,MessNumbVect);

                holder275.append(NewFB.toUtf8());

                FBsizeOpcode = "2000" + IN_4029s.CurrentLevel + IN_4029s.StartXPbar;

                NewFB = CurrentItem.FBSize(FBsizeOpcode);

                qDebug() << "NewFB5" << NewFB;

                MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_4029s.CharSelectID);
                Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                Text2 = Text.count(Servers_Last_FB_MessageNumber);
                index = NewFB.indexOf("XXXX");
                NewFB.replace(index,4,Text2);
                Servers_Last_FB_MessageNumber = Text2.toUpper();

                qDebug() << "NewFB6" << NewFB;

                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                Opcodes::CharID_ServerMessageNumbers.insert(IN_4029s.CharSelectID,MessNumbVect);

                holder275.append(NewFB.toUtf8());

                for(int re = 0; re < 4; re++)
                {
                    FBsizeOpcode = "7D000000000000";

                    NewFB = CurrentItem.FBSize(FBsizeOpcode);

                    qDebug() << "NewFB5" << NewFB;

                    MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_4029s.CharSelectID);
                    Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    Text2 = Text.count(Servers_Last_FB_MessageNumber);
                    index = NewFB.indexOf("XXXX");
                    NewFB.replace(index,4,Text2);
                    Servers_Last_FB_MessageNumber = Text2.toUpper();

                    qDebug() << "NewFB6" << NewFB;

                    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                    Opcodes::CharID_ServerMessageNumbers.insert(IN_4029s.CharSelectID,MessNumbVect);

                    holder275.append(NewFB.toUtf8());
                }

                FBsizeOpcode = Correct55550d41;
                NewFB = CurrentItem.FBSize(FBsizeOpcode);

                MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_4029s.CharSelectID);
                Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                Text2 = Text.count(Servers_Last_FB_MessageNumber);
                index = NewFB.indexOf("XXXX");
                NewFB.replace(index,4,Text2);
                Servers_Last_FB_MessageNumber = Text2.toUpper();

                MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

                Opcodes::CharID_ServerMessageNumbers.insert(IN_4029s.CharSelectID,MessNumbVect);

                holder275.append(NewFB.toUtf8());

                holder275.replace(0,4,IN_4029s.serverid1);
                holder275.replace(4,4,IN_4029s.clientid1);

                packetsize newsize3; //call size class
                QByteArray sizeresult3 = newsize3.switchsize(holder275); //get new size
                holder275.replace(8,4,sizeresult3); //add new size

                MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_4029s.CharSelectID);
                QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

                increment GrpOut;
                QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
                holder275.replace(30,4,GrpOut_a);
                Servers_Last_MessageNumber = GrpOut_a.toUpper();

                MessNumbVect.replace(0,Servers_Last_MessageNumber);

                Opcodes::CharID_ServerMessageNumbers.insert(IN_4029s.CharSelectID,MessNumbVect);

                IN_4029s.Return4029.append(holder275);
                IN_4029s.Return4029.append("Server_275_");

                IN_4029s.flipped = true;
                IN_4029s.loader275 = false; //set to false so it only goes off once

                qDebug() << "IN_4029s.loader275 =" << IN_4029s.loader275;

                if(objectpacket::DeadListVector.contains(IN_4029s.CharSelectID))
                {
                    int index = objectpacket::DeadListVector.indexOf(IN_4029s.CharSelectID);
                    objectpacket::DeadListVector.removeAt(index);
                }


                QVector<QByteArray> ID_IP_PORT;

                ID_IP_PORT.append(ClientID );
                ID_IP_PORT.append(ClientsIP);
                ID_IP_PORT.append(ClientsPort);

                Opcodes::CharID_CLientID_IP_PORT_Map.insert(IN_4029s.CharSelectID,ID_IP_PORT);
                
                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_4029s);

                IN_4029s = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
            }



            qDebug() << "4029_1";

            if(IN_4029s.currentxor > IN_4029s.WE_stack.size())
            {
                IN_4029s.currentxor = 0;
            }

            if(IN_4029s.currentxor == 0)
            {
                QString currentxor1 = QString("%1").arg(IN_4029s.currentxor,2,10,QLatin1Char('0'));

                //////////////////////////////West & East///////////////////////////////////////
                QByteArray WEbyte1 = IN_4029s.decopacket1.mid(20,4); //get facing
                int WandE = WEbyte1.toInt(nullptr,16);
                
                if(WandE >= 0x8000)
                {
                    WandE = WandE - 0x8000;
                    QString masterWE1 = QString("%1").arg(WandE,4,16,QLatin1Char('0'));
                    IN_4029s.masterWE = masterWE1.toUtf8().toUpper();
                }
                //                else
                //                {
                //                    WandE =  WandE + 0x8000;
                //                    QString masterWE1 = QString("%1").arg(WandE,4,16,QLatin1Char('0'));
                //                    IN_4029s.masterWE = masterWE1.toUtf8().toUpper();
                //                }
                
                IN_4029s.WE_stack.push(WandE);
                //////////////////////////////West & East///////////////////////////////////////

                //////////////////////////////West & East///////////////////////////////////////
                QByteArray NSbyte1 = IN_4029s.decopacket1.mid(28,4); //get facing
                int NandS = NSbyte1.toInt(nullptr,16);
                
                if(NandS >= 0x8000)
                {
                    NandS = NandS - 0x8000;
                    QString masterNS1 = QString("%1").arg(NandS,4,16,QLatin1Char('0'));
                    IN_4029s.masterNS = masterNS1.toUtf8().toUpper();
                }
                //                else
                //                {
                //                    NandS =  NandS + 0x8000;
                //                    QString masterNS1 = QString("%1").arg(NandS,4,16,QLatin1Char('0'));
                //                    IN_4029s.masterNS = masterNS1.toUtf8().toUpper();
                //                }

                IN_4029s.NS_stack.push(NandS);
                //////////////////////////////West & East///////////////////////////////////////

                //////////////////////////////Facing///////////////////////////////////////
                QByteArray facingbyte1 = IN_4029s.decopacket1.mid(44,2); //get facing
                facedeconumber1 = facingbyte1.toInt(nullptr,16);
                IN_4029s.F_stack.push(facedeconumber1);

                if(facedeconumber1 >= 0x80)
                {
                    res4 = facedeconumber1 - 0x80;
                    QString facerez10 = QString("%1").arg(res4,2,16,QLatin1Char('0'));
                    IN_4029s.masterf = facerez10.toUtf8().toUpper();
                }
                else
                {
                    res4 = facedeconumber1 + 0x80;
                    QString facerez11 = QString("%1").arg(res4,2,16,QLatin1Char('0'));
                    IN_4029s.masterf = facerez11.toUtf8().toUpper();
                }
                //////////////////////////////Facing///////////////////////////////////////

                //////////////////////////////Target///////////////////////////////////////
                QByteArray Targetbyte1 = IN_4029s.decopacket1.mid(74,8); //get Target
                qlonglong ClientTargetIDXOR1 = Targetbyte1.toLongLong(nullptr,16);//xor value
                IN_4029s.Target_ID = QString("%1").arg(ClientTargetIDXOR1,8,16,QLatin1Char('0')).toUtf8();
                IN_4029s.T_stack.push(ClientTargetIDXOR1);
                //////////////////////////////Target///////////////////////////////////////

                //////////////////////////////Animation////////////////////////////////////
                QByteArray Animation1 = IN_4029s.decopacket1.mid(70,2); //get Animation
                int ClientAnimationIDXOR1 = Animation1.toInt(nullptr,16);//xor value
                IN_4029s.myAnimation = QString("%1").arg(ClientAnimationIDXOR1,2,16,QLatin1Char('0')).toUtf8();
                IN_4029s.A_stack.push(ClientAnimationIDXOR1);
                //////////////////////////////Animation////////////////////////////////////

                //////////////////////////////IN_4029s.CurrentCombat///////////////////////////////////////
                int oldCombatValue = IN_4029s.CurrentCombat.toInt(nullptr,16);
                QByteArray CurrentCombat1 = IN_4029s.decopacket1.mid(72,2);
                int Combat_XOR = CurrentCombat1.toInt(nullptr,16);
                int NewCombatNumber = oldCombatValue ^ Combat_XOR;
                QString NewCombatNumber1 = QString("%1").arg(NewCombatNumber,2,16,QLatin1Char('0'));
                IN_4029s.CurrentCombat = NewCombatNumber1.toUtf8().toUpper();
                IN_4029s.C_stack.push(Combat_XOR);
                //////////////////////////////IN_4029s.CurrentCombat///////////////////////////////////////

                

                if(IN_4029s.F_stack.size()  >  255)
                {
                    IN_4029s.WE_stack.removeAt(0);
                    IN_4029s.NS_stack.removeAt(0);
                    IN_4029s.F_stack.removeAt(0);
                    IN_4029s.A_stack.removeAt(0);
                    IN_4029s.C_stack.removeAt(0);
                    IN_4029s.T_stack.removeAt(0);
                }
            }

            qDebug() << "4029_2";

            if(IN_4029s.currentxor != 0 && IN_4029s.currentxor <= 255)
            {
                QString currentxor1 = QString("%1").arg(IN_4029s.currentxor,2,10,QLatin1Char('0'));

                //////////////////////////////West and East///////////////////////////////////////
                QByteArray WEholder1 = IN_4029s.decopacket1.mid(20,4);
                int WandE = WEholder1.toInt(nullptr,16);

                int WEstackpos1 = IN_4029s.currentxor;
                int WEstackcount1 = IN_4029s.WE_stack.size();



                int WEclientnumber1 = IN_4029s.WE_stack.at(WEstackcount1 - WEstackpos1);
                int WEresult1 = WandE ^ WEclientnumber1;
                
                if(WEresult1 >= 0x8000)
                {
                    WEresult1 = WEresult1 - 0x8000;
                    QString masterWE1 = QString("%1").arg(WEresult1,4,16,QLatin1Char('0'));
                    IN_4029s.masterWE = masterWE1.toUtf8().toUpper();
                }
                //                else
                //                {
                //                    WEresult1 =  WEresult1 + 0x8000;
                //                    QString masterWE1 = QString("%1").arg(WEresult1,4,16,QLatin1Char('0'));
                //                    IN_4029s.masterWE = masterWE1.toUtf8().toUpper();
                //                }
                
                IN_4029s.WE_stack.push(WEresult1);
                //////////////////////////////West and East///////////////////////////////////////

                //////////////////////////////North and South///////////////////////////////////////
                QByteArray NSholder1 = IN_4029s.decopacket1.mid(28,4);
                int NandS = NSholder1.toInt(nullptr,16);

                int NSstackpos1 = IN_4029s.currentxor;
                int NSstackcount1 = IN_4029s.NS_stack.size();

                int NSclientnumber1 = IN_4029s.NS_stack.at(NSstackcount1 - NSstackpos1);
                int NSresult1 = NandS ^ NSclientnumber1;
                
                if(NSresult1 >= 0x8000)
                {
                    NSresult1 = NSresult1 - 0x8000;
                    QString masterNS1 = QString("%1").arg(NSresult1,4,16,QLatin1Char('0'));
                    IN_4029s.masterNS = masterNS1.toUtf8().toUpper();
                }
                //                else
                //                {
                //                    NSresult1 =  NSresult1 + 0x8000;
                //                    QString masterNS1 = QString("%1").arg(NSresult1,4,16,QLatin1Char('0'));
                //                    IN_4029s.masterNS = masterNS1.toUtf8().toUpper();
                //                }
                
                IN_4029s.NS_stack.push(NSresult1);
                //////////////////////////////North and South///////////////////////////////////////


                //////////////////////////////Facing///////////////////////////////////////
                QByteArray faceholder1 = IN_4029s.decopacket1.mid(44,2); //get Facing
                facedeconumber1 = faceholder1.toInt(nullptr,16);

                int facestackpos1 = IN_4029s.currentxor;
                int facestackcount1 = IN_4029s.F_stack.size();

                int faceclientnumber1 = IN_4029s.F_stack.at(facestackcount1 - facestackpos1);
                faceresult1 = facedeconumber1 ^ faceclientnumber1; //xor with ringbuffer
                IN_4029s.F_stack.push(faceresult1);

                if(faceresult1 >= 0x80)
                {
                    res4 = faceresult1 - 0x80;
                    QString facerez12 = QString("%1").arg(res4,2,16,QLatin1Char('0'));
                    IN_4029s.masterf = facerez12.toUtf8().toUpper();
                }
                else
                {
                    res4 =  faceresult1 + 0x80;
                    QString facerez13 = QString("%1").arg(res4,2,16,QLatin1Char('0'));
                    IN_4029s.masterf = facerez13.toUtf8().toUpper();
                }
                //////////////////////////////Facing///////////////////////////////////////

                //////////////////////////////Target///////////////////////////////////////
                QByteArray Targetbyte1 = IN_4029s.decopacket1.mid(74,8); //get facing
                qlonglong Targetdeconumber1 = Targetbyte1.toLongLong(nullptr,16);
                QString Targetdeconumber2 = QString("%1").arg(Targetdeconumber1,8,16,QLatin1Char('0'));

                int TargeT_stackpos1 = IN_4029s.currentxor;
                int TargeT_stackcount1 = IN_4029s.T_stack.size();

                qlonglong Targetclientnumber1 = IN_4029s.T_stack.at(TargeT_stackcount1 - TargeT_stackpos1);
                QString Targetclientnumber2 = QString("%1").arg(Targetclientnumber1,8,16,QLatin1Char('0'));

                qlonglong Targetresult1 = Targetdeconumber1 ^ Targetclientnumber1; //xor with ringbuffer
                QString Targetresult2 = QString("%1").arg(Targetresult1,8,16,QLatin1Char('0'));

                if(Targetresult2.size() == 16)
                {
                    Targetresult2 = Targetresult2.mid(8,8).toUtf8().toUpper();

                    if(Targetresult2 == "FFFFFFFF")
                    {
                        IN_4029s.Target_ID = Targetresult2.toUtf8().toUpper();
                    }

                    qlonglong Targetresult1 = Targetresult2.toLongLong(nullptr,16);
                    IN_4029s.T_stack.push(Targetresult1);
                }
                else
                {
                    if(Targetresult2 == "FFFFFFFF")
                    {
                        IN_4029s.Target_ID = Targetresult2.toUtf8().toUpper();
                    }

                    IN_4029s.T_stack.push(Targetresult1);
                }

                //////////////////////////////Target///////////////////////////////////////

                //////////////////////////////Animation////////////////////////////////////
                QByteArray Animation1 = IN_4029s.decopacket1.mid(70,2); //get Animation
                int Animationdeconumber1 = Animation1.toInt(nullptr,16);//xor value

                int Animationstackpos1 = IN_4029s.currentxor;
                int Animationstackcount1 = IN_4029s.A_stack.size();

                int Animationclientnumber1 = IN_4029s.A_stack.at(Animationstackcount1 - Animationstackpos1);
                QString Animationclientnumber2 = QString("%1").arg(Animationclientnumber1,2,16,QLatin1Char('0'));

                int Animationresult1 = Animationdeconumber1 ^ Animationclientnumber1; //xor with ringbuffer
                QString Animationresult2 = QString("%1").arg(Animationresult1,2,16,QLatin1Char('0'));

                IN_4029s.myAnimation = Animationresult2.toUtf8().toUpper();
                IN_4029s.A_stack.push(Animationresult1);

                //////////////////////////////Animation////////////////////////////////////

                //////////////////////////////IN_4029s.CurrentCombat///////////////////////////////////////
                CurrentCombat1 = IN_4029s.decopacket1.mid(72,2); //get IN_4029s.CurrentCombat
                int CurrentCombatdeconumber1 = CurrentCombat1.toInt(nullptr,16);//xor value

                int CurrentCombaT_stackpos1 = IN_4029s.currentxor;
                int CurrentCombaT_stackcount1 = IN_4029s.C_stack.size();

                QString C_stackSize = QString("%1").arg(CurrentCombaT_stackcount1,2,16,QLatin1Char('0'));

                int CurrentCombatclientnumber1 = IN_4029s.C_stack.at(CurrentCombaT_stackcount1 - CurrentCombaT_stackpos1);
                CurrentCombatclientnumber2 = QString("%1").arg(CurrentCombatclientnumber1,2,16,QLatin1Char('0'));


                int oldCombatValue = IN_4029s.CurrentCombat.toInt(nullptr,16);
                int CurrentCombatresult1 = CurrentCombatdeconumber1 ^ CurrentCombatclientnumber1; //xor with ringbuffer
                QString CurrentCombatresult2 = QString("%1").arg(CurrentCombatresult1,2,16,QLatin1Char('0'));



                int NewCombatNumber = oldCombatValue ^ CurrentCombatdeconumber1;
                QString NewCombatNumber1 = QString("%1").arg(NewCombatNumber,2,16,QLatin1Char('0'));
                IN_4029s.CurrentCombat = CurrentCombatresult2.toUtf8().toUpper();
                IN_4029s.C_stack.push(CurrentCombatresult1);

                //////////////////////////////IN_4029s.CurrentCombat///////////////////////////////////////
                ;

                if(IN_4029s.F_stack.size()  >  255)
                {
                    IN_4029s.WE_stack.removeAt(0);
                    IN_4029s.NS_stack.removeAt(0);
                    IN_4029s.F_stack.removeAt(0);
                    IN_4029s.A_stack.removeAt(0);
                    IN_4029s.C_stack.removeAt(0);
                    IN_4029s.T_stack.removeAt(0);
                }
            }

            /////////////////////////////////////////////X//////////////////////

            //            if(!//file.open(QIODevice::Append))
            //            {

            //            }

            //            //file.write("IN_4029s.decopacket1 = " + IN_4029s.decopacket1);
            //            //file.write("\n");

            qDebug() << "4029_3";

            if(IN_4029s.currentxor == 0)
            {
                QByteArray xholder0 = IN_4029s.decopacket1.mid(2,6); //get X
                xdeconumber1 = xholder0.toInt(nullptr,16);
                res1 = xdeconumber1 - 0x1C71C7;

                IN_4029s.X_stack.push(xdeconumber1);

                if(IN_4029s.X_stack.size()  >  255)
                {
                    IN_4029s.X_stack.removeAt(0);
                }



                //                //file.write("xholder0 = " + xholder0);
                //                //file.write("\n");
                //                //file.write("res1 = " + QString::number(res1).toUtf8());
                //                //file.write("\n");

            }

            qDebug() << "4029_4";


            if(IN_4029s.currentxor != 0 && IN_4029s.currentxor <= 255)
            {
                QByteArray xholder1 = IN_4029s.decopacket1.mid(2,6); //get X
                xdeconumber1 = xholder1.toInt(nullptr,16);
                int stackposx1 = IN_4029s.currentxor; //get current xor in decimal
                int stackcountx1 = IN_4029s.X_stack.size(); //get current size of stack
                int xclientnumber1 = IN_4029s.X_stack.at(stackcountx1 - stackposx1); //read at position
                xorresult1 = xdeconumber1 ^ xclientnumber1; //xor with ringbuffer
                IN_4029s.X_stack.push(xorresult1);

                if(IN_4029s.X_stack.size()  >  255)
                {
                    IN_4029s.X_stack.removeAt(0);
                }

                res1 =  xorresult1 - 0x1C71C7;

                //                //file.write("xholder1 = " + xholder1);
                //                //file.write("\n");
                //                //file.write("res1 = " + QString::number(res1).toUtf8());
                //                //file.write("\n");

            }



            qDebug() << "4029_5";


            float fourty = res1 / 466.033798648;
            int helper = fourty * 128.0; //change to float 128.0 for full float

            QString fourtxout = QString("%1").arg(helper,6,16,QLatin1Char('0'));
            IN_4029s.masterx = fourtxout.toUtf8().toUpper();

            //            //file.write("IN_4029s.masterx = " +  IN_4029s.masterx);
            //            //file.write("\n");
            //            //file.write("helper = " + QString::number(helper).toUtf8());
            //            //file.write("\n");




            /////////////////////////////////////////////////////////X//////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////////Z/////////////////////////////////////////////////

            qDebug() << "4029_6";

            if(IN_4029s.currentxor == 0)
            {
                QByteArray zholder0 = IN_4029s.decopacket1.mid(8,6);
                zdeconumber1 = zholder0.toInt(nullptr,16);
                res3 = zdeconumber1 - 0x800000;

                IN_4029s.Z_stack.push(zdeconumber1);

                if(IN_4029s.Z_stack.size()  >  255)
                {
                    IN_4029s.Z_stack.removeAt(0);
                }

                //                //file.write("zholder0 = " +  zholder0);
                //                //file.write("\n");
                //                //file.write("res3 = " + QString::number(res3).toUtf8());
                //                //file.write("\n");

            }

            qDebug() << "4029_7";

            if(IN_4029s.currentxor != 0 && IN_4029s.currentxor <= 255)
            {
                QByteArray zholder1 = IN_4029s.decopacket1.mid(8,6); //get Z
                zdeconumber1 = zholder1.toInt(nullptr,16);
                int stackposz1 = IN_4029s.currentxor;
                int stackcountz1 = IN_4029s.Z_stack.size();
                int zclientnumber1 = IN_4029s.Z_stack.at(stackcountz1 - stackposz1);
                xorresult3 = zdeconumber1 ^ zclientnumber1; //xor with ringbuffer
                IN_4029s.Z_stack.push(xorresult3);

                if(IN_4029s.Z_stack.size()  >  255)
                {
                    IN_4029s.Z_stack.removeAt(0);
                }

                res3 =  xorresult3 -  0x800000;

                //                //file.write("zholder1 = " +  zholder1);
                //                //file.write("\n");
                //                //file.write("res3 = " + QString::number(res3).toUtf8());
                //                //file.write("\n");

            }


            qDebug() << "4029_8";

            float fourtybelow = res3 / 8388.604;
            int helper3 = fourtybelow * 128.0;

            QString helper4 = QString("%1").arg(helper3,6,16,QLatin1Char('0'));


            if(helper3 < 0)
            {

                QByteArray neg1 = neg1.number(helper3,16);
                QByteArray neg2 = neg1.mid(neg1.size() - 6,6);
                IN_4029s.masterz = neg2.toUpper();

            }
            else
            {
                QString fourtzout = QString("%1").arg(helper3,6,16,QLatin1Char('0'));
                IN_4029s.masterz = fourtzout.toUtf8().toUpper();
            }

            //            //file.write("IN_4029s.masterz = " +  IN_4029s.masterz);
            //            //file.write("\n");
            //            //file.write("helper3 = " + QString::number(helper3).toUtf8());
            //            //file.write("\n");

            /////////////////////////////////////////////////////////Y//////////////////////////////////////////////////

            qDebug() << "4029_9";

            if(IN_4029s.currentxor == 0)
            {
                QByteArray yholder0 = IN_4029s.decopacket1.mid(14,6);
                ydeconumber1 = yholder0.toInt(nullptr,16);
                res2 = ydeconumber1 - 0x1C71C7;

                IN_4029s.Y_stack.push(ydeconumber1);

                if(IN_4029s.Y_stack.size()  >  255)
                {
                    IN_4029s.Y_stack.removeAt(0);
                }

                //                //file.write("yholder0 = " +  yholder0);
                //                //file.write("\n");
                //                //file.write("res2 = " + QString::number(res2).toUtf8());
                //                //file.write("\n");

            }

            qDebug() << "4029_10";

            if(IN_4029s.currentxor != 0 && IN_4029s.currentxor <= 255)
            {
                QByteArray yholder1 = IN_4029s.decopacket1.mid(14,6); //get Y
                ydeconumber1 = yholder1.toInt(nullptr,16);
                int stackposy1 = IN_4029s.currentxor;
                int stackcounty1 = IN_4029s.Y_stack.size();
                int yclientnumber1 = IN_4029s.Y_stack.at(stackcounty1 - stackposy1);
                xorresult2 = ydeconumber1 ^ yclientnumber1; //xor with ringbuffer
                IN_4029s.Y_stack.push(xorresult2);

                if(IN_4029s.Y_stack.size()  >  255)
                {
                    IN_4029s.Y_stack.removeAt(0);
                }

                res2 =  xorresult2 - 0x1C71C7;

                //                //file.write("yholder1 = " +  yholder1);
                //                //file.write("\n");
                //                //file.write("res2 = " + QString::number(res2).toUtf8());
                //                //file.write("\n");

            }

            qDebug() << "4029_11";



            float fourtyflo = res2 /  466.033798648;
            int helper2 = fourtyflo * 128.0;

            QString fourtyout = QString("%1").arg(helper2,6,16,QLatin1Char('0'));
            IN_4029s.mastery = fourtyout.toUtf8().toUpper();



            //            //file.write("IN_4029s.mastery = " +  IN_4029s.mastery);
            //            //file.write("\n");
            //            //file.write("helper2 = " + QString::number(helper2).toUtf8());
            //            //file.write("\n");

            ////////////////////////////////////////////////////////////////////////////////Y//////////////////////////////////////////////////


            //            //file.write("IN_4029s.masterf = " + IN_4029s.masterf.toUpper());
            //            //file.write("\n");
            //            //file.write("IN_4029s.myAnimation = " + IN_4029s.myAnimation.toUpper());
            //            //file.write("\n");
            //            //file.close();



            QString  myx1 = IN_4029s.masterx;
            int xfromhex1 = myx1.toInt(nullptr,16);
            float maintoonx1 = xfromhex1 / 128.0;

            QString  myz1 = IN_4029s.masterz;
            int zfromhex1 = myz1.toInt(nullptr,16);
            float maintoonz1 = zfromhex1 / 128.0;

            if(maintoonz1 > 130000)
            {
                maintoonz1 = maintoonz1 - 131072.00195193;
            }


            QString  myy1 = IN_4029s.mastery;
            int yfromhex1 = myy1.toInt(nullptr,16);
            float maintoony1 = yfromhex1 / 128.0;



            float maintoonx2 = maintoonx1;
            float maintoonz2 = maintoonz1;
            float maintoony2 = maintoony1;

            float X = maintoonx2;
            float Z = maintoonz2;
            float Y = maintoony2;

            IN_4029s.Xh = X;
            IN_4029s.Zh = Z;
            IN_4029s.Yh = Y;

            QString isitfalse = "";

            qDebug() << "4029_12";
            
            if(IN_4029s.MyZone != IN_4029s.LastZone || IN_4029s.LastWorld != IN_4029s.masterworld)//Checking the zone only when leaving a zone.
            {
                IN_4029s.CheckingTheZone = true;

                qDebug() << "4029_12_1";

                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_4029s);

                checkthezone SendTo;
                SendTo.CheckTheZone(ClientID ,ClientsIP,ClientsPort);//send out to check the zone

                IN_4029s = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

                qDebug() << "4029_12_2";

                IN_4029s.LastWorld = IN_4029s.masterworld;
                IN_4029s.LastZone = IN_4029s.MyZone;

                IN_4029s.ZoneX = IN_4029s.XandYvector.at(0);
                IN_4029s.ZoneY = IN_4029s.XandYvector.at(1);
                
                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_4029s);


                IN_4029s = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

                
            }

            qDebug() << "4029_13";


            float West1 = 0;
            float East1 = 0;
            float North1 = 0;
            float South1 = 0;

            West1 = 133.34;
            East1 = 133.34;
            North1 = 133.34;
            South1 = 133.34;

            float West0 = 0.0;
            float East0 = 0.0;
            float North0 = 0.0;
            float South0 = 0.0;

            West0 = X - IN_4029s.ZoneX  + 2000/6;
            East0 = IN_4029s.ZoneX - X + 2000/6;
            North0 = Y - IN_4029s.ZoneY + 2000/6;
            South0 = IN_4029s.ZoneY - Y + 2000/6;

            QString North2 = QString("%1").arg(North0,0,'g',10,QLatin1Char('0'));
            QString West2 = QString("%1").arg(West0,0,'g',10,QLatin1Char('0'));
            QString East2 = QString("%1").arg(East0,0,'g',10,QLatin1Char('0'));
            QString South2 = QString("%1").arg(South0,0,'g',10,QLatin1Char('0'));

            QString world10 = QString("%1").arg(IN_4029s.masterworld,1,10,QLatin1Char('0')).toUpper();

            int masterworld2 = world10.toInt(nullptr,10);

            QVector<QString> AllZoneNames = checkthezone::WorldZoneNamesVector.at(masterworld2);

            float West = IN_4029s.ZoneX - West1;
            float East = IN_4029s.ZoneX + East1;
            float North = IN_4029s.ZoneY - North1;
            float South = IN_4029s.ZoneY + South1;

            QString North3 = QString("%1").arg(North,0,'g',10,QLatin1Char('0'));
            QString West3 = QString("%1").arg(West,0,'g',10,QLatin1Char('0'));
            QString East3 = QString("%1").arg(East,0,'g',10,QLatin1Char('0'));
            QString South3 = QString("%1").arg(South,0,'g',10,QLatin1Char('0'));

            if(X > West)
            {
                IN_4029s.CheckWest = "NO";
            }
            else
            {
                if(X >= West - 200 && X <= West && IN_4029s.CheckWest == "NO")
                {
                    IN_4029s.CheckCentral = "YES";
                    IN_4029s.CheckWest = "YES";
                    IN_4029s.MyZone = "CheckTheZone";
                }
            }

            if(X < East)
            {
                IN_4029s.CheckEast = "NO";
            }
            else
            {
                if(X >= East && X <= East + 200 && IN_4029s.CheckEast == "NO")
                {
                    IN_4029s.CheckCentral = "YES";
                    IN_4029s.CheckEast = "YES";
                    IN_4029s.MyZone = "CheckTheZone";
                }
            }

            if(Y > North)
            {
                IN_4029s.CheckNorth = "NO";
            }
            else
            {
                if(Y >= North - 200 && Y <= North && IN_4029s.CheckNorth == "NO")
                {
                    IN_4029s.CheckCentral = "YES";
                    IN_4029s.CheckNorth = "YES";
                    IN_4029s.MyZone = "CheckTheZone";
                }
            }

            if(Y < South)
            {
                IN_4029s.CheckSouth = "NO";
            }
            else
            {
                if(Y >= South && Y <= South + 200 && IN_4029s.CheckSouth == "NO")
                {
                    IN_4029s.CheckCentral = "YES";
                    IN_4029s.CheckSouth = "YES";
                    IN_4029s.MyZone = "CheckTheZone";
                }
            }

            if(
                    X > West && X < East &&
                    Y > North && Y < South
                    )
            {
                if(IN_4029s.CheckCentral == "YES")
                {
                    IN_4029s.CheckCentral = "NO";
                    IN_4029s.MyZone = "CheckTheZone";
                }
            }

            if(
                    X <= IN_4029s.ZoneX - 333.33 || X >= IN_4029s.ZoneX + 333.33 ||
                    Y <= IN_4029s.ZoneY - 333.33 || Y >= IN_4029s.ZoneY + 333.33)
            {
                IN_4029s.MyZone = "CheckTheZone";
            }

            float result = 0.0;

            QStringList holdid;
            QStringList holdx;
            QStringList holdz;
            QStringList holdy;

            if(IN_4029s.holdid2.contains(IN_4029s.Target_ID))
            {
                holdid = IN_4029s.holdid2;
                holdx = IN_4029s.holdx2;
                holdz = IN_4029s.holdz2;
                holdy = IN_4029s.holdy2;
            }
            else
            {
                holdid = IN_4029s.holdid1;
                holdx = IN_4029s.holdx;
                holdz = IN_4029s.holdz;
                holdy = IN_4029s.holdy;
            }

            if(holdid.contains(IN_4029s.Target_ID))
            {
                int Index = holdid.indexOf(IN_4029s.Target_ID);

                QString tempstrx = holdx.at(Index);
                int tempint1 = tempstrx.toInt(nullptr,16);
                float xinterlude = tempint1 / 128.0;

                QString tempstrz = holdz.at(Index);
                int tempint2 = tempstrz.toInt(nullptr,16);
                float  zinterlude = tempint2 / 128.0;

                if(zinterlude > 130000)
                {
                    zinterlude = zinterlude - 131072.00195193;
                }

                QString tempstry = holdy.at(Index);
                int tempint3 = tempstry.toInt(nullptr,16);
                float yinterlude = tempint3 / 128.0;


                float sub1 = maintoonx1 - xinterlude;
                float sub2 = maintoonz1 - zinterlude;
                float sub3 = maintoony1 - yinterlude;

                float power1 = qPow(sub1,2);
                float power2 = qPow(sub2,2);
                float power3 = qPow(sub3,2);

                float firstresult = power1 + power2 + power3;
                result = qSqrt(firstresult);
            }

            qDebug() << "4029_14";


            IN_4029s.MySwingRadius = result;

            QString output1 = QString("%1").arg(IN_4029s.MySwingRadius,0,'g',10,QLatin1Char('0'));

            if(IN_4029s.CombatAllValuesMap.isEmpty() == false)
            {
                QMapIterator<QString,QVector<QString>> iter(IN_4029s.CombatAllValuesMap);
                while(iter.hasNext())
                {
                    iter.next();
                    QString NPCID = iter.key();

                    if(NPCID != IN_4029s.CharSelectID_Main && NPCID.isEmpty() == false && !MainWindow::StopMovementVector.contains(NPCID))
                    {
                        FacingDirection New_Movement1; //get class

                        QString MOB_Master_c9 = objectpacket::Master_Map.value(NPCID.toUtf8().toUpper());
                        QString MobX = MOB_Master_c9.mid(12,6);
                        QString MobZ = MOB_Master_c9.mid(18,6);
                        QString MobY = MOB_Master_c9.mid(24,6);
                        QString MobHP_OldVal2 = MOB_Master_c9.mid(54,2);//HP2

                        qDebug() << "MobX2" << MobX;
                        qDebug() << "MobZ2" << MobZ;
                        qDebug() << "MobY2" << MobY;
                        qDebug() << "MobC9HP" << MobHP_OldVal2;

                        QByteArray ID_IP_PORT = IN_4029s.ClientsID + IN_4029s.ClientsIP + IN_4029s.ClientsPort;

                        if(MobHP_OldVal2 != "00")
                        {
                            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_4029s);

                            QString xorC9 = New_Movement1.AGGRO_Radius_Movement
                                    (IN_4029s.ClientsID,IN_4029s.ClientsIP,IN_4029s.ClientsPort,
                                     IN_4029s.masterx, IN_4029s.masterz, IN_4029s.mastery,
                                     NPCID,MobX, MobZ, MobY);

                            IN_4029s = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);


                            if(xorC9.isEmpty() == false)
                            {
                                QByteArray Channel = xorC9.mid(0,2).toUtf8();

                                qDebug() << "Channel" << Channel;

                                QVector <QByteArray> CurrenChannelVector = IN_4029s.MapOfChannelVectors.value(Channel);

                                qDebug() << "CurrenChannelVector.size() = " << CurrenChannelVector.size();
                                qDebug() << "CurrenChannelVector" << CurrenChannelVector;

                                CurrenChannelVector.append(xorC9.toUtf8());
                                IN_4029s.MapOfChannelVectors.insert(Channel,CurrenChannelVector);

                                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_4029s);

                                IN_4029s = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                            }
                        }
                    }
                }
            }
        }

        qDebug() << "IN_4029s.fortyexist2" << IN_4029s.fortyexist;

        carata = 0;
        caratb = 0;
        high2 = 0;
        low2 = 0;
        packetsize2 = 0;
        where2 = 0;

        qDebug() << "4029_15";

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_4029s);

        IN_4029s = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    }




    qDebug() << "IN_4029s.Return4029.size()" << IN_4029s.Return4029.size();

    //        ui->lineEdit_2->setText(IN_4029s.masterWE.toUpper());
    //        ui->lineEdit_3->setText(IN_4029s.masterNS.toUpper());
    //        ui->lineEdit_4->setText(IN_4029s.masterx.toUpper());
    //        ui->lineEdit_5->setText(IN_4029s.masterz.toUpper());
    //        ui->lineEdit_6->setText(IN_4029s.mastery.toUpper());
    //        ui->lineEdit_7->setText(IN_4029s.masterf.toUpper());
    //        ui->lineEdit_8->setText(IN_4029s.myAnimation.toUpper());

    if(IN_4029s.Return4029.isEmpty() == false)
    {
        QHostAddress current_client_address;
        current_client_address.setAddress(QString::fromUtf8(ClientsIP));
        quint16 current_client_port = ClientsPort.toUShort(nullptr,10);
        QString changeIP = current_client_address.toString();


        for(int rd = 0; rd < IN_4029s.Return4029.size(); rd+=2)
        {
            QByteArray Data = IN_4029s.Return4029.at(rd);
            QByteArray Description = IN_4029s.Return4029.at(rd + 1);

            Data.replace(22,6,IN_4029s.SessionID);

            crc sendcrc;
            QByteArray outcrc =  sendcrc.elcrc(Data); //send to get a crc
            Data.append(outcrc);

            QString MainToon = checkthezone::NPCsNames.value(IN_4029s.CharSelectID);


            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

            qDebug() << "";
            qDebug() << "";
            qDebug() << "";
            qDebug() << dateTimeString;

            qDebug() << "4029_Return" + Description;
            qDebug() << "To Client: " + MainToon;
            qDebug() << "IP: " + ClientsIP;
            qDebug() << "Port: " + ClientsPort;
            qDebug() << "From Server: ";
            qDebug() << Data;
            qDebug() << "";

            qDebug() << "4029_Return" << Data;
            QByteArray gameout2 = Data.fromHex(Data);

            QHostAddress newsender = QHostAddress(changeIP);
            worldthread::udpsocket3->writeDatagram(gameout2,newsender,current_client_port);

            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("");
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append(dateTimeString);
            MainWindow::UiOputVector.append("4029_Return");
            MainWindow::UiOputVector.append("To Client: " + MainToon);
            MainWindow::UiOputVector.append("IP: " + ClientsIP);
            MainWindow::UiOputVector.append("Port: " + ClientsPort);
            MainWindow::UiOputVector.append("From Server: Packet:");
            MainWindow::UiOputVector.append(Data);
            MainWindow::UiOputVector.append("----------------------------------------------------------");
            MainWindow::UiOputVector.append("");
        }

        IN_4029s.Return4029.clear();

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_4029s);

    }


    qDebug() << "IN_4029s.fortyexist3" << IN_4029s.fortyexist;


    return "4029_Return";

}


