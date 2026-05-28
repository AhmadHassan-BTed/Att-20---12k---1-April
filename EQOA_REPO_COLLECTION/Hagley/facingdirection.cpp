#include "facingdirection.h"
#include "xorpacket.h"
#include "objectpacket.h"
#include "increment.h"
#include "packetsize.h"
#include "qmath.h"
#include <QtMath>
#include "math.h"
#include "packetparsing.h"



FacingDirection::FacingDirection(QObject *parent) : QObject(parent)
{

}

QVector<QByteArray> FacingDirection::SendToRoaming(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QByteArray masterx, QByteArray masterz, QByteArray mastery)
{
    packetparsing::packetvars IN_FacingDirection;
    IN_FacingDirection = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QVector<QByteArray> SendChannelsVector;

    if(objectpacket::Roamers_List.isEmpty() == false)
    {
        ////qDebug() << "objectpacket::Roamers_List.size()" << objectpacket::Roamers_List.size();

        QMapIterator<QString,QString> iterroamer(objectpacket::Roamers_List);
        while (iterroamer.hasNext())
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);

            iterroamer.next();
            Roamer_ID_key = iterroamer.key().toUpper();
            QString Status = iterroamer.value();

            //float Return_Distance = objectpacket::Temp_Distance_ID_Map.value(Roamer_ID_key);

            if(Status != "Combat")// && Return_Distance <= 200.0)
            {





                QMap <QString,QMap<int,QString>> TempMap;

                if(Now_Roaming == "YES")
                {
                    TempMap = Path_Maps;
                }

                if(TempMap.contains(Roamer_ID_key))
                {
                    QMap<int,QString> Path_Counter_XZY = TempMap.value(Roamer_ID_key); // path maps

                    if(Path_Counter_XZY.isEmpty() == false && Path_CounterMap.contains(Roamer_ID_key))
                    {
                        int Roaming_Counter = Path_CounterMap.value(Roamer_ID_key);

                        if(Path_Counter_XZY.contains(Roaming_Counter))
                        {
                            Path_XZY = Path_Counter_XZY.value(Roaming_Counter).toUpper();

                            QString Next_X = Path_XZY.mid(0,6);
                            QString Next_Z = Path_XZY.mid(6,6);
                            QString Next_Y = Path_XZY.mid(12,6);

                            QString AGGRO_Starting_X = Next_X.toUtf8(); // current steps ending X
                            QString AGGRO_Starting_Z = Next_Z.toUtf8(); // current steps ending Z
                            QString AGGRO_Starting_Y = Next_Y.toUtf8(); // current steps ending Y

                            if(objectpacket::Master_Map.contains(Roamer_ID_key.toUtf8()))
                            {
                                MOB_Master_c9 = objectpacket::Master_Map.value(Roamer_ID_key.toUtf8());
                            }

                            MOB_Master_c9 = MOB_Master_c9.toUpper();

                            QString MOB_X_val = MOB_Master_c9.mid(12,6);//      Last X value
                            QString MOB_Z_val = MOB_Master_c9.mid(18,6);//      Last Z value
                            QString MOB_Y_val = MOB_Master_c9.mid(24,6);//      Last Y value
                            QString MOB_face_val = MOB_Master_c9.mid(30,2);//   Last Facing Value
                            QString MOB_Animation = MOB_Master_c9.mid(116,2);// Last Animation value

                            QString MOB_Counter_X = MOB_X_val;
                            QString MOB_Counter_Z = MOB_Z_val;
                            QString MOB_Counter_Y = MOB_Y_val;
                            QString MOB_Counter_Facing = MOB_face_val;
                            QString MOB_Animation_Old = MOB_Animation.toUtf8();

                            FacingDirection New_Facing_Home; //get class
                            QString MOB_Facing_Home = New_Facing_Home.Facing_Home(ClientID,ClientsIP,ClientsPort,AGGRO_Starting_X,AGGRO_Starting_Z,AGGRO_Starting_Y,
                                                                                  MOB_Counter_X,MOB_Counter_Z,MOB_Counter_Y); //send xyz to FacingDirection
                            MOB_Movement_Rate = "Return Home";

                            int MOB_Facing_Home1 = MOB_Facing_Home.toInt(nullptr,16);// New facing..

                            Mob_ALL_Values2 = Mob_ALL_Values;//-------------------------------------201 00's
                            Xor_Mob_movement2 = Xor_Mob_movement;


                            //slowly increment facing first..


                            // if its already = it will skip the facing part and go to the movement part
                            if(MOB_Counter_Facing.toUpper() != MOB_Facing_Home.toUpper())
                            {
                                int MOB_Counter_Facing1 = MOB_Counter_Facing.toInt(nullptr,16);// Old Facing

                                int Difference = 0;
                                int Difference3 = 0;
                                int Difference4 = 0;
                                int New_Difference_int = 0;
                                int New_Difference4_int = 0;
                                int Difference_Best = 0;
                                int MOB_Counter_Facing2 = 0;
                                QString New_Difference = "" ;
                                QString New_Difference4 = "" ;
                                QByteArray Facing_Lock2 = "00";


                                //find the fifference each time it increments

                                if(MOB_Facing_Home1 > MOB_Counter_Facing1)
                                {
                                    Difference3 = MOB_Facing_Home1 - MOB_Counter_Facing1;
                                }

                                if(MOB_Counter_Facing1 > MOB_Facing_Home1)
                                {
                                    Difference3 = MOB_Counter_Facing1 - MOB_Facing_Home1;
                                }

                                ////qDebug() << "Difference3" << Difference3;

                                // if its 8 or less it will skip this part and xor to the ending facing..
                                if(Difference3 > 8)
                                {
                                    Difference = MOB_Facing_Home1 - MOB_Counter_Facing1;
                                    New_Difference = QString("%1").arg(Difference,2,16,QLatin1Char('0'));


                                    //can probably fix this with the -255 or +255 but when i tried it
                                    // it messed up the facing..
                                    if(New_Difference.size() > 2)//if size is >= 3
                                    {
                                        QString New_Difference2 = New_Difference.mid(14,2);
                                        New_Difference_int = New_Difference2.toInt(nullptr,16);
                                    }

                                    if(New_Difference.size() == 2)//if size is 3. and if size is >3
                                    {
                                        New_Difference_int = New_Difference.toInt(nullptr,16);
                                    }

                                    ////qDebug() << "New_Difference_int" << New_Difference_int;

                                    Difference4 = MOB_Counter_Facing1 - MOB_Facing_Home1;
                                    New_Difference4 = QString("%1").arg(Difference4,2,16,QLatin1Char('0'));// for debug

                                    //"ffffffffffffff53"
                                    if(New_Difference4.size() > 2)
                                    {
                                        QString New_Difference4_cut = New_Difference4.mid(14,2);
                                        New_Difference4_int = New_Difference4_cut.toInt(nullptr,16);
                                    }
                                    else
                                    {
                                        New_Difference4_int = New_Difference4.toInt(nullptr,16);
                                    }

                                    ////qDebug() << "New_Difference4_int" << New_Difference4_int;


                                    // this part picks the value that is the shortest way to turn
                                    if(New_Difference_int < New_Difference4_int)
                                    {
                                        Difference_Best = New_Difference_int;// reg value is closer
                                    }
                                    if(New_Difference4_int < New_Difference_int)
                                    {
                                        Difference_Best = New_Difference4_int;// neg value is closer
                                    }
                                    if(New_Difference_int == New_Difference4_int)
                                    {
                                        Difference_Best = New_Difference_int;
                                    }

                                    ////qDebug() << "Difference_Best" << Difference_Best;

                                    QString Difference_Best2 = QString("%1").arg(Difference_Best,2,16,QLatin1Char('0'));// for debugging



                                    ////qDebug() << "Difference_Best2" << Difference_Best2;

                                    // without these locks the npc would spin past 00 then instead of adding it would start subtracting
                                    // which causes them to turn back and fourth..

                                    if(Facing_Lock_Map.contains(Roamer_ID_key))
                                    {
                                        Facing_Lock2 = Facing_Lock_Map.value(Roamer_ID_key);
                                    }


                                    ////qDebug() << "Facing_Lock2" << Facing_Lock2;

                                    int inc_by = 4; // turning speed


                                    if(Facing_Lock2 == "00" || Facing_Lock2 == "01")
                                    {
                                        if(Facing_Lock2 == "01")
                                        {
                                            MOB_Counter_Facing2 = MOB_Counter_Facing1 + inc_by;
                                        }

                                        if(Facing_Lock2 == "00")
                                        {
                                            //                      Not neg
                                            if((Difference_Best == New_Difference_int) && (MOB_Facing_Home1 > MOB_Counter_Facing1) && New_Difference.size() == 2)
                                            {
                                                Facing_Lock2 = "01";

                                                MOB_Counter_Facing2 = MOB_Counter_Facing1 + inc_by;
                                                Animation_New = "04";//turning left
                                            }
                                        }
                                    }

                                    if(Facing_Lock2 == "00" || Facing_Lock2 == "02")
                                    {

                                        if(Facing_Lock2 == "02")
                                        {
                                            MOB_Counter_Facing2 = MOB_Counter_Facing1 - inc_by;
                                        }

                                        if(Facing_Lock2 == "00")
                                        {
                                            // is neg
                                            if((Difference_Best == New_Difference_int) && (MOB_Facing_Home1 > MOB_Counter_Facing1) && New_Difference.size() > 2)
                                            {
                                                Facing_Lock2 = "02";

                                                MOB_Counter_Facing2 = MOB_Counter_Facing1 - inc_by;
                                                Animation_New = "05";//turning right
                                            }
                                        }
                                    }

                                    if(Facing_Lock2 == "00" || Facing_Lock2 == "03")
                                    {


                                        if(Facing_Lock2 == "03")
                                        {
                                            MOB_Counter_Facing2 = MOB_Counter_Facing1 - inc_by;
                                        }

                                        if(Facing_Lock2 == "00")
                                        {
                                            //                      Not neg
                                            if((Difference_Best == New_Difference_int) && (MOB_Facing_Home1 < MOB_Counter_Facing1) && New_Difference.size() == 2)
                                            {
                                                Facing_Lock2 = "03";

                                                MOB_Counter_Facing2 = MOB_Counter_Facing1 - inc_by;
                                                Animation_New = "04";//turning left
                                            }
                                        }
                                    }

                                    if(Facing_Lock2 == "00" || Facing_Lock2 == "04")
                                    {


                                        if(Facing_Lock2 == "04")
                                        {
                                            MOB_Counter_Facing2 = MOB_Counter_Facing1 + inc_by;
                                        }

                                        if(Facing_Lock2 == "00")
                                        {
                                            // is neg
                                            if((Difference_Best == New_Difference_int) && (MOB_Facing_Home1 < MOB_Counter_Facing1) && New_Difference.size() > 2)
                                            {
                                                Facing_Lock2 = "04";

                                                MOB_Counter_Facing2 = MOB_Counter_Facing1 + inc_by;
                                                Animation_New = "05";//turning right
                                            }
                                        }
                                    }




                                    if(Facing_Lock2 == "00" || Facing_Lock2 == "05")
                                    {


                                        if(Facing_Lock2 == "05")
                                        {
                                            MOB_Counter_Facing2 = MOB_Counter_Facing1 + inc_by;
                                        }

                                        if(Facing_Lock2 == "00")
                                        {
                                            //                      not neg
                                            if(Difference_Best == New_Difference4_int && (MOB_Facing_Home1 > MOB_Counter_Facing1) && New_Difference4.size() == 2)
                                            {
                                                Facing_Lock2 = "05";

                                                MOB_Counter_Facing2 = MOB_Counter_Facing1 + inc_by;
                                                Animation_New = "04";//turning left
                                            }
                                        }
                                    }

                                    if(Facing_Lock2 == "00" || Facing_Lock2 == "06")
                                    {


                                        if(Facing_Lock2 == "06")
                                        {
                                            MOB_Counter_Facing2 = MOB_Counter_Facing1 - inc_by;
                                        }

                                        if(Facing_Lock2 == "00")
                                        {
                                            //                      is neg
                                            if(Difference_Best == New_Difference4_int && (MOB_Facing_Home1 > MOB_Counter_Facing1) && New_Difference4.size() > 2)
                                            {
                                                Facing_Lock2 = "06";

                                                MOB_Counter_Facing2 = MOB_Counter_Facing1 - inc_by;
                                                Animation_New = "05";//turning right
                                            }
                                        }
                                    }

                                    if(Facing_Lock2 == "00" || Facing_Lock2 == "07")
                                    {


                                        if(Facing_Lock2 == "07")
                                        {
                                            MOB_Counter_Facing2 = MOB_Counter_Facing1 - inc_by;
                                        }

                                        if(Facing_Lock2 == "00")
                                        {
                                            //                      not neg
                                            if(Difference_Best == New_Difference4_int && (MOB_Facing_Home1 < MOB_Counter_Facing1) && New_Difference4.size() == 2)
                                            {
                                                Facing_Lock2 = "07";

                                                MOB_Counter_Facing2 = MOB_Counter_Facing1 - inc_by;
                                                Animation_New = "04";//turning left
                                            }
                                        }
                                    }

                                    if(Facing_Lock2 == "00" || Facing_Lock2 == "08")
                                    {


                                        if(Facing_Lock2 == "08")
                                        {
                                            MOB_Counter_Facing2 = MOB_Counter_Facing1 + inc_by;
                                        }

                                        if(Facing_Lock2 == "00")
                                        {
                                            //                      Is neg
                                            if(Difference_Best == New_Difference4_int && (MOB_Facing_Home1 < MOB_Counter_Facing1) && New_Difference4.size() > 2)
                                            {
                                                Facing_Lock2 = "08";

                                                MOB_Counter_Facing2 = MOB_Counter_Facing1 + inc_by;
                                                Animation_New = "05";//turning right
                                            }
                                        }
                                    }

                                    ////qDebug() << "Facing_Lock2" << Facing_Lock2;


                                    Facing_Lock2_Map.insert(Roamer_ID_key,Facing_Lock2);// locks the value that it falls under untill the marker they are on is over


                                    int MOB_Animation_Old1 = MOB_Animation_Old.toInt(nullptr,16);
                                    int Animation_New1 = Animation_New.toInt(nullptr,16);
                                    int Animation_New2 = MOB_Animation_Old1 ^ Animation_New1;
                                    Output_New_Aimation = QString("%1").arg(Animation_New2,2,16,QLatin1Char('0'));
                                    MOB_Animation_Old = Animation_New.toUpper();//--------Animation


                                    if(MOB_Counter_Facing2 > 255)
                                    {
                                        int MOB_Counter_Facing3 = MOB_Counter_Facing2 - 255;
                                        MOB_Counter_Facing2 = MOB_Counter_Facing3;
                                    }

                                    if(MOB_Counter_Facing2 < 0)
                                    {
                                        int MOB_Counter_Facing3 = MOB_Counter_Facing2 + 255;
                                        MOB_Counter_Facing2 = MOB_Counter_Facing3;
                                    }

                                    QString New_facing_out = QString("%1").arg(MOB_Counter_Facing2,2,16,QLatin1Char('0'));
                                    MOB_Counter_Facing = New_facing_out;


                                    int Output_Xor_F = MOB_Counter_Facing1 ^ MOB_Counter_Facing2;
                                    QString Output_Xor_F_21 = QString("%1").arg(Output_Xor_F,2,16,QLatin1Char('0'));

                                    //could probably remove these 3 if statements now that we have the -255 and +255
                                    // and just put..
                                    //Mob_ALL_Values2.replace(18,2,Output_Xor_F_21.toUtf8());//----------------facing

                                    if(Output_Xor_F_21.size() > 3)//if size is >3 neg
                                    {
                                        QString New_out_Xor = Output_Xor_F_21.mid(14,2);
                                        Mob_ALL_Values2.replace(18,2,New_out_Xor.toUtf8());//----------------facing

                                        Output_Xor_F_21 = New_out_Xor;
                                    }

                                    if(Output_Xor_F_21.size() == 3)//if size is 3 neg
                                    {
                                        QString New_out_Xor = Output_Xor_F_21.mid(1,2);
                                        Mob_ALL_Values2.replace(18,2,New_out_Xor.toUtf8());//----------------facing

                                        Output_Xor_F_21 = New_out_Xor;
                                    }

                                    if(Output_Xor_F_21.size() == 2)
                                    {
                                        Mob_ALL_Values2.replace(18,2,Output_Xor_F_21.toUtf8());//----------------facing
                                    }
                                }





                                if((Difference3 >= 0 && Difference3 <= 8))
                                {
                                    Animation_New = "01";//walk

                                    int MOB_Animation_Old1 = MOB_Animation_Old.toInt(nullptr,16);
                                    int Animation_New1 = Animation_New.toInt(nullptr,16);
                                    int Animation_New2 = MOB_Animation_Old1 ^ Animation_New1;
                                    Output_New_Aimation = QString("%1").arg(Animation_New2,2,16,QLatin1Char('0'));
                                    MOB_Animation_Old = Animation_New.toUpper();//--------Animation


                                    int Output_Xor_F = MOB_Counter_Facing1 ^ MOB_Facing_Home1;
                                    QString Output_Xor_F_21 = QString("%1").arg(Output_Xor_F,2,16,QLatin1Char('0'));
                                    MOB_Counter_Facing = MOB_Facing_Home;

                                    Mob_ALL_Values2.replace(18,2,Output_Xor_F_21.toUtf8());//----------------facing
                                }



                                ////qDebug() << "MOB_Ending_Facing" << MOB_Counter_Facing.toUpper();

                                MOB_Master_c9.replace(30,2,MOB_Counter_Facing);
                                MOB_Master_c9.replace(116,2,MOB_Animation_Old);

                                objectpacket::Master_Map.insert(Roamer_ID_key.toUtf8(),MOB_Master_c9.toUtf8());



                                float Return_Distance = IN_FacingDirection.Temp_Distance_ID_Map.value(Roamer_ID_key);

                                if(Return_Distance <= IN_FacingDirection.Farthest_distance) // if the roamers are close enough to have a channel then send c9
                                {

                                    Xor_Mob_movement2.replace(12,20,Mob_ALL_Values2.toUpper());
                                    Xor_Mob_movement2.replace(116,2,Output_New_Aimation.toUtf8());



                                    xorpacket packet2;
                                    QByteArray Recompressed2 = packet2.packetencrypt(Xor_Mob_movement2);

                                    // Recompress the c9 ///////// //



                                    QMapIterator<QString,QString> iter1 (IN_FacingDirection.Map_Channel_ID); // get current roamers channel
                                    while (iter1.hasNext())
                                    {

                                        iter1.next();
                                        QString Current_Chan = iter1.key();
                                        QString Current_ID = iter1.value();


                                        if(Current_ID == Roamer_ID_key)
                                        {

                                            QString New_Chan = IN_FacingDirection.Map_Channel_ID.key(Roamer_ID_key); // set roamers channel

                                            if(IN_FacingDirection.incrementmap.contains(New_Chan))
                                            {
                                                if(New_Chan.isEmpty() == false)
                                                {
                                                    if(IN_FacingDirection.channelmap.key("free") == nullptr && FacingDirection::Now_Roaming == "YES")
                                                    {
                                                        QByteArray Header_Channel = "xxc9xxxx01";
                                                        Header_Channel.replace(0,2,New_Chan.toUtf8().toUpper());

                                                        Header_Channel.append(Recompressed2);

                                                        SendChannelsVector.append(Header_Channel);
                                                    }

                                                    if(FacingDirection::Now_Roaming == "NO" && SendChannelsVector.isEmpty() == false)
                                                    {
                                                        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_FacingDirection);
                                                        return SendChannelsVector;

                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {

                                Mob_ALL_Values2 = Mob_ALL_Values;//-------------------------------------201 00's
                                Xor_Mob_movement2 = Xor_Mob_movement;

                                // //////////// Send to facing to increment xzy and get radius //////////////

                                FacingDirection New_Movement1; //get class

                                MOB_X_MOB_Z_MOB_Y_MOB_Radius = New_Movement1.MOB_Radius_Movement
                                        (ClientID,ClientsIP,ClientsPort,AGGRO_Starting_X, AGGRO_Starting_Z, AGGRO_Starting_Y,
                                         MOB_Counter_X, MOB_Counter_Z, MOB_Counter_Y); //send xyz to FacingDirection

                                QString MOB_Counter_X3 = MOB_X_MOB_Z_MOB_Y_MOB_Radius.mid(0,6);
                                QString MOB_Counter_Z3 = MOB_X_MOB_Z_MOB_Y_MOB_Radius.mid(6,6);
                                QString MOB_Counter_Y3 = MOB_X_MOB_Z_MOB_Y_MOB_Radius.mid(12,6);


                                MOB_X = MOB_Counter_X.toInt(nullptr,16);
                                MOB_Z = MOB_Counter_Z.toInt(nullptr,16);
                                MOB_Y = MOB_Counter_Y.toInt(nullptr,16);



                                QByteArray MOB_Counter_X2 = MOB_Counter_X3.toUtf8();
                                QByteArray MOB_Counter_Z2 = MOB_Counter_Z3.toUtf8();
                                QByteArray MOB_Counter_Y2 = MOB_Counter_Y3.toUtf8();

                                MOB_X_MOB_Z_MOB_Y_MOB_Radius.remove(0,18);

                                QString Roamers_Radius1 = MOB_X_MOB_Z_MOB_Y_MOB_Radius;


                                objectpacket::Roamers_Radius = Roamers_Radius1.toFloat();

                                ////qDebug() << "objectpacket::Roamers_Radius" << objectpacket::Roamers_Radius;

                                // //////////// Send to facing to increment xzy and get radius //////////////



                                if(objectpacket::Roamers_Radius >= 0.1)//--------------------------------------------------------XOR
                                {

                                    if(Roamer_ID_key == "9B1D0500")
                                    {
                                        ////qDebug() << "Still Going" << Roamer_ID_key;
                                    }

                                    QByteArray X_Xor = MOB_Counter_X3.toUtf8();//--------------------------------X
                                    qlonglong X_Xor_2 = X_Xor.toLongLong(nullptr,16);
                                    qlonglong Output_Xor = MOB_X ^ X_Xor_2;
                                    QString Output_Xor_2 = QString("%1").arg(Output_Xor,6,16,QLatin1Char('0'));
                                    MOB_Counter_X = MOB_Counter_X3;
                                    Mob_ALL_Values2.replace(0,6,Output_Xor_2.toUtf8());//-----------------------X

                                    QByteArray Mobz_Xor_z = MOB_Counter_Z3.toUtf8();//---------------------------Z
                                    qlonglong Z_Xor_2 = Mobz_Xor_z.toLongLong(nullptr,16);
                                    qlonglong Output_Xor_Z_1 = MOB_Z ^ Z_Xor_2;
                                    QString Output_Xor_Z = QString("%1").arg(Output_Xor_Z_1,6,16,QLatin1Char('0'));
                                    MOB_Counter_Z = MOB_Counter_Z3;
                                    Mob_ALL_Values2.replace(6,12,Output_Xor_Z.toUtf8());//----------------------Z

                                    QByteArray Y_Xor = MOB_Counter_Y3.toUtf8();//--------------------------------Y
                                    qlonglong Y_Xor_2 = Y_Xor.toLongLong(nullptr,16);
                                    qlonglong Output_X_Y = MOB_Y ^ Y_Xor_2;
                                    QString Output_Xor_Y_2 = QString("%1").arg(Output_X_Y,6,16,QLatin1Char('0'));
                                    MOB_Counter_Y = MOB_Counter_Y3;
                                    Mob_ALL_Values2.replace(12,6,Output_Xor_Y_2.toUtf8());//--------------------Y



                                }//--------------------------------------------------------------------------XOR





                                if(objectpacket::Roamers_Radius < 0.5)// next marker location reached
                                {

                                    QByteArray X_Xor = AGGRO_Starting_X.toUtf8();
                                    qlonglong X_Xor_2 = X_Xor.toLongLong(nullptr,16);
                                    qlonglong Output_Xor = MOB_X ^ X_Xor_2;
                                    QString Output_Xor_2 = QString("%1").arg(Output_Xor,6,16,QLatin1Char('0'));
                                    MOB_Counter_X = AGGRO_Starting_X;
                                    Mob_ALL_Values2.replace(0,6,Output_Xor_2.toUtf8());//-----------------------X

                                    QByteArray Mobz_Xor_z = AGGRO_Starting_Z.toUtf8();//------------------------------------Z
                                    qlonglong Z_Xor_2 = Mobz_Xor_z.toLongLong(nullptr,16);
                                    qlonglong Output_Xor_Z_1 = MOB_Z ^ Z_Xor_2;
                                    QString Output_Xor_Z = QString("%1").arg(Output_Xor_Z_1,6,16,QLatin1Char('0'));
                                    MOB_Counter_Z = AGGRO_Starting_Z;
                                    Mob_ALL_Values2.replace(6,12,Output_Xor_Z.toUtf8());//----------------------Z



                                    QByteArray Y_Xor = AGGRO_Starting_Y.toUtf8();//-----------------------------------------Y
                                    qlonglong Y_Xor_2 = Y_Xor.toLongLong(nullptr,16);
                                    qlonglong Output_X_Y = MOB_Y ^ Y_Xor_2;
                                    QString Output_Xor_Y_2 = QString("%1").arg(Output_X_Y,6,16,QLatin1Char('0'));
                                    MOB_Counter_Y = AGGRO_Starting_Y;
                                    Mob_ALL_Values2.replace(12,6,Output_Xor_Y_2.toUtf8());//--------------------Y




                                    if(Roaming_Counter == Path_Counter_XZY.count())// when they have reched the last marker
                                    {
                                        Roaming_Counter = 1;
                                    }
                                    else
                                    {
                                        Roaming_Counter = Roaming_Counter + 1;
                                    }

                                    QByteArray Facing_Lock2 = "00";

                                    //                                    if(!SnowMan_c9_DM.contains(Roamer_ID_key) && !SnowMan_c9_HP.contains(Roamer_ID_key) && !SnowMan_c9_FP.contains(Roamer_ID_key) && !RandomIDsMAP.contains(Roamer_ID_key))
                                    //                                    {
                                    //                                        Path_CounterMap.insert(Roamer_ID_key,Roaming_Counter);
                                    //                                        Facing_Lock2_Map.insert(Roamer_ID_key,Facing_Lock2);
                                    //                                    }

                                    if(RandomIDsMAP.contains(Roamer_ID_key))
                                    {
                                        int Max = RandomIDsMAP.value(Roamer_ID_key);

                                        std::uniform_int_distribution <int> distribution2(1, Max);
                                        int Roaming_Counter = distribution2(*QRandomGenerator::global());

                                        Path_CounterMap.insert(Roamer_ID_key,Roaming_Counter);
                                        Facing_Lock2_Map.insert(Roamer_ID_key,Facing_Lock2);
                                    }

                                    //                                    if(SnowMan_c9_DM.contains(Roamer_ID_key))
                                    //                                    {
                                    //                                        std::uniform_int_distribution <int> distribution2(1, 40);
                                    //                                        Roaming_Counter2 = distribution2(*QRandomGenerator::global());

                                    //                                        Path_CounterMap.insert(Roamer_ID_key,Roaming_Counter2);
                                    //                                        Facing_Lock2_Map.insert(Roamer_ID_key,Facing_Lock2);
                                    //                                    }

                                    //                                    if(SnowMan_c9_HP.contains(Roamer_ID_key))
                                    //                                    {
                                    //                                        std::uniform_int_distribution <int> distribution3(1, 60);// choose id at random
                                    //                                        Roaming_Counter3 = distribution3(*QRandomGenerator::global());

                                    //                                        Path_CounterMap.insert(Roamer_ID_key,Roaming_Counter3);
                                    //                                        Facing_Lock2_Map.insert(Roamer_ID_key,Facing_Lock2);
                                    //                                    }

                                    //                                    if(SnowMan_c9_FP.contains(Roamer_ID_key))
                                    //                                    {
                                    //                                        std::uniform_int_distribution <int> distribution4(1, 24);// choose id at random
                                    //                                        Roaming_Counter4 = distribution4(*QRandomGenerator::global());

                                    //                                        Path_CounterMap.insert(Roamer_ID_key,Roaming_Counter4);
                                    //                                        Facing_Lock2_Map.insert(Roamer_ID_key,Facing_Lock2);
                                    //                                    }
                                }





                                // insert all the values back into the master map //
                                MOB_Master_c9.replace(12,6,MOB_Counter_X);
                                MOB_Master_c9.replace(18,6,MOB_Counter_Z);
                                MOB_Master_c9.replace(24,6,MOB_Counter_Y);
                                MOB_Master_c9.replace(116,2,MOB_Animation_Old);

                                objectpacket::Master_Map.insert(Roamer_ID_key.toUtf8(),MOB_Master_c9.toUtf8());


                                int index = IN_FacingDirection.holdid1.indexOf(Roamer_ID_key);

                                if(index != -1)
                                {
                                    IN_FacingDirection.holdx.replace(index,MOB_Master_c9.mid(12,6));
                                    IN_FacingDirection.holdz.replace(index,MOB_Master_c9.mid(18,6));
                                    IN_FacingDirection.holdy.replace(index,MOB_Master_c9.mid(24,6));
                                }

                                index = IN_FacingDirection.holdid2.indexOf(Roamer_ID_key);

                                if(index != -1)
                                {
                                    IN_FacingDirection.holdx2.replace(index,MOB_Master_c9.mid(12,6));
                                    IN_FacingDirection.holdz2.replace(index,MOB_Master_c9.mid(18,6));
                                    IN_FacingDirection.holdy2.replace(index,MOB_Master_c9.mid(24,6));
                                }


                                // insert all the values back into the master map //




                                float Return_Distance = IN_FacingDirection.Temp_Distance_ID_Map.value(Roamer_ID_key);

                                if(Return_Distance <= IN_FacingDirection.Farthest_distance) // if the roamers are close enough to have a channel then send c9
                                {


                                    // Recompress the c9 ///////// //

                                    Xor_Mob_movement2.replace(12,20,Mob_ALL_Values2.toUpper());

                                    xorpacket packet2;
                                    QByteArray Recompressed2 = packet2.packetencrypt(Xor_Mob_movement2);

                                    // Recompress the c9 ///////// //

                                    QMapIterator<QString,QString> iter1 (IN_FacingDirection.Map_Channel_ID); // get current roamers channel
                                    while (iter1.hasNext())
                                    {

                                        iter1.next();
                                        QString Current_Chan = iter1.key();
                                        QString Current_ID = iter1.value();


                                        if(Current_ID == Roamer_ID_key)
                                        {

                                            QString New_Chan = IN_FacingDirection.Map_Channel_ID.key(Roamer_ID_key); // set roamers channel

                                            if(IN_FacingDirection.incrementmap.contains(New_Chan))
                                            {
                                                if(New_Chan.isEmpty() == false)
                                                {
                                                    if(IN_FacingDirection.channelmap.key("free") == nullptr && FacingDirection::Now_Roaming == "YES")
                                                    {
                                                        QByteArray Header_Channel = "xxc9xxxx01";
                                                        Header_Channel.replace(0,2,New_Chan.toUtf8().toUpper());

                                                        Header_Channel.append(Recompressed2);

                                                        SendChannelsVector.append(Header_Channel);
                                                    }

                                                    if(Now_Roaming == "NO" && SendChannelsVector.isEmpty() == false)
                                                    {
                                                        packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_FacingDirection);
                                                        return SendChannelsVector;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_FacingDirection);

    return SendChannelsVector;

};

QByteArray FacingDirection::SendToFacing(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QByteArray masterx, QByteArray masterz, QByteArray mastery)
{
    packetparsing::packetvars IN_FacingDirection;
    IN_FacingDirection = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    QString AGGROkey = "";
    QString HolderSendBack = "";

    QMapIterator<QString,QString> iterroamer(objectpacket::AGGRO_List);
    while (iterroamer.hasNext())
    {

        iterroamer.next();

        AGGROkey = iterroamer.key();




        FacingDirection New_Facing_Player; //get class
        AGGRO_Facing_Player =
                New_Facing_Player.Facing_Player(ClientID,ClientsIP,ClientsPort,
                                                NPC_Counter_X,NPC_Counter_Y); //send xyz to FacingDirection

        AGGRO_Movement_Rate = "AGGRO Follow";




        // //////////// Send to facing to increment xzy and get radius //////////////

        FacingDirection New_Movement1; //get class

        HolderSendBack = New_Movement1.AGGRO_Radius_Movement
                (ClientID,ClientsIP,ClientsPort,masterx,masterz,mastery,AGGROkey,
                 NPC_Counter_X, NPC_Counter_Z, NPC_Counter_Y); //send xyz to FacingDirection
    }

    QByteArray PacketOut = HolderSendBack.toUtf8();

    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_FacingDirection);

    return PacketOut;
};


QString FacingDirection::Facing_Player(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,
                                       QString NPC_Counter_X, QString NPC_Counter_Y)
{
    packetparsing::packetvars IN_FacingDirection;
    IN_FacingDirection = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    New_NPC_Facing.clear();

    QByteArray hex1_1 = IN_FacingDirection.masterx;
    int hex2_1 = hex1_1.toInt(nullptr,16);
    float numtofloat_1 = hex2_1 / 128.0;
    Targets_X = Targets_X.number(numtofloat_1,'g',9);



    QByteArray hex1 = IN_FacingDirection.mastery;
    int hex2 = hex1.toInt(nullptr,16);
    float numtofloat = hex2 / 128.0;
    Targets_Y = Targets_Y.number(numtofloat,'g',9);



    float Targets_X_3 = Targets_X.toFloat();
    Targets_X_2 = Targets_X_3;

    float Targets_Y_3 = Targets_Y.toFloat();
    Targets_Y_2 = Targets_Y_3;


    int hex3 = NPC_Counter_X.toInt(nullptr,16);
    float numtofloat2 = hex3 / 128.0;
    QString hextofloat_X2 = hextofloat_X2.number(numtofloat2,'g',9);


    int hex4 = NPC_Counter_Y.toInt(nullptr,16);
    float numtofloat3 = hex4 / 128.0;
    QString hextofloat_Y2 = hextofloat_Y2.number(numtofloat3,'g',9);

    float extra1 = Targets_X_2 - hextofloat_X2.toFloat();
    float extra2 = Targets_Y_2 - hextofloat_Y2.toFloat();

    float result2 = qAtan2(extra1,extra2);

    float degrees = qRadiansToDegrees(result2);

    mobfacing = degrees   + 180;


    if (mobfacing >= 270)
    {
        mobfacing = ((360 + mobfacing) /1.411764706);
        Facing_output = QString("%1").arg(mobfacing,2,16,QLatin1Char('0'));

    }

    else
    {
        mobfacing = (mobfacing / 1.411764706);
        Facing_output = QString("%1").arg(mobfacing,2,16,QLatin1Char('0'));

    }

    New_NPC_Facing = Facing_output.toUtf8();
    QString New_NPC_Facing2 = New_NPC_Facing.mid(0,3);
    QString New_NPC_Facing3 = New_NPC_Facing.mid(14,2);



    if(New_NPC_Facing2 == "FFF")
    {
        New_NPC_Facing = New_NPC_Facing3;
        //FFFFFFFFFFFFFFE6
    }


    if(New_NPC_Facing2.size() >= 3 )
    {
        QString New_NPC_Facing5 = New_NPC_Facing.mid(1,2);
        New_NPC_Facing = New_NPC_Facing5;
    }



    // above works for facing away from player


    int New_NPC_Facing1 = New_NPC_Facing.toInt(nullptr,16);

    if(New_NPC_Facing1 >= 0x00 && New_NPC_Facing1 <= 0x7F)
    {
        int New_NPC_Facing2 = New_NPC_Facing1 + 0x80;

        QString Facing_output_Plus = QString("%1").arg(New_NPC_Facing2,2,16,QLatin1Char('0'));

        New_NPC_Facing = Facing_output_Plus.toUtf8().toUpper();
    }

    if(New_NPC_Facing1 >= 0x80 && New_NPC_Facing1 <= 0xFF)
    {
        int New_NPC_Facing2 = New_NPC_Facing1 - 0x80;

        QString Facing_output_Minus = QString("%1").arg(New_NPC_Facing2,2,16,QLatin1Char('0'));

        New_NPC_Facing = Facing_output_Minus.toUtf8().toUpper();
    }



    //qDebug() << "New_NPC_Facing" << New_NPC_Facing;

    return New_NPC_Facing;

}


QString FacingDirection::Facing_Home(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString AGGRO_Starting_X, QString AGGRO_Starting_Z, QString AGGRO_Starting_Y,
                                     QString MOB_Counter_X, QString MOB_Counter_Z, QString MOB_Counter_Y)
{


    New_NPC_Facing.clear();

    QByteArray hex1_1 = AGGRO_Starting_X.toUtf8();
    int hex2_1 = hex1_1.toInt(nullptr,16);
    float numtofloat_1 = hex2_1 / 128.0;
    Targets_X = Targets_X.number(numtofloat_1,'g',9);



    QByteArray hex1 = AGGRO_Starting_Y.toUtf8();
    int hex2 = hex1.toInt(nullptr,16);
    float numtofloat = hex2 / 128.0;
    Targets_Y = Targets_Y.number(numtofloat,'g',9);



    float Targets_X_3 = Targets_X.toFloat();
    Targets_X_2 = Targets_X_3;

    float Targets_Y_3 = Targets_Y.toFloat();
    Targets_Y_2 = Targets_Y_3;


    int hex3 = MOB_Counter_X.toInt(nullptr,16);
    float numtofloat2 = hex3 / 128.0;
    QString hextofloat_X2 = hextofloat_X2.number(numtofloat2,'g',9);


    int hex4 = MOB_Counter_Y.toInt(nullptr,16);
    float numtofloat3 = hex4 / 128.0;
    QString hextofloat_Y2 = hextofloat_Y2.number(numtofloat3,'g',9);

    float extra1 = Targets_X_2 - hextofloat_X2.toFloat();
    float extra2 = Targets_Y_2 - hextofloat_Y2.toFloat();

    float result2 = qAtan2(extra1,extra2);

    float degrees = qRadiansToDegrees(result2);

    mobfacing = degrees   + 180;


    if (mobfacing >= 270)
    {
        mobfacing = ((360 + mobfacing) /1.411764706);
        Facing_output = QString("%1").arg(mobfacing,2,16,QLatin1Char('0'));

    }

    else
    {
        mobfacing = (mobfacing / 1.411764706);
        Facing_output = QString("%1").arg(mobfacing,2,16,QLatin1Char('0'));

    }

    New_NPC_Facing = Facing_output.toUtf8();
    QString New_NPC_Facing2 = New_NPC_Facing.mid(0,3);
    QString New_NPC_Facing3 = New_NPC_Facing.mid(14,2);



    if(New_NPC_Facing2 == "FFF")
    {
        New_NPC_Facing = New_NPC_Facing3;
        //FFFFFFFFFFFFFFE6
    }


    if(New_NPC_Facing2.size() >= 3 )
    {
        QString New_NPC_Facing5 = New_NPC_Facing.mid(1,2);
        New_NPC_Facing = New_NPC_Facing5;
    }



    // above works for facing away from player


    int New_NPC_Facing1 = New_NPC_Facing.toInt(nullptr,16);

    if(New_NPC_Facing1 >= 0x00 && New_NPC_Facing1 <= 0x7F)
    {
        int New_NPC_Facing2 = New_NPC_Facing1 + 0x80;

        QString Facing_output_Plus = QString("%1").arg(New_NPC_Facing2,2,16,QLatin1Char('0'));

        New_NPC_Facing = Facing_output_Plus.toUtf8().toUpper();
    }

    if(New_NPC_Facing1 >= 0x80 && New_NPC_Facing1 <= 0xFF)
    {
        int New_NPC_Facing2 = New_NPC_Facing1 - 0x80;

        QString Facing_output_Minus = QString("%1").arg(New_NPC_Facing2,2,16,QLatin1Char('0'));

        New_NPC_Facing = Facing_output_Minus.toUtf8().toUpper();
    }



    return New_NPC_Facing;


}

QString FacingDirection::AGGRO_Radius_Movement(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QByteArray EndX, QByteArray EndZ, QByteArray EndY, QString NPCID,
                                               QString NPC_Counter_X, QString NPC_Counter_Z, QString NPC_Counter_Y)
{

    //qDebug() << "AGGRO_Radius_Movement.NPCID" << NPCID;

    QByteArray C_ID_IP_PORT = ClientID + ClientsIP + ClientsPort;

    //qDebug() << "AGGRO_Radius_Movement.C_ID_IP_PORT" << C_ID_IP_PORT;

    packetparsing::packetvars IN_FacingDirection;
    IN_FacingDirection = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    if(!IN_FacingDirection.FollowMap.contains(NPCID))
    {
        IN_FacingDirection.FollowMap.insert(NPCID,true);
    }

    QString NPC_Counter_X1 = "";
    QString NPC_Counter_Z1 = "";
    QString NPC_Counter_Y1 = "";
    QByteArray Animation_New = "";
    QByteArray AGGRO_Animation_Old = "";

    Main_X = EndX.toInt(nullptr,16);
    qlonglong NPC_Counter_X_EAST = 0x000000;
    qlonglong NPC_Counter_X_WEST = 0x000000;
    NPC_X = NPC_Counter_X.toInt(nullptr,16);

    Main_Z = EndZ.toInt(nullptr,16);
    qlonglong NPC_Counter_Z_Ground_Z = 0x000000;
    NPC_Z = NPC_Counter_Z.toInt(nullptr,16);


    Main_Y = EndY.toInt(nullptr,16);
    qlonglong NPC_Counter_Y_South = 0x000000;
    qlonglong NPC_Counter_Y_North = 0x000000;
    NPC_Y = NPC_Counter_Y.toInt(nullptr,16);

    // ///// AGGRO Radius //////

    QByteArray  myx1 = EndX; //send as 3 byte hex
    int xfromhex1 = myx1.toInt(nullptr,16);
    float maintoonx1 = xfromhex1 / 128.0;

    QByteArray  myz1 = EndZ;
    int zfromhex1 = myz1.toInt(nullptr,16);
    float maintoonz1 = zfromhex1 / 128.0;

    if(maintoonz1 > 130000)
    {
        maintoonz1 = maintoonz1 - 131072.00195193;
    }

    QByteArray  myy1 = EndY;
    int yfromhex1 = myy1.toInt(nullptr,16);
    float maintoony1 = yfromhex1 / 128.0;



    //AGGRO xzy


    QString tempstrx = NPC_Counter_X;
    int tempint1 = tempstrx.toInt(nullptr,16);
    float xinterlude = tempint1 / 128.0;

    QString tempstrz = NPC_Counter_Z;
    int tempint3 = tempstrz.toInt(nullptr,16);
    float  zinterlude = tempint3 / 128.0;

    if(zinterlude > 130000)
    {
        zinterlude = zinterlude - 131072.00195193;
    }

    QString tempstry = NPC_Counter_Y;
    int tempint2 = tempstry.toInt(nullptr,16);
    float yinterlude = tempint2 / 128.0;



    float sub1 = maintoonx1 - xinterlude;
    float sub2 = maintoonz1 - zinterlude;
    float sub3 = maintoony1 - yinterlude;

    float power1 = qPow(sub1,2);
    float power2 = qPow(sub2,2);
    float power3 = qPow(sub3,2);

    float firstresult = power1 + power2 + power3;

    float result1 = qSqrt(firstresult);

    AggroDistance = QString::number(result1);



    float AggroRunDistance1 = AggroRunDistance;

    bool Follow = true;

    Follow = IN_FacingDirection.FollowMap.value(NPCID);

    if(result1 >= AggroRunDistance1 && Follow == false)
    {
        Follow = true;
    }



    if(result1 >= 3.5 )//&& Follow == true)
    {
        Divide_By =  AggroSpeed1;


        //EAST
        if(NPC_X > Main_X && NPC_Y == Main_Y) // East only
        {

            float east = NPC_X - Main_X;
            tx1 = east;
            float dist = qSqrt(tx1*tx1);
            int speed = Divide_By;
            float velX = (tx1/dist)*speed;
            NPC_Counter_X_EAST = NPC_X - velX;
            NPC_Counter_X1 = QString("%1").arg(NPC_Counter_X_EAST,6,16,QLatin1Char('0')).toUpper();
        }

        if(NPC_X > Main_X && NPC_Y > Main_Y) // East south
        {
            float east = NPC_X - Main_X;
            float south = NPC_Y - Main_Y;
            tx1 = east;
            ty1 = south;
            float dist = qSqrt(tx1 * tx1 + ty1 * ty1);
            int speed = Divide_By;
            float velx = (tx1/dist)*speed;
            float vely = (ty1/dist)*speed;
            NPC_Counter_X_EAST = NPC_X - velx;
            NPC_Counter_Y_South = NPC_Y - vely;
            NPC_Counter_X1 = QString("%1").arg(NPC_Counter_X_EAST,6,16,QLatin1Char('0')).toUpper();
            NPC_Counter_Y1 = QString("%1").arg(NPC_Counter_Y_South,6,16,QLatin1Char('0')).toUpper();
        }

        if(NPC_X > Main_X && NPC_Y < Main_Y) // East North
        {
            float east = NPC_X - Main_X;
            float north = Main_Y - NPC_Y;
            tx1 = east;
            ty1 = north;
            float dist = qSqrt(tx1 * tx1 + ty1 * ty1);
            int speed = Divide_By;
            float velx = (tx1/dist)*speed;
            float vely = (ty1/dist)*speed;
            NPC_Counter_X_EAST = NPC_X - velx;
            NPC_Counter_Y_North = NPC_Y + vely;
            NPC_Counter_X1 = QString("%1").arg(NPC_Counter_X_EAST,6,16,QLatin1Char('0')).toUpper();
            NPC_Counter_Y1 = QString("%1").arg(NPC_Counter_Y_North,6,16,QLatin1Char('0')).toUpper();
        }



        //west
        if(NPC_X < Main_X && NPC_Y == Main_Y) // west only
        {
            float west = Main_X - NPC_X;
            tx1 = west;
            float dist = qSqrt(tx1*tx1);
            int speed = Divide_By;
            float velX = (tx1/dist)*speed;
            NPC_Counter_X_WEST = NPC_X + velX;
            NPC_Counter_X1 = QString("%1").arg(NPC_Counter_X_WEST,6,16,QLatin1Char('0')).toUpper();
        }


        if(NPC_X < Main_X && NPC_Y > Main_Y) // west south
        {
            float west = Main_X - NPC_X;//---------West
            float south = NPC_Y - Main_Y;
            tx1 = west;
            ty1 = south;
            float dist = qSqrt(tx1 * tx1 + ty1 * ty1);
            int speed = Divide_By;
            float velx = (tx1/dist)*speed;
            float vely = (ty1/dist)*speed;
            NPC_Counter_X_WEST = NPC_X + velx;
            NPC_Counter_Y_South = NPC_Y - vely;
            NPC_Counter_X1 = QString("%1").arg(NPC_Counter_X_WEST,6,16,QLatin1Char('0')).toUpper();
            NPC_Counter_Y1 = QString("%1").arg(NPC_Counter_Y_South,6,16,QLatin1Char('0')).toUpper();
        }

        if(NPC_X < Main_X && NPC_Y < Main_Y) // west north
        {
            float west = Main_X - NPC_X;
            float north = Main_Y - NPC_Y;
            tx1 = west;
            ty1 = north;
            float dist = qSqrt(tx1 * tx1 + ty1 * ty1);
            int speed = Divide_By;
            float velx = (tx1/dist)*speed;
            float vely = (ty1/dist)*speed;
            NPC_Counter_X_WEST = NPC_X + velx;
            NPC_Counter_Y_North = NPC_Y + vely;
            NPC_Counter_X1 = QString("%1").arg(NPC_Counter_X_WEST,6,16,QLatin1Char('0')).toUpper();
            NPC_Counter_Y1 = QString("%1").arg(NPC_Counter_Y_North,6,16,QLatin1Char('0')).toUpper();
        }


        //south
        if(NPC_Y > Main_Y && NPC_X == Main_X)
        {
            float south = NPC_Y - Main_Y;
            ty1 = south;
            float dist = qSqrt(ty1 * ty1);
            int speed = Divide_By;
            float vely = (ty1/dist)*speed;
            NPC_Counter_Y_South = NPC_Y - vely;
            NPC_Counter_X1 = QString("%1").arg(NPC_Counter_Y_South,6,16,QLatin1Char('0')).toUpper();
        }

        //north
        if(NPC_Y < Main_Y && NPC_X == Main_X)
        {
            float north = Main_Y - NPC_Y;
            ty1 = north;
            float dist = qSqrt(ty1 * ty1);
            int speed = Divide_By;
            float vely = (ty1/dist)*speed;
            NPC_Counter_Y_North = NPC_Y + vely;
            NPC_Counter_Y1 = QString("%1").arg(NPC_Counter_Y_North,6,16,QLatin1Char('0')).toUpper();
        }

        if(NPC_Z < Main_Z)
        {
            float Z_Difference2 = Main_Z - NPC_Z;

            tz1 = Z_Difference2;
            float dist = qSqrt(tx1 * tx1 + ty1 * ty1 + tz1 * tz1);
            float speed = Divide_By;
            float velz = (tz1/dist)*speed;
            NPC_Counter_Z_Ground_Z = NPC_Z + velz;

            NPC_Counter_Z1 = QString("%1").arg(NPC_Counter_Z_Ground_Z,6,16,QLatin1Char('0')).toUpper();
        }

        if(NPC_Z > Main_Z)
        {
            float Z_Difference2 = NPC_Z - Main_Z;

            tz1 = Z_Difference2;
            float dist = qSqrt(tx1 * tx1 + ty1 * ty1 + tz1 * tz1);
            float speed = Divide_By;
            float velz = (tz1/dist)*speed;
            NPC_Counter_Z_Ground_Z = NPC_Z - velz;

            NPC_Counter_Z1 = QString("%1").arg(NPC_Counter_Z_Ground_Z,6,16,QLatin1Char('0')).toUpper();
        }
    }


    if(NPC_X == Main_X || NPC_Counter_X1.isEmpty() == true)
    {

        NPC_Counter_X1 = NPC_Counter_X;
    }

    if(NPC_Z == Main_Z || NPC_Counter_Z1.isEmpty() == true)
    {

        NPC_Counter_Z1 = NPC_Counter_Z;
    }

    if(NPC_Y == Main_Y || NPC_Counter_Y1.isEmpty() == true)
    {

        NPC_Counter_Y1 = NPC_Counter_Y;
    }

    if(Follow == true)
    {
        QString imagex = NPC_Counter_X1;
        int imagex3 = imagex.toInt(nullptr,16);
        float  X = imagex3 / 128.0;

        QString imagey = NPC_Counter_Y1;
        int imagey3 = imagey.toInt(nullptr,16);
        float  Y = imagey3 / 128.0;

        int West = IN_FacingDirection.MyZoneCentralX - 1000;
        int East = IN_FacingDirection.MyZoneCentralX + 1000;
        int North = IN_FacingDirection.MyZoneCentralY - 1000;
        int South = IN_FacingDirection.MyZoneCentralY + 1000;

        if (X >= West && X <= East)
        {
            X = X - West;
        }


        if (Y >= North && Y <= South)
        {
            Y = Y - North;
        }

        QByteArray ID_IP_PORT = IN_FacingDirection.ClientsID + IN_FacingDirection.ClientsIP + IN_FacingDirection.ClientsPort;

        //qDebug() << "IN_FacingDirection.ID_IP_PORT" << ID_IP_PORT;


        currentImage = FacingDirection::CurrentImageMap.value(ID_IP_PORT);

        //qDebug() << "IN_CheckForLoS.FacingDirection::CurrentImageMap" << FacingDirection::CurrentImageMap;

        QImage image2 = currentImage.toImage();

        QColor pixel = image2.pixelColor(X,Y); //ask qcolor what the triplet is


        int pixr = pixel.red(); //get red
        int pixg = pixel.green(); //get green
        int pixb = pixel.blue(); //get blue

        pixr >>= 3; //convert to 0-65535
        pixg >>=2;
        pixb >>=3;

        int fullint1 = (pixr << 11) | (pixg << 5) | pixb;

        fullint1 = fullint1 - 32767;

        int helper = fullint1 * 128.0; //change to float 128.0 for full float

        NPC_Counter_Z1 = QString("%1").arg(helper,6,16,QLatin1Char('0')).toUpper();

        //qDebug() << "NPC_Counter_Z1" << NPC_Counter_Z1;


        // //////////// Send to facing to increment xzy and get radius //////////////
    }

    Dial_ALL_Values2 = Dial_ALL_Values;//-------------------------------------201 00's

    QByteArray AGGRO_Master_c91 = objectpacket::Master_Map.value(NPCID.toUtf8());

    QString MobX = AGGRO_Master_c91.mid(12,6);
    QString MobZ = AGGRO_Master_c91.mid(18,6);
    QString MobY = AGGRO_Master_c91.mid(24,6);
    QString MobFacing = AGGRO_Master_c91.mid(30,2);
    AGGRO_Animation_Old = AGGRO_Master_c91.mid(116,2);
    Animation_New = AGGRO_Master_c91.mid(116,2);

    if(result1 >= 3.5 && Follow == true)//--------------------------------------------------------XOR
    {
        QByteArray X_Xor = NPC_Counter_X1.toUtf8();//--------------------------------X
        qlonglong X_Xor_2 = X_Xor.toLongLong(nullptr,16);
        qlonglong Output_Xor = NPC_X ^ X_Xor_2;
        QString Output_Xor_2 = QString("%1").arg(Output_Xor,6,16,QLatin1Char('0'));
        Dial_ALL_Values2.replace(0,6,Output_Xor_2.toUtf8());//-----------------------X

        AGGRO_Master_c91.replace(12,6,NPC_Counter_X1.toUtf8());

        QByteArray Mobz_Xor_z = NPC_Counter_Z1.toUtf8();//---------------------------Z
        qlonglong Z_Xor_2 = Mobz_Xor_z.toLongLong(nullptr,16);
        qlonglong Output_Xor_Z_1 = NPC_Z ^ Z_Xor_2;
        QString Output_Xor_Z = QString("%1").arg(Output_Xor_Z_1,6,16,QLatin1Char('0'));
        Dial_ALL_Values2.replace(6,12,Output_Xor_Z.toUtf8());//----------------------Z

        AGGRO_Master_c91.replace(18,6,NPC_Counter_Z1.toUtf8());

        QByteArray Y_Xor = NPC_Counter_Y1.toUtf8();//--------------------------------Y
        qlonglong Y_Xor_2 = Y_Xor.toLongLong(nullptr,16);
        qlonglong Output_X_Y = NPC_Y ^ Y_Xor_2;
        QString Output_Xor_Y_2 = QString("%1").arg(Output_X_Y,6,16,QLatin1Char('0'));
        Dial_ALL_Values2.replace(12,6,Output_Xor_Y_2.toUtf8());

        AGGRO_Master_c91.replace(24,6,NPC_Counter_Y1.toUtf8());
    }//--------------------------------------------------------------------------XOR


    // /////// Decompressed /////////

    Xor_Object_ID2 = Xor_Object_ID;

    if(result1 >= 3.5 && Follow == true && Animation_New == "00")
    {
        Animation_New = "03";//---------------------------------Animation
        int AGGRO_Animation_Old1 = AGGRO_Animation_Old.toInt(nullptr,16);
        int Animation_New1 = Animation_New.toInt(nullptr,16);
        int Animation_New2 = AGGRO_Animation_Old1 ^ Animation_New1;
        QString Output_New = QString("%1").arg(Animation_New2,2,16,QLatin1Char('0'));
        Xor_Object_ID2.replace(116,2,Output_New.toUpper().toUtf8());

        AGGRO_Master_c91.replace(116,2,Animation_New.toUpper());
    }

    if(result1 < 3.5 && Follow == true && Animation_New == "03")
    {
        Follow = false;

        Animation_New = "00";//---------------------------------Animation
        int AGGRO_Animation_Old1 = AGGRO_Animation_Old.toInt(nullptr,16);
        int Animation_New1 = Animation_New.toInt(nullptr,16);
        int Animation_New2 = AGGRO_Animation_Old1 ^ Animation_New1;
        QString Output_New = QString("%1").arg(Animation_New2,2,16,QLatin1Char('0'));
        Xor_Object_ID2.replace(116,2,Output_New.toUpper().toUtf8());

        AGGRO_Master_c91.replace(116,2,Animation_New.toUpper());
    }

    IN_FacingDirection.FollowMap.insert(NPCID,Follow);


    QString NewFacing = Facing_Player(IN_FacingDirection.ClientsID,IN_FacingDirection.ClientsIP,IN_FacingDirection.ClientsPort,
                                      MobX, MobY);

    //qDebug() << "OldMobFacing" << MobFacing;

    //qDebug() << "NewMobFacing" << NewFacing;

    if(NewFacing.toUpper() != MobFacing.toUpper())
    {
        int OldFacing = MobFacing.toInt(nullptr,16);
        int NewFacing1 = NewFacing.toInt(nullptr,16);
        int Output_Xor1 = OldFacing ^ NewFacing1;
        QString Output_Xor2 = QString("%1").arg(Output_Xor1,2,16,QLatin1Char('0'));
        Dial_ALL_Values2.replace(18,2,Output_Xor2.toUtf8().toUpper());//----------------facing

        AGGRO_Master_c91.replace(30,2,NewFacing.toUtf8().toUpper());
    }

    objectpacket::Master_Map.insert(NPCID.toUtf8(),AGGRO_Master_c91);

    if(IN_FacingDirection.holdid2.contains(NPCID) && Follow == true)
    {
        int index = IN_FacingDirection.holdid2.indexOf(NPCID);

        IN_FacingDirection.holdx2.replace(index,NPC_Counter_X1);
        IN_FacingDirection.holdz2.replace(index,NPC_Counter_Z1);
        IN_FacingDirection.holdy2.replace(index,NPC_Counter_Y1);
    }

    QByteArray HolderSendBack = "";

    if(IN_FacingDirection.HPxorMap.contains(NPCID.toUtf8()))
    {
        QByteArray NewHPxor = IN_FacingDirection.HPxorMap.value(NPCID.toUtf8()).toUpper();
        IN_FacingDirection.HPxorMap.remove(NPCID.toUtf8());
        Xor_Object_ID2.replace(54,2,NewHPxor);
    }



    Xor_Object_ID2.replace(12,20,Dial_ALL_Values2.toUpper());

    if(Xor_Object_ID2 != Xor_Object_ID)
    {

        //qDebug() << "Xor_Object_ID2" << Xor_Object_ID2;

        xorpacket packet2;
        QByteArray Recompressed2 = packet2.packetencrypt(Xor_Object_ID2);

        //qDebug() << "Recompressed2" << Recompressed2;

        QByteArray Combined1 = Header_AGGRO + Recompressed2;

        //qDebug() << "Header_AGGRO" << Header_AGGRO;

        QString New_Chan = "";

        //qDebug() << "AGGRO_Radius_Movement.NPCID2 =" << NPCID;

        New_Chan = IN_FacingDirection.Map_Channel_ID.key(NPCID);

        //qDebug() << "IN_FacingDirection.Map_Channel_ID" << IN_FacingDirection.Map_Channel_ID;

        //qDebug() << "New_Chan" << New_Chan;

        if(IN_FacingDirection.Map_Channel_ID.contains(New_Chan))
        {
            HolderSendBack = Combined1;

            HolderSendBack.replace(0,2,New_Chan.toUtf8().toUpper());
        }
    }




    packetparsing::IPandPort_AllvariablesMap.insert(IN_FacingDirection.ClientsID + IN_FacingDirection.ClientsIP + IN_FacingDirection.ClientsPort,IN_FacingDirection);

    //qDebug() << "HolderSendBack" << HolderSendBack;

    return HolderSendBack;
}



// ///////////// AGGRO_Return ///////////////////////////



QString FacingDirection::MOB_Radius_Movement(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString AGGRO_Starting_X, QString AGGRO_Starting_Z, QString AGGRO_Starting_Y,
                                             QString MOB_Counter_X, QString MOB_Counter_Z, QString MOB_Counter_Y)
{


    packetparsing::packetvars IN_FacingDirection;
    IN_FacingDirection = packetparsing::IPandPort_AllvariablesMap.value(ClientID + ClientsIP + ClientsPort);

    MOB_Counter_Z1 = MOB_Counter_Z;


    Main_MOB_X = AGGRO_Starting_X.toInt(nullptr,16);
    qlonglong MOB_Counter_X_EAST = 0x000000;
    qlonglong MOB_Counter_X_WEST = 0x000000;
    MOB_X = MOB_Counter_X.toInt(nullptr,16);

    Main_MOB_Z = AGGRO_Starting_Z.toInt(nullptr,16);
    qlonglong MOB_Counter_Z_Ground_Z = 0x000000;
    MOB_Z = MOB_Counter_Z.toInt(nullptr,16);

    Main_MOB_Y = AGGRO_Starting_Y.toInt(nullptr,16);
    qlonglong MOB_Counter_Y_South = 0x000000;
    qlonglong MOB_Counter_Y_North = 0x000000;
    MOB_Y = MOB_Counter_Y.toInt(nullptr,16);

    QByteArray  myx1 = AGGRO_Starting_X.toUtf8(); //send as 3 byte hex
    int xfromhex1 = myx1.toInt(nullptr,16);
    float maintoonx1 = xfromhex1 / 128.0;

    QByteArray  myz1 = AGGRO_Starting_Z.toUtf8();
    int zfromhex1 = myz1.toInt(nullptr,16);
    float maintoonz1 = zfromhex1 / 128.0;

    QByteArray  myy1 = AGGRO_Starting_Y.toUtf8();
    int yfromhex1 = myy1.toInt(nullptr,16);
    float maintoony1 = yfromhex1 / 128.0;



    //AGGRO xzy


    QString tempstrx = MOB_Counter_X;
    int tempint1 = tempstrx.toInt(nullptr,16);
    float xinterlude = tempint1 / 128.0;

    QString tempstrz = MOB_Counter_Z;
    int tempint3 = tempstrz.toInt(nullptr,16);
    float  zinterlude = tempint3 / 128.0;

    QString tempstry = MOB_Counter_Y;
    int tempint2 = tempstry.toInt(nullptr,16);
    float yinterlude = tempint2 / 128.0;



    float sub1 = maintoonx1 - xinterlude;
    float sub2 = maintoonz1 - zinterlude;
    float sub3 = maintoony1 - yinterlude;

    float power1 = qPow(sub1,2);
    float power2 = qPow(sub2,2);
    float power3 = qPow(sub3,2);

    float firstresult = power1 + power2 + power3;

    float result2 = qSqrt(firstresult);


    AGGRO_out1 = QString("%1").arg(result2,0,'g',10,QLatin1Char('0')); //float result to string
    QString AGGRO_out4 = QString("%1").arg(result2,0,'g',10,QLatin1Char('0')); //float result to string



    float test = AGGRO_out1.toFloat(nullptr);
    int test2 = static_cast<int>(test); //cast to int only
    QString test3 = test3.number(test2,10);


    if(result2 >= 0.5)
    {
        MOB_Speed =  40.0;
    }

    //EAST
    if(MOB_X > Main_MOB_X && MOB_Y == Main_MOB_Y) // East only
    {

        float east = MOB_X - Main_MOB_X;
        tx = east;
        float dist = qSqrt(tx*tx);
        float speed = MOB_Speed;
        float velX = (tx/dist)*speed;
        MOB_Counter_X_EAST = MOB_X - velX;
        MOB_Counter_X1 = QString("%1").arg(MOB_Counter_X_EAST,6,16,QLatin1Char('0')).toUpper();
    }

    if(MOB_X > Main_MOB_X && MOB_Y > Main_MOB_Y) // East south
    {

        float east = MOB_X - Main_MOB_X;
        float south = MOB_Y - Main_MOB_Y;
        tx = east;
        ty = south;
        float dist = qSqrt(tx * tx + ty * ty);
        float speed = MOB_Speed;
        float velx = (tx/dist)*speed;
        float vely = (ty/dist)*speed;
        MOB_Counter_X_EAST = MOB_X - velx;
        MOB_Counter_Y_South = MOB_Y - vely;
        MOB_Counter_X1 = QString("%1").arg(MOB_Counter_X_EAST,6,16,QLatin1Char('0')).toUpper();
        MOB_Counter_Y1 = QString("%1").arg(MOB_Counter_Y_South,6,16,QLatin1Char('0')).toUpper();
    }

    if(MOB_X > Main_MOB_X && MOB_Y < Main_MOB_Y) // East North
    {

        float east = MOB_X - Main_MOB_X;
        float north = Main_MOB_Y - MOB_Y;
        tx = east;
        ty = north;
        float dist = qSqrt(tx * tx + ty * ty);
        float speed = MOB_Speed;
        float velx = (tx/dist)*speed;
        float vely = (ty/dist)*speed;
        MOB_Counter_X_EAST = MOB_X - velx;
        MOB_Counter_Y_North = MOB_Y + vely;
        MOB_Counter_X1 = QString("%1").arg(MOB_Counter_X_EAST,6,16,QLatin1Char('0')).toUpper();
        MOB_Counter_Y1 = QString("%1").arg(MOB_Counter_Y_North,6,16,QLatin1Char('0')).toUpper();
    }



    //west
    if(MOB_X < Main_MOB_X && MOB_Y == Main_MOB_Y) // west only
    {


        float west = Main_MOB_X - MOB_X;
        tx = west;
        float dist = qSqrt(tx*tx);
        float speed = MOB_Speed;
        float velX = (tx/dist)*speed;
        MOB_Counter_X_WEST = MOB_X + velX;
        MOB_Counter_X1 = QString("%1").arg(MOB_Counter_X_WEST,6,16,QLatin1Char('0')).toUpper();
    }


    if(MOB_X < Main_MOB_X && MOB_Y > Main_MOB_Y) // west south
    {


        float west = Main_MOB_X - MOB_X;//---------West
        float south = MOB_Y - Main_MOB_Y;



        tx = west;
        ty = south;
        float dist = qSqrt(tx * tx + ty * ty);



        float speed = MOB_Speed;



        float velx = (tx/dist)*speed;
        float vely = (ty/dist)*speed;



        MOB_Counter_X_WEST = MOB_X + velx;
        MOB_Counter_Y_South = MOB_Y - vely;



        MOB_Counter_X1 = QString("%1").arg(MOB_Counter_X_WEST,6,16,QLatin1Char('0')).toUpper();
        MOB_Counter_Y1 = QString("%1").arg(MOB_Counter_Y_South,6,16,QLatin1Char('0')).toUpper();
    }


    if(MOB_X < Main_MOB_X && MOB_Y < Main_MOB_Y) // west north
    {


        float west = Main_MOB_X - MOB_X;
        float north = Main_MOB_Y - MOB_Y;
        tx = west;
        ty = north;
        float dist = qSqrt(tx * tx + ty * ty);
        float speed = MOB_Speed;
        float velx = (tx/dist)*speed;
        float vely = (ty/dist)*speed;
        MOB_Counter_X_WEST = MOB_X + velx;
        MOB_Counter_Y_North = MOB_Y + vely;
        MOB_Counter_X1 = QString("%1").arg(MOB_Counter_X_WEST,6,16,QLatin1Char('0')).toUpper();
        MOB_Counter_Y1 = QString("%1").arg(MOB_Counter_Y_North,6,16,QLatin1Char('0')).toUpper();
    }


    //south
    if(MOB_Y > Main_MOB_Y && MOB_X == Main_MOB_X)
    {

        float south = MOB_Y - Main_MOB_Y;
        ty = south;
        float dist = qSqrt(ty*ty);
        float speed = MOB_Speed;
        float vely = (ty/dist)*speed;
        MOB_Counter_Y_South = MOB_Y - vely;
        MOB_Counter_Y1 = QString("%1").arg(MOB_Counter_Y_South,6,16,QLatin1Char('0')).toUpper();
    }

    //north
    if(MOB_Y < Main_MOB_Y && MOB_X == Main_MOB_X)
    {

        float north = Main_MOB_Y - MOB_Y;
        ty = north;
        float dist = qSqrt(ty*ty);
        float speed = MOB_Speed;
        float vely = (ty/dist)*speed;
        MOB_Counter_Y_North = MOB_Y + vely;
        MOB_Counter_Y1 = QString("%1").arg(MOB_Counter_Y_North,6,16,QLatin1Char('0')).toUpper();
    }




    // ////// Z /////// //



    if(MOB_Z < Main_MOB_Z)
    {

        Z_Difference = Main_MOB_Z - MOB_Z;

        tz = Z_Difference;
        float dist = qSqrt(tx * tx + ty * ty + tz * tz);
        float speed = MOB_Speed;
        float velz = (tz/dist)*speed;
        MOB_Counter_Z_Ground_Z = MOB_Z + velz;

        MOB_Counter_Z1 = QString("%1").arg(MOB_Counter_Z_Ground_Z,6,16,QLatin1Char('0')).toUpper();
    }



    if(MOB_Z > Main_MOB_Z)
    {

        Z_Difference = MOB_Z - Main_MOB_Z;

        tz = Z_Difference;
        float dist = qSqrt(tx * tx + ty * ty + tz * tz);
        float speed = MOB_Speed;
        float velz = (tz/dist)*speed;
        MOB_Counter_Z_Ground_Z = MOB_Z - velz;

        MOB_Counter_Z1 = QString("%1").arg(MOB_Counter_Z_Ground_Z,6,16,QLatin1Char('0')).toUpper();
    }





    if(MOB_X == Main_MOB_X || MOB_Counter_X1.isEmpty() == true)
    {

        MOB_Counter_X1 = MOB_Counter_X;
    }

    if(MOB_Z == Main_MOB_Z || MOB_Counter_Z1.isEmpty() == true)
    {

        MOB_Counter_Z1 = MOB_Counter_Z;
    }

    if(MOB_Y == Main_MOB_Y || MOB_Counter_Y1.isEmpty() == true)
    {

        MOB_Counter_Y1 = MOB_Counter_Y;
    }





    Combined2 = MOB_Counter_X1 + MOB_Counter_Z1 + MOB_Counter_Y1 + AGGRO_out1;
    packetparsing::IPandPort_AllvariablesMap.insert(ClientID + ClientsIP + ClientsPort,IN_FacingDirection);

    return Combined2;
}








