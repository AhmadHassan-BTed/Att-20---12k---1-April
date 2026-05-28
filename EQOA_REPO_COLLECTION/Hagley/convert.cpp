#include "convert.h"
#include "checkthezone.h"

QVector <QString> checkthezone::TunariaZoneNames;

///////////////////////////////////////

QVector <QString> checkthezone::RatheZoneNames;

///////////////////////////////////////

QVector <QString> checkthezone::OdusZoneNames;

///////////////////////////////////////

QVector <QString> checkthezone::LSMZoneNames;

///////////////////////////////////////

QVector <QString> checkthezone::PoSZoneNames;

///////////////////////////////////////

QVector <QString> checkthezone::SecretsZoneNames;

///////////////////////////////////////



QVector <QVector<float>> checkthezone::TunariaZoneCentralVectors;

///////////////////////////////////////

QVector <QVector<float>> checkthezone::RatheZoneCentralVectors;

///////////////////////////////////////

QVector <QVector<float>> checkthezone::OdusZoneCentralVectors;


///////////////////////////////////////

QVector <QVector<float>> checkthezone::LSMZoneCentralVectors;

///////////////////////////////////////

QVector <QVector<float>> checkthezone::PoSZoneCentralVectors;

///////////////////////////////////////

QVector <QVector<float>> checkthezone::SecretZoneCentralVectors;

///////////////////////////////////////

QMap <QString,QString> checkthezone::NPCsNames;
QMap <QString,int> checkthezone::IDsAndWorlds;

QVector <QVector <QVector<float>>> checkthezone::VectorOfWorlds =
{
    checkthezone::TunariaZoneCentralVectors,checkthezone::RatheZoneCentralVectors,
    checkthezone::OdusZoneCentralVectors,checkthezone::LSMZoneCentralVectors,
    checkthezone::PoSZoneCentralVectors,checkthezone::SecretZoneCentralVectors
};

///////////////////////////////////////

QVector<QVector<QString>> checkthezone::WorldZoneNamesVector =
{
    checkthezone::TunariaZoneNames,checkthezone::RatheZoneNames,
    checkthezone::OdusZoneNames,checkthezone::LSMZoneNames,
    checkthezone::PoSZoneNames,checkthezone::SecretsZoneNames
};

///////////////////////////////////////

QMap <QString,QVector <QString>> checkthezone::ZoneLocationsIDsVectorMap;

QVector<QMap <QString,QVector <QString>>> checkthezone::VectorOFWorldsZones =
{
    checkthezone::ZoneLocationsIDsVectorMap,checkthezone::ZoneLocationsIDsVectorMap,
    checkthezone::ZoneLocationsIDsVectorMap,checkthezone::ZoneLocationsIDsVectorMap,
    checkthezone::ZoneLocationsIDsVectorMap,checkthezone::ZoneLocationsIDsVectorMap
};

QMap <QString,QVector<QStringList>> checkthezone::Zones_IDXZYvectorMap;






///////////////////////////////////////

QMap<QString,QMap<QString,QVector<QString>>> checkthezone::ZoneForOBJvectorsmaps;

QVector<QMap<QString,QMap<QString,QVector<QString>>>> checkthezone::WorldsVectorOFObjectZonesMaps =
{
    checkthezone::ZoneForOBJvectorsmaps,checkthezone::ZoneForOBJvectorsmaps,checkthezone::ZoneForOBJvectorsmaps,
    checkthezone::ZoneForOBJvectorsmaps,checkthezone::ZoneForOBJvectorsmaps,checkthezone::ZoneForOBJvectorsmaps
};

QMap <QString,int> checkthezone::IDandZoneNumber;

Convert::Convert(QObject *parent) : QObject(parent)
{

}

QString Convert::Bytes4to5(QString fourBytes)
{
    QString Byte5Total = "";

    QString FourByte = fourBytes.toUpper();
    QString Byte_1st = FourByte.mid(0,2);
    QString Byte_2nd = FourByte.mid(2,2);
    QString Byte_3rd = FourByte.mid(4,2);
    QString Byte_4th = FourByte.mid(6,2);
    QString Reverse = Byte_4th + Byte_3rd + Byte_2nd + Byte_1st;

    int FourByte1 = Reverse.toInt(nullptr,16);

    if(FourByte1 == 0)
    {
        int Byte1st1 = Byte_1st.toInt(nullptr,16);
        int xorValue = Byte1st1 ^ 0xFF;
        QString Byte1st2 = QString("%1").arg(xorValue,2,16,QLatin1Char('0'));

        int Byte2nd1 = Byte_2nd.toInt(nullptr,16);
        xorValue = Byte2nd1 ^ 0xFF;
        QString Byte2nd2 = QString("%1").arg(xorValue,2,16,QLatin1Char('0'));

        int Byte3rd1 = Byte_3rd.toInt(nullptr,16);
        xorValue = Byte3rd1 ^ 0xFF;
        QString Byte3rd2 = QString("%1").arg(xorValue,2,16,QLatin1Char('0'));

        int Byte4th1 = Byte_4th.toInt(nullptr,16);
        xorValue = Byte4th1 ^ 0xFF;
        QString Byte4th2 = QString("%1").arg(xorValue,2,16,QLatin1Char('0'));

        Reverse = Byte4th2 + Byte3rd2 + Byte2nd2 + Byte1st2;

        FourByte1 = Reverse.toInt(nullptr,16);

        Plus1 = "Plus1";
    }

    int Byte5th = 0;
    int Byte4th = 0;
    int Byte3rd = 0;
    int Byte2nd = 0;
    int Byte1st = 0;
    int Total = 0;
    int Difference = 0;

    Byte5th = FourByte1 / 134217728;

    if(Byte5th > 0)
    {
        Total = Byte5th * 134217728;
        Difference = FourByte1 - Total;
        FourByte1 = Difference;

        QString Byte5th1 = QString("%1").arg(Byte5th,2,16,QLatin1Char('0'));
        Byte5Total.prepend(Byte5th1);
    }

    Byte4th = FourByte1 / 1048576;

    if(Byte4th > 0)
    {
        Total = Byte4th * 1048576;
        Difference = FourByte1 - Total;
        FourByte1 = Difference;

        if(Byte4th < 128 && Byte5th > 0)
        {
            Byte4th = Byte4th + 128;
            QString Byte4th1 = QString("%1").arg(Byte4th,2,16,QLatin1Char('0'));
            Byte5Total.prepend(Byte4th1);
        }
        else
        {
            QString Byte4th1 = QString("%1").arg(Byte4th,2,16,QLatin1Char('0'));
            Byte5Total.prepend(Byte4th1);
        }
    }

    if(Byte4th == 0 && Byte5th > 0)
    {
        Byte4th = 128;
        QString Byte4th1 = QString("%1").arg(Byte4th,2,16,QLatin1Char('0'));
        Byte5Total.prepend(Byte4th1);
    }

    Byte3rd = FourByte1 / 8192;

    if(Byte3rd > 0)
    {
        Total = Byte3rd * 8192;
        Difference = FourByte1 - Total;
        FourByte1 = Difference;

        if(Byte3rd < 128 && Byte4th > 0)
        {
            Byte3rd = Byte3rd + 128;
            QString Byte3rd1 = QString("%1").arg(Byte3rd,2,16,QLatin1Char('0'));
            Byte5Total.prepend(Byte3rd1);
        }
        else
        {
            QString Byte3rd1 = QString("%1").arg(Byte3rd,2,16,QLatin1Char('0'));
            Byte5Total.prepend(Byte3rd1);
        }
    }

    if(Byte3rd == 0 && Byte4th > 0)
    {
        Byte3rd = 128;
        QString Byte3rd1 = QString("%1").arg(Byte3rd,2,16,QLatin1Char('0'));
        Byte5Total.prepend(Byte3rd1);
    }

    Byte2nd = FourByte1 / 64;

    if(Byte2nd > 0)
    {
        Total = Byte2nd * 64;
        Difference = FourByte1 - Total;
        FourByte1 = Difference;

        if(Byte2nd < 128 && Byte3rd > 0)
        {
            Byte2nd = Byte2nd + 128;
            QString Byte2nd1 = QString("%1").arg(Byte2nd,2,16,QLatin1Char('0'));
            Byte5Total.prepend(Byte2nd1);
        }
        else
        {
            QString Byte2nd1 = QString("%1").arg(Byte2nd,2,16,QLatin1Char('0'));
            Byte5Total.prepend(Byte2nd1);
        }
    }

    if(Byte2nd == 0 && Byte3rd > 0)
    {
        Byte2nd = 128;
        QString Byte2nd1 = QString("%1").arg(Byte2nd,2,16,QLatin1Char('0'));
        Byte5Total.prepend(Byte2nd1);
    }

    Byte1st = FourByte1 * 2;

    if(Byte1st > 0)
    {
        if(Byte1st < 128 && Byte2nd > 0)
        {
            if(Plus1 == "Plus1")
            {
                Byte1st = Byte1st + 129;
            }
            else
            {
                Byte1st = Byte1st + 128;
            }
            QString Byte1st1 = QString("%1").arg(Byte1st,2,16,QLatin1Char('0'));
            Byte5Total.prepend(Byte1st1);
        }
        else
        {
            QString Byte1st1 = QString("%1").arg(Byte1st,2,16,QLatin1Char('0'));
            Byte5Total.prepend(Byte1st1);
        }
    }

    if(Byte1st == 0 && Byte2nd > 0)
    {

        if(Plus1 == "Plus1")
        {
            Byte1st = 129;
        }
        else
        {
            Byte1st = 128;
        }
        QString Byte1st1 = QString("%1").arg(Byte1st,2,16,QLatin1Char('0'));
        Byte5Total.prepend(Byte1st1);
    }

    Plus1 = "";

    return Byte5Total.toUpper();
};


QString Convert::ConvertTo(QString Dec_value)
{

    QByteArray XOR_Reverse = Dec_value.toUtf8().toUpper();
    QString XOR_Reverse1 = XOR_Reverse.mid(0,2);
    QString XOR_Reverse2 = XOR_Reverse.mid(2,2);
    QString XOR_Reverse3 = XOR_Reverse.mid(4,2);
    QString XOR_Reverse4 = XOR_Reverse.mid(6,2);



    QString XOR_Reverse111 = QString("%1").arg(XOR_Reverse1,2,QLatin1Char('0'));
    QString XOR_Reverse222 = QString("%1").arg(XOR_Reverse2,2,QLatin1Char('0'));
    QString XOR_Reverse333 = QString("%1").arg(XOR_Reverse3,2,QLatin1Char('0'));
    QString XOR_Reverse444 = QString("%1").arg(XOR_Reverse4,2,QLatin1Char('0'));

    QString fourBytes =
            XOR_Reverse444.toUpper() +
            XOR_Reverse333.toUpper() +
            XOR_Reverse222.toUpper() +
            XOR_Reverse111.toUpper()
            ;

    QByteArray hexnum4 = Dec_value.toUtf8().toUpper();

    long tes565 = hexnum4.toLong(nullptr,10);

    QString test454 = QString("%1").arg(tes565,8,16,QLatin1Char('0')).toUpper();

    fourBytes = test454.toUpper();



    QString Bytes_append;
    QString Bytes_append2;


    ////qDebug() << "fourBytes" << fourBytes;

    QByteArray test1 = fourBytes.toUtf8().toUpper();
    QByteArray firstchunk = test1.mid(6,2); //get last digit
    long firstlong = firstchunk.toInt(nullptr,16);
    long firstres = (firstlong * 2) + 0x80;
    //QByteArray step6 = step5.number(((thirdnumber4 * 2) - 1) + 0x80 ,16);

    manipulate1 = firstres;
    while(manipulate1 >= 0x100)
    {
        manipulate1 = manipulate1 - 0x80;
    }






    Bytes_append = QString("%1").arg(manipulate1,2,16,QLatin1Char('0')).toUpper();

    Bytes_append2.append(Bytes_append);

    ////qDebug() << "manipulate1" << Bytes_append;

    long secondlong = test1.toLong(nullptr,16); //get full number
    qlonglong secondres = secondlong / 0x40; //divide full number by 0x40
    QByteArray seconddiv = seconddiv.number(secondres,16); //first full division result
    ////qDebug() << "seconddiv" << seconddiv;

    QByteArray secondcut = seconddiv.mid(seconddiv.size() - 2,2); //get last 2 digits
    ////qDebug() << "secondcut" << secondcut;


    long secondtry = secondcut.toLong(nullptr,16);
    long secondres2 = secondtry + 0x80;

    manipulate2 = secondres2;
    while(manipulate2 >= 0x100)
    {
        manipulate2 = manipulate2 - 0x80;
    }

    Bytes_append = QString("%1").arg(manipulate2,2,16,QLatin1Char('0')).toUpper();
    //manipulate2 = 0;

    Bytes_append2.append(Bytes_append);

    ////qDebug() << "manipulate2" << Bytes_append;

    qlonglong thirdres = secondres / 0x80;
    QByteArray thirddiv = thirddiv.number(thirdres,16);
    ////qDebug() << "thirddiv" << thirddiv;

    QByteArray thirdcut = thirddiv.mid(thirddiv.size() - 2,2); //get last 2 digits
    long thirdlong = thirdcut.toLong(nullptr,16); //change to long
    long thirdlong2 = thirdlong + 0x80;

    manipulate3 = thirdlong2;
    while(manipulate3 >= 0x100)
    {
        manipulate3 = manipulate3 - 0x80;
    }


    Bytes_append = QString("%1").arg(manipulate3,2,16,QLatin1Char('0')).toUpper();
    //manipulate3 = 0;
    Bytes_append2.append(Bytes_append);

    ////qDebug() << "manipulate3" << Bytes_append;

    qlonglong fourthres = thirdres / 0x80; //get fourth
    QByteArray fourthdiv = fourthdiv.number(fourthres,16);
    ////qDebug() << "fourthdiv" << fourthdiv;

    QByteArray fourthcut = fourthdiv.mid(fourthdiv.size() - 2,2);
    long fourthlong = fourthcut.toLong(nullptr,16);
    long fourthlong2 = fourthlong + 0x80;

    manipulate4 = fourthlong2;
    while(manipulate4 >= 0x100)
    {
        manipulate4 = manipulate4 - 0x80;
    }

    Bytes_append = QString("%1").arg(manipulate4,2,16,QLatin1Char('0')).toUpper();
    //manipulate4 = 0;

    Bytes_append2.append(Bytes_append);

    ////qDebug() << "manipulate4" << Bytes_append;

    qlonglong fifthres = fourthres / 0x80;

    manipulate5 = fifthres; //waa waa im losing precision on a least signifigant bit :p
    while(manipulate5 >= 0x100)
    {
        manipulate5 = manipulate5 - 0x80;
    }

    Bytes_append = QString("%1").arg(manipulate5,2,16,QLatin1Char('0')).toUpper();  //make sure its two digits, doesnt seem to overflow
    //manipulate5 = 0;
    Bytes_append2.append(Bytes_append);



    ////qDebug() << "manipulate5" << Bytes_append;

    QString test2 = Bytes_append2.toUpper();
    for (int x = 0; x < test2.size(); x++)
    {
        theresult.append(test2.mid(x,2));
        x += 2;
    }

//        //qDebug() << "manipulate5" << Bytes_append;
//        //qDebug() << "test2.size()" << test2.size();




    QByteArray Split = Bytes_append2.toUtf8().toUpper();

    ////qDebug() << "Split" << Split;

    QString Final_out;

    QByteArray Byte_1st = Split.mid(0,2);
    QByteArray Byte_2nd = Split.mid(2,2);
    QByteArray Byte_3rd = Split.mid(4,2);
    QByteArray Byte_4th = Split.mid(6,2);
    QByteArray Byte_5th = Split.mid(8,2);

    int Byte_1st2 = Byte_1st.toInt(nullptr,16);
    int Byte_2nd2 = Byte_2nd.toInt(nullptr,16);
    int Byte_3rd2 = Byte_3rd.toInt(nullptr,16);
    int Byte_4th2 = Byte_4th.toInt(nullptr,16);
    int Byte_5th2 = Byte_5th.toInt(nullptr,16);

    if(Byte_5th2 == 0x00 && Byte_4th2 == 0x80 && Byte_3rd2 == 0x80 && Byte_2nd2 == 0x80)
    {
        int manipulate1 = Byte_1st2;

        if(manipulate1 >= 0x80)
        {
            manipulate1 = manipulate1 - 0x80;
        }


        QString Byte_1st3 = QString("%1").arg(manipulate1,2,16,QLatin1Char('0')).toUpper();

        Final_out = Byte_1st3;
    }

    if(Byte_5th2 == 0x00 && Byte_4th2 == 0x80 && Byte_3rd2 == 0x80 && Byte_2nd2 > 0x80)
    {
        int manipulate1 = Byte_1st2;
        int manipulate2 = Byte_2nd2 - 0x80;

        QString Byte_1st3 = QString("%1").arg(manipulate1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd3 = QString("%1").arg(manipulate2,2,16,QLatin1Char('0')).toUpper();

       Final_out = Byte_1st3 + Byte_2nd3;
    }

    if(Byte_5th2 == 0x00 && Byte_4th2 == 0x80 && Byte_3rd2 > 0x80)
    {
        int manipulate1 = Byte_1st2;
        int manipulate2 = Byte_2nd2;
        int manipulate3 = Byte_3rd2 - 0x80;

        QString Byte_1st3 = QString("%1").arg(manipulate1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd3 = QString("%1").arg(manipulate2,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd3 = QString("%1").arg(manipulate3,2,16,QLatin1Char('0')).toUpper();

        Final_out = Byte_1st3 + Byte_2nd3 + Byte_3rd3;
    }

    if(Byte_5th2 == 0x00 && Byte_4th2 > 0x80)
    {
        int manipulate1 = Byte_1st2;
        int manipulate2 = Byte_2nd2;
        int manipulate3 = Byte_3rd2;
        int manipulate4 = Byte_4th2 - 0x80;

        QString Byte_1st3 = QString("%1").arg(manipulate1,2,16,QLatin1Char('0')).toUpper();
        QString Byte_2nd3 = QString("%1").arg(manipulate2,2,16,QLatin1Char('0')).toUpper();
        QString Byte_3rd3 = QString("%1").arg(manipulate3,2,16,QLatin1Char('0')).toUpper();
        QString Byte_4th3 = QString("%1").arg(manipulate4,2,16,QLatin1Char('0')).toUpper();

        Final_out = Byte_1st3 + Byte_2nd3 + Byte_3rd3 + Byte_4th3;
    }






    theresult = "";




    manipulate1 = 0;
    manipulate2 = 0;
    manipulate3 = 0;
    manipulate4 = 0;
    manipulate5 = 0;

    ////qDebug() << "Final_out = " << Final_out;
return Final_out;


};
