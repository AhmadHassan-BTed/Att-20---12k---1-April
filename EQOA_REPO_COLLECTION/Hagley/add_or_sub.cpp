#include "add_or_sub.h"
#include "increment.h"
#include "packetparsing.h"
#include "opcodes.h"





ADD_OR_SUB::ADD_OR_SUB(QObject *parent) : QObject(parent)
{

}



QString ADD_OR_SUB::BackToDec(QString Old_Val)
{


    QByteArray Split = Old_Val.toUtf8().toUpper();

    QByteArray Byte_1st = Split.mid(0,2);
    QByteArray Byte_2nd = Split.mid(2,2);
    QByteArray Byte_3rd = Split.mid(4,2);
    QByteArray Byte_4th = Split.mid(6,2);
    QByteArray Byte_5th = Split.mid(8,2);

    int Byte_1st22 = Byte_1st.toInt(nullptr,16);
    int Byte_2nd22 = Byte_2nd.toInt(nullptr,16);
    int Byte_3rd22 = Byte_3rd.toInt(nullptr,16);
    int Byte_4th22 = Byte_4th.toInt(nullptr,16);
    int Byte_5th22 = Byte_5th.toInt(nullptr,16);


    int Dec_Byte1 = 0;
    int Dec_Byte2 = 0;
    int Dec_Byte3 = 0;
    int Dec_Byte4 = 0;
    int Dec_Byte5 = 0;



    if(Byte_1st22 >= 0x80)
    {
        Byte_1st22 = Byte_1st22 - 0x80;
        Byte_1st22 = Byte_1st22 / 2;

        QString Byte_1st4 = QString("%1").arg(Byte_1st22,1,10,QLatin1Char('0')).toUpper();
        Dec_Byte1 = Byte_1st4.toInt(nullptr,10);
    }
    else
    {
        Byte_1st22 = Byte_1st22 / 2;
        QString Byte_1st4 = QString("%1").arg(Byte_1st22,1,10,QLatin1Char('0')).toUpper();
        Dec_Byte1 = Byte_1st4.toInt(nullptr,10);
    }

    if(Byte_2nd22 >= 0x80)
    {
        Byte_2nd22 = Byte_2nd22 - 0x80;
        Byte_2nd22 = Byte_2nd22 * 64;

        QString Byte_2nd4 = QString("%1").arg(Byte_2nd22,1,10,QLatin1Char('0')).toUpper();
        Dec_Byte2 = Byte_2nd4.toInt(nullptr,10);
    }
    else
    {
        Byte_2nd22 = Byte_2nd22 * 64;
        QString Byte_2nd4 = QString("%1").arg(Byte_2nd22,1,10,QLatin1Char('0')).toUpper();
        Dec_Byte2 = Byte_2nd4.toInt(nullptr,10);
    }

    if(Byte_3rd22 >= 0x80)
    {
        Byte_3rd22 = Byte_3rd22 - 0x80;
        Byte_3rd22 = Byte_3rd22 * 8192;

        QString Byte_3rd4 = QString("%1").arg(Byte_3rd22,1,10,QLatin1Char('0')).toUpper();
        Dec_Byte3 = Byte_3rd4.toInt(nullptr,10);
    }
    else
    {
        Byte_3rd22 = Byte_3rd22 * 8192;

        QString Byte_3rd4 = QString("%1").arg(Byte_3rd22,1,10,QLatin1Char('0')).toUpper();
        Dec_Byte3 = Byte_3rd4.toInt(nullptr,10);
    }

    if(Byte_4th22 >= 0x80)
    {
        Byte_4th22 = Byte_4th22 - 0x80;
        Byte_4th22 = Byte_4th22 * 1048576;

        QString Byte_4th4 = QString("%1").arg(Byte_4th22,1,10,QLatin1Char('0')).toUpper();
        Dec_Byte4 = Byte_4th4.toInt(nullptr,10);
    }
    else
    {
        Byte_4th22 = Byte_4th22 * 1048576;

        QString Byte_4th4 = QString("%1").arg(Byte_4th22,1,10,QLatin1Char('0')).toUpper();
        Dec_Byte4 = Byte_4th4.toInt(nullptr,10);
    }

    if(Byte_5th22 >= 0x80)
    {
        Byte_5th22 = Byte_5th22 - 0x80;
        Byte_5th22 = Byte_5th22 * 134217728;

        QString Byte_5th5 = QString("%1").arg(Byte_5th22,1,10,QLatin1Char('0')).toUpper();
        Dec_Byte5 = Byte_5th5.toInt(nullptr,10);
    }
    else
    {
        Byte_5th22 = Byte_5th22 * 134217728;

        QString Byte_5th5 = QString("%1").arg(Byte_5th22,1,10,QLatin1Char('0')).toUpper();
        Dec_Byte5 = Byte_5th5.toInt(nullptr,10);
    }




    int Total_Dec = 0;



    Total_Dec = Dec_Byte1 + Dec_Byte2 + Dec_Byte3 + Dec_Byte4 + Dec_Byte5;

    // this is for negative

    QByteArray NegVal = Split.mid(1,1);

    if(NegVal == "1" || NegVal == "3" || NegVal == "5" || NegVal == "7" || NegVal == "9" ||
            NegVal == "B" || NegVal == "D" || NegVal == "F")
    {
        Total_Dec = Total_Dec + 1;
        QString Total_Dec1 = QString("%1").arg(Total_Dec,1,10,QLatin1Char('0')).toUpper();
        Total_Dec2  = "-" + Total_Dec1;
    }

    // this is for positive
    else
    {
        QString Total_Dec1 = QString("%1").arg(Total_Dec,1,10,QLatin1Char('0')).toUpper();
        Total_Dec2 = Total_Dec1;
    }

    return Total_Dec2;


}

QString ADD_OR_SUB::ConvertFormat(int Randomizer)
{
    int result3 = Randomizer; //get it in decimal
    //qDebug() << "result3" << result3;

    int reverse = result3 / 64; //8240 4097
    //qDebug() << "reverse" << reverse;
    int multiplier = reverse * 64;
    //qDebug() << "multiplier" << multiplier;
    int thirdnumber = result3 - multiplier;
    //qDebug() << "thirdnumber" << thirdnumber;

    QString step0 = QString("%1").arg(reverse,2,16,QLatin1Char('0'));
    //qDebug() << "step0" << step0;
    QByteArray  step1 = step0.toUtf8();
    //qDebug() << "step1" << step1;
    QByteArray step2 = step2.number((thirdnumber * 2) + 0x80,16);
    //qDebug() << "step2" << step2;
    QByteArray Currency = step2+ step1;

    return Currency;
}
QString ADD_OR_SUB::FBSize(QString CurrentFB)
{


    FBText.clear();

    int text_size3 = CurrentFB.size();
    text_size3 = text_size3 / 2;
    QString text_size1 = QString("%1").arg(text_size3,2,16,QLatin1Char('0')).toUpper();
    int text_size = text_size1.size();

    ////qDebug() << "text_size" << text_size;

    if(text_size == 2)
    {
        QByteArray FB_Header = "FBXXxxxx";



        FB_Header.replace(2,2,text_size1.toUtf8().toUpper());
        FBText.append(FB_Header + CurrentFB.toUtf8());
    }

    if(text_size == 3)
    {
        text_size1.insert(0,"0");
        QByteArray FB_Header = "FBFFXXXXxxxx";



        QString Split1 = text_size1.mid(0,2);
        QString Split2 = text_size1.mid(2,2);
        text_size1 = Split2 + Split1;

        FB_Header.replace(4,4,text_size1.toUtf8().toUpper());
        FBText.append(FB_Header + CurrentFB.toUtf8());
    }

    ////qDebug() << "FBText" << FBText.toUpper();

    return FBText.toUpper();
}

QByteArray ADD_OR_SUB::TextSize1(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString CurrentText, QByteArray serveropcode, QByteArray Red_Or_White_Text)
{
    packetparsing::packetvars p5;
    p5 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    FBText.clear();
    QByteArray text_1 = CurrentText.toUtf8();
    QByteArray textascii2 = text_1.toHex();
    QByteArray textascii = textascii2;


    for (int cc = 0; cc < textascii2.size() * 2; cc+=2)
    {
        textascii.insert(cc + 2,"00");
        cc += 2;
    }

    int mysize = textascii.size() / 4;
    QString sizeout3 = QString("%1").arg(mysize,8,16,QLatin1Char('0')).toUpper();

    QByteArray switch_it = sizeout3.toUtf8();
    QByteArray First_Byte  = switch_it.mid(0,2);
    QByteArray Second_Byte = switch_it.mid(2,2);
    QByteArray Third_Byte  = switch_it.mid(4,2);
    QByteArray Fourth_Byte = switch_it.mid(6,2);

    QByteArray Text_Size_rev = Fourth_Byte + Third_Byte + Second_Byte + First_Byte;
    QByteArray Combined = Red_Or_White_Text + Text_Size_rev + textascii; // Add opcode + size + data
    int text_size3 = Combined.size() / 2;

    if (text_size3 < 255)
    {
        QByteArray FB_Header = "FBXXxxxx";
        QString text_size1 = QString("%1").arg(text_size3,2,16,QLatin1Char('0')).toUpper();
        FB_Header.replace(4,4,serveropcode);
        FB_Header.replace(2,2,text_size1.toUtf8());
        FBText.append(FB_Header + Combined);
        return FBText;
    }
    else
    {
        QByteArray FB_Header = "FBFFXXXXxxxx";
        QString text_size1 = QString("%1").arg(text_size3,3,16,QLatin1Char('0')).toUpper();
        text_size1.insert(0,"0");
        FB_Header.replace(8,4,serveropcode);
        QString Split1 = text_size1.mid(0,2);
        QString Split2 = text_size1.mid(2,2);
        text_size1 = Split2 + Split1;
        FB_Header.replace(4,4,text_size1.toUtf8());
        FBText.append(FB_Header + Combined);
        return FBText;
    }

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,p5);
}

QString ADD_OR_SUB::TextSizeNo_00s(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString CurrentText, QString Opcode)
{
    packetparsing::packetvars p5;
    p5 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    FBText.clear();

    OpcodeColor = Opcode.toUtf8();

    //qDebug() << "OpcodeColor" << OpcodeColor;

    QByteArray test1 = CurrentText.toUtf8();
    QByteArray textascii = test1.toHex();



    QString textascii4 = QString::fromUtf8(textascii);
    QByteArray Text_Len = textascii4.toUtf8();

    int mysize = Text_Len.size()/ 2;
    QString sizeout1 = sizeout1.number(mysize,16);

    int sizeout2 = sizeout1.toInt(nullptr,16);

    QString sizeout3 = QString("%1").arg(sizeout2,8,16,QLatin1Char('0')).toUpper();

    QByteArray switch_it = sizeout3.toUtf8();

    QByteArray First_Byte  = switch_it.mid(0,2);
    QByteArray Second_Byte = switch_it.mid(2,2);
    QByteArray Third_Byte  = switch_it.mid(4,2);
    QByteArray Fourth_Byte = switch_it.mid(6,2);


    Text_Size = Fourth_Byte + Third_Byte + Second_Byte + First_Byte;


    QByteArray Combined0 = OpcodeColor +Text_Size + Text_Len;

    //qDebug() << "Combined0" << Combined0;




    return Combined0;
}

float ADD_OR_SUB::DistanceCheck(QByteArray MasterX, QByteArray MasterZ, QByteArray MasterY, QByteArray PLayerX, QByteArray PLayerZ, QByteArray PLayerY)
{
    QByteArray  myx1 = MasterX; //send as 3 byte hex
    int xfromhex1 = myx1.toInt(nullptr,16);
    float maintoonx1 = xfromhex1 / 128.0;

    QByteArray  myz1 = MasterZ;
    int zfromhex1 = myz1.toInt(nullptr,16);
    float maintoonz1 = zfromhex1 / 128.0;

    if(maintoonz1 > 130000)
    {
        maintoonz1 = maintoonz1 - 131072.00195193;
    }

    QByteArray  myy1 = MasterY;
    int yfromhex1 = myy1.toInt(nullptr,16);
    float maintoony1 = yfromhex1 / 128.0;


    // ///////////////////////////////////////////////////////////

    QString tempstrx = PLayerX;
    int tempint1 = tempstrx.toInt(nullptr,16);
    float xinterlude = tempint1 / 128.0;

    QString tempstrz = PLayerZ;
    int tempint3 = tempstrz.toInt(nullptr,16);
    float  zinterlude = tempint3 / 128.0;

    if(zinterlude > 130000)
    {
        zinterlude = zinterlude - 131072.00195193;
    }

    QString tempstry = PLayerY;
    int tempint2 = tempstry.toInt(nullptr,16);
    float yinterlude = tempint2 / 128.0;

    // ///////////////////////////////////////////////////////////


    float sub1 = maintoonx1 - xinterlude;
    float sub2 = maintoony1 - yinterlude;
    float sub3 = maintoonz1 - zinterlude;

    float power1 = qPow(sub1,2);
    float power2 = qPow(sub2,2);
    float power3 = qPow(sub3,2);

    float firstresult = power1 + power2 + power3;
    float result = qSqrt(firstresult);

    return result;
}

QString ADD_OR_SUB::TextSize(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString CurrentText, bool Trimm, QString Opcode, QByteArray Servers_Last_FB_MessageNumber)
{
    packetparsing::packetvars p5;
    p5 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    FBText.clear();

    OpcodeColor = Opcode.toUtf8();

    //qDebug() << "OpcodeColor" << OpcodeColor;

    QByteArray test1 = CurrentText.toUtf8();
    QByteArray textascii2 = test1.toHex();
    QByteArray textascii = textascii2;

    for (int cc = 0; cc < textascii2.size() * 2; cc+=2)
    {
        textascii.insert(cc + 2,"00");
        cc += 2;
    }


    QString textascii4 = QString::fromUtf8(textascii);
    QByteArray Text_Len = textascii4.toUtf8();

    int mysize = Text_Len.size()/ 4;
    QString sizeout1 = sizeout1.number(mysize,16);

    int sizeout2 = sizeout1.toInt(nullptr,16);

    QString sizeout3 = QString("%1").arg(sizeout2,8,16,QLatin1Char('0')).toUpper();

    QByteArray switch_it = sizeout3.toUtf8();

    QByteArray First_Byte  = switch_it.mid(0,2);
    QByteArray Second_Byte = switch_it.mid(2,2);
    QByteArray Third_Byte  = switch_it.mid(4,2);
    QByteArray Fourth_Byte = switch_it.mid(6,2);


    Text_Size = Fourth_Byte + Third_Byte + Second_Byte + First_Byte;


    QByteArray Combined0 = OpcodeColor + Text_Size + Text_Len;

    //qDebug() << "Combined0" << Combined0;

    if(Trimm == false)
    {
        QByteArray Combined = Combined0; // Add opcode
        int text_size3 = Combined.size();
        text_size3 = text_size3 / 2;
        QString text_size1 = QString("%1").arg(text_size3,2,16,QLatin1Char('0')).toUpper();
        int text_size = text_size1.size();

        ////qDebug() << "text_size" << text_size;

        if(text_size == 2)
        {
            QByteArray FB_Header = "FBXXxxxx";

            increment Text;
            QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
            FB_Header.replace(4,4,Text2);
            Servers_Last_FB_MessageNumber = Text2.toUpper();

            FB_Header.replace(2,2,text_size1.toUtf8().toUpper());
            FBText.append(FB_Header + Combined);
        }

        if(text_size == 3)
        {
            text_size1.insert(0,"0");
            QByteArray FB_Header = "FBFFXXXXxxxx";

            increment Text;
            QByteArray Text2 = Text.count(Servers_Last_FB_MessageNumber);
            FB_Header.replace(8,4,Text2);
            Servers_Last_FB_MessageNumber = Text2.toUpper();

            QString Split1 = text_size1.mid(0,2);
            QString Split2 = text_size1.mid(2,2);
            text_size1 = Split2 + Split1;

            FB_Header.replace(4,4,text_size1.toUtf8().toUpper());
            FBText.append(FB_Header + Combined);
        }



        return Servers_Last_FB_MessageNumber + FBText.toUpper();
    }


    return Combined0;

}

QString ADD_OR_SUB::SubforNeg(QString Neg_val)
{
    QString Starting1 = Neg_val;

    QString Byte_1st = Starting1.mid(0,2);
    QString Byte_2nd = Starting1.mid(2,2);
    QString Byte_3rd = Starting1.mid(4,2);
    QString Byte_4th = Starting1.mid(6,2);
    QString Byte_5th = Starting1.mid(8,2);

    int Byte_1st1 = Byte_1st.toInt(nullptr,16);
    int Byte_2nd1 = Byte_2nd.toInt(nullptr,16);
    int Byte_3rd1 = Byte_3rd.toInt(nullptr,16);
    int Byte_4th1 = Byte_4th.toInt(nullptr,16);
    int Byte_5th1 = Byte_5th.toInt(nullptr,16);

    QString Combine_Reverse = "";
    qlonglong Starting2 = 0;

    if(Starting1.size() == 2)
    {

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 4)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 6)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 8)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        if(Byte_3rd1 >= 0x80)
        {
            Byte_3rd1 = Byte_3rd1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 10)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        if(Byte_3rd1 >= 0x80)
        {
            Byte_3rd1 = Byte_3rd1 - 0x80;
        }

        if(Byte_4th1 >= 0x80)
        {
            Byte_4th1 = Byte_4th1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th2 = QString("%1").arg(Byte_5th1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_5th2 + Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    ///////////////////////////////////////////////////////////////////////////////////////




    QString Add_Sub1 = "01";

    QString Byte_1st2 = Add_Sub1.mid(0,2);
    QString Byte_2nd2 = Add_Sub1.mid(2,2);
    QString Byte_3rd2 = Add_Sub1.mid(4,2);
    QString Byte_4th2 = Add_Sub1.mid(6,2);
    QString Byte_5th2 = Add_Sub1.mid(8,2);

    int Byte_1st7 = Byte_1st2.toInt(nullptr,16);
    int Byte_2nd7 = Byte_2nd2.toInt(nullptr,16);
    int Byte_3rd7 = Byte_3rd2.toInt(nullptr,16);
    int Byte_4th7 = Byte_4th2.toInt(nullptr,16);
    int Byte_5th7 = Byte_5th2.toInt(nullptr,16);

    QString Combine_Reverse2 = "";
    qlonglong Add_Sub2 = 0;

    if(Add_Sub1.size() == 2)
    {

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 4)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }
    if(Add_Sub1.size() == 6)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 8)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        if(Byte_3rd7 >= 0x80)
        {
            Byte_3rd7 = Byte_3rd7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 10)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        if(Byte_3rd7 >= 0x80)
        {
            Byte_3rd7 = Byte_3rd7 - 0x80;
        }

        if(Byte_4th7 >= 0x80)
        {
            Byte_4th7 = Byte_4th7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th2 = QString("%1").arg(Byte_5th7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_5th2 + Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    qlonglong Total = 0;

    Total = Starting2 - Add_Sub2; // subtract from wallet

    QString Total2 = QString("%1").arg(Total,10,16,QLatin1Char('0')).toUpper();





    QString Total3 = Total2.toUpper();

    QString Byte_1st3 = Total3.mid(0,2);
    QString Byte_2nd3 = Total3.mid(2,2);
    QString Byte_3rd3 = Total3.mid(4,2);
    QString Byte_4th3 = Total3.mid(6,2);
    QString Byte_5th3 = Total3.mid(8,2);


    QString Combine_Reverse3 = Byte_5th3 + Byte_4th3 + Byte_3rd3 + Byte_2nd3 +  Byte_1st3;

    QString Byte_1st4 = Combine_Reverse3.mid(0,2);
    QString Byte_2nd4 = Combine_Reverse3.mid(2,2);
    QString Byte_3rd4 = Combine_Reverse3.mid(4,2);
    QString Byte_4th4 = Combine_Reverse3.mid(6,2);
    QString Byte_5th4 = Combine_Reverse3.mid(8,2);

    QString Combine_Reverse4;


    int Byte_1st5 = Byte_1st4.toInt(nullptr,16);
    int Byte_2nd5 = Byte_2nd4.toInt(nullptr,16);
    int Byte_3rd5 = Byte_3rd4.toInt(nullptr,16);
    int Byte_4th5 = Byte_4th4.toInt(nullptr,16);
    int Byte_5th5 = Byte_5th4.toInt(nullptr,16);

    if(Combine_Reverse3.size() == 2)
    {

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6;
    }

    if(Combine_Reverse3.size() == 4)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6;
    }

    if(Combine_Reverse3.size() == 6)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6;
    }

    if(Combine_Reverse3.size() == 8)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        if(Byte_3rd5 < 0x80)
        {
            Byte_3rd5 = Byte_3rd5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th6 = QString("%1").arg(Byte_4th5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6 + Byte_4th6;
    }

    if(Combine_Reverse3.size() == 10)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        if(Byte_3rd5 < 0x80)
        {
            Byte_3rd5 = Byte_3rd5 + 0x80;
        }

        if(Byte_4th5 < 0x80)
        {
            Byte_4th5 = Byte_4th5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th6 = QString("%1").arg(Byte_4th5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th6 = QString("%1").arg(Byte_5th5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6 + Byte_4th6 + Byte_5th6;
    }






    ///// Trim the Fat /////


    QByteArray Split = Combine_Reverse4.toUtf8().toUpper();

    Byte_1st = Split.mid(0,2);
    Byte_2nd = Split.mid(2,2);
    Byte_3rd = Split.mid(4,2);
    Byte_4th = Split.mid(6,2);
    Byte_5th = Split.mid(8,2);

    int Byte_1st22 = Byte_1st.toInt(nullptr,16);
    int Byte_2nd22 = Byte_2nd.toInt(nullptr,16);
    int Byte_3rd22 = Byte_3rd.toInt(nullptr,16);
    int Byte_4th22 = Byte_4th.toInt(nullptr,16);
    int Byte_5th22 = Byte_5th.toInt(nullptr,16);

    if(Byte_5th22 == 0x00 && Byte_4th22 == 0x80 && Byte_3rd22 == 0x80 && Byte_2nd22 == 0x80)
    {
        int manipulate1 = Byte_1st22;

        if(manipulate1 >= 0x80)
        {
            manipulate1 = manipulate1 - 0x80;
        }

        int manipulate2 = Byte_2nd22;
        int manipulate3 = Byte_3rd22;
        int manipulate4 = Byte_4th22;
        int manipulate5 = Byte_5th22;

        QString Byte_1st3 = QString("%1").arg(manipulate1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd3 = QString("%1").arg(manipulate2,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd3 = QString("%1").arg(manipulate3,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th3 = QString("%1").arg(manipulate4,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th3 = QString("%1").arg(manipulate5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st3;
    }

    if(Byte_5th22 == 0x00 && Byte_4th22 == 0x80 && Byte_3rd22 == 0x80 && Byte_2nd22 > 0x80)
    {
        int manipulate1 = Byte_1st22;
        int manipulate2 = Byte_2nd22 - 0x80;
        int manipulate3 = Byte_3rd22;
        int manipulate4 = Byte_4th22;
        int manipulate5 = Byte_5th22;

        QString Byte_1st3 = QString("%1").arg(manipulate1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd3 = QString("%1").arg(manipulate2,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd3 = QString("%1").arg(manipulate3,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th3 = QString("%1").arg(manipulate4,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th3 = QString("%1").arg(manipulate5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st3 + Byte_2nd3;
    }

    if(Byte_5th22 == 0x00 && Byte_4th22 == 0x80 && Byte_3rd22 > 0x80)
    {
        int manipulate1 = Byte_1st22;
        int manipulate2 = Byte_2nd22;
        int manipulate3 = Byte_3rd22 - 0x80;
        int manipulate4 = Byte_4th22;
        int manipulate5 = Byte_5th22;

        QString Byte_1st3 = QString("%1").arg(manipulate1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd3 = QString("%1").arg(manipulate2,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd3 = QString("%1").arg(manipulate3,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th3 = QString("%1").arg(manipulate4,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th3 = QString("%1").arg(manipulate5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st3 + Byte_2nd3 + Byte_3rd3;
    }

    if(Byte_5th22 == 0x00 && Byte_4th22 > 0x80)
    {
        int manipulate1 = Byte_1st22;
        int manipulate2 = Byte_2nd22;
        int manipulate3 = Byte_3rd22;
        int manipulate4 = Byte_4th22 - 0x80;
        int manipulate5 = Byte_5th22;

        QString Byte_1st3 = QString("%1").arg(manipulate1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd3 = QString("%1").arg(manipulate2,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd3 = QString("%1").arg(manipulate3,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th3 = QString("%1").arg(manipulate4,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th3 = QString("%1").arg(manipulate5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st3 + Byte_2nd3 + Byte_3rd3 + Byte_4th3;
    }



    return Combine_Reverse4;

}

QString ADD_OR_SUB::AddtoWallet(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString Currency3,QByteArray Servers_Last_FB_MessageNumber)
{
    packetparsing::packetvars p5;
    p5 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QString Starting1 = p5.Cash_on_hand;

    QString Byte_1st = Starting1.mid(0,2);
    QString Byte_2nd = Starting1.mid(2,2);
    QString Byte_3rd = Starting1.mid(4,2);
    QString Byte_4th = Starting1.mid(6,2);
    QString Byte_5th = Starting1.mid(8,2);

    int Byte_1st1 = Byte_1st.toInt(nullptr,16);
    int Byte_2nd1 = Byte_2nd.toInt(nullptr,16);
    int Byte_3rd1 = Byte_3rd.toInt(nullptr,16);
    int Byte_4th1 = Byte_4th.toInt(nullptr,16);
    int Byte_5th1 = Byte_5th.toInt(nullptr,16);

    QString Combine_Reverse = "";
    qlonglong Starting2 = 0;

    // if the Starting1 (p5.Cash_on_hand) is only 1 byte
    if(Starting1.size() == 2)
    {

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    // if the Starting1 (p5.Cash_on_hand) is 2 bytes
    if(Starting1.size() == 4)
    {
        if(Byte_1st1 >= 0x80)
        {
            //removes the 80 if its there
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    // if the Starting1 (p5.Cash_on_hand) is 3 bytes
    if(Starting1.size() == 6)
    {
        if(Byte_1st1 >= 0x80)
        {
            //removes the 80 if its there
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            //removes the 80 if its there
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    // if the Starting1 (p5.Cash_on_hand) is 4 bytes
    if(Starting1.size() == 8)
    {
        if(Byte_1st1 >= 0x80)
        {
            //removes the 80 if its there
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            //removes the 80 if its there
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        if(Byte_3rd1 >= 0x80)
        {
            //removes the 80 if its there
            Byte_3rd1 = Byte_3rd1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    // if the Starting1 (p5.Cash_on_hand) is 5 bytes
    if(Starting1.size() == 10)
    {
        if(Byte_1st1 >= 0x80)
        {
            //removes the 80 if its there
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            //removes the 80 if its there
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        if(Byte_3rd1 >= 0x80)
        {
            //removes the 80 if its there
            Byte_3rd1 = Byte_3rd1 - 0x80;
        }

        if(Byte_4th1 >= 0x80)
        {
            //removes the 80 if its there
            Byte_4th1 = Byte_4th1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th2 = QString("%1").arg(Byte_5th1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_5th2 + Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    ///////////////////////////////////////////////////////////////////////////////////////



    // ////// This part is to convert the new value that we are adding.
    // ////// like we already did to what we started with in our wallet.

    QString Add_Sub1 = Currency3;

    QString Byte_1st2 = Add_Sub1.mid(0,2);
    QString Byte_2nd2 = Add_Sub1.mid(2,2);
    QString Byte_3rd2 = Add_Sub1.mid(4,2);
    QString Byte_4th2 = Add_Sub1.mid(6,2);
    QString Byte_5th2 = Add_Sub1.mid(8,2);

    int Byte_1st7 = Byte_1st2.toInt(nullptr,16);
    int Byte_2nd7 = Byte_2nd2.toInt(nullptr,16);
    int Byte_3rd7 = Byte_3rd2.toInt(nullptr,16);
    int Byte_4th7 = Byte_4th2.toInt(nullptr,16);
    int Byte_5th7 = Byte_5th2.toInt(nullptr,16);

    QString Combine_Reverse2 = "";
    qlonglong Add_Sub2 = 0;

    // if the Currency3 is only 1 byte
    if(Add_Sub1.size() == 2)
    {

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    // if the Currency3 (Value to add) is 2 bytes
    if(Add_Sub1.size() == 4)
    {
        if(Byte_1st7 >= 0x80)
        {
            //removes the 80 if its there
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    // if the Currency3 (Value to add) is 3 bytes
    if(Add_Sub1.size() == 6)
    {
        if(Byte_1st7 >= 0x80)
        {
            //removes the 80 if its there
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            //removes the 80 if its there
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    // if the Currency3 (Value to add) is 4 bytes
    if(Add_Sub1.size() == 8)
    {
        if(Byte_1st7 >= 0x80)
        {
            //removes the 80 if its there
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            //removes the 80 if its there
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        if(Byte_3rd7 >= 0x80)
        {
            //removes the 80 if its there
            Byte_3rd7 = Byte_3rd7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    // if the Currency3 (Value to add) is 5 bytes
    if(Add_Sub1.size() == 10)
    {
        if(Byte_1st7 >= 0x80)
        {
            //removes the 80 if its there
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            //removes the 80 if its there
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        if(Byte_3rd7 >= 0x80)
        {
            //removes the 80 if its there
            Byte_3rd7 = Byte_3rd7 - 0x80;
        }

        if(Byte_4th7 >= 0x80)
        {
            //removes the 80 if its there
            Byte_4th7 = Byte_4th7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th2 = QString("%1").arg(Byte_5th7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_5th2 + Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }






    qlonglong Total = 0;

    // Starting2 is what you are starting with in your wallet
    // Add_Sub2 is what you are adding to your wallet

    Total = Starting2 + Add_Sub2; // add to wallet

    if(Total >= 128)
    {
        // this is for when it first passes 64 on the first byte otherwise it will get stuck in a loop
        Total = Total + 0x80;
    }

    QString Total2 = QString("%1").arg(Total,10,16,QLatin1Char('0')).toUpper();

    //qDebug() << "Starting2" << Starting2;
    //qDebug() << "Add_Sub2" << Add_Sub2;
    //qDebug() << "Total2" << Total2;


    QString Total3 = Total2.toUpper();

    /////////////////////////////////////////////////////////////////////////////////////////

    // /////// This is just to reverse it back to how it was when we started
    // /////// or else it will be backwards.

    QString Byte_1st3 = Total3.mid(0,2);
    QString Byte_2nd3 = Total3.mid(2,2);
    QString Byte_3rd3 = Total3.mid(4,2);
    QString Byte_4th3 = Total3.mid(6,2);
    QString Byte_5th3 = Total3.mid(8,2);


    QString Combine_Reverse3 = Byte_5th3 + Byte_4th3 + Byte_3rd3 + Byte_2nd3 +  Byte_1st3;

    QString Byte_1st4 = Combine_Reverse3.mid(0,2);
    QString Byte_2nd4 = Combine_Reverse3.mid(2,2);
    QString Byte_3rd4 = Combine_Reverse3.mid(4,2);
    QString Byte_4th4 = Combine_Reverse3.mid(6,2);
    QString Byte_5th4 = Combine_Reverse3.mid(8,2);

    QString Combine_Reverse4 = "";


    int Byte_1st5 = Byte_1st4.toInt(nullptr,16);
    int Byte_2nd5 = Byte_2nd4.toInt(nullptr,16);
    int Byte_3rd5 = Byte_3rd4.toInt(nullptr,16);
    int Byte_4th5 = Byte_4th4.toInt(nullptr,16);
    int Byte_5th5 = Byte_5th4.toInt(nullptr,16);

    // if the total is only 1 byte
    if(Combine_Reverse3.size() == 2)
    {

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6;
    }

    // if the total is 2 bytes
    if(Combine_Reverse3.size() == 4)
    {
        if(Byte_1st5 < 0x80)
        {
            //Adds the 80 back if its not already there
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6;
    }

    // if the total is 3 bytes
    if(Combine_Reverse3.size() == 6)
    {
        if(Byte_1st5 < 0x80)
        {
            //Adds the 80 back if its not already there
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            //Adds the 80 back if its not already there
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6;
    }

    // if the total is 4 bytes
    if(Combine_Reverse3.size() == 8)
    {
        if(Byte_1st5 < 0x80)
        {
            //Adds the 80 back if its not already there
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            //Adds the 80 back if its not already there
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        if(Byte_3rd5 < 0x80)
        {
            //Adds the 80 back if its not already there
            Byte_3rd5 = Byte_3rd5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th6 = QString("%1").arg(Byte_4th5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6 + Byte_4th6;
    }

    // if the total is 5 bytes
    if(Combine_Reverse3.size() == 10)
    {
        if(Byte_1st5 < 0x80)
        {
            //Adds the 80 back if its not already there
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            //Adds the 80 back if its not already there
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        if(Byte_3rd5 < 0x80)
        {
            //Adds the 80 back if its not already there
            Byte_3rd5 = Byte_3rd5 + 0x80;
        }

        if(Byte_4th5 < 0x80)
        {
            //Adds the 80 back if its not already there
            Byte_4th5 = Byte_4th5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th6 = QString("%1").arg(Byte_4th5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th6 = QString("%1").arg(Byte_5th5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6 + Byte_4th6 + Byte_5th6;
    }



    p5.Cash_on_hand = Combine_Reverse4;

    // im sure you can figure this part out

    int Pocket_size = Combine_Reverse4.size() /2 +2;
    QString Pocket_size1 = QString("%1").arg(Pocket_size,2,16,QLatin1Char('0')).toUpper();
    QByteArray Pocket_opcode = "fb04xxxx5200";

    increment Bank_tunar1;
    QByteArray Bank_tunar2 = Bank_tunar1.count(Servers_Last_FB_MessageNumber); //2nd opcode increment
    Pocket_opcode.replace(4,4,Bank_tunar2); //replace new count at 4
    Servers_Last_FB_MessageNumber = Bank_tunar2.toUpper(); //replace new count


    Pocket_opcode.append(Combine_Reverse4.toUtf8());
    Pocket_opcode.replace(2,2,Pocket_size1.toUtf8().toUpper());

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,p5);

    return Pocket_opcode;

}

// the rest are the same thing //
QString ADD_OR_SUB::SubfromWallet(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString Currency4,QByteArray Servers_Last_FB_MessageNumber)
{
    packetparsing::packetvars p5;
    p5 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QString Starting1 = p5.Cash_on_hand;

    QString Byte_1st = Starting1.mid(0,2);
    QString Byte_2nd = Starting1.mid(2,2);
    QString Byte_3rd = Starting1.mid(4,2);
    QString Byte_4th = Starting1.mid(6,2);
    QString Byte_5th = Starting1.mid(8,2);

    int Byte_1st1 = Byte_1st.toInt(nullptr,16);
    int Byte_2nd1 = Byte_2nd.toInt(nullptr,16);
    int Byte_3rd1 = Byte_3rd.toInt(nullptr,16);
    int Byte_4th1 = Byte_4th.toInt(nullptr,16);
    int Byte_5th1 = Byte_5th.toInt(nullptr,16);

    QString Combine_Reverse = "";
    qlonglong Starting2 = 0;

    if(Starting1.size() == 2)
    {

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 4)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 6)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 8)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        if(Byte_3rd1 >= 0x80)
        {
            Byte_3rd1 = Byte_3rd1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 10)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        if(Byte_3rd1 >= 0x80)
        {
            Byte_3rd1 = Byte_3rd1 - 0x80;
        }

        if(Byte_4th1 >= 0x80)
        {
            Byte_4th1 = Byte_4th1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th2 = QString("%1").arg(Byte_5th1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_5th2 + Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    ///////////////////////////////////////////////////////////////////////////////////////




    QString Add_Sub1 = Currency4;

    QString Byte_1st2 = Add_Sub1.mid(0,2);
    QString Byte_2nd2 = Add_Sub1.mid(2,2);
    QString Byte_3rd2 = Add_Sub1.mid(4,2);
    QString Byte_4th2 = Add_Sub1.mid(6,2);
    QString Byte_5th2 = Add_Sub1.mid(8,2);

    int Byte_1st7 = Byte_1st2.toInt(nullptr,16);
    int Byte_2nd7 = Byte_2nd2.toInt(nullptr,16);
    int Byte_3rd7 = Byte_3rd2.toInt(nullptr,16);
    int Byte_4th7 = Byte_4th2.toInt(nullptr,16);
    int Byte_5th7 = Byte_5th2.toInt(nullptr,16);

    QString Combine_Reverse2 = "";
    qlonglong Add_Sub2 = 0;

    if(Add_Sub1.size() == 2)
    {

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 4)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }
    if(Add_Sub1.size() == 6)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 8)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        if(Byte_3rd7 >= 0x80)
        {
            Byte_3rd7 = Byte_3rd7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 10)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        if(Byte_3rd7 >= 0x80)
        {
            Byte_3rd7 = Byte_3rd7 - 0x80;
        }

        if(Byte_4th7 >= 0x80)
        {
            Byte_4th7 = Byte_4th7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th2 = QString("%1").arg(Byte_5th7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_5th2 + Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    qlonglong Total = 0;

    Total = Starting2 - Add_Sub2; // subtract from wallet

    QString Total2 = QString("%1").arg(Total,10,16,QLatin1Char('0')).toUpper();





    QString Total3 = Total2.toUpper();

    QString Byte_1st3 = Total3.mid(0,2);
    QString Byte_2nd3 = Total3.mid(2,2);
    QString Byte_3rd3 = Total3.mid(4,2);
    QString Byte_4th3 = Total3.mid(6,2);
    QString Byte_5th3 = Total3.mid(8,2);


    QString Combine_Reverse3 = Byte_5th3 + Byte_4th3 + Byte_3rd3 + Byte_2nd3 +  Byte_1st3;

    QString Byte_1st4 = Combine_Reverse3.mid(0,2);
    QString Byte_2nd4 = Combine_Reverse3.mid(2,2);
    QString Byte_3rd4 = Combine_Reverse3.mid(4,2);
    QString Byte_4th4 = Combine_Reverse3.mid(6,2);
    QString Byte_5th4 = Combine_Reverse3.mid(8,2);

    QString Combine_Reverse4;


    int Byte_1st5 = Byte_1st4.toInt(nullptr,16);
    int Byte_2nd5 = Byte_2nd4.toInt(nullptr,16);
    int Byte_3rd5 = Byte_3rd4.toInt(nullptr,16);
    int Byte_4th5 = Byte_4th4.toInt(nullptr,16);
    int Byte_5th5 = Byte_5th4.toInt(nullptr,16);

    if(Combine_Reverse3.size() == 2)
    {

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6;
    }

    if(Combine_Reverse3.size() == 4)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6;
    }

    if(Combine_Reverse3.size() == 6)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6;
    }

    if(Combine_Reverse3.size() == 8)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        if(Byte_3rd5 < 0x80)
        {
            Byte_3rd5 = Byte_3rd5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th6 = QString("%1").arg(Byte_4th5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6 + Byte_4th6;
    }

    if(Combine_Reverse3.size() == 10)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        if(Byte_3rd5 < 0x80)
        {
            Byte_3rd5 = Byte_3rd5 + 0x80;
        }

        if(Byte_4th5 < 0x80)
        {
            Byte_4th5 = Byte_4th5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th6 = QString("%1").arg(Byte_4th5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th6 = QString("%1").arg(Byte_5th5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6 + Byte_4th6 + Byte_5th6;
    }



    p5.Cash_on_hand = Combine_Reverse4;

    int Pocket_size = Combine_Reverse4.size() /2 +2;
    QString Pocket_size1 = QString("%1").arg(Pocket_size,2,16,QLatin1Char('0')).toUpper();
    QByteArray Pocket_opcode = "fb04xxxx5200";

    increment Bank_tunar1;
    QByteArray Bank_tunar2 = Bank_tunar1.count(Servers_Last_FB_MessageNumber); //2nd opcode increment
    Pocket_opcode.replace(4,4,Bank_tunar2); //replace new count at 4
    Servers_Last_FB_MessageNumber = Bank_tunar2.toUpper(); //replace new count


    Pocket_opcode.append(Combine_Reverse4.toUtf8());
    Pocket_opcode.replace(2,2,Pocket_size1.toUtf8().toUpper());

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,p5);

    return Pocket_opcode;

}


QString ADD_OR_SUB::Withdraw(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString Currency1,QByteArray Servers_Last_FB_MessageNumber)
{
    packetparsing::packetvars p5;
    p5 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QString Starting1 = p5.Current_Available_balance;

    QString Byte_1st = Starting1.mid(0,2);
    QString Byte_2nd = Starting1.mid(2,2);
    QString Byte_3rd = Starting1.mid(4,2);
    QString Byte_4th = Starting1.mid(6,2);
    QString Byte_5th = Starting1.mid(8,2);

    int Byte_1st1 = Byte_1st.toInt(nullptr,16);
    int Byte_2nd1 = Byte_2nd.toInt(nullptr,16);
    int Byte_3rd1 = Byte_3rd.toInt(nullptr,16);
    int Byte_4th1 = Byte_4th.toInt(nullptr,16);
    int Byte_5th1 = Byte_5th.toInt(nullptr,16);

    QString Combine_Reverse = "";
    qlonglong Starting2 = 0;

    if(Starting1.size() == 2)
    {

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 4)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 6)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 8)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        if(Byte_3rd1 >= 0x80)
        {
            Byte_3rd1 = Byte_3rd1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 10)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        if(Byte_3rd1 >= 0x80)
        {
            Byte_3rd1 = Byte_3rd1 - 0x80;
        }

        if(Byte_4th1 >= 0x80)
        {
            Byte_4th1 = Byte_4th1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th2 = QString("%1").arg(Byte_5th1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_5th2 + Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    ///////////////////////////////////////////////////////////////////////////////////////




    QString Add_Sub1 = Currency1;

    QString Byte_1st2 = Add_Sub1.mid(0,2);
    QString Byte_2nd2 = Add_Sub1.mid(2,2);
    QString Byte_3rd2 = Add_Sub1.mid(4,2);
    QString Byte_4th2 = Add_Sub1.mid(6,2);
    QString Byte_5th2 = Add_Sub1.mid(8,2);

    int Byte_1st7 = Byte_1st2.toInt(nullptr,16);
    int Byte_2nd7 = Byte_2nd2.toInt(nullptr,16);
    int Byte_3rd7 = Byte_3rd2.toInt(nullptr,16);
    int Byte_4th7 = Byte_4th2.toInt(nullptr,16);
    int Byte_5th7 = Byte_5th2.toInt(nullptr,16);

    QString Combine_Reverse2 = "";
    qlonglong Add_Sub2 = 0;

    if(Add_Sub1.size() == 2)
    {

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 4)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }
    if(Add_Sub1.size() == 6)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 8)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        if(Byte_3rd7 >= 0x80)
        {
            Byte_3rd7 = Byte_3rd7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 10)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        if(Byte_3rd7 >= 0x80)
        {
            Byte_3rd7 = Byte_3rd7 - 0x80;
        }

        if(Byte_4th7 >= 0x80)
        {
            Byte_4th7 = Byte_4th7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th2 = QString("%1").arg(Byte_5th7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_5th2 + Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    qlonglong Total = 0;

    Total = Starting2 - Add_Sub2; // adding to the bank

    QString Total2 = QString("%1").arg(Total,10,16,QLatin1Char('0')).toUpper();





    QString Total3 = Total2.toUpper();

    QString Byte_1st3 = Total3.mid(0,2);
    QString Byte_2nd3 = Total3.mid(2,2);
    QString Byte_3rd3 = Total3.mid(4,2);
    QString Byte_4th3 = Total3.mid(6,2);
    QString Byte_5th3 = Total3.mid(8,2);


    QString Combine_Reverse3 = Byte_5th3 + Byte_4th3 + Byte_3rd3 + Byte_2nd3 +  Byte_1st3;

    QString Byte_1st4 = Combine_Reverse3.mid(0,2);
    QString Byte_2nd4 = Combine_Reverse3.mid(2,2);
    QString Byte_3rd4 = Combine_Reverse3.mid(4,2);
    QString Byte_4th4 = Combine_Reverse3.mid(6,2);
    QString Byte_5th4 = Combine_Reverse3.mid(8,2);

    QString Combine_Reverse4;


    int Byte_1st5 = Byte_1st4.toInt(nullptr,16);
    int Byte_2nd5 = Byte_2nd4.toInt(nullptr,16);
    int Byte_3rd5 = Byte_3rd4.toInt(nullptr,16);
    int Byte_4th5 = Byte_4th4.toInt(nullptr,16);
    int Byte_5th5 = Byte_5th4.toInt(nullptr,16);

    if(Combine_Reverse3.size() == 2)
    {

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6;
    }

    if(Combine_Reverse3.size() == 4)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6;
    }

    if(Combine_Reverse3.size() == 6)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6;
    }

    if(Combine_Reverse3.size() == 8)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        if(Byte_3rd5 < 0x80)
        {
            Byte_3rd5 = Byte_3rd5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th6 = QString("%1").arg(Byte_4th5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6 + Byte_4th6;
    }

    if(Combine_Reverse3.size() == 10)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        if(Byte_3rd5 < 0x80)
        {
            Byte_3rd5 = Byte_3rd5 + 0x80;
        }

        if(Byte_4th5 < 0x80)
        {
            Byte_4th5 = Byte_4th5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th6 = QString("%1").arg(Byte_4th5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th6 = QString("%1").arg(Byte_5th5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6 + Byte_4th6 + Byte_5th6;
    }



    p5.Current_Available_balance = Combine_Reverse4;

    int Bank_size = Combine_Reverse4.size() /2 +2;
    QString Bank_size1 = QString("%1").arg(Bank_size,2,16,QLatin1Char('0')).toUpper();
    QByteArray Bank_opcode = "fb04xxxx5312";

    increment Bank_tunar1;
    QByteArray Bank_tunar2 = Bank_tunar1.count(Servers_Last_FB_MessageNumber); //1st opcode increment
    Bank_opcode.replace(4,4,Bank_tunar2); //replace new count at 4
    Servers_Last_FB_MessageNumber = Bank_tunar2.toUpper(); //replace new count


    Bank_opcode.append(Combine_Reverse4.toUtf8());
    Bank_opcode.replace(2,2,Bank_size1.toUtf8().toUpper());

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,p5);

    return Bank_opcode;

}


QString ADD_OR_SUB::Deposit(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString Currency2,QByteArray Servers_Last_FB_MessageNumber)
{
    packetparsing::packetvars p5;
    p5 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QString Starting1 = p5.Current_Available_balance;

    QString Byte_1st = Starting1.mid(0,2);
    QString Byte_2nd = Starting1.mid(2,2);
    QString Byte_3rd = Starting1.mid(4,2);
    QString Byte_4th = Starting1.mid(6,2);
    QString Byte_5th = Starting1.mid(8,2);

    int Byte_1st1 = Byte_1st.toInt(nullptr,16);
    int Byte_2nd1 = Byte_2nd.toInt(nullptr,16);
    int Byte_3rd1 = Byte_3rd.toInt(nullptr,16);
    int Byte_4th1 = Byte_4th.toInt(nullptr,16);
    int Byte_5th1 = Byte_5th.toInt(nullptr,16);

    QString Combine_Reverse = "";
    qlonglong Starting2 = 0;

    if(Starting1.size() == 2)
    {

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 4)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 6)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 8)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        if(Byte_3rd1 >= 0x80)
        {
            Byte_3rd1 = Byte_3rd1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    if(Starting1.size() == 10)
    {
        if(Byte_1st1 >= 0x80)
        {
            Byte_1st1 = Byte_1st1 - 0x80;
        }

        if(Byte_2nd1 >= 0x80)
        {
            Byte_2nd1 = Byte_2nd1 - 0x80;
        }

        if(Byte_3rd1 >= 0x80)
        {
            Byte_3rd1 = Byte_3rd1 - 0x80;
        }

        if(Byte_4th1 >= 0x80)
        {
            Byte_4th1 = Byte_4th1 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th2 = QString("%1").arg(Byte_5th1,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse = Byte_5th2 + Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Starting2 = Combine_Reverse.toLongLong(nullptr,16);
    }

    ///////////////////////////////////////////////////////////////////////////////////////




    QString Add_Sub1 = Currency2;

    QString Byte_1st2 = Add_Sub1.mid(0,2);
    QString Byte_2nd2 = Add_Sub1.mid(2,2);
    QString Byte_3rd2 = Add_Sub1.mid(4,2);
    QString Byte_4th2 = Add_Sub1.mid(6,2);
    QString Byte_5th2 = Add_Sub1.mid(8,2);

    int Byte_1st7 = Byte_1st2.toInt(nullptr,16);
    int Byte_2nd7 = Byte_2nd2.toInt(nullptr,16);
    int Byte_3rd7 = Byte_3rd2.toInt(nullptr,16);
    int Byte_4th7 = Byte_4th2.toInt(nullptr,16);
    int Byte_5th7 = Byte_5th2.toInt(nullptr,16);

    QString Combine_Reverse2 = "";
    qlonglong Add_Sub2 = 0;

    if(Add_Sub1.size() == 2)
    {

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 4)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }
    if(Add_Sub1.size() == 6)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 8)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        if(Byte_3rd7 >= 0x80)
        {
            Byte_3rd7 = Byte_3rd7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    if(Add_Sub1.size() == 10)
    {
        if(Byte_1st7 >= 0x80)
        {
            Byte_1st7 = Byte_1st7 - 0x80;
        }

        if(Byte_2nd7 >= 0x80)
        {
            Byte_2nd7 = Byte_2nd7 - 0x80;
        }

        if(Byte_3rd7 >= 0x80)
        {
            Byte_3rd7 = Byte_3rd7 - 0x80;
        }

        if(Byte_4th7 >= 0x80)
        {
            Byte_4th7 = Byte_4th7 - 0x80;
        }

        QString Byte_1st2 = QString("%1").arg(Byte_1st7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd2 = QString("%1").arg(Byte_2nd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd2 = QString("%1").arg(Byte_3rd7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th2 = QString("%1").arg(Byte_4th7,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th2 = QString("%1").arg(Byte_5th7,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse2 = Byte_5th2 + Byte_4th2 + Byte_3rd2 + Byte_2nd2 + Byte_1st2;
        Add_Sub2 = Combine_Reverse2.toLongLong(nullptr,16);
    }

    qlonglong Total = 0;

    Total = Starting2 + Add_Sub2; // adding to the bank

    QString Total2 = QString("%1").arg(Total,10,16,QLatin1Char('0')).toUpper();







    QString Total3 = Total2.toUpper();

    QString Byte_1st3 = Total3.mid(0,2);
    QString Byte_2nd3 = Total3.mid(2,2);
    QString Byte_3rd3 = Total3.mid(4,2);
    QString Byte_4th3 = Total3.mid(6,2);
    QString Byte_5th3 = Total3.mid(8,2);


    QString Combine_Reverse3 = Byte_5th3 + Byte_4th3 + Byte_3rd3 + Byte_2nd3 +  Byte_1st3;

    QString Byte_1st4 = Combine_Reverse3.mid(0,2);
    QString Byte_2nd4 = Combine_Reverse3.mid(2,2);
    QString Byte_3rd4 = Combine_Reverse3.mid(4,2);
    QString Byte_4th4 = Combine_Reverse3.mid(6,2);
    QString Byte_5th4 = Combine_Reverse3.mid(8,2);

    QString Combine_Reverse4;


    int Byte_1st5 = Byte_1st4.toInt(nullptr,16);
    int Byte_2nd5 = Byte_2nd4.toInt(nullptr,16);
    int Byte_3rd5 = Byte_3rd4.toInt(nullptr,16);
    int Byte_4th5 = Byte_4th4.toInt(nullptr,16);
    int Byte_5th5 = Byte_5th4.toInt(nullptr,16);

    if(Combine_Reverse3.size() == 2)
    {

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6;
    }

    if(Combine_Reverse3.size() == 4)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6;
    }

    if(Combine_Reverse3.size() == 6)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6;
    }

    if(Combine_Reverse3.size() == 8)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        if(Byte_3rd5 < 0x80)
        {
            Byte_3rd5 = Byte_3rd5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th6 = QString("%1").arg(Byte_4th5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6 + Byte_4th6;
    }

    if(Combine_Reverse3.size() == 10)
    {
        if(Byte_1st5 < 0x80)
        {
            Byte_1st5 = Byte_1st5 + 0x80;
        }

        if(Byte_2nd5 < 0x80)
        {
            Byte_2nd5 = Byte_2nd5 + 0x80;
        }

        if(Byte_3rd5 < 0x80)
        {
            Byte_3rd5 = Byte_3rd5 + 0x80;
        }

        if(Byte_4th5 < 0x80)
        {
            Byte_4th5 = Byte_4th5 + 0x80;
        }

        QString Byte_1st6 = QString("%1").arg(Byte_1st5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd6 = QString("%1").arg(Byte_2nd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd6 = QString("%1").arg(Byte_3rd5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th6 = QString("%1").arg(Byte_4th5,2,16,QLatin1Char('0')).toUpper();
        QString Byte_5th6 = QString("%1").arg(Byte_5th5,2,16,QLatin1Char('0')).toUpper();

        Combine_Reverse4 = Byte_1st6 + Byte_2nd6 + Byte_3rd6 + Byte_4th6 + Byte_5th6;
    }



    p5.Current_Available_balance = Combine_Reverse4;

    int Bank_size = Combine_Reverse4.size() /2 +2;
    QString Bank_size1 = QString("%1").arg(Bank_size,2,16,QLatin1Char('0')).toUpper();
    QByteArray Bank_opcode = "fb04xxxx5312";

    increment Bank_tunar1;
    QByteArray Bank_tunar2 = Bank_tunar1.count(Servers_Last_FB_MessageNumber); //1st opcode increment
    Bank_opcode.replace(4,4,Bank_tunar2); //replace new count at 4
    Servers_Last_FB_MessageNumber = Bank_tunar2.toUpper(); //replace new count


    Bank_opcode.append(Combine_Reverse4.toUtf8());
    Bank_opcode.replace(2,2,Bank_size1.toUtf8().toUpper());

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,p5);

    return Bank_opcode;

}
