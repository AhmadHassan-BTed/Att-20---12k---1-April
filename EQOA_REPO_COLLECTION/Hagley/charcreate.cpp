#include "charcreate.h"
#include "add_or_sub.h"
#include "convert.h"
#include "generator.h"
#include "combat.h"
#include "packetparsing.h"
#include "login.h"
#include "opcodes.h"
#include "checkthezone.h"
#include "objectpacket.h"

CharCreate::CharCreate(QObject *parent) : QObject(parent)
{

}

QByteArray CharCreate::CharDelete(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString DeleteSelectID)
{
    packetparsing::packetvars IN_CharDelete;
    IN_CharDelete = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    if(db.isOpen())
    {
        db.close();
        db.removeDatabase(QSqlDatabase::defaultConnection);
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(Opcodes::m_FileName);
    db.open();


    QVector <QString> tempCharIDsVector = Opcodes::UserName_CharSlots.value(IN_CharDelete.UserName);

    QVector <QString> ServerCharIDsVector;
    QVector <QString> RestructureIDsVector;

    QString ColumnName = ColumnServerIDMap.value(IN_CharDelete.serverid1);

    Convert Sendto;

    qDebug() << "";
    qDebug() << "inside Delete character";
    qDebug() << "IN_CharDelete.serverid1" << IN_CharDelete.serverid1;
    qDebug() << "ColumnName" << ColumnName;

    int index = Opcodes::ServerFieldsVector.indexOf(ColumnName + "_Slot1");

    int indexHold = index;

    qDebug() << "index" << index;

    for (int CSS = index; CSS < index + 8; CSS++)
    {
        QString CharID1 = tempCharIDsVector.at(CSS);
        qDebug() << "CharIDx = " << CharID1;

        if(CharID1.isEmpty() == false)
        {
            ServerCharIDsVector.append(CharID1);
        }

    }

    QString HexVal = DeleteSelectID.toUpper();
    ADD_OR_SUB SendtoDec;
    QString ConvertedValue = SendtoDec.BackToDec(HexVal);

    int FourByte1 = ConvertedValue.toInt(nullptr,10);
    QString FourByte2 = QString("%1").arg(FourByte1,8,16,QLatin1Char('0'));
    QString Byte1 = FourByte2.mid(0,2);
    QString Byte2 = FourByte2.mid(2,2);
    QString Byte3 = FourByte2.mid(4,2);
    QString Byte4 = FourByte2.mid(6,2);
    QString Reverse = Byte4 + Byte3 + Byte2 + Byte1;

    DeleteSelectID = Reverse.toUpper();

    QByteArray current_C9 = generator::Players_Map.value(DeleteSelectID.toUtf8()).toUpper();

    qDebug() << "current_C9.size()" << current_C9.size();

    QByteArray Name24Hex = current_C9.mid(310,48);

    QString textout = Name24Hex.fromHex(Name24Hex);

    int indexX = Opcodes::PlayerNames.indexOf(textout);

    if(indexX != -1)
    {
        Opcodes::PlayerNames.append(textout);
    }

    int Names_PacketID = 0;

    if(Opcodes::Name_PacketStatusVectorMap.contains(textout))
    {
        QVector<QString> StatusVect = Opcodes::Name_PacketStatusVectorMap.value(textout);

        Names_PacketID = StatusVect.at(0).toInt();
        StatusVect.replace(1,"NO");

        Opcodes::Name_PacketStatusVectorMap.insert(textout,StatusVect);

        db.transaction();

        QSqlQuery query;

        QString Table_Name = "NAME_GENERATOR";
        QString Column_Name = "Status";
        query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
        query.bindValue(":val","NO");
        query.bindValue(":id",Names_PacketID);
        query.exec();

        if(!query.exec())
        {
            qDebug() << "NAME_GENERATOR Fail = " << Column_Name.toUtf8() + " NO";
        }

        db.commit();
    }

    QVector<QByteArray> CharacterBytes = Opcodes::UserName_CharCountVectorMap.value(IN_CharDelete.UserName);
    QVector<QString> tempCharVect = Opcodes::UserName_CharIDs.value(IN_CharDelete.UserName);

    index = tempCharVect.indexOf(DeleteSelectID);

    if(index != -1)
    {
        tempCharVect.removeAt(index);
        CharacterBytes.removeAt(index);
        Opcodes::UserName_CharIDs.insert(IN_CharDelete.UserName,tempCharVect);
        Opcodes::UserName_CharCountVectorMap.insert(IN_CharDelete.UserName,CharacterBytes);
    }


    index = ServerCharIDsVector.indexOf(DeleteSelectID);

    if(index != -1)
    {
        ServerCharIDsVector.remove(index);
        ServerCharIDsVector.append("");
    }

    for(int RE = 0; RE < ServerCharIDsVector.size(); RE++)//remove the nulls and emptys
    {
        QString CharID1 = ServerCharIDsVector.at(RE);

        if(CharID1.size() == 8)
        {
            RestructureIDsVector.append(CharID1);
        }
        else
        {
            RestructureIDsVector.append(nullptr);//Vector needs to be size of 8
        }
    }

    ServerCharIDsVector = RestructureIDsVector;

    int packetid1 = Opcodes::UserName_PacketID.value(IN_CharDelete.UserName);

    QVector<QString> temV;

    index = tempCharIDsVector.indexOf(DeleteSelectID);

    if(index != -1)
    {
        tempCharIDsVector.replace(index,nullptr);
    }

    for (int CSS = indexHold; CSS < indexHold + 8; CSS++)
    {
        QString CharID1 = tempCharIDsVector.at(CSS);
        qDebug() << "CharIDx4 = " << CharID1;

        if(CharID1.isEmpty() == false)
        {
            temV.append(CharID1);
        }
    }

    do
    {
        temV.append(nullptr);
    }
    while(temV.size() < 8);

    qDebug() << "temV.size() = " << temV.size();

    int count = 0;

    for (int CSS = indexHold; CSS < indexHold + 8; CSS++)
    {
        QString CharID1 = temV.at(count);
        count++;

        qDebug() << "CharIDx5 = " << CharID1;

        tempCharIDsVector.replace(CSS,CharID1);
    }

    Opcodes::UserName_CharSlots.insert(IN_CharDelete.UserName,tempCharIDsVector);


    db.transaction();

    for (int rt = 0; rt < ServerCharIDsVector.count(); rt++)//this way the db will reflect the same order as the char select screen
    {
        QString CharID0 = ServerCharIDsVector.at(rt);
        index = tempCharIDsVector.indexOf(CharID0);

        QString CharID1 = tempCharIDsVector.at(index);

        Table_Name = "ACCOUNTS";
        QString place1 = Opcodes::ServerFieldsVector.at(index);
        Column_Name = place1;

        QSqlQuery query;
        query.prepare(QString("UPDATE [%1] SET [%2] = :val1 WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
        query.bindValue(":val1",CharID1);
        query.bindValue(":id",packetid1);
        query.exec();

        if(!query.exec())
        {
            return "Fail_ACCOUNTSX = " + CharID1.toUtf8();
        }
    }

    db.commit();

    db.close();
    db.removeDatabase(QSqlDatabase::defaultConnection);

    return "Create Packet";
}

QByteArray CharCreate::PullCharacter(QString CharSelectID)
{
    QByteArray PacketOut = "";

    if(generator::Players_Map.contains(CharSelectID.toUtf8().toUpper()) && CharSelectID.isEmpty() == false)
    {
        qDebug() << "it is in player map";

        QByteArray current_C9 = generator::Players_Map.value(CharSelectID.toUtf8()).toUpper();

        qDebug() << "current_C9.size()" << current_C9.size();

        QByteArray Name24Hex = current_C9.mid(310,48);

        QString textout = Name24Hex.fromHex(Name24Hex);
        QByteArray NameHex = textout.toUtf8().toHex();

        int NameHex1 = NameHex.size();
        int NameHex2 = NameHex1 / 2;
        QString NameHex3 = QString("%1").arg(NameHex2,2,16,QLatin1Char('0'));

        PacketOut.append(NameHex3.toUtf8() + "000000");//Name size 4Bytes
        PacketOut.append(NameHex);//NameHex

        // //////////Change 4Byte CharID to 5Bytes////////////////////
        QByteArray CharID = current_C9.mid(2,8);


        Convert Sendto;
        QString CharID5Byte = Sendto.Bytes4to5(CharID);
        PacketOut.append(CharID5Byte.toUtf8());
        // //////////Change 4Byte CharID to 5Bytes////////////////////

        // //////////Change 4Byte Model to 5Bytes////////////////////
        QString ModelID = current_C9.mid(56,8);
        QString Model5Byte = Sendto.Bytes4to5(ModelID);
        PacketOut.append(Model5Byte.toUtf8());
        // //////////Change 4Byte Model to 5Bytes////////////////////

        QString Class = current_C9.mid(368,2);
        int Class1 = Class.toInt(nullptr,16);
        Class1 = Class1 * 2;
        QString Class2 = QString("%1").arg(Class1,2,16,QLatin1Char('0'));
        PacketOut.append(Class2.toUtf8());

        QString Race = current_C9.mid(300,2);
        int Race1 = Race.toInt(nullptr,16);
        Race1 = Race1 * 2;
        QString Race2 = QString("%1").arg(Race1,2,16,QLatin1Char('0'));
        PacketOut.append(Race2.toUtf8());

        QString Level = current_C9.mid(358,2);
        int Level1 = Level.toInt(nullptr,16);
        Level1 = Level1 * 2;
        QString Level2 = QString("%1").arg(Level1,2,16,QLatin1Char('0'));
        PacketOut.append(Level2.toUtf8());

        QString HairColor = current_C9.mid(290,2);
        int HairColor1 = HairColor.toInt(nullptr,16);
        HairColor1 = HairColor1 * 2;
        QString HairColor2 = QString("%1").arg(HairColor1,2,16,QLatin1Char('0'));
        PacketOut.append(HairColor2.toUtf8());

        QString HairLength = current_C9.mid(292,2);
        int HairLength1 = HairLength.toInt(nullptr,16);
        HairLength1 = HairLength1 * 2;
        QString HairLength2 = QString("%1").arg(HairLength1,2,16,QLatin1Char('0'));
        PacketOut.append(HairLength2.toUtf8());

        QString HairStyle = current_C9.mid(294,2);
        int HairStyle1 = HairStyle.toInt(nullptr,16);
        HairStyle1 = HairStyle1 * 2;
        QString HairStyle2 = QString("%1").arg(HairStyle1,2,16,QLatin1Char('0'));
        PacketOut.append(HairStyle2.toUtf8());

        QString Face = current_C9.mid(296,2);
        int Face1 = Face.toInt(nullptr,16);
        Face1 = Face1 * 2;
        QString Face2 = QString("%1").arg(Face1,2,16,QLatin1Char('0'));
        PacketOut.append(Face2.toUtf8());

        QByteArray RobeType = current_C9.mid(298,2);
        PacketOut.append(RobeType);

        if(RobeType == "FF")
        {
            PacketOut.append("FFFFFF");
        }
        else
        {
            PacketOut.append("000000");
        }

        QByteArray Primary = current_C9.mid(134,8);
        PacketOut.append(Primary);

        QByteArray Secondary = current_C9.mid(142,8);
        PacketOut.append(Secondary);

        QByteArray Shield = current_C9.mid(150,8);
        PacketOut.append(Shield);

        QByteArray ATTGraphic = current_C9.mid(182,4);
        PacketOut.append(ATTGraphic + "00");// + UNK 00

        QByteArray Torso = current_C9.mid(186,2);
        PacketOut.append(Torso);

        QByteArray Forearms = current_C9.mid(188,2);
        PacketOut.append(Forearms);

        QByteArray Gloves = current_C9.mid(190,2);
        PacketOut.append(Gloves);

        QByteArray Legs = current_C9.mid(192,2);
        PacketOut.append(Legs);

        QByteArray Feet = current_C9.mid(194,2);
        PacketOut.append(Feet);

        QByteArray Head = current_C9.mid(196,2);
        PacketOut.append(Head);

        PacketOut.append("000000000000FFFFFFFFFFFFFFFFFFFFFFFF");

        QByteArray TorsoC = current_C9.mid(222,8);
        PacketOut.append(TorsoC);

        QByteArray ForearmsC = current_C9.mid(232,8);
        PacketOut.append(ForearmsC);

        QByteArray GlovesC = current_C9.mid(242,8);
        PacketOut.append(GlovesC);

        QByteArray LegsC = current_C9.mid(252,8);
        PacketOut.append(LegsC);

        QByteArray FeetC = current_C9.mid(262,8);
        PacketOut.append(FeetC);

        QByteArray HeadC = current_C9.mid(272,8);
        PacketOut.append(HeadC);

        QByteArray RobeC = current_C9.mid(282,8);
        PacketOut.append(RobeC);



        return PacketOut;
    }
    else
    {
        qDebug() << "";

        qDebug() << "Must not be in the map";
        QMapIterator<QByteArray,QByteArray> iter (generator::Players_Map);
        while (iter.hasNext())
        {
            iter.next();
            QByteArray PlayersID = iter.key();
            QByteArray PlayersC9 = iter.value();

            qDebug() << "";

            qDebug() << "generator::Players_Map.size() = " << generator::Players_Map.size();

            if(CharSelectID == PlayersID)
            {
                qDebug() << "CharID2 = " << CharSelectID;
                qDebug() << "PlayersID = " << PlayersID;
                qDebug() << "PlayersC9 = " << PlayersC9;
            }
            else
            {
                qDebug() << "CharID2 = " << CharSelectID;
                qDebug() << "PlayersID = " << PlayersID;
                qDebug() << "IDs dont match";
            }

            qDebug() << "";
        }
    }

    return PacketOut;
};

QByteArray CharCreate::CharSelectScreen(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort)
{
    packetparsing::packetvars IN_CharSelectScreen;
    IN_CharSelectScreen = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    PacketOut.clear();


    qDebug() << "";
    qDebug() << "Inside Character Selection";
    qDebug() << "ClientID" << ClientID;
    qDebug() << "ClientsIP" << ClientsIP;
    qDebug() << "ClientsPort" << ClientsPort;
    qDebug() << "IN_CharSelectScreen.UserName" << IN_CharSelectScreen.UserName;

    QVector <QString> tempCharIDsVector = Opcodes::UserName_CharSlots.value(IN_CharSelectScreen.UserName);

    qDebug() << "tempCharIDsVector.size()" << tempCharIDsVector.size();
    qDebug() << "tempCharIDsVector" << tempCharIDsVector;

    QVector <QString> ServerCharIDsVector;

    QString ColumnName = ColumnServerIDMap.value(IN_CharSelectScreen.serverid1);

    qDebug() << "IN_CharSelectScreen.serverid1" << IN_CharSelectScreen.serverid1;
    qDebug() << "ColumnName" << ColumnName;

    int index = Opcodes::ServerFieldsVector.indexOf(ColumnName + "_Slot1");

    qDebug() << "index" << index;

    for (int CSS = index; CSS < index + 8; CSS++)
    {
        QString CharID1 = tempCharIDsVector.at(CSS);
        qDebug() << "CharID2 = " << CharID1;

        if(CharID1.isEmpty() == false)
        {
            ServerCharIDsVector.append(CharID1);
        }
    }

    qDebug() << "ServerCharIDsVector.size() = " << ServerCharIDsVector.size();

    int CharCount = ServerCharIDsVector.size() * 2;

    qDebug() << "CharCount = " << CharCount;
    QString CharCount1 = QString("%1").arg(CharCount,2,16,QLatin1Char('0'));
    qDebug() << "CharCount1 = " << CharCount1;
    qDebug() << "";


    QVector<QByteArray> tempCharVect = Opcodes::UserName_CharCountVectorMap.value(IN_CharSelectScreen.UserName);


    QVector<QByteArray> PacketOut_CharVect = tempCharVect;

    PacketOut.append("2C00" + CharCount1.toUtf8());// add opcode and toon count

    for(int CP = 0; CP < ServerCharIDsVector.size(); CP++)
    {
        QString CharID2 = ServerCharIDsVector.at(CP);

        qDebug() << "CP = " << CP;

        qDebug() << "CharID2 = " << CharID2;

        CharCreate SendTo;
        QByteArray CharPacketBack = SendTo.PullCharacter(CharID2);

        if(CharPacketBack.isEmpty() == false)
        {
            int index = PacketOut_CharVect.indexOf(CharPacketBack);

            if(index == -1)
            {
                PacketOut_CharVect.append(CharPacketBack);
            }

            if(!PacketOut.contains(CharPacketBack))
            {
                PacketOut.append(CharPacketBack);
            }

            qDebug() << "PacketOut = " << PacketOut;
            qDebug() << "";

            if(PacketOut_CharVect.size() <= tempCharVect.size())
            {
                Opcodes::UserName_CharCountVectorMap.insert(IN_CharSelectScreen.UserName,tempCharVect);
            }
            else
            {
                Opcodes::UserName_CharCountVectorMap.insert(IN_CharSelectScreen.UserName,PacketOut_CharVect);
            }
        }
    }




    return PacketOut;
};

QByteArray CharCreate::CharacterCreation(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort)
{
    packetparsing::packetvars IN_CharacterCreation;
    IN_CharacterCreation = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    rowline = "";

    if(!db.isOpen())
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(Opcodes::m_FileName);
        db.open();
    }

    qDebug() << "";
    qDebug() << "inside_Character_Creation";
    qDebug() << "IN_CharacterCreation.CharSelectID_3 = " << IN_CharacterCreation.CharSelectID;

    QByteArray NameHex = IN_CharacterCreation.Name.toUtf8();
    QString textout = NameHex.fromHex(NameHex);




    // //////////////////////Rotate CharID///////////////////////////////
    QString FourByte = IN_CharacterCreation.CharSelectID.toUpper();
    QString Byte1 = FourByte.mid(0,2);
    QString Byte2 = FourByte.mid(2,2);
    QString Byte3 = FourByte.mid(4,2);
    QString Byte4 = FourByte.mid(6,2);
    QString Reverse = Byte4 + Byte3 + Byte2 + Byte1;

    int FourByte1 = Reverse.toInt(nullptr,16);
    int plus1 = FourByte1 + 1;

    QString FourByte2 = QString("%1").arg(plus1,8,16,QLatin1Char('0'));
    Byte1 = FourByte2.mid(0,2);
    Byte2 = FourByte2.mid(2,2);
    Byte3 = FourByte2.mid(4,2);
    Byte4 = FourByte2.mid(6,2);
    Reverse = Byte4 + Byte3 + Byte2 + Byte1;

    IN_CharacterCreation.CharSelectID = Reverse.toUpper();

    Opcodes::MaintoonsCharID = IN_CharacterCreation.CharSelectID.toUtf8().toUpper();

    qDebug() << "IN_CharacterCreation.CharSelectID_4 = " << IN_CharacterCreation.CharSelectID;
    // //////////////////////Rotate CharID///////////////////////////////

    // /////////////Insert and Update Coaches and Factions///////////////
    Combat AllValues;
    rowline.clear();





    int Names_PacketID = 0;

    if(Opcodes::Name_PacketStatusVectorMap.contains(textout))
    {
        QVector<QString> StatusVect = Opcodes::Name_PacketStatusVectorMap.value(textout);

        Names_PacketID = StatusVect.at(0).toInt();
        StatusVect.replace(1,"YES");

        Opcodes::Name_PacketStatusVectorMap.insert(textout,StatusVect);

        db.transaction();

        QSqlQuery query;

        QString Table_Name = "NAME_GENERATOR";
        QString Column_Name = "Status";
        query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
        query.bindValue(":val","YES");
        query.bindValue(":id",Names_PacketID);
        query.exec();

        if(!query.exec())
        {
            qDebug() << "NAME_GENERATOR Fail = " << Column_Name.toUtf8() + " YES";
        }

        db.commit();
    }

    qDebug() << "IN_CharacterCreation.Name = " << IN_CharacterCreation.Name;
    qDebug() << "textout = " << textout;
    qDebug() << "IN_CharacterCreation.packetid_2 = " << IN_CharacterCreation.packetid;


    // /////////////Insert and Update ACCOUNTS////////////////


    db.transaction();

    Table_Name = "ACCOUNTS";
    QString CurrentUser = IN_CharacterCreation.UserName;

    qDebug() << "CurrentUser" << IN_CharacterCreation.UserName;


    QSqlQuery query;
    QSqlRecord record;

    QVector <QString> tempCharIDsVector = Opcodes::UserName_CharSlots.value(IN_CharacterCreation.UserName);

    QVector <QString> ServerCharIDsVector;

    QString ColumnName = ColumnServerIDMap.value(IN_CharacterCreation.serverid1);

    qDebug() << "IN_CharacterCreation.serverid1" << IN_CharacterCreation.serverid1;
    qDebug() << "ColumnName" << ColumnName;

    int index = Opcodes::ServerFieldsVector.indexOf(ColumnName + "_Slot1");

    qDebug() << "index" << index;

    for (int CSS = index; CSS < index + 8; CSS++)
    {
        QString CharID1 = tempCharIDsVector.at(CSS);
        qDebug() << "CharIDX2 = " << CharID1;

        if(CharID1.isEmpty() == false)
        {
            ServerCharIDsVector.append(CharID1);
        }

        if(!tempCharIDsVector.contains(IN_CharacterCreation.CharSelectID) && CharID1.isEmpty() == true)
        {
             qDebug() << "tempCharIDsVector added id  = " << IN_CharacterCreation.CharSelectID;
            tempCharIDsVector.replace(CSS,IN_CharacterCreation.CharSelectID);
        }
    }

    Opcodes::UserName_CharSlots.insert(IN_CharacterCreation.UserName,tempCharIDsVector);

    ServerCharIDsVector.append(IN_CharacterCreation.CharSelectID);

    QVector<QString> tempCharidVect = Opcodes::UserName_CharIDs.value(IN_CharacterCreation.UserName);

    int index1 = tempCharidVect.indexOf(IN_CharacterCreation.CharSelectID);

    if(index1 == -1)
    {
        tempCharidVect.append(IN_CharacterCreation.CharSelectID);
        Opcodes::UserName_CharIDs.insert(IN_CharacterCreation.UserName,tempCharidVect);
    }

    for(int amnt = ServerCharIDsVector.size(); amnt < 8; amnt++)
    {
        ServerCharIDsVector.append(nullptr);
    }

    int packetid1 = Opcodes::UserName_PacketID.value(IN_CharacterCreation.UserName);

    for (int rt = index; rt < index + 8; rt++)
    {
        QString place1 = Opcodes::ServerFieldsVector.value(rt);
        QString NextID = ServerCharIDsVector.at(rt-index);

        qDebug() << "place1 = " << place1;
        qDebug() << "NextID = " << NextID;
        qDebug() << "CurrentUser = " << CurrentUser;

        Table_Name = "ACCOUNTS";
        Column_Name = place1;

        query.prepare(QString("UPDATE [%1] SET [%2] = :val1 WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
        query.bindValue(":val1",NextID);
        query.bindValue(":id",packetid1);
        query.exec();

        if(!query.exec())
        {
            return "Fail_ACCOUNTSX = " + NextID.toUtf8();
        }
    }


    // /////////////Insert and Update ACCOUNTS////////////////

    Table_Name = "COACHES";
    Column_Name = "Packet (8)";
    query.prepare(QString("INSERT OR IGNORE INTO [%1] ([%2])""VALUES(:id)").arg(Table_Name).arg(Column_Name));
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_Coaches = " + Column_Name.toUtf8();
    }

    Column_value = textout.toUtf8();

    Column_Name = "Name (24 Max)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_Coaches = " + Column_Name.toUtf8();
    }

    Column_Name = "ObjectID (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",IN_CharacterCreation.CharSelectID);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_Coaches = " + Column_Name.toUtf8();
    }

    QString objectidb1 = "ObjectID (4)";
    QString maintoonb1 = IN_CharacterCreation.CharSelectID;

    query.prepare(QStringLiteral("SELECT * FROM COACHES WHERE [%1] = '%2'").arg(objectidb1).arg(maintoonb1));
    query.exec();
    query.next();

    if(!query.exec())
    {
        return "Fail_Coaches = " + Column_Name.toUtf8();
    }

   record = query.record();


    for (int rr = 0; rr < record.count(); rr++)
    {
        IN_CharacterCreation.ellisto.append(record.value(rr).toString());
        IN_CharacterCreation.positionlist.append(record.fieldName(rr));
    }

    IN_CharacterCreation.ellisto.removeAt(0);
    IN_CharacterCreation.ellisto.removeAt(0);
    IN_CharacterCreation.ellisto.removeAt(0);

    IN_CharacterCreation.positionlist.removeAt(0);
    IN_CharacterCreation.positionlist.removeAt(0);
    IN_CharacterCreation.positionlist.removeAt(0);

    QVector<QVector<QString>> TempVectMain;

    TempVectMain.append(IN_CharacterCreation.ellisto);
    TempVectMain.append(IN_CharacterCreation.positionlist);

    Opcodes::ID_VectorOfCoachesVectorMap.insert(IN_CharacterCreation.CharSelectID,TempVectMain);


    for (int rt = 0; rt < positionlist2.count(); rt++)
    {
        QString place1 = positionlist2.value(rt);
        query.prepare(QString("UPDATE COACHES SET [%1] = :val WHERE [%2] = :val2").arg(place1).arg(objectidb1));

        writenum = StartingCoachVector.at(rt);

        query.bindValue(":val",writenum);
        query.bindValue(":val2",maintoonb1);
        query.exec();

        if(!query.exec())
        {
            return "Fail_CoachesX";
        }
    }

    writenum = 0;



    qDebug() << "NameHex" << IN_CharacterCreation.Name;


    qDebug() << "NameActual" << textout;

    Table_Name = "FACTION";
    Column_Name = "Packet (8)";
    query.prepare(QString("INSERT OR IGNORE INTO [%1] ([%2])""VALUES(:id)").arg(Table_Name).arg(Column_Name));
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_FACTION = " + Column_Name.toUtf8();
    }




    Column_value = textout.toUtf8();

    Column_Name = "Name (24 Max)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_FACTION = " + Column_Name.toUtf8();
    }


    Column_Name = "ObjectID (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",IN_CharacterCreation.CharSelectID);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_FACTION = " + Column_Name.toUtf8();
    }

    objectidb1 = "ObjectID (4)";
    maintoonb1 = IN_CharacterCreation.CharSelectID;
    query.prepare(QStringLiteral("SELECT * FROM FACTION WHERE [%1] = '%2'").arg(objectidb1).arg(maintoonb1));
    query.exec();

    if(!query.exec())
    {
        return "Fail_FACTION = " + objectidb1.toUtf8();
    }

    query.next();
    record = query.record();

    for (int rr = 0; rr < record.count(); rr++)
    {
        positionlist2.append(record.fieldName(rr));
    }

    positionlist2.removeAt(0);
    positionlist2.removeAt(0);
    positionlist2.removeAt(0);

    qDebug() << "positionlist2F" << positionlist2.size();

    for (int rt = 0; rt < positionlist2.count(); rt++)
    {
        QString place1 = positionlist2.value(rt);

        query.prepare(QString("UPDATE FACTION SET [%1] = :val WHERE [%2] = :val2").arg(place1).arg(objectidb1));

        writenum = StartingFactionVector.at(rt);

        query.bindValue(":val",writenum);
        query.bindValue(":val2",maintoonb1);
        query.exec();

        if(!query.exec())
        {
            return "Fail_FACTIONX = " + Column_Name.toUtf8();
        }
    }

    positionlist2.clear();
    writenum = 0;
    // /////////////Insert and Update Coaches and Factions///////////////



    // /////////////Insert and Update Maintoon and Make C9///////////////
    int Level1 = IN_CharacterCreation.sLevel.toInt(nullptr,16);
    Level1 = Level1 / 2;
    QString Level2 = QString("%1").arg(Level1,2,16,QLatin1Char('0')).toUpper();

    int Race1 = IN_CharacterCreation.Race.toInt(nullptr,16);
    Race1 = Race1 / 2;
    QString Race2 = QString("%1").arg(Race1,2,16,QLatin1Char('0')).toUpper();

    int Class1 = IN_CharacterCreation.Class.toInt(nullptr,16);
    Class1 = Class1 / 2;
    QString Class2 = QString("%1").arg(Class1,2,16,QLatin1Char('0')).toUpper();

    int HairColor1 = IN_CharacterCreation.HairColor.toInt(nullptr,16);
    HairColor1 = HairColor1 / 2;
    QString HairColor2 = QString("%1").arg(HairColor1,2,16,QLatin1Char('0')).toUpper();

    int HairLength1 = IN_CharacterCreation.HairLength.toInt(nullptr,16);
    HairLength1 = HairLength1 / 2;
    QString HairLength2 = QString("%1").arg(HairLength1,2,16,QLatin1Char('0')).toUpper();

    int HairStyle1 = IN_CharacterCreation.HairStyle.toInt(nullptr,16);
    HairStyle1 = HairStyle1 / 2;
    QString HairStyle2 = QString("%1").arg(HairStyle1,2,16,QLatin1Char('0')).toUpper();

    int Face1 = IN_CharacterCreation.Face.toInt(nullptr,16);
    Face1 = Face1 / 2;
    QString Face2 = QString("%1").arg(Face1,2,16,QLatin1Char('0')).toUpper();

    QString StartingLoaction = Race2 + Class2 + IN_CharacterCreation.FPorNot;

    qDebug() << "StartingLoaction" << StartingLoaction;


    QString StartingXZYF = StartingXZYandFacingMap.value(StartingLoaction);

    QString StartingX = StartingXZYF.mid(0,6);
    QString StartingZ = StartingXZYF.mid(6,6);
    QString StartingY = StartingXZYF.mid(12,6);
    QString StartingF = StartingXZYF.mid(18,2);



    objectpacket::holdid1.append(IN_CharacterCreation.CharSelectID);
    objectpacket::holdx.append(StartingX);
    objectpacket::holdz.append(StartingZ);
    objectpacket::holdy.append(StartingY);

    QString StartingStats = StartingStatsMap.value(StartingLoaction);

    if(StartingLoaction == "Oggok")
    {
        IN_CharacterCreation.masterworld = 1;
    }
    else
    {
        IN_CharacterCreation.masterworld = 0;
    }

    IN_CharacterCreation.tele_world = "0" + QString::number(IN_CharacterCreation.masterworld);

    NPC_Location_XZY_2 = NPC_Location_XZY;

    NPC_Location_XZY_2.replace(4,2,IN_CharacterCreation.tele_world.toUtf8());
    NPC_Location_XZY_2.replace(6,8,StartingX.toUtf8());
    NPC_Location_XZY_2.replace(14,8,StartingZ.toUtf8());
    NPC_Location_XZY_2.replace(22,8,StartingY.toUtf8());

    QString Facing = StartingF;

    NPC_Location_XZY_2.replace(30,8,Facing.toUtf8());


    IN_CharacterCreation.teleport_destination = NPC_Location_XZY_2;

    QString STR1 = StartingStats.mid(0,2);
    QString STA1 = StartingStats.mid(2,2);
    QString AGI1 = StartingStats.mid(4,2);
    QString DEX1 = StartingStats.mid(6,2);
    QString WIS1 = StartingStats.mid(8,2);
    QString INT1 = StartingStats.mid(10,2);
    QString CHA1 = StartingStats.mid(12,2);

    int STR2 = STR1.toInt(nullptr,10);
    int STA2 = STA1.toInt(nullptr,10);
    int AGI2 = AGI1.toInt(nullptr,10);
    int DEX2 = DEX1.toInt(nullptr,10);
    int WIS2 = WIS1.toInt(nullptr,10);
    int INT2 = INT1.toInt(nullptr,10);
    int CHA2 = CHA1.toInt(nullptr,10);


    int STRtotal = STR2 + IN_CharacterCreation.STR1;
    int STAtotal = STA2 + IN_CharacterCreation.STA1;
    int AGItotal = AGI2 + IN_CharacterCreation.AGI1;
    int DEXtotal = DEX2 + IN_CharacterCreation.DEX1;
    int WIStotal = WIS2 + IN_CharacterCreation.WIS1;
    int INTtotal = INT2 + IN_CharacterCreation.INT1;
    int CHAtotal = CHA2 + IN_CharacterCreation.CHA1;


    QString STR3 = QString("%1").arg(STRtotal,2,16,QLatin1Char('0')).toUpper();
    QString STR4 = QString("%1").arg(STRtotal,2,10,QLatin1Char('0')).toUpper();
    QString STA3 = QString("%1").arg(STAtotal,2,16,QLatin1Char('0')).toUpper();
    QString STA4 = QString("%1").arg(STAtotal,2,10,QLatin1Char('0')).toUpper();
    QString AGI3 = QString("%1").arg(AGItotal,2,16,QLatin1Char('0')).toUpper();
    QString AGI4 = QString("%1").arg(AGItotal,2,10,QLatin1Char('0')).toUpper();
    QString DEX3 = QString("%1").arg(DEXtotal,2,16,QLatin1Char('0')).toUpper();
    QString DEX4 = QString("%1").arg(DEXtotal,2,10,QLatin1Char('0')).toUpper();
    QString WIS3 = QString("%1").arg(WIStotal,2,16,QLatin1Char('0')).toUpper();
    QString WIS4 = QString("%1").arg(WIStotal,2,10,QLatin1Char('0')).toUpper();
    QString INT3 = QString("%1").arg(INTtotal,2,16,QLatin1Char('0')).toUpper();
    QString INT4 = QString("%1").arg(INTtotal,2,10,QLatin1Char('0')).toUpper();
    QString CHA3 = QString("%1").arg(CHAtotal,2,16,QLatin1Char('0')).toUpper();
    QString CHA4 = QString("%1").arg(CHAtotal,2,10,QLatin1Char('0')).toUpper();


    QString MyHP1 = AllValues.SendToMAXHP(ClientID,ClientsIP,ClientsPort,Level2, Class2, STA4);
    int MyHP2 = MyHP1.toInt(nullptr,10);
    QString MyHP = QString("%1").arg(MyHP2,2,16,QLatin1Char('0')).toUpper();
    QString MyHPMax = MyHP;

    QMap <QString,QString> ClassPrimarySecondaryStatMap = //had to put it here so map could update
    {
        {"00",STR4+STA4},{"02",STR4+STA4},{"03",STR4+STA4},                                  // TANK
        {"09",WIS4+CHA4},{"07",WIS4+DEX4},{"08",WIS4+STA4},                                  // PRIEST
        {"05",AGI4+STA4},{"04",DEX4+STA4},{"01",AGI4+DEX4},{"06",AGI4+DEX4},                 // MELEE
        {"0E",INT4+WIS4},{"0C",INT4+CHA4},{"0A",INT4+AGI4},{"0B",INT4+STA4},{"0D",INT4+DEX4} // CASTER
    };

    QString StatsPower = ClassPrimarySecondaryStatMap.value(Class2);

    QString PrimaryStat = StatsPower.mid(0,2);
    QString SecondaryStat = StatsPower.mid(2,2);

    QString MyPOW1 = AllValues.SendToMAXPOW(ClientID,ClientsIP,ClientsPort,Level2, PrimaryStat, SecondaryStat);
    int MyPOW2 = MyPOW1.toInt(nullptr,10);
    QString MyPOW = QString("%1").arg(MyPOW2,2,16,QLatin1Char('0')).toUpper();
    QString MyPOWMax = MyPOW;

    QString UNK01 = "00";

    QString MyHOT = "";
    QString MyPOT = "";

    if(MyHP2 >= 99)
    {
        MyHOT = "02";
    }
    else
    {
        MyHOT = "01";
    }

    if(MyPOW2 >= 99)
    {
        MyPOT = "02";
    }
    else
    {
        MyPOT = "01";
    }


    QString MyAC = "00";

    QString UNK02 = "00000000000000000000000000000000000000000000000000";

    QString MyFishing = "00";

    QString MYBaseSTR = STR3;
    QString MYBaseSTA = STA3;
    QString MYBaseAGI = AGI3;
    QString MYBaseDEX = DEX3;
    QString MYBaseWIS = WIS3;
    QString MYBaseINT = INT3;
    QString MYBaseCHA = CHA3;

    QString MYHPActual = MyHP;
    QString MYBaseHP = MyHP;
    QString MYPOWActual = MyPOW;
    QString MYBasePOW = MyPOW;
    QString UNK03 = "00";
    QString MyBaseHOT = MyHOT;
    QString MyBasePOT = MyPOT;
    QString MYBaseAC = MyAC;
    QString UNK04 = "00000000000000000000000000000000000000000000000000";


    QString MyBasePR = "00";
    QString MyBaseDR = "00";
    QString MyBaseFR = "00";
    QString MyBaseCR = "00";
    QString MyBaseLR = "00";
    QString MyBaseAR = "00";
    QString MYBaseFishing = MyFishing;
    QString Breath = "FF";

    QVector <QString> ALLStatsVector =
    {
        textout,IN_CharacterCreation.CharSelectID,STR3,STA3,AGI3,DEX3,WIS3,INT3,CHA3,
        MyHP,MyHPMax,MyPOW,MyPOWMax,UNK01,
        MyHOT,MyPOT,MyAC,UNK02,
        "00","00","00","00","00","00",MyFishing,
        MYBaseSTR,MYBaseSTA,MYBaseAGI,MYBaseDEX,
        MYBaseWIS,MYBaseINT,MYBaseCHA,MYHPActual,
        MYBaseHP,MYPOWActual,MYBasePOW,UNK03,
        MyBaseHOT,MyBasePOT,MYBaseAC,UNK04,
        MyBasePR,MyBaseDR,MyBaseFR,MyBaseCR,
        MyBaseLR,MyBaseAR,MYBaseFishing,Breath
    };

    Table_Name = "STATS42EC";
    Column_Name = "Packet (8)";
    query.prepare(QString("INSERT OR IGNORE INTO [%1] ([%2])""VALUES(:id)").arg(Table_Name).arg(Column_Name));
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_STATS42EC = " + Column_Name.toUtf8();
    }

    qDebug() << "StatsFieldsVector.size()" << StatsFieldsVector.size();
    qDebug() << "ALLStatsVector.size()" << ALLStatsVector.size();

    for (int rt = 0; rt < StatsFieldsVector.size(); rt++)
    {
        QString place1 = StatsFieldsVector.value(rt);

        QString EC42Val = ALLStatsVector.value(rt);

        query.prepare(QString("UPDATE STATS42EC SET [%1] = :val1 WHERE ROWID = :id").arg(place1));

        if(rt > 1)
        {
            query.bindValue(":val1",EC42Val+"000000");
            query.bindValue(":id",IN_CharacterCreation.packetid);
            query.exec();

            if(!query.exec())
            {
                return "Fail_STATS42ECX_1 = " + EC42Val.toUtf8();
            }

            qDebug() << "rt" << rt << "EC42Val2" << EC42Val+"000000";
        }
        else
        {
            query.bindValue(":val1",EC42Val);
            query.bindValue(":id",IN_CharacterCreation.packetid);
            query.exec();

            if(!query.exec())
            {
                return "Fail_STATS42ECX_2 = " + EC42Val.toUtf8();
            }

            qDebug() << "rt" << rt << "EC42Val1" << EC42Val;
        }
    }

    // ////////////////////////////////////////////////////////////////////////////////////////
    QString RaceName = RaceValueToString.value(Race2);
    QString ClassName = ClassValueToString.value(Class2);
    QString StartingCity = "";

    qDebug() << "";
    qDebug() << "Race2" << Race2;
    qDebug() << "RaceName" << RaceName;
    qDebug() << "Class2" << Class2;
    qDebug() << "ClassName" << ClassName;

    if(RaceName == "HUM")
    {
        if(IN_CharacterCreation.FPorNot == "02")
        {
            StartingCity = "Freeport";
        }
        else
        {
            if(ClassName == "DRD" || ClassName == "RAN")
            {
                StartingCity = "Surefall Glade";
            }
            else
            {
                StartingCity = "Qeynos";
            }
        }
    }
    else
    {
        if(RaceName == "ELF")
        {
            QVector<QString> TethelinClassVector =
            {
                "BRD","DRD","RAN","RGE"
            };

            if(TethelinClassVector.contains(ClassName))
            {
                StartingCity = "Tethelin";
            }
            else
            {
                StartingCity = "Fayspires";
            }
        }
        else
        {
            QMap <QString,QString> RaceStartingLocationsMap =
            {
                {"DELF","Neriak"},
                {"GNO","Klick'Anon"},
                {"DWF","Moradhim"},
                {"TRL","Grobb"},
                {"BAR","Halas"},
                {"HLF","Rivervale"},
                {"ERU","Highbourne"},
                {"OGR","Oggok"}
            };

            StartingCity = RaceStartingLocationsMap.value(RaceName);

        }
    }

    qDebug() << "StartingCity" << StartingCity;

    CharCreate::ID_StartingCityMap.insert(IN_CharacterCreation.CharSelectID,StartingCity);

    Table_Name = "MAINTOON_STARTING_LOCATIONS";

    Column_Name = "PacketID";
    query.prepare(QString("INSERT OR IGNORE INTO [%1] ([%2])""VALUES(:id)").arg(Table_Name).arg(Column_Name));
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////


    Column_Name = "Name (24 Max)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",textout);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }


    Column_Name = "ObjectID (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",IN_CharacterCreation.CharSelectID);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }


    Column_Name = "Starting_City";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",StartingCity);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    Column_Name = "BindLocation";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",IN_CharacterCreation.teleport_destination);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    Column_Name = "MainFlag";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",-1);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }


    // ////////////////////////////////////////////////////////////////////////////////////////


    Table_Name = "MAINTOON";

    Column_Name = "Packet (8)";
    query.prepare(QString("INSERT OR IGNORE INTO [%1] ([%2])""VALUES(:id)").arg(Table_Name).arg(Column_Name));
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////


    QString Column_value7 = textout;

    Column_Name = "Name (24 Max)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value7);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    checkthezone::NPCsNames.insert(IN_CharacterCreation.CharSelectID,textout);
    Opcodes::PlayerNames.append(textout);
    ////////////////////////////////////////////////////////////////////////////////////////////

    QString RemainingTPs1 = QString("%1").arg(IN_CharacterCreation.RemainingTPs,1,10,QLatin1Char('0')).toUpper();
    Convert SendToHex;
    QString RemainingTPs2 = SendToHex.ConvertTo(RemainingTPs1);

    Column_Name = "RemainingTPs";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",RemainingTPs2);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    QString TotalTPSpent1 = QString("%1").arg(IN_CharacterCreation.TotalTPSpent,1,10,QLatin1Char('0')).toUpper();
    QString TotalTPSpent2 = SendToHex.ConvertTo(TotalTPSpent1);

    Column_Name = "TotalTPSpent";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",TotalTPSpent2);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "TunarOnPlayer";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    Column_value = "00";
    Column_Name = "TunarInBank";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "XPbar";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "TotalXP";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "CMxp";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "TotalCMxp";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "UnSpentCMs";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "SpentCMs";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "01";
    Column_Name = "Unk-a (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_Name = "ObjectID (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",IN_CharacterCreation.CharSelectID);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(IN_CharacterCreation.CharSelectID.toUtf8());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "82";
    Column_Name = "Unk-b (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = StartingX.toUtf8().toUpper();
    Column_Name = "X (3)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = StartingZ.toUtf8().toUpper();
    Column_Name = "Z (3)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = StartingY.toUtf8().toUpper();
    Column_Name = "Y (3)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = StartingF.toUtf8().toUpper();
    Column_Name = "Facing (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "V Facing (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    QString Column_valuew = "";

    if(Race2 == "09")
    {
        Column_valuew = "01";
    }
    else
    {
        Column_valuew = "00";
    }

    Column_Name = "World (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_valuew);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_valuew.toUtf8());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "0000000000000000";
    Column_Name = "Date/Time Kill (8)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "01";
    Column_Name = "Hp (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FF";
    Column_Name = "Hp % (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = RaceAndGenderMap.value(Race2.toUtf8() + IN_CharacterCreation.Gender.toUtf8());
    Column_Name = "Model ID (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "U Model ID (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "0000803F";
    Column_Name = "Size (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "0000";
    Column_Name = "W/E Form (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////



    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "0000";
    Column_Name = "N/S Form (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Unk-d (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "0000";
    Column_Name = "W/E Cam (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "0000";
    Column_Name = "N/S Cam (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Unk-f (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "Z Coord (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "Facing (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Animation (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFFFFFF";
    Column_Name = "Target (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "Unk-g (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "MH Weapon (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "OH Weapon (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "Shield (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "Primary MH (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "Primary OH (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "Primary Shield (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "0000";
    Column_Name = "Unk-h (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Torso (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Forearm (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Gloves (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Legs (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Feet (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Head (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFF";
    Column_Name = "Chest Color (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFF";
    Column_Name = "Bracer Color (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFF";
    Column_Name = "Glove Color (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFF";
    Column_Name = "Pant Color (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFF";
    Column_Name = "Boot Color (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFF";
    Column_Name = "Helm Color (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFFFFFF00";
    Column_Name = "Chest Rbga (5)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFFFFFF00";
    Column_Name = "Bracer Rbga (5)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFFFFFF00";
    Column_Name = "Glove Rbga (5)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFFFFFF00";
    Column_Name = "Pant Rbga (5)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFFFFFF00";
    Column_Name = "Boot Rbga (5)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFFFFFF00";
    Column_Name = "Helm Rbga (5)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFFFFFF";
    Column_Name = "Robe Rbga (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = HairColor2.toUtf8();
    Column_Name = "Hair Color (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = HairLength2.toUtf8();
    Column_Name = "Hair Length (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = HairStyle2.toUtf8();
    Column_Name = "Hair Style (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = Face2.toUtf8();
    Column_Name = "Face (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FF";
    Column_Name = "Robe Style (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = Race2.toUtf8();
    Column_Name = "Race-A (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00000000";
    Column_Name = "Unk-j (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    QByteArray arg2 = textout.toUtf8().toHex();
    int Mysize = arg2.size() /2; //divide by two for bytes
    int numzeros = 24 - Mysize;

    for (int x = 0; x < numzeros; x++)
    {
        arg2.append("00");
    }

    Column_value = arg2;
    Column_Name = "Character Name (24 Max)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = Level2.toUtf8();
    Column_Name = "Level (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "01";
    Column_Name = "Player/NPC (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "02";
    Column_Name = "Con (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Target (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = Race2.toUtf8();
    Column_Name = "Race (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = Class2.toUtf8();
    Column_Name = "Class (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "0000";
    Column_Name = "Npc Type (2)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "FFFFFFFF";
    Column_Name = "Npc ID (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Unk-k (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "15";
    Column_Name = "Unk-l (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "PDI Icon (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {

        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Unk-m (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "01";
    Column_Name = "Unk-n (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {

        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "00";
    Column_Name = "Unk-o (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = "74737271";
    Column_Name = "Message End (4)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_CharacterCreation.packetid);
    query.exec();

    if(!query.exec())
    {
        return "Fail_MAINTOON = " + Column_Name.toUtf8();
    }
    else
    {
        rowline.append(Column_value);
    }




    db.commit();

    db.close();
    db.removeDatabase(QSqlDatabase::defaultConnection);

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_CharacterCreation);

    qDebug() << "rowline.size()" << rowline.size();
    qDebug() << "rowline" << rowline;

    objectpacket::Master_Map.insert(IN_CharacterCreation.CharSelectID.toUtf8(),rowline);

    return rowline;

}
