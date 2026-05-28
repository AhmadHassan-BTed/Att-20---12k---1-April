#include "xorpacket.h"
#include "add_or_sub.h"
#include "generator.h"
#include "combat.h"
#include "convert.h"
#include "packetparsing.h"
#include "login.h"
#include "opcodes.h"
xorpacket::xorpacket(QObject *parent) : QObject(parent)
{

}

QByteArray xorpacket::Add42ECtoDB(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort)
{
    packetparsing::packetvars p13;
    p13 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    if (db.isOpen())
    {

        db.close();
        db = QSqlDatabase();
        db.removeDatabase(QSqlDatabase::defaultConnection);
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(Opcodes::m_FileName);
    db.open();

    int NextStat = 0;

    QString HPMax = p13.StatsChange.mid(64,8);

    for (int rt = 0; rt < StatsFieldsVector.size(); rt++)
    {
        QString place1 = StatsFieldsVector.value(rt);

        QString EC42Val = "";

        if(place1 == "UNK-02" || place1 == "UNK-04")
        {
            EC42Val = p13.StatsChange.mid(NextStat,56);
            NextStat += 56;
        }
        else
        {
            if(place1 == "HP" || place1 == "HP-2")
            {
                EC42Val = HPMax;
                NextStat += 8;
            }
            else
            {
                EC42Val = p13.StatsChange.mid(NextStat,8);
                NextStat += 8;
            }
        }

        QSqlQuery query;
        query.prepare(QString("UPDATE STATS42EC SET [%1] = %2 WHERE ROWID = %3").arg(place1).arg(EC42Val).arg(p13.packetid));
        query.exec();
    }


    return "db update finished";
}


QByteArray xorpacket::packetswitch(QByteArray byte2) //send packet here as byte2
{
    decopacket.clear(); //clear here cause we cant clear after return

    /////////////////////////////DECOMPRESSION////////////////////
    packetsize = byte2.size();


    where = byte2.indexOf("c9",carat1); //find C9
    // qDebug() <<"where" <<where;
    carat1 = where - 2; //go back two for byte
    //   qDebug() << "back" <<carat1;
    QByteArray hold1 = byte2.mid(carat1,2); //find byte before c9

    QByteArray c9data = byte2.mid(carat1 + 6,2); //find double zero
    //   qDebug() << "hold1" <<hold1;
    if (hold1 >= "80" || c9data != "00") //is this a fill with zeros channel, or c9 in the data itself?
    {
        where = byte2.indexOf("c9",carat1); //find next c9
        carat1 = where + 8;
    }


    //  qDebug() <<"backagain" << carat1;
    else
    {
        carat1 += 10;
    }

    do
    {


        decom1 = byte2.mid(carat1,2); //get first compressed

        int over1 = decom1.toInt(nullptr,16); //change to int
        //  qDebug() <<"decom1" << decom1;

        if (over1 > 0x79) //is it over 79h?
        {
            decom1 = byte2.mid(carat1,4); //get two bytes

            int hold1 = decom1.mid(0,2).toInt(nullptr,16); //get highbyte
            high = hold1 - 0x80;  //subtract 80h
            //    qDebug() << "high" << high;
            low = decom1.mid(2,2).toInt(nullptr,16); //get lowbyte
            carat1 += 4; //increment by what was read
            //     qDebug() <<"low" <<low;
        }



        else
        {
            high = decom1.mid(0,1).toInt(nullptr,16); //get one byte
            low = decom1.mid(1,1).toInt(nullptr,16); //get one byte

            if (high == 0 && low == 0) //breakout when done
            {
                //         qDebug() << "two zeros" <<high << low;


                where = byte2.indexOf("c9",carat1); //find C9
                //             qDebug() <<"where2" <<where;
                carat1 = where - 2; //go back find byte before C9
                //            qDebug() << "back2" <<carat1;
                QByteArray hold1 = byte2.mid(carat1,2); //find byte before c9
                QByteArray c9data2 = byte2.mid(carat1 + 6,2); //find double zero
                if (hold1 == "C9" || c9data2 != "00") //check


                {
                    where = byte2.indexOf("c9",carat1); //find next c9
                    qDebug() <<"where-2" <<where;
                    carat1 = where + 8;
                    if (where == -1)
                    {
                        break; //295 and others with text above or below
                    }
                }
                //          qDebug() << "hold1-2" <<hold1;
                //set cursor two less back to first compression byte
                //         qDebug() <<"backagain2" << carat1;
                else
                {
                    carat1 += 8; //testing 296
                }


            }
            carat1 += 2; //increment by what was read
        }
        qDebug() <<"decom1" <<decom1;

        QByteArray zeros = "00"; //zero byte
        QByteArray zeroholder = ""; //hold number of zeros

        zeroholder = zeros.repeated(low); //repeat #
        decopacket.insert(carat2,zeroholder); //insert zeros

        carat2 += low * 2; //set new carat position

        QByteArray decom2 = byte2.mid(carat1,high * 2); //get bytes to keep

        decopacket.insert(carat2,decom2); //insert kept bytes

        carat1 += high *2 ; //set new carat position
        carat2 += high *2; //set new carat position

        // qDebug() <<"highbyte" <<high;
        // qDebug() <<"lowbyte" <<low;
        //  qDebug() <<"packet" <<decopacket;
        //  qDebug() << "carat1" <<carat1;



    }
    while (carat1 < packetsize -2);


    carat1 = 0;
    carat2 = 0;
    high = 0;
    low = 0;
    where = 0;
    where2 = 0;


    return decopacket;
}

//  qDebug() << "DECO" <<decopacket;
/////////////////////////////DECOMPRESSION////////////////////


/////////////////////////RECOMPRESSION/////////////////////////////////////

QByteArray xorpacket::Recompress42EC(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QList <QByteArray> mylist1, QString CharSelectID1, QByteArray StatsChange1, QString HoldOldHP1, int NewCurrentHP1, int NewCurrentPOW1, int MyNewHP)
{
    packetparsing::packetvars p13;
    p13 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    Convert SendToHex;

    if(mylist1.isEmpty() == true)
    {
        mylist1 = {"00","00","00","00","00","00","00"};
    }

    p13.mylist = mylist1;
    qDebug() << "";
    qDebug() << "Sent-to-42EC";
    qDebug() << "mylist2" << p13.mylist;
    p13.CharSelectID = CharSelectID1;
    p13.StatsChange = StatsChange1;

    qDebug() << "StatsChange1" << StatsChange1;

    QByteArray NEW42ECxor1 = NEW42ECxor;

    QByteArray STRold = p13.StatsChange.mid(0,4);
    QByteArray STRoldA = STRold.mid(0,2);
    QByteArray STRoldB = STRold.mid(2,2);
    QByteArray revSTRold = STRoldB + STRoldA;
    int STRold1 = revSTRold.toInt(nullptr,16);
    qDebug() << "STRold1" << STRold1;

    QByteArray STAold = p13.StatsChange.mid(8,4);
    QByteArray STAoldA = STAold.mid(0,2);
    QByteArray STAoldB = STAold.mid(2,2);
    QByteArray revSTAold = STAoldB + STAoldA;
    int STAold1 = revSTAold.toInt(nullptr,16);
    qDebug() << "STAold1" << STAold1;

    QByteArray AGIold = p13.StatsChange.mid(16,4);
    QByteArray AGIoldA = AGIold.mid(0,2);
    QByteArray AGIoldB = AGIold.mid(2,2);
    QByteArray revAGIold = AGIoldB + AGIoldA;
    int AGIold1 = revAGIold.toInt(nullptr,16);
    qDebug() << "AGIold1" << AGIold1;

    QByteArray DEXold = p13.StatsChange.mid(24,4);
    QByteArray DEXoldA = DEXold.mid(0,2);
    QByteArray DEXoldB = DEXold.mid(2,2);
    QByteArray revDEXold = DEXoldB + DEXoldA;
    int DEXold1 = revDEXold.toInt(nullptr,16);
    qDebug() << "DEXold1" << DEXold1;

    QByteArray WISold = p13.StatsChange.mid(32,4);
    QByteArray WISoldA = WISold.mid(0,2);
    QByteArray WISoldB = WISold.mid(2,2);
    QByteArray revWISold = WISoldB + WISoldA;
    int WISold1 = revWISold.toInt(nullptr,16);
    qDebug() << "WISold1" << WISold1;

    QByteArray INTold = p13.StatsChange.mid(40,4);
    QByteArray INToldA = INTold.mid(0,2);
    QByteArray INToldB = INTold.mid(2,2);
    QByteArray revINTold = INToldB + INToldA;
    int INTold1 = revINTold.toInt(nullptr,16);
    qDebug() << "INTold1" << INTold1;

    QByteArray CHAold = p13.StatsChange.mid(48,4);
    QByteArray CHAoldA = CHAold.mid(0,2);
    QByteArray CHAoldB = CHAold.mid(2,2);
    QByteArray revCHAold = CHAoldB + CHAoldA;
    int CHAold1 = revCHAold.toInt(nullptr,16);
    qDebug() << "CHAold1" << CHAold1;

    QString NewSTR = "";
    QString NewSTA = "";
    QString NewAGI = "";
    QString NewDEX = "";
    QString NewWIS = "";
    QString NewINT = "";
    QString NewCHA = "";

    if(p13.mylist.isEmpty() == false)
    {

        ADD_OR_SUB SendToDec;
        QString STR = p13.mylist.at(0);
        QString DecVal = SendToDec.BackToDec(STR);
        QString STR4 = DecVal.toUpper();
        int DecVal1 = DecVal.toInt(nullptr,10);
        DecVal1 = DecVal1 + STRold1;
        NewSTR = QString("%1").arg(DecVal1,1,10,QLatin1Char('0')).toUtf8().toUpper();
        QString STR0 = QString("%1").arg(DecVal1,4,16,QLatin1Char('0')).toUtf8().toUpper();
        QString STR1 = STR0.mid(0,2);
        QString STR2 = STR0.mid(2,2);
        QString reverseSTR = STR2 + STR1;
        qDebug() << "reverseSTR_StatsChange_0,4/232,4" << reverseSTR.toUtf8();
        p13.StatsChange.replace(0,4,reverseSTR.toUtf8());
        p13.StatsChange.replace(232,4,reverseSTR.toUtf8());

        Old_value = STRold.toLongLong(nullptr,16);
        New_value = reverseSTR.toUtf8();
        New_value2 = New_value.toLongLong(nullptr,16);
        Output_Xor = Old_value ^ New_value2;
        Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
        NEW42ECxor1.replace(0,4,Output_Xor_2.toUtf8());
        NEW42ECxor1.replace(232,4,Output_Xor_2.toUtf8());


        QString STA = p13.mylist.at(1);
        DecVal = SendToDec.BackToDec(STA);
        QString STA4 = DecVal.toUpper();
        DecVal1 = DecVal.toInt(nullptr,10);
        DecVal1 = DecVal1 + STAold1;
        NewSTA = QString("%1").arg(DecVal1,1,10,QLatin1Char('0')).toUtf8().toUpper();
        QString STA0 = QString("%1").arg(DecVal1,4,16,QLatin1Char('0')).toUtf8().toUpper();
        QString STA1 = STA0.mid(0,2);
        QString STA2 = STA0.mid(2,2);
        QString reverseSTA = STA2 + STA1;
        qDebug() << "reverseSTA_StatsChange_8,4/240,4" << reverseSTA.toUtf8();
        p13.StatsChange.replace(8,4,reverseSTA.toUtf8());
        p13.StatsChange.replace(240,4,reverseSTA.toUtf8());

        Old_value = STAold.toLongLong(nullptr,16);
        New_value = reverseSTA.toUtf8();
        New_value2 = New_value.toLongLong(nullptr,16);
        Output_Xor = Old_value ^ New_value2;
        Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
        NEW42ECxor1.replace(8,4,Output_Xor_2.toUtf8());
        NEW42ECxor1.replace(240,4,Output_Xor_2.toUtf8());

        QString AGI = p13.mylist.at(2);
        DecVal = SendToDec.BackToDec(AGI);
        QString AGI4 = DecVal.toUpper();
        DecVal1 = DecVal.toInt(nullptr,10);
        DecVal1 = DecVal1 + AGIold1;
        NewAGI = QString("%1").arg(DecVal1,1,10,QLatin1Char('0')).toUtf8().toUpper();
        QString AGI0 = QString("%1").arg(DecVal1,4,16,QLatin1Char('0')).toUtf8().toUpper();
        QString AGI1 = AGI0.mid(0,2);
        QString AGI2 = AGI0.mid(2,2);
        QString reverseAGI = AGI2 + AGI1;
        qDebug() << "reverseAGI_StatsChange_16,4/248,4" << reverseAGI.toUtf8();
        p13.StatsChange.replace(16,4,reverseAGI.toUtf8());
        p13.StatsChange.replace(248,4,reverseAGI.toUtf8());

        Old_value = AGIold.toLongLong(nullptr,16);
        New_value = reverseAGI.toUtf8();
        New_value2 = New_value.toLongLong(nullptr,16);
        Output_Xor = Old_value ^ New_value2;
        Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
        NEW42ECxor1.replace(16,4,Output_Xor_2.toUtf8());
        NEW42ECxor1.replace(248,4,Output_Xor_2.toUtf8());

        QString DEX = p13.mylist.at(3);
        DecVal = SendToDec.BackToDec(DEX);
        QString DEX4 = DecVal.toUpper();
        DecVal1 = DecVal.toInt(nullptr,10);
        DecVal1 = DecVal1 + DEXold1;
        NewDEX = QString("%1").arg(DecVal1,1,10,QLatin1Char('0')).toUtf8().toUpper();
        QString DEX0 = QString("%1").arg(DecVal1,4,16,QLatin1Char('0')).toUtf8().toUpper();
        QString DEX1 = DEX0.mid(0,2);
        QString DEX2 = DEX0.mid(2,2);
        QString reverseDEX = DEX2 + DEX1;
        qDebug() << "reverseDEX_StatsChange_24,4/256,4" << reverseDEX.toUtf8();
        p13.StatsChange.replace(24,4,reverseDEX.toUtf8());
        p13.StatsChange.replace(256,4,reverseDEX.toUtf8());

        Old_value = DEXold.toLongLong(nullptr,16);
        New_value = reverseDEX.toUtf8();
        New_value2 = New_value.toLongLong(nullptr,16);
        Output_Xor = Old_value ^ New_value2;
        Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
        NEW42ECxor1.replace(24,4,Output_Xor_2.toUtf8());
        NEW42ECxor1.replace(256,4,Output_Xor_2.toUtf8());

        QString WIS = p13.mylist.at(4);
        DecVal = SendToDec.BackToDec(WIS);
        QString WIS4 = DecVal.toUpper();
        DecVal1 = DecVal.toInt(nullptr,10);
        DecVal1 = DecVal1 + WISold1;
        NewWIS = QString("%1").arg(DecVal1,1,10,QLatin1Char('0')).toUtf8().toUpper();
        QString WIS0 = QString("%1").arg(DecVal1,4,16,QLatin1Char('0')).toUtf8().toUpper();
        QString WIS1 = WIS0.mid(0,2);
        QString WIS2 = WIS0.mid(2,2);
        QString reverseWIS = WIS2 + WIS1;
        qDebug() << "reverseWIS_StatsChange_32,4/264,4" << reverseWIS.toUtf8();
        p13.StatsChange.replace(32,4,reverseWIS.toUtf8());
        p13.StatsChange.replace(264,4,reverseWIS.toUtf8());

        Old_value = WISold.toLongLong(nullptr,16);
        New_value = reverseWIS.toUtf8();
        New_value2 = New_value.toLongLong(nullptr,16);
        Output_Xor = Old_value ^ New_value2;
        Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
        NEW42ECxor1.replace(32,4,Output_Xor_2.toUtf8());
        NEW42ECxor1.replace(264,4,Output_Xor_2.toUtf8());

        QString INT = p13.mylist.at(5);
        DecVal = SendToDec.BackToDec(INT);
        QString INT4 = DecVal.toUpper();
        DecVal1 = DecVal.toInt(nullptr,10);
        DecVal1 = DecVal1 + INTold1;
        NewINT = QString("%1").arg(DecVal1,1,10,QLatin1Char('0')).toUtf8().toUpper();
        QString INT0 = QString("%1").arg(DecVal1,4,16,QLatin1Char('0')).toUtf8().toUpper();
        QString INT1 = INT0.mid(0,2);
        QString INT2 = INT0.mid(2,2);
        QString reverseINT = INT2 + INT1;
        qDebug() << "reverseINT_StatsChange_40,4/272,4" << reverseINT.toUtf8();
        p13.StatsChange.replace(40,4,reverseINT.toUtf8());
        p13.StatsChange.replace(272,4,reverseINT.toUtf8());


        Old_value = INTold.toLongLong(nullptr,16);
        New_value = reverseINT.toUtf8();
        New_value2 = New_value.toLongLong(nullptr,16);
        Output_Xor = Old_value ^ New_value2;
        Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
        NEW42ECxor1.replace(40,4,Output_Xor_2.toUtf8());
        NEW42ECxor1.replace(272,4,Output_Xor_2.toUtf8());

        QString CHA = p13.mylist.at(6);
        DecVal = SendToDec.BackToDec(CHA);
        QString CHA4 = DecVal.toUpper();
        DecVal1 = DecVal.toInt(nullptr,10);
        DecVal1 = DecVal1 + CHAold1;
        NewCHA = QString("%1").arg(DecVal1,1,10,QLatin1Char('0')).toUtf8().toUpper();
        QString CHA0 = QString("%1").arg(DecVal1,4,16,QLatin1Char('0')).toUtf8().toUpper();
        QString CHA1 = CHA0.mid(0,2);
        QString CHA2 = CHA0.mid(2,2);
        QString reverseCHA = CHA2 + CHA1;
        qDebug() << "reverseCHA_StatsChange_48,4/280,4" << reverseCHA.toUtf8();
        p13.StatsChange.replace(48,4,reverseCHA.toUtf8());
        p13.StatsChange.replace(280,4,reverseCHA.toUtf8());

        Old_value = CHAold.toLongLong(nullptr,16);
        New_value = reverseCHA.toUtf8();
        New_value2 = New_value.toLongLong(nullptr,16);
        Output_Xor = Old_value ^ New_value2;
        Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
        NEW42ECxor1.replace(48,4,Output_Xor_2.toUtf8());
        NEW42ECxor1.replace(280,4,Output_Xor_2.toUtf8());
    }

    ///////////////// HP and POW ///////////////////////////

    current_c9 = generator::Players_Map.value(p13.CharSelectID.toUtf8());

    QString Level2 = current_c9.mid(358,2);//level
    QString Class2 = current_c9.mid(368,2);//class

    Combat AllValues;
    QString MyHP1 = AllValues.SendToMAXHP(ClientID,ClientsIP,ClientsPort,Level2, Class2, NewSTA);
    int MyHP2 = MyHP1.toInt(nullptr,10);
    QString NewHP = QString("%1").arg(MyHP2,4,16,QLatin1Char('0')).toUpper();
    QString HP1 = NewHP.mid(0,2);
    QString HP2 = NewHP.mid(2,2);
    QString NewReverseHP = HP2 + HP1;
    QString NewHPMax = NewReverseHP;


    QString CurrentHP = p13.StatsChange.mid(56,4);
    HP1 = CurrentHP.mid(0,2);
    HP2 = CurrentHP.mid(2,2);
    QString NewReverseCurrentHP = HP2 + HP1;
    int NewReverseHP1 = NewReverseCurrentHP.toInt(nullptr,16);


    if(MyNewHP > -1)
    {
        NewReverseHP1 = MyNewHP;
    }



    QString HP = p13.StatsChange.mid(64,4);
    HP1 = HP.mid(0,2);
    HP2 = HP.mid(2,2);
    QString MaxReverseHP = HP2 + HP1;
    int MaxReverseHP2 = MaxReverseHP.toInt(nullptr,16);



    if((HoldOldHP1 != "YES" && NewReverseHP1 < MaxReverseHP2) || (NewReverseHP1 == MaxReverseHP2))
    {
        int Total = 0;

        Total = NewReverseHP1 + NewCurrentHP1;

        if(Total > MaxReverseHP2)
        {
            Total = MaxReverseHP2;
        }
        if(Total < 1)
        {
            Total = 0;
        }

        QString NewHP = QString("%1").arg(Total,4,16,QLatin1Char('0')).toUpper();
        HP1 = NewHP.mid(0,2);
        HP2 = NewHP.mid(2,2);
        QString NewReverseHP = HP2 + HP1;
        qDebug() << "NewReverseHP_StatsChange_56,4/288,4" << NewReverseHP.toUtf8().toUpper();
        p13.StatsChange.replace(56,4,NewReverseHP.toUtf8().toUpper());
        p13.StatsChange.replace(288,4,NewReverseHP.toUtf8().toUpper());

        int Old_value = CurrentHP.toInt(nullptr,16);
        int New_value2 = NewReverseHP.toInt(nullptr,16);
        int Output_Xor = Old_value ^ New_value2;
        Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
        NEW42ECxor1.replace(56,4,Output_Xor_2.toUtf8());
        NEW42ECxor1.replace(288,4,Output_Xor_2.toUtf8());        
    }

    QString CurrentHP1 = p13.StatsChange.mid(56,4);

    qDebug() << "NewReverseHP_StatsChange_64,4/296,4" << NewReverseHP.toUtf8().toUpper();
    p13.StatsChange.replace(64,4,NewReverseHP.toUtf8());
    p13.StatsChange.replace(296,4,NewHPMax.toUtf8());


    Old_value = HP.toLongLong(nullptr,16);
    New_value2 = NewReverseHP.toLongLong(nullptr,16);
    Output_Xor = Old_value ^ New_value2;
    Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
    NEW42ECxor1.replace(64,4,Output_Xor_2.toUtf8());
    NEW42ECxor1.replace(296,4,Output_Xor_2.toUtf8());

    QMap <QString,QVector <QString>> ClassPrimarySecondaryStatMap = //had to put it here so map could update
    {
        {"00",{NewSTR,NewSTA}},{"02",{NewSTR,NewSTA}},{"03",{NewSTR,NewSTA}},                                  // TANK
        {"09",{NewWIS,NewCHA}},{"07",{NewWIS,NewDEX}},{"08",{NewWIS,NewSTA}},                                  // PRIEST
        {"05",{NewAGI,NewSTA}},{"04",{NewDEX,NewSTA}},{"01",{NewAGI,NewDEX}},{"06",{NewAGI,NewDEX}},                 // MELEE
        {"0E",{NewINT,NewWIS}},{"0C",{NewINT,NewCHA}},{"0A",{NewINT,NewAGI}},{"0B",{NewINT,NewSTA}},{"0D",{NewINT,NewDEX}} // CASTER
    };

    QVector <QString> StatsPower = ClassPrimarySecondaryStatMap.value(Class2);

    QString PrimaryStat = StatsPower.at(0);
    QString SecondaryStat = StatsPower.at(1);

    QString MyPOW1 = AllValues.SendToMAXPOW(ClientID,ClientsIP,ClientsPort,Level2, PrimaryStat, SecondaryStat);
    int MyPOW2 = MyPOW1.toInt(nullptr,10);
    QString NewPOW = QString("%1").arg(MyPOW2,4,16,QLatin1Char('0')).toUpper();
    QString POW1 = NewPOW.mid(0,2);
    QString POW2 = NewPOW.mid(2,2);
    QString NewReversePOW = POW2 + POW1;
    QString NewPOWMax = NewReversePOW;

    QString CurrentPOW = p13.StatsChange.mid(72,4);
    QString POW = p13.StatsChange.mid(80,4);

    if(HoldOldHP1 != "YES")
    {
        QString POW1 = POW.mid(0,2);
        QString POW2 = POW.mid(2,2);
        QString BaseReversePOW = POW2 + POW1;
        int BaseReversePOW2 = BaseReversePOW.toInt(nullptr,16);

        POW1 = CurrentPOW.mid(0,2);
        POW2 = CurrentPOW.mid(2,2);
        QString NewReversePOW = POW2 + POW1;
        int NewReversePOW1 = NewReversePOW.toInt(nullptr,16);

        int Total = NewReversePOW1 + NewCurrentPOW1;

        if(Total > BaseReversePOW2)
        {
            Total = BaseReversePOW2;
        }

        QString NewPOW = QString("%1").arg(Total,4,16,QLatin1Char('0')).toUpper();
        POW1 = NewPOW.mid(0,2);
        POW2 = NewPOW.mid(2,2);
        NewReversePOW = POW2 + POW1;
        qDebug() << "NewReversePOW_StatsChange_72,4/304,4" << NewReversePOW.toUtf8().toUpper();
        p13.StatsChange.replace(72,4,NewReversePOW.toUtf8().toUpper());
        p13.StatsChange.replace(304,4,NewPOWMax.toUtf8().toUpper());

        Old_value = CurrentPOW.toLongLong(nullptr,16);
        New_value2 = NewReversePOW.toLongLong(nullptr,16);
        Output_Xor = Old_value ^ New_value2;
        Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
        NEW42ECxor1.replace(72,4,Output_Xor_2.toUtf8());
        NEW42ECxor1.replace(304,4,Output_Xor_2.toUtf8());


    }

    qDebug() << "NewReversePOW_StatsChange_80,4/312,4" << NewReversePOW.toUtf8().toUpper();
    p13.StatsChange.replace(80,4,NewReversePOW.toUtf8());
    p13.StatsChange.replace(312,4,NewPOWMax.toUtf8());


    Old_value = POW.toLongLong(nullptr,16);
    New_value2 = NewReversePOW.toLongLong(nullptr,16);
    Output_Xor = Old_value ^ New_value2;
    Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
    NEW42ECxor1.replace(80,4,Output_Xor_2.toUtf8());
    NEW42ECxor1.replace(312,4,Output_Xor_2.toUtf8());
    ///////////////// HP and POW ///////////////////////////

    ///////////////////HoT/PoT/////////////////////////////////

    //HoT
    QString OldHoT1 = p13.StatsChange.mid(96,4);

    int NewHoT2 = MyHP2 / 50;
    if(NewHoT2 < 1)
    {
        NewHoT2 = 1;
    }
    QString NewHoT = QString("%1").arg(NewHoT2,4,16,QLatin1Char('0')).toUpper();
    QString HoT1 = NewHoT.mid(0,2);
    QString HoT2 = NewHoT.mid(2,2);
    QString NewReverseHoT = HoT2 + HoT1;
    qDebug() << "NewReverseHoT_StatsChange_96,4/328,4" << NewReverseHoT.toUtf8().toUpper();
    p13.StatsChange.replace(96,4,NewReverseHoT.toUtf8().toUpper());
    p13.StatsChange.replace(328,4,NewReverseHoT.toUtf8().toUpper());

    Old_value = OldHoT1.toLongLong(nullptr,16);
    New_value2 = NewReverseHoT.toLongLong(nullptr,16);
    Output_Xor = Old_value ^ New_value2;
    Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
    NEW42ECxor1.replace(96,4,Output_Xor_2.toUtf8());
    NEW42ECxor1.replace(328,4,Output_Xor_2.toUtf8());

    //PoT
    QString OldPoT1 = p13.StatsChange.mid(104,4);

    int NewPoT2 = MyPOW2 / 50;
    if(NewPoT2 < 1)
    {
        NewPoT2 = 1;
    }
    QString NewPoT = QString("%1").arg(NewPoT2,4,16,QLatin1Char('0')).toUpper();
    QString PoT1 = NewPoT.mid(0,2);
    QString PoT2 = NewPoT.mid(2,2);
    QString NewReversePoT = PoT2 + PoT1;
    qDebug() << "NewReversePoT_StatsChange_104,4/336,4" << NewReversePoT.toUtf8().toUpper();
    p13.StatsChange.replace(104,4,NewReversePoT.toUtf8().toUpper());
    p13.StatsChange.replace(336,4,NewReversePoT.toUtf8().toUpper());

    Old_value = OldPoT1.toLongLong(nullptr,16);
    New_value2 = NewReversePoT.toLongLong(nullptr,16);
    Output_Xor = Old_value ^ New_value2;
    Output_Xor_2 = QString("%1").arg(Output_Xor,4,16,QLatin1Char('0'));
    NEW42ECxor1.replace(104,4,Output_Xor_2.toUtf8());
    NEW42ECxor1.replace(336,4,Output_Xor_2.toUtf8());

    qDebug() << "StatsChange2" << p13.StatsChange;

    ///////////////////HoT/PoT/////////////////////////////////


    encryptpacket.clear();

    if(MyNewHP == -2)
    {
        NEW42ECxor1 = p13.StatsChange;
    }
    workpacket = NEW42ECxor1;

    do
    {
        encom1 = workpacket.mid(encarat1,2);
        holdit = encom1.toInt(nullptr,16);

        if (holdit != 0x00)
        {
            enhigh++;
            hold2 = workpacket.mid(encarat1,2);

            encarat1 += 2;
            encom3.append(hold2);

            if    (encarat1 == workpacket.size() || encarat1 == 402  || encarat1 == 472  || encarat1 == 804  || encarat1 == 1206
                   || encarat1 == 1608 || encarat1 == 2010 || encarat1 == 2412
                   || encarat1 == 2814 || encarat1 == 3216 || encarat1 == 3618 || encarat1 == 4020
                   || encarat1 == 4422 || encarat1 == 4824 || encarat1 == 5226 || encarat1 == 5628
                   || encarat1 == 6030 || encarat1 == 6432 || encarat1 == 6834 || encarat1 == 7236
                   || encarat1 == 7638 || encarat1 == 8040 || encarat1 == 8442 || encarat1 == 8844
                   || encarat1 == 9246 || encarat1 == 9648 || encarat1 == 10050)

            {
                QByteArray hold3 = hold3.number(enhigh,16); //get # of non zero bytes
                hold4 = hold4.number(enlow,16); //get # of zeros
                encryptpacket.append(hold3);
                encryptpacket.append(hold4);
                encryptpacket.append(encom3);
                enhigh = 0;
                enlow = 0;
                zerocounter = 0;
                encom3 = "";
            }
        }
        else
        {
            if(enhigh > 0x7 && enlow <= 0xF)
            {
                enhigh += 0x80;

                QByteArray padder = "0";
                hold4.append(padder);
                hold4.append(hold4.number(enlow,16));
            }
            else
            {
                hold4 = hold4.number(enlow,16); //get # of zeros
            }

            if ((enhigh <= 0x7  ||  enhigh > 0x7) && enlow > 0xF)
            {
                enhigh += 0x80;
            }

            QByteArray hold3 = hold3.number(enhigh,16); //get # of non zero bytes

            if (encarat1 == 0) //clear everything on first read
            {
                hold3 = "";
                hold4 = "";
            }

            encryptpacket.append(hold3);
            encryptpacket.append(hold4);
            encryptpacket.append(encom3);
            encom3 = "";
            hold4 = "";
            enlow = 0;
            enhigh = 0;
            zerocounter = 0;

            do
            {
                encom2 = workpacket.mid(encarat1,2); //spin and count zeros
                zerocounter++;
                encarat1 += 2;
                enlow = zerocounter - 1;

                if(encarat1 == workpacket.size() || encarat1 == 402  || encarat1 == 472  || encarat1 == 804  || encarat1 == 1206
                        || encarat1 == 1608 || encarat1 == 2010 || encarat1 == 2412
                        || encarat1 == 2814 || encarat1 == 3216 || encarat1 == 3618 || encarat1 == 4020
                        || encarat1 == 4422 || encarat1 == 4824 || encarat1 == 5226 || encarat1 == 5628
                        || encarat1 == 6030 || encarat1 == 6432 || encarat1 == 6834 || encarat1 == 7236
                        || encarat1 == 7638 || encarat1 == 8040 || encarat1 == 8442 || encarat1 == 8844
                        || encarat1 == 9246 || encarat1 == 9648 || encarat1 == 10050)

                {
                    if (enhigh > 0x7 && enlow <= 0xF)
                    {
                        enhigh += 0x80;
                        QByteArray padder = "0";
                        hold4.append(padder);
                        hold4.append(hold4.number(enlow,16));
                    }
                    else
                    {
                        enlow++;
                        hold4 = hold4.number(enlow,16); //get # of zeros
                    }

                    if ((enhigh <= 0x7  ||  enhigh > 0x7) && enlow > 0xF)
                    {
                        enhigh += 0x80;
                    }

                    QByteArray hold3 = hold3.number(enhigh,16);
                    encryptpacket.append(hold3);
                    encryptpacket.append(hold4);
                    encryptpacket.append(encom3);
                    enhigh = 0;
                    enlow = 0;
                    zerocounter = 0;
                }
            }while(encom2 == "00");

            encarat1 -= 2;
        }
    } while(encarat1 < workpacket.size());

    encryptpacket.append("00");
    encom2 = "";
    encarat1 = 0;
    zerocounter = 0;
    enhigh = 0;
    enlow = 0;
    encom3 = "";
    hold4 = "";
    holdit = 0;

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,p13);

    QByteArray returned = Add42ECtoDB(ClientID,ClientsIP,ClientsPort);


    qDebug() << "returned" << returned;

    return encryptpacket;


};

QByteArray xorpacket::packetencrypt(QByteArray workpacket) //send packet here as byte2
{
    encryptpacket.clear();

    do
    {
        encom1 = workpacket.mid(encarat1,2);
        holdit = encom1.toInt(nullptr,16);

        if (holdit != 0x00)
        {
            enhigh++;
            hold2 = workpacket.mid(encarat1,2);

            encarat1 += 2;
            encom3.append(hold2);

            if    (encarat1 == workpacket.size() || encarat1 == 402  || encarat1 == 472  || encarat1 == 804  || encarat1 == 1206
                   || encarat1 == 1608 || encarat1 == 2010 || encarat1 == 2412
                   || encarat1 == 2814 || encarat1 == 3216 || encarat1 == 3618 || encarat1 == 4020
                   || encarat1 == 4422 || encarat1 == 4824 || encarat1 == 5226 || encarat1 == 5628
                   || encarat1 == 6030 || encarat1 == 6432 || encarat1 == 6834 || encarat1 == 7236
                   || encarat1 == 7638 || encarat1 == 8040 || encarat1 == 8442 || encarat1 == 8844
                   || encarat1 == 9246 || encarat1 == 9648 || encarat1 == 10050)

            {
                QByteArray hold3 = hold3.number(enhigh,16); //get # of non zero bytes
                hold4 = hold4.number(enlow,16); //get # of zeros
                encryptpacket.append(hold3);
                encryptpacket.append(hold4);
                encryptpacket.append(encom3);
                enhigh = 0;
                enlow = 0;
                zerocounter = 0;
                encom3 = "";
            }
        }
        else
        {
            if(enhigh > 0x7 && enlow <= 0xF)
            {
                enhigh += 0x80;

                QByteArray padder = "0";
                hold4.append(padder);
                hold4.append(hold4.number(enlow,16));
            }
            else
            {
                hold4 = hold4.number(enlow,16); //get # of zeros
            }

            if ((enhigh <= 0x7  ||  enhigh > 0x7) && enlow > 0xF)
            {
                enhigh += 0x80;
            }

            QByteArray hold3 = hold3.number(enhigh,16); //get # of non zero bytes

            if (encarat1 == 0) //clear everything on first read
            {
                hold3 = "";
                hold4 = "";
            }

            encryptpacket.append(hold3);
            encryptpacket.append(hold4);
            encryptpacket.append(encom3);
            encom3 = "";
            hold4 = "";
            enlow = 0;
            enhigh = 0;
            zerocounter = 0;

            do
            {
                encom2 = workpacket.mid(encarat1,2); //spin and count zeros
                zerocounter++;
                encarat1 += 2;
                enlow = zerocounter - 1;

                if(
                        encarat1 == workpacket.size() || encarat1 == 402  || encarat1 == 472  || encarat1 == 804  || encarat1 == 1206
                        || encarat1 == 1608 || encarat1 == 2010 || encarat1 == 2412
                        || encarat1 == 2814 || encarat1 == 3216 || encarat1 == 3618 || encarat1 == 4020
                        || encarat1 == 4422 || encarat1 == 4824 || encarat1 == 5226 || encarat1 == 5628
                        || encarat1 == 6030 || encarat1 == 6432 || encarat1 == 6834 || encarat1 == 7236
                        || encarat1 == 7638 || encarat1 == 8040 || encarat1 == 8442 || encarat1 == 8844
                        || encarat1 == 9246 || encarat1 == 9648 || encarat1 == 10050)

                {
                    if (enhigh > 0x7 && enlow <= 0xF)
                    {
                        enhigh += 0x80;
                        QByteArray padder = "0";
                        hold4.append(padder);
                        hold4.append(hold4.number(enlow,16));
                    }
                    else
                    {
                        enlow++;
                        hold4 = hold4.number(enlow,16); //get # of zeros
                    }

                    if ((enhigh <= 0x7  ||  enhigh > 0x7) && enlow > 0xF)
                    {
                        enhigh += 0x80;
                    }

                    QByteArray hold3 = hold3.number(enhigh,16);
                    encryptpacket.append(hold3);
                    encryptpacket.append(hold4);
                    encryptpacket.append(encom3);
                    enhigh = 0;
                    enlow = 0;
                    zerocounter = 0;
                }
            }while(encom2 == "00");

            encarat1 -= 2;
        }
    } while(encarat1 < workpacket.size());

    encryptpacket.append("00");
    encom2 = "";
    encarat1 = 0;
    zerocounter = 0;
    enhigh = 0;
    enlow = 0;
    encom3 = "";
    hold4 = "";
    holdit = 0;


    return encryptpacket;

}
//////////////////////////////RECOMPRESSION////////////////////////////


