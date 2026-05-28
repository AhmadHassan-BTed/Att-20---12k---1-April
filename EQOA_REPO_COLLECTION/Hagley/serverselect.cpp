#include "serverselect.h"
#include "crc.h"
#include "packetparsing.h"
#include "login.h"
#include "opcodes.h"
#include "increment.h"
#include "packetsize.h"

serverselect::serverselect(QObject *parent) : QObject(parent)
{

}
QVector <QByteArray> serverselect::serverselect_main(QByteArray ClientIP, QByteArray ClientPort, QByteArray serv_input, QString mainip)
{

    QByteArray gamebuf2 = serv_input;
    QByteArray clientid2 = gamebuf2.mid(0,4).toUpper(); //get client id
    QByteArray serverid2 = gamebuf2.mid(4,4); //get server id


    qDebug() << "";
    qDebug() << "";
    qDebug() << "Inside serverselect_main Serverselect.cpp";

    qDebug() << "mainip" << mainip;



    packetparsing::packetvars IN_serverselect;

    QByteArray ClientMain = "";

    if(packetparsing::IPandPort_AllvariablesMap.contains(clientid2 + ClientIP + ClientPort))
    {
        qDebug() << "Already contained after Server Seletion";

        ClientMain = clientid2 + ClientIP + ClientPort;

        IN_serverselect = packetparsing::IPandPort_AllvariablesMap.value(ClientMain);
    }
    else
    {
        if(packetparsing::IPandPort_AllvariablesMap.contains(clientid2 + ClientIP))
        {
            qDebug() << "Already added at Server Selection";

            ClientMain = clientid2 + ClientIP;

            IN_serverselect = packetparsing::IPandPort_AllvariablesMap.value(ClientMain);
        }
        else
        {
            if(packetparsing::IPandPort_AllvariablesMap.contains(ClientIP))
            {
                qDebug() << "Already Contained Before Server Selection";

                ClientMain = ClientIP;

                IN_serverselect = packetparsing::IPandPort_AllvariablesMap.value(ClientMain);
            }
            else
            {
                qDebug() << "Did not contain";


                if(!packetparsing::IPandPort_AllvariablesMap.contains(ClientIP))
                {
                    ClientMain = ClientIP;

                    IN_serverselect = packetparsing::pX;
                    IN_serverselect.CharSelectID = ClientIP;
                    packetparsing::IPandPort_AllvariablesMap.insert(ClientMain,IN_serverselect);
                    qDebug() << "Now it contains";
                }
            }
        }
    }

    QByteArray clientdata = serv_input.toUpper();
    QByteArray Original = clientdata;

    if(clientdata.size() == 42)
    {

        qDebug() << "Client packet size fixed at 18";
        qDebug() << "Original Packet";
        qDebug() << Original;

        clientdata.insert(18,IN_serverselect.OptionFlag);
        qDebug() << clientdata;
    }

    if(clientdata.size() == 36)
    {

        qDebug() << "Client packet size fixed at 12";
        qDebug() << "Original Packet";
        qDebug() << Original;

        clientdata.insert(12,IN_serverselect.ServerFlag + clientdata.mid(0,4) + IN_serverselect.OptionFlag);
        qDebug() << clientdata;
    }

    gamebuf2 = clientdata;

    QByteArray messageid2 = gamebuf2.mid(12,16);
    QByteArray serverflag1 = gamebuf2.mid(12,2);
    QByteArray serverhello = gamebuf2.mid(22,2);
    QByteArray optionflag1 = gamebuf2.mid(18,4);

    QByteArray RemoveFromGamebuf1 = gamebuf2.toUpper();

    RemoveFromGamebuf1.remove(gamebuf2.size() -8,8);//remove crc

    qDebug() << "IN_serverselect.RemoveFromGamebuf1 = " << RemoveFromGamebuf1;

    //864EC18488E001864E0200206200F901030012
    //8A9EC18488E0018A9E0100200700F901050012xxxxxxxx
    //A243C18488E001A2430400200300F9010300122AFBCF01
    //3E4DC18488E0013E4D0200200300F901030012428EC547
    //D6C1C184882001D6C10100200600F901060012F00C6E48
    //8A9EC1848820          200800F901060012
    //8AF5C1848820018AF5    200600F901060012
    //8AF5C18488E0018AF50100200300F901030012
    //E000C18488E001E0000A00200500F901050012




    RemoveFromGamebuf1.remove(0,24);

    IN_serverselect.TrimTheGamebuf1 = IN_serverselect.gamebuf1;

    IN_serverselect.TrimTheGamebuf1.remove(0,28);



    int indexF9 = RemoveFromGamebuf1.indexOf("F901");
    int indexF9trim = IN_serverselect.TrimTheGamebuf1.indexOf("F901");

    IN_serverselect.Clients_Last_MessageNumber =  RemoveFromGamebuf1.mid(0,4);

    qDebug() << "IN_serverselect.RemoveFromGamebuf2 = " << RemoveFromGamebuf1;
    qDebug() << "IN_serverselect.TrimTheGamebuf1 = " << IN_serverselect.TrimTheGamebuf1;
    qDebug() << "indexF9_1 = " << indexF9;
    qDebug() << "indexF9trim_1 = " << indexF9trim;

    packetparsing::IPandPort_AllvariablesMap.insert(ClientMain,IN_serverselect);
    IN_serverselect = packetparsing::IPandPort_AllvariablesMap.value(ClientMain);

    QByteArray Holder = "";


    indexF9 = RemoveFromGamebuf1.indexOf("F901");
    indexF9trim = IN_serverselect.TrimTheGamebuf1.indexOf("F901");

    qDebug() << "indexF9_1 = " << indexF9;
    qDebug() << "indexF9trim_1 = " << indexF9trim;

    if(indexF9 != -1)
    {
        qDebug() << "Inside F901's";
        //F901050012
        QByteArray FullFB = RemoveFromGamebuf1.mid(indexF9,10).toUpper();

        qDebug() << "FullFB = " << FullFB;

        QByteArray opcoderesult1 = FullFB.mid(8,2).toUpper();

        qDebug() << "opcoderesult1 = " << opcoderesult1;

        if(opvect.contains(opcoderesult1)) //check against the list to see if its a real opcode
        {
            IN_serverselect.FullFB = FullFB;

            QByteArray FromClient = IN_serverselect.FullFB.mid(4,4).toUpper();
            QByteArray Size = IN_serverselect.FullFB.mid(2,2).toUpper();
            QByteArray twelve = IN_serverselect.FullFB.mid(8,2).toUpper();

            IN_serverselect.opcoderesult1 = IN_serverselect.FullFB.mid(8,2).toUpper();

            increment sendto;
            QByteArray incthis1 = sendto.count(IN_serverselect.Clients_Last_FB_MessageNumber).toUpper();
            QByteArray incthis2 = sendto.count(incthis1).toUpper();//0700
            QByteArray incthis3 = sendto.count(incthis2).toUpper();

            qDebug() << "Size = 01" << Size;
            qDebug() << "twelve = 12" << twelve;

            if(Size == "01" && twelve == "12")
            {
                if(FromClient.toUpper() != incthis1.toUpper() &&//not true
                        FromClient.toUpper() != incthis2.toUpper() &&//not true
                        FromClient.toUpper() != incthis3.toUpper() &&//not true
                        FromClient.toUpper() != IN_serverselect.Clients_Last_FB_MessageNumber.toUpper())//not true)
                {
                    IN_serverselect.FBMessageNumber =  IN_serverselect.FBMessageNumber.toUpper();
                    IN_serverselect.Clients_Last_FB_MessageNumber = IN_serverselect.FBMessageNumber;

                    qDebug() << "if(From client then) ";
                    qDebug() << "IN_serverselect.Clients_Last_MessageNumber = " << IN_serverselect.Clients_Last_MessageNumber;
                    qDebug() << "IN_serverselect.Clients_Last_FB_MessageNumber = " << IN_serverselect.Clients_Last_FB_MessageNumber;
                    qDebug() << "IN_serverselect.FBMessageNumber = " << IN_serverselect.FBMessageNumber;
                    qDebug() << "IN_serverselect.FullFB = " << IN_serverselect.FullFB;
                    qDebug() << "IN_serverselect.TrimTheGamebuf1 = " << IN_serverselect.TrimTheGamebuf1;
                    qDebug() << "IN_serverselect.FBMessageNumber = " << IN_serverselect.FBMessageNumber;
                }
                else
                {
                    qDebug() << "if(From client else) ";
                    IN_serverselect.Clients_Last_FB_MessageNumber = FromClient.toUpper();

                    if(IN_serverselect.OptionFlag.isEmpty() == true)
                    {
                        IN_serverselect.OptionFlag = optionflag1;
                        IN_serverselect.ServerFlag = serverflag1;
                    }

                    IN_serverselect.FBMessageNumber = FromClient.toUpper();

                    Holder = serverid2 + clientid2 + "8760" + clientid2 + IN_serverselect.OptionFlag + "23";

                    qDebug() << "IN_serverselect.Clients_Last_MessageNumber = " << IN_serverselect.Clients_Last_MessageNumber;
                    qDebug() << "IN_serverselect.Clients_Last_FB_MessageNumber = " << IN_serverselect.Clients_Last_FB_MessageNumber;
                    qDebug() << "IN_serverselect.FBMessageNumber = " << IN_serverselect.FBMessageNumber;
                    qDebug() << "IN_serverselect.FullFB = " << IN_serverselect.FullFB;
                    qDebug() << "IN_serverselect.TrimTheGamebuf1 = " << IN_serverselect.TrimTheGamebuf1;
                    qDebug() << "IN_serverselect.FBMessageNumber = " << IN_serverselect.FBMessageNumber;



                    if(indexF9trim != -1)
                    {
                        IN_serverselect.TrimTheGamebuf1.remove(indexF9trim,IN_serverselect.FullFB.size());
                        qDebug() << "IN_serverselect.TrimTheGamebuf1 = " << IN_serverselect.TrimTheGamebuf1;
                    }

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientMain,IN_serverselect);
                }
            }
        }
        else
        {
            IN_serverselect.FBMessageNumber =  IN_serverselect.Clients_Last_FB_MessageNumber.toUpper();
            packetparsing::IPandPort_AllvariablesMap.insert(ClientMain,IN_serverselect);
        }

        RemoveFromGamebuf1.remove(indexF9,2);
    }


    qDebug() << "Inside_serverselect.cpp_IN_serverselect.UserName = " << IN_serverselect.UserName;

    if(Holder.isEmpty() == false)
    {

        QVector <QByteArray> serv1vector;
        serv1vector.append("Server_Keep_Alive");
        serv1vector.append("ServerSelection_F901_Reply");
        serv1vector.append(Holder);

        return serv1vector;
    }




    if((serverflag1 == "01" && serverhello == "23" && IN_serverselect.Packet156 == true) ||
            (serverflag1 == "21" && serverhello == "23" && IN_serverselect.Packet156 == true))
    {
        IN_serverselect.Packet156 = false;

        QByteArray session1 = messageid2; //get session id
        QByteArray chopped2 = session1.mid(2,4);

        if(chopped2 == clientid2) //are they the same?
        {
            QByteArray Session = IN_serverselect.SessionID;
            packetparsing::IPandPort_AllvariablesMap.remove(ClientMain);

            IN_serverselect.clientid1 = clientid2;
            IN_serverselect.serverid1 = serverid2;
            IN_serverselect.SessionID = Session;

            packetparsing::IPandPort_AllvariablesMap.insert(clientid2 + ClientIP,IN_serverselect);

            qDebug() << "ServerSelectScreen_IN_serverselect.SessionID = " << IN_serverselect.SessionID;
            qDebug() << "ServerSelectScreen_IN_serverselect.CharSelectID = " << IN_serverselect.CharSelectID;
            qDebug() << "ServerSelectScreen_IN_serverselect.UserName = " << IN_serverselect.UserName;

            if(!Opcodes::CharID_ServerMessageNumbers.contains(IN_serverselect.CharSelectID) && IN_serverselect.CharSelectID.isEmpty() == false)
            {
                qDebug() << "Opcodes::CharID_ServerMessageNumbers did not contain";

                QVector<QByteArray> tempVect =
                {"0000","0000","0000"};

                Opcodes::CharID_ServerMessageNumbers.insert(IN_serverselect.CharSelectID,tempVect);
            }
            else
            {
                qDebug() << "Opcodes::CharID_ServerMessageNumbers does contain";
            }

            qDebug() << "IN_serverselect.CharSelectID" << IN_serverselect.CharSelectID;
            qDebug() << "IN_serverselect.Opcodes::CharID_ServerMessageNumbers" << Opcodes::CharID_ServerMessageNumbers;



            QByteArray holder158 = packet158;
            holder158.replace(4,4,clientid2); //write client id into response 2 places
            holder158.replace(12,4,clientid2);
            holder158.replace(16,4,optionflag1); //add server select 03-05 option from client
            QString hexip = QString("%1").arg(QHostAddress(mainip).toIPv4Address(), 8, 16, QLatin1Char( '0' )); //make sure we get a 8 byte ip
            QByteArray outip = hexip.toUtf8(); //move ip to array
            QByteArray first2 =  outip.mid(0,2); //reverse it
            QByteArray second2 = outip.mid(2,2);
            QByteArray third2 =  outip.mid(4,2);
            QByteArray fourth2 = outip.mid(6,2);
            QByteArray bigendianip = fourth2+= third2 += second2 += first2;

//            holder158.replace(122,8,bigendianip); //castle lightwolf
//            holder158.replace(190,8,bigendianip); //diren hold
//            holder158.replace(270,8,bigendianip); //ferrans hope
//            holder158.replace(330,8,bigendianip); //hodstock
//            holder158.replace(402,8,bigendianip); //marrs fist
//            holder158.replace(498,8,bigendianip); //proudpine outpost
//            holder158.replace(578,8,bigendianip); //hagley test

            ///////////////////////ip replacement table/////////////////////////////
//            holder158.replace(250,8,bigendianip); //castle lightwolf
//            holder158.replace(318,8,bigendianip); //diren hold
//            holder158.replace(398,8,bigendianip); //ferrans hope
//            holder158.replace(458,8,bigendianip); //hodstock
//            holder158.replace(530,8,bigendianip); //marrs fist
//            holder158.replace(626,8,bigendianip); //proudpine outpost
//            holder158.replace(706,8,bigendianip); //hagley test
            ///////////////////////ip replacement table////////////////////////////

//            packetsize ResizePacket;
//            QByteArray ResizePacket2 = ResizePacket.switchsize(holder158);
//            holder158.replace(8,4,ResizePacket2);

            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_serverselect.CharSelectID);
            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

            increment GrpOut;
            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
            holder158.replace(22,4,GrpOut_a);
            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            Opcodes::CharID_ServerMessageNumbers.insert(IN_serverselect.CharSelectID,MessNumbVect);


            QByteArray byte1 = holder158; //send array to crc
            crc sendcrc;
            QByteArray outcrc =  sendcrc.elcrc(byte1); //send to get a crc
            holder158.append(outcrc);

            qDebug() << "Server Selection Screen" << holder158;
            qDebug() << "IN_serverselect.Clients_Last_FB_MessageNumber 163" << IN_serverselect.Clients_Last_FB_MessageNumber;

            qDebug() << "";

            QByteArray gameout1 = holder158.fromHex(holder158);

            QVector <QByteArray> serv1vector;
            serv1vector.append(gameout1);
            serv1vector.append("Server_Select...");
            serv1vector.append(holder158);

            return serv1vector;

        }
    }
//20D0FEFFCAE02120D00100200100FB060100000025000000FB390200040900030000000400000045514F4105000000546573743201ED18C727C06E5786C3035A08E5A041E0B71C12AE76C498FDF3CEEB444A0A49B53BC7ED49

    if(gamebuf2.mid(56,4) == "0409" && gamebuf2.contains("45514F41"))
    {
        int index = gamebuf2.indexOf("45514F41");

        QByteArray UserNameSize1 = gamebuf2.mid(index + 8,2);
        int UserNameSize2 = UserNameSize1.toInt(nullptr,16);
        int UserNameSize3 = UserNameSize2 * 2;
        IN_serverselect.UserName = gamebuf2.mid(index + 16,UserNameSize3);

        qDebug() << "156_pS.CharSelectID = " << IN_serverselect.CharSelectID;

        if(!Opcodes::CharID_ServerMessageNumbers.contains(IN_serverselect.CharSelectID) && IN_serverselect.CharSelectID.isEmpty() == false)
        {
            qDebug() << "Opcodes::CharID_ServerMessageNumbers did not contain";

            QVector<QByteArray> tempVect =
            {"0000","0000","0000"};

            Opcodes::CharID_ServerMessageNumbers.insert(IN_serverselect.CharSelectID,tempVect);
        }
        else
        {
            qDebug() << "Opcodes::CharID_ServerMessageNumbers does contain";
        }

        qDebug() << "IN_serverselect.CharSelectID" << IN_serverselect.PacketReply;
        qDebug() << "IN_serverselect.Opcodes::CharID_ServerMessageNumbers" << Opcodes::CharID_ServerMessageNumbers;

        if(Opcodes::UserName_PasswordMap.contains(IN_serverselect.UserName))
        {
            IN_serverselect.PassWord = Opcodes::UserName_PasswordMap.value(IN_serverselect.UserName);
        }


        increment clientidC;

        QByteArray ClientsID_inc1 = clientidC.count(clientid2);
        QByteArray ClientsID_inc2 = gamebuf2.mid(14,4);

        qDebug() << "";
        qDebug() << "ClientsID_inc3 = " << ClientsID_inc1;
        qDebug() << "ClientsID_inc4 = " << ClientsID_inc2;

        if(ClientsID_inc1 != ClientsID_inc2)
        {
            ClientsID_inc1 = clientid2;
        }

        QByteArray holder156 = packet156;
        holder156.replace(4,4,clientid2); //write client id into response 3 places
        holder156.replace(12,4,ClientsID_inc1);
        holder156.replace(22,4,ClientsID_inc1);
        holder156.replace(16,4,optionflag1);
        holder156.replace(26,4,optionflag1);

        //0A1F1870956019702C006319702C23020003000200fb060100000025000000822BBD1B
        //C184FA4E9560FA4E600063FA4E6023010001000200fb060100000025000000B516A6AC
        //C18452A4956052A403006352A40300010001000200FB060100000025000000D8E86878

        QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_serverselect.CharSelectID);
        QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

        increment GrpOut;
        QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);
        holder156.replace(30,4,GrpOut_a);
        Servers_Last_MessageNumber = GrpOut_a.toUpper();

        MessNumbVect.replace(0,Servers_Last_MessageNumber);

        Opcodes::CharID_ServerMessageNumbers.insert(IN_serverselect.CharSelectID,MessNumbVect);

        QByteArray byte1 = holder156; //send array to crc
        crc sendcrc;
        QByteArray outcrc =  sendcrc.elcrc(byte1); //send to get a crc
        holder156.append(outcrc);

        qDebug() << "Server Packet 156" << holder156;
        qDebug() << "IN_serverselect.UserName" << IN_serverselect.UserName;

        QByteArray gameout1 =holder156.fromHex(holder156);
        QVector <QByteArray> serv1vector;
        serv1vector.append(gameout1);
        serv1vector.append("Packet 156 Sent...");
        serv1vector.append(holder156);

        IN_serverselect.Packet156 = true;

        packetparsing::IPandPort_AllvariablesMap.insert(ClientMain,IN_serverselect);

        return serv1vector;

    }







    QVector <QByteArray> null;
    null.append("NULL");
    return null;
}
