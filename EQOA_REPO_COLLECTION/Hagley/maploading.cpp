#include "maploading.h"
#include "login.h"
#include "generator.h"
#include "opcodes.h"
#include "objectpacket.h"
#include "checkthezone.h"
#include "merchants.h"
#include "facingdirection.h"
#include "mainwindow.h"
#include "increment.h"
#include "quests.h"
#include "charcreate.h"
#include "coaching.h"
#include "loot.h"


maploading::maploading(QObject *parent) : QObject(parent)
{

}


void maploading::MapStart()
{
    f1 =  QtConcurrent::run(this,&maploading::Mapconstruction);
    watcher->setFuture(f1);
}


void maploading::Mapconstruction()
{

    QThread::currentThread()->setObjectName("MapLoading_Thread");
    qDebug() << "Maploading" << QThread::currentThread();

    //connect(watcher, SIGNAL(finished()),this, SLOT(Mapfinished()));


    if(db.isOpen())
    {
        db.close();
        db.removeDatabase(QSqlDatabase::defaultConnection);
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(Opcodes::m_FileName);
    db.open();


    qDebug() << "Opcodes::m_FileName" << Opcodes::m_FileName;
    qDebug() << "db.connectionName() " << db.connectionName();

    db.transaction();

    QSqlQuery query ("SELECT COUNT (*) FROM NPCS");

    QString Packet_ID = "";

    if(db.isOpen())
    {
        while(query.next())
        {
            Packet_ID = query.value(0).toString();
        }


        int Packet_ID2 = Packet_ID.toInt(nullptr,10);
        int Packet_ID3 = Packet_ID2 + 1;
        QString Packet_ID4 = QString::number(Packet_ID3,10);
        MaploadingToUIvector.append(Packet_ID4);

    }




////////////////////////////////////////////////////////////////////////////////////////////////

    QSqlQuery querysl ("SELECT COUNT (*) FROM MAINTOON_STARTING_LOCATIONS");

    int rotatesl = 0;

    while(querysl.next())
    {
        rotatesl = querysl.value(0).toInt();
    }

    qDebug() << "";
    qDebug() << "MAINTOON_STARTING_LOCATIONS_Count = " << rotatesl;

    for(int it = 1; it <= rotatesl; it++)
    {
        emit watcher->progressValueChanged(it);

        querysl.prepare (QString ("SELECT * FROM MAINTOON_STARTING_LOCATIONS WHERE rowid = :val"));
        querysl.bindValue(":val",it);
        querysl.exec();


        QString PlayersName = "";
        QString PlayersID = "";
        QString StartingCity = "";

        while(querysl.next())
        {
            PlayersName = querysl.value(1).toString();
            PlayersID = querysl.value(2).toString();
            StartingCity = querysl.value(3).toString();
        }

        qDebug() << "PlayersName = " << PlayersName;
        qDebug() << "PlayersID = " << PlayersID;
        qDebug() << "StartingCity = " << StartingCity;
        qDebug() << "";

        CharCreate::ID_StartingCityMap.insert(PlayersID,StartingCity);
    }

    //----------------------------------------------------------------------------------------------------

    QSqlQuery querysSM ("SELECT COUNT (*) FROM SPIRITMASTERS");

    int rotatesSM = 0;

    while(querysSM.next())
    {
        rotatesSM = querysSM.value(0).toInt();
    }

    int PacketID = 0;

    for(int it = 1; it < rotatesSM; it++)
    {
        emit watcher->progressValueChanged(it);

        querysSM.prepare (QString ("SELECT * FROM SPIRITMASTERS WHERE rowid = :val"));
        querysSM.bindValue(":val",it);
        querysSM.exec();

        QString Name = "";
        QString SpiritmastersID = "";
        QString OpcodeAndValue = "";
        QMap<int,QString> PacketID_Location;

        QVector<QString> tempvect;

        QSqlRecord record = querysSM.record();

        while(querysSM.next())
        {
            PacketID = querysSM.value(0).toInt();
            Name = querysSM.value(1).toString();
            SpiritmastersID = querysSM.value(2).toString();
            OpcodeAndValue = querysSM.value(3).toString();

            for(int re = 4; re < record.count(); re++)
            {
                QString Result = querysSM.value(re).toString();

                tempvect.append(Result);
            }
        }

        Quests::SpiritmastersID_MenuMap.insert(SpiritmastersID,tempvect);

        PacketID_Location.insert(PacketID,OpcodeAndValue);

        Quests::SpiritMastersID_PacketNumber_LocationMaps.insert(SpiritmastersID,PacketID_Location);
    }

    PacketID++;
    Quests::SpiritMastersPacketID = PacketID;


    //----------------------------------------------------------------------------------------------------


    //////////////////////////////////////////////////////////////////////////////////////////////////////

    QSqlQuery querysDF ("SELECT COUNT (*) FROM NPCS");

    int rotatesDF = 0;

    while(querysDF.next())
    {
        rotatesDF = querysDF.value(0).toInt();
    }

    querysDF.prepare (QString ("SELECT * FROM NPCS WHERE rowid = :val"));
    querysDF.bindValue(":val",rotatesDF);
    querysDF.exec();

    while (querysDF.next())
    {
        Opcodes::Old_ID = querysDF.value(5).toString().toUpper();
        Opcodes::RespawnID = querysDF.value(5).toString().toUpper();
    }

    Opcodes::NewspawnID = Opcodes::RespawnID;

    QByteArray split1 = Opcodes::Old_ID.toUtf8(); //AAAABBBB

    QByteArray first_4_digits = split1.mid(0,4); //AAAA
    QByteArray last_4_digits = split1.mid(4,4); //BBBB

    increment first1; //get class object
    QByteArray firstresult1 = first1.count(first_4_digits).toUpper(); //ABAA

    if(firstresult1 == "0000")
    {
        first_4_digits = "0000";

        increment second1; //get class object
        QByteArray secondresult1 = second1.count(last_4_digits).toUpper(); //if first is FFFF this will be BCBB //secondresult is bytearray in header
        QByteArray New_Char_ID = firstresult1 + secondresult1.toUpper();
        MaploadingToUIvector.append(New_Char_ID);
        MaploadingToUIvector.append(New_Char_ID);
        Opcodes::NewspawnID = New_Char_ID;
    }

    else
    {
        QByteArray New_Char_ID = firstresult1 + last_4_digits.toUpper();
        MaploadingToUIvector.append(New_Char_ID);
        MaploadingToUIvector.append(New_Char_ID);
        Opcodes::NewspawnID = New_Char_ID;
    }

    qDebug() << "";
    qDebug() << "";
    qDebug() << "";

    QString newtext = "NPCS";
    QString playtext = "MAINTOON";
    QString itemtext = "ITEMS";
    QString mertext = "MERCHANTS";
    QString bufftext = "BUFFS";
    QString accounttext = "ACCOUNTS";
    QString ZoneXZYs = "ZONES";
    QString RaceGen = "NAME_GENERATOR";
    QString Coaching = "COACHES";

    QSqlQuery queryC;
    queryC.prepare(QString("SELECT COUNT (*) FROM %1").arg(Coaching));
    queryC.exec();
    while (queryC.next())
    {
        tablesizeC =  queryC.value(0).toInt(nullptr);

    }

    QSqlQuery queryG;
    queryG.prepare(QString("SELECT COUNT (*) FROM %1").arg(RaceGen));
    queryG.exec();
    while (queryG.next())
    {
        tablesizeG =  queryG.value(0).toInt(nullptr);

    }

    QSqlQuery queryZ;
    queryZ.prepare(QString("SELECT COUNT (*) FROM %1").arg(ZoneXZYs));
    queryZ.exec();
    while (queryZ.next())
    {
        tablesizeZ =  queryZ.value(0).toInt(nullptr);

    }

    QSqlQuery query3;
    query3.prepare(QString("SELECT COUNT (*) FROM %1").arg(newtext)); //get size of npc table
    query3.exec();
    while (query3.next())
    {
        tablesize =  query3.value(0).toInt(nullptr);
    }

    QSqlQuery query4;
    query4.prepare(QString("SELECT COUNT (*) FROM %1").arg(playtext)); //get size of players table
    query4.exec();
    while(query4.next())
    {
        Opcodes::MaintoonTableSize = query4.value(0).toInt(nullptr);
        tablesize2 = query4.value(0).toInt(nullptr);
    }

    QSqlQuery query5;
    query5.prepare(QString("SELECT COUNT (*) FROM %1").arg(itemtext)); //get size of items table
    query5.exec();
    while(query5.next())
    {
        tablesize3 = query5.value(0).toInt(nullptr);
    }

    QSqlQuery query6;
    query6.prepare(QString("SELECT COUNT (*) FROM %1").arg(mertext)); //get size of merchant table
    query6.exec();
    while(query6.next())
    {
        tablesize4 = query6.value(0).toInt(nullptr);

    }

    QSqlQuery query7;
    query7.prepare(QString("SELECT COUNT (*) FROM %1").arg(bufftext)); //get size of buff table
    query7.exec();
    while(query7.next())
    {
        tablesize5 = query7.value(0).toInt(nullptr);

    }

    QSqlQuery query8;
    query8.prepare(QString("SELECT COUNT (*) FROM %1").arg(accounttext)); //get size of account table
    query8.exec();
    while(query8.next())
    {
        tablesize6 = query8.value(0).toInt(nullptr);

    }

    for (rotateC = 1; rotateC <= tablesizeC; rotateC++) // cause db starts on row 1
    {
        emit watcher->progressValueChanged(rotateC);

        QSqlQuery  query3 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(Coaching).arg(rotateC));

        QString CharselectID = "";
        QVector<QVector<QString>> TempVectMain;

        while(query3.next())
        {
            QSqlRecord record1 = query3.record();

            QVector<QString> ellisto;
            QVector<QString> positionlist;

            CharselectID = record1.value(2).toString();

            for (int ee = 3; ee < record1.count(); ee++) //skip1 so our first coach value is 1 in the list
            {
                ellisto.append(record1.value(ee).toString());
                positionlist.append(record1.fieldName(ee));
            }

            ellisto.insert(0,"0");
            positionlist.insert(0,"NULL");

            TempVectMain.append(ellisto);
            TempVectMain.append(positionlist);
        }

        Opcodes::ID_VectorOfCoachesVectorMap.insert(CharselectID,TempVectMain);
    }

    rotate2 = 0;


    qDebug() << "";
    qDebug() << "";
    qDebug() << "";
    QMapIterator<QString,QVector<QVector<QString>>> iter (Opcodes::ID_VectorOfCoachesVectorMap);
    while (iter.hasNext())
    {
        iter.next();
        QString PlayersID = iter.key();
        QVector<QVector<QString>> CoachingMainVector = iter.value();

        QVector<QString> ellisto = CoachingMainVector.at(0);
        QVector<QString> positionlist = CoachingMainVector.at(0);

        for(int val = 0; val < ellisto.size(); val++)
        {
            QString FieldName = ellisto.at(val);
            QString FieldValue = positionlist.at(val);
        }

    }




    QString RaceAndGender = "";
    QString Player_Name = "";
    int packetid = 0;
    QString NameStatus = "";

    for (rotate2 = 1; rotate2 <= tablesizeG; rotate2++) // cause db starts on row 1
    {
        emit watcher->progressValueChanged(rotate2);

        QSqlQuery  query3 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(RaceGen).arg(rotate2));

        while(query3.next())
        {
            QSqlRecord record2 = query3.record();

            packetid = record2.value(0).toInt();
            RaceAndGender = record2.value(1).toString();
            Player_Name = record2.value(2).toString();
            NameStatus= record2.value(3).toString();
        }

        QVector<QString> TempVect;

        if(Opcodes::RaceGen_NameVectorMap.contains(RaceAndGender))
        {
            TempVect = Opcodes::RaceGen_NameVectorMap.value(RaceAndGender);
        }

        TempVect.append(Player_Name);

        Opcodes::RaceGen_NameVectorMap.insert(RaceAndGender,TempVect);

        QString RowID = QString::number(packetid);

        TempVect.clear();

        TempVect.append(RowID);
        TempVect.append(NameStatus);


        Opcodes::Name_PacketStatusVectorMap.insert(Player_Name,TempVect);
    }

    rotate2 = 0;




        int PacketNumber = 0;
        QString Zones_Name = "";
        float Xval = 0;
        float Zval = 0;
        float Yval = 0;



        for(rotateZ = 1; rotateZ < tablesizeZ; rotateZ++)
        {
            emit watcher->progressValueChanged(rotateZ);

            QVector <QString> CurrentZoneNamesVect;
            QVector <QVector<float>> CurrentZoneCentralVectors;

            QSqlQuery query4 (QString("SELECT * FROM %1 WHERE rowid = %2").arg(ZoneXZYs).arg(rotateZ));

            while (query4.next())
            {
                QSqlRecord record = query4.record();


                PacketNumber = record.value(0).toInt();

                Zones_Name = record.value(1).toString();
                Xval = record.value(2).toFloat();
                Zval = record.value(3).toFloat();
                Yval = record.value(4).toFloat();

                if(PacketNumber >= 1 && PacketNumber <= 1386)
                {
                    CurrentZoneNamesVect = checkthezone::WorldZoneNamesVector.at(0);
                    CurrentZoneNamesVect.append(Zones_Name);
                    checkthezone::WorldZoneNamesVector.replace(0,CurrentZoneNamesVect);

                    CurrentZoneCentralVectors = checkthezone::VectorOfWorlds.at(0);

                    QVector<float> CurrentXZYVect =
                    {
                       Xval, Zval, Yval
                    };

                    CurrentZoneCentralVectors.append(CurrentXZYVect);

                    checkthezone::VectorOfWorlds.replace(0,CurrentZoneCentralVectors);
                }

                if(PacketNumber >= 1387 && PacketNumber <= 1539)
                {
                    CurrentZoneNamesVect = checkthezone::WorldZoneNamesVector.at(1);
                    CurrentZoneNamesVect.append(Zones_Name);
                    checkthezone::WorldZoneNamesVector.replace(1,CurrentZoneNamesVect);

                    CurrentZoneCentralVectors = checkthezone::VectorOfWorlds.at(1);

                    QVector<float> CurrentXZYVect =
                    {
                       Xval, Zval, Yval
                    };

                    CurrentZoneCentralVectors.append(CurrentXZYVect);

                    checkthezone::VectorOfWorlds.replace(1,CurrentZoneCentralVectors);
                }

                if(PacketNumber >= 1540 && PacketNumber <= 1899)
                {
                    CurrentZoneNamesVect = checkthezone::WorldZoneNamesVector.at(2);
                    CurrentZoneNamesVect.append(Zones_Name);
                    checkthezone::WorldZoneNamesVector.replace(2,CurrentZoneNamesVect);

                    CurrentZoneCentralVectors = checkthezone::VectorOfWorlds.at(2);

                    QVector<float> CurrentXZYVect =
                    {
                       Xval, Zval, Yval
                    };

                    CurrentZoneCentralVectors.append(CurrentXZYVect);

                    checkthezone::VectorOfWorlds.replace(2,CurrentZoneCentralVectors);
                }

                if(PacketNumber >= 1900 && PacketNumber <= 1953)
                {
                    CurrentZoneNamesVect = checkthezone::WorldZoneNamesVector.at(3);
                    CurrentZoneNamesVect.append(Zones_Name);
                    checkthezone::WorldZoneNamesVector.replace(3,CurrentZoneNamesVect);

                    CurrentZoneCentralVectors = checkthezone::VectorOfWorlds.at(3);

                    QVector<float> CurrentXZYVect =
                    {
                       Xval, Zval, Yval
                    };

                    CurrentZoneCentralVectors.append(CurrentXZYVect);

                    checkthezone::VectorOfWorlds.replace(3,CurrentZoneCentralVectors);
                }

                if(PacketNumber >= 1954 && PacketNumber <= 1971)
                {
                    CurrentZoneNamesVect = checkthezone::WorldZoneNamesVector.at(4);
                    CurrentZoneNamesVect.append(Zones_Name);
                    checkthezone::WorldZoneNamesVector.replace(4,CurrentZoneNamesVect);

                    CurrentZoneCentralVectors = checkthezone::VectorOfWorlds.at(4);

                    QVector<float> CurrentXZYVect =
                    {
                       Xval, Zval, Yval
                    };

                    CurrentZoneCentralVectors.append(CurrentXZYVect);

                    checkthezone::VectorOfWorlds.replace(4,CurrentZoneCentralVectors);
                }

                if(PacketNumber >= 1972 && PacketNumber <= 1989)
                {
                    CurrentZoneNamesVect = checkthezone::WorldZoneNamesVector.at(5);
                    CurrentZoneNamesVect.append(Zones_Name);
                    checkthezone:: WorldZoneNamesVector.replace(5,CurrentZoneNamesVect);

                    CurrentZoneCentralVectors = checkthezone::VectorOfWorlds.at(5);

                    QVector<float> CurrentXZYVect =
                    {
                        Xval, Zval, Yval
                    };

                    CurrentZoneCentralVectors.append(CurrentXZYVect);

                    checkthezone::VectorOfWorlds.replace(5,CurrentZoneCentralVectors);
                }
            }

            if(!query4.exec())
            {
                qDebug() << "lastError" << query4.lastError().text();
                return;
            }

        }

        //Maintoon

        QByteArray  Play_ID = "";

    for (rotate2 = 1; rotate2 <= tablesize2; rotate2++) // cause db starts on row 1
    {
        emit watcher->progressValueChanged(rotate2);

        QSqlQuery  query3 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(playtext).arg(rotate2));

        QString Player_Name = "";

        while(query3.next())
        {
            QSqlRecord record2 = query3.record();

            Player_Name = record2.value(1).toString();

            qDebug() << "Player_Name" << Player_Name;


            for(int j=12; j < record2.count(); j++) //was j=2
            {
                rowline2.append(record2.value(j).toString());

            }
        }

        Play_ID = rowline2.mid(2,8).toUtf8();
        qDebug() << "Players_ID" << Play_ID;
        generator::Players_Map.insert(Play_ID,rowline2.toUtf8());

        checkthezone::NPCsNames.insert(rowline2.mid(2,8),Player_Name);

        if(!Opcodes::PlayerNames.contains(Player_Name) && Player_Name.isEmpty() == false)
        {
            Opcodes::PlayerNames.append(Player_Name);
        }

        objectpacket::Master_Map.insert(Play_ID,rowline2.toUtf8());

        objectpacket::holdid1.append(Play_ID);
        objectpacket::holdx.append(rowline2.mid(12,6));
        objectpacket::holdz.append(rowline2.mid(18,6));
        objectpacket::holdy.append(rowline2.mid(24,6));

        rowline2.clear();
        QString Packet_8 = QString("%1").arg(rotate2,1,10,QLatin1Char('0')).toUpper();
        Opcodes::ID_PacketNumber_Map.insert(Play_ID,Packet_8);
    }

    rotate2 = 0;


    if(Play_ID.isEmpty() == true)
    {
        Play_ID = "01012000";
    }

    Opcodes::MaintoonsCharID = Play_ID;




    ////////////////////////////////////////////////////////////////


    QByteArray Main_ID = "";



    for (rotate = 1; rotate < tablesize; rotate++) //use the number of rows of npcs table
    {
        emit watcher->progressValueChanged(rotate);

        QSqlQuery  query4 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(newtext).arg(rotate));

        QString ZonesName = "";
        int newWorld = 0;
        int ZoneNumber = 0;


        while (query4.next())
        {


            QSqlRecord record = query4.record();
            int PacketNumber = record.value(0).toInt();
            QString NPC_Name = record.value(1).toString();
            ZoneNumber = record.value(2).toInt();
            QString ID = record.value(5).toString();
            Opcodes::StartingIDXZYF.append(ID);
            Opcodes::EndingIDXZYF.append(ID);

            for(int i=4; i < record.count(); i++) //was i=2
            {
                rowline.append(record.value(i).toString());
            }

            Opcodes::Name_PacketNumber_Map.insert(NPC_Name,record.value(0).toString());
            Opcodes::NPC_Map.insert(PacketNumber,NPC_Name);
            QString newWorld1 = record.value(12).toString();
            newWorld = newWorld1.toInt(nullptr,10);
            checkthezone::NPCsNames.insert(rowline.mid(2,8),NPC_Name);
            checkthezone::IDsAndWorlds.insert(rowline.mid(2,8),newWorld);
            QVector <QString> tempWorldZoneNamesVector = checkthezone::WorldZoneNamesVector.at(newWorld);
            ZonesName = tempWorldZoneNamesVector.at(ZoneNumber);

            Quests::NPCsName_ZoneMap.insert(NPC_Name,ZonesName);

//            qDebug() << "";
//                qDebug() << "PacketNumber" << PacketNumber;
//                qDebug() << "NPC_Name" << NPC_Name;
//                qDebug() << "NPCsID" << ID;
//                qDebug() << "World" << newWorld1;
//                qDebug() << "ZoneNumber" << ZoneNumber;
//                qDebug() << "ZonesName" << ZonesName;
//                qDebug() << "";

//                if(PacketNumber == 17666)
//                {
//                    qDebug() << "tempWorldZoneNamesVector" << tempWorldZoneNamesVector;
//                }

//                qDebug() << "";

        }

        QMap <QString,QVector <QString>> ZoneLocationsIDsVectorMap = checkthezone::VectorOFWorldsZones.value(newWorld);
        QVector <QString> LocationsIDsVector;

        if(ZoneLocationsIDsVectorMap.isEmpty() == false)
        {
            LocationsIDsVector = ZoneLocationsIDsVectorMap.value(ZonesName);
        }

        LocationsIDsVector.append(rowline.mid(2,8));
        checkthezone::IDandZoneNumber.insert(rowline.mid(2,8),ZoneNumber);
        ZoneLocationsIDsVectorMap.insert(ZonesName,LocationsIDsVector);
        checkthezone::VectorOFWorldsZones.replace(newWorld,ZoneLocationsIDsVectorMap);
        QString Packet_8 = QString("%1").arg(rotate,1,10,QLatin1Char('0')).toUpper();
        Main_ID = rowline.mid(2,8).toUtf8();
        Opcodes::ID_PacketNumber_Map.insert(Main_ID,Packet_8);

        objectpacket::Master_Map.insert(Main_ID,rowline.toUtf8());

        objectpacket::holdid1.append(Main_ID);
        objectpacket::holdx.append(rowline.mid(12,6));
        objectpacket::holdz.append(rowline.mid(18,6));
        objectpacket::holdy.append(rowline.mid(24,6));


        rowline.clear();

    }


    increment Respawn;
    QByteArray Respawn2 = Respawn.count(Main_ID);
    Opcodes::RespawnID = Respawn2.toUpper();



    rotate = 0;

    //////////////////////////////////////////////////////////

    for(int i = 0; i < checkthezone::VectorOFWorldsZones.size(); i++)
    {
        rotate++;

        emit watcher->progressValueChanged(rotate);

        QMap <QString,QVector <QString>> ZoneLocationsIDsVectorMap = checkthezone::VectorOFWorldsZones.value(i);
        QVector <QString> tempWorldZoneNamesVector = checkthezone::WorldZoneNamesVector.at(i);

        QString World = WorldNameVector.at(i);

        QVector <QVector<float>> VectorOfWorlds1 = checkthezone::VectorOfWorlds.at(i);


        for(int u = 0; u < tempWorldZoneNamesVector.size(); u++)
        {
            rotate++;

            emit watcher->progressValueChanged(rotate);

            QString ZonesName = tempWorldZoneNamesVector.at(u);

            QVector <QStringList> TempHoldsVector;
            QStringList holdid1;
            QStringList holdx;
            QStringList holdz;
            QStringList holdy;

            QVector <QString> LocationsIDsVector = ZoneLocationsIDsVectorMap.value(ZonesName);


            for(int m = 0; m < LocationsIDsVector.size(); m++)
            {
                rotate++;

                emit watcher->progressValueChanged(rotate);

                QString NPCsID = LocationsIDsVector.at(m);

                int index = objectpacket::holdid1.indexOf(NPCsID);

                QString Xvzl = objectpacket::holdx.at(index);
                QString Zvzl = objectpacket::holdz.at(index);
                QString Yvzl = objectpacket::holdy.at(index);

                holdid1.append(NPCsID);
                holdx.append(Xvzl);
                holdz.append(Zvzl);
                holdy.append(Yvzl);

                QString NPCsName = checkthezone::NPCsNames.value(NPCsID);

            }

            TempHoldsVector.append(holdid1);
            TempHoldsVector.append(holdx);
            TempHoldsVector.append(holdz);
            TempHoldsVector.append(holdy);

            checkthezone::Zones_IDXZYvectorMap.insert(ZonesName,TempHoldsVector);

        }

        objectpacket::MasterMap2 = objectpacket::Master_Map;


    }

    rotate = 0;




    ////////////////////////////////////////////////////////

    for (rotate3 = 1; rotate3 <= tablesize3; rotate3++) //equal cause db starts on row 1
    {

        emit watcher->progressValueChanged(rotate3);

        QVector <QString> ItemFields;
        QSqlQuery  query5 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(itemtext).arg(rotate3));

        QString ItemsName = "";
        QByteArray  Items_ID = "";

        while(query5.next())
        {
            QSqlRecord record5 = query5.record();

            for(int k=6; k < record5.count(); k++) //read 6 past now
            {

                QString FieldValue = record5.value(k).toString();
                if (FieldValue.isEmpty() == false)// this is to remove the empty stats at the end
                {
                    rowline3.append(record5.value(k).toString());
                    ItemFields.append(record5.value(k).toString());
                }

            }
            ItemFields.prepend(record5.value(5).toString()); //Armor Type
            ItemFields.prepend(record5.value(4).toString()); //Gear Model
            ItemFields.prepend(record5.value(2).toString()); //Gear Color
            ItemsName = record5.value(1).toString();

            Items_ID = record5.value(12).toByteArray();
        }

        merchants::Items_Map.insert(Items_ID,rowline3.toUtf8()); //master item map here


        merchants::MasterItems_Vector_Map.insert(Items_ID,ItemFields);




        rowline3.clear();

        //qDebug() << "ItemsName" << ItemsName << "ItemsID" << Items_ID;

        Opcodes::ItemsID_ItemsName_Map.insert(Items_ID,ItemsName);
    }

    rotate3 = 0;


    //////////////////////////////////////////////////////////////////////////

    for (rotate4 = 1; rotate4 <= tablesize4; rotate4++) //1 cause db starts on row 1
    {

        emit watcher->progressValueChanged(rotate4);

        QVector<QString> FieldValues;

        QSqlQuery query6 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(mertext).arg(rotate4));

        QString NPCsID = "";
        QString Greeting = "";

        while(query6.next())
        {
            QSqlRecord record6 = query6.record();

            for(int k = 6; k < record6.count(); k+=2) //read 6 past, skip 1
            {
                QString FieldValue = record6.value(k).toString();

                if (FieldValue.isEmpty() == false)
                {
                    FieldValues.append(record6.value(k).toByteArray());
                }
                else
                {
                    break;
                }
            }

            NPCsID = (record6.value(2).toString()); //object id
            merchants changetext;
            QString returned_text = record6.value(4).toString();
            Greeting = changetext.Text_to_Unicode(returned_text); //this is so we can type normal text into the db
            merchants::Merchant_Text_Map.insert(NPCsID,Greeting);
            int PacketID = record6.value(0).toInt();
            merchants::MerchantIDs_PacketIDsMap.insert(NPCsID,PacketID);
        }

        merchants::Merchant_List_Map.insert(NPCsID,FieldValues);
    }

    rotate4 = 0;

    //--------------------------------------------------------------------------------------------

    QString Table = "LOOT_TABLES";
    QSqlQuery queryLoot;
    queryLoot.prepare(QString("SELECT COUNT (*) FROM %1").arg(Table)); //get size of merchant table
    queryLoot.exec();
    while(queryLoot.next())
    {
        tablesize4 = queryLoot.value(0).toInt(nullptr);

    }

    for (rotate4 = 1; rotate4 <= tablesize4; rotate4++) //1 cause db starts on row 1
    {

        emit watcher->progressValueChanged(rotate4);

        QVector<QString> FieldValues;

        QSqlQuery query6 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(Table).arg(rotate4));

        QString NPCsID = "";

        while(query6.next())
        {
            QSqlRecord record6 = query6.record();

            for(int k = 5; k < record6.count(); k+=2) //read 6 past, skip 1
            {
                QString FieldValue = record6.value(k).toString();

                if (FieldValue.isEmpty() == false)
                {
                    FieldValues.append(record6.value(k).toByteArray());
                }
                else
                {
                    break;
                }
            }

            NPCsID = (record6.value(2).toString()); //object id
            int PacketID = record6.value(0).toInt();
            Loot::LootIDs_PacketIDsMap.insert(NPCsID,PacketID);
        }

        Loot::Loot_List_Map.insert(NPCsID,FieldValues);
    }

    rotate4 = 0;

    //--------------------------------------------------------------------------------------------

    ///////////////////////////////////////////////////////////////////////// not added yet effects.cpp

    //    QString bufftext  = "BUFFS";
    //    QVector <QByteArray> position;
    //    for (rotate5 = 1; rotate5 <= tablesize5; rotate5++) // cause db starts on row 1
    //    {
    //    QSqlQuery  query8 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(bufftext).arg(rotate5));

    //    while(query8.next())
    //    {
    //    QSqlRecord record8 = query8.record();

    //    for(int j=3; j < record8.count(); j++)
    //    {
    //    rowline5 = record8.value(2).toByteArray();
    //    position.append(record8.value(j).toByteArray());
    //    }
    //    }

    //    effects::CastingID_BuffVectorMap.insert(rowline5,position);
    //    position.clear();
    //    //   qDebug() << "mainbuffmap" <<effects::CastingID_BuffVectorMap;

    //    }

    //    rotate5 = 0;




    //////////////////////////////////////////////////////////////////////////////

    QSqlQuery queryCl ("SELECT * FROM CLASSVALUES");

    rotate = 0;

    while(queryCl.next())
    {
        rotate++;

        emit watcher->progressValueChanged(rotate);

        QString ClassValue = queryCl.value(1).toString();
        QString templist = queryCl.value(2).toString();
        QVector <QString> temp;
        QStringList mylist = templist.split(",");
        QString firststring = "";

        for(int i = 0; i < mylist.size(); i++)
        {
            firststring = mylist.value(i);
            temp.append(firststring);
        }
        objectpacket::MonsterClassVectorMap.insert(ClassValue,temp);
        temp.clear();
    }

    ///////////////////////////////////////////////////////////////////////

    QSqlQuery queryR ("SELECT * FROM RACEVALUES");

    rotate = 0;

    while(queryR.next())
    {
        rotate++;

        emit watcher->progressValueChanged(rotate);

        QString RaceValue = queryR.value(1).toString();
        QString templist = queryR.value(2).toString();
        QVector <QString> temp;
        QStringList mylist = templist.split(",");
        QString firststring = "";

        for(int i = 0; i < mylist.size(); i++)
        {
            firststring = mylist.value(i);
            temp.append(firststring);
        }
        objectpacket::MonsterRaceVectorMap.insert(RaceValue,temp);
        temp.clear();
    }

    ///////////////////////////////////////////////////////////////////////


    rotate = 0;


    QSqlQuery query0;

    // get all usernames and passwords
    for(rotate6 = 1; rotate6 <=  tablesize6; rotate6++) //equal cause db starts on row 1
    {

        emit watcher->progressValueChanged(rotate6);

        QString Packet = QString("%1").arg(rotate6,1,10,QLatin1Char('0'));
        query0.exec(QString("SELECT * FROM ACCOUNTS WHERE ROWID = '"+ Packet +"'"));

        QSqlRecord record;


        QVector <QString> ServerCharIDsVector;


        while(query0.next())
        {
            record = query0.record();

            Opcodes::UserName_PasswordMap.insert(query0.value(1).toString(),query0.value(2).toString());

            Opcodes::UserName_PacketID.insert(query0.value(1).toString(),query0.value(0).toInt());

            int count = 0;

            for (int CSS = 3; CSS < record.count(); CSS++)
            {
                if(Opcodes::ServerFieldsVector.size() < 56)
                {
                    Opcodes::ServerFieldsVector.append(record.fieldName(CSS));
                }

                QString CharID1 = query0.value(CSS).toString();

                if(CharID1.size() == 8)
                {
                    count++;
                    ServerCharIDsVector.append(CharID1);
                }
                else
                {
                    CharID1 = "";
                    ServerCharIDsVector.append(CharID1);
                }
            }

            Opcodes::UserName_CharSlots.insert(query0.value(1).toString(),ServerCharIDsVector);

            qDebug() << "";
            qDebug() << "UserName" << query0.value(1).toString();
            qDebug() << "Opcodes::UserName_CharSlots.size()" << Opcodes::UserName_CharSlots.size();
            qDebug() << "ServerCharIDsVector.size()" << ServerCharIDsVector.size();
            qDebug() << "amount of characters" << count;
            qDebug() << "Opcodes::ServerFieldsVector.size()" << Opcodes::ServerFieldsVector.size();
            qDebug() << "";


        }
    }
    /////////////////////////////////////////////////////

    QSqlQuery query99;
    query99.prepare(QString("SELECT COUNT (*) FROM PATHS")); //get size of paths table
    query99.exec();

    while(query99.next())
    {
        tablesize7 = query99.value(0).toInt(nullptr);
    }





    for(rotate7 = 1; rotate7 <= tablesize7; rotate7++) //1 cause db starts on row 1
    {
        emit watcher->progressValueChanged(rotate7);

        QString RoamingID = "";
        QString NextMarker = "";
        QSqlQuery Roaming (QString("SELECT * FROM PATHS WHERE rowid = %1").arg(rotate7));

        while(Roaming.next())
        {
            QSqlRecord record6 = Roaming.record();
            int NextStep = 0;
            QMap <int,QString> TempMap;

            for(int rr = 4; rr < record6.count(); rr++)
            {
                QString FieldValue = record6.value(rr).toString();

                if(FieldValue.isEmpty() == false)
                {
                    NextStep++;
                    NextMarker = record6.value(rr).toString();
                    TempMap.insert(NextStep,NextMarker);
                }
            }

            RoamingID = record6.value(2).toString();
            QString RandomOrNot = record6.value(3).toString();

            if(RandomOrNot == "01")
            {
                FacingDirection::RandomIDsMAP.insert(RoamingID,TempMap.size());
            }

            FacingDirection::Path_Maps.insert(RoamingID,TempMap);
            objectpacket::Roamers_List.insert(RoamingID,"Roaming");
            FacingDirection::Path_CounterMap.insert(RoamingID,1);
            FacingDirection::Facing_Lock2_Map.insert(RoamingID,"00");
        }
    }

    qDebug() << "";
    qDebug() << "";
    qDebug() << "objectpacket::Master_Map.size() = " << objectpacket::Master_Map.size();
    qDebug() << "objectpacket::holdid1.size() " << objectpacket::holdid1.size();
    qDebug() << "objectpacket::holdx.size() " << objectpacket::holdx.size();
    qDebug() << "objectpacket::holdz.size() " << objectpacket::holdz.size();
    qDebug() << "objectpacket::holdy.size() " << objectpacket::holdy.size();

    qDebug() << "";
    qDebug() << "";

    rotate = 0;


    maploading::MapLoadingFinished = true;

    emit watcher->progressValueChanged(99);

    db.commit();

    Mapfinished();

}



QVector<QByteArray> maploading::Mapfinished()
{
    QVector <QByteArray> finished;
    finished.append("Maploading Finished...");

    return finished;
}

