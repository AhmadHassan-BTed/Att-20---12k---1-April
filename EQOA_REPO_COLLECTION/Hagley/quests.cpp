#include "quests.h"
#include "increment.h"
#include "add_or_sub.h"
#include "packetparsing.h"
#include "charcreate.h"
#include "opcodes.h"
#include "packetsize.h"
#include "crc.h"
#include "worldthread.h"
#include "checkthezone.h"
#include "mainwindow.h"
#include "convert.h"
#include "generator.h"
#include "xorpacket.h"
#include "flipped.h"
#include "merchants.h"
#include "objectpacket.h"

Quests::Quests(QObject *parent) : QObject(parent)
{

}



QByteArray Quests::SendoutSpell(QByteArray ID_IP_PORT, QString ResultID)
{
    packetparsing::packetvars IN_SendoutSpell;
    IN_SendoutSpell = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    ResultID.remove(0,3);

    QString Caster = ResultID.mid(0,8);

    int index = ResultID.indexOf("$");

    QString Val = ResultID.mid(index+1,1);


    QString Target = "";

    if(Val == "T")
    {
        Target = IN_SendoutSpell.CharSelectID_Main;
    }

    if(Val == "S")
    {
        Target = Caster;
    }

    int index1 = ResultID.indexOf("#");

    QString SpellGraphic = ResultID.mid(index1+1,ResultID.size());

    QString main_spell = "b100" + Caster.toUtf8() +  Target.toUtf8() + "00e8030000" + SpellGraphic.toUtf8();

    ADD_OR_SUB CurrentItem;
    QString NewFB = CurrentItem.FBSize(main_spell);

    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_SendoutSpell.CharSelectID);
    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

    increment Cash1;
    QByteArray Cash2 = Cash1.count(Servers_Last_FB_MessageNumber);
    index = NewFB.indexOf("XXXX");
    NewFB.replace(index,4,Cash2);
    Servers_Last_FB_MessageNumber = Cash2.toUpper();

    QByteArray Packet = NewFB.toUtf8();

    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
    Opcodes::CharID_ServerMessageNumbers.insert(IN_SendoutSpell.CharSelectID,MessNumbVect);


    return Packet;

}

QByteArray Quests::SendoutMenus(QByteArray ID_IP_PORT, QString ResultID)
{
    packetparsing::packetvars IN_SendoutMenus;
    IN_SendoutMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    QVector<QString>MenuTextVect;
    QVector<QString> tempVect;
    QString LargeString = "";
    QString Send_FB = "";
    QString HeaderText = "";
    int SizeOut = 0;

    //qDebug() << "IN_SendoutMenus.ResultID" << ResultID;

    //qDebug() << "IN_SendoutMenus.InGame_MenuesVectorMap" << InGame_MenuesVectorMap;

    if(!ResultID.contains("SM_"))
    {
        MenuTextVect = InGame_MenuesVectorMap.value(ResultID);

        if(MenuTextVect.isEmpty() == false)
        {
            LargeString = MenuTextVect.at(0);

            SizeOut = MenuTextVect.size()-1;
        }
    }

    QString ClassName = ClassValueToString.value(IN_SendoutMenus.Class);

    //qDebug() << "IN_SendoutMenus.CharSelectID" << IN_SendoutMenus.CharSelectID;
    //qDebug() << "IN_SendoutMenus.Class" << IN_SendoutMenus.Class;
    //qDebug() << "IN_SendoutMenus.ClassName" << ClassName;

    QString StartingCity = CharCreate::ID_StartingCityMap.value(IN_SendoutMenus.CharSelectID);

    //qDebug() << "IN_SendoutMenus.StartingCity" << StartingCity;

    QMap<QString,QVector<QString>> ClassGreetings_VectorMap = StartingCity_WorldGreetings_VectorMap.value(StartingCity);

    QVector<QString> ClassStuffVector = ClassGreetings_VectorMap.value(ClassName);

    ClassStuffVector.append(StartingCity);

    //qDebug() << "IN_SendoutMenus.ClassStuffVector" << ClassStuffVector;

    if(ResultID.contains("SM_"))
    {
        QString NPCsID = ResultID.mid(3,11);
        tempVect = Quests::SpiritmastersID_MenuMap.value(NPCsID);

        if(tempVect.isEmpty() == false)
        {
            LargeString = tempVect.at(0);
        }

        SizeOut = 2;
    }

    //qDebug() << "IN_SendoutMenus.LargeString" << LargeString;

    HeaderText = LargeString;

    for(int re = 0; re < PlaceHolderStuffVector.size(); re++)
    {
        QString FakeStuff = PlaceHolderStuffVector.at(re);

        if(HeaderText.contains(FakeStuff))
        {
            do
            {
                int index = HeaderText.indexOf(FakeStuff);

                QString RealStuff = ClassStuffVector.at(re);

                //qDebug() << FakeStuff << "replaced with" << RealStuff;

                HeaderText.replace(index,FakeStuff.size(),RealStuff);

            }while(HeaderText.contains(FakeStuff));
        }
    }

    if(HeaderText.contains("!@#"))
    {
        do
        {
            int index = HeaderText.indexOf("!@#");
            HeaderText.remove(index,3);

        }while(HeaderText.contains("!@#"));
    }
    //-----------------------------------------

    increment send;
    IN_SendoutMenus.QuestMenuCounter = send.count(IN_SendoutMenus.QuestMenuCounter);

    QString Opcode = "3400" + IN_SendoutMenus.QuestMenuCounter + "0000";

    //qDebug() << "IN_SendoutMenus.HeaderText" << HeaderText;

    QByteArray ConvertedHeader = MenuHeaderConverter(HeaderText);

    QString OptionCount = QString("%1").arg((SizeOut),2,16,QLatin1Char('0')).toUpper();

    Send_FB.append(Opcode.toUtf8() + ConvertedHeader + OptionCount.toUtf8());

    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_SendoutMenus.CharSelectID);
    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

    increment Resending;
    QByteArray Resending2 = Resending.count(Servers_Last_FB_MessageNumber);
    Servers_Last_FB_MessageNumber = Resending2.toUpper();

    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

    Opcodes::CharID_ServerMessageNumbers.insert(IN_SendoutMenus.CharSelectID,MessNumbVect);



    if(!ResultID.contains("SM_"))
    {
        for(int send = 1; send < MenuTextVect.size(); send+=2)
        {
            QString OptionText = MenuTextVect.at(send+1);

            //qDebug() << "IN_SendoutMenus.OptionText1" << OptionText;

            for(int re = 0; re < PlaceHolderStuffVector.size(); re++)
            {
                QString FakeStuff = PlaceHolderStuffVector.at(re);

                if(OptionText.contains(FakeStuff))
                {
                    do
                    {
                        int index = OptionText.indexOf(FakeStuff);

                        QString RealStuff = ClassStuffVector.at(re);

                        //qDebug() << FakeStuff << "replaced with" << RealStuff;

                        OptionText.replace(index,FakeStuff.size(),RealStuff);

                    }while(OptionText.contains(FakeStuff));
                }
            }

            if(OptionText.contains("!@#"))
            {
                do
                {
                    int index = OptionText.indexOf("!@#");
                    OptionText.remove(index,3);

                }while(OptionText.contains("!@#"));
            }

            QByteArray ConvertedText = MenuHeaderConverter(OptionText);

            Send_FB.append(ConvertedText);
        }
    }
    else
    {
        if(tempVect.isEmpty() == false)
        {
            for(int send = 1; send < tempVect.size(); send++)
            {
                QString OptionText = tempVect.at(send);

                //qDebug() << "IN_SendoutMenus.OptionText2" << OptionText;

                for(int re = 0; re < PlaceHolderStuffVector.size(); re++)
                {
                    QString FakeStuff = PlaceHolderStuffVector.at(re);

                    if(OptionText.contains(FakeStuff))
                    {
                        do
                        {
                            int index = OptionText.indexOf(FakeStuff);

                            QString RealStuff = ClassStuffVector.at(re);

                            //qDebug() << FakeStuff << "replaced with" << RealStuff;

                            OptionText.replace(index,FakeStuff.size(),RealStuff);

                        }while(OptionText.contains(FakeStuff));
                    }
                }

                if(OptionText.contains("!@#"))
                {
                    do
                    {
                        int index = OptionText.indexOf("!@#");
                        OptionText.remove(index,3);

                    }while(OptionText.contains("!@#"));
                }

                QByteArray ConvertedText = MenuHeaderConverter(OptionText);

                Send_FB.append(ConvertedText);
            }
        }
    }

    QByteArray ConvertedText = TestingMenus(Send_FB);

    int index = ConvertedText.indexOf("yyyy");
    ConvertedText.replace(index,4,Servers_Last_FB_MessageNumber);

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_SendoutMenus);

    return ConvertedText;

}

QByteArray Quests::SendPopUps(QByteArray ID_IP_PORT, QString ResultID)
{
    packetparsing::packetvars IN_SendPopUps;
    IN_SendPopUps = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    QByteArray ConvertedText = "";
    QString Text = "";

    if(ResultID.contains("P_SM_"))
    {
        ResultID.remove(0,5);
        Text = ResultID;
    }
    else
    {
        Text = Quests::PopUpID_PopUpText_Map.value(ResultID);
    }

    //qDebug() << "IN_SendPopUps.Text" << Text;

    QString Opcode = "4600xxxx0000";

    QString ClassName = ClassValueToString.value(IN_SendPopUps.Class);

    //qDebug() << "IN_SendPopUps.CharSelectID" << IN_SendPopUps.CharSelectID;
    //qDebug() << "IN_SendPopUps.Class" << IN_SendPopUps.Class;
    //qDebug() << "IN_SendPopUps.ClassName" << ClassName;

    QString StartingCity = CharCreate::ID_StartingCityMap.value(IN_SendPopUps.CharSelectID);

    //qDebug() << "IN_SendPopUps.StartingCity" << StartingCity;

    QMap<QString,QVector<QString>> ClassGreetings_VectorMap = StartingCity_WorldGreetings_VectorMap.value(StartingCity);

    QVector<QString> ClassStuffVector = ClassGreetings_VectorMap.value(ClassName);

    ClassStuffVector.append(StartingCity);

    //qDebug() << "IN_SendPopUps.ClassStuffVector" << ClassStuffVector;


    //qDebug() << "IN_SendPopUps.PlaceHolderStuffVector" << PlaceHolderStuffVector;

    for(int re = 0; re < PlaceHolderStuffVector.size(); re++)
    {
        QString FakeStuff = PlaceHolderStuffVector.at(re);

        if(Text.contains(FakeStuff))
        {
            do
            {
                int index = Text.indexOf(FakeStuff);

                QString RealStuff = ClassStuffVector.at(re);

                //qDebug() << FakeStuff << "replaced with" << RealStuff;

                Text.replace(index,FakeStuff.size(),RealStuff);

            }while(Text.contains(FakeStuff));
        }
    }

    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_SendPopUps.CharSelectID);
    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_SendPopUps);

    if(Text.contains("!@#"))
    {
        do
        {
            int index = Text.indexOf("!@#");
            Text.remove(index,3);

        }while(Text.contains("!@#"));
    }

    //qDebug() << "IN_SendPopUps.Text" << Text;

    Quests sendto;
    ConvertedText = sendto.TestingOptions(Opcode, Text, Servers_Last_FB_MessageNumber,IN_SendPopUps.ClientsID + IN_SendPopUps.ClientsIP + IN_SendPopUps.ClientsPort);

    IN_SendPopUps = packetparsing::IPandPort_AllvariablesMap.value(IN_SendPopUps.ClientsID + IN_SendPopUps.ClientsIP + IN_SendPopUps.ClientsPort);

    Servers_Last_FB_MessageNumber = ConvertedText.mid(0,4);

    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

    Opcodes::CharID_ServerMessageNumbers.insert(IN_SendPopUps.CharSelectID,MessNumbVect);

    ConvertedText.remove(0,4);


    //qDebug() << "IN_SendPopUps.ConvertedText" << ConvertedText;

    return ConvertedText;
}

bool Quests::SendtoItems(QByteArray ID_IP_PORT, QString ResultID)
{


    int RemoveQuantity = 0;

    packetparsing::packetvars IN_SendtoItems;
    IN_SendtoItems = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    //qDebug() << "";
    //qDebug() << "";
    //qDebug() << "";
    //qDebug() << "IN_SendtoItems.ResultID" << ResultID;

    QByteArray Packet = "";

    QString ItemsID = ResultID.mid(3,10);

    //qDebug() << "IN_SendtoItems.ItemsID1" << ItemsID;

    QString FBsizeOpcode1 = "";
    QString FBsizeOpcode2 = "";
    QString SendText = "";

    QString ItemStatus = "";

    bool InventoryIsFull = false;

    if(ResultID.contains("IG_"))
    {
        //IT_BCBC808000#2


        QString Size = ResultID.mid(14,ResultID.size());

        RemoveQuantity = Size.toInt(nullptr,10);

        QString CurrentInventoryQuantity = "";

        //qDebug() << "IN_SendtoItems.ItemsID2" << ItemsID;

        int Inventoryid = IN_SendtoItems.InventorySlots_ItemIDs.indexOf(ItemsID);

        //qDebug() << "IN_SendtoItems.InventorySlots_ItemIDs2" << IN_SendtoItems.InventorySlots_ItemIDs;

        CurrentInventoryQuantity = IN_SendtoItems.InventoryItem_Quantity.at(Inventoryid);

        ADD_OR_SUB ConvertBack;
        QString BackToDec = ConvertBack.BackToDec(CurrentInventoryQuantity);
        int InvQuantity = BackToDec.toInt(nullptr,10);

        int RemoveSubTotal = InvQuantity - RemoveQuantity;

        if(RemoveSubTotal == 0)
        {
            IN_SendtoItems.InventorySlots_ItemIDs.removeAt(Inventoryid);
            IN_SendtoItems.Inventory_Vector.removeAt(Inventoryid);
            IN_SendtoItems.InventoryItem_Quantity.removeAt(Inventoryid);

            IN_SendtoItems.ItemIsEquippedVector.removeAt(Inventoryid);
            IN_SendtoItems.InvItem_Type.removeAt(Inventoryid);
            IN_SendtoItems.EQuipSots_InvSlots.removeAt(Inventoryid);

            IN_SendtoItems.InventoryMAX--;
        }
        else
        {
            QString Remove_Item2 = QString("%1").arg(RemoveSubTotal,2,10,QLatin1Char('0')).toUpper();
            Convert Send;
            Remove_Item2 = Send.ConvertTo(Remove_Item2);
            IN_SendtoItems.InventoryItem_Quantity.replace(Inventoryid,Remove_Item2);
        }

        QString slotValue1 = QString("%1").arg(Inventoryid,2,16,QLatin1Char('0')).toUpper();

        QString Remove_Item2 = QString("%1").arg(RemoveQuantity,2,10,QLatin1Char('0')).toUpper();
        Convert Send;
        Remove_Item2 = Send.ConvertTo(Remove_Item2);

        QString FBsizeOpcode = "3C00" + slotValue1.toUtf8() + "01" + Remove_Item2.toUtf8();

        IN_SendtoItems.SendFBsVector.append(FBsizeOpcode.toUtf8());
        IN_SendtoItems.FBReplyOrNotVector.append("03");
        IN_SendtoItems.FBPacketDescriptionVector.append("Remove_Item, ");

        //send PopUp

        QString ItemsName = Opcodes::ItemsID_ItemsName_Map.value(ItemsID);

        FBsizeOpcode2 = "You have given away " + ItemsName;

        ItemStatus = "You_have_given_Away_" + ItemsName;
    }


    if(ResultID.contains("IR_"))
    {

        QString Size = ResultID.mid(14,ResultID.size());

        RemoveQuantity = Size.toInt(nullptr,10);

        QString ItemsID = ResultID.mid(3,10);

        QVector <QString> ItemsFields = merchants::MasterItems_Vector_Map.value(ItemsID);

        QString ItemsMaxSize = ItemsFields.at(23);
        QString CostPerItem = ItemsFields.at(10);
        QString ArmorColor = ItemsFields.at(0);
        QString WeaponGraphic = ItemsFields.at(1);
        QString ArmorGraphic = ItemsFields.at(2);
        QString Charges = ItemsFields.at(5);
        QString EQuipmentsType = ItemsFields.at(14);
        QString DoesItHaveCharges = "";
        bool IsItStackable;
        bool NewItem;

        if(Charges != "00")
        {
            DoesItHaveCharges = "YES";
        }
        else
        {
            DoesItHaveCharges = "NO";
        }

        //qDebug() << "IN_SendtoItems.DoesItHaveCharges" << DoesItHaveCharges;

        if(ItemsMaxSize == "02")
        {
            IsItStackable = false;

        }
        else
        {
            IsItStackable = true;
        }

        //qDebug() << "IN_SendtoItems.IsItStackable" << IsItStackable;

        ADD_OR_SUB ConvertBack;
        QString BackToDec = "";
        int QuantityLocation = 0;
        int InventoryID = 0;
        QString Inventoryidxxxxxxxx = "";
        QString CurrentInventoryQuantity = "";
        Convert Send;
        ADD_OR_SUB SendToText;
        QString FBText = "";
        QString TextFull = "";
        QString Add_ItemTotal = "";
        QString ConvertedValue = "";

        bool ItemsFull;

        if(IN_SendtoItems.InventorySlots_ItemIDs.contains(ItemsID))
        {
            if(IsItStackable == true) // if it is a stack and it is in the inventory already
            {
                NewItem = false;
                QuantityLocation = IN_SendtoItems.InventorySlots_ItemIDs.indexOf(ItemsID);
                Inventoryidxxxxxxxx = IN_SendtoItems.Inventory_Vector.at(QuantityLocation);
                CurrentInventoryQuantity = IN_SendtoItems.InventoryItem_Quantity.at(QuantityLocation);
                //qDebug() << "item is in here1 ";
            }
            else // if it is not a stack and there is one in the inventory already
            {    // needs to be treated as a new item

                CurrentInventoryQuantity = "00";
                NewItem = true;
                //qDebug() << "item is in here2 ";
            }
        }
        else // if it is not in the inventory
        {
            CurrentInventoryQuantity = "00";
            NewItem = true;
            //qDebug() << "item is in here3 ";
        }

        //qDebug() << "IsItStackable" << IsItStackable;
        //qDebug() << "CurrentInventoryQuantity" << CurrentInventoryQuantity;
        //qDebug() << "NewItem" << NewItem;


        QString Item_Quantity = Size;

        //qDebug() << "Item_Quantity" << Item_Quantity;

        int AddQuantity = Item_Quantity.toInt(nullptr,10);
        BackToDec = ConvertBack.BackToDec(CurrentInventoryQuantity);
        int InvQuantity = BackToDec.toInt(nullptr,10);
        BackToDec = ConvertBack.BackToDec(ItemsMaxSize);
        int ItemsMaxSize2 = BackToDec.toInt(nullptr,10);
        int total = InvQuantity + AddQuantity;

        //qDebug() << "AddQuantity" << AddQuantity;
        //qDebug() << "InvQuantity" << InvQuantity;
        //qDebug() << "total" << total;

        if(total <= ItemsMaxSize2 || IsItStackable == false)//if the stack can fit or its not stackable
        {
            //qDebug() << "item is in here4 ";
            ItemsFull = false; // false = the item is not full

            Add_ItemTotal = QString("%1").arg(AddQuantity,2,10,QLatin1Char('0')).toUpper();
            Add_ItemTotal = Send.ConvertTo(Add_ItemTotal);

            QString Add_Item2 = QString("%1").arg(total,2,10,QLatin1Char('0')).toUpper();

            ConvertedValue = Send.ConvertTo(Add_Item2);
        }



        // if the stack amount you picked does not fit
        if(total > ItemsMaxSize2)// if we try to Add more than we can fit
        {
            //qDebug() << "item is in here5 ";

            int total2 = total - ItemsMaxSize2;// how much we cant fit
            int total3 = AddQuantity - total2;// subtract what we cant fit

            if(total3 > 0)//if what we can fit is less then the max stack size
            {
                ItemsFull = false;//then Item is not full

                Add_ItemTotal = QString("%1").arg(total3,2,10,QLatin1Char('0')).toUpper();
                Add_ItemTotal = Send.ConvertTo(Add_ItemTotal);

                QString Add_Item2 = QString("%1").arg(total3 + InvQuantity,2,10,QLatin1Char('0')).toUpper();

                ConvertedValue = Send.ConvertTo(Add_Item2);
            }
            else
            {
                //qDebug() << "item is in here6 ";
                ItemsFull = true;
                //qDebug() << "ItemsFull4 =" << ItemsFull;
            }
        }

        //qDebug() << "ItemsFull" << ItemsFull;

        //qDebug() << "IN_SendtoItems.InventoryMAX" << IN_SendtoItems.InventoryMAX;

        if(
                (IN_SendtoItems.InventoryMAX < 40 && ItemsFull == false) ||
                (IN_SendtoItems.InventoryMAX == 40 && ItemsFull == false && IsItStackable == true
                 && NewItem == false))
        {
            //qDebug() << "item is in here7 ";

            QString Add_Item = merchants::Items_Map.value(ItemsID);

            Add_Item.replace(0,2,Add_ItemTotal.toUtf8().toUpper());//change the number of items to what we are Adding

            if(NewItem == true)
            {
                //qDebug() << "item is in here8 ";
                InventoryID = Add_Item.indexOf("XXXXXXXX");
                increment Add;
                QByteArray Add2 = Add.count(IN_SendtoItems.InventoryIDCounter); //InventoryID increment
                Add_Item.replace(InventoryID,8,IN_SendtoItems.InventoryIDCounter + "0000");
                IN_SendtoItems.InventoryIDCounter = Add2.toUpper();

                IN_SendtoItems.InventorySlots_ItemIDs.append(ItemsID);
                IN_SendtoItems.Inventory_Vector.append(IN_SendtoItems.InventoryIDCounter + "0000");

                IN_SendtoItems.InventoryMAX = IN_SendtoItems.InventoryMAX + 1;

                IN_SendtoItems.ItemIsEquippedVector.append("NO");
                IN_SendtoItems.EQuipSots_InvSlots.append("XX");

                if(DoesItHaveCharges == "NO")
                {
                    //qDebug() << "item is in here9 ";
                    //qDebug() << "ConvertedValue" << ConvertedValue;
                    IN_SendtoItems.InventoryItem_Quantity.append(ConvertedValue);
                    //qDebug() << "IN_SendtoItems.InventoryItem_Quantity =" << IN_SendtoItems.InventoryItem_Quantity;
                }
                else
                {
                    //qDebug() << "item is in here10 ";
                    IN_SendtoItems.InventoryItem_Quantity.append(Charges);
                    //qDebug() << "IN_SendtoItems.InventoryItem_Quantity =" << IN_SendtoItems.InventoryItem_Quantity;
                }



                if(EQuipmentsType == "01")
                {
                    //qDebug() << "item is in here11 ";
                    IN_SendtoItems.InvItem_Type.append("Other");
                }
                else
                {
                    //qDebug() << "item is in here12 ";
                    IN_SendtoItems.InvItem_Type.append("EQuipment");
                }
            }

            if(IsItStackable == true && NewItem == false)
            {
                //qDebug() << "item is in here13 ";
                InventoryID = Add_Item.indexOf("XXXXXXXX");
                Add_Item.replace(InventoryID,8,Inventoryidxxxxxxxx.toUtf8().toUpper());
                IN_SendtoItems.InventoryItem_Quantity.replace(QuantityLocation,ConvertedValue);
            }

            QString FBsizeOpcode = "3B00" + Add_Item; // Add opcode

            IN_SendtoItems.SendFBsVector.append(FBsizeOpcode.toUtf8());
            IN_SendtoItems.FBReplyOrNotVector.append("03");
            IN_SendtoItems.FBPacketDescriptionVector.append("Add_Item1, ");

            QString ItemsName = Opcodes::ItemsID_ItemsName_Map.value(ItemsID);

            FBsizeOpcode2 = "You have received " + ItemsName + ".";

            //qDebug() << "FBsizeOpcode2" << FBsizeOpcode2;

            ItemStatus = "You_have_received_" + ItemsName;
        }
        else
        {
            //qDebug() << "item is in here14 ";

            // there are 2 ways your inventory can be full
            if(IsItStackable == true)
            {
                //qDebug() << "item is in here15 ";

                if(NewItem == false)
                {
                    //qDebug() << "item is in here16 ";

                    //this part is for invtory is full opcode
                    IN_SendtoItems.SendFBsVector.append("D700");
                    IN_SendtoItems.FBReplyOrNotVector.append("03");
                    IN_SendtoItems.FBPacketDescriptionVector.append("Fake_Opcode, ");
                    //this part is for invtory is full opcode


                    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_SendtoItems.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(0);


                    TextFull = "Your inventory for that stack item is full.";
                    FBText =  SendToText.TextSize(IN_SendtoItems.ClientsID,IN_SendtoItems.ClientsIP,IN_SendtoItems.ClientsPort,TextFull,true,"CE00",Servers_Last_FB_MessageNumber);

                    IN_SendtoItems.SendFBsVector.append(FBText.toUtf8());
                    IN_SendtoItems.FBReplyOrNotVector.append("03");
                    IN_SendtoItems.FBPacketDescriptionVector.append("Inventory_Full, ");

                    InventoryIsFull = true;

                }
                else
                {
                    //qDebug() << "item is in here17 ";

                    //this part is for invtory is full opcode
                    IN_SendtoItems.SendFBsVector.append("D700");
                    //this part is for invtory is full opcode
                    IN_SendtoItems.FBReplyOrNotVector.append("03");
                    IN_SendtoItems.FBPacketDescriptionVector.append("Fake_Opcode, ");

                    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_SendtoItems.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(0);

                    TextFull = "Your inventory is full.";
                    FBText =  SendToText.TextSize(IN_SendtoItems.ClientsID,IN_SendtoItems.ClientsIP,IN_SendtoItems.ClientsPort,TextFull,true,"CE00",Servers_Last_FB_MessageNumber);

                    IN_SendtoItems.SendFBsVector.append(FBText.toUtf8());
                    IN_SendtoItems.FBReplyOrNotVector.append("03");
                    IN_SendtoItems.FBPacketDescriptionVector.append("Inventory_Full, ");

                    ItemStatus = "Inventory_Full";

                    InventoryIsFull = true;
                }
            }
            else
            {
                //qDebug() << "item is in here18 ";

                //this part is for invtory is full opcode
                IN_SendtoItems.SendFBsVector.append("D700");
                IN_SendtoItems.FBReplyOrNotVector.append("03");
                IN_SendtoItems.FBPacketDescriptionVector.append("Fake_Opcode, ");
                //this part is for invtory is full opcode

                QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_SendtoItems.CharSelectID);
                QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(0);

                TextFull = "Your inventory is full.";
                FBText =  SendToText.TextSize(IN_SendtoItems.ClientsID,IN_SendtoItems.ClientsIP,IN_SendtoItems.ClientsPort,TextFull,true,"CE00",Servers_Last_FB_MessageNumber);

                IN_SendtoItems.SendFBsVector.append(FBText.toUtf8());
                IN_SendtoItems.FBReplyOrNotVector.append("03");
                IN_SendtoItems.FBPacketDescriptionVector.append("Inventory_Full, ");

                ItemStatus = "Inventory_Full";

                InventoryIsFull = true;
            }
        }

        packetparsing::IPandPort_AllvariablesMap.insert(IN_SendtoItems.ClientsID + IN_SendtoItems.ClientsIP + IN_SendtoItems.ClientsPort,IN_SendtoItems);
    }

    //---------------------------------------------------------------------------------------------

    if(FBsizeOpcode2.isEmpty() == false)
    {
        int rotate = 1;
        //qDebug() << "item is in here19 ";

        if(InventoryIsFull == false)
        {
            //qDebug() << "item is in here20 ";
            rotate = RemoveQuantity;
        }

        //qDebug() << "rotate" << rotate;

        //qDebug() << "RemoveQuantity" << RemoveQuantity;

        for(int re = 0; re < rotate; re++)
        {
            //qDebug() << "item is in here21 ";

            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_SendtoItems);
            IN_SendtoItems = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_SendtoItems.CharSelectID);
            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

            ADD_OR_SUB SendToText;
            QString EQuipText = FBsizeOpcode2;
            QString FBText =  SendToText.TextSize(IN_SendtoItems.ClientsID,IN_SendtoItems.ClientsIP,IN_SendtoItems.ClientsPort,EQuipText,true,"460001000000",Servers_Last_FB_MessageNumber);

            IN_SendtoItems.SendFBsVector.append(FBText.toUtf8());
            IN_SendtoItems.FBReplyOrNotVector.append("00");
            IN_SendtoItems.FBPacketDescriptionVector.append(ItemStatus + ", ");

            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_SendtoItems);
            IN_SendtoItems = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
        }

        flipped sento;
        sento.SendOut(IN_SendtoItems.ClientsID, IN_SendtoItems.ClientsIP, IN_SendtoItems.ClientsPort);
    }

    return InventoryIsFull;
}

QByteArray Quests::SendtoTunar(QByteArray ID_IP_PORT, QString ResultID)
{
    packetparsing::packetvars IN_SendtoTunar;
    IN_SendtoTunar = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    QByteArray Packet = "";

    //qDebug() << "ResultID" << ResultID;

    QString Tunar = ResultID.mid(3,ResultID.size());

    ADD_OR_SUB ConvertBack;
    QString BackToDec = ConvertBack.BackToDec(IN_SendtoTunar.Cash_on_hand);
    int Cash_on_hand1 = BackToDec.toInt(nullptr,10);

    int newTotal = 0;
    int TunarAmount = Tunar.toInt(nullptr,10);

    QString Text = "";

    if(ResultID.contains("TG_"))
    {
        newTotal = Cash_on_hand1 - TunarAmount;
        Text = "You have given away " + QString::number(TunarAmount) + " tunar.";
    }

    if(ResultID.contains("TR_"))
    {
        newTotal = Cash_on_hand1 + TunarAmount;
        Text = "You have received " + QString::number(TunarAmount) + " tunar.";
    }

    QString Cash_on_hand2 = QString("%1").arg(newTotal,2,10,QLatin1Char('0')).toUpper();
    Convert sendto;
    IN_SendtoTunar.Cash_on_hand = sendto.ConvertTo(Cash_on_hand2);

    QString FBsizeOpcode = "5200" + IN_SendtoTunar.Cash_on_hand.toUtf8();

    ADD_OR_SUB CurrentItem;
    QString NewFB = CurrentItem.FBSize(FBsizeOpcode);

    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_SendtoTunar.CharSelectID);
    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

    increment Cash1;
    QByteArray Cash2 = Cash1.count(Servers_Last_FB_MessageNumber);
    int index = NewFB.indexOf("XXXX");
    NewFB.replace(index,4,Cash2);
    Servers_Last_FB_MessageNumber = Cash2.toUpper();

    Packet = NewFB.toUtf8();

    //---------------------------------------------------------------------------------------------------------------

    ADD_OR_SUB SendToText5;
    QString FBText5 =  SendToText5.TextSize(IN_SendtoTunar.ClientsID,IN_SendtoTunar.ClientsIP,IN_SendtoTunar.ClientsPort,Text,false,"7A0A",Servers_Last_FB_MessageNumber);

    Servers_Last_FB_MessageNumber = FBText5.mid(0,4).toUtf8();
    FBText5.remove(0,4);

    MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
    Opcodes::CharID_ServerMessageNumbers.insert(IN_SendtoTunar.CharSelectID,MessNumbVect);

    Packet.append(FBText5.toUtf8());

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_SendtoTunar);

    return Packet;
}


void Quests::SendoutPacket(QByteArray ID_IP_PORT,QByteArray FullPacket)
{
    packetparsing::packetvars IN_SendoutPacket;
    IN_SendoutPacket = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    QByteArray Send_FB = "";

    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_SendoutPacket.CharSelectID);
    QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

    increment Resending;
    QByteArray Resending2 = Resending.count(Servers_Last_MessageNumber);
    Servers_Last_MessageNumber = Resending2.toUpper();

    MessNumbVect.replace(0,Servers_Last_MessageNumber);

    Opcodes::CharID_ServerMessageNumbers.insert(IN_SendoutPacket.CharSelectID,MessNumbVect);

    Send_FB = IN_SendoutPacket.serverid1 + IN_SendoutPacket.clientid1 + "xxxx01BF543213" + IN_SendoutPacket.SessionID + "03" +
            Servers_Last_MessageNumber + IN_SendoutPacket.Clients_Last_MessageNumber + IN_SendoutPacket.Clients_Last_FB_MessageNumber + FullPacket;

    packetsize ResizePacket; //call size class
    QByteArray ResizePacket2 = ResizePacket.switchsize(Send_FB); //get new size
    Send_FB.replace(8,4,ResizePacket2);

    crc sendcrc;
    QByteArray outcrc =  sendcrc.elcrc(Send_FB);
    Send_FB.append(outcrc);

    QByteArray Damage_out = Send_FB.fromHex(Send_FB);

    QHostAddress current_client_address;
    current_client_address.setAddress(QString::fromUtf8(IN_SendoutPacket.ClientsIP));
    quint16 current_client_port = IN_SendoutPacket.ClientsPort.toUShort(nullptr,10);
    QString changeIP = current_client_address.toString();

    QHostAddress newsender = QHostAddress(changeIP);
    worldthread::udpsocket3->writeDatagram(Damage_out,newsender,current_client_port);

    QString MainToon = checkthezone::NPCsNames.value(IN_SendoutPacket.CharSelectID);


    QDateTime dateTime = dateTime.currentDateTime();
    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

    //qDebug() << "";
    //qDebug() << "";
    //qDebug() << "----------------------------------------------------------";
    //qDebug() << dateTimeString;
    //qDebug() << "Quest Related";
    //qDebug() << "From Client: " + MainToon;
    //qDebug() << "IP: " + changeIP;
    //qDebug() << "Port: " + QString::number(current_client_port,10);
    //qDebug() << "To Server:";
    //qDebug() << QString::fromUtf8(Send_FB);
    //qDebug() << "----------------------------------------------------------";
    //qDebug() << "";

}


QByteArray Quests::QuestMenus(QByteArray ID_IP_PORT, QByteArray Clients_Last_MessageNumber, QByteArray Clients_Last_FB_MessageNumber, QByteArray opholder, QString QuestsID, QString NPCsID)
{
    SendPacketOut.clear();

    packetparsing::packetvars IN_QuestMenus;
    IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    //qDebug() << "IN_QuestMenus.QuestsID" << QuestsID;

    QVector<QString> optionVect;
    QString MenusReturn = "";
    QString PopUPsReturn = "";
    QString XP = "";
    QMap<int,QVector<QString>> tempvectmap;
    QVector<QString> ResultsVector;
    QVector<QString> PocketCheckVector;

    bool  InventoryIsFull = false;

    int client_return1 = opholder.mid(20,2).toInt(nullptr,16);

    //qDebug() << "client_return1" << client_return1;

    if(QuestsID.contains("SM_"))
    {
        NPCsID = QuestsID.mid(3,11);

        optionVect = Quests::SpiritmastersID_MenuMap.value(NPCsID);

        //qDebug() << "IN_QuestMenus.QuestsID1" << QuestsID;
    }
    else
    {

        //qDebug() << "IN_QuestMenus.CurrentQuestMenuID_9" << IN_QuestMenus.CurrentQuestMenuID;

        //        if(IN_QuestMenus.WorldGreetingFlag == true)
        //        {
        //            IN_QuestMenus.WorldGreetingFlag = false;
        //            IN_QuestMenus.CurrentQuestMenuID = "M_01000000";

        //            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);
        //            IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
        //        }

        //qDebug() << "IN_QuestMenus.CurrentQuestMenuID_10" << IN_QuestMenus.CurrentQuestMenuID;

        QMap<QString,QVector<QString>> OptionsIDs_ResultsVectorMap = Quests::MenuID_OptionsIDs_ResultsVectorMap.value(IN_QuestMenus.CurrentQuestMenuID);

        //qDebug() << "OptionsIDs_ResultsVectorMap" << OptionsIDs_ResultsVectorMap;



        QVector<QString> tempMenuVect = Quests::InGame_MenuesVectorMap.value(IN_QuestMenus.CurrentQuestMenuID);

        //qDebug() << "tempMenuVect" << tempMenuVect;
        //qDebug() << "Quests::InGame_MenuesVectorMap" << Quests::InGame_MenuesVectorMap;



        int counter = 0;

        if(tempMenuVect.isEmpty() == false)
        {
            for(int re = 1; re < tempMenuVect.size(); re+=2)
            {
                QString OptionID = tempMenuVect.at(re);
                //qDebug() << "OptionID" << OptionID;

                ResultsVector = OptionsIDs_ResultsVectorMap.value(OptionID);

                tempvectmap.insert(counter,ResultsVector);
                counter++;
            }
        }



        //qDebug() << "tempvectmap" << tempvectmap;



        if(client_return1 == 0)
        {
            //qDebug() << "IN_client_return1.NPCsID" << NPCsID;

            QMap<QString,QMap<int,QVector<QString>>> OP_Flag_CheckVectorMaps = Quests::QGID_OP_Flag_CheckVectorMaps.value(NPCsID);
            QMap<int,QVector<QString>> Flag_CheckVectorMap;
            QVector<QString> CheckVector;

            //----------------------------------------------------------------
            Flag_CheckVectorMap = OP_Flag_CheckVectorMaps.value("8");//IC

            //qDebug() << "IN_client_return1.IN_QuestMenus.MainFlag" << IN_QuestMenus.MainFlag;
            //qDebug() << "IN_client_return1.Flag_CheckVectorMap" << Flag_CheckVectorMap;

            if(Flag_CheckVectorMap.contains(IN_QuestMenus.MainFlag))
            {
                CheckVector = Flag_CheckVectorMap.value(IN_QuestMenus.MainFlag);
            }
            //----------------------------------------------------------------
            Flag_CheckVectorMap = OP_Flag_CheckVectorMaps.value("32");//TC

            if(Flag_CheckVectorMap.contains(IN_QuestMenus.MainFlag))
            {
                QVector<QString> PopupVect = Flag_CheckVectorMap.value(IN_QuestMenus.MainFlag);

                if(PopupVect.isEmpty() == false)
                {
                    QString Popup = PopupVect.at(0);

                    if(!CheckVector.contains(Popup))
                    {
                        CheckVector.append(Popup);
                    }
                }
            }
            //----------------------------------------------------------------

            //qDebug() << "IN_client_return1.CheckVector" << CheckVector;

            if(CheckVector.isEmpty() == false)
            {
                CheckVector.insert(0,NPCsID);

                //qDebug() << "CheckVector.size()B" << CheckVector.size();

                Quests sendto;
                PocketCheckVector = sendto.CheckYoPockets(ID_IP_PORT,CheckVector);

                IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                //qDebug() << "IN_QuestMenus.PocketCheckVector.size()A" << PocketCheckVector.size();

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
        }
    }

    //qDebug() << "IN_QuestMenus.PocketCheckVector" << PocketCheckVector;

    if(PocketCheckVector.isEmpty() == true)
    {
        //qDebug() << "IN_QuestMenus.QuestsID2" << QuestsID;

        if(QuestsID.contains("SM_"))
        {
            //qDebug() << "IN_QuestMenus.client_return2" << client_return1;

            if(client_return1 == 0)
            {
                ResultsVector.append("P_SM_" + optionVect.at(3));

                //qDebug() << "IN_QuestMenus.ResultsVector" << ResultsVector;

                QMap<int,QString> PacketID_Location = Quests::SpiritMastersID_PacketNumber_LocationMaps.value(NPCsID);

                QString OpcodeAndValue = "";

                QMapIterator<int,QString> iter(PacketID_Location);
                while(iter.hasNext())
                {
                    iter.next();

                    OpcodeAndValue = iter.value();
                }

                //TS_279E0D00$T#55B9F308

                QString ResultID1 = "TS_" + NPCsID + "$T#55B9F308";

                QString TargetSpellReturn = SendoutSpell(ID_IP_PORT,ResultID1);
                SendPacketOut.append(TargetSpellReturn.toUtf8());

                //qDebug() << "TargetSpellReturn" << TargetSpellReturn;

                IN_QuestMenus.BindLocation = OpcodeAndValue;

                if(IN_QuestMenus.packetid == 0)
                {
                    IN_QuestMenus.packetid = Opcodes::ID_PacketNumber_Map.value(IN_QuestMenus.CharSelectID).toInt(nullptr,10);
                }

                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);

                IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                QSqlQuery query;

                db.transaction();

                QString Table_Name = "MAINTOON_STARTING_LOCATIONS";
                QString Column_Name = "BindLocation";
                query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
                query.bindValue(":val",IN_QuestMenus.BindLocation);
                query.bindValue(":id",IN_QuestMenus.packetid);
                query.exec();

                if(!query.exec())
                {
                    return "Fail_MAINTOON = " + Column_Name.toUtf8();
                }

                db.commit();
            }

            if(client_return1 == 1)
            {
                ResultsVector.append("P_SM_" + optionVect.at(4));
            }
        }
        else
        {
            ResultsVector = tempvectmap.value(client_return1);
        }

        //qDebug() << "ResultsVector.size()" << ResultsVector.size();
        //qDebug() << "ResultsVector1" << ResultsVector;
        //qDebug() << "ResultsVector4" << ResultsVector;

        QString CurrentQuestMenuID = IN_QuestMenus.CurrentQuestMenuID;
        QString ItemsID = "";
        QString Quantity = "";
        QString TunarQty = "";

        //qDebug() << "IN_QuestMenus.MenuID_QuestID2" << IN_QuestMenus.MenuID_QuestID;

        for(int re = 0; re < ResultsVector.size(); re++)
        {
            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);

            IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

            QString ResultID = ResultsVector.at(re);

            //qDebug() << "ResultID" << ResultID;


            if(ResultID.contains("M_") && !ResultID.contains("SM_"))
            {
                IN_QuestMenus.CurrentQuestMenuID = ResultID;
                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);

                MenusReturn = SendoutMenus(ID_IP_PORT,ResultID);
                SendPacketOut.append(MenusReturn.toUtf8());

                IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
            }

            if(ResultID.contains("P_"))
            {
                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);

                PopUPsReturn = SendPopUps(ID_IP_PORT,ResultID);
                SendPacketOut.append(PopUPsReturn.toUtf8());

                IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
            }

            if(ResultID.contains("IG_") || ResultID.contains("IR_"))
            {
                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);

                InventoryIsFull = SendtoItems(ID_IP_PORT,ResultID);

                IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
            }

            if(ResultID.contains("TR_") || ResultID.contains("TG_"))
            {
                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);

                QByteArray Tunar = SendtoTunar(ID_IP_PORT,ResultID);
                SendPacketOut.append(Tunar);

                IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
            }

            if(ResultID.contains("xp_"))
            {
                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);

                XP = GenerateXP(ID_IP_PORT,ResultID);

                IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
            }

            if(ResultID.contains("sp_"))
            {

            }

            if(ResultID.contains("TS_"))
            {
                QString NPCsID = ResultID.mid(3,11);

                QString TargetSpellReturn = SendoutSpell(ID_IP_PORT,ResultID);
                SendPacketOut.append(TargetSpellReturn.toUtf8());

                QMap<int,QString> PacketID_Location = Quests::SpiritMastersID_PacketNumber_LocationMaps.value(NPCsID);

                QString OpcodeAndValue = "";

                QMapIterator<int,QString> iter(PacketID_Location);
                while(iter.hasNext())
                {
                    iter.next();

                    OpcodeAndValue = iter.value();
                }

                IN_QuestMenus.BindLocation = OpcodeAndValue;

                if(IN_QuestMenus.packetid == 0)
                {
                    IN_QuestMenus.packetid = Opcodes::ID_PacketNumber_Map.value(IN_QuestMenus.CharSelectID).toInt(nullptr,10);
                }

                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);
                IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                QSqlQuery query;

                db.transaction();

                QString Table_Name = "MAINTOON_STARTING_LOCATIONS";
                QString Column_Name = "BindLocation";
                query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
                query.bindValue(":val",IN_QuestMenus.teleport_destination);
                query.bindValue(":id",IN_QuestMenus.packetid);
                query.exec();

                if(!query.exec())
                {
                    return "Fail_MAINTOON = " + Column_Name.toUtf8();
                }

                db.commit();

                //qDebug() << "TargetSpellReturn" << TargetSpellReturn;
            }

            if(ResultID.contains("QLa_"))
            {
                //qDebug() << "in_QLa_";

                ResultID.remove(0,4);
                QByteArray QuestLog = AddQuestLog(ID_IP_PORT,ResultID);

                SendPacketOut.append(QuestLog);

                IN_QuestMenus.MainFlag++;

                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);
                IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                //qDebug() << "QuestsID" << QuestsID;
                //qDebug() << "QuestLogID" << ResultID;
                //qDebug() << "QuestLog" << QuestLog;


                //qDebug() << "IN_QuestMenus.MainFlag_QLa" << IN_QuestMenus.MainFlag;
            }

            if(ResultID.contains("QLr_"))
            {
                if(InventoryIsFull == false)
                {
                    ResultID.remove(0,4);
                    QByteArray QuestLog = RemoveQuestLog(ID_IP_PORT,ResultID);

                    if(QuestLog.isEmpty() == false)
                    {
                        SendPacketOut.append(QuestLog);
                    }

                    IN_QuestMenus.MainFlag++;

                    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);
                    IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                }

                //qDebug() << "IN_QuestMenus.MainFlag_QLr" << IN_QuestMenus.MainFlag;
            }

            if(ResultID.contains("MC_"))
            {
                IN_QuestMenus.ModelTimer = 4000;
                IN_QuestMenus.ModelResult = ResultID;
                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);
                IN_QuestMenus = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

            }
        }
    }


    //qDebug() << "IN_QuestMenus.CurrentQuestMenuID_11" << IN_QuestMenus.CurrentQuestMenuID;

    //qDebug() << "IN_QuestMenus.SendPacketOut" << SendPacketOut;

    QVector<QByteArray> TempSendVect;

    if(SendPacketOut.isEmpty() == false)
    {
        QByteArray NextPacket = "";
        QByteArray NextOp = "";
        int SizeNP = 0;
        int re = 0;

        for(re = 0; re < SendPacketOut.size(); re++)
        {
            //qDebug() << "A " <<  re << " of " << SendPacketOut.size();

            NextOp = SendPacketOut.at(re);

            if(NextPacket.size() + NextOp.size() <= 2374)
            {
                NextPacket.append(NextOp);
                SizeNP = NextPacket.size();

                //qDebug() << "B " <<  re << " of " << SendPacketOut.size();

                //qDebug() << "IN_QuestMenus.re_1" << re;
                //qDebug() << "IN_QuestMenus.SizeNP_1" << SizeNP;
                //qDebug() << "IN_QuestMenus.NextPacket_1" << NextPacket.size();
                //qDebug() << "IN_QuestMenus.NextOp_1" << NextOp;
            }
            else
            {
                TempSendVect.append(NextPacket);

                SizeNP = 2375;

                NextPacket = NextOp;

                //qDebug() << "";
                //qDebug() << "IN_QuestMenus.Not_Added";
                //qDebug() << "IN_QuestMenus.re_2" << re;
                //qDebug() << "IN_QuestMenus.SizeNP_2" << SizeNP;
                //qDebug() << "IN_QuestMenus.NextPacket_2" << NextPacket.size();
                //qDebug() << "IN_QuestMenus.NextOp_2" << NextOp;
            }

            //qDebug() << "C " << re << " of " << SendPacketOut.size();
        }

        if((TempSendVect.isEmpty() == true && re == SendPacketOut.size())
                || (!TempSendVect.contains(NextPacket)))
        {
            TempSendVect.append(NextPacket);
        }

        //qDebug() << "D " <<  re << " of " << SendPacketOut.size();

        //qDebug() << "IN_QuestMenus.TempSendVect" << TempSendVect;

        for(int re = 0; re < TempSendVect.size(); re++)
        {
            QByteArray NextPacket = TempSendVect.at(re);

            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_QuestMenus.CharSelectID);
            QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);

            increment GrpOut;
            QByteArray GrpOut_a = GrpOut.count(Servers_Last_MessageNumber);

            Servers_Last_MessageNumber = GrpOut_a.toUpper();

            MessNumbVect.replace(0,Servers_Last_MessageNumber);

            Opcodes::CharID_ServerMessageNumbers.insert(IN_QuestMenus.CharSelectID,MessNumbVect);

            QByteArray Send_FB = IN_QuestMenus.serverid1 + IN_QuestMenus.clientid1 + "xxxx01BF543213" + IN_QuestMenus.SessionID + "03" +
                    Servers_Last_MessageNumber + Clients_Last_MessageNumber + Clients_Last_FB_MessageNumber + NextPacket;

            packetsize ResizePacket; //call size class
            QByteArray ResizePacket2 = ResizePacket.switchsize(Send_FB); //get new size
            Send_FB.replace(8,4,ResizePacket2);

            crc sendcrc;
            QByteArray outcrc =  sendcrc.elcrc(Send_FB);
            Send_FB.append(outcrc);

            QByteArray Send_FB_out = Send_FB.fromHex(Send_FB);

            QHostAddress current_client_address;
            current_client_address.setAddress(QString::fromUtf8(IN_QuestMenus.ClientsIP));
            quint16 current_client_port = IN_QuestMenus.ClientsPort.toUShort(nullptr,10);
            QString changeIP = current_client_address.toString();

            QHostAddress newsender = QHostAddress(changeIP);
            worldthread::udpsocket3->writeDatagram(Send_FB_out,newsender,current_client_port);

            QString MainToon = checkthezone::NPCsNames.value(IN_QuestMenus.CharSelectID);

            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zz:a");

            //qDebug() << "";
            //qDebug() << "";
            //qDebug() << "----------------------------------------------------------";
            //qDebug() << dateTimeString;
            //qDebug() << "Menus_and_popups " << re+1;
            //qDebug() << "To Client: " + MainToon;
            //qDebug() << "IN_QuestMenus.CharSelectID = " << IN_QuestMenus.CharSelectID;
            //qDebug() << "IP: " + IN_QuestMenus.ClientsIP;
            //qDebug() << "Port: " + IN_QuestMenus.ClientsPort;
            //qDebug() << "From Server: ";
            //qDebug() << Send_FB;
            //qDebug() << "----------------------------------------------------------";
            //qDebug() << "";
        }
    }

    //qDebug() << "IN_QuestMenus.CurrentQuestMenuID_12" << IN_QuestMenus.CurrentQuestMenuID;

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_QuestMenus);

    return "";

}







QByteArray Quests::WelcomeGreetings(QByteArray ID_IP_PORT)
{
    packetparsing::packetvars IN_WelcomeGreetings;
    IN_WelcomeGreetings = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    if(IN_WelcomeGreetings.CharSelectID.isEmpty() == false)
    {
        QString LargeString = "";

        IN_WelcomeGreetings.WorldGreetingFlag = false;

        QVector<QString>MenuTextVect;

        QMap<QString,QString> StartingCity_WelcomePopUp =
        {
            {"Freeport","P_0A000000"},{"Qeynos","P_0B000000"},{"Surefall Glade","P_0C000000"},
            {"Fayspires","P_0D000000"},{"Tethelin","P_0E000000"},{"Neriak","P_0F000000"},
            {"Klick'Anon","P_10000000"},{"Moradhim","P_11000000"},{"Grobb","P_12000000"},
            {"Halas","P_13000000"},{"Rivervale","P_14000000"},{"Highbourne","P_15000000"},
            {"Oggok","P_16000000"}
        };

        QString StartingCity = CharCreate::ID_StartingCityMap.value(IN_WelcomeGreetings.CharSelectID);

        //qDebug() << "";
        //qDebug() << "IN_WelcomeGreetings.CharSelectID" << IN_WelcomeGreetings.CharSelectID;

        //qDebug() << "IN_WelcomeGreetings.StartingCity" << StartingCity;

        IN_WelcomeGreetings.QuestPopUpID = StartingCity_WelcomePopUp.value(StartingCity);

        //qDebug() << "IN_WelcomeGreetings.QuestPopUpID" << IN_WelcomeGreetings.QuestPopUpID;

        QVector<QString> ResultsVector;

        ResultsVector.append(IN_WelcomeGreetings.QuestPopUpID);
        ResultsVector.append("P_09000000");

        QVector<QString> ResultTextVector;

        for(int re = 0; re < ResultsVector.size(); re++)
        {
            QString PopUpID = ResultsVector.at(re);

            QString PopUpText = Quests::PopUpID_PopUpText_Map.value(PopUpID);

            ResultTextVector.append(PopUpText);
        }

        QByteArray Send_FB = "";

        QByteArray PopUp_FB = "";

        packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_WelcomeGreetings);

        for(int send = 0; send < ResultTextVector.size(); send++)
        {
            IN_WelcomeGreetings = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_WelcomeGreetings.CharSelectID);
            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

            QString Text = ResultTextVector.at(send);
            QString Opcode = "4600xxxx0000";

            QString ClassName = ClassValueToString.value(IN_WelcomeGreetings.Class);

            //qDebug() << "IN_WelcomeGreetings.Class" << IN_WelcomeGreetings.Class;
            //qDebug() << "ClassName" << ClassName;

            QString StartingCity = CharCreate::ID_StartingCityMap.value(IN_WelcomeGreetings.CharSelectID);

            //qDebug() << "IN_WelcomeGreetings.StartingCity" << StartingCity;

            QMap<QString,QVector<QString>> ClassGreetings_VectorMap = StartingCity_WorldGreetings_VectorMap.value(StartingCity);

            QVector<QString> ClassStuffVector = ClassGreetings_VectorMap.value(ClassName);

            ClassStuffVector.append(StartingCity);

            //qDebug() << "IN_WelcomeGreetings.ClassStuffVector" << ClassStuffVector;


            //qDebug() << "IN_WelcomeGreetings.PlaceHolderStuffVector" << PlaceHolderStuffVector;

            for(int re = 0; re < PlaceHolderStuffVector.size(); re++)
            {
                QString FakeStuff = PlaceHolderStuffVector.at(re);

                //qDebug() << "IN_WelcomeGreetings.Text" << Text;

                if(Text.contains(FakeStuff))
                {
                    do
                    {
                        int index = Text.indexOf(FakeStuff);

                        QString RealStuff = ClassStuffVector.at(re);

                        //qDebug() << FakeStuff << "replaced with" << RealStuff;

                        Text.replace(index,FakeStuff.size(),RealStuff);

                    }while(Text.contains(FakeStuff));
                }
            }

            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_WelcomeGreetings);

            QByteArray ConvertedText = TestingOptions(Opcode, Text, Servers_Last_FB_MessageNumber,ID_IP_PORT);

            IN_WelcomeGreetings = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

            Servers_Last_FB_MessageNumber = ConvertedText.mid(0,4);

            MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

            Opcodes::CharID_ServerMessageNumbers.insert(IN_WelcomeGreetings.CharSelectID,MessNumbVect);

            ConvertedText.remove(0,4);

            Send_FB.append(ConvertedText);
        }

        PopUp_FB = Send_FB;

        Send_FB.clear();

        IN_WelcomeGreetings.CurrentQuestMenuID = "M_01000000";

        MenuTextVect = InGame_MenuesVectorMap.value(IN_WelcomeGreetings.CurrentQuestMenuID);

        if(MenuTextVect.isEmpty() == false)
        {
            LargeString = MenuTextVect.at(0);
        }

        QString HeaderText = LargeString;

        increment send;
        IN_WelcomeGreetings.QuestMenuCounter = send.count(IN_WelcomeGreetings.QuestMenuCounter);

        QString Opcode = "3400" + IN_WelcomeGreetings.QuestMenuCounter + "0000";

        QByteArray ConvertedHeader = MenuHeaderConverter(HeaderText);

        ////qDebug() << "ConvertedHeaderText" << ConvertedHeader;

        QString OptionCount = QString("%1").arg((MenuTextVect.size()-1)/2,2,16,QLatin1Char('0')).toUpper();

        Send_FB.append(Opcode.toUtf8() + ConvertedHeader + OptionCount.toUtf8());

        QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_WelcomeGreetings.CharSelectID);
        QByteArray Servers_Last_MessageNumber = MessNumbVect.at(0);
        QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

        increment Resending;
        QByteArray Resending2 = Resending.count(Servers_Last_MessageNumber);
        Servers_Last_MessageNumber = Resending2.toUpper();

        Resending2 = Resending.count(Servers_Last_FB_MessageNumber);
        Servers_Last_FB_MessageNumber = Resending2.toUpper();

        MessNumbVect.replace(0,Servers_Last_MessageNumber);
        MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

        Opcodes::CharID_ServerMessageNumbers.insert(IN_WelcomeGreetings.CharSelectID,MessNumbVect);

        for(int send = 1; send < MenuTextVect.size(); send+=2)
        {
            QString OptionText = MenuTextVect.at(send+1);

            QByteArray ConvertedText = MenuHeaderConverter(OptionText);

            ////qDebug() << "ConvertedOptionsText" << ConvertedText;

            Send_FB.append(ConvertedText);
        }

        QByteArray ConvertedText = TestingMenus(Send_FB);

        int index = ConvertedText.indexOf("yyyy");
        ConvertedText.replace(index,4,Servers_Last_FB_MessageNumber);

        Send_FB.clear();

        ConvertedText.insert(0,PopUp_FB);

        Send_FB = IN_WelcomeGreetings.serverid1 + IN_WelcomeGreetings.clientid1 + "xxxx01BF543213" + IN_WelcomeGreetings.SessionID + "00" +
                Servers_Last_MessageNumber + ConvertedText;

        packetsize ResizePacket; //call size class
        QByteArray ResizePacket2 = ResizePacket.switchsize(Send_FB); //get new size
        Send_FB.replace(8,4,ResizePacket2);

        //qDebug() << "IN_WelcomeGreetings.CurrentQuestMenuID_1" << IN_WelcomeGreetings.CurrentQuestMenuID;

        crc sendcrc;
        QByteArray outcrc =  sendcrc.elcrc(Send_FB);
        Send_FB.append(outcrc);


        QByteArray Damage_out = Send_FB.fromHex(Send_FB);

        QHostAddress current_client_address;
        current_client_address.setAddress(QString::fromUtf8(IN_WelcomeGreetings.ClientsIP));
        quint16 current_client_port = IN_WelcomeGreetings.ClientsPort.toUShort(nullptr,10);
        QString changeIP = current_client_address.toString();

        QHostAddress newsender = QHostAddress(changeIP);
        worldthread::udpsocket3->writeDatagram(Damage_out,newsender,current_client_port);

        packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_WelcomeGreetings);


        return "yo mama";
    }
}

QByteArray Quests::TestingOptions(QString Opcode, QString Text, QByteArray Servers_Last_FB_MessageNumber, QByteArray ID_IP_PORT)
{
    packetparsing::packetvars IN_TestingOptions;
    IN_TestingOptions = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    QByteArray Counter = "";

    if(Opcode == "4600xxxx0000")
    {
        Counter = IN_TestingOptions.QuestPopUpCounter;

        increment send;
        QByteArray rotate = send.count(Counter);
        Counter = rotate;

        IN_TestingOptions.QuestPopUpCounter = Counter;
    }






    int index = Opcode.indexOf("xxxx");
    Opcode.replace(index,4,Counter);

    QByteArray change1 = Text.toUtf8();
    QByteArray   textascii2 = change1.toHex();
    QByteArray textascii = textascii2;

    for (int cc = 0; cc < textascii2.size() * 2; cc+=2)
    {
        textascii.insert(cc + 2,"00");
        cc += 2;
    }

    textascii.insert(0,"FF000000");


    int insidehexcount =  (textascii.count() /2 - 4) / 2;
    textascii.replace(0,2,QString("%1").arg(insidehexcount,2,16,QLatin1Char('0')).toUtf8());

    textascii.insert(0,Opcode.toUtf8());

    int text_size3 = textascii.size();
    text_size3 = text_size3 / 2;
    QString text_size1 = QString("%1").arg(text_size3,2,16,QLatin1Char('0')).toUpper();
    int text_size = text_size1.size();

    increment Resening;
    QByteArray Resening2 = Resening.count(Servers_Last_FB_MessageNumber);
    Servers_Last_FB_MessageNumber = Resening2.toUpper();

    QByteArray FBText = "";


    if(text_size == 2)
    {
        QByteArray FB_Header = "FBXX" + Servers_Last_FB_MessageNumber;

        FB_Header.replace(2,2,text_size1.toUtf8().toUpper());
        FBText.append(FB_Header + textascii);
    }

    if(text_size == 3)
    {
        text_size1.insert(0,"0");
        QByteArray FB_Header = "FBFFXXXX" + Servers_Last_FB_MessageNumber;

        QString Split1 = text_size1.mid(0,2);
        QString Split2 = text_size1.mid(2,2);
        text_size1 = Split2 + Split1;

        FB_Header.replace(4,4,text_size1.toUtf8().toUpper());
        FBText.append(FB_Header + textascii);
    }

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_TestingOptions);

    return Servers_Last_FB_MessageNumber + FBText;

}

QByteArray Quests::MenuHeaderConverter(QString Text)
{
    QByteArray change1 = Text.toUtf8();
    QByteArray   textascii2 = change1.toHex();
    QByteArray textascii = textascii2;

    for (int cc = 0; cc < textascii2.size() * 2; cc+=2)
    {
        textascii.insert(cc + 2,"00");
        cc += 2;
    }

    textascii.insert(0,"FF000000");


    int insidehexcount =  (textascii.count() /2 - 4) / 2;
    textascii.replace(0,2,QString("%1").arg(insidehexcount,2,16,QLatin1Char('0')).toUtf8());


    return textascii;
}

QByteArray Quests::TestingMenus(QString Text)
{
    int text_size3 = Text.size();
    text_size3 = text_size3 / 2;
    QString text_size1 = QString("%1").arg(text_size3,2,16,QLatin1Char('0')).toUpper();
    int text_size = text_size1.size();


    QByteArray FBText = "";

    //qDebug() << "text_size" << text_size;

    if(text_size == 2)
    {
        QByteArray FB_Header = "FBXXyyyy";

        FB_Header.replace(2,2,text_size1.toUtf8().toUpper());
        FBText.append(FB_Header + Text.toUtf8());
    }

    if(text_size == 3)
    {
        text_size1.insert(0,"0");
        QByteArray FB_Header = "FBFFXXXXyyyy";

        QString Split1 = text_size1.mid(0,2);
        QString Split2 = text_size1.mid(2,2);
        text_size1 = Split2 + Split1;

        FB_Header.replace(4,4,text_size1.toUtf8().toUpper());
        FBText.append(FB_Header + Text.toUtf8());
    }


    return FBText;
}


//Receive XP
QByteArray Quests::GenerateXP(QByteArray ID_IP_PORT,QString ResultID)
{
    packetparsing::packetvars IN_GenerateXP;
    IN_GenerateXP = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    int Total = 0;

    IN_GenerateXP.XPperKill = ResultID.mid(3,ResultID.size()).toInt(nullptr,10);

    //qDebug() << "";
    //qDebug() << "";
    //qDebug() << "";
    //qDebug() << "IN_GenerateXP";
    //qDebug() << "IN_GenerateXP.XPperKill" << IN_GenerateXP.XPperKill;
    IN_GenerateXP.Level = IN_GenerateXP.CurrentLevel.toInt(nullptr,16)/2;

    //qDebug() << "IN_GenerateXP.CurrentLevel" << IN_GenerateXP.CurrentLevel;
    //qDebug() << "IN_GenerateXP.Level" << IN_GenerateXP.Level;

    QString XPperKill1 = QString("%1").arg(IN_GenerateXP.XPperKill,1,10,QLatin1Char('0'));

    //qDebug() << "XPperKill1" << XPperKill1;

    Total = IN_GenerateXP.XPperKill + IN_GenerateXP.XPBar;

    //qDebug() << "IN_GenerateXP.XPBar" << IN_GenerateXP.XPBar;

    //qDebug() << "Total" << Total;

    QString Total1 = QString("%1").arg(Total,1,10,QLatin1Char('0')).toUpper();

    //qDebug() << "Total1" << Total1;

    IN_GenerateXP.XPBar = Total;

    //qDebug() << "IN_GenerateXP.XPBar" << IN_GenerateXP.XPBar;

    ////////////////////////////////////////////////////////////////


    QByteArray dumMy1 = "";

    Convert SendTo;

    Total1 = SendTo.ConvertTo(Total1);

    QString CurrentXP = QString("%1").arg(IN_GenerateXP.XPperKill,1,10,QLatin1Char('0')).toUpper();

    //qDebug() << "CurrentXP" << CurrentXP;

    QString FBsizeOpcode = "You receive " + CurrentXP + " XP.";

    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_GenerateXP.CharSelectID);
    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

    ADD_OR_SUB SendToText;
    QString EQuipText = FBsizeOpcode;
    QString FBText =  SendToText.TextSize(IN_GenerateXP.ClientsID,IN_GenerateXP.ClientsIP,IN_GenerateXP.ClientsPort,EQuipText,true,"7A0A",Servers_Last_FB_MessageNumber);

    IN_GenerateXP.SendFBsVector.append(FBText.toUtf8());
    IN_GenerateXP.FBReplyOrNotVector.append("00");
    IN_GenerateXP.FBPacketDescriptionVector.append("XP, ");

    IN_GenerateXP.XPNextLevel = XPtoNextLevelMap.value(IN_GenerateXP.Level);

    //qDebug() << "IN_GenerateXP.XPNextLevel" << IN_GenerateXP.XPNextLevel;

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_GenerateXP);

    IN_GenerateXP = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    if(IN_GenerateXP.XPBar >= IN_GenerateXP.XPNextLevel)
    {
        QString New42ec = "";

        do
        {
            IN_GenerateXP = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

            Total = IN_GenerateXP.XPBar - IN_GenerateXP.XPNextLevel;
            IN_GenerateXP.XPBar = Total;

            //qDebug() << "IN_GenerateXP.XPBar" << IN_GenerateXP.XPBar;


            QString Total1 = QString("%1").arg(Total,1,10,QLatin1Char('0')).toUpper();

            int Level1 = IN_GenerateXP.Level + 1;
            IN_GenerateXP.Level = Level1;

            //qDebug() << "IN_GenerateXP.Level" << IN_GenerateXP.Level;


            //send to hp and pow... adjust c9 for hp FF and update 42EC..

            QString Level2 = QString("%1").arg(Level1,1,10,QLatin1Char('0')).toUpper();

            //qDebug() << "Level2" << Level2;

            IN_GenerateXP.XPNextLevel = XPtoNextLevelMap.value(IN_GenerateXP.Level);

            //qDebug() << "IN_GenerateXP.XPNextLevel" << IN_GenerateXP.XPNextLevel;


            QString XPNextLevel1 = QString("%1").arg(IN_GenerateXP.XPNextLevel,1,10,QLatin1Char('0')).toUpper();

            //qDebug() << "XPNextLevel1" << XPNextLevel1;

            QString CurrentLevel1 = QString("%1").arg(Level1,1,10,QLatin1Char('0')).toUpper();

            //qDebug() << "XPNextLevel1" << XPNextLevel1;

            FBsizeOpcode = "You have reached level " + CurrentLevel1 + "!";

            QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_GenerateXP.CharSelectID);
            QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

            ADD_OR_SUB SendToText;
            QString EQuipText = FBsizeOpcode;
            QString FBText =  SendToText.TextSize(IN_GenerateXP.ClientsID,IN_GenerateXP.ClientsIP,IN_GenerateXP.ClientsPort,EQuipText,true,"7A0A",Servers_Last_FB_MessageNumber);

            IN_GenerateXP.SendFBsVector.append(FBText.toUtf8());
            IN_GenerateXP.FBReplyOrNotVector.append("00");
            IN_GenerateXP.FBPacketDescriptionVector.append("Level_Up_Text, ");

            Convert SendTo;
            IN_GenerateXP.CurrentLevel = SendTo.ConvertTo(CurrentLevel1);
            Total1 = SendTo.ConvertTo(Total1);

            //qDebug() << "IN_GenerateXP.CurrentLevel" << IN_GenerateXP.CurrentLevel;

            //qDebug() << "Total1" << Total1;

            FBsizeOpcode = "2000" + IN_GenerateXP.CurrentLevel + Total1;
            IN_GenerateXP.SendFBsVector.append(FBsizeOpcode.toUtf8());
            IN_GenerateXP.FBReplyOrNotVector.append("00");
            IN_GenerateXP.FBPacketDescriptionVector.append("Update_XP, ");

            QByteArray current_c9 = generator::Players_Map.value(IN_GenerateXP.CharSelectID.toUtf8());
            QString Level3 = QString("%1").arg(Level1,2,16,QLatin1Char('0')).toUpper();
            current_c9.replace(358,2,Level3.toUtf8());



            generator::Players_Map.insert(IN_GenerateXP.CharSelectID.toUtf8(),current_c9);

            QString MyOldHPHexC9 = current_c9.mid(54,2);

            QString MyOldHP0 = IN_GenerateXP.StatsChange.mid(56,4);

            QString Byte1 = MyOldHP0.mid(0,2);
            QString Byte2 = MyOldHP0.mid(2,2);
            QString MyOldHP2 = Byte2 + Byte1;
            int MyOldHP = MyOldHP2.toInt(nullptr,16);
            QString MyOldHP1 = QString("%1").arg(MyOldHP,1,10,QLatin1Char('0')).toUpper();
            MyOldHP = MyOldHP1.toInt(nullptr,10);

            int NewCurrentHP = 0;
            int NewCurrentPOW = 0;
            int MyNewHP = -1;

            QString CurrentPOW1 = IN_GenerateXP.StatsChange.mid(72,4);
            QString POWByte1 = CurrentPOW1.mid(0,2);
            QString POWByte2 = CurrentPOW1.mid(2,2);
            QString reversePOW = POWByte2 + POWByte1;
            int CurrentPOW = reversePOW.toInt(nullptr,16);

            QString MaxPOW1 = IN_GenerateXP.StatsChange.mid(80,4);
            POWByte1 = MaxPOW1.mid(0,2);
            POWByte2 = MaxPOW1.mid(2,2);
            QString reverseMaxPOW = POWByte2 + POWByte1;
            int MaxPOW = reverseMaxPOW.toInt(nullptr,16);

            QString MyNewC9_hp3 = "";



            QString Current_HP = QString("%1").arg(MyNewHP,1,10,QLatin1Char('0')).toUpper();

            QString Current_POWMAX = QString("%1").arg(MaxPOW,1,10,QLatin1Char('0')).toUpper();

            QString Current_POW = QString("%1").arg(CurrentPOW,1,10,QLatin1Char('0')).toUpper();


            IN_GenerateXP.HoldOldHP = "YES";
            xorpacket SendTo42;
            New42ec = SendTo42.Recompress42EC(IN_GenerateXP.ClientsID, IN_GenerateXP.ClientsIP, IN_GenerateXP.ClientsPort, IN_GenerateXP.mylist,IN_GenerateXP.CharSelectID,IN_GenerateXP.StatsChange,IN_GenerateXP.HoldOldHP,NewCurrentHP,NewCurrentPOW,MyNewHP);

            QByteArray StatHeader = "42ECxxxx01";



            IN_GenerateXP.Send42ECsVector.append(StatHeader + New42ec.toUtf8());


            QString MyMaxHP0 = IN_GenerateXP.StatsChange.mid(64,4);
            Byte1 = MyMaxHP0.mid(0,2);
            Byte2 = MyMaxHP0.mid(2,2);
            QString MyMaxHP2 = Byte2 + Byte1;
            int MyMaxHP = MyMaxHP2.toInt(nullptr,16);
            QString MyMaxHP1 = QString("%1").arg(MyMaxHP,1,10,QLatin1Char('0')).toUpper();
            MyMaxHP = MyMaxHP1.toInt(nullptr,10);

            QString Current_HPMAX = QString("%1").arg(MyMaxHP,1,10,QLatin1Char('0')).toUpper();

            int MyCurrentHP = 0;
            double MySubHP1 = 0;
            double MyNewC9_hp1 = 0;

            MyCurrentHP = MyOldHP * 100;
            MySubHP1 =  MyCurrentHP / MyMaxHP;
            MyNewC9_hp1 = MySubHP1 * 2.55;

            int MyNewC9_hp4 = MyNewC9_hp1;

            MyNewC9_hp3 = QString("%1").arg(MyNewC9_hp4,2,16,QLatin1Char('0')).toUpper();


            current_c9 = generator::Players_Map.value(IN_GenerateXP.CharSelectID.toUtf8());
            current_c9.replace(54,2,MyNewC9_hp3.toUtf8());//-----------------------HP2

            generator::Players_Map.insert(IN_GenerateXP.CharSelectID.toUtf8(),current_c9);


            packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_GenerateXP);

            IN_GenerateXP = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

        }while(IN_GenerateXP.XPBar >= IN_GenerateXP.XPNextLevel);

        IN_GenerateXP.HoldOldHP = "";


        QString mainid4 = IN_GenerateXP.CharSelectID;
        generator packet4;
        QByteArray holdergen = packet4.maintoonswitch(mainid4,"",IN_GenerateXP.CharSelectID_Main);
        holdergen.insert(0,"00c9xxxx00");

        IN_GenerateXP.SendZeroChannelVector.append(holdergen);

        packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_GenerateXP);

        IN_GenerateXP = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
    }
    else
    {
        //qDebug() << "IN_GenerateXP.CurrentLevel" << IN_GenerateXP.CurrentLevel;
        //qDebug() << "Total1" << Total1;

        FBsizeOpcode = "2000" + IN_GenerateXP.CurrentLevel + Total1;
        IN_GenerateXP.SendFBsVector.append(FBsizeOpcode.toUtf8());
        IN_GenerateXP.FBReplyOrNotVector.append("00");
        IN_GenerateXP.FBPacketDescriptionVector.append("Update_XP, ");

        packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_GenerateXP);

        IN_GenerateXP = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
    }


    Total = IN_GenerateXP.XPperKill + IN_GenerateXP.TotalXp;
    IN_GenerateXP.TotalXp = Total;

    //qDebug() << "Total" << Total;
    //qDebug() << "IN_GenerateXP.TotalXp" << IN_GenerateXP.TotalXp;


    Total1 = QString("%1").arg(Total,1,10,QLatin1Char('0')).toUpper();

    //qDebug() << "Total1" << Total1;

    QString NextVal = "";

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_GenerateXP);

    IN_GenerateXP = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    for(int tp = 0; tp < ToltalXPperTPsVector.size(); tp++)
    {
        QString lastVal = ToltalXPperTPsVector.at(tp);
        int lastVal1 = 0;
        lastVal1 = lastVal.toInt(nullptr,10);
        qlonglong lastVal2 = 0;

        if(lastVal1 == 0)
        {
            lastVal2 = lastVal.toLongLong(nullptr,10);
        }

        if(tp < 461)
        {
            NextVal = ToltalXPperTPsVector.at(tp + 1);

            //qDebug() << "Next_TP_Val" << NextVal;

            int NextVal1 = 0;
            NextVal1 = NextVal.toInt(nullptr,10);
            qlonglong NextVal2 = 0;

            //qDebug() << "NextVal1" << NextVal1;

            if(NextVal1 == 0)
            {
                NextVal2 = NextVal.toLongLong(nullptr,10);
            }

            //qDebug() << "NextVal2" << NextVal2;

            if((IN_GenerateXP.TotalXp >= lastVal1 && IN_GenerateXP.TotalXp < NextVal1 && NextVal1 !=0) ||
                    (IN_GenerateXP.TotalXp >= lastVal2 && IN_GenerateXP.TotalXp < NextVal2 && lastVal2 != 0) ||
                    (IN_GenerateXP.TotalXp >= lastVal1 && IN_GenerateXP.TotalXp < NextVal2 && NextVal2 != 0 && lastVal1 != 0))
            {
                //send to hp and pow... adjust c9 for hp FF and update 42EC..

                int nextTPtotal = TPsPerToltalXPVector.at(tp);

                //qDebug() << "nextTPtotal" << nextTPtotal;

                QString Total1 = QString("%1").arg(nextTPtotal,1,10,QLatin1Char('0')).toUpper();

                Total = IN_GenerateXP.UnSpentTPs + IN_GenerateXP.SpentTPs;

                //qDebug() << "IN_GenerateXP.UnSpentTPs" << IN_GenerateXP.UnSpentTPs;
                //qDebug() << "IN_GenerateXP.SpentTPs" << IN_GenerateXP.SpentTPs;
                //qDebug() << "Total" << Total;

                int Total2 = nextTPtotal - Total;

                //qDebug() << "Total2" << Total2;

                if(Total2 > 0)
                {
                    QString NewTP = QString("%1").arg(Total2,1,10,QLatin1Char('0')).toUpper();

                    //qDebug() << "NewTP" << NewTP;

                    if(Total2 > 1)
                    {
                        FBsizeOpcode = "You have received " + NewTP + " training points!";
                    }
                    else
                    {
                        FBsizeOpcode = "You have received " + NewTP + " training point!";
                    }

                    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_GenerateXP.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    ADD_OR_SUB SendToText;
                    QString EQuipText = FBsizeOpcode;
                    QString FBText =  SendToText.TextSize(IN_GenerateXP.ClientsID,IN_GenerateXP.ClientsIP,IN_GenerateXP.ClientsPort,EQuipText,true,"460001000000",Servers_Last_FB_MessageNumber);

                    IN_GenerateXP.SendFBsVector.append(FBText.toUtf8());
                    IN_GenerateXP.FBReplyOrNotVector.append("00");
                    IN_GenerateXP.FBPacketDescriptionVector.append("Training_Point_Pop_Up, ");

                    int Total3 = Total2 + IN_GenerateXP.UnSpentTPs;

                    //qDebug() << "Total3" << Total3;

                    IN_GenerateXP.UnSpentTPs = Total3;

                    //qDebug() << "IN_GenerateXP.UnSpentTPs" << IN_GenerateXP.UnSpentTPs;

                    //                    Total1 = QString("%1").arg(Total3,1,10,QLatin1Char('0')).toUpper();

                    //                    Total1 = QString("%1").arg(Total,1,10,QLatin1Char('0')).toUpper();

                    Convert SendTo;
                    Total1 = SendTo.ConvertTo(NewTP);

                    //qDebug() << "Total1" << Total1;

                    ADD_OR_SUB Change;
                    QString ToNeg = Change.SubforNeg(Total1);

                    //qDebug() << "ToNeg" << ToNeg;

                    FBsizeOpcode = "1D00" + ToNeg;
                    IN_GenerateXP.SendFBsVector.append(FBsizeOpcode.toUtf8());
                    IN_GenerateXP.FBReplyOrNotVector.append("00");
                    IN_GenerateXP.FBPacketDescriptionVector.append("Receive_Training_Point(s), ");

                    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_GenerateXP);

                    IN_GenerateXP = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
                }

                break;
            }
        }
        else
        {
            if(IN_GenerateXP.TotalXp >= lastVal2)
            {
                int nextTPtotal = TPsPerToltalXPVector.at(tp);

                //qDebug() << "nextTPtotal" << nextTPtotal;

                QString Total1 = QString("%1").arg(nextTPtotal,1,10,QLatin1Char('0')).toUpper();

                //qDebug() << "Total1" << Total1;

                Total = IN_GenerateXP.UnSpentTPs + IN_GenerateXP.SpentTPs;

                //qDebug() << "IN_GenerateXP.UnSpentTPs" << IN_GenerateXP.UnSpentTPs;
                //qDebug() << "IN_GenerateXP.SpentTPs" << IN_GenerateXP.SpentTPs;
                //qDebug() << "Total" << Total;

                int Total2 = nextTPtotal - Total;

                //qDebug() << "Total2" << Total2;

                if(Total2 > 0)
                {
                    QString NewTP = QString("%1").arg(Total2,1,10,QLatin1Char('0')).toUpper();

                    //qDebug() << "NewTP" << NewTP;

                    if(Total2 > 1)
                    {
                        FBsizeOpcode = "You have received " + NewTP + " training points.";
                    }
                    else
                    {
                        FBsizeOpcode = "You have received " + NewTP + " training point.";
                    }


                    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_GenerateXP.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    ADD_OR_SUB SendToText;
                    QString EQuipText = FBsizeOpcode;
                    QString FBText =  SendToText.TextSize(IN_GenerateXP.ClientsID,IN_GenerateXP.ClientsIP,IN_GenerateXP.ClientsPort,EQuipText,true,"460001000000",Servers_Last_FB_MessageNumber);

                    IN_GenerateXP.SendFBsVector.append(FBText.toUtf8());
                    IN_GenerateXP.FBReplyOrNotVector.append("00");
                    IN_GenerateXP.FBPacketDescriptionVector.append("Training_Point_Pop_Up, ");


                    int Total3 = Total2 + IN_GenerateXP.UnSpentTPs;

                    //qDebug() << "Total3" << Total3;

                    IN_GenerateXP.UnSpentTPs = Total3;

                    //qDebug() << "IN_GenerateXP.UnSpentTPs" << IN_GenerateXP.UnSpentTPs;

                    Total1 = QString("%1").arg(Total3,1,10,QLatin1Char('0')).toUpper();

                    Convert SendTo;
                    Total1 = SendTo.ConvertTo(Total1);

                    //qDebug() << "Total1" << Total1;

                    ADD_OR_SUB Change;
                    QString ToNeg = Change.SubforNeg(NewTP);

                    //qDebug() << "ToNeg" << ToNeg;

                    FBsizeOpcode = "1D00" + ToNeg;
                    IN_GenerateXP.SendFBsVector.append(FBsizeOpcode.toUtf8());
                    IN_GenerateXP.FBReplyOrNotVector.append("00");
                    IN_GenerateXP.FBPacketDescriptionVector.append("Receive_Training_Point(s), ");

                    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_GenerateXP);

                    IN_GenerateXP = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

                }

                break;
            }
        }
    }



    //update db

    QString PacketNumber = Opcodes::ID_PacketNumber_Map.value(IN_GenerateXP.CharSelectID);
    int PacketNumber2 = PacketNumber.toInt(nullptr,10);
    IN_GenerateXP.packetid = PacketNumber2;

    IN_GenerateXP.StartXPbar = QString::number(IN_GenerateXP.XPBar);
    IN_GenerateXP.StartTotalXP = QString::number(IN_GenerateXP.TotalXp);

    IN_GenerateXP.StartLevel = QString("%1").arg(IN_GenerateXP.Level,2,16,QLatin1Char('0')).toUpper();

    if(db.isOpen())
    {
        db.close();
        db.removeDatabase(QSqlDatabase::defaultConnection);
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(Opcodes::m_FileName);
    db.open();


    db.transaction();

    QSqlQuery query;

    QString Table_Name = "MAINTOON";

    QString RemainingTPs1 = QString("%1").arg(IN_GenerateXP.UnSpentTPs,1,10,QLatin1Char('0')).toUpper();
    Convert SendToHex;
    QString RemainingTPs2 = SendToHex.ConvertTo(RemainingTPs1);

    QString Column_Name = "RemainingTPs";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",RemainingTPs2);
    query.bindValue(":id",IN_GenerateXP.packetid);
    query.exec();

    IN_GenerateXP.StartUnSpentTPs = RemainingTPs2;

    ////////////////////////////////////////////////////////////////////////////////////////////

    QString TotalTPSpent1 = QString("%1").arg(IN_GenerateXP.SpentTPs,1,10,QLatin1Char('0')).toUpper();
    QString TotalTPSpent2 = SendToHex.ConvertTo(TotalTPSpent1);

    Column_Name = "TotalTPSpent";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",TotalTPSpent2);
    query.bindValue(":id",IN_GenerateXP.packetid);
    query.exec();

    IN_GenerateXP.StartSpentTPs = TotalTPSpent2;

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_GenerateXP);

    flipped sento;
    sento.SendOut(IN_GenerateXP.ClientsID, IN_GenerateXP.ClientsIP, IN_GenerateXP.ClientsPort);

    ////////////////////////////////////////////////////////////////////////////////////////////

    QString Column_value = IN_GenerateXP.Cash_on_hand.toUtf8();
    Column_Name = "TunarOnPlayer";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_GenerateXP.packetid);
    query.exec();


    Column_value = IN_GenerateXP.Current_Available_balance.toUtf8();
    Column_Name = "TunarInBank";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_GenerateXP.packetid);
    query.exec();


    ////////////////////////////////////////////////////////////////////////////////////////////

    QString StartXPbar2 = SendToHex.ConvertTo(IN_GenerateXP.StartXPbar);

    Column_value = StartXPbar2.toUtf8();
    Column_Name = "XPbar";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_GenerateXP.packetid);
    query.exec();


    ////////////////////////////////////////////////////////////////////////////////////////////

    QString StartTotalXP2 = SendToHex.ConvertTo(IN_GenerateXP.StartTotalXP);

    Column_value = StartTotalXP2.toUtf8();
    Column_Name = "TotalXP";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_GenerateXP.packetid);
    query.exec();

    ////////////////////////////////////////////////////////////////////////////////////////////

    Column_value = IN_GenerateXP.StartLevel.toUtf8();
    Column_Name = "Level (1)";
    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    query.bindValue(":val",Column_value);
    query.bindValue(":id",IN_GenerateXP.packetid);
    query.exec();


    //    ////////////////////////////////////////////////////////////////////////////////////////////

    //    Column_value = "00";
    //    Column_Name = "TotalCMxp";
    //    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    //    query.bindValue(":val",Column_value);
    //    query.bindValue(":id",packetid);
    //    query.exec();


    //    ////////////////////////////////////////////////////////////////////////////////////////////

    //    Column_value = "00";
    //    Column_Name = "UnSpentCMs";
    //    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    //    query.bindValue(":val",Column_value);
    //    query.bindValue(":id",packetid);
    //    query.exec();


    //    ////////////////////////////////////////////////////////////////////////////////////////////

    //    Column_value = "00";
    //    Column_Name = "SpentCMs";
    //    query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
    //    query.bindValue(":val",Column_value);
    //    query.bindValue(":id",packetid);
    //    query.exec();



    db.commit();

    return dumMy1;
}


QByteArray Quests::AddQuestLog(QByteArray ID_IP_PORT, QString ResultID)
{
    packetparsing::packetvars IN_AddQuestLog;
    IN_AddQuestLog = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    //qDebug() << "IN_AddQuestLog.ResultID" << ResultID;

    bool QuestLogisFull = false;
    QString Slot = "";

    QMap<QString,QString> QuestLogSlot_QuestID_Map = Quests::PlayersID_QuestLogSlot_QuestID_MapOfMaps.value(IN_AddQuestLog.CharSelectID);

    QString Slot_00 = QuestLogSlot_QuestID_Map.value("00");

    if(Slot_00.isEmpty() == true)
    {
        QuestLogSlot_QuestID_Map.insert("00",ResultID);
        Slot = "00";
    }
    else
    {
        QString Slot_02 = QuestLogSlot_QuestID_Map.value("02");

        if(Slot_02.isEmpty() == true)
        {
            QuestLogSlot_QuestID_Map.insert("02",ResultID);
            Slot = "02";
        }
        else
        {
            QString Slot_04 = QuestLogSlot_QuestID_Map.value("04");

            if(Slot_04.isEmpty() == true)
            {
                QuestLogSlot_QuestID_Map.insert("04",ResultID);
                Slot = "04";
            }
            else
            {
                QString Slot_06 = QuestLogSlot_QuestID_Map.value("06");

                if(Slot_06.isEmpty() == true)
                {
                    QuestLogSlot_QuestID_Map.insert("06",ResultID);
                    Slot = "06";
                }
                else
                {
                    QString Slot_08 = QuestLogSlot_QuestID_Map.value("08");

                    if(Slot_08.isEmpty() == true)
                    {
                        QuestLogSlot_QuestID_Map.insert("08",ResultID);
                        Slot = "08";
                    }
                    else
                    {
                        QString Slot_0A = QuestLogSlot_QuestID_Map.value("0A");

                        if(Slot_0A.isEmpty() == true)
                        {
                            QuestLogSlot_QuestID_Map.insert("0A",ResultID);
                            Slot = "0A";
                        }
                        else
                        {
                            QString Slot_0C = QuestLogSlot_QuestID_Map.value("0C");

                            if(Slot_0C.isEmpty() == true)
                            {
                                QuestLogSlot_QuestID_Map.insert("0C",ResultID);
                                Slot = "0C";
                            }
                            else
                            {
                                QString Slot_0E = QuestLogSlot_QuestID_Map.value("0E");

                                if(Slot_0E.isEmpty() == true)
                                {
                                    QuestLogSlot_QuestID_Map.insert("0E",ResultID);
                                    Slot = "0E";
                                }
                                else
                                {
                                    QuestLogisFull = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //qDebug() << "Slot" << Slot;
    //qDebug() << "QuestLogisFull" << QuestLogisFull;
    //qDebug() << "QuestLogSlot_QuestID_Map" << QuestLogSlot_QuestID_Map;

    QByteArray Packet = "";

    if(QuestLogisFull == false)
    {
        Quests::PlayersID_QuestLogSlot_QuestID_MapOfMaps.insert(IN_AddQuestLog.CharSelectID,QuestLogSlot_QuestID_Map);

        QString LogsText = Quests::LogTextID_Text.value(ResultID);

        QString Opcode = "7C00010000" + Slot;

        QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_AddQuestLog.CharSelectID);
        QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

        ADD_OR_SUB sendto;
        QString FBText5 =  sendto.TextSize(IN_AddQuestLog.ClientsID,IN_AddQuestLog.ClientsIP,IN_AddQuestLog.ClientsPort,LogsText,false,Opcode,Servers_Last_FB_MessageNumber);

        Servers_Last_FB_MessageNumber = FBText5.mid(0,4).toUtf8();
        FBText5.remove(0,4);

        MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);
        Opcodes::CharID_ServerMessageNumbers.insert(IN_AddQuestLog.CharSelectID,MessNumbVect);

        Packet.append(FBText5.toUtf8());

        QString qText = "You have received a quest.";
        QString FBText =  sendto.TextSize(IN_AddQuestLog.ClientsID,IN_AddQuestLog.ClientsIP,IN_AddQuestLog.ClientsPort,qText,true,"460001000000",Servers_Last_FB_MessageNumber);

        IN_AddQuestLog.SendFBsVector.append(FBText.toUtf8());
        IN_AddQuestLog.FBReplyOrNotVector.append("03");
        IN_AddQuestLog.FBPacketDescriptionVector.append("Quest_Received, ");

        packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_AddQuestLog);

        flipped sento;
        sento.SendOut(IN_AddQuestLog.ClientsID, IN_AddQuestLog.ClientsIP, IN_AddQuestLog.ClientsPort);

    }

    return Packet;
}

QVector<QString> Quests::CheckYoPockets(QByteArray ID_IP_PORT, QVector<QString> CheckVector)
{
    packetparsing::packetvars IN_CheckYoPockets;
    IN_CheckYoPockets = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    QVector<QString> QMIQ_TempVect = CheckVector;

    //qDebug() << "IN_QuestMenus.QMIQ_TempVect" << QMIQ_TempVect;

    QVector<QString> ReturnResultsVector;

    QString NPCsID = QMIQ_TempVect.takeAt(0);

    QVector<QString> Popup_9sVector;

    bool SendToXP = false;
    bool checkAll = true;

    if(QMIQ_TempVect.isEmpty() == false)
    {
        QString CheckResultID = "";
        QString ItemsID = "";
        QString Quantity = "";

        for(int re = 0; re < QMIQ_TempVect.size(); re++)
        {
            CheckResultID = QMIQ_TempVect.at(re);

            if(CheckResultID.contains("IC"))
            {
                int index = CheckResultID.indexOf("#");
                ItemsID = CheckResultID.mid(3,10);
                Quantity = CheckResultID.mid(index+1,-1);

                //qDebug() << "IN_CheckYoPockets.ItemsID" << ItemsID;
                //qDebug() << "IN_CheckYoPockets.QuantityRequired" << Quantity;

                if(IN_CheckYoPockets.InventorySlots_ItemIDs.contains(ItemsID))
                {
                    int index = IN_CheckYoPockets.InventorySlots_ItemIDs.indexOf(ItemsID);
                    QString DQuantity = IN_CheckYoPockets.InventoryItem_Quantity.at(index);

                    //qDebug() << "IN_CheckYoPockets.DQuantity" << DQuantity;

                    ADD_OR_SUB sendto;
                    QString QuantityCheck = sendto.BackToDec(DQuantity);

                    int qvalNeeded = Quantity.toInt(nullptr,10);
                    int qvalHave = QuantityCheck.toInt(nullptr,10);

                    //qDebug() << "IN_CheckYoPockets.qvalNeed" << qvalNeeded;
                    //qDebug() << "IN_CheckYoPockets.qvalHave" << qvalHave;

                    if(qvalNeeded > qvalHave)
                    {
                        SendToXP = false;
                        checkAll = false;

                        QMap<QString,QMap<int,QVector<QString>>> OP_Flag_Popup_9sVectorMaps;
                        QMap<int,QVector<QString>> Flag_Popup_9sVectorMaps;

                        OP_Flag_Popup_9sVectorMaps = Quests::QGID_OP_Flag_Popup_9sVectorMaps.value(NPCsID);


                        if(OP_Flag_Popup_9sVectorMaps.contains("9"))
                        {
                            Flag_Popup_9sVectorMaps = OP_Flag_Popup_9sVectorMaps.value("9");
                        }

                        Popup_9sVector = Flag_Popup_9sVectorMaps.value(IN_CheckYoPockets.MainFlag);

                        ReturnResultsVector = Popup_9sVector;
                        //qDebug() << "IN_CheckYoPockets.ReturnResultsVector1" << ReturnResultsVector;
                        break;
                    }
                    else
                    {
                        SendToXP = true;
                    }
                }
                else
                {
                    SendToXP = false;
                    checkAll = false;

                    QMap<QString,QMap<int,QVector<QString>>> OP_Flag_Popup_9sVectorMaps;
                    QMap<int,QVector<QString>> Flag_Popup_9sVectorMaps;

                    OP_Flag_Popup_9sVectorMaps = Quests::QGID_OP_Flag_Popup_9sVectorMaps.value(NPCsID);

                    if(OP_Flag_Popup_9sVectorMaps.contains("9"))
                    {
                        Flag_Popup_9sVectorMaps = OP_Flag_Popup_9sVectorMaps.value("9");
                    }

                    ReturnResultsVector = Flag_Popup_9sVectorMaps.value(IN_CheckYoPockets.MainFlag);

                    //qDebug() << "IN_CheckYoPockets.ReturnResultsVector2" << ReturnResultsVector;
                    break;
                }
            }

            if(CheckResultID.contains("TC"))
            {
                QString TunarQty = CheckResultID.mid(3,-1);

                //qDebug() << "IN_CheckYoPockets.TunarQty" << TunarQty;

                ADD_OR_SUB sendto;
                QString TunarQtyCheck = sendto.BackToDec(IN_CheckYoPockets.Cash_on_hand);

                int Tunar_U_Need = TunarQty.toInt(nullptr,10);
                int Tunar_U_Have = TunarQtyCheck.toInt(nullptr,10);

                //qDebug() << "IN_CheckYoPockets.Tunar_U_Need" << Tunar_U_Need;
                //qDebug() << "IN_CheckYoPockets.Tunar_U_Have" << Tunar_U_Have;

                if(Tunar_U_Need > Tunar_U_Have)
                {
                    int Balance = Tunar_U_Need - Tunar_U_Have;

                    QMap<QString,QMap<int,QVector<QString>>> OP_Flag_Popup_9sVectorMaps;
                    QMap<int,QVector<QString>> Flag_Popup_9sVectorMaps;

                    OP_Flag_Popup_9sVectorMaps = Quests::QGID_OP_Flag_Popup_9sVectorMaps.value(NPCsID);

                    if(OP_Flag_Popup_9sVectorMaps.contains("9"))
                    {
                        Flag_Popup_9sVectorMaps = OP_Flag_Popup_9sVectorMaps.value("9");
                    }

                    Popup_9sVector = Flag_Popup_9sVectorMaps.value(IN_CheckYoPockets.MainFlag);

                    if(ReturnResultsVector.isEmpty() == true)
                    {
                        ReturnResultsVector = Popup_9sVector;
                    }
                    //qDebug() << "IN_CheckYoPockets.ReturnResultsVector3" << ReturnResultsVector;

                    QString FBsizeOpcode2 = "You need " + QString::number(Balance) + " more tunar to complete your quest.";

                    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_CheckYoPockets.CharSelectID);
                    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

                    ADD_OR_SUB SendToText;
                    QString EQuipText = FBsizeOpcode2;
                    QString FBText =  SendToText.TextSize(IN_CheckYoPockets.ClientsID,IN_CheckYoPockets.ClientsIP,IN_CheckYoPockets.ClientsPort,EQuipText,true,"460001000000",Servers_Last_FB_MessageNumber);

                    SendToXP = false;

                    IN_CheckYoPockets.SendFBsVector.append(FBText.toUtf8());
                    IN_CheckYoPockets.FBReplyOrNotVector.append("00");
                    IN_CheckYoPockets.FBPacketDescriptionVector.append("Insufficient_Funds, ");
                }
                else
                {
                    SendToXP = true;
                }
            }
        }
    }


    if(checkAll == false)
    {
        SendToXP = false;
    }

    if(ReturnResultsVector.isEmpty() == false)
    {
        IN_CheckYoPockets.CurrentQuestMenuID = ReturnResultsVector.at(2);
    }

    //qDebug() << "IN_CheckYoPockets.checkAll" << checkAll;
    //qDebug() << "IN_CheckYoPockets.SendToXP" << SendToXP;
    //qDebug() << "IN_CheckYoPockets.CurrentQuestMenuID" << IN_CheckYoPockets.CurrentQuestMenuID;

    if(SendToXP == true)
    {
        QVector<QString> tempPopUpResultVect = Quests::PopUpID_PopUpResltsVectorMap.value(IN_CheckYoPockets.CurrentQuestMenuID);

        for(int re = 0; re < tempPopUpResultVect.size(); re++)
        {
            QString ResultID = tempPopUpResultVect.at(re);

            if(ResultID.contains("xp_"))
            {
                packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_CheckYoPockets);

                QString XP = GenerateXP(ID_IP_PORT,ResultID);

                //qDebug() << "IN_CheckYoPockets.XP" << XP;

                IN_CheckYoPockets = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);
            }
        }
    }

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_CheckYoPockets);

    return ReturnResultsVector;

}

QByteArray Quests::RemoveQuestLog(QByteArray ID_IP_PORT, QString ResultID)
{
    packetparsing::packetvars IN_RemoveQuestLog;
    IN_RemoveQuestLog = packetparsing::IPandPort_AllvariablesMap.value(ID_IP_PORT);

    //qDebug() << "ResultID" << ResultID;

    QMap<QString,QString> QuestLogSlot_QuestID_Map = Quests::PlayersID_QuestLogSlot_QuestID_MapOfMaps.value(IN_RemoveQuestLog.CharSelectID);

    //qDebug() << "QuestLogSlot_QuestID_Map" << QuestLogSlot_QuestID_Map;

    QString Slot = QuestLogSlot_QuestID_Map.key(ResultID);

    ADD_OR_SUB sendto;
    QString Returned = "";

    QVector <QByteArray> MessNumbVect = Opcodes::CharID_ServerMessageNumbers.value(IN_RemoveQuestLog.CharSelectID);
    QByteArray Servers_Last_FB_MessageNumber = MessNumbVect.at(1);

    if(Slot.isEmpty() == false)
    {
        QuestLogSlot_QuestID_Map.insert(Slot,"");

        Quests::PlayersID_QuestLogSlot_QuestID_MapOfMaps.insert(IN_RemoveQuestLog.CharSelectID,QuestLogSlot_QuestID_Map);

        //qDebug() << "Slot" << Slot;

        QString Opcode = "7D0000000000" + Slot;

        Returned = sendto.FBSize(Opcode);

        increment Text;
        QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
        int index = Returned.indexOf("XXXX");
        Returned.replace(index,4,Text2);
        Servers_Last_FB_MessageNumber = Text2.toUpper();

        MessNumbVect.replace(1,Servers_Last_FB_MessageNumber);

        Opcodes::CharID_ServerMessageNumbers.insert(IN_RemoveQuestLog.CharSelectID,MessNumbVect);
    }

    QByteArray Packet = Returned.toUtf8();


    QString qText = "You have finished a quest.";
    QString FBText =  sendto.TextSize(IN_RemoveQuestLog.ClientsID,IN_RemoveQuestLog.ClientsIP,IN_RemoveQuestLog.ClientsPort,qText,true,"460001000000",Servers_Last_FB_MessageNumber);

    IN_RemoveQuestLog.SendFBsVector.append(FBText.toUtf8());
    IN_RemoveQuestLog.FBReplyOrNotVector.append("03");
    IN_RemoveQuestLog.FBPacketDescriptionVector.append("Quest_Finished, ");

    packetparsing::IPandPort_AllvariablesMap.insert(ID_IP_PORT,IN_RemoveQuestLog);

    flipped sento;
    sento.SendOut(IN_RemoveQuestLog.ClientsID, IN_RemoveQuestLog.ClientsIP, IN_RemoveQuestLog.ClientsPort);

    return Packet;
}


