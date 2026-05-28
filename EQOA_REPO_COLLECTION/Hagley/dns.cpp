#include "dns.h"
#include "mainwindow.h"
#include "worldthread.h"
#include "opcodes.h"


dns::dns(QObject *parent) : QObject(parent)
{

}

QVector <QByteArray> dns::dns_main(QByteArray dns_input, QString mainip, QString sender, QByteArray senderPort)
{

    QByteArray hold = dns_input;


    QByteArray middle1 = hold.mid(26,16);
    QByteArray udpcheck1 = middle1.toUpper();
    qDebug() <<"gate/patch" <<hold;
    QString Data = hold.fromHex(hold);
    qDebug() <<"Data" << Data;

    QString MessageType = Data.mid(0,4);

    if(MessageType == "XXXX")
    {
        QHostAddress newsender = QHostAddress(sender);
        quint16 senderPort1 = senderPort.toUShort(nullptr,10);

        QString LargeString = "";

        if(db.isOpen())
        {
            db.close();
            db.removeDatabase(QSqlDatabase::defaultConnection);
        }

        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(Opcodes::m_FileName);
        db.open();

        QSqlQuery querysl ("SELECT COUNT (*) FROM MAINTOON_STARTING_LOCATIONS");

        int rotatesl = 0;

        while(querysl.next())
        {
            rotatesl = querysl.value(0).toInt();
        }

        for(int it = 1; it < rotatesl+1; it++)
        {

            querysl.prepare (QString ("SELECT * FROM MAINTOON_STARTING_LOCATIONS WHERE rowid = :val"));
            querysl.bindValue(":val",it);
            querysl.exec();

            qDebug() << "";



            QString PacketID = "";
            QString PlayersName = "";
            QString PlayersID = "";
            QString StartingCity = "";

            LargeString.clear();

            while(querysl.next())
            {
                PacketID = querysl.value(0).toString();;
                PlayersName = querysl.value(1).toString();;
                PlayersID = querysl.value(2).toString();
                StartingCity = querysl.value(3).toString();

                qDebug() << "PacketID" << PacketID;
                qDebug() << "PlayersID" << PlayersID;
                qDebug() << "PlayersName" << PlayersName;
                qDebug() << "StartingCity" << StartingCity;

                LargeString.append(PacketID + "#1#" + PlayersID + "#2#" + PlayersName + "#3#" + StartingCity);
            }



            worldthread::udpsocket3->writeDatagram(LargeString.toUtf8(),newsender,senderPort1);
        }

        LargeString = "Finished";
        worldthread::udpsocket3->writeDatagram(LargeString.toUtf8(),newsender,senderPort1);
    }




    QByteArray SendOut = "";


//000201000001000000000000057061746368196576657271756573746F6E6C696E65616476656E747572657303636F6D0000010001
    //0001010000010000000000000236390130033136380331393207696E2D61646472046172706100000C0001
    //BC940100000100000000000003777777076578616D706C6503636F6D0000010001

    int skip = 0;

    if (hold.contains("0001010000010000000000000") || hold.contains("7061746368196576") || hold.contains("23639013003313638033")) //PATCH DNS CALL
    {
        skip = 1;
        QByteArray header = hold.mid(0,4); //get 2 byte header from received packet
        QByteArray gateout2 = patchdns;
        header.append(gateout2);
        QString hexip = QString("%1").arg(QHostAddress(MainWindow::PublicMainip).toIPv4Address(), 8, 16, QLatin1Char( '0' )); //make sure we get a 8 byte ip

        qDebug() << "MainWindow::PublicMainip2" << MainWindow::PublicMainip;
        qDebug() << "hexip" << hexip;

        QByteArray outip = hexip.toUtf8().toUpper(); //move ip to array
        qDebug() << "outip2" << outip;
        QByteArray patchresult1 = header; //add ip to the packet
        qDebug() << "header" << header;
        qDebug() << "patchresult1" << patchresult1;
        QByteArray finalpatch1 = patchresult1.fromHex(patchresult1); //change from hex to send it out









        QByteArray outip1 = outip.mid(0,2);
        qDebug() << "outip1" << outip1;

        if(outip1 == "3A")
        {
            QString change = "\x3A";
            outip1 = change.toUtf8();

            qDebug() << "outip1x" << outip1;
        }
        else
        {
            outip1 = outip1.fromHex(outip1);
            qDebug() << "outip1xx" << outip1;
        }

        QByteArray outip2 = outip.mid(2,2);
        qDebug() << "outip2" << outip2;

        if(outip2 == "3A")
        {
            outip1.append("\\");

            QString change = "x3A";
            qDebug() << "change" << change;
            outip2 = change.toUtf8();

            qDebug() << "outip2x" << outip2;
        }
        else
        {
            outip2 = outip2.fromHex(outip2);
            qDebug() << "outip2xx" << outip2;
        }

        QByteArray outip3 = outip.mid(4,2);
        qDebug() << "outip3" << outip3;

        if(outip3 == "3A")
        {
            QString change = "\x3A";
            outip3 = change.toUtf8();

            qDebug() << "outip3x" << outip3;
        }
        else
        {
            outip3 = outip3.fromHex(outip3);
            qDebug() << "outip3xx" << outip3;
        }

        QByteArray outip4 = outip.mid(6,2);
        qDebug() << "outip4" << outip4;

        if(outip4 == "3A")
        {
            QString change = "\x3A";
            outip4 = change.toUtf8();

            qDebug() << "outip4x" << outip4;
        }
        else
        {
            outip4 = outip4.fromHex(outip4);
            qDebug() << "outip1xx" << outip4;
        }

        QByteArray Merge = outip1 + outip2 + outip3 + outip4;
        qDebug() << "Merge1" << Merge;

//        if(Merge.contains("\\x"))
//        {
//            int index = Merge.indexOf("\\");
//            qDebug() << "index" << index;

//            Merge.remove(index,1);

//            qDebug() << "Merge2" << Merge;
//        }


        finalpatch1.append(Merge);

        qDebug() << "finalpatch1" << finalpatch1;


        QVector <QByteArray> dnsvector;
        dnsvector.append(finalpatch1);
        dnsvector.append("PATCH DNS Done...");

        qDebug() << "finalpatch1" << finalpatch1;
        qDebug() << "PATCH DNS Done...";

        return dnsvector;


    }

    if ((hold.contains("0001010000010000000000000") && skip == 0) || (hold.contains("6761746531027573") && skip == 0)) //GATE DNS CALL
    {
        QByteArray header = hold.mid(0,4); //get 2 byte header from received packet
        QByteArray gateout1 = gatedns;
        header.append(gateout1);
        QString hexip = QString("%1").arg(QHostAddress(MainWindow::PublicMainip).toIPv4Address(), 8, 16, QLatin1Char( '0' )).toUpper(); //make sure we get a 8 byte ip

        qDebug() << "MainWindow::PublicMainip3" << MainWindow::PublicMainip;
        qDebug() << "hexip" << hexip;

        QByteArray outip = hexip.toUtf8().toUpper(); //move ip to array
        qDebug() << "outip3" << outip;
        QByteArray gateresult1 = header; //add ip to the packet
        qDebug() << "header" << header;
        qDebug() << "gateresult1" << gateresult1;
        QByteArray finalgate1 = gateresult1.fromHex(gateresult1); //change from hex to send it out

        QByteArray outip1 = outip.mid(0,2);
        outip1 = outip1.fromHex(outip1);
        QByteArray outip2 = outip.mid(2,2);
        outip2 = outip2.fromHex(outip2);
        QByteArray outip3 = outip.mid(4,2);
        outip3 = outip3.fromHex(outip3);
        QByteArray outip4 = outip.mid(6,2);
        outip4 = outip4.fromHex(outip4);

        qDebug() << "outip1" << outip1;
        qDebug() << "outip2" << outip2;
        qDebug() << "outip3" << outip3;
        qDebug() << "outip4" << outip4;

        if(outip1 == ":")
        {
            QString change = "\x3A";
            outip1 = change.toUtf8();

            qDebug() << "outip1x" << outip1;
        }

        if(outip2 == ":")
        {
            QString change = "\x3A";
            outip2 = change.toUtf8();

            qDebug() << "outip2x" << outip2;
        }

        if(outip3 == ":")
        {
            QString change = "\x3A";
            outip3 = change.toUtf8();

            qDebug() << "outip3x" << outip3;
        }

        if(outip4.contains(":"))
        {
            QString change = "\x3A";
            outip4 = change.toUtf8();

            qDebug() << "outip4x" << outip4;
        }

        QByteArray Merge = outip1 + outip2 + outip3 + outip4;
        qDebug() << "Merge1" << Merge;





        finalgate1.append(Merge);

        qDebug() << "finalgate1" << finalgate1;



        QVector <QByteArray> dnsvector;
        dnsvector.append(finalgate1);
        dnsvector.append("Gate DNS Done...");

        qDebug() << "finalpatch1" << finalgate1;
        qDebug() << "Gate DNS Done...";

        return dnsvector;

    }

    QVector <QByteArray> null;
    null.append("NULL");
    return null;
}
