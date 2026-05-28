#include "objectpacket.h"
#include "increment.h"
#include "packetparsing.h"
#include "generator.h"
#include "opcodes.h"
#include "checkthezone.h"

objectpacket::objectpacket(QObject *parent) : QObject(parent)
{

}

QByteArray objectpacket::packetswitch(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort)
{
    packetparsing::packetvars IN_objectpacket;
    IN_objectpacket = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);



    IN_objectpacket.map1.clear();
    IN_objectpacket.filteredset1.clear();
    workpacket.clear();
    encryptpacket.clear();

    QStringList TempHoldid1;
    QStringList TempHoldx;
    QStringList TempHoldz;
    QStringList TempHoldy;


    TempHoldid1 = IN_objectpacket.holdid1;
    TempHoldx = IN_objectpacket.holdx;
    TempHoldz = IN_objectpacket.holdz;
    TempHoldy = IN_objectpacket.holdy;

    if(IN_objectpacket.holdid2.isEmpty() == false)
    {
        TempHoldid1 = IN_objectpacket.holdid2;
        TempHoldx = IN_objectpacket.holdx2;
        TempHoldz = IN_objectpacket.holdz2;
        TempHoldy = IN_objectpacket.holdy2;
    }

    QByteArray  myx1 = IN_objectpacket.masterx; //send as 3 byte hex
    int xfromhex1 = myx1.toInt(nullptr,16);
    float maintoonx1 = xfromhex1 / 128.0;

    QByteArray  myz1 = IN_objectpacket.masterz;
    int zfromhex1 = myz1.toInt(nullptr,16);
    float maintoonz1 = zfromhex1 / 128.0;

    if(maintoonz1 > 130000)
    {
        maintoonz1 = maintoonz1 - 131072.00195193;
    }

    QByteArray  myy1 = IN_objectpacket.mastery;
    int yfromhex1 = myy1.toInt(nullptr,16);
    float maintoony1 = yfromhex1 / 128.0;

    if(TempHoldid1.isEmpty() == false)
    {
        for (int q = 0; q < TempHoldid1.size(); q++)
        {
            QString tempid1 = TempHoldid1.at(q);

            if(tempid1.isEmpty() == false)
            {
                QString tempstrx = TempHoldx.at(q);
                int tempint1 = tempstrx.toInt(nullptr,16);
                float xinterlude = tempint1 / 128.0;

                QString tempstry = TempHoldy.at(q);
                int tempint2 = tempstry.toInt(nullptr,16);
                float yinterlude = tempint2 / 128.0;

                QString tempstrz = TempHoldz.at(q);
                int tempint3 = tempstrz.toInt(nullptr,16);
                float  zinterlude = tempint3 / 128.0;

                if(zinterlude > 130000)
                {
                    zinterlude = zinterlude - 131072.00195193;
                }

                float sub1 = maintoonx1 - xinterlude;
                float sub2 = maintoony1 - yinterlude;
                float sub3 = maintoonz1 - zinterlude;

                float power1 = qPow(sub1,2);
                float power2 = qPow(sub2,2);
                float power3 = qPow(sub3,2);

                float firstresult = power1 + power2 + power3;
                float result = qSqrt(firstresult);

                QString output1 = QString("%1").arg(result,0,'g',10,QLatin1Char('0')); //float result to string


                distancecheck1 = 200.0;



                if ((result <= distancecheck1 && !DespawnDelete_Map.contains(tempid1) && !DeadListVector.contains(tempid1) && IN_objectpacket.CharSelectID != tempid1 &&
                     !generator::Players_Map.contains(tempid1.toUtf8())) ||
                        (result <= distancecheck1 && !DespawnDelete_Map.contains(tempid1) && !DeadListVector.contains(tempid1) && IN_objectpacket.CharSelectID != tempid1 &&
                         Opcodes::CharID_CLientID_IP_PORT_Map.contains(tempid1) && generator::Players_Map.contains(tempid1.toUtf8()))) //radius check
                {
                    if(IN_objectpacket.CharSelectID != tempid1.toUpper())
                    {
                        IN_objectpacket.map1.insert(tempid1,result);
                        IN_objectpacket.filteredset1.append(result);
                    }
                }



                std::sort(IN_objectpacket.filteredset1.begin(),IN_objectpacket.filteredset1.end(), std::less<float>());

                if(Roamers_List.contains(tempid1))
                {
                    IN_objectpacket.Temp_Distance_ID_Map.insert(tempid1,result);
                }

                if(IN_objectpacket.Map_Channel_ID.isEmpty() == false) //need to do this so channel distance matches the time something is loaded
                {
                    QMapIterator<QString,QString> iter1 (IN_objectpacket.Map_Channel_ID);
                    while (iter1.hasNext())
                    {
                        iter1.next();
                        QString Current_Chan = iter1.key();
                        QString Current_ID = iter1.value();

                        if(Current_ID == tempid1)
                        {
                            IN_objectpacket.Map_Channel_Distance.insert(Current_Chan,result);

                        }
                    }
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////making comparelist

    if(IN_objectpacket.filteredset1.isEmpty() == false)
    {
        for (int ch=0; ch < IN_objectpacket.filteredset1.size();ch++) //walks thru all the ids in the filterlist
        {
            float Distance = IN_objectpacket.filteredset1.at(ch);
            pullidout1 = IN_objectpacket.map1.key(Distance);

            if (!IN_objectpacket.sent_ids_map.contains(pullidout1) && pullidout1.isEmpty() == false) //if this id is not in the kept list ((this makes it run thru once then not again until u move
            {
                IN_objectpacket.comparelist1.append(pullidout1); //add the ids to the comparelist to be sent
                IN_objectpacket.sent_ids_map.insert(pullidout1,"");
            }

            if (ch > 21) //starts at zero so greater than 21 breaks at 22 and will get 23 in list always
            {
                IN_objectpacket.Farthest_distance = Distance;
                break;
            }
        }
    }





    /////////////////////////////////////////////////////////////////////////////////////MAIN LOOP




    if (IN_objectpacket.comparelist1.count() != 0) //main loop if comparelist still has ids in it
    {
        if (IN_objectpacket.comparelist1.count()  >= 3) //check the size of comparelist to find how many c9s to send
        {
            supercounter = 3;
        }
        if (IN_objectpacket.comparelist1.count() == 2)
        {
            supercounter = 2;
        }
        if (IN_objectpacket.comparelist1.count() == 1)
        {
            supercounter = 1;
        }

        for (int tt = 0; tt < supercounter; tt++) //main db loop--- comparelist sends
        {
            QString chanres1 = IN_objectpacket.comparelist1.at(tt).toUpper(); //get first id


            QString Name1 = checkthezone::NPCsNames.value(chanres1.toUpper());
            QString Name2 = checkthezone::NPCsNames.value(IN_objectpacket.CharSelectID);
            qDebug() << "";
            qDebug() << "IN_objectpacket.PlayersName = " << Name2;
            qDebug() << "IN_objectpacket.Name_In_Map = " << Name1;
            qDebug() << "IN_objectpacket.ObjectsID = " << chanres1.toUpper();

            float Distance = IN_objectpacket.map1.value(chanres1.toUpper());

            qDebug() << "IN_objectpacket.Distance = " << Distance;

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// channel stuff
            ///  everytime we send something out we find a free channel, then get that channels increment number from the incrementmap
            /// and we pull it out and increment each channel seperately

            if (IN_objectpacket.channelmap.key("free") == nullptr && supercounter != 0)
            {
                QMapIterator<QString,float> iter1 (IN_objectpacket.Map_Channel_Distance);
                while (iter1.hasNext())
                {
                    iter1.next();
                    IN_objectpacket.distancelist1.append(iter1.value());
                }
                std::sort(IN_objectpacket.distancelist1.begin(),IN_objectpacket.distancelist1.end(), std::greater<float>());

                if(supercounter == 1)
                {
                    float newdist1 = IN_objectpacket.distancelist1.at(0);
                    QString newchan1 = IN_objectpacket.Map_Channel_Distance.key(newdist1);
                    IN_objectpacket.channelmap.insert(newchan1,"free");
                }

                if(supercounter == 2)
                {
                    float newdist1 = IN_objectpacket.distancelist1.at(0);
                    float newdist2 = IN_objectpacket.distancelist1.at(1);
                    QString newchan1 = IN_objectpacket.Map_Channel_Distance.key(newdist1);
                    QString newchan2 = IN_objectpacket.Map_Channel_Distance.key(newdist2);
                    IN_objectpacket.channelmap.insert(newchan1,"free");
                    IN_objectpacket.channelmap.insert(newchan2,"free");
                }


                if(supercounter == 3)
                {
                    float newdist1 =  IN_objectpacket.distancelist1.at(0);
                    float newdist2 =  IN_objectpacket.distancelist1.at(1);
                    float newdist3 =  IN_objectpacket.distancelist1.at(2);
                    QString newchan1 = IN_objectpacket.Map_Channel_Distance.key(newdist1);
                    QString newchan2 = IN_objectpacket.Map_Channel_Distance.key(newdist2);
                    QString newchan3 = IN_objectpacket.Map_Channel_Distance.key(newdist3);

                    IN_objectpacket.channelmap.insert(newchan1,"free");
                    IN_objectpacket.channelmap.insert(newchan2,"free");
                    IN_objectpacket.channelmap.insert(newchan3,"free");
                }
                IN_objectpacket.distancelist1.clear();
            }



            QString channum1 =  IN_objectpacket.channelmap.key("free"); //find first free



            //set first free as now used

            qDebug() << "IN_objectpacket.channum1" << channum1;

            if(chanres1.isEmpty() == false)
            {
                IN_objectpacket.Map_Channel_ID.insert(channum1.toUpper(),chanres1.toUpper());
                IN_objectpacket.channelmap.insert(channum1.toUpper(),"used");

                qDebug() << "IN_objectpacket.Map_Channel_ID" << IN_objectpacket.Map_Channel_ID;


                QString extrasub1 = "xxC9xxxx00";

                extrasub1.replace(0,2,channum1);


                //KEPTLIST...these are pretty much the same thing
                IN_objectpacket.keptlist1.append(chanres1.toUpper()); //add them to the kept list so i can use it to delete out of the comparelist...
                QByteArray id1 = chanres1.toUtf8().toUpper();

                qDebug() << "id1 = " << id1 << objectpacket::Master_Map.value(id1);


                workpacket.append(objectpacket::Master_Map.value(id1));


                /////////////////////////RECOMPRESSION/////////////////////////////////////


                do
                {
                    encom1 = workpacket.mid(encarat1,2); //get one byte
                    holdit = encom1.toInt(nullptr,16); //change to int

                    if (holdit != 0x00)
                    {
                        enhigh++; // increment bytes to keep counter
                        hold2 = workpacket.mid(encarat1,2); //get bytes to keep

                        encarat1 += 2;  //move carat
                        encom3.append(hold2);

                        if (encom3 == "74737271")
                        {
                            enlow = 1;
                            QByteArray hold3 = hold3.number(enhigh,16); //get # of non zero bytes
                            hold4 = hold4.number(enlow,16); //get # of zeros
                            encryptpacket.append(hold3);
                            encryptpacket.append(hold4);
                            encryptpacket.append(encom3);

                            enlow = 0;
                            zerocounter = 0;
                            encom3 = "";
                        }
                    }
                    else
                    {
                        if (enhigh > 0x7 && enlow <= 0xF) //working
                        {
                            enhigh += 0x80;

                            QByteArray padder = "0";
                            hold4.append(padder);
                            hold4.append(hold4.number(enlow,16)); //get # of zeros
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
                        encryptpacket.append(hold3);
                        encryptpacket.append(hold4);
                        encryptpacket.append(encom3);
                        enlow = 0;
                        zerocounter = 0;

                        do
                        {
                            encom2 = workpacket.mid(encarat1,2); //spin and count zeros
                            zerocounter++;
                            encarat1 += 2;
                            enlow = zerocounter - 1;

                        }while (encom2 == "00");

                        encarat1 -= 2;
                        enhigh = 0;
                        hold4 = "";
                        encom3 = "";
                    }

                } while(encarat1 < workpacket.size());

                encryptpacket.insert(0,extrasub1.toUtf8());
                encryptpacket.append("00"); //add ending zeros

                IN_objectpacket.ActivateOP.append(encryptpacket);

                qDebug() << "IN_objectpacket.ActivateOP" << IN_objectpacket.ActivateOP;

                packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_objectpacket);
                IN_objectpacket = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);


                encryptpacket = "";
                encarat1 = 0;
                workpacket = "";
                enhigh = 0;
                enlow = 0;

                //////////////////////////////RECOMPRESSION////////////////////////////

            }

        } //end main db loop


        for (int ou = 0; ou < IN_objectpacket.keptlist1.size(); ou++) //go thru the kept list of ids
        {
            QString takenid = IN_objectpacket.keptlist1.at(ou); //get ids
            int wherearewe =  IN_objectpacket.comparelist1.indexOf(takenid,0); //find the id from keptlist and get the index in comparelist
            IN_objectpacket.comparelist1.removeAt(wherearewe); //remove the sent ids
        }
        //             qDebug() <<"COMPARELIST" <<IN_objectpacket.comparelist1;
        //              qDebug() <<"KEPTLIST" <<IN_objectpacket.keptlist1;

        /////////////////////////////////////////////////////Headers/////////////////////////////////

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_objectpacket);
    }
    else // comparelist is zero so everything in it has been sent.....comparelist = filteredlist, db, sublist
    {

        IN_objectpacket.sent_ids_map.clear();
        IN_objectpacket.keptlist1.clear();
        //change this to something else
        if (IN_objectpacket.Map_Channel_ID.isEmpty() == false) //extra insurance
        {
            QMapIterator<QString,QString> iter4 (IN_objectpacket.Map_Channel_ID); //go thru your map
            while (iter4.hasNext())
            {


                iter4.next();

                QString Channel = iter4.key();
                QString currentnewvalue = iter4.value(); //get each id
                if (currentnewvalue != "00000000")
                {
                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_objectpacket);
                    IN_objectpacket = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

                    IN_objectpacket.keptlist1.append(currentnewvalue);
                    IN_objectpacket.sent_ids_map.insert(currentnewvalue,"");

                    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_objectpacket);
                    IN_objectpacket = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);
                }
            }

            if(IN_objectpacket.AllChannelsFull == false)
            {
                IN_objectpacket.AllChannelsFull = true;
            }

            IN_objectpacket.filteredset1.clear();
            workpacket.clear();

            packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_objectpacket);

         }

        qDebug() << "IN_objectpacket.CurrentQuestMenuID_18" << IN_objectpacket.CurrentQuestMenuID;

        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_objectpacket);
    }

    return "Bad";
}

