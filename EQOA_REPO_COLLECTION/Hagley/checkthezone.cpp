#include "checkthezone.h"
#include "packetparsing.h"
#include "facingdirection.h"
#include "generator.h"
#include "objectpacket.h"
#include "charcreate.h"

checkthezone::checkthezone(QObject *parent) : QObject(parent)
{

}

QByteArray checkthezone::CheckTheZone(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort)
{
    packetparsing::packetvars IN_CheckTheZone;
    IN_CheckTheZone = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QStringList holdid3;// = TempHoldsVector.at(0);
    QStringList holdx3;// = TempHoldsVector.at(1);
    QStringList holdz3;// = TempHoldsVector.at(2);
    QStringList holdy3;// = TempHoldsVector.at(3);

    QStringList holdid4;// = TempHoldsVector.at(0);
    QStringList holdx4;// = TempHoldsVector.at(1);
    QStringList holdz4;// = TempHoldsVector.at(2);
    QStringList holdy4;// = TempHoldsVector.at(3);

    QVector <QVector<float>> CurrentZoneCentralVector = VectorOfWorlds.at(IN_CheckTheZone.masterworld);

    QString CurrentWorld = WorldNameVector.at(IN_CheckTheZone.masterworld);

    for(int CZ = 0; CZ < CurrentZoneCentralVector.size(); CZ++)
    {
        QVector <QString> tempWorldZoneNamesVector = WorldZoneNamesVector.at(IN_CheckTheZone.masterworld);

        QVector <float> CurrentZone = CurrentZoneCentralVector.at(CZ);
        QString ZonesName = tempWorldZoneNamesVector.at(CZ);

        QVector <QString> LocationsIDsVector;

        qDebug() << "IN_CheckTheZone.CurrentZone" << CurrentZone;


        float ZoneX1 = 0.0;
        float ZoneY1 = 0.0;



        ZoneX1 = CurrentZone.at(0);
        ZoneY1 = CurrentZone.at(2);

        //qDebug() << "IN_CheckTheZone.masterx" << IN_CheckTheZone.masterx;
        //qDebug() << "IN_CheckTheZone.mastery" << IN_CheckTheZone.mastery;
        //qDebug() << "IN_CheckTheZone.ZoneX1" << ZoneX1;
        //qDebug() << "IN_CheckTheZone.ZoneY1" << ZoneY1;

        QString tempstrx = IN_CheckTheZone.masterx;
        int tempint1 = tempstrx.toInt(nullptr,16);
        float NPCx = tempint1 / 128.0;



        QString tempstry = IN_CheckTheZone.mastery;
        int tempint3 = tempstry.toInt(nullptr,16);
        float NPCy = tempint3 / 128.0;

        float West = 0;
        float East = 0;
        float North = 0;
        float South = 0;

        //qDebug() << "IN_CheckTheZone.NPCx" << NPCx;
        //qDebug() << "IN_CheckTheZone.NPCy" << NPCy;


        if
                (
                 //North                 //South
                 NPCx >= ZoneX1 - 1000 && NPCx <= ZoneX1 + 1000 &&

                 //West                  //East
                 NPCy >= ZoneY1 - 1000 && NPCy <= ZoneY1 + 1000

                 )
        {
            if(ZonesName.contains("(C)"))
            {
                QString FileName = "/ZoneMapsPNG/" + ZonesName + "/" + ZonesName + ".png";

                IN_CheckTheZone.MyZoneCentral = directory + FileName;

                QPixmap pixmap;
                pixmap.load(IN_CheckTheZone.MyZoneCentral);

                qDebug() << "IN_CheckTheZone.MyZoneCentral" << IN_CheckTheZone.MyZoneCentral;

                FacingDirection::CurrentImageMap.insert(ClientID + ClientsIP + ClientsPort,pixmap);

                IN_CheckTheZone.MyZoneCentralX = ZoneX1;
                IN_CheckTheZone.MyZoneCentralY = ZoneY1;

                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_CheckTheZone);

                IN_CheckTheZone = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
            }
        }

        West = 333.34;
        East = 333.34;
        North = 333.34;
        South = 333.34;


        if
                (
                 //North                 //South
                 NPCx >= ZoneX1 - West && NPCx <= ZoneX1 + East &&

                 //West                  //East
                 NPCy >= ZoneY1 - North && NPCy <= ZoneY1 + South

                 )
        {




            IN_CheckTheZone.MyZone = ZonesName;

            qDebug() << "IN_CheckTheZone.MyZone" << IN_CheckTheZone.MyZone;



            //qDebug() << "IN_CheckTheZone.ZoneX1" << ZoneX1;
            //qDebug() << "IN_CheckTheZone.ZoneY1" << ZoneY1;
            //qDebug() << "IN_CheckTheZone.ZoneX" << IN_CheckTheZone.ZoneX;
            //qDebug() << "IN_CheckTheZone.ZoneY" << IN_CheckTheZone.ZoneY;

            if(ZoneX1 > 0 &&  ZoneY1 > 0)
            {
                IN_CheckTheZone.XandYvector.append(ZoneX1);
                IN_CheckTheZone.XandYvector.append(ZoneY1);
            }
            else
            {
                IN_CheckTheZone.XandYvector.append(IN_CheckTheZone.ZoneX);
                IN_CheckTheZone.XandYvector.append(IN_CheckTheZone.ZoneY);
            }




            QMap<QString,QMap<QString,QVector<QString>>> ZoneForOBJvectorsmaps = WorldsVectorOFObjectZonesMaps.at(IN_CheckTheZone.masterworld);

            QMap<QString,QVector<QString>> OBJ_XZYsVectorMap = ZoneForOBJvectorsmaps.value(ZonesName);




            QVector <QStringList> TempHoldsVector = Zones_IDXZYvectorMap.value(ZonesName);

            //qDebug() << "TempHoldsVector1.size()" << TempHoldsVector.size();

            QStringList holdid4 = TempHoldsVector.at(0);
            QStringList holdx4 = TempHoldsVector.at(1);
            QStringList holdz4 = TempHoldsVector.at(2);
            QStringList holdy4 = TempHoldsVector.at(3);

            for(int i = 0; i < holdid4.size(); i++)
            {
                if(!holdid3.contains(holdid4.at(i)))
                {
                    holdid3.append(holdid4.at(i));
                    holdx3.append(holdx4.at(i));
                    holdz3.append(holdz4.at(i));
                    holdy3.append(holdy4.at(i));
                }
            }

            for(int i = 0; i < holdid4.size(); i++)
            {
                if(!IN_CheckTheZone.holdid2.contains(holdid4.at(i)))
                {
                    IN_CheckTheZone.holdid2.append(holdid4.at(i));
                    IN_CheckTheZone.holdx2.append(holdx4.at(i));
                    IN_CheckTheZone.holdz2.append(holdz4.at(i));
                    IN_CheckTheZone.holdy2.append(holdy4.at(i));
                }
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_CheckTheZone);

            IN_CheckTheZone = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

            for(int i = 0; i < IN_CheckTheZone.holdid2.size(); i++)
            {
                if(!holdid3.contains(IN_CheckTheZone.holdid2.at(i)))
                {
                    if(!generator::Players_Map.contains(IN_CheckTheZone.holdid2.at(i).toUtf8()))
                    {
                        int removeAt = IN_CheckTheZone.holdid2.indexOf(IN_CheckTheZone.holdid2.at(i));
                        IN_CheckTheZone.holdid2.removeAt(removeAt);
                        IN_CheckTheZone.holdx2.removeAt(removeAt);
                        IN_CheckTheZone.holdz2.removeAt(removeAt);
                        IN_CheckTheZone.holdy2.removeAt(removeAt);
                    }
                }
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_CheckTheZone);

            IN_CheckTheZone = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);


        }

        West = 533.34;
        East = 533.34;
        North = 533.34;
        South = 533.34;


        if
                (
                 //North                 //South
                 NPCx >= ZoneX1 - West && NPCx <= ZoneX1 + East &&

                 //West                  //East
                 NPCy >= ZoneY1 - North && NPCy <= ZoneY1 + South && ZonesName != IN_CheckTheZone.MyZone

                 )
        {
            qDebug() << "ZonesName2" << ZonesName;



            QVector <QStringList> TempHoldsVector = Zones_IDXZYvectorMap.value(ZonesName);

            //qDebug() << "TempHoldsVector2.size()" << TempHoldsVector.size();

            QStringList holdid4 = TempHoldsVector.at(0);
            QStringList holdx4 = TempHoldsVector.at(1);
            QStringList holdz4 = TempHoldsVector.at(2);
            QStringList holdy4 = TempHoldsVector.at(3);

            for(int i = 0; i < holdid4.size(); i++)
            {
                if(!holdid3.contains(holdid4.at(i)))
                {
                    holdid3.append(holdid4.at(i));
                    holdx3.append(holdx4.at(i));
                    holdz3.append(holdz4.at(i));
                    holdy3.append(holdy4.at(i));
                }
            }

            for(int i = 0; i < holdid4.size(); i++)
            {
                if(!IN_CheckTheZone.holdid2.contains(holdid4.at(i)))
                {
                    IN_CheckTheZone.holdid2.append(holdid4.at(i));
                    IN_CheckTheZone.holdx2.append(holdx4.at(i));
                    IN_CheckTheZone.holdz2.append(holdz4.at(i));
                    IN_CheckTheZone.holdy2.append(holdy4.at(i));
                }
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_CheckTheZone);

            IN_CheckTheZone = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
        }
    }

    for(int i = 0; i < IN_CheckTheZone.holdid2.size(); i++)
    {
        if(!holdid3.contains(IN_CheckTheZone.holdid2.at(i)))
        {
            if(!generator::Players_Map.contains(IN_CheckTheZone.holdid2.at(i).toUtf8()))
            {
                int removeAt = IN_CheckTheZone.holdid2.indexOf(IN_CheckTheZone.holdid2.at(i));
                IN_CheckTheZone.holdid2.removeAt(removeAt);
                IN_CheckTheZone.holdx2.removeAt(removeAt);
                IN_CheckTheZone.holdz2.removeAt(removeAt);
                IN_CheckTheZone.holdy2.removeAt(removeAt);
            }
        }
    }

    QMapIterator<QByteArray,QByteArray> iter(generator::Players_Map);
    while(iter.hasNext())
    {
        iter.next();
        QByteArray PlayerID = iter.key();

        QString PlayersName = checkthezone::NPCsNames.value(PlayerID);
        qDebug() << "";
        qDebug() << "IN_CheckTheZone.PlayersName" << PlayersName;
        qDebug() << "IN_CheckTheZone.PlayerID" << PlayerID;
        qDebug() << "IN_CheckTheZone.CharSelectID" << IN_CheckTheZone.CharSelectID;

        if(PlayerID != IN_CheckTheZone.CharSelectID)// dont add yourself to your own map
        {
            int index = objectpacket::holdid1.indexOf(PlayerID);

            QString X = objectpacket::holdx.at(index);
            QString Z = objectpacket::holdz.at(index);
            QString Y = objectpacket::holdy.at(index);

            if(!IN_CheckTheZone.holdid2.contains(PlayerID))
            {
                IN_CheckTheZone.holdid2.append(PlayerID);
                IN_CheckTheZone.holdx2.append(X);
                IN_CheckTheZone.holdz2.append(Z);
                IN_CheckTheZone.holdy2.append(Y);

                qDebug() << "IN_CheckTheZone.X" << X;
                qDebug() << "IN_CheckTheZone.Z" << Z;
                qDebug() << "IN_CheckTheZone.Y" << Y;
            }
            else
            {
                int index = IN_CheckTheZone.holdid2.indexOf(PlayerID);

                IN_CheckTheZone.holdx2.replace(index,X);
                IN_CheckTheZone.holdz2.replace(index,Z);
                IN_CheckTheZone.holdy2.replace(index,Y);

                qDebug() << "IN_CheckTheZone.X_" << X;
                qDebug() << "IN_CheckTheZone.Z_" << Z;
                qDebug() << "IN_CheckTheZone.Y_" << Y;
            }
        }
        else
        {
            qDebug() << "IN_CheckTheZone.PlayersName Skip youself";
        }
    }





    IN_CheckTheZone.CheckingTheZone = false;

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_CheckTheZone);

    //qDebug() << "IN_CheckTheZone.FacingDirection::CurrentImageMap" << FacingDirection::CurrentImageMap;

    //qDebug() << "IN_CheckTheZone2.MyZone" << IN_CheckTheZone.MyZone;

    return "IN_CheckTheZone";

}
