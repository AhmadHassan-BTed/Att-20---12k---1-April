#include "targeting.h"
#include "objectpacket.h"
#include "generator.h"
#include "packetparsing.h"
#include "quests.h"

QMap <int,QVector <QString>> Quests::light_blue_map =
{
    {4,{"03"}},
    {5,{"03"}},
    {6,{"03"}},
    {7,{"03"}},
    {8,{"03"}},
    {9,{"03"}},
    {10,{"03"}},
    {11,{"03"}},
    {12,{"03"}},
    {13,{"03"}},
    {14,{"03"}},
    {15,{"03"}},
    {16,{"03","04"}},
    {17,{"03","04"}},
    {18,{"03","04"}},
    {19,{"03","04"}},
    {20,{"03","04","05"}},
    {21,{"03","04","05"}},
    {22,{"03","04","05"}},
    {23,{"03","04","05"}},
    {24,{"03","04","05","06"}},
    {25,{"03","04","05","06"}},
    {26,{"03","04","05","06"}},
    {27,{"03","04","05","06"}},
    {28,{"03","04","05","06","07"}},
    {29,{"03","04","05","06","07"}},
    {30,{"03","04","05","06","07"}},
    {31,{"03","04","05","06","07"}},
    {32,{"03","04","05","06","07","08"}},
    {33,{"03","04","05","06","07","08"}},
    {34,{"03","04","05","06","07","08"}},
    {35,{"03","04","05","06","07","08"}},
    {36,{"03","04","05","06","07","08","09"}},
    {37,{"03","04","05","06","07","08","09"}},
    {38,{"03","04","05","06","07","08","09"}},
    {39,{"03","04","05","06","07","08","09"}},
    {40,{"03","04","05","06","07","08","09","10"}},
    {41,{"03","04","05","06","07","08","09","10"}},
    {42,{"03","04","05","06","07","08","09","10"}},
    {43,{"03","04","05","06","07","08","09","10"}},
    {44,{"03","04","05","06","07","08","09","10","11"}},
    {45,{"03","04","05","06","07","08","09","10","11"}},
    {46,{"03","04","05","06","07","08","09","10","11"}},
    {47,{"03","04","05","06","07","08","09","10","11"}},
    {48,{"03","04","05","06","07","08","09","10","11","12"}},
    {49,{"03","04","05","06","07","08","09","10","11","12"}},
    {50,{"03","04","05","06","07","08","09","10","11","12"}},
    {51,{"03","04","05","06","07","08","09","10","11","12"}},
    {52,{"03","04","05","06","07","08","09","10","11","12","13"}},
    {53,{"03","04","05","06","07","08","09","10","11","12","13"}},
    {54,{"03","04","05","06","07","08","09","10","11","12","13"}},
    {55,{"03","04","05","06","07","08","09","10","11","12","13"}},
    {56,{"03","04","05","06","07","08","09","10","11","12","13","14"}},
    {57,{"03","04","05","06","07","08","09","10","11","12","13","14"}},
    {58,{"03","04","05","06","07","08","09","10","11","12","13","14"}},
    {59,{"03","04","05","06","07","08","09","10","11","12","13","14"}},
    {60,{"03","04","05","06","07","08","09","10","11","12","13","14","15"}}
};


targeting::targeting(QObject *parent) : QObject(parent)
{

}


QByteArray targeting::target(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QByteArray target_id)
{
    packetparsing::packetvars IN_targeting;
    IN_targeting = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    if(objectpacket::Master_Map.contains(target_id))
    {
        QByteArray fullnpcc9  = objectpacket::Master_Map.value(target_id);
        QString npclevel1 =fullnpcc9.mid(358,2);
        mob_level = npclevel1.toInt(nullptr,16);

        QByteArray fullmainc9 = generator::Players_Map.value(IN_targeting.targetCharID.toUtf8());
        QString mainlevel1 = fullmainc9.mid(358,2);
        main_level = mainlevel1.toInt(nullptr,16);

        int diff_a = main_level - mob_level;

        if (diff_a < 0) //Mob is Greater
        {
            diff_a = mob_level - main_level;

            if (diff_a >= 3) //red
            {
                QByteArray redcolor = "00";
                return redcolor;
            }

            if (diff_a >= 1 && diff_a <=2) //yellow
            {
                QByteArray yellowcolor = "01";
                return yellowcolor;
            }
        }

        if (diff_a == 0) //white
        {
            QByteArray whitecolor = "02";
            return whitecolor;
        }


        else //Maintoon is Greater
        {
            if (diff_a >= 1 && diff_a <= 2)
            {
                QByteArray darkbluecolor = "03";
                return darkbluecolor;
            }

            QVector<QString> possible_list;

            possible_list = Quests::light_blue_map.value(main_level); //find results from level of main toon

            if (possible_list.contains(QString("%1").arg(diff_a,2,10,QLatin1Char('0'))))
            {
                QByteArray lightbluecolor = "04";
                return lightbluecolor;
            }
            else
            {
                QByteArray greencolor = "05";
                return greencolor;
            }
        }
    }

    QByteArray nomatch = "02"; //no match default return white
    return nomatch;
}
