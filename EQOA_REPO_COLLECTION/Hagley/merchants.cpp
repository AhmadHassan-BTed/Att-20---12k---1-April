#include "merchants.h"
#include "packetparsing.h"
#include "increment.h"

merchants::merchants(QObject *parent) : QObject(parent)
{

}

QMap <int,QByteArray> merchants::mer_packet(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QByteArray merch_id)
{
    qDebug() << "";
    qDebug() << "";
    qDebug() << "";

    packetparsing::packetvars p14;
    p14 = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    p14.merchant_vector.clear(); //clear the list

    if(Merchant_List_Map.contains(merch_id))
    {
        qDebug() << "";
        qDebug() << "merch_id" << merch_id;
        p14.merchant_vector = Merchant_List_Map.value(merch_id);
        merch_text_header = Merchant_Text_Map.value(merch_id);

        qDebug() << "p14.merchant_vector" << p14.merchant_vector;
        qDebug() << "merch_text_header" << merch_text_header;
    }
    else
    {
        QMap <int, QByteArray> nomatch = {{0,"NULL"}};
        return nomatch;
    }

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,p14);

    total_size = 0;
    working_size = 0;
    merch_carat1 = 0;
    packet_count = 0;

    QByteArray Counter = "0000";

    for(int ce = 0; ce < p14.merchant_vector.size(); ce++)
    {

        QString itemid1 = p14.merchant_vector.at(ce);

        qDebug() << "itemid1" << itemid1;

        if(Items_Map.contains(itemid1))
        {
            QString mer_result1 = Items_Map.value(itemid1);

            qDebug() << "mer_result1" << mer_result1;

            increment sendto;
            QByteArray NewCount = sendto.count(Counter);
            Counter = NewCount;

            qDebug() << "Counter" << Counter;

            int inv_pos1 = mer_result1.indexOf("XXXXXXXX");
            mer_result1.replace(inv_pos1,8,Counter + "0000");

            total_packet.append(mer_result1);
            total_size+= mer_result1.size();

            qDebug() << "total_packet1" << total_packet;
        }
    }

    if(total_size < 2312) //small headers, only 1 packet
    {
        total_packet.append(merch_text_header); //insert ending text
        total_packet.insert(0,OpcodeAndheader); //add header for size
        total_packet.replace(4,8,merch_id); //add the new id

        int num_items = Counter.mid(0,2).toInt(nullptr,16); //get number of times looped
        QString new_num_items = QString("%1").arg(num_items,2,16,QLatin1Char('0'));
        total_packet.replace(22,2,new_num_items.toUtf8()); //replace new item count

        int doubled_num_items = num_items * 0x2;
        QString new_doubled_num_items = QString("%1").arg(doubled_num_items,2,16,QLatin1Char('0'));
        total_packet.replace(20,2,new_doubled_num_items.toUtf8()); //replace doubled count

        QString currentfbsize1 = QString::number(total_packet.size() / 2,16); //divide by two
        int fb_size3 = currentfbsize1.toInt(nullptr,16); //same but int

        if(fb_size3 < 255)
        {
            total_packet.insert(0,fb_small_header); //use small header
            total_packet.replace(2,2,currentfbsize1.toUtf8());
        }
        else
        {
            total_packet.insert(0,fb_large_header); //add fb header //add setup for end large here too
            currentfbsize1.insert(0,"0"); //adding 0 to the beginning
            QString firststring2 = currentfbsize1.mid(0,2);
            QString secondstring2 = currentfbsize1.mid(2,2);   //poor mans bit shift
            QString comp_size = secondstring2 + firststring2;
            QByteArray comp_fb_size = comp_size.toUtf8();
            //     qDebug() << "fb-small-size" <<comp_fb_size;
            total_packet.replace(4,4,comp_fb_size);
        }

        qDebug() << "total_packet2" << total_packet;

        merch_output_map.insert(packet_count,total_packet); //add to map
        return merch_output_map; //return the filled map
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(total_size >= 2312) //large headers, more than one packet
    {
        total_packet.insert(0,OpcodeAndheader); //put in the starter header
        total_packet.replace(4,8,merch_id); //add the new merchant id
        int num_items2 = Counter.mid(0,2).toInt(nullptr,16); //get number of times looped
        QString new_num_items2 = QString("%1").arg(num_items2,2,16,QLatin1Char('0'));

        qDebug() << "new_num_items2" << new_num_items2;
        qDebug() << "total_packet3" << total_packet;

        total_packet.replace(22,2,new_num_items2.toUtf8());

        qDebug() << "total_packet4" << total_packet;



        int doubled_num_items1 = num_items2 * 2;
        QString new_doubled_num_items2 = QString("%1").arg(doubled_num_items1,2,16,QLatin1Char('0'));
        total_packet.replace(20,2,new_doubled_num_items2.toUtf8()); //replace doubled count

        qDebug() << "new_doubled_num_items2" << new_doubled_num_items2;
        qDebug() << "total_packet5" << total_packet;

        total_size = total_packet.size();

        qDebug() << "total_packet3" << total_packet;

        do
        {
            working_size = total_size - merch_carat1;

            if(working_size <= 0)
            {
                break;
            }

            if(working_size >=2312)
            {
                working_size = 2312;
            }

            QByteArray packet_res = total_packet.mid(merch_carat1,working_size);

            if(working_size >=2312) //is this a large packet or small?
            {
                packet_res.insert(0,fa_largeheader);
            }
            else //this is less than 1156 bytes
            {
                packet_res.append(merch_text_header); //insert ending text
                QString currentfbsize2 = QString::number(packet_res.size() / 2,16); //divide by two in hex
                int fb_size4 = currentfbsize2.toInt(nullptr,16); //same but int decimal

                if(fb_size4 < 255)
                {
                    packet_res.insert(0,fb_small_header); //use small header
                    packet_res.replace(2,2,currentfbsize2.toUtf8());
                }
                else
                {
                    packet_res.insert(0,fb_large_header); //insert large fb header
                    currentfbsize2.insert(0,"0"); //adding 0 to the beginning
                    QString firststring2 = currentfbsize2.mid(0,2);
                    QString secondstring2 = currentfbsize2.mid(2,2);   //poor mans bit shift
                    QString comp_size = secondstring2 + firststring2;
                    QByteArray comp_fb_size = comp_size.toUtf8();
                    packet_res.replace(4,4,comp_fb_size);
                }
            }

            qDebug() << "packet_res1" << packet_res;

            merch_output_map.insert(packet_count,packet_res); //write to map


            packet_count++;

            merch_carat1 += 2312;
        } while(working_size > 0);
    }

    qDebug() << "merch_output_map" << merch_output_map;
    qDebug() << "";
    qDebug() << "";
    qDebug() << "";

    return merch_output_map; //return the filled map
} //end

QString merchants::Text_to_Unicode(QString text)
{
    QByteArray temp_1 = text.toUtf8().toHex();
    QByteArray uni_1 = temp_1;

    for(int cc = 0; cc < temp_1.size() * 2; cc+=2)
    {
        uni_1.insert(cc + 2,"00");
        cc += 2;
    }

    uni_1.insert(0,"FF000000");
    int num_count = uni_1.count() / 2;

    if(num_count < 512)
    {
        int insidehexcount2 =  (uni_1.count() /2 - 4) / 2;
        uni_1.replace(0,2,QString("%1").arg(insidehexcount2,2,16,QLatin1Char('0')).toUtf8());
    }

    return uni_1;
}
