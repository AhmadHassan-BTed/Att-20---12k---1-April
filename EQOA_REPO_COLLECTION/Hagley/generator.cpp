#include "generator.h"
#include "mainwindow.h"
#include "packetparsing.h"

bool MainWindow::SetTableSize;
QString MainWindow::Spreadsheets = "";
QVector<QString> MainWindow::UiOputVector;
QVector <QString> MainWindow::StopMovementVector;
QHostAddress MainWindow::PublicMainip;
QByteArray MainWindow::MastersClientsID = "";
QByteArray MainWindow::MastersClientsIP = "";
QByteArray MainWindow::MastersClientsPort = "";
QByteArray MainWindow::MastersClientsObjectID = "";
QMap<QString,QString> MainWindow::MobsAggroTargetMap;
QString MainWindow::News;

QVector <QTimer*> MainWindow::ModelTimersVector;
QVector <QTimer*> MainWindow::OutgoingTimersVector;
QVector <QTimer*> MainWindow::CheckForAggroMovementTimersVector;
QVector <QTimer*> MainWindow::GroupTimersVector;
QVector <QTimer*> MainWindow::TeleportTimersVector;
QVector <QTimer*> MainWindow::CoachTimersVector;
QVector <QTimer*> MainWindow::StandingStillTimersVector;
QVector <QTimer*> MainWindow::UpdateMapsTimersVector;
QVector <QTimer*> MainWindow::LoS_TimersVector;
QVector <QTimer*> MainWindow::MySwingTimersVector;
QVector <QTimer*> MainWindow::MobSwingTimersVector;
QVector <QTimer*> MainWindow::SpawningTimersVector;
QVector <QTimer*> MainWindow::reSpawningTimersVector;
QVector <QTimer*> MainWindow::RoamingTimersVector;
QVector <QTimer*> MainWindow::RegenTimersVector;
QVector <QTimer*> MainWindow::reSend42ecTimersVector;
QVector <QTimer*> MainWindow::CheckForAggroTimersVector;
QVector <QTimer*> MainWindow::reSendFBsTimersVector;
QVector <QTimer*> MainWindow::reSendChannelsTimersVector;
QVector <QTimer*> MainWindow::StopAllMovementsTimersVector;
QVector <QTimer*> MainWindow::ReviveTimersVector;
QVector <QTimer*> MainWindow::StandUpTimersVector;

generator::generator(QObject *parent) : QObject(parent)
{

}


QByteArray generator::maintoonswitch(QString mainid, QString Change1F,QString CharSelectID_Main) //no packet sent here just spits out 00c9s from the db
{
    maintoonarray.clear();
    encryptpacket2.clear(); //clear these each call

    QByteArray id1_main = mainid.toUtf8().toUpper();
    maintoonarray.append(Players_Map.value(id1_main).toUpper()); // our main toon c9

    QString CharSelectID_MainCut = CharSelectID_Main.mid(0,6);
    maintoonarray.replace(2,6,CharSelectID_MainCut.toUtf8());

    if(Change1F.isEmpty() == false)
    {
        maintoonarray.replace(384,2,Change1F.toUtf8());
    }



    /////////////////////////RECOMPRESSION/////////////////////////////////////


    do
    {
        encom4 = maintoonarray.mid(encarat3,2); //get one byte
        holdit2 = encom4.toInt(0,16); //change to int

        if(holdit2 != 0x00)
        {
            enhigh2++; // increment bytes to keep counter
            hold5 = maintoonarray.mid(encarat3,2); //get bytes to keep
            encarat3 += 2;  //move carat
            encom6.append(hold5);

            if(encom6 == "74737271")
            {
                enlow2 = 1;
                QByteArray hold3 = hold3.number(enhigh2,16); //get # of non zero bytes
                hold6 = hold6.number(enlow2,16); //get # of zeros
                encryptpacket2.append(hold3);
                encryptpacket2.append(hold6);
                encryptpacket2.append(encom6);

                enlow2 = 0;
                zerocounter2 = 0;
                encom6 = "";
            }
        }

        else
        {

            if(enhigh2 > 0x7 && enlow2 <= 0xF) //working
            {
                enhigh2 += 0x80;

                QByteArray padder = "0";
                hold6.append(padder);
                hold6.append(hold6.number(enlow2,16)); //get # of zeros


            }
            else
            {
                hold6 = hold6.number(enlow2,16); //get # of zeros
            }


            if((enhigh2 <= 0x7  ||  enhigh2 > 0x7) && enlow2 > 0xF)
            {
                enhigh2 += 0x80;

            }

            QByteArray hold3 = hold3.number(enhigh2,16); //get # of non zero bytes
            encryptpacket2.append(hold3);
            encryptpacket2.append(hold6);
            encryptpacket2.append(encom6);
            enlow2 = 0;
            zerocounter2 = 0;

            do
            {

                encom5 = maintoonarray.mid(encarat3,2); //spin and count zeros
                zerocounter2++;
                encarat3 += 2;
                enlow2 = zerocounter2 - 1;

            } while (encom5 == "00");

            encarat3 -= 2;
            enhigh2 = 0;
            hold6 = "";
            encom6 = "";





        }

    } while(encarat3 < maintoonarray.size());

    encarat3 = 0;
    encryptpacket2.append("00");

    enhigh2 = 0;
    enlow2 = 0;

    //////////////////////////////RECOMPRESSION////////////////////////////

    ////qDebug() << "- maintoon -encryptpacket2" <<encryptpacket2;

    return  encryptpacket2;



}
