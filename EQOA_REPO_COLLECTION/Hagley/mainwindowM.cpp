#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "objectpacket.h"
#include "packetsize.h"
#include "generator.h"
#include "increment.h"
#include "coaching.h"
#include "targeting.h"
#include "xorpacket.h"
#include "merchants.h"

//Hagley-2 By Devin K
//Litany 60 Druid Marrs Fist
//Knights Templar
//Thanks to the EQOA Revival Team
//Thanks to Dustin and Mike for all the work and help
////////////////////////////Auto Spawn Objectpacket///////////////////////////////////
QStringList objectpacket::keptlist1;
QMap<QString,QString> objectpacket::sent_ids_map;
QMap<QString,QString> objectpacket::channelmap = {{"01","free"},{"02","free"},{"03","free"},{"04","free"},
                                                  {"05","free"},{"06","free"},{"07","free"},{"08","free"},
                                                  {"09","free"},{"0A","free"},{"0B","free"},{"0C","free"},
                                                  {"0D","free"},{"0E","free"},{"0F","free"},{"10","free"},
                                                  {"11","free"},{"12","free"},{"13","free"},{"14","free"},
                                                  {"15","free"},{"16","free"},{"17","free"}};
QMap<QString,QByteArray> objectpacket::incrementmap = {{"01","0000"},{"02","0000"},{"03","0000"},{"04","0000"},
                                                       {"05","0000"},{"06","0000"},{"07","0000"},{"08","0000"},
                                                       {"09","0000"},{"0A","0000"},{"0B","0000"},{"0C","0000"},
                                                       {"0D","0000"},{"0E","0000"},{"0F","0000"},{"10","0000"},
                                                       {"11","0000"},{"12","0000"},{"13","0000"},{"14","0000"},
                                                       {"15","0000"},{"16","0000"},{"17","0000"}};

QMap<QString,QString> objectpacket::Map_Channel_ID =
{
    {"01","00000000"},{"02","00000000"},{"03","00000000"},{"04","00000000"},{"05","00000000"},{"06","00000000"},
    {"07","00000000"},{"08","00000000"},{"09","00000000"},{"0A","00000000"},{"0B","00000000"},{"0C","00000000"},
    {"0D","00000000"},{"0E","00000000"},{"0F","00000000"},{"10","00000000"},{"11","00000000"},{"12","00000000"},
    {"13","00000000"},{"14","00000000"},{"15","00000000"},{"16","00000000"},{"17","00000000"}};

QMap<QString,float> objectpacket::Map_Channel_Distance =
{{"01",0},{"02",0},{"03",0},{"04",0},{"05",0},{"06",0},
 {"07",0},{"08",0},{"09",0},{"0A",0},{"0B",0},{"0C",0},
 {"0D",0},{"0E",0},{"0F",0},{"10",0},{"11",0},{"12",0},
 {"13",0},{"14",0},{"15",0},{"16",0},{"17",0}};

QList<float> objectpacket::distancelist1;



QStringList objectpacket::comparelist1;
QStringList objectpacket::extrafilteredlist;
QStringList objectpacket::sendingchannels;
QStringList objectpacket::filteredset1;
QMap<QString,QString>  objectpacket::map1;
QList<float> objectpacket::xyzlist;

QMap <QByteArray,QByteArray> objectpacket::Master_Map;
QStringList  objectpacket::holdx;
QStringList  objectpacket::holdy;
QStringList  objectpacket::holdz;
QStringList  objectpacket::holdid1;

///////////////////////////////Auto Spawn Objectpacket////////////////////////////////

///////////////////////////Generator//////////////
QMap<QByteArray,QByteArray> generator::Players_Map;

///////////////////////////Items/////////////////////////
QMap<QByteArray,QByteArray>merchants::Items_Map;

//////////////////////////////Coaching///////////////////////////////
QList <QString> Coaching::positionlist;
QMap <int,QByteArray> Coaching::swapnames;
QVector <int> Coaching::destination;
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////Teleport////////////////
QStandardItem *treetop;
QStandardItem *one;
QStandardItem *two;
QStandardItem *three;
QStandardItemModel  *standardModel = new QStandardItemModel; //Set Up The Tree Model
QStandardItem *rootNode = standardModel->invisibleRootItem();//Set Invisible Root Item
/////////////////////////////////////////////////////////////



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setInputMask("000.000.000.000");
    ui->pushButton_2->setDisabled(true);
    ui->pushButton_4->setDisabled(true);

    ui->actionReset_Main_Coordinates->setDisabled(true);
    ui->actionReset_All_Coaches->setDisabled(true);
    letmein.append(packet259);
    letmein.append(packet260);
    letmein.append(packet261);
    letmein.append(packet262);
    letmein.append(packet263);

    /////////////////////////DB offload to Map/////////////
    connect(watcher, SIGNAL(finished()),this, SLOT(mapfinished()));
    connect(watcher,SIGNAL(progressValueChanged(int)),this,SLOT(mapprogress(int)));
    ui->progressBar->setMinimum(0);
    ui->progressBar->reset();
    ///////////////////////////////////////////////////////

    /////////////////////////Teleport///////////////////
    letmeinagain.append(packetfc1);
    letmeinagain.append(packetfc1);
    letmeinagain.append(packetfc1);
    letmeinagain.append(packetfc1);
    letmeinagain.append(packetfc1);

    connect(&timer, &QTimer::timeout, this, &MainWindow::teleportoff);

    connect(&coachtimer,&QTimer::timeout,this,&MainWindow::coachingoff);

    connect(&loginpausing,&QTimer::timeout,this,&MainWindow::loginpause);
    ////////////////////////Teleport//////////////////

    ////////////////////////////Map/////////////////////
    scene = new QGraphicsScene(this);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    scene->setBackgroundBrush(QColor (95, 213, 253));
    item2 = scene->addPixmap(a);
    item2->setPos(210,230);
    scene->setSceneRect(1600,1600,100,100);
    item1 = scene->addPixmap(b.scaled(100,100));
    /////////////////////////////Map////////////////////
}

MainWindow::~MainWindow()
{
    watcher->deleteLater();
    if (f1.isRunning())
    {
        f1.cancel();
    }

    delete ui;
}



QHostAddress mainip;
QByteArray outip = "";
QByteArray byte1 = "";
QByteArray outcrc = "";
quint32 counter2 = 0;
float ax = 0.0;
float ay = 0.0;
float az = 0.0;
QString coorstr = "";
QString outextracoor = "";


QByteArray gamebuf1 = "";
QByteArray gamebuf2 = "";
int carata = 0;
int caratb = 0;
int where2 = 0;
QByteArray decopacket1 = "";
int high2 = 0;
int low2 = 0;
int packetsize2 = 0;
QByteArray decom40 = "";
QByteArray ophold1 = "";
int oppos1 = 0;
int opcarat = 0;
QByteArray mess1 = "";
int messint1 = 0;
int messcarat1 = 0;
int messint2 = 0;
int messint3 = 0;
QByteArray messext1 = "";
QByteArray messwork1 = "";
int chancounter1 = 0;
int chancounter2 = 0;
int chancounter3 = 0;
bool fortyexist = false;
bool messageexist = false;
bool opcodeexist = false;
int place1 = 0;
int placeholder1 = 0;
int positiona = 0;
QByteArray messageend = "";
QByteArray messageholder1 = "";
QByteArray opcoderesult1 = "";
QByteArray opholder = "";
QByteArray fortytwentninenum1 = "";
int packetcounter1 = 0;
QByteArray opmessagenumber1 = "";
QByteArray opmessagenumber2 = "";
int ninecounter1 = 0;
QByteArray fnine1 = "";
bool halfidmarker = false;
bool injectmarker = false;
bool twoninetysixmarker = false;
QByteArray combine1 = "";
QByteArray zerocounter = "0300";
bool flipped = false;
QByteArray holdercustom2 = "";


void MainWindow::on_pushButton_clicked() //set up ip
{


    QString ip2 = ui->lineEdit->text(); //get ip from ui
    qDebug() <<"ip2" << ip2;
    bool getit =  mainip.setAddress(ip2); //set ip address in hostadress
    qDebug() << getit;
    if (getit == false)
    {
        QMessageBox msgBox3; //something bad happened
        msgBox3.setText("Please Enter IP");
        msgBox3.exec();
        return;
    }

    if (!db.open())
    {
        QMessageBox msgBox4; //something bad happened
        msgBox4.setText("Please Load Database");
        msgBox4.exec();
        ui->actionReset_Main_Coordinates->setDisabled(true);
        ui->actionReset_All_Coaches->setDisabled(true);
        return;
    }

    /////UI//////
    ui->actionAbout_Hagley2->setDisabled(true);
    ui->actionLoad_Database->setDisabled(true);
    ui->lineEdit->setDisabled(true);
    ui->pushButton_2->setDisabled(false);
    ui->pushButton->setDisabled(true);
    ui->textEdit_5->append("Started UDP Server...");
    ui->textEdit_5->append("Started TCP Server...");
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();
    ui->lineEdit_10->clear();
    ui->lineEdit_11->clear();
    ui->lineEdit_12->clear();
    ui->lineEdit_13->clear();
    ui->lineEdit_14->clear();
    ui->lineEdit_15->clear();
    ui->lineEdit_16->clear();
    ui->lineEdit_17->clear();

    /////UI//////
    QString hexip = QString("%1").arg(QHostAddress(mainip).toIPv4Address(), 8, 16, QLatin1Char( '0' )); //make sure we get a 8 byte ip

    qDebug() << "hexip" << hexip;
    outip = hexip.toUtf8(); //move ip to array
    qDebug() << "outip" << outip;

    //teleport
    teleport_ip = outip; //save ip for use for teleport
    //teleport

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Start all the sockets..you can bool error check these if u want i didnt bother
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    udpsocket.bind(mainip, 53); //start udp and tcp
    connect(&udpsocket, SIGNAL(readyRead()), this, SLOT(read_pending()));

    server.listen(mainip,7000); //patch port 7000
    connect(&server, SIGNAL(newConnection()), this, SLOT(acceptconnection()));

    server2.listen(mainip,9735); //login server
    connect(&server2, SIGNAL(newConnection()), this, SLOT(loginagain()));


    udpsocket2.bind(mainip,10070); //start game port 10070
    connect(&udpsocket2, SIGNAL(readyRead()), this, SLOT(gameread1()));


    udpsocket3.bind(mainip,10071); //start game port 10071
    connect(&udpsocket3, SIGNAL(readyRead()), this, SLOT(gameread2()));

    //////////////////////////////////DB offload to Map///////////////////////
    tablesize = 0;
    tablesize2 = 0;
    generator::Players_Map.clear();
    ui->progressBar->setMinimum(0);
    ui->progressBar->reset();
    objectpacket::Master_Map.clear(); //clear here if we remake the map due to stopping or reloading db
    objectpacket::holdx.clear();
    objectpacket::holdy.clear();
    objectpacket::holdz.clear();
    objectpacket::holdid1.clear();
    ui->textEdit_5->append("Offloading Database......\nPlease Wait.........");
    db.transaction();
    QString newtext = "NPCS";
    QString playtext = "MAINTOON";
    QString itemtext = "ITEMS";

    QSqlQuery query3;
    query3.prepare(QString("SELECT COUNT (*) FROM %1").arg(newtext));
    query3.exec();
    while (query3.next())
    {
        tablesize =  query3.value(0).toInt(nullptr);
    }

    QSqlQuery query4;
    query4.prepare(QString("SELECT COUNT (*) FROM %1").arg(playtext)); //get size of players db
    query4.exec();
    while(query4.next())
    {
        tablesize2 = query4.value(0).toInt(nullptr);
    }
    db.commit();
    qDebug() << "DB Npc Tablesize" << tablesize;

    QSqlQuery query5;
    query5.prepare(QString("SELECT COUNT (*) FROM %1").arg(itemtext)); //get size of players db
    query5.exec();
    while(query5.next())
    {
        tablesize3 = query5.value(0).toInt(nullptr);
    }




    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(tablesize);
    f1 =  QtConcurrent::run(this,&MainWindow::mapconstruction);
    watcher->setFuture(f1);



}

void MainWindow::mapfinished()
{

    ui->textEdit_5->append("Offloading Complete........\nWaiting For Client......");
}
void MainWindow::mapprogress(int val1)
{
    ui->progressBar->setValue(val1 + 1);
}

void MainWindow::mapconstruction()
{

    QString playertext  = "MAINTOON";
    db.transaction();
    //   qDebug() << "tablesize2" <<tablesize2;
    for (rotate2 = 1; rotate2 <= tablesize2; rotate2++) //equal cause db starts on row 1
    {

        QSqlQuery  query3 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(playertext).arg(rotate2));

        while(query3.next())
        {
            QSqlRecord record2 = query3.record();

            for(int j=2; j < record2.count(); j++)
            {
                rowline2.append(record2.value(j).toString());

            }
        }
        QByteArray  Play_ID = rowline2.mid(2,8).toUtf8();
        generator::Players_Map.insert(Play_ID,rowline2.toUtf8());
        qDebug() << "MasterMap-Player"<<  Play_ID;
        rowline2.clear();

    }
    db.commit();
    rotate2 = 0;




    ////////////////////////////////////////////////////////////////
    QString newtext = "NPCS";
    db.transaction();
    for (rotate = 1; rotate < tablesize + 1; rotate++) //use the number of rows of npcs table
    {

        QSqlQuery  query4 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(newtext).arg(rotate));

        emit watcher->progressValueChanged(rotate);

        while (query4.next())
        {
            QSqlRecord record = query4.record();
            for(int i=2; i < record.count(); i++)
            {
                rowline.append(record.value(i).toString());
            }
        }
        QByteArray Main_ID = rowline.mid(2,8).toUtf8();
        objectpacket::Master_Map.insert(Main_ID,rowline.toUtf8());
        objectpacket::holdx.append(rowline.mid(12,6));
        objectpacket::holdz.append(rowline.mid(18,6));
        objectpacket::holdy.append(rowline.mid(24,6));
        objectpacket::holdid1.append(Main_ID);
        qDebug() << "MasterMap"<< Main_ID;
        rowline.clear();

    }
    db.commit();
    rotate = 0;


    //Items
    QString itemtext  = "ITEMS";
    db.transaction();
    qDebug() << "tablesize3" <<tablesize3;
    for (rotate3 = 1; rotate3 <= tablesize3; rotate3++) //equal cause db starts on row 1
    {

        QSqlQuery  query5 (QString("SELECT * FROM %1 WHERE rowid=%2").arg(itemtext).arg(rotate3));

        while(query5.next())
        {
            QSqlRecord record5 = query5.record();

            for(int k=4; k < record5.count(); k++) //read 4 past
            {
                rowline3.append(record5.value(k).toString());

            }
        }
        int item_pos1 = rowline3.indexOf("XXXXXXXX");
        QByteArray  Items_ID = rowline3.mid(item_pos1 + 8,10).toUtf8();
        merchants::Items_Map.insert(Items_ID,rowline3.toUtf8()); //master item map here
        qDebug() << "MasterMap-Items"<<  Items_ID;
        rowline3.clear();

    }
    db.commit();
    rotate3 = 0;

}


void MainWindow::read_pending() //DNS calls
{
    ui->actionReset_Main_Coordinates->setDisabled(true); //grey out changing db once we start getting packets
    ui->actionReset_All_Coaches->setDisabled(true);

    while(udpsocket.hasPendingDatagrams())
    {

        masterserver = "2A00"; //reset the counter if stop isnt selected 2A00
        QByteArray datagram;
        datagram.resize(udpsocket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpsocket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QByteArray hold = datagram.toHex().toUpper();
        QByteArray middle1 = hold.mid(26,16);
        QByteArray udpcheck1 = middle1.toUpper();
        qDebug() <<"udpcheck1" << udpcheck1;



        QDateTime dateTime = dateTime.currentDateTime();
        QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Client DNS-" + dateTimeString);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText(hold.toUpper());
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Client-DNS-" + dateTimeString);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->moveCursor(QTextCursor::End);




        if (udpcheck1 == "6761746531027573") //GATE DNS CALL
        {

            ui->lineEdit_2->clear(); //clear the middle of ui since its a new connection
            ui->lineEdit_3->clear();
            ui->lineEdit_4->clear();
            ui->lineEdit_5->clear();
            ui->lineEdit_6->clear();
            ui->lineEdit_10->clear();
            ui->lineEdit_11->clear();
            ui->lineEdit_12->clear();
            ui->lineEdit_13->clear();
            ui->lineEdit_14->clear();
            ui->lineEdit_15->clear();
            ui->lineEdit_16->clear();
            ui->lineEdit_17->clear();

            QByteArray header = hold.mid(0,4); //get 2 byte header from received packet
            QByteArray gateout1 = gatedns;
            header.append(gateout1);
            QByteArray gateresult1 = header.append(outip); //add ip to the packet
            QByteArray finalgate1 = gateresult1.fromHex(gateresult1); //change from hex to send it out
            udpsocket.writeDatagram(finalgate1, sender, senderPort);
            ui->textEdit_5->append("Gate DNS Done...");

            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->appendPlainText("Server-Gate-" + dateTimeString);
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->appendPlainText(gateresult1.toUpper());
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->appendPlainText("Server-Gate-" + dateTimeString);
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->moveCursor(QTextCursor::End);





        }


        if (udpcheck1 == "7061746368196576") //PATCH DNS CALL
        {

            QByteArray header2 = hold.mid(0,4); //get 2 byte header from received packet
            QByteArray gateout2 = patchdns;
            header2.append(gateout2);
            QByteArray patchresult1 = header2.append(outip);
            QByteArray finalpatch1 = patchresult1.fromHex(patchresult1);
            udpsocket.writeDatagram(finalpatch1, sender, senderPort);
            ui->textEdit_5->append("Patch DNS Done...");

            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->appendPlainText("Server-Patch-" + dateTimeString);
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->appendPlainText(patchresult1.toUpper());
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->appendPlainText("Server-Patch-" + dateTimeString);
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->moveCursor(QTextCursor::End);


        }


    }
}







void MainWindow::acceptconnection() //GET PATCH request
{
    qDebug() << "tcpsocket in use";
    tcpsocket = server.nextPendingConnection();
    connect(tcpsocket,SIGNAL(readyRead()),this,SLOT(readpacket()));

}


void MainWindow::readpacket()
{

    QByteArray patchtcp1 = "";
    patchtcp1.resize(tcpsocket->bytesAvailable());
    patchtcp1.append(tcpsocket->read(tcpsocket->bytesAvailable()));
    QByteArray hold3 = patchtcp1.toHex().toUpper();
    ui->textEdit_4->append(hold3);
    ui->textEdit_4->append("_______Client Tcp________");
    QByteArray middle2 = hold3.mid(208,20);
    QByteArray check1 = middle2.toUpper();
    qDebug() << "tcpcheck1" << check1;

    if (check1 == "474554202F7061746368") //GET PATCH Request
    {

        QByteArray httphead ="HTTP/1.1 200 OK\r\n";
        httphead += "Content-Type: application/octet-stream\r\n";
        httphead += "Content-Length: 145\r\n";
        httphead += "Accept-Ranges: bytes\r\n";
        httphead += "Content-Disposition: attachment; filename=\"eqoa-frontierslive-update.xml.gz\";\r\n\r\n";

        QString filename = "/eqoa-frontierslive-update.xml.gz";
        QString directory = QCoreApplication::applicationDirPath();
        directory += filename;
        qDebug() <<"file" <<directory;


        QFile masterfile(directory);
        if (!masterfile.open(QIODevice::ReadOnly)) //Open File
        {
            QMessageBox msgBox1; //something bad happened
            msgBox1.setText(masterfile.errorString());
            msgBox1.exec();
            qDebug() << masterfile.errorString();
            return;
        }


        httphead += masterfile.readAll();
        masterfile.close();

        ui->textEdit_4->append(httphead); //send to ui
        ui->textEdit_4->append("_______Patch Tcp_________");
        tcpsocket->write(httphead); //write the data out
        ui->textEdit_5->append("Patch Done...");

    }


    QByteArray middle3 = hold3.mid(220,28);
    QByteArray motd1 = middle3.toUpper();
    qDebug() << "motd1" << motd1;

    if (motd1 == "6D6F74642F6E6577732E68746D6C") //motd news
    {

        QByteArray newshead = "HTTP/1.1 200 OK\r\n";
        newshead += "Content-Type: text/html\r\n";
        newshead += "Content-Length: 43\r\n";
        newshead += "Accept-Ranges: bytes\r\n";
        newshead += "Content-Disposition: attachment; filename=\"news.html\";\r\n\r\n";
        QString filename2 = "/news.html";
        QString directory2 = QCoreApplication::applicationDirPath();
        directory2 += filename2;
        qDebug() <<"file2" << directory2;

        QFile masterfile2(directory2);
        if (!masterfile2.open(QIODevice::ReadOnly)) //Open File
        {
            QMessageBox msgBox2; //something bad happened
            msgBox2.setText(masterfile2.errorString());
            msgBox2.exec();
            qDebug() << masterfile2.errorString();
            return;
        }


        newshead += masterfile2.readAll();
        masterfile2.close();

        ui->textEdit_4->append(newshead);
        ui->textEdit_4->append("__________News Tcp__________");
        tcpsocket->write(newshead);
        ui->textEdit_5->append("News Done...");

    }

}

void MainWindow::loginagain()
{
    qDebug() << "tcpsocket2 in use";
    tcpsocket2 = server2.nextPendingConnection();
    connect(tcpsocket2,SIGNAL(readyRead()),this,SLOT(readagain()));


}






void MainWindow::readagain() //LOGIN
{
    QByteArray login1 = "";
    login1.resize(tcpsocket2->bytesAvailable());
    login1.append(tcpsocket2->read(tcpsocket2->bytesAvailable()));
    QByteArray hold4 = login1.toHex().toUpper();
    ui->textEdit_4->append(hold4);
    ui->textEdit_4->append("________Client Login________");
    QByteArray logrsp = login;
    QByteArray logdone = logrsp.fromHex(logrsp);
    ui->textEdit_4->append(logrsp);
    ui->textEdit_4->append("________Login________");
    tcpsocket2->write(logdone);
    ui->textEdit_5->append("Login Done...");

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///game starts here///
///////////////////////////////////////////////////////////////////////////////////////////////////////////



void MainWindow::gameread1() //Server Select
{

    /////////////////////////Ui//////////////
    ui->lineEdit_12->clear();
    ui->lineEdit_13->clear();
    ui->lineEdit_17->clear();
    ringbuffer1 = "";
    currentxor = 0;
    xorresult1 = 0;
    xorresult2 = 0;
    xorresult3 = 0;
    res1 = 0;
    res2 = 0;
    res3 = 0;
    xdeconumber1 = 0;
    ydeconumber1 = 0;
    zdeconumber1 = 0;
    xstack.clear();
    ystack.clear();
    zstack.clear();
    fstack.clear();
    ecchannel = "0100"; //reset channel count on 42ec if we arrive back here
    teleport_destination.clear(); //clearing teleport
    ui->textEdit->clear();
    ui->textEdit_3->clear();
    ui->textEdit_6->clear();

    senderheld1.clear();
    senderPortheld1 = 0;
    masterx.clear();
    mastery.clear();
    masterz.clear();
    xout1.clear();
    yout1.clear();
    zout1.clear();
    worldout1.clear();
    facingout1.clear();
    serveropcode1 = "0000";
    client_opcode = "0000";
    masterworld = 0;
    clientheld_id = "";
    serverheld_id = "";
    last_opcode_messnum = "0000";
    coach_counter = "0000";
    coachreply_counter = "0000";
    client_order = "03000400";
    C_9007_endcount = "0200";
    C_9007_servidcount = "D37C";
    C_closer9007_endcount = "0000";

    //resetting maps in objectpacket on server select
    QMapIterator<QString,QString> iter (objectpacket::channelmap); //go thru map to get ids and ranges
    while (iter.hasNext())
    {

        iter.next();
        QString chkey1 = iter.key();
        QString mapval1 = "free";
        objectpacket::channelmap.insert(chkey1,mapval1);

    }

    //   qDebug() << "objectpacket channel map clear - server select" << objectpacket::channelmap;

    QMapIterator<QString,QByteArray> iter2 (objectpacket::incrementmap); //go thru map to reset
    while (iter2.hasNext())
    {

        iter2.next();
        QString inkey2 = iter2.key();
        QByteArray mapval2 = "0000";
        objectpacket::incrementmap.insert(inkey2,mapval2);

    }

    //        qDebug() << "objectpacket increment map clear - server select" << objectpacket::incrementmap;
    //resetting maps in objectpacket on server select
    objectpacket::sent_ids_map.clear();



    while(udpsocket2.hasPendingDatagrams())
    {
        halfidmarker = false;
        injectmarker = false;
        twoninetysixmarker = false;
        flipped = false;
        masterserver = "2A00"; //reset the server message number if we arrive back here instead of login
        zerocounter = "0300"; //reset zero counter
        mastertelinc = "0000"; //reset teleport end counter
        C_9007_endcount = "0200"; //reset 9007 endcount
        C_9007_servidcount = "D37C"; //reset teleport ids
        C_closer9007_endcount = "0000"; //reset closer increment
        last_opcode_messnum = "0000";
        coach_counter = "0000";
        coachreply_counter = "0000";
        client_order = "03000400"; //reset early login packets
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        QByteArray datagram2;
        datagram2.resize(udpsocket2.pendingDatagramSize());
        QHostAddress sender2;
        quint16 senderPort2;

        udpsocket2.readDatagram(datagram2.data(), datagram2.size(), &sender2, &senderPort2);
        gamebuf2 = datagram2.toHex().toUpper();




        QDateTime dateTime = dateTime.currentDateTime();
        QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Client-" + dateTimeString);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText(gamebuf2.toUpper());
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Client-" + dateTimeString);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->moveCursor(QTextCursor::End);


        QByteArray clientid2 = gamebuf2.mid(0,4); //get client id
        ui->lineEdit_2->setText(clientid2);
        QByteArray serverid2 = gamebuf2.mid(4,4); //get server id
        ui->lineEdit_3->setText(serverid2);
        QByteArray messageid2 =gamebuf2.mid(12,16); //get session id
        ui->lineEdit_11->setText(messageid2); //sessionid
        QByteArray messagenumb2 =  gamebuf2.mid(28,4); //get messagenumber client 28 server 30
        ui->lineEdit_4->setText(messagenumb2); //client message #

        QByteArray serverflag1 = gamebuf2.mid(12,2);
        QByteArray serverhello = gamebuf2.mid(22,14);
        QByteArray optionflag1 = gamebuf2.mid(18,2); //read server select option flag usually 01-03 after the client ids
        QByteArray kieshaesha = gamebuf2.mid(52,26);
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        if (kieshaesha == "02000409000300000004000000")
        {

            //packet 155 - load packet 156
            QByteArray holder156 = packet156;
            //keep server id clients is FFEF
            holder156.replace(4,4,clientid2); //write client id into response 3 places
            holder156.replace(12,4,clientid2);
            holder156.replace(22,4,clientid2);
            holder156.replace(16,2,optionflag1);
            holder156.replace(26,2,optionflag1);

            QByteArray serverno1 = holder156.mid(30,4);
            ui->lineEdit_5->setText(serverno1.toUpper());
            byte1 = holder156; //send array to crc
            elcrc(byte1); //send to get a crc
            holder156.append(outcrc);

            QByteArray gameout1 =holder156.fromHex(holder156);
            udpsocket2.writeDatagram(gameout1, sender2, senderPort2);

            QDateTime dateTime = dateTime.currentDateTime();
            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->appendPlainText("Server-156-" + dateTimeString);
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->appendPlainText(holder156.toUpper());
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->appendPlainText("Server-156-" + dateTimeString);
            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
            ui->plainTextEdit->moveCursor(QTextCursor::End);

        }



        if (serverflag1 == "01" && serverhello == "23020001000100")

        {

            QByteArray session1 = messageid2; //get session id
            QByteArray chopped2 = session1.mid(2,4);

            if (chopped2 == clientid2) //are they the same?
            {
                //same id - packet 157 - load server select - packet 158
                QByteArray holder158 = packet158;

                //keep server id for now
                holder158.replace(4,4,clientid2); //write client id into response 2 places
                holder158.replace(12,4,clientid2);
                holder158.replace(16,2,optionflag1); //add server select 03-05 option from client


                QByteArray originalip = outip; //get current ip

                QByteArray first2 =  originalip.mid(0,2); //reverse it
                QByteArray second2 = originalip.mid(2,2);
                QByteArray third2 =  originalip.mid(4,2);
                QByteArray fourth2 = originalip.mid(6,2);

                QByteArray bigendianip = fourth2+= third2 += second2 += first2;

                ///////////////////////ip replacement table/////////////////////////////
                holder158.replace(122,8,bigendianip); //castle lightwolf
                holder158.replace(190,8,bigendianip); //diren hold
                holder158.replace(270,8,bigendianip); //ferrans hope
                holder158.replace(330,8,bigendianip); //hodstock
                holder158.replace(402,8,bigendianip); //marrs fist
                holder158.replace(498,8,bigendianip); //proudpine outpost
                holder158.replace(578,8,bigendianip); //hagley test
                ///////////////////////ip replacement table////////////////////////////
                //i also changed all 56s to 57 to go on 10071


                byte1 = holder158; //send array to crc
                elcrc(byte1); //send to get a crc
                holder158.append(outcrc);

                QByteArray gameout2 = holder158.fromHex(holder158);
                udpsocket2.writeDatagram(gameout2, sender2, senderPort2);
                ui->textEdit_5->append("Server Select Done...");


                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-Server-Select-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holder158.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-Server-Select-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);

                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////clear out the channels
                for (int y = 0 ; y <= 54; y+=2)
                {

                    QByteArray channels2 = "000102030405060708090A0B0C0D0E0F101112131415161740414243";
                    QByteArray channelsmix2 = channels2.mid(y,2); //get channels
                    int chan2 = QString("%1").arg(channelsmix2.toInt(0,16),2,16,QLatin1Char('0')).toInt(0,16);
                    QString values2 = "0000";
                    channel_map.insert(chan2,values2); //insert them all
                }

                chancounter2 = 0;
                foreach (QString value2 , channel_map)

                {
                    QTextEdit* entries2[] = { ui->textEdit_12,ui->textEdit_13,ui->textEdit_14,ui->textEdit_15,ui->textEdit_16,
                                              ui->textEdit_17,ui->textEdit_18,ui->textEdit_19,ui->textEdit_20,ui->textEdit_21,
                                              ui->textEdit_22,ui->textEdit_23,ui->textEdit_24,ui->textEdit_25,ui->textEdit_26,
                                              ui->textEdit_27,ui->textEdit_28,ui->textEdit_29,ui->textEdit_30,ui->textEdit_31,
                                              ui->textEdit_32,ui->textEdit_33,ui->textEdit_34,ui->textEdit_35,ui->textEdit_36,
                                              ui->textEdit_37,ui->textEdit_38,ui->textEdit_39};


                    entries2[chancounter2]->setText(value2.toUpper());
                    chancounter2++;

                }
                chancounter2 = 0;
            }

        }


    }



}



void MainWindow::gameread2() //character select 10071
{

    while(udpsocket3.hasPendingDatagrams())
    {

        QByteArray datagram3;
        datagram3.resize(udpsocket3.pendingDatagramSize());

        QHostAddress sender3;
        quint16 senderPort3;
        udpsocket3.readDatagram(datagram3.data(), datagram3.size(), &sender3, &senderPort3);
        gamebuf1 = datagram3.toHex().toUpper();

        QDateTime dateTime = dateTime.currentDateTime();
        QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Client-" + dateTimeString);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText(gamebuf1.toUpper());
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Client-" + dateTimeString);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->moveCursor(QTextCursor::End);
        //     qDebug() << "client packet" <<gamebuf1;

        //teleport
        senderheld1 = sender3;
        senderPortheld1 = senderPort3;

        //teleport

        if (halfidmarker == true && twoninetysixmarker == true)
        {
            QByteArray fninehalfid1 = gamebuf1.mid(20,6);

            if (fninehalfid1 != "D9BC0D")
            {
                QByteArray frontheader1 = gamebuf1.mid(0,12);
                //        qDebug() << "frontheader" << frontheader1;
                QByteArray middleheader1 = "BF543213";
                //         qDebug() << "middleheader" << middleheader1;
                int length1 = gamebuf1.size() - 12;
                QByteArray endheader1 = gamebuf1.mid(12,length1);
                //         qDebug() << "endheader" << endheader1;
                QByteArray restructured1 = frontheader1+= middleheader1 += endheader1;
                gamebuf1 = restructured1;
                //          qDebug() << "gamebuf1" << gamebuf1;
            }

        }




        oldclientval = gamebuf1.mid(0,4); //get client id
        if (oldclientval.toUpper() != "FFFF")
        {

            clientid1 = oldclientval.toUpper(); //TESTING FILTERING OF FFFF
            ui->lineEdit_2->setText(clientid1.toUpper());
            //teleport
            clientheld_id = clientid1.toUpper();
            //teleport


            //////////////////////////
            serverid1 = gamebuf1.mid(4,4); //get server id
            ui->lineEdit_3->setText(serverid1.toUpper());
            //teleport
            serverheld_id = serverid1.toUpper();
            //teleport
            messageid =gamebuf1.mid(12,16); //get session id
            ui->lineEdit_11->setText(messageid); //sessionid
            messagenumb1 =  gamebuf1.mid(28,4); //get messagenumber
            ui->lineEdit_4->setText(messagenumb1); //client message #
        }
        QByteArray optionflag1 = gamebuf1.mid(18,2); //read character select option flag usually 01-03 after the client ids
        QByteArray clientffcall = gamebuf1.mid(0,8); //find FFFF9209xxxx


        //////////////////////////////////////////////////////////////////////Opcodes////////////////////////////


        gamebuf1.remove(gamebuf1.size() - 8,8); //remove crc
        ophold1 = gamebuf1.toUpper(); //get the packet
        ophold1.remove(0,32); //remove headers
        packetsize2 = ophold1.size();


        do
        {
            oppos1 = ophold1.indexOf("FB",opcarat); //find FB
            //           qDebug() << "oppos1" <<oppos1;
            int boundcheck1 = ophold1.indexOf("4029"); //are we inside a 4029?
            //              qDebug() << "boundcheck" << boundcheck1;

            if (boundcheck1 != -1 && boundcheck1 < oppos1)
            {
                //                          qDebug() << "inside 4029";

                break;
            }



            if (oppos1 == -1)
            {

                break;
            }

            //               qDebug() <<"oppos" << oppos1;

            opcarat = oppos1 + 2;
            //               qDebug() << "opcarat" <<opcarat;


            QByteArray oplen = ophold1.mid(opcarat,2);
            int opresult = oplen.toInt(0,16);
            opcarat = oppos1;
            //       qDebug() << "opcaratb" <<opcarat;
            QByteArray opout = ophold1.mid(opcarat,opresult * 2 + 8);
            opholder = opout; //save our output
            ui->textEdit_7->append(opout); //send opcodes out to screen even if they are wrong

            opcarat += opresult * 2 + 6;
            //         qDebug() << "opcarat3" <<opcarat;

            QByteArray stripped1 = opout.remove(0,8); //remove the FB size and message number
            QByteArray opcoderesult2 = stripped1.mid(0,4); //get opcode
            QByteArray opcodecheck = opcoderesult2;

            if (opvect.contains(opcodecheck)) //check against the list to see if its a real opcode
            {
                opcodeexist = true;
                ui->textEdit_9->setText(opholder); //send opcodes out to screen
                opmessagenumber2  = opholder.mid(4,4); //get opcode number
                ui->lineEdit_6->setText(opmessagenumber2);  //set opcode message on ui
                client_opcode = opmessagenumber2; //save client opcode number
                last_opcode_messnum = gamebuf1.mid(28,4); //save last message number that had a opcode

            }
            else
            {
                //not a known opcode
                opcodeexist = false;
                opcoderesult1 = "9999"; //null it out before sending down
            }

        }
        while (oppos1 != -1);

        opcarat = 0;
        oppos1 = 0;
        packetsize2 = 0;






        ////////////////////////////////////////////////////////4029 Decryption///////////////////////////////////


        //crc is gone on gamebuf1
        packetsize2 = gamebuf1.size();
        //      qDebug() << "packetsize2" << packetsize2;
        where2 = gamebuf1.indexOf("4029",32); //find 4029
        //        qDebug() <<"where2" <<where2;
        carata = where2 + 10;

        if (where2 == -1)
        {
            carata = gamebuf1.size(); //set to end if no 4029
            fortyexist = false;

        }

        else
        {
            fortyexist = true;
            fortytwentninenum1 = gamebuf1.mid(where2+ 4,4); //4029 number
            QByteArray holdmyxor = gamebuf1.mid(where2+ 8,2); //Xor
            currentxor = holdmyxor.toInt(0,16);
            //            qDebug() << "CURRENT XOR" <<currentxor;
            ui->lineEdit_12->setText(fortytwentninenum1.toUpper());
            ui->lineEdit_13->setText(holdmyxor.toUpper());
            combine1 = messagenumb1; //client message number

            QByteArray worker1 = opmessagenumber2;
            worker1.append("40");
            combine1 += worker1;
            worker1 = ""; //clear it out

            QByteArray combine2 = fortytwentninenum1; //4029 message number
            combine1+= combine2+= "F8"; //gives us the replies to the 4029s
            //       qDebug() << "COMBINE1" <<combine1;





            do
            {


                decom40 = gamebuf1.mid(carata,2); //get first compressed
                int over1 = decom40.toInt(0,16); //change to int
                //      qDebug() <<"decom40" << decom40;

                if (over1 > 0x79) //is it over 79h?
                {
                    decom40 = gamebuf1.mid(carata,4); //get two bytes
                    int hold1 = decom40.mid(0,2).toInt(0,16); //get highbyte
                    high2 = hold1 - 0x80;  //subtract 80h
                    //        qDebug() << "high2" << high2;
                    low2 = decom40.mid(2,2).toInt(0,16); //get lowbyte
                    carata += 4; //increment by what was read
                    //          qDebug() <<"low2" <<low2;
                }



                else
                {
                    high2 = decom40.mid(0,1).toInt(0,16); //get one byte
                    low2 = decom40.mid(1,1).toInt(0,16); //get one byte

                    if (high2 == 0 && low2 == 0) //breakout when done
                    {
                        //               qDebug() << "two zeros" <<high2 << low2;
                        where2 = gamebuf1.indexOf("4029",carata); //find 4029
                        //                 qDebug() <<"where22" <<where2;

                        if (where2 == -1)
                        {
                            break;
                        }

                        carata = where2 + 8;

                    }
                    carata += 2; //increment by what was read
                }


                QByteArray zeros = "00"; //zero byte
                QByteArray zeroholder = ""; //hold number of zeros
                zeroholder = zeros.repeated(low2); //repeat #
                decopacket1.insert(caratb,zeroholder); //insert zeros
                caratb += low2 * 2; //set new carat position
                QByteArray decom2 = gamebuf1.mid(carata,high2 * 2); //get bytes to keep
                decopacket1.insert(caratb,decom2); //insert kept bytes
                carata += high2 *2 ; //set new carat position
                caratb += high2 *2; //set new carat position

                //    qDebug() <<"highbyte" <<high2;
                //     qDebug() <<"lowbyte" <<low2;
                //       qDebug() <<"packet" <<decopacket1;
                //       qDebug() << "carata" <<carata;



            }
            while (carata  <  packetsize2 -2);

            //       qDebug() << "DECO" <<decopacket1;
            ui->textEdit_8->setText(decopacket1);
            ui->plainTextEdit_2->appendPlainText(decopacket1);
            ui->plainTextEdit_2->moveCursor(QTextCursor::End);

            //////////////////////////////4029 xyz///////////////////////////////////////

            if (fortytwentninenum1 == "0100") //set the buffers to get the first 3 4029s
            {
                ringbuffer1 = decopacket1;
                QByteArray xsnip1 = ringbuffer1.mid(2,6); //get X
                QByteArray ysnip1 = ringbuffer1.mid(14,6); //get Y
                QByteArray zsnip1 = ringbuffer1.mid(8,6); //get Z
                QByteArray fsnip1 = ringbuffer1.mid(44,2); //get Facing
                int xnum1 = xsnip1.toInt(0,16);
                int ynum1 = ysnip1.toInt(0,16);
                int znum1 = zsnip1.toInt(0,16);
                int fnum1 = fsnip1.toInt(0,16);
                xstack.push(xnum1); //this will go off twice
                ystack.push(ynum1);
                zstack.push(znum1);
                fstack.push(fnum1);


            }
            if (fortytwentninenum1 == "0200")
            {

                QByteArray xsnip2 = ringbuffer1.mid(2,6); //get X from 0100 again
                QByteArray ysnip2 = ringbuffer1.mid(14,6); //get Y
                QByteArray zsnip2 = ringbuffer1.mid(8,6); //get Z
                QByteArray fsnip2 = ringbuffer1.mid(44,2); //get Facing
                int xnum2 = xsnip2.toInt(0,16);
                int ynum2 = ysnip2.toInt(0,16);
                int znum2 = zsnip2.toInt(0,16);
                int fnum2 = fsnip2.toInt(0,16);
                xstack.push(xnum2); //this will go off once
                ystack.push(ynum2);
                zstack.push(znum2);
                fstack.push(fnum2);
            }

            if (currentxor == 0)
            {
                QByteArray facingbyte1 = decopacket1.mid(44,2); //get facing
                facedeconumber1 = facingbyte1.toInt(0,16);
                fstack.push(facedeconumber1);

                if (fstack.size()  >  255)
                {
                    fstack.removeAt(0);
                }

                if (facedeconumber1 >= 0x80)
                {
                    res4 = facedeconumber1 - 0x80;
                    QString facerez10 = QString("%1").arg(res4,2,16,QLatin1Char('0'));
                    ui->lineEdit_17->setText(facerez10.toUpper());
                }
                else
                {
                    res4 = facedeconumber1 + 0x80;
                    QString facerez11 = QString("%1").arg(res4,2,16,QLatin1Char('0'));
                    ui->lineEdit_17->setText(facerez11.toUpper());
                }
            }

            if (currentxor != 0 && currentxor <= 255)
            {
                QByteArray faceholder1 = decopacket1.mid(44,2); //get Facing
                facedeconumber1 = faceholder1.toInt(0,16);
                int facestackpos1 = currentxor;
                int facestackcount1 = fstack.size();
                //            qDebug() << "Check-F-Position" << facestackpos1;
                //             qDebug() << "Check-F-Count" << facestackcount1;
                int faceclientnumber1 = fstack.at(facestackcount1 - facestackpos1);
                faceresult1 = facedeconumber1 ^ faceclientnumber1; //xor with ringbuffer
                fstack.push(faceresult1);

                if (fstack.size()  >  255)
                {
                    fstack.removeAt(0);
                }

                //               qDebug() << "FSTACK" <<fstack;
                if (faceresult1 >= 0x80)
                {
                    res4 = faceresult1 - 0x80;
                    QString facerez12 = QString("%1").arg(res4,2,16,QLatin1Char('0'));
                    ui->lineEdit_17->setText(facerez12.toUpper());

                }
                else
                {
                    res4 =  faceresult1 + 0x80;
                    QString facerez13 = QString("%1").arg(res4,2,16,QLatin1Char('0'));
                    ui->lineEdit_17->setText(facerez13.toUpper());

                }
            }


            ////////////////////////////////////////////Facing///////////////////////////

            /////////////////////////////////////////////X//////////////////////


            if (currentxor == 0)
            {
                QByteArray xholderzero = decopacket1.mid(2,6); //get X
                xdeconumber1 = xholderzero.toInt(0,16);
                res1 = xdeconumber1 - 0x1C71C7;

                //             qDebug() << "res-X-zero" << res1;
                xstack.push(xdeconumber1);

                if (xstack.size()  >  255)
                {
                    xstack.removeAt(0);
                }

            }

            if (currentxor != 0 && currentxor <= 255)
            {
                QByteArray xholder1 = decopacket1.mid(2,6); //get X
                xdeconumber1 = xholder1.toInt(0,16);
                int stackposx1 = currentxor; //get current xor in decimal
                int stackcountx1 = xstack.size(); //get current size of stack
                //           qDebug() << "Check-X-Position" << stackposx1;
                //            qDebug() << "Check-X-Count" << stackcountx1;
                int xclientnumber1 = xstack.at(stackcountx1 - stackposx1); //read at position
                xorresult1 = xdeconumber1 ^ xclientnumber1; //xor with ringbuffer
                xstack.push(xorresult1);

                if (xstack.size()  >  255)
                {
                    xstack.removeAt(0);
                }

                //             qDebug() << "XSTACK" <<xstack;
                res1 =  xorresult1 - 0x1C71C7;

            }





            float fourty = res1 / 466.033798648;
            int helper = fourty * 128.0; //change to float 128.0 for full float

            QString fourtxout = QString("%1").arg(helper,6,16,QLatin1Char('0'));
            ui->lineEdit_14->setText(fourtxout.toUpper()); //3 byte X
            masterx = fourtxout.toUtf8();

            ui->lineEdit_18->setText(fourtxout.toUpper()); //set on world window
            QByteArray hex4 = fourtxout.toUtf8();
            int hex5 = hex4.toInt(0,16);
            int numtofloat2 = hex5 / 128.0;
            QByteArray outfloat2 = outfloat2.number(numtofloat2,10);
            ui->lineEdit_21->setText(outfloat2); //float



            int mapcoordx7 = numtofloat2; //map coords
            int mapcoordx8 = mapcoordx7 / 20;
            QString mapoutx4 = mapoutx4.number(mapcoordx8,10);
            ui->lineEdit_30->setText(mapoutx4);
            firstmapxone = mapcoordx8;



            /////////////////////////////////////////////////////////X//////////////////////////////////////////////////
            /////////////////////////////////////////////////////////Y//////////////////////////////////////////////////

            if (currentxor == 0)
            {
                QByteArray yholderzero = decopacket1.mid(14,6);
                ydeconumber1 = yholderzero.toInt(0,16);
                res2 = ydeconumber1 - 0x1C71C7;

                ystack.push(ydeconumber1);

                if (ystack.size()  >  255)
                {
                    ystack.removeAt(0);
                }


            }

            if (currentxor != 0 && currentxor <= 255)
            {
                QByteArray yholder1 = decopacket1.mid(14,6); //get Y
                ydeconumber1 = yholder1.toInt(0,16);
                int stackposy1 = currentxor;
                int stackcounty1 = ystack.size();
                //                 qDebug() << "Check-Y-Position" << stackposy1;
                //                  qDebug() << "Check-Y-Count" << stackcounty1;
                int yclientnumber1 = ystack.at(stackcounty1 - stackposy1);
                xorresult2 = ydeconumber1 ^ yclientnumber1; //xor with ringbuffer
                ystack.push(xorresult2);

                if (ystack.size()  >  255)
                {
                    ystack.removeAt(0);
                }

                //                 qDebug() << "YSTACK" <<ystack;
                res2 =  xorresult2 - 0x1C71C7;

            }



            float fourtyflo = res2 /  466.033798648;
            int helper2 = fourtyflo * 128.0;

            QString fourtyout = QString("%1").arg(helper2,6,16,QLatin1Char('0'));
            ui->lineEdit_15->setText(fourtyout.toUpper()); //3 Byte Y
            mastery = fourtyout.toUtf8();



            ui->lineEdit_19->setText(fourtyout.toUpper()); //set on world window
            QByteArray hex10 = fourtyout.toUtf8();
            int hex11 = hex10.toInt(0,16);
            int numtofloat4 = hex11 / 128.0;
            QByteArray outfloat4 = outfloat4.number(numtofloat4,10);
            ui->lineEdit_22->setText(outfloat4); //float

            int mapcoordy7 = numtofloat4; //map coords
            int mapcoordy8 = mapcoordy7 / 20;
            QString mapouty4 = mapouty4.number(mapcoordy8,10);
            ui->lineEdit_31->setText(mapouty4);
            secondmapyone = mapcoordy8;



            ////////////////////////////////////////////////////////////////////////////////Y//////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////////Z/////////////////////////////////////////////////

            if (currentxor == 0)
            {
                QByteArray zholderzero = decopacket1.mid(8,6);
                zdeconumber1 = zholderzero.toInt(0,16);
                res3 = zdeconumber1 - 0x800000;

                zstack.push(zdeconumber1);

                if (zstack.size()  >  255)
                {
                    zstack.removeAt(0);
                }

            }

            if (currentxor != 0 && currentxor <= 255)
            {
                QByteArray zholder1 = decopacket1.mid(8,6); //get Z
                zdeconumber1 = zholder1.toInt(0,16);
                int stackposz1 = currentxor;
                int stackcountz1 = zstack.size();
                //             qDebug() << "Check-Z-Position" << stackposz1;
                //                qDebug() << "Check-Z-Count" << stackcountz1;
                int zclientnumber1 = zstack.at(stackcountz1 - stackposz1);
                xorresult3 = zdeconumber1 ^ zclientnumber1; //xor with ringbuffer
                zstack.push(xorresult3);

                if (zstack.size()  >  255)
                {
                    zstack.removeAt(0);
                }

                //                     qDebug() << "ZSTACK" <<zstack;
                res3 =  xorresult3 -  0x800000;

            }



            float fourtybelow = res3 / 8388.604;
            int helper3 = fourtybelow * 128.0;



            if (helper3 < 0)
            {

                QByteArray neg1 = neg1.number(helper3,16);
                QByteArray neg2 = neg1.mid(neg1.size() - 6,6);
                ui->lineEdit_16->setText(neg2.toUpper()); //3 byte Negative Z
                masterz = neg2;

            }
            else
            {
                QString fourtzout = QString("%1").arg(helper3,6,16,QLatin1Char('0'));
                ui->lineEdit_16->setText(fourtzout.toUpper()); //3 byte Positive Z
                masterz = fourtzout.toUtf8();
            }


            /////////////////////////////////////////////////////////////////Z////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            decopacket1 = ""; //clear the packet

            ui->graphicsView->setSceneRect(firstmapxone,secondmapyone,100,100); //world map 100,100
            item1->setPos(firstmapxone,secondmapyone);
            ui->graphicsView->setScene(scene);
            ui->graphicsView->show();

        }

        carata = 0;
        caratb = 0;
        high2 = 0;
        low2 = 0;
        packetsize2 = 0;
        where2 = 0;





        //////////////////////////////4029 XYZ END///////////////////////////////////////
        ///////////////// /////////////////////////////////////////////Received Messages///////////////////////////////////////////


        mess1 = gamebuf1.toUpper();
        packetsize2 = mess1.size();

        QByteArray fnine1 = mess1; //find the f9's
        int f9place1 = mess1.indexOf("F901",0); //is there any f9 messages?
        QByteArray fnineholder1 = mess1.mid(f9place1,10);
        QByteArray marker = fnineholder1.mid(8,2);

        if (f9place1 != -1  &&  f9place1 >  30 && marker == "12")
        {

            fnine1 = mess1.mid(f9place1,10);


        }

        messint2 = mess1.indexOf("F8",32); //find F8 beyond the header

        //          qDebug() << "messint2-f8 pos" << messint2;
        int boundcheck2 = mess1.indexOf("4029"); //are we inside a 4029?
        //           qDebug() << "boundcheck2" << boundcheck2;

        if ((boundcheck2 != -1 && boundcheck2 < messint2) || messint2 == -1)
        {
            //                qDebug() << "inside 4029 in messages, or -1";
            //cant find it, is it loading packet?
            messageexist = false;
            int rangetest1 = mess1.indexOf("4029",32); //look for 4029 after the header
            int rangetest2 = mess1.indexOf("FB",32); //look for opcode after header

            if (rangetest1  == -1 && rangetest2 == -1)
            {
                //                     qDebug() << "no opcodes or 4029 or F8";

                messageexist = true;
                QByteArray  oldermessage1 = mess1.mid(32,mess1.size() - 32);
                messageholder1 = oldermessage1; //hold the message
                //                qDebug() << "oldermessage" <<oldermessage1;
                ui->textEdit_10->setText(oldermessage1);
            }

            if (rangetest1 != -1)
            {
                //is there a message between the 4029 and the message #?
                int span1 = mess1.indexOf("4029",32);
                int span2 =32;
                int span3 = span1 - span2; //subtract
                //                qDebug() <<"span3" << span3;
                if (span3 == 8)
                {
                    messageexist = true;
                    QByteArray oldermessage3 = mess1.mid(32,8);
                    messageholder1 = oldermessage3;
                    ui->textEdit_10->setText(oldermessage3);
                }



            }

            if (rangetest2 != -1) //is it a mixed message with opcode and next load?
            {
                QByteArray opcheck1 = mess1.mid(72,10); //is it a 2A00 reply?
                QByteArray mixmessage1 = mess1.mid(88,14); //look for session id
                int fbcheck = mess1.indexOf("FB"); //find FB
                int numbpos1 = 32; //find messagenumber end
                int span4 = fbcheck - numbpos1; //subract to see if there is a message
                if (span4 == 8)
                {
                    messageexist = true;
                    QByteArray oldermessage5 = mess1.mid(32,8);
                    messageholder1 = oldermessage5;
                    ui->textEdit_10->setText(oldermessage5);
                }

                if (opcheck1 == "2A00FBB012")
                {
                    QByteArray oldermessage2 = mess1.mid(32,8);
                    messageexist = true;
                    messageholder1 = oldermessage2;
                    ui->textEdit_10->setText(oldermessage2);
                }

                if (mixmessage1 == "BF543213D9BC0D")
                {
                    QByteArray oldermessage3 = mess1.mid(108,8);
                    messageexist = true;
                    messageholder1 = oldermessage3;
                    ui->textEdit_10->setText(oldermessage3);
                }
            }

        }


        else
        {
            int messwidth = messint2 - 30; //dont forget the F8 at end
            //            qDebug() << "messwidth" << messwidth;
            messext1 = mess1.mid(32,messwidth);
            //              qDebug() << "messext1" <<messext1;
            messageexist = true;
            messageholder1 = messext1;



            for (int x = 8 ; x < messext1.size() ; x+=2)
            {


                messext1.insert(x,"\r\n");
                x+=6;

            }
            serveropcode1 = mess1.mid(36,4); //saving what the client thinks the servers opcode is
            ui->lineEdit_10->setText(serveropcode1);
            ui->textEdit_10->setText(messext1);

            //////////////////////////////////////////////////////////////////////////////Ui Output///////////////////

            for (int y = 8 ; y < messageholder1.size() ; y+=2)
            {

                QByteArray channels1 = messageholder1.mid(y,2);
                int chan1 = channels1.toInt(0,16);
                //                   qDebug() << "channels" << channels1;



                if (channel_map.contains(chan1))
                {

                    QByteArray values1 = messageholder1.mid(y+2,4);
                    QString val1 = values1;
                    //                      qDebug() << "value" << values1;

                    channel_map.insert(chan1,val1);
                    //                           qDebug() << "map" << channel_map;
                }
                y+=4;

            }

            chancounter1 = 0;
            foreach (QString value, channel_map)


            {
                QTextEdit *entries[] = { ui->textEdit_12,ui->textEdit_13,ui->textEdit_14,ui->textEdit_15,ui->textEdit_16,
                                         ui->textEdit_17,ui->textEdit_18,ui->textEdit_19,ui->textEdit_20,ui->textEdit_21,
                                         ui->textEdit_22,ui->textEdit_23,ui->textEdit_24,ui->textEdit_25,ui->textEdit_26,
                                         ui->textEdit_27,ui->textEdit_28,ui->textEdit_29,ui->textEdit_30,ui->textEdit_31,
                                         ui->textEdit_32,ui->textEdit_33,ui->textEdit_34,ui->textEdit_35,ui->textEdit_36,
                                         ui->textEdit_37,ui->textEdit_38,ui->textEdit_39};


                if (chancounter1 >= 28)
                {
                    break;
                }


                entries[chancounter1]->setText(value.toUpper());
                chancounter1++;

            }
            chancounter1 = 0;
        }

        ///////////////////////////////////////////////////////////////////////F9,Channel Replies,Main Hearbeat Loop/////////////////////////////////////////////////////////////



        if (fnine1 == "FFFF2C61F111") //bad login...fix later this is just so we dont assert
        {
            flipped = false; //shut down main loop
            ui->textEdit_5->append("Bad Login--Please Restart Server--Error FFFF");
        }




        if (flipped == true) //testing this for teleport control

        {
            halfidmarker = true;
            twoninetysixmarker = true;
            QByteArray stripholder2 = fnine1.mid(0,4);
            QByteArray stripholder3 = fnine1.mid(4,4);
            //                               qDebug() << "F9999999" << fnine1;

            tunare.clear(); //clear out the tunaria.esf-to oaarmark loader..and keep it clear!


            if (stripholder2 == "F901" && stripholder3  >  "0100") //F9 Replies
            {

                QByteArray holder2591 = packet259; //lets change around packet 259
                holder2591.replace(4,4,clientid1);
                holder2591.replace(0,4,C_9007_servidcount);
                QByteArray pull5 = masterserver; //get current server message number
                QByteArray front5 = pull5.mid(0,2);
                QByteArray back5 = pull5.mid(2,2);
                QByteArray top5 = QString("%1").arg(front5.toInt(0,16) + 1,2,16, QLatin1Char( '0' )).toUtf8(); //increment
                QByteArray bottom5 = QString("%1").arg(back5.toInt(0,16),2,16,QLatin1Char('0')).toUtf8();

                if (top5 == "100")
                {
                    top5 = "00";
                    bottom5 = QString("%1").arg(back5.toInt(0,16) + 1,2,16,QLatin1Char('0')).toUtf8(); //increment
                }
                if (bottom5 == "100")
                {
                    flipped = true;
                    bottom5 = "00";
                }

                QByteArray result5 = top5 += bottom5;
                ui->lineEdit_5->setText(result5.toUpper()); //set new message number on ui
                masterserver = result5;
                holder2591.replace(30,4,result5); //set new message number
                holder2591.replace(34,4,messagenumb1); //set clients messagenumber
                holder2591.replace(38,4,stripholder3); //set number from client
                opmessagenumber2 = stripholder3; //get the opcode number
                ui->lineEdit_6->setText(opmessagenumber2); //show on ui
                byte1 = holder2591;
                elcrc(byte1);
                holder2591.append(outcrc);
                QByteArray fnineresponse = holder2591.fromHex(holder2591);
                udpsocket3.writeDatagram(fnineresponse, sender3, senderPort3);
                packetcounter1 = 0; //set to zero so main loop keeps going

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-F9-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holder2591.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-F9-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);


            }


            //                              qDebug() << "CLIENT FFFF" <<clientffcall;
            //////////////Teleport//////////////

            if (clientffcall == "FFFF9209" && teleporting == true) //FFFF reply
            {
                teleportmainswitch = false; //turn off fc02 packets
                QByteArray ffsendpacket = packetffff;
                QByteArray clientvalue1 = gamebuf1.mid(8,4); //get the count from the client
                ffsendpacket.replace(8,4,clientvalue1);
                QByteArray ffffmessage = ffsendpacket.fromHex(ffsendpacket);
                udpsocket3.writeDatagram(ffffmessage, sender3, senderPort3);

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-FFFF-Call-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(ffsendpacket.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-FFFF-Call-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);


            }

            QByteArray sleepswitch1 = gamebuf1.mid(42,4).toUpper(); //find 4900 in client packet was set to 36 other 32
            if (sleepswitch1 == "4900")
            {
                past4900 = true;
                //   ui->textEdit_4->append("turned off fc02 teleport..." + QString::fromUtf8(sleepswitch1));
            }

            if (teleportmainswitch == true && teleporting == true && past4900 == false) //4900 false
            {
                //   past4900 = true;
                //sent F607 and 9007...send fc02s on any client reply
                for (int telecounter2 = 0; telecounter2 < 3; telecounter2++)
                {
                    QByteArray fccall = letmeinagain.at(telecounter2);
                    fccall.replace(4,4,clientid1); //swap IDs
                    fccall.replace(0,4,serverid1); //also set the server id!!

                    //main message count
                    increment packetcount8; //get class
                    QByteArray newcount8 = packetcount8.count(masterserver); //will be masterserver
                    fccall.replace(30,4,newcount8); //replace new count at 30
                    masterserver = newcount8; //replace new count
                    ui->lineEdit_5->setText(newcount8.toUpper());


                    byte1 = fccall; //send array to crc
                    elcrc(byte1); //send to get a crc
                    fccall.append(outcrc);
                    QByteArray fcmessage2 = fccall.fromHex(fccall);
                    udpsocket3.writeDatagram(fcmessage2, sender3, senderPort3);

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-FC-Call-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(fccall.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-FC-Call-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);

                }
            }
            if (teleporting == true && fortyexist == true) //testing sending  00c9s on leaving teleport/coach if client asks for it
            {

                QString mainid6 = "595E0300";
                generator packet6;
                QByteArray holdergen3 = packet6.maintoonswitch(mainid6); //send the id we want to get a 00c9
                holdergen3.insert(0,packetcustom2); //add the header

                packetsize newsize6;
                QByteArray sizeresult6 = newsize6.switchsize(holdergen3); //send to get new size
                holdergen3.replace(8,4,sizeresult6);

                holdergen3.replace(4,4,clientid1);
                holdergen3.replace(0,4,serverid1);
                holdergen3.replace(34,16,combine1); //add the F8 response

                increment nother6; //get class
                QByteArray maintoonc6 = nother6.count(masterserver); //packet increment
                holdergen3.replace(30,4,maintoonc6); //replace new count at 30
                masterserver = maintoonc6.toUpper(); //replace new count
                ui->lineEdit_5->setText(maintoonc6.toUpper());

                //channel zero increment

                increment nother7; //get class
                QByteArray maintoonc8 = nother7.count(zerocounter); //zerocounter increment
                holdergen3.replace(54,4,maintoonc8); //replace new count at 54
                zerocounter = maintoonc8.toUpper(); //replace new count

                //channel zero increment
                byte1 = holdergen3;
                elcrc(byte1);
                holdergen3.append(outcrc);
                QByteArray custom3out9 = holdergen3.fromHex(holdergen3);
                udpsocket3.writeDatagram(custom3out9, sender3, senderPort3);


                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-00C9-Leaving-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holdergen3.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-00C9-Leaving-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);
            }




            ////////////////////////////Custom Packets////////////////////////////////////////////



            if (fortyexist == true)
            {                                                                            //Generator 00c9 with f8 reply

                if (teleporting == false)
                {
                    QString mainid4 = "595E0300";
                    generator packet4;
                    QByteArray holdergen = packet4.maintoonswitch(mainid4); //send the id we want to get a 00c9
                    holdergen.insert(0,packetcustom2); //add the header

                    packetsize newsize4;
                    QByteArray sizeresult = newsize4.switchsize(holdergen); //send to get new size
                    holdergen.replace(8,4,sizeresult);

                    holdergen.replace(4,4,clientid1);
                    holdergen.replace(0,4,C_9007_servidcount);
                    holdergen.replace(34,16,combine1); //add the F8 response

                    increment main_c9; //get class
                    QByteArray maintoonc1 = main_c9.count(masterserver); //packet increment
                    holdergen.replace(30,4,maintoonc1); //replace new count at 30
                    masterserver = maintoonc1.toUpper(); //replace new count
                    ui->lineEdit_5->setText(maintoonc1.toUpper());

                    //channel zero increment

                    increment main_chan0; //get class
                    QByteArray maintoonchan0 = main_chan0.count(zerocounter); //zerocounter increment
                    holdergen.replace(54,4,maintoonchan0); //replace new count at 54
                    zerocounter = maintoonchan0.toUpper(); //replace new count

                    //channel zero increment
                    byte1 = holdergen;
                    elcrc(byte1);
                    holdergen.append(outcrc);
                    QByteArray custom2out = holdergen.fromHex(holdergen);
                    udpsocket3.writeDatagram(custom2out, sender3, senderPort3);
                    packetcounter1++;

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-00C9-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(holdergen.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-00C9-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);




                    objectpacket newpacket2; //get class
                    QByteArray spawn2 = newpacket2.packetswitch(masterx,mastery,masterz); //send xyz to autospawn


                    if (spawn2 != "NULL")

                    {
                        spawn2.replace(4,4,clientid1);
                        spawn2.replace(0,4,C_9007_servidcount);

                        increment spawning_b; //get class
                        QByteArray mainspawn_b = spawning_b.count(masterserver); //packet increment
                        spawn2.replace(30,4,mainspawn_b); //replace new count at 30
                        masterserver = mainspawn_b.toUpper(); //replace new count
                        ui->lineEdit_5->setText(mainspawn_b.toUpper());

                        byte1 = spawn2;
                        elcrc(byte1);
                        spawn2.append(outcrc);
                        QByteArray custom4out = spawn2.fromHex(spawn2);
                        udpsocket3.writeDatagram(custom4out, sender3, senderPort3);
                        packetcounter1++;

                        QDateTime dateTime = dateTime.currentDateTime();
                        QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Auto-Moving-" + dateTimeString);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText(spawn2.toUpper());
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Auto-Moving-" + dateTimeString);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->moveCursor(QTextCursor::End);

                    }
                }
            }




            if (fortyexist == false)
            {
                //Normal AutoSpawn
                if (teleporting == false)
                {
                    objectpacket newpacket; //get class
                    QByteArray spawn1 = newpacket.packetswitch(masterx,mastery,masterz); //send xyz to autospawn


                    if (spawn1 != "NULL")

                    {
                        spawn1.replace(4,4,clientid1);
                        spawn1.replace(0,4,C_9007_servidcount); //adding serverid...check if its valid, if not use c_9007 value

                        increment spawning_c; //get class
                        QByteArray mainspawn_c = spawning_c.count(masterserver); //packet increment
                        spawn1.replace(30,4,mainspawn_c); //replace new count at 30
                        masterserver = mainspawn_c.toUpper(); //replace new count
                        ui->lineEdit_5->setText(mainspawn_c.toUpper());

                        byte1 = spawn1;
                        elcrc(byte1);
                        spawn1.append(outcrc);
                        QByteArray custom1out = spawn1.fromHex(spawn1);
                        udpsocket3.writeDatagram(custom1out, sender3, senderPort3);
                        packetcounter1++;

                        QDateTime dateTime = dateTime.currentDateTime();
                        QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Auto-Still-" + dateTimeString);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText(spawn1.toUpper());
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Auto-Still-" + dateTimeString);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->moveCursor(QTextCursor::End);


                    }


                    else
                    {
                        //standing still heartbeat this actually moves the mana bar slightly

                        QByteArray dummy1 = heartbeatstill;
                        dummy1.replace(4,4,clientid1);
                        dummy1.replace(0,4,C_9007_servidcount);

                        increment buffs_d; //get class
                        QByteArray dummysend_a = buffs_d.count(masterserver); //packet increment
                        dummy1.replace(30,4,dummysend_a); //replace new count at 30
                        masterserver = dummysend_a.toUpper(); //replace new count
                        ui->lineEdit_5->setText(dummysend_a.toUpper());

                        //channel 42 increment

                        increment ecc_a; //get class
                        QByteArray eccchan_e = ecc_a.count(ecchannel); //ecchannel increment
                        dummy1.replace(38,4,eccchan_e); //replace new count at 38
                        ecchannel = eccchan_e.toUpper(); //replace new count

                        //channel 42 increment
                        byte1 = dummy1;
                        elcrc(byte1);
                        dummy1.append(outcrc);
                        QByteArray custom5out = dummy1.fromHex(dummy1);
                        udpsocket3.writeDatagram(custom5out, sender3, senderPort3);
                        packetcounter1++;

                        QDateTime dateTime = dateTime.currentDateTime();
                        QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Heartbeat-" + dateTimeString);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText(dummy1.toUpper());
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Heartbeat-" + dateTimeString);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->moveCursor(QTextCursor::End);



                    } //else
                } //if fortyexist

            } //fortyexist


            //////////////////////////////////////////////////////////////////////////////////////////////// FC02D007

            if (packetcounter1 >= 5)

            {
                //Normal FC02D007
                packetcounter1 = 0; //set to zero
                QByteArray dummyfc = heartbeatfc;
                dummyfc.replace(4,4,clientid1);
                dummyfc.replace(0,4,C_9007_servidcount);

                increment fcsend_a; //get class
                QByteArray fcreply_b = fcsend_a.count(masterserver); //packet increment
                dummyfc.replace(30,4,fcreply_b); //replace new count at 30
                masterserver = fcreply_b.toUpper(); //replace new count
                ui->lineEdit_5->setText(fcreply_b.toUpper());


                byte1 = dummyfc;
                elcrc(byte1);
                dummyfc.append(outcrc);
                QByteArray custom3out = dummyfc.fromHex(dummyfc);
                udpsocket3.writeDatagram(custom3out, sender3, senderPort3);

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-FC02-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(dummyfc.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-FC02-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);

            } //if packetcounter

            //         qDebug() << "PACKETCOUNTER" << packetcounter1;

        } //if flipped



        //////////////////////////////////////////////////////////////////////Custom Packets/////////////////////////////////////////////////



        //////////////////////////////////////////////////////////////////////Responses  4029/////////////////////////////////////////////////

        if (fortyexist == true) //did we get one?...make a F8 response...!!!move combine  to the top so we get it before the packets get sent
        {

            QByteArray fortytrigger1 = fortytwentninenum1; //use our 4029 number

            ////////////////////////////////////////////////////////////////Testing//////////////////

            /////////////////////////////////////////////////////////////////////////////////

            if (fortytrigger1 == "0100")
            {
                ninecounter1++;

                if (ninecounter1 == 2)
                {
                    ninecounter1 = 0;
                    //send packet 266 267 268 - shout chat
                    QByteArray holder266 = packet266;    //FFFF930901000000
                    QByteArray ffpacket = holder266.fromHex(holder266);
                    udpsocket3.writeDatagram(ffpacket, sender3, senderPort3);

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-266-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(holder266.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-266-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);

                    QByteArray holder267 = packet267;  //fc02d007
                    holder267.replace(0,4,serverid1); //swap IDs
                    holder267.replace(4,4,clientid1); //swap IDs
                    QByteArray serverno30 = holder267.mid(30,4);
                    ui->lineEdit_5->setText(serverno30.toUpper());
                    byte1 = holder267; //send array to crc
                    elcrc(byte1); //send to get a crc
                    holder267.append(outcrc);
                    QByteArray endfcpacket = holder267.fromHex(holder267);
                    udpsocket3.writeDatagram(endfcpacket, sender3, senderPort3);

                    QDateTime dateTime2 = dateTime2.currentDateTime();
                    QString dateTimeString2 = dateTime2.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-267-" + dateTimeString2);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(holder267.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-267-" + dateTimeString2);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);

                    QByteArray holder268 = packet268;  //shout chat
                    holder268.replace(0,4,serverid1); //swap IDs
                    holder268.replace(4,4,clientid1); //swap IDs
                    QByteArray serverno31 = holder268.mid(30,4);
                    ui->lineEdit_5->setText(serverno31.toUpper());
                    byte1 = holder268; //send array to crc
                    elcrc(byte1); //send to get a crc
                    holder268.append(outcrc);
                    QByteArray shoutchat = holder268.fromHex(holder268);
                    udpsocket3.writeDatagram(shoutchat, sender3, senderPort3);
                    fortytrigger1 = ""; //clear trigger

                    QDateTime dateTime3 = dateTime3.currentDateTime();
                    QString dateTimeString3 = dateTime3.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-268-" + dateTimeString3);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(holder268.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-268-" + dateTimeString3);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);


                }

                fortytrigger1 = "";
            }






            if (fortytrigger1 == "0200")
            {
                //send 271,272,273
                QString mainid = "595E0300";
                generator packet;
                QByteArray holder271 = packet.maintoonswitch(mainid); //send the id we want to get a 00c9
                holder271.insert(0,twosevenone); //add the header
                holder271.append(twosevenoneadd); //add the buffs
                packetsize newsize;
                QByteArray sizeresult = newsize.switchsize(holder271); //send to get new size
                holder271.replace(8,4,sizeresult);
                holder271.replace(4,4,clientid1);
                QByteArray serverno11 = holder271.mid(30,4);
                ui->lineEdit_5->setText(serverno11.toUpper());

                byte1 = holder271;
                elcrc(byte1);
                holder271.append(outcrc);
                QByteArray twentyresponse1 = holder271.fromHex(holder271);
                udpsocket3.writeDatagram(twentyresponse1, sender3, senderPort3);

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-271-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holder271.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-271-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);


                QString mainid2 = "595E0300";
                generator packet2;
                QByteArray holder272 = packet2.maintoonswitch(mainid2); //send the id we want to get a 00c9
                holder272.insert(0,twoseventwo); //add the header
                packetsize newsize2;
                QByteArray sizeresult2 = newsize2.switchsize(holder272);
                holder272.replace(8,4,sizeresult2);
                holder272.replace(4,4,clientid1);
                QByteArray serverno12 = holder272.mid(30,4);
                ui->lineEdit_5->setText(serverno12.toUpper());

                byte1 = holder272;
                elcrc(byte1);
                holder272.append(outcrc);
                QByteArray twentyresponse2 = holder272.fromHex(holder272);
                udpsocket3.writeDatagram(twentyresponse2, sender3, senderPort3);

                QDateTime dateTime4 = dateTime4.currentDateTime();
                QString dateTimeString4 = dateTime4.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-272-" + dateTimeString4);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holder272.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-272-" + dateTimeString4);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);


                QByteArray holder273 = packet273; //start of d37c server id
                holder273.replace(4,4,clientid1);
                QByteArray serverno13 = holder273.mid(30,4);
                ui->lineEdit_5->setText(serverno13.toUpper());

                byte1 = holder273;
                elcrc(byte1);
                holder273.append(outcrc);
                QByteArray sessionclose1 = holder273.fromHex(holder273);
                udpsocket3.writeDatagram(sessionclose1, sender3, senderPort3);
                fortytrigger1 = "";
                loader275 = true; //set this to load 275 so it only goes off once

                QDateTime dateTime5 = dateTime5.currentDateTime();
                QString dateTimeString5 = dateTime5.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-273-" + dateTimeString5);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holder273.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-273-" + dateTimeString5);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);

            }


            fortytwentninenum1 = "";
            fortytrigger1 = "";
            fortyexist = false;


        }

        //right here, need to add a resend using each packet as a backup to resend...
        ///////////////////////////////////////////////////////////////////////////////Messages////////////////////////////////////////
        if (messageexist == true)
        {

            messageend = messageholder1;
            //            qDebug() << "end" << messageend;
            int cutsize = messageholder1.size();
            if (cutsize == 8)
            {

                tunare.append(messageholder1);
            }

            //                  qDebug() <<"TUNARE" <<tunare; //hold the messages

            if (messageend == "0100")
            {
                //get clientid and sessionid
                QByteArray clientid2 = clientid1; //get client id
                QByteArray session1 = messageid; //get session id
                QByteArray chopped1 = session1.mid(2,4);


                if (chopped1 != clientid2) //are they the same?


                {
                    //different id - packet 164 - load packet 166

                    QByteArray holder166 = packet166;
                    holder166.replace(0,4,serverid1); //swap IDs
                    holder166.replace(4,4,clientid1); //swap IDs
                    QByteArray serverno4 = holder166.mid(30,4);
                    ui->lineEdit_5->setText(serverno4.toUpper());

                    byte1 = holder166; //send array to crc
                    elcrc(byte1); //send to get a crc
                    holder166.append(outcrc);
                    QByteArray gameout4 = holder166.fromHex(holder166);
                    udpsocket3.writeDatagram(gameout4, sender3, senderPort3);

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-166-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(holder166.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-166-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);


                }
            }

            if (messageend == "3213010001000200")
            {
                QByteArray messageno2 = messagenumb1; //get messagenumber
                //                     qDebug() <<"messageno2" << messageno2;
                tunare.clear(); //clear out the tunaria.esf-to oaarmark loader

                if (messageno2 == "F146")
                {
                    //packet 168 - load character select packet 169

                    QByteArray holder169 = packet169; //(169)
                    holder169.replace(0,4,serverid1); //swap IDs
                    holder169.replace(4,4,clientid1); //swap IDs
                    QByteArray serverno5 = holder169.mid(30,4);
                    ui->lineEdit_5->setText(serverno5.toUpper());
                    byte1 = holder169; //send array to crc
                    elcrc(byte1); //send to get a crc
                    holder169.append(outcrc);
                    QByteArray gameout5 = holder169.fromHex(holder169);
                    ui->textEdit_5->append("Character Select Done...");
                    udpsocket3.writeDatagram(gameout5, sender3, senderPort3);

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-Char-Select-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(holder169.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-Char-Select-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);

                    ////////////////////////////////////////////////////////////////////////Clear Channels////////////////////////////////////////////
                    for (int y = 0 ; y <= 54; y+=2)
                    {

                        QByteArray channels3 = "000102030405060708090A0B0C0D0E0F101112131415161740414243";
                        QByteArray channelsmix3 = channels3.mid(y,2); //get channels
                        int chan3 = QString("%1").arg(channelsmix3.toInt(0,16),2,16,QLatin1Char('0')).toInt(0,16);
                        QString values3 = "0000";
                        channel_map.insert(chan3,values3); //insert them all
                    }
                    chancounter3 = 0;
                    foreach (QString value3 , channel_map)

                    {
                        QTextEdit *entries3[] = { ui->textEdit_12,ui->textEdit_13,ui->textEdit_14,ui->textEdit_15,ui->textEdit_16,
                                                  ui->textEdit_17,ui->textEdit_18,ui->textEdit_19,ui->textEdit_20,ui->textEdit_21,
                                                  ui->textEdit_22,ui->textEdit_23,ui->textEdit_24,ui->textEdit_25,ui->textEdit_26,
                                                  ui->textEdit_27,ui->textEdit_28,ui->textEdit_29,ui->textEdit_30,ui->textEdit_31,
                                                  ui->textEdit_32,ui->textEdit_33,ui->textEdit_34,ui->textEdit_35,ui->textEdit_36,
                                                  ui->textEdit_37,ui->textEdit_38,ui->textEdit_39};


                        entries3[chancounter3]->setText(value3.toUpper());
                        chancounter3++;

                    }
                    chancounter3 = 0;

                }

                if (messageno2 == "BF54")
                {
                    //packet 174 - load more pet commands - packet 175
                    QByteArray holder175 = packet175;
                    holder175.replace(0,4,serverid1); //swap IDs
                    holder175.replace(4,4,clientid1); //swap IDs
                    QByteArray serverno7 = holder175.mid(30,4);
                    ui->lineEdit_5->setText(serverno7.toUpper());
                    byte1 = holder175; //send array to crc
                    elcrc(byte1); //send to get a crc
                    holder175.append(outcrc);
                    QByteArray gameout7 = holder175.fromHex(holder175);
                    udpsocket3.writeDatagram(gameout7, sender3, senderPort3);

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-175-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(holder175.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-175-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);

                }
            }

            if (messageend == "02000300")
            {
                QByteArray messageno3 = messageid; //get session id
                QByteArray strip2 = messageno3.mid(0,8); //get session A



                if (strip2 == "BF543213")
                {
                    //packet 177 - load duel invite - packet 178
                    QByteArray holder178 = packet178;
                    holder178.replace(0,4,serverid1);
                    holder178.replace(4,4,clientid1);
                    QByteArray serverno8 = holder178.mid(30,4);
                    ui->lineEdit_5->setText(serverno8.toUpper());
                    byte1 = holder178;
                    elcrc(byte1);
                    holder178.append(outcrc);
                    QByteArray gameout8 = holder178.fromHex(holder178);
                    udpsocket3.writeDatagram(gameout8, sender3, senderPort3);

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-178-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(holder178.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-178-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);


                }

            }

            if (messageend == "1C001E00")
            {

                QByteArray holder251 = packet251;
                holder251.replace(0,4,serverid1); //swap IDs
                holder251.replace(4,4,clientid1);
                QByteArray servernoee35 = holder251.mid(30,4);
                ui->lineEdit_5->setText(servernoee35.toUpper());
                byte1 = holder251;
                elcrc(byte1);
                holder251.append(outcrc);
                QByteArray gameoutee35 = holder251.fromHex(holder251);
                udpsocket3.writeDatagram(gameoutee35, sender3, senderPort3);

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-251-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holder251.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-251-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);

            }




            if (messageend == "1D002F00")       //write ip into oaarmark gungarr
            {
                QByteArray holderpmap2 = packet254; //sending packet 254
                QByteArray holdip1 = outip;
                QByteArray first3 =  holdip1.mid(0,2);
                QByteArray second3 = holdip1.mid(2,2);
                QByteArray third3 =  holdip1.mid(4,2);
                QByteArray fourth3 = holdip1.mid(6,2);
                QByteArray bigendianip2 = fourth3+= third3 += second3+= first3;
                holderpmap2.replace(1250,8,bigendianip2);

                holderpmap2.replace(0,4,serverid1);
                holderpmap2.replace(4,4,clientid1);
                QByteArray serverno9 = holderpmap2.mid(30,4);
                ui->lineEdit_5->setText(serverno9.toUpper());
                byte1 = holderpmap2;
                elcrc(byte1);
                holderpmap2.append(outcrc);
                QByteArray gameoutmap2 = holderpmap2.fromHex(holderpmap2);
                udpsocket3.writeDatagram(gameoutmap2, sender3, senderPort3);

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-Packet-Map-End-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holderpmap2.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-Packet-Map-End-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);

            }



            if (messageend =="F901010012" )
            {

                for (counter2 = 0; counter2 < 5; counter2++)
                {
                    QByteArray holder259 = letmein.at(counter2);
                    holder259.replace(0,4,serverid1); //swap IDs
                    holder259.replace(4,4,clientid1); //swap IDs
                    QByteArray serverno10 = holder259.mid(30,4);
                    ui->lineEdit_5->setText(serverno10.toUpper());
                    byte1 = holder259; //send array to crc
                    elcrc(byte1); //send to get a crc
                    holder259.append(outcrc);
                    QByteArray fcmessages = holder259.fromHex(holder259);
                    udpsocket3.writeDatagram(fcmessages, sender3, senderPort3);

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-259-263-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(holder259.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-259-263-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);
                }
            }
            ////////////////////////////////////////////////////////////////////////////////////////////

            if (messageend == "1E004A00")
            {

                QByteArray holder257 = packet257;
                holder257.replace(0,4,serverid1); //swap IDs
                holder257.replace(4,4,clientid1);
                QByteArray serverno35 = holder257.mid(30,4);
                ui->lineEdit_5->setText(serverno35.toUpper());
                byte1 = holder257;
                elcrc(byte1);
                holder257.append(outcrc);
                QByteArray gameout35 = holder257.fromHex(holder257);
                udpsocket3.writeDatagram(gameout35, sender3, senderPort3);

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-257-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holder257.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-257-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);

            }

            /////////////////////////////////////////////////////////////Login Backup//////////////////////////////////////////////
            if (!tunare.isEmpty()) //make sure its not empty or we will assert and crash
            {
                if (earlypacket_map.contains(tunare.last())) //This is backup login for packets 181-249
                {
                    if (client_order.toUpper() == tunare.last().toUpper())
                    {
                        loginpausing.start(500); //this waits 1/2 second and compares with client order and will send if opcode side hasent
                    }
                }
            }



            ////////////////////////////////////////////////////////////////////////Channel Messages////////////////////////////////

            if (loader275 == true)
            {
                //packet 274  - load packet 275

                QString mainid3 = "595E0300";
                generator packet3;
                QByteArray holder275 = packet3.maintoonswitch(mainid3); //send the id we want to get a 00c9
                holder275.insert(0,twosevenfive); //add the header
                packetsize newsize3; //call size class
                QByteArray sizeresult3 = newsize3.switchsize(holder275); //get new size
                holder275.replace(8,4,sizeresult3); //add new size
                holder275.replace(4,4,clientid1);
                QByteArray serverno14 = holder275.mid(30,4);
                ui->lineEdit_5->setText(serverno14.toUpper());
                byte1 = holder275; //send for crc
                elcrc(byte1);
                holder275.append(outcrc);
                QByteArray dbpacketout1 = holder275.fromHex(holder275);
                udpsocket3.writeDatagram(dbpacketout1, sender3, senderPort3);
                flipped = true;
                loader275 = false; //set to false so it only goes off once

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-275-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holder275.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-275-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);
            }






            messageholder1 = ""; //clear out the message
            messageexist = false;

        }

        ////////////////////////////////////////////////////////////////////////Opcodes////////////////////////////////////
        if (opcodeexist == true)
        {
            QByteArray opcodeend = opholder; //get our opcodes

            do
            {

                //             qDebug() << "place1 - before" << place1;
                //              qDebug() << "positiona- before" << positiona;
                placeholder1 = place1 + 1;

                if (placeholder1 == 1)
                {
                    placeholder1 = 0;
                }

                place1 = opcodeend.indexOf("\n",positiona);
                QByteArray holder1 = opcodeend.mid(positiona,place1 - placeholder1);
                positiona = place1 + 1;

                //               qDebug() << "holder1" << holder1;
                //                qDebug() << "place1" << place1;
                //              qDebug() << "positiona" << positiona;

                QByteArray stripped2 = holder1.remove(0,8); //remove the FB size and message number
                opcoderesult1 = stripped2.mid(0,4); //get opcode

                //               qDebug() <<"stripped2" <<stripped2;
                //                 qDebug() <<"opcode" << opcoderesult1;

            }while (place1 != -1);


            ///////////////////////////////////////Testing Login from Opcode side/////////////////////////////////
            if (messagenumb1 <= "1D00")
            {
                QByteArray blae = messagenumb1; //use the client message so we dont skip
                QByteArray slice1 = blae.mid(0,2);
                QByteArray slice2 = blae.mid(2,2);
                int fwee = slice1.toInt(0,16);
                int grah = fwee - 0x1;
                QString newfront = QString("%1").arg(grah,2,16,QLatin1Char('0'));
                QByteArray newfront2 = newfront.toUtf8();

                QByteArray frontend = newfront2 + slice2 + blae;

                tunare.append(frontend.toUpper());
            }

            /////////////////////////////////////////////////////////


            if (opcoderesult1 == "0000")
            {

            }

            if (opcoderesult1 == "0100") //packet 163
            {
                //packet 162  - load packet 163 //add serverid now...this packet changes 5ae7 to 5be7
                QByteArray holder163 = packet163;
                holder163.replace(0,4,serverid1); //swap IDs
                holder163.replace(4,4,clientid1); //swap IDs
                holder163.replace(12,4,clientid1); //second id
                holder163.replace(22,4,clientid1); //third id
                holder163.replace(16,2,optionflag1); //# after id
                holder163.replace(26,2,optionflag1); //# after id
                QByteArray adding1 = clientid1.mid(1,1); //get char to change
                quint32 addint = adding1.toUInt(0,16); //change to int
                addint = addint + 0x1; //add one
                QByteArray addingdone = addingdone.number(addint,16);
                holder163.replace(13,1,addingdone); //replace single byte
                holder163.replace(23,1,addingdone); //replace single byte
                QByteArray serverno3 = holder163.mid(30,4);
                ui->lineEdit_5->setText(serverno3.toUpper());
                byte1 = holder163; //send array to crc
                elcrc(byte1); //send to get a crc
                holder163.append(outcrc);

                QByteArray gameout3 = holder163.fromHex(holder163);
                udpsocket3.writeDatagram(gameout3, sender3, senderPort3);

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-163-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holder163.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-163-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);

            }
            ///////////////////////////////////////////////////////////0400/3400////Coaching////////////////////////////////////

            if (opcoderesult1 == "0400")
            {
                QByteArray coach1 =  opholder.mid(12,8); //might be the right place?? lol
                coachtext1 = QString::fromUtf8(coach1);
                Coaching maincoach;
                QByteArray coach_packet = maincoach.coach(coachtext1);



                if (coach_packet != "NULL")
                {
                    coach_packet.replace(0,4,serverid1);
                    coach_packet.replace(4,4,clientid1);
                    coach_packet.replace(34,4,last_opcode_messnum);
                    coach_packet.replace(38,4,client_opcode);

                    increment coachcount1; //get class
                    QByteArray coachcount2 = coachcount1.count(masterserver); //packet increment
                    coach_packet.replace(30,4,coachcount2); //replace new count at 30
                    masterserver = coachcount2.toUpper(); //replace new count
                    ui->lineEdit_5->setText(coachcount2.toUpper());

                    int largeorsmall = coach_packet.indexOf("FBFF");
                    if (largeorsmall == -1)
                    {  //small
                        increment coachopcount1; //get class first opcode
                        QByteArray coachopcount3 = coachopcount1.count(serveropcode1); //first opcode increment
                        coach_packet.replace(46,4,coachopcount3); //replace new op # at 46
                        serveropcode1 = coachopcount3.toUpper(); //replace new  op #
                        ui->lineEdit_10->setText(coachopcount3.toUpper());

                        increment coachcount3; //get class first opcode
                        QByteArray coachcount4 = coachcount3.count(coach_counter); //first opcode increment
                        coach_packet.replace(54,4,coachcount4); //replace new count at 54
                        coach_counter = coachcount4.toUpper(); //replace new  count
                    }
                    else
                    {  //large
                        increment coachopcount2; //get class first opcode
                        QByteArray coachopcount4 = coachopcount2.count(serveropcode1); //first opcode increment
                        coach_packet.replace(50,4,coachopcount4); //replace new op # at 50
                        serveropcode1 = coachopcount4.toUpper(); //replace new  op #
                        ui->lineEdit_10->setText(coachopcount4.toUpper());

                        increment coachcount4; //get class first opcode
                        QByteArray coachcount5 = coachcount4.count(coach_counter); //first opcode increment
                        coach_packet.replace(58,4,coachcount5); //replace new count at 58
                        coach_counter = coachcount5.toUpper(); //replace new  count

                    }

                    packetsize getnewsize1; //call size class
                    QByteArray   coachnewsize = getnewsize1.switchsize(coach_packet); //get new size
                    coach_packet.replace(8,4,coachnewsize); //add new size


                    byte1 = coach_packet;
                    elcrc(byte1);
                    coach_packet.append(outcrc);
                    //                qDebug() << "coach-packet" <<coach_packet;
                    QByteArray coach_out2 = coach_packet.fromHex(coach_packet);
                    udpsocket3.writeDatagram(coach_out2, sender3, senderPort3);

                    QDateTime dateTimes_2 = dateTime.currentDateTime();
                    QString dateTimeStrings_2 = dateTimes_2.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-Coach-" + dateTimeStrings_2);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(coach_packet.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-Coach-" + dateTimeStrings_2);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);

                } //if coach isnt null

            }


            /////////////////////////////////////////////3200/CD00/CE00//////////////////////////////////////////////////////////////////////////


            if (opcoderesult1 == "3200") //button presses on the spell bar
            {
                QByteArray slot1 = opholder.mid(21,1); //slot
                QByteArray target1 = opholder.mid(22,8); //target


                if ((slot1 == "0" && slot_0_trigger == true) ||
                        (slot1 == "1" && slot_1_trigger == true) || //only send if spell has been cast
                        (slot1 == "2" && slot_2_trigger == true) ||
                        (slot1 == "3" && slot_3_trigger == true) ||
                        (slot1 == "4" && slot_4_trigger == true) ||
                        (slot1 == "5" && slot_5_trigger == true) ||
                        (slot1 == "6" && slot_6_trigger == true) ||
                        (slot1 == "7" && slot_7_trigger == true) ||
                        (slot1 == "8" && slot_8_trigger == true) ||
                        (slot1 == "9" && slot_9_trigger == true))
                {

                    //////////////////////Spell Recovery//////////////////////////////


                    QByteArray spell_recovery = "D37C5ae788c101bf543213d9bc0d032600aaaaaaaafb664b00ce00300000005200650063006F0076006500720079"
                                                "002000740069006D006500200066006F0072002000740068006100740020006100620069006C006900740079002000"
                                                "68006100730020006E006F00740020006200650065006E0020006D00650074002E00fb13d605b100595e0300595e030"
                                                "000000000004a85c79e";

                    spell_recovery.replace(0,4,serverid1);
                    spell_recovery.replace(4,4,clientid1);

                    spell_recovery.replace(34,4,last_opcode_messnum);
                    spell_recovery.replace(38,4,client_opcode);

                    increment recover1; //get class
                    QByteArray recovercount2 = recover1.count(masterserver); //packet increment
                    spell_recovery.replace(30,4,recovercount2); //replace new count at 30
                    masterserver = recovercount2.toUpper(); //replace new count
                    ui->lineEdit_5->setText(recovercount2.toUpper());

                    increment recover2; //get class first opcode
                    QByteArray recovercount3 = recover2.count(serveropcode1); //first opcode increment
                    spell_recovery.replace(46,4,recovercount3); //replace new count at 38 /46
                    serveropcode1 = recovercount3.toUpper(); //replace new count
                    ui->lineEdit_10->setText(recovercount3.toUpper());

                    increment recover3; //get class second opcode
                    QByteArray recovercount4 = recover3.count(serveropcode1); //second opcode increment
                    spell_recovery.replace(258,4,recovercount4); //replace new count at 250 / 258
                    serveropcode1 = recovercount4.toUpper(); //replace new count
                    ui->lineEdit_10->setText(recovercount4.toUpper());

                    spell_recovery.replace(274,8,target1); //replace target 274

                    byte1 = spell_recovery;
                    elcrc(byte1);
                    spell_recovery.append(outcrc);

                    QByteArray recovery_out2 = spell_recovery.fromHex(spell_recovery);
                    udpsocket3.writeDatagram(recovery_out2, sender3, senderPort3);

                    QDateTime dateTimes_2 = dateTime.currentDateTime();
                    QString dateTimeStrings_2 = dateTimes_2.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-Spell-Recovery-" + dateTimeStrings_2);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(spell_recovery.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-Spell-Recovery-" + dateTimeStrings_2);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);
                }

                //////////////////////////////////////Main Spells//////////////////////////////////////////

                cooldown_0 = false;
                cooldown_1 = false;
                cooldown_2 = false;
                cooldown_3 = false;
                cooldown_4 = false;
                cooldown_5 = false;
                cooldown_6 = false;
                cooldown_7 = false;
                cooldown_8 = false;
                cooldown_9 = false;

                if (slot1 == "0" && slot_0_trigger == false)
                {
                    spell_position =  spellslot_map.value(0);
                    spell_str1 = spellbook_map.value(spell_position);
                    spell_idout = QString("%1").arg(spell_position,2,16,QLatin1Char('0'));
                    slot_0_trigger = true;
                    cooldown_0 = true;
                    increment kick4; //get class
                    QByteArray kickcount5 = kick4.count(spell_0_counter); //spell counter
                    spell_0_counter = kickcount5.toUpper(); //replace new count
                    another_spell_count = spell_0_counter;

                }
                if (slot1 == "1" && slot_1_trigger == false)
                {
                    spell_position =  spellslot_map.value(1);
                    spell_str1 = spellbook_map.value(spell_position);
                    spell_idout = QString("%1").arg(spell_position,2,16,QLatin1Char('0'));
                    slot_1_trigger = true;
                    cooldown_1 = true;
                    increment kick4; //get class
                    QByteArray kickcount5 = kick4.count(spell_1_counter); //spell counter
                    spell_1_counter = kickcount5.toUpper(); //replace new count
                    another_spell_count = spell_1_counter;
                }
                if (slot1 == "2" && slot_2_trigger == false)
                {
                    spell_position =  spellslot_map.value(2);
                    spell_str1 = spellbook_map.value(spell_position);
                    spell_idout = QString("%1").arg(spell_position,2,16,QLatin1Char('0'));
                    slot_2_trigger = true;
                    cooldown_2 = true;
                    increment kick4; //get class
                    QByteArray kickcount5 = kick4.count(spell_2_counter); //spell counter
                    spell_2_counter = kickcount5.toUpper(); //replace new count
                    another_spell_count = spell_2_counter;
                }
                if (slot1 == "3" && slot_3_trigger == false)
                {
                    spell_position =  spellslot_map.value(3);
                    spell_str1 = spellbook_map.value(spell_position);
                    spell_idout = QString("%1").arg(spell_position,2,16,QLatin1Char('0'));
                    slot_3_trigger = true;
                    cooldown_3 = true;
                    increment kick4; //get class
                    QByteArray kickcount5 = kick4.count(spell_3_counter); //spell counter
                    spell_3_counter = kickcount5.toUpper(); //replace new count
                    another_spell_count = spell_3_counter;
                }
                if (slot1 == "4" && slot_4_trigger == false)
                {
                    spell_position =  spellslot_map.value(4);
                    spell_str1 = spellbook_map.value(spell_position);
                    spell_idout = QString("%1").arg(spell_position,2,16,QLatin1Char('0'));
                    slot_4_trigger = true;
                    cooldown_4 = true;
                    increment kick4; //get class
                    QByteArray kickcount5 = kick4.count(spell_4_counter); //spell counter
                    spell_4_counter = kickcount5.toUpper(); //replace new count
                    another_spell_count = spell_4_counter;
                }
                if (slot1 == "5" && slot_5_trigger == false)
                {
                    spell_position =  spellslot_map.value(5);
                    spell_str1 = spellbook_map.value(spell_position);
                    spell_idout = QString("%1").arg(spell_position,2,16,QLatin1Char('0'));
                    slot_5_trigger = true;
                    cooldown_5 = true;
                    increment kick4; //get class
                    QByteArray kickcount5 = kick4.count(spell_5_counter); //spell counter
                    spell_5_counter = kickcount5.toUpper(); //replace new count
                    another_spell_count = spell_5_counter;
                }

                if (slot1 == "6" && slot_6_trigger == false)
                {
                    spell_position =  spellslot_map.value(6);
                    spell_str1 = spellbook_map.value(spell_position);
                    spell_idout = QString("%1").arg(spell_position,2,16,QLatin1Char('0'));
                    slot_6_trigger = true;
                    cooldown_6 = true;
                    increment kick4; //get class
                    QByteArray kickcount5 = kick4.count(spell_6_counter); //spell counter
                    spell_6_counter = kickcount5.toUpper(); //replace new count
                    another_spell_count = spell_6_counter;
                }

                if (slot1 == "7"  && slot_7_trigger == false)
                {
                    spell_position =  spellslot_map.value(7);
                    spell_str1 = spellbook_map.value(spell_position);
                    spell_idout = QString("%1").arg(spell_position,2,16,QLatin1Char('0'));
                    slot_7_trigger = true;
                    cooldown_7 = true;
                    increment kick4; //get class
                    QByteArray kickcount5 = kick4.count(spell_7_counter); //spell counter
                    spell_7_counter = kickcount5.toUpper(); //replace new count
                    another_spell_count = spell_7_counter;
                }
                if (slot1 == "8"  && slot_8_trigger == false)
                {
                    spell_position =  spellslot_map.value(8);
                    spell_str1 = spellbook_map.value(spell_position);
                    spell_idout = QString("%1").arg(spell_position,2,16,QLatin1Char('0'));
                    slot_8_trigger = true;
                    cooldown_8 = true;
                    increment kick4; //get class
                    QByteArray kickcount5 = kick4.count(spell_8_counter); //spell counter
                    spell_8_counter = kickcount5.toUpper(); //replace new count
                    another_spell_count = spell_8_counter;
                }
                if (slot1 == "9"  && slot_9_trigger == false)
                {
                    spell_position =  spellslot_map.value(9);
                    spell_str1 = spellbook_map.value(spell_position);
                    spell_idout = QString("%1").arg(spell_position,2,16,QLatin1Char('0'));
                    slot_9_trigger = true;
                    cooldown_9 = true;
                    increment kick4; //get class
                    QByteArray kickcount5 = kick4.count(spell_9_counter); //spell counter
                    spell_9_counter = kickcount5.toUpper(); //replace new count
                    another_spell_count = spell_9_counter;
                }


                if ((slot1 == "0" && cooldown_0 == true) ||
                        (slot1 == "1" &&  cooldown_1 == true) || //only send if spell has been cast
                        (slot1 == "2" &&  cooldown_2 == true) ||
                        (slot1 == "3" && cooldown_3 ==   true) ||
                        (slot1 == "4" && cooldown_4 ==  true) ||
                        (slot1 == "5" && cooldown_5 ==   true) ||
                        (slot1 == "6" && cooldown_6 ==    true) ||
                        (slot1 == "7" && cooldown_7 ==   true) ||
                        (slot1 == "8" && cooldown_8 == true) ||
                        (slot1 == "9" && cooldown_9 ==  true))
                {

                    int currsize = spell_str1.size();
                    int timer_pos1 = spell_str1.lastIndexOf("##",-1);
                    int spell_graphic1 = spell_str1.indexOf("##",0);
                    QString spell_graph2 = spell_str1.mid(spell_graphic1 + 2, 8); //graphic
                    QString timer_times1 = spell_str1.mid(timer_pos1 + 2,currsize - timer_pos1); //time
                    int book_pos = spell_str1.indexOf("%%",0);
                    spell_str1.replace(book_pos + 2,2,QString("%1").arg(spell_position,2,16,QLatin1Char('0'))); //current book id
                    qDebug() << "Spell String" << spell_str1;
                    qDebug() << "Spell Map" << spellslot_map;
                    //    ui->textEdit_4->append(spell_str1);



                    QTimer *timer2  = new QTimer(this);
                    timer2->setInterval(timer_times1.toInt(nullptr,10));
                    timer2->setSingleShot(true);
                    timer2->setObjectName(spell_str1);
                    timers1.append(timer2);
                    timer2->start();

                    connect(timer2 ,SIGNAL (timeout()),this,SLOT(spellcooldown()));



                    QByteArray main_spell = "D67C5AE7ABC001BF543213D9BC0D032500aaaaaaaafb094C00CD000000000000D007fb139b01b100595E03005a18060000e80300002E6E280A";

                    main_spell.replace(0,4,serverid1);
                    main_spell.replace(4,4,clientid1);
                    main_spell.replace(34,4,last_opcode_messnum);
                    main_spell.replace(38,4,client_opcode);
                    main_spell.replace(106,8,spell_graph2.toUtf8()); //graphic
                    main_spell.replace(54,2,spell_idout.toUtf8()); //spell id out


                    increment kick1; //get class
                    QByteArray kickcount2 = kick1.count(masterserver); //packet increment
                    main_spell.replace(30,4,kickcount2); //replace new count at 30
                    masterserver = kickcount2.toUpper(); //replace new count
                    ui->lineEdit_5->setText(kickcount2.toUpper());

                    increment kick2; //get class first opcode
                    QByteArray kickcount3 = kick2.count(serveropcode1); //server opcode increment
                    main_spell.replace(46,4,kickcount3); //replace new count at 38/46
                    serveropcode1 = kickcount3.toUpper(); //replace new count
                    ui->lineEdit_10->setText(kickcount3.toUpper());

                    increment kick3; //get class second opcode
                    QByteArray kickcount4 = kick3.count(serveropcode1); //server opcode increment
                    main_spell.replace(72,4,kickcount4); //replace new count at 64 /72
                    serveropcode1 = kickcount4.toUpper(); //replace new count
                    ui->lineEdit_10->setText(kickcount4.toUpper());

                    main_spell.replace(56,4,another_spell_count); //replace new count at 48  56
                    main_spell.replace(88,8,target1); //replace target

                    byte1 = main_spell;
                    elcrc(byte1);
                    main_spell.append(outcrc);

                    QByteArray kickout1 = main_spell.fromHex(main_spell);
                    udpsocket3.writeDatagram(kickout1, sender3, senderPort3);

                    QDateTime dateTimes_1 = dateTime.currentDateTime();
                    QString dateTimeStrings_1 = dateTimes_1.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-Slot-Spell-" + dateTimeStrings_1);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(main_spell.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-Slot-Spell-" + dateTimeStrings_1);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);
                }

            }



            //////////////////////////////////////////////////////////3300 Spell_Bar_Memorize////////////////////////////////////////////////////////////////////////////////////////////////

            //7CF8D37C8F30D9BC0D00A504 FB081A00 3300 00000000[01 00][BCCBF90E]

            if (opcoderesult1 == "3300") //memorize spell
            {
                QByteArray Old_ID = opholder.mid(20,2);
                QByteArray New_Slot = opholder.mid(22,2);
                QByteArray Inc_Memorize = opholder.mid(12,4);

                int set_idmap1 = Old_ID.toInt(nullptr,16); //spell id
                int set_posmap1 = New_Slot.toInt(nullptr,10); //slot

                spellslot_map.insert(set_posmap1,set_idmap1);
                qDebug() << "Spell Map Changed" << spellslot_map;

                //                                         QMapIterator<int, int>  iter1 (spellslot_map); //showing on ui...remove later
                //                                                                                                 while (iter1.hasNext())
                //                                                                                                 {
                //                                                                                                     iter1.next();
                //                                                                                                     int curr_key = iter1.key();
                //                                                                                                     int curr_val = iter1.value();
                //                                                                                                     QString curr_out1 = QString::number(curr_key,10);
                //                                                                                                     QString curr_out2 = QString::number(curr_val,16);
                //                                                                                                     ui->textEdit_4->append(curr_out1 + "..." + curr_out2);
                //                                                                                                 }



                QByteArray bar_swap = "d37c5ae7a0c101bf543213d9bc0d03280022000600FB080700B400010000000205";
                bar_swap.replace(0,4,serverid1);
                bar_swap.replace(4,4,clientid1);
                bar_swap.replace(34,4,last_opcode_messnum);
                bar_swap.replace(38,4,client_opcode);
                bar_swap.replace(62,2,Old_ID);
                bar_swap.replace(64,2,New_Slot);


                packetsize ResizePacket; //call size class
                QByteArray ResizePacket2 = ResizePacket.switchsize(bar_swap); //get new size
                bar_swap.replace(8,4,ResizePacket2);

                increment bar_inc;
                QByteArray barcount2 = bar_inc.count(masterserver); //packet increment
                bar_swap.replace(30,4,barcount2); //replace new count at 30
                masterserver = barcount2.toUpper(); //replace new count
                ui->lineEdit_5->setText(barcount2.toUpper());

                increment bar_op1; //get class first opcode
                QByteArray barcount3 = bar_op1.count(serveropcode1); //first opcode increment
                bar_swap.replace(46,4,barcount3); //replace new count at 46
                serveropcode1 = barcount3.toUpper(); //replace new count
                ui->lineEdit_10->setText(barcount3.toUpper());

                QByteArray Memorize2 = bar_inc.count(Inc_Memorize); //packet increment
                bar_swap.replace(54,4,Memorize2); //replace new count at 54

                byte1 = bar_swap;
                elcrc(byte1);
                bar_swap.append(outcrc);

                QByteArray swap_spell_out2 =  bar_swap.fromHex(bar_swap);
                udpsocket3.writeDatagram(swap_spell_out2, sender3, senderPort3);

                QDateTime dateTimes_2 = dateTime.currentDateTime();
                QString dateTimeStrings_2 = dateTimes_2.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-Spell-Memorize-" + dateTimeStrings_2);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(bar_swap.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-Spell-Memorize-" + dateTimeStrings_2);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);


                spellunmemorize(); //jumping to spellunmemorize

            }




            ///////////////////////////////////////////////////3500/4600//////////////////////////////////////////////////////////////////////////////

            if (opcoderesult1 == "3500") //coach result
            {

                QByteArray client_return1 = opholder.mid(21,1); //client return number think this is where??
                int  client_return_input = client_return1.toInt(0,16);
                int dest_count = Coaching::destination.value(client_return_input);

                if (dest_count == 0 && client_return_input != 0xF)
                {

                    QByteArray coachout = Coaching::swapnames.value(dest_count); //get the number out of the map
                    QString ledger_name = Coaching::positionlist.value(coachout.toInt(0,10)); //get the number in the record
                    QString coach_name = Coaching::positionlist.value(coachout.toInt(0,10) + 1);

                    int  indexfirst1 =  ledger_name.indexOf("\"",0);
                    int indexsecond1 = ledger_name.indexOf(")",0);
                    int posi_1 = indexsecond1 - indexfirst1;
                    QString coachname_out1 = ledger_name.mid(indexfirst1 + 1,posi_1);

                    int  indexfirst2 =  coach_name.indexOf("\"",0);
                    int indexsecond2 = coach_name.indexOf(")",0);
                    int posi_2 = indexsecond2 - indexfirst2;
                    QString coachname_out2 = coach_name.mid(indexfirst2 + 1,posi_2);




                    if (db.isOpen())
                    {

                        db.transaction();

                        QString objid1 = "595E0300";
                        QString objtext = "ObjectID (4)";
                        QSqlQuery querya4;
                        querya4.prepare(QString("UPDATE COACHES SET [%1] = :val WHERE [%2] = :id").arg(coachname_out1).arg(objtext));
                        querya4.bindValue(":val",0); //this is ledger will always be zero
                        querya4.bindValue(":id",objid1);
                        querya4.exec();


                        QSqlQuery querya5;
                        querya5.prepare(QString("UPDATE COACHES SET [%1] = :val WHERE [%2] = :id").arg(coachname_out2).arg(objtext));
                        querya5.bindValue(":val",1); //this is the coach, will always be 1
                        querya5.bindValue(":id",objid1);
                        querya5.exec();

                        db.commit();
                    }

                    //send signed ledger text here

                    Coaching reply2;
                    QByteArray signed_coach1 =  reply2.leaving(coachtext1,0);

                    if (signed_coach1 != "NULL")
                    {
                        signed_coach1.replace(0,4,serverid1);
                        signed_coach1.replace(4,4,clientid1);
                        signed_coach1.replace(34,4,last_opcode_messnum);
                        signed_coach1.replace(38,4,client_opcode);

                        increment leave1; //get class
                        QByteArray leavecoach2 = leave1.count(masterserver); //packet increment
                        signed_coach1.replace(30,4,leavecoach2); //replace new count at 30
                        masterserver = leavecoach2.toUpper(); //replace new count
                        ui->lineEdit_5->setText(leavecoach2.toUpper());

                        int largeorsmall2 = signed_coach1.indexOf("FBFF");

                        if (largeorsmall2 == -1)
                        {  //small
                            increment leave2; //get class first opcode
                            QByteArray leavecoach3 = leave2.count(serveropcode1); //first opcode increment
                            signed_coach1.replace(46,4,leavecoach3); //replace new op # at 46
                            serveropcode1 = leavecoach3.toUpper(); //replace new  op #
                            ui->lineEdit_10->setText(leavecoach3.toUpper());

                            increment leave3; //get class first opcode
                            QByteArray leavecoach4 = leave3.count(coachreply_counter); //first opcode increment
                            signed_coach1.replace(54,4,leavecoach4); //replace new count at 54
                            coachreply_counter = leavecoach4.toUpper(); //replace new  count
                        }

                        else
                        {  //large
                            increment leave4; //get class first opcode
                            QByteArray leavecoach5 = leave4.count(serveropcode1); //first opcode increment
                            signed_coach1.replace(50,4,leavecoach5); //replace new op # at 50
                            serveropcode1 = leavecoach5.toUpper(); //replace new  op #
                            ui->lineEdit_10->setText(leavecoach5.toUpper());

                            signed_coach1.replace(58,4,coach_counter); //replace new count at 58 same num as 3400

                        }
                        packetsize getnewsize2; //call size class
                        QByteArray coachnewsize2 = getnewsize2.switchsize(signed_coach1); //get new size
                        signed_coach1.replace(8,4,coachnewsize2); //add new size


                        byte1 = signed_coach1;
                        elcrc(byte1);
                        signed_coach1.append(outcrc);
                        qDebug() << "coach-packet" << signed_coach1;
                        QByteArray coach_out3 = signed_coach1.fromHex(signed_coach1);
                        udpsocket3.writeDatagram(coach_out3, sender3, senderPort3);

                        QDateTime dateTimes_3 = dateTime.currentDateTime();
                        QString dateTimeStrings_3 = dateTimes_3.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Coach-End-" + dateTimeStrings_3);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText(signed_coach1.toUpper());
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Coach-End-" + dateTimeStrings_3);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->moveCursor(QTextCursor::End);
                        coachtext1 = ""; //clear the id result out
                    }
                }

                if (dest_count != 0 && client_return_input != 0xF)
                {

                    Coaching reply3;
                    QByteArray signed_coach2 =  reply3.leaving(coachtext1,5);

                    if (signed_coach2 != "NULL")
                    {
                        signed_coach2.replace(0,4,serverid1);
                        signed_coach2.replace(4,4,clientid1);
                        signed_coach2.replace(34,4,last_opcode_messnum);
                        signed_coach2.replace(38,4,client_opcode);

                        increment leave6; //get class
                        QByteArray leavecoach8 = leave6.count(masterserver); //packet increment
                        signed_coach2.replace(30,4,leavecoach8); //replace new count at 30
                        masterserver = leavecoach8.toUpper(); //replace new count
                        ui->lineEdit_5->setText(leavecoach8.toUpper());

                        int largeorsmall3 = signed_coach2.indexOf("FBFF");

                        if (largeorsmall3 == -1)
                        {  //small
                            increment leave7; //get class first opcode
                            QByteArray leavecoach9 = leave7.count(serveropcode1); //first opcode increment
                            signed_coach2.replace(46,4,leavecoach9); //replace new op # at 46
                            serveropcode1 = leavecoach9.toUpper(); //replace new  op #
                            ui->lineEdit_10->setText(leavecoach9.toUpper());

                            increment leave8; //get class first opcode
                            QByteArray leavecoach10 = leave8.count(coachreply_counter); //first opcode increment
                            signed_coach2.replace(54,4,leavecoach10); //replace new count at 54
                            coachreply_counter = leavecoach10.toUpper(); //replace new  count
                        }
                        else
                        {  //large
                            increment leave9; //get class first opcode
                            QByteArray leavecoach11 = leave9.count(serveropcode1); //first opcode increment
                            signed_coach2.replace(50,4,leavecoach11); //replace new op # at 50
                            serveropcode1 = leavecoach11.toUpper(); //replace new  op #
                            ui->lineEdit_10->setText(leavecoach11.toUpper());

                            signed_coach2.replace(58,4,coach_counter); //replace new count at 58

                        }

                        packetsize getnewsize3; //call size class
                        QByteArray coachnewsize3 = getnewsize3.switchsize(signed_coach2); //get new size
                        signed_coach2.replace(8,4,coachnewsize3); //add new size


                        byte1 = signed_coach2;
                        elcrc(byte1);
                        signed_coach2.append(outcrc);
                        qDebug() << "coach-packet" << signed_coach2;
                        QByteArray coach_out4 = signed_coach2.fromHex(signed_coach2);
                        udpsocket3.writeDatagram(coach_out4, sender3, senderPort3);

                        QDateTime dateTimes_4 = dateTime.currentDateTime();
                        QString dateTimeStrings_4 = dateTimes_4.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Coach-End-" + dateTimeStrings_4);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText(signed_coach2.toUpper());
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Coach-End-" + dateTimeStrings_4);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->moveCursor(QTextCursor::End);
                        coachtext1 = ""; //clear the id result out


                        QByteArray teleportcoachout2 = Coaching::swapnames.value(dest_count);
                        QByteArray teleportcoach_packet = coach_destination_map.value(teleportcoachout2.toInt(0,10));
                        int oggokport1 = teleportcoachout2.toInt(0,10);

                        if (oggokport1 == 52)
                        {
                            masterworld = 01;
                        }
                        else
                        {
                            masterworld = 00;
                        }
                        teleport_destination = teleportcoach_packet;
                        coachtimer.start(1000); //1 second timer for end packet

                    }
                }
            }

            ///////////////////////////////////////////////////3A00 Spell Book Moves//////////////////////////////////////////////////////////////////////////////////
            if(opcoderesult1 == "3A00")
            {
                //FB08 2900 3A00 00000000 0305
                QByteArray Spell_arr = opholder.mid(20,2);
                QByteArray New_Slot = opholder.mid(22,2);
                //    ui->textEdit_4->append(Spell_arr + "...id...slot..." + New_Slot);
                spell_id = Spell_arr.toInt(nullptr,16);
                spell_bookslot = New_Slot.toInt(nullptr,16);
                ////////////////////////////////////SLOT //////////////////////////////////////////
                for(int yy = 0; yy < spellslot_map.size(); yy++) //create history of which spell is currently in which slot do this before sorting
                {
                    int temp_spell1 = spellslot_map.value(yy); //get spell ids
                    QString new_spell_res = spellbook_map.value(temp_spell1); //get the new id (position)
                    spellslot_copy.insert(yy,new_spell_res); //this is slot, spell string

                }
                qDebug() << "spell slot new" << spellslot_map;
                qDebug() << "spell slot copy" << spellslot_copy;
                ////////////////////////////////////////////Arranging

                int result_sp = spell_id - spell_bookslot; //sub

                if (result_sp < 0) //going down
                {

                    QString row1 = spellbook_map.value(spell_id); //get spell at position
                    qDebug() << "row1" <<row1;
                    spellbook_copy.insert(spell_bookslot,row1);

                    for (fee2 = spell_bookslot; fee2 > -1; fee2--)
                    { ///
                        if (fee2 == spell_id)
                        {

                            QString row5 = spellbook_map.value(fee2 - 1);
                            qDebug() << "row5" <<row5;
                            switch2 = true;
                            spellbook_copy.insert(fee2 - 1,row5);
                            fee2--;

                        }
                        if (switch2 == true)
                        {
                            QString row1 = spellbook_map.value(fee2);
                            spellbook_copy.insert(fee2,row1);
                        }
                        else
                        {

                            QString row1 = spellbook_map.value(fee2);
                            spellbook_copy.insert(fee2 - 1,row1);
                        }

                    } ///
                    switch2 = false;

                    for(int xx = spell_bookslot + 1; xx < spellbook_map.size(); xx++) //complete the map copy
                    {
                        QString frr = spellbook_map.value(xx);
                        spellbook_copy.insert(xx,frr);
                    }
                    spellbook_map.clear();
                    spellbook_map = spellbook_copy; //transfer our new map
                    spellbook_copy.clear(); //clear this we are done with it
                }
                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                else
                {
                    //     ui->textEdit_4->append("going up"); //going up

                    for (fee = spell_id; fee > -1; fee--)
                    {

                        if (fee == spell_bookslot)
                        {
                            QString row1 = spellbook_map.value(spell_id);
                            qDebug() << "row1" <<row1;
                            spellbook_copy.insert(fee,row1);
                            switch1 = true;
                            QString row2 = spellbook_map.value(spell_bookslot);
                            qDebug() << "row2" << row2;
                            spellbook_copy.insert(fee - 1,row2);

                        }
                        else
                        {   ///
                            if (switch1 == true)
                            {
                                QString row1 = spellbook_map.value(fee);
                                spellbook_copy.insert(fee,row1);
                            }
                            else
                            {

                                QString row1 = spellbook_map.value(fee - 1);
                                spellbook_copy.insert(fee,row1);
                            }

                        }   ///
                    }
                    switch1 = false;

                    for(int xx = spell_id + 1; xx < spellbook_map.size(); xx++) //complete the map copy
                    {
                        QString frr = spellbook_map.value(xx);
                        spellbook_copy.insert(xx,frr);
                    }
                    spellbook_map.clear();
                    spellbook_map = spellbook_copy; //transfer our new map
                    spellbook_copy.clear(); //clear this we are done with it
                }

                //just to show
                //                                                      QMapIterator<int, QString>  iter1 (spellbook_map); //showing on ui
                //                                                                                                              while (iter1.hasNext())
                //                                                                                                              {
                //                                                                                                                  iter1.next();
                //                                                                                                                  int curr_key = iter1.key();
                //                                                                                                                  QString curr_out1 = iter1.value();

                //                                                                                                                  QString curr_out2 = QString::number(curr_key,16);
                //                                                                                                                  ui->textEdit_4->append(curr_out2 + "..." + curr_out1);
                //                                                                                                              }

                ///////////////////////////////////////////Slot ///////////////////////////////////////
                for (int zz = 0; zz < spellslot_copy.size(); zz++)
                {
                    QString new_spell_res2 = spellslot_copy.value(zz); //get spell description
                    int new_val_spell = spellbook_map.key(new_spell_res2); //get the new position since we rearranged the book ids
                    spellslot_map.insert(zz,new_val_spell); //insert new spell id for the spell description
                }
                spellslot_copy.clear();



                qDebug() << "Spellbook Map" << spellbook_map;


                QByteArray Inc_Arrange = opholder.mid(12,4);
                QByteArray Arrange_Spells = "d37c5ae7a0c101bf543213d9bc0d03280022000600FB080700B600010000000102";
                packetsize ResizePacket; //call size class
                QByteArray ResizePacket2 = ResizePacket.switchsize(Arrange_Spells); //get new size

                qDebug() << "Client Book Move" << opholder;
                Arrange_Spells.replace(8,4,ResizePacket2);
                Arrange_Spells.replace(0,4,serverid1);
                Arrange_Spells.replace(4,4,clientid1);
                Arrange_Spells.replace(34,4,last_opcode_messnum);
                Arrange_Spells.replace(38,4,client_opcode);
                Arrange_Spells.replace(62,2,Spell_arr);
                Arrange_Spells.replace(64,2,New_Slot);
                //get class
                increment Arrange;
                QByteArray Arrange2 = Arrange.count(masterserver); //packet increment //packet increment
                Arrange_Spells.replace(30,4,Arrange2); //replace new count at 30
                masterserver = Arrange2.toUpper(); //replace new count
                ui->lineEdit_5->setText(Arrange2.toUpper());

                Arrange2 = Arrange.count(serveropcode1); //first opcode increment
                Arrange_Spells.replace(46,4,Arrange2); //replace new count at 46
                serveropcode1 = Arrange2.toUpper(); //replace new count
                ui->lineEdit_10->setText(Arrange2.toUpper());

                Arrange2 = Arrange.count(Inc_Arrange); //packet increment
                Arrange_Spells.replace(54,4,Arrange2); //replace new count at 54

                byte1 = Arrange_Spells;
                elcrc(byte1);
                Arrange_Spells.append(outcrc);

                QByteArray Arrange_out = Arrange_Spells.fromHex(Arrange_Spells);
                udpsocket3.writeDatagram(Arrange_out, sender3, senderPort3);

                QDateTime dateTimes_2 = dateTime.currentDateTime();
                QString dateTimeStrings_2 = dateTimes_2.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-Arrange_Spells-" + dateTimeStrings_2);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(Arrange_Spells.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-Arrange_Spells-" + dateTimeStrings_2);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);

                spellunmemorize(); //calling unmemorize
            }


            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            if (opcoderesult1 == "4900" && teleporting == true && yourmom == true)
            {
                qDebug() << "X-Stack-Teleport" << xstack;
                /////////////////////////////////////////////////////Stack Fix/////////////////////////////////////////////////////////////////////////////
                QByteArray didwedrop = gamebuf1.mid(36,4).toUpper(); //find the client opcode

                if (dropped1 ==  didwedrop)
                {
                    dropped1 = ""; //set it back
                    //     ui->textEdit_4->append("C700-dropped or doubled" + QString::fromUtf8(dropped1) +"..." + didwedrop);

                    int newstackcountx = xstack.size();
                    int laststackx = xstack.at(newstackcountx - 2); //-1 for last 4029

                    xstack.push(laststackx);


                    if (xstack.size()  >  255)
                    {
                        xstack.removeAt(0);
                    }

                    int newstackcountz = zstack.size();
                    int laststackz = zstack.at(newstackcountz - 2);

                    zstack.push(laststackz);


                    if (zstack.size()  >  255)
                    {
                        zstack.removeAt(0);
                    }
                    int newstackcounty = ystack.size();
                    int laststacky = ystack.at(newstackcounty - 2);

                    ystack.push(laststacky);


                    if (ystack.size()  >  255)
                    {
                        ystack.removeAt(0);
                    }
                }
                qDebug() << "X-Stack-Teleport-After-"<< xstack;

                dropped1 = gamebuf1.mid(36,4).toUpper(); //find client opcode, keep til the next go around
                qDebug() << "Dropped" << dropped1 +"..." + didwedrop;
                ///////////////////////////////////////////////////////////////C700/4900///////////////////////////////////////////////////////////

                QByteArray sevenhund = "d37c5ae792c001bf543213d9bc0d13260020000500400100f8fb034c00c70000"; //(c700)
                sevenhund.replace(4,4,clientid1); //replace client id
                sevenhund.replace(0,4,C_9007_servidcount); //replace server id
                //also replace serverid cause it will change now!!!

                //packet increment

                increment C700_Inc; //get class
                QByteArray count_c700 = C700_Inc.count(masterserver); //packet increment
                sevenhund.replace(30,4,count_c700); //replace new count at 30
                masterserver = count_c700.toUpper(); //replace new count
                ui->lineEdit_5->setText(count_c700.toUpper());

                //opcode increment
                increment C700_Packet; //get class
                QByteArray countop_c700 = C700_Packet.count(serveropcode1); //server opcode increment
                sevenhund.replace(54,4,countop_c700); //replace new count at 54
                serveropcode1 = countop_c700.toUpper(); //replace new count
                ui->lineEdit_10->setText(countop_c700.toUpper());

                //F8 Message
                sevenhund.replace(34,16,combine1); //set new f8 reply

                byte1 = sevenhund;
                elcrc(byte1);
                sevenhund.append(outcrc);
                QByteArray sevenout = sevenhund.fromHex(sevenhund);


                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-C700-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(sevenhund.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-C700-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);
                udpsocket3.writeDatagram(sevenout, sender3, senderPort3);


                qDebug() <<"Stack X Before- X" << masterx;
                qDebug() <<"Stack Z Before- Z" << masterz;
                qDebug() <<"Stack Y Before- Y" << mastery;
                QString masterxhold1 = QString::fromUtf8(masterx);
                QString masterzhold1 = QString::fromUtf8(masterz);
                QString masteryhold1 = QString::fromUtf8(mastery);

                ui->textEdit_5->append(QString("Teleport-4900-X-%1-Z-%2-Y-%3").arg(masterxhold1).arg(masterzhold1).arg(masteryhold1));


            }

            /////////////////////////////////////////////////////4C00 Merchants/////////////////////////////////////////////////////////////////////
            if (opcoderesult1 == "4C00")
            {
                int merch_index = opholder.size();
                QByteArray new_merch_id = gamebuf1.mid(merch_index - 8,8); //get last 8 pretty sure crc is gone
                merchants sending1;
                QMap <int,QByteArray> merch_output_map = sending1.mer_packet(new_merch_id);
                QByteArray no_match = merch_output_map.value(0).toUpper();
                if (no_match == "NULL")
                {
                    break; //break out and do nothing if there is no data
                }


                for (int re = 0; re < merch_output_map.size(); re++)
                {
                    QByteArray work_merch1 = merch_output_map.value(re).toUpper(); //change to upper to find correct indexes
                    int fb_position = work_merch1.indexOf("FB");
                    int fa_position = work_merch1.indexOf("FA");
                    qDebug() << "work" << work_merch1;
                    qDebug() << "FA" << fa_position;
                    qDebug() << "FB" <<fb_position;

                    if (fa_position != -1)
                    {
                        increment merch_inc1; //get class
                        QByteArray count_merch1 = merch_inc1.count(masterserver); //packet increment
                        work_merch1.replace(30,4,count_merch1); //replace new count at 30
                        masterserver = count_merch1.toUpper(); //replace new count
                        ui->lineEdit_5->setText(count_merch1.toUpper());

                        increment merch_opc; //get class
                        QByteArray merch_serv1 = merch_opc.count(serveropcode1); //server opcode increment
                        work_merch1.replace(fa_position + 8,4,merch_serv1); //replace new count at fa + 8
                        serveropcode1 = merch_serv1.toUpper(); //replace new count
                        ui->lineEdit_10->setText(merch_serv1.toUpper());

                        byte1 = work_merch1;
                        elcrc(byte1);
                        work_merch1.append(outcrc);
                        QByteArray merch_out = work_merch1.fromHex(work_merch1);

                        QDateTime dateTime = dateTime.currentDateTime();
                        QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Merchant-" + dateTimeString);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText(work_merch1.toUpper());
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->appendPlainText("Server-Merchant-" + dateTimeString);
                        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                        ui->plainTextEdit->moveCursor(QTextCursor::End);
                        udpsocket3.writeDatagram(merch_out, sender3, senderPort3);

                    }
                    if (fb_position != -1)
                    {
                        QByteArray find_ff = work_merch1.mid(fb_position + 2,2).toUpper();
                        qDebug() << "find ff" << find_ff;
                        if (find_ff == "FF") //ff large header
                        {
                            increment merch_inc1; //get class
                            QByteArray count_merch1 = merch_inc1.count(masterserver); //packet increment
                            work_merch1.replace(30,4,count_merch1); //replace new count at 30
                            masterserver = count_merch1.toUpper(); //replace new count
                            ui->lineEdit_5->setText(count_merch1.toUpper());

                            increment merch_opc; //get class
                            QByteArray merch_serv1 = merch_opc.count(serveropcode1); //server opcode increment
                            work_merch1.replace(fb_position + 8,4,merch_serv1); //replace new count at fb + 8
                            serveropcode1 = merch_serv1.toUpper(); //replace new count
                            ui->lineEdit_10->setText(merch_serv1.toUpper());

                            byte1 = work_merch1;
                            elcrc(byte1);
                            work_merch1.append(outcrc);
                            QByteArray merch_out = work_merch1.fromHex(work_merch1);

                            QDateTime dateTime = dateTime.currentDateTime();
                            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->appendPlainText("Server-Merchant-" + dateTimeString);
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->appendPlainText(work_merch1.toUpper());
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->appendPlainText("Server-Merchant-" + dateTimeString);
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->moveCursor(QTextCursor::End);
                            udpsocket3.writeDatagram(merch_out, sender3, senderPort3);


                        }
                        else
                        { //small header
                            increment merch_inc1; //get class
                            QByteArray count_merch1 = merch_inc1.count(masterserver); //packet increment
                            work_merch1.replace(30,4,count_merch1); //replace new count at 30
                            masterserver = count_merch1.toUpper(); //replace new count
                            ui->lineEdit_5->setText(count_merch1.toUpper());

                            increment merch_opc; //get class
                            QByteArray merch_serv1 = merch_opc.count(serveropcode1); //server opcode increment
                            work_merch1.replace(fb_position + 4,4,merch_serv1); //replace new count at fb + 4
                            serveropcode1 = merch_serv1.toUpper(); //replace new count
                            ui->lineEdit_10->setText(merch_serv1.toUpper());

                            byte1 = work_merch1;
                            elcrc(byte1);
                            work_merch1.append(outcrc);
                            QByteArray merch_out = work_merch1.fromHex(work_merch1);

                            QDateTime dateTime = dateTime.currentDateTime();
                            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->appendPlainText("Server-Merchant-" + dateTimeString);
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->appendPlainText(work_merch1.toUpper());
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->appendPlainText("Server-Merchant-" + dateTimeString);
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->moveCursor(QTextCursor::End);
                            udpsocket3.writeDatagram(merch_out, sender3, senderPort3);
                        }
                    }
                }
            }


            ////////////////////////////////////////////6007/6107//////////////////////////////////////////////////////////////////////////////////////
            if (opcoderesult1 == "6007")
            {
                QByteArray Target_ID = opholder.mid(20,8);
                QString Level_00 = "Level (1)";
                //                                  QString table1_00 = "MAINTOON";
                QString table1_01 = "NPCS";
                QString table1_02 = "FACTION";
                QString table1_03 = "FACTION_ID";
                QString objectid2 = "ObjectID (4)";
                //                                  QString maintarget = "595E0300";
                //                                  QString id2 =  QString::fromUtf8(Target_ID);
                QString npc_id =   "NPC_ID";
                QString othernpcid = "Npc ID (4)";
                QString factionname = "FACTION_NAME";

                targeting changecolor1;
                QString returnedcolor = changecolor1.target(Target_ID);
                Con_Color_V = returnedcolor.toInt(nullptr,16);


                if(Target_ID == "595E0300")// if self target
                {

                    Face_Color_V = 04; //blue?
                    holder_Con = Self_Packet; //send self packet
                }


                if(Target_ID != "595E0300")// if not self target
                {


                    //npc
                    QSqlQuery query_02;
                    query_02.prepare(QString("SELECT [%1],[%2] FROM %3 WHERE [%4] = :val2").arg(Level_00).arg(othernpcid).arg(table1_01).arg(objectid2));
                    query_02.bindValue(":val2",Target_ID);
                    query_02.exec();
                    query_02.next();



                    QString npc_value = query_02.value(1).toString();
                    //factionid
                    QSqlQuery query_03;
                    query_03.prepare(QString("SELECT [%1] FROM %2 WHERE [%3] = :val4").arg(factionname).arg(table1_03).arg(npc_id));
                    query_03.bindValue(":val4",npc_value); //select faction name from faction id where npcid = the result from last query
                    query_03.exec();
                    query_03.next();

                    QString new_faction_name = query_03.value(0).toString();
                    //faction
                    QSqlQuery query_04;
                    query_04.prepare(QString("SELECT [%1] FROM %2").arg(new_faction_name).arg(table1_02));
                    query_04.exec();
                    query_04.next();

                    int factionnumber1 = query_04.value(0).toInt();

                    if(factionnumber1 <= 9800)
                    {

                        Face_Color_V = 00;
                    }
                    if (factionnumber1 >= 9801 && factionnumber1 <= 10199)
                    {

                        Face_Color_V = 02;
                    }
                    if(factionnumber1 >= 10199)
                    {

                        Face_Color_V = 04;
                    }


                    db.commit();
                }
                holder_Con = Con_Packet; //send the con packet


                QString facecolor1 =  QString("%1").arg(Face_Color_V,2,10,QLatin1Char('0'));
                QString concolor1 = QString("%1").arg(Con_Color_V,2,10,QLatin1Char('0'));


                holder_Con.replace(4,4,clientid1);
                holder_Con.replace(0,4,serverid1);

                holder_Con.replace(50,2,facecolor1.toUtf8());
                holder_Con.replace(52,2,concolor1.toUtf8());

                QByteArray Target_Counter_01 = opholder.mid(12,8);
                holder_Con.replace(568,8,Target_Counter_01); //set new target amount number

                //main count
                increment packetcount7; //get class
                QByteArray count_con1 = packetcount7.count(masterserver); //packet increment
                holder_Con.replace(30,4,count_con1); //replace new count at 30
                masterserver = count_con1.toUpper(); //replace new count
                ui->lineEdit_5->setText(count_con1.toUpper());

                //opcode count
                increment packetcount6; //get class
                QByteArray newcount6 = packetcount6.count(serveropcode1); //will be server opcode
                holder_Con.replace(42,4,newcount6); //replace new opcount at 42
                serveropcode1 = newcount6.toUpper(); //replace new count
                ui->lineEdit_10->setText(newcount6.toUpper());

                byte1 = holder_Con;
                elcrc(byte1);
                holder_Con.append(outcrc);
                QByteArray Target_Con_out1 = holder_Con.fromHex(holder_Con);
                udpsocket3.writeDatagram(Target_Con_out1, sender3, senderPort3);

                QDateTime dateTime_c1 = dateTime.currentDateTime();
                QString dateTimeString_c1 = dateTime_c1.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-6107-" + dateTimeString_c1);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holder_Con.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-6107-" + dateTimeString_c1);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);

                mylevel = 0;
                moblevel  = 0;
                difference1  = 0;
                difference4 = 0;
                Con_Color_V = 0;
                Face_Color_V = 0;
                holder_Con.clear();
            }



            ///////////////////////////////////////////////////9107/9007/////////////////////////////////////////////////////////////////




            if (opcoderesult1 == "9107" && teleporting == true)
            {
                qDebug() << "9107 Sent";

                QByteArray closer9007 = "d47c5ae7a1c001bf543213d9bc0d039e0099000c00fb16fc009007ffffffffffffffff040000005ae73b1d69776218";

                closer9007.replace(4,4,clientid1);
                closer9007.replace(0,4,C_9007_servidcount); //set new server id from original 9007
                ui->lineEdit_3->setText(C_9007_servidcount); //show new id on ui
                //packet number
                increment packetcount5; //get class
                QByteArray newcount5 = packetcount5.count(masterserver); //will be masterserver
                closer9007.replace(30,4,newcount5); //replace new count at 30
                masterserver = newcount5.toUpper(); //replace new count
                ui->lineEdit_5->setText(newcount5.toUpper());

                //client values
                QByteArray clientno1 = messagenumb1;
                closer9007.replace(34,4,clientno1); //insert client message number
                QByteArray clientno2 = client_opcode;
                closer9007.replace(38,4,clientno2); //insert client opcode

                //opcode count
                increment packetcount6; //get class
                QByteArray newcount6 = packetcount6.count(serveropcode1); //will be server opcode
                closer9007.replace(46,4,newcount6); //replace new opcount at 46
                serveropcode1 = newcount6.toUpper(); //replace new count
                ui->lineEdit_10->setText(newcount6.toUpper());


                //end of packet count this should always be one more than the first sent 9007 after the teleport packet
                increment packetcount7; //get class
                QByteArray newcount7 = packetcount7.count(C_9007_endcount); //use old count from first 9007
                closer9007.replace(70,4,newcount7); //replace new end count at 70

                byte1 = closer9007;
                elcrc(byte1);
                closer9007.append(outcrc);
                QByteArray finishport = closer9007.fromHex(closer9007);

                QDateTime dateTimex1 = dateTime.currentDateTime();
                QString dateTimeStringx1 = dateTimex1.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-9007-Closer-" + dateTimeStringx1);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(closer9007.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-9007-Closer-" + dateTimeStringx1);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);
                udpsocket3.writeDatagram(finishport, sender3, senderPort3);


                timer.start(50); //lets try 1/50th second after in case we get more 9107s from the client

            }

            /////////////////////////////////////////B009 Disconnect//////////////////////////////////////////////////////////////


            if (opcoderesult1 == "B009") // disconnect thru in game menu
            {
                if (db.isOpen())
                {
                    /////////////////////////Save Location///////////////////////////

                    QString tablesel3 = "MAINTOON";
                    QString objecttlpt3 = "ObjectID (4)";
                    QString xtlpt3 = "X (3)";
                    QString ytlpt3 = "Y (3)";
                    QString ztlpt3 = "Z (3)";
                    //no world because the user would either set it in the db, or it will get set by teleporting
                    QString tlptid3 = "595E0300";
                    QString x13 = QString::fromUtf8(masterx);
                    QString y13 = QString::fromUtf8(mastery);
                    QString z13 = QString::fromUtf8(masterz);
                    db.transaction();

                    QSqlQuery querytlpt3;
                    querytlpt3.prepare(QStringLiteral("UPDATE %1 SET [%2] = :val2,[%3] = :val3,[%4] = :val4 WHERE [%5] = :val").arg(tablesel3).arg(xtlpt3).arg(ytlpt3).arg(ztlpt3).arg(objecttlpt3));

                    querytlpt3.bindValue(":val",tlptid3);
                    querytlpt3.bindValue(":val2",x13.toUpper());
                    querytlpt3.bindValue(":val3",y13.toUpper());
                    querytlpt3.bindValue(":val4",z13.toUpper());

                    querytlpt3.exec();
                    db.commit();

                }

            }
            /////////////////////////////////////////FE00 Unmemorize Spells//////////////////////////////////////////////////////////////////////////
            if (opcoderesult1 == "FE00")     //FB060F00FE0002000000

            {

                unmemorize = true; //set this bool so we can trick ungrey to ungrey all of our slots



                QByteArray Current_Slot = opholder.mid(12,8);
                QByteArray UnMemorize_All = "d37c5ae7a0c101bf543213d9bc0d03280022000600FB060700FF0001000000";

                packetsize ResizePacket; //call size class
                QByteArray ResizePacket2 = ResizePacket.switchsize(UnMemorize_All); //get new size
                UnMemorize_All.replace(8,4,ResizePacket2);

                UnMemorize_All.replace(0,4,serverid1);
                UnMemorize_All.replace(4,4,clientid1);
                UnMemorize_All.replace(34,4,last_opcode_messnum);
                UnMemorize_All.replace(38,4,client_opcode);
                UnMemorize_All.replace(54,8,Current_Slot);

                increment UnMemorize;
                QByteArray UnMemorize2 = UnMemorize.count(masterserver); //packet increment //packet increment
                UnMemorize_All.replace(30,4,UnMemorize2); //replace new count at 30
                masterserver = UnMemorize2.toUpper(); //replace new count
                ui->lineEdit_5->setText(UnMemorize2.toUpper());

                UnMemorize2 = UnMemorize.count(serveropcode1); //first opcode increment
                UnMemorize_All.replace(46,4,UnMemorize2); //replace new count at 46
                serveropcode1 = UnMemorize2.toUpper(); //replace new count
                ui->lineEdit_10->setText(UnMemorize2.toUpper());

                byte1 = UnMemorize_All;
                elcrc(byte1);
                UnMemorize_All.append(outcrc);

                QByteArray UnMemorize_out = UnMemorize_All.fromHex(UnMemorize_All);
                udpsocket3.writeDatagram(UnMemorize_out, sender3, senderPort3);

                QDateTime dateTimes_2 = dateTime.currentDateTime();
                QString dateTimeStrings_2 = dateTimes_2.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-UnMemorize_All-" + dateTimeStrings_2);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(UnMemorize_All.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-UnMemorize_All-" + dateTimeStrings_2);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);

                spellunmemorize();//jump to spellcooldown

            }


            ///////////////////////////////////////////////////////////Packet 173//////////////////////////////////////////////////////////////



            if (opcoderesult1 == "2A00")
            {

                QByteArray  messnumb5  = messagenumb1; //client message number
                QByteArray sessiona1 = messageid;

                if (messnumb5 == "0300"  && sessiona1 == "F1463213D8BC0D20")
                {
                    //packet 170 - load tunaria.esf - packet 173");

                    QString table1 = "MAINTOON";
                    QString objectid2 = "ObjectID (4)";
                    QString xval = "X (3)";
                    QString yval = "Y (3)";
                    QString zval = "Z (3)";
                    QString world1 = "World (1)";
                    QString id2 =  "595E0300";
                    QString facing = "Facing (4)";

                    db.transaction();

                    QSqlQuery query173(QStringLiteral("SELECT [%1],[%2],[%3],[%4],[%5] FROM %6 WHERE [%7] = '%8'").arg(xval).arg(yval).arg(zval).arg(world1).arg(facing).arg(table1).arg(objectid2).arg(id2));



                    while (query173.next())
                    {
                        QSqlRecord record173 = query173.record();
                        qDebug() << "173 DB Call" << record173;
                        xout1 = record173.value(0).toString();
                        yout1 = record173.value(1).toString();
                        zout1 = record173.value(2).toString();
                        worldout1 = record173.value(3).toString();
                        facingout1 = record173.value(4).toString();
                    }
                    db.commit();

                    qlonglong xinter = xout1.toLongLong(0,16); //hex long to divide
                    qlonglong yinter = yout1.toLongLong(0,16); //hex long to divide
                    qlonglong zinter = zout1.toLongLong(0,16); //hex long to divide

                    float xfloat = xinter / 128.0; //divide by 128.0 to get float to cast for 4 byte
                    float yfloat = yinter / 128.0; //divide by 128.0 to get float to cast for 4 byte
                    float zfloat = zinter / 128.0; //divide by 128.0 to get float to cast for 4 byte

                    if (zfloat > 131000.0) //neg z fix leave this in!
                    {
                        float smallsub2 = zfloat - 131072.00195193;
                        zfloat = smallsub2;

                    }




                    qDebug() <<"FLOAT" << xfloat;

                    QByteArray axarray (reinterpret_cast<const char*>(&xfloat), sizeof(xfloat));
                    QByteArray ayarray (reinterpret_cast<const char*>(&yfloat), sizeof(yfloat));
                    QByteArray azarray (reinterpret_cast<const char*>(&zfloat), sizeof(zfloat));
                    QString axstr = axarray.toHex().toUpper();
                    QString aystr = ayarray.toHex().toUpper();
                    QString azstr = azarray.toHex().toUpper();
                    qDebug() <<  "173 Z" << azstr;
                    QString coorstr = axstr+azstr+aystr+facingout1; //xzy 4 byte
                    QByteArray chngcoor = coorstr.toUtf8();
                    int worlddouble = worldout1.toInt(0,16);
                    if (worlddouble != 0) //double world byte
                    {
                        worlddouble = worlddouble * 2;
                        QString doubleout = QString("%1").arg(worlddouble,2,16,QLatin1Char('0'));
                        worldout1 = doubleout;
                    }
                    QByteArray chngwrld = worldout1.toUtf8();

                    QByteArray holder173 = packet173;
                    holder173.replace(0,4,serverid1); //swap IDs
                    holder173.replace(4,4,clientid1); //swap IDs
                    holder173.replace(202,32,chngcoor); //xzy 4 byte hex and facing
                    holder173.replace(200,2,chngwrld); //world byte


                    QByteArray serverno6 = holder173.mid(30,4);
                    ui->lineEdit_5->setText(serverno6.toUpper());

                    byte1 = holder173; //send array to crc
                    elcrc(byte1); //send to get a crc
                    holder173.append(outcrc);
                    QByteArray gameout6 = holder173.fromHex(holder173);
                    ui->textEdit_5->append("Entering World...");
                    udpsocket3.writeDatagram(gameout6, sender3, senderPort3);

                    xout1 = "";
                    yout1="";
                    zout1="";
                    worldout1 = "";
                    facingout1 = "";

                    QDateTime dateTime = dateTime.currentDateTime();
                    QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-173-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText(holder173.toUpper());
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->appendPlainText("Server-173-" + dateTimeString);
                    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                    ui->plainTextEdit->moveCursor(QTextCursor::End);

                }

                ///////////////////////////////////////////////Early Login//////////////////////////////////////////////////////////////////////

                if (!tunare.isEmpty()) //make sure its not empty or we will assert and crash
                {
                    if (earlypacket_map.contains(tunare.last())) //load the rest with qmap packet 181-263
                    {

                        //testing
                        if (client_order.toUpper() == tunare.last().toUpper())
                        {
                            QByteArray firstslice2 = client_order.mid(0,4); //client order is always incremented after u get it
                            QByteArray secondslice2 = client_order.mid(4,4);
                            increment newearly2;
                            QByteArray earlyout3 = newearly2.count(firstslice2);
                            QByteArray earlyout4 = newearly2.count(secondslice2);
                            client_order = earlyout3 + earlyout4; //incremented to next num //will work up to 1B001C00
                            //testing
                            QByteArray holderpmap = earlypacket_map.value(tunare.last()).toUpper();
                            qDebug() << "HOLDER" <<holderpmap;
                            qDebug() <<"MESSAGEEND" << messageend;

                            holderpmap.replace(0,4,serverid1);
                            holderpmap.replace(4,4,clientid1);
                            QByteArray serverno9 = holderpmap.mid(30,4);
                            ui->lineEdit_5->setText(serverno9.toUpper());
                            byte1 = holderpmap;
                            elcrc(byte1);
                            holderpmap.append(outcrc);
                            QByteArray gameoutmap = holderpmap.fromHex(holderpmap);
                            udpsocket3.writeDatagram(gameoutmap, sender3, senderPort3);

                            QDateTime dateTime = dateTime.currentDateTime();
                            QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->appendPlainText("Server-Packet-Map-" + dateTimeString);
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->appendPlainText(holderpmap.toUpper());
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->appendPlainText("Server-Packet-Map-" + dateTimeString);
                            ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                            ui->plainTextEdit->moveCursor(QTextCursor::End);

                        }
                    }
                }
            }

            opcodeend = "";
            opcodeexist = false;
            opcoderesult1 = "";


        } //if opcode exist

        mess1 = "";
        messcarat1 = 0;
        messint1 = 0;
        messint2 = 0;
        messint3 = 0;
        packetsize2 = 0;
        messext1 = "";
        messwork1 = "";
        chancounter1 = 0;
        chancounter2 = 0;
        chancounter3 = 0;
        place1 = 0;
        positiona = 0;
        placeholder1 = 0;
        messageend = "";
        messageholder1 = "";
        opcoderesult1 = "";
        opholder = "";
        fortytwentninenum1 = "";
        gamebuf1 = "";
        gamebuf2 = "";
        fnine1 = "";
        //  combine1 = "";
    }

}
////////////////////////////////////////////////////////////////////////////CRC///////////////////////////


QByteArray MainWindow::elcrc(QByteArray byte1)
{

    quint32 crc32 = 0;
    qint64 i = 0;

    QByteArray test1 = byte1.fromHex(byte1);


    crc32 = 0xFFFFFFFF;



    for (i = 0; i < test1.size(); i++)

    {

        crc32 = (crc32 >> 8) ^ crc32_tab[(crc32 & 0xff) ^ (test1.at(i) & 0xff)];


    }


    crc32 ^= 0x11F19ED3;

    QString hexvalue = QString("%1").arg(crc32, 8, 16, QLatin1Char( '0' ));
    QByteArray originalcrc = hexvalue.toUtf8();
    QByteArray first =  originalcrc.mid(0,2);
    QByteArray second = originalcrc.mid(2,2);
    QByteArray third =  originalcrc.mid(4,2);
    QByteArray fourth = originalcrc.mid(6,2);
    outcrc = fourth+= third += second += first;
    qDebug() <<"fixedcrc" << outcrc;
    return outcrc;
}



void MainWindow::on_pushButton_2_clicked() //STOP SERVER
{



    ui->textEdit_5->append("Stopped UDP Server...");
    ui->textEdit_5->append("Stopped TCP Server...");
    ui->lineEdit->setDisabled(false);
    ui->pushButton->setDisabled(false);
    ui->pushButton_2->setDisabled(true);
    ui->actionLoad_Database->setDisabled(false);
    ui->actionAbout_Hagley2->setDisabled(false);
    ui->actionReset_Main_Coordinates->setDisabled(false);
    ui->actionReset_All_Coaches->setDisabled(false);
    ui->treeView->setDisabled(true);
    ui->progressBar->reset();

    udpsocket.close();
    udpsocket2.close();
    udpsocket3.close();

    server.disconnect();
    server.close();

    server2.disconnect();
    server2.close();

    gamebuf1 = "";
    gamebuf2 = "";
    carata = 0;
    caratb = 0;
    where2 = 0;
    decopacket1 = "";
    high2 = 0;
    low2 = 0;
    packetsize2 = 0;
    decom40 = "";
    ophold1 = "";
    oppos1 = 0;
    opcarat = 0;
    mess1 = "";
    messint1 = 0;
    messcarat1 = 0;
    messint2 = 0;
    messint3 = 0;
    messext1 = "";
    messwork1 = "";
    chancounter1 = 0;
    chancounter2 = 0;
    chancounter3 = 0;
    fortyexist = false;
    messageexist = false;
    opcodeexist = false;
    place1 = 0;
    placeholder1 = 0;
    positiona = 0;
    messageend = "";
    messageholder1 = "";
    opcoderesult1 = "";
    opholder = "";
    fortytwentninenum1 = "";
    packetcounter1 = 0;
    opmessagenumber1 = "";
    opmessagenumber2= "";
    ninecounter1 = 0;
    fnine1 = "";
    halfidmarker = false;
    injectmarker = false;
    twoninetysixmarker = false;
    masterserver = "2A00";
    combine1 = "";
    zerocounter = "0300";
    flipped = false;
    ringbuffer1 = "";
    currentxor = 0;
    xorresult1 = 0;
    xorresult2 = 0;
    xorresult3 = 0;
    faceresult1 = 0;
    res1 = 0;
    res2 = 0;
    res3 = 0;
    res4 = 0;
    xdeconumber1 = 0;
    ydeconumber1 = 0;
    zdeconumber1 = 0;
    facedeconumber1 = 0;
    xstack.clear();
    ystack.clear();
    zstack.clear();
    fstack.clear();
    teleport_destination.clear(); //clearing teleport
    ui->textEdit->clear();
    ui->textEdit_3->clear();
    ui->textEdit_6->clear();
    senderheld1.clear();
    senderPortheld1 = 0;
    ui->lineEdit_10->clear();
    ecchannel = "0100";
    masterx.clear();
    mastery.clear();
    masterz.clear();
    xout1.clear();
    yout1.clear();
    zout1.clear();
    worldout1.clear();
    facingout1.clear();
    serveropcode1.clear();
    masterworld = 0;
    mastertelinc = "0000";
    C_9007_endcount = "0200";
    C_9007_servidcount = "D37C";
    C_closer9007_endcount = "0000";
    clientheld_id = "";
    serverheld_id = "";
    client_opcode = "0000";
    clientid1 = "";
    serverid1 = "";
    messageid = "";
    messagenumb1 = "";
    last_opcode_messnum = "0000";
    coach_counter = "0000";
    coachreply_counter = "0000";
    client_order = "03000400";

    //resetting maps in objectpacket on stop
    QMapIterator<QString,QString> iter (objectpacket::channelmap); //go thru map to get ids and ranges
    while (iter.hasNext())
    {

        iter.next();
        QString chkey1 = iter.key();
        QString mapval1 = "free";
        objectpacket::channelmap.insert(chkey1,mapval1);

    }

    qDebug() << "objectpacket channel map clear" << objectpacket::channelmap;

    QMapIterator<QString,QByteArray> iter3 (objectpacket::incrementmap); //go thru map to reset
    while (iter3.hasNext())
    {

        iter3.next();
        QString inkey2 = iter3.key();
        QByteArray mapval2 = "0000";
        objectpacket::incrementmap.insert(inkey2,mapval2);

    }

    qDebug() << "objectpacket increment map clear" << objectpacket::incrementmap;
    objectpacket::sent_ids_map.clear();
}




void MainWindow::on_actionAbout_Hagley2_triggered()
{
    QMessageBox msgBox4; //about triggered
    msgBox4.setText("EQOA Revival Test Server Hagley-2\r\nVersion 5.8\r\n''Teleport sucks...''\r\n\r\nBy Devin K.\r\nLitany 60 Druid\r\nMarrs Fist\r\nKnights Templar\r\n\r\n& Dustin F.\r\nFaxon 60 rge \r\nPPO");
    msgBox4.exec();
    return;
}

void MainWindow::on_pushButton_3_clicked()
{



    ui->textEdit_4->clear();
    ui->textEdit_5->clear();
    ui->textEdit_7->clear();
    ui->textEdit_8->clear();
    ui->textEdit_9->clear();
    ui->textEdit_10->clear();
    ui->plainTextEdit->clear();
    ui->plainTextEdit_2->clear();
    ui->lineEdit_14->clear();
    ui->lineEdit_15->clear();
    ui->lineEdit_16->clear();
    ui->lineEdit_17->clear();


}

void MainWindow::on_actionLoad_Database_triggered()
{



    QString m_FileName = QFileDialog::getOpenFileName(this,tr("Open File"),"/",tr("DB Files (*.db)")); //Open File Dialog

    if (!m_FileName.isEmpty()) //If Cancel Is Not Pressed
    {
        if (db.isOpen())
        {
            db.close();
            db = QSqlDatabase();
            db.removeDatabase(QSqlDatabase::defaultConnection);
        }

        ui->textEdit_5->append(m_FileName); //Show Path and Filename

        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_FileName);

        if (!db.open())
        {
            QString error1 = "Error Opening Database...";
            ui->textEdit_5->append(error1.toUpper());
            ui->textEdit_5->append(db.lastError().text().toUpper());
            return;
        }

        QStringList rightdb1;
        rightdb1 = db.tables(QSql::Tables);
        qDebug() << "list" << rightdb1;

        if (rightdb1.contains("COACHES") && rightdb1.contains("FACTION"))

        {
            QString success1 = "Database Loaded...";
            ui->textEdit_5->append(success1);
            ui->actionReset_Main_Coordinates->setDisabled(false);
            ui->actionReset_All_Coaches->setDisabled(false);

        }

        else
        {
            QString error2 = "Error Old Database Version...";
            db.close();
            db = QSqlDatabase();
            db.removeDatabase(QSqlDatabase::defaultConnection);
            ui->textEdit_5->append(error2);
            return;
        }
    }
}




void MainWindow::on_pushButton_4_clicked() //TELEPORT send out packet here
{
    spellunmemorize(); //clearing all active spells before port...they wont ungrey after for some reason
    if (teleport_destination.count() != 0)
    {
        past4900 = false;
        ui->treeView->setDisabled(true); //grey it out til we arrive
        QByteArray telepacket = teleport_destination;
        telepacket.replace(4,4,clientheld_id);
        telepacket.replace(0,4,C_9007_servidcount);
        //also replace server since we are going to change!!

        //packet increment

        increment F607_Packet; //get class
        QByteArray count_f607 = F607_Packet.count(masterserver); //packet increment
        telepacket.replace(30,4,count_f607); //replace new count at 30
        masterserver = count_f607.toUpper(); //replace new count
        ui->lineEdit_5->setText(count_f607.toUpper());

        //opcode
        increment F607_Port; //get class
        QByteArray newf607 = F607_Port.count(serveropcode1); //old opcode count
        telepacket.replace(38,4,newf607); //replace new count at 38
        serveropcode1 = newf607.toUpper(); //replace new count
        ui->lineEdit_10->setText(newf607.toUpper());

        //end packet
        increment F607_End; //get class
        QByteArray endpacket_f607 = F607_End.count(mastertelinc); //end packet increment
        telepacket.replace(96,4,mastertelinc); //replace new count at 96 //!! trying to be one behind so it starts at 0000
        mastertelinc = endpacket_f607.toUpper(); //replace new count

        byte1 = telepacket;
        elcrc(byte1);
        telepacket.append(outcrc);
        QByteArray teleportout = telepacket.fromHex(telepacket);


        QDateTime dateTime = dateTime.currentDateTime();
        QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Server-Main-Teleport-" + dateTimeString);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText(telepacket.toUpper());
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Server-Main-Teleport-" + dateTimeString);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->moveCursor(QTextCursor::End);
        udpsocket3.writeDatagram(teleportout, senderheld1, senderPortheld1);

        ui->pushButton_4->setEnabled(false);
        ui->textEdit->clear();
        ui->textEdit_3->clear();
        ui->textEdit_6->clear();

        ////////////////////////4 Byte to 3 Byte//////////////////////////////


        QByteArray Teleport_Code = teleport_destination;
        QByteArray Teleport_Code_X = Teleport_Code.mid(48,8);
        QByteArray Teleport_Code_Z = Teleport_Code.mid(56,8);
        QByteArray Teleport_Code_Y = Teleport_Code.mid(64,8);

        QByteArray numinput1 = Teleport_Code_X;
        qlonglong outnum1 = numinput1.toLongLong(0,16);
        quint32 reversenum5 = ((outnum1 & 0xFF) << 24) | ((outnum1 & 0xFF00) << 8) | ((outnum1 >> 8) & 0xFF00) | (outnum1 >> 24);
        QString outhex5 = QString("%1").arg(reversenum5,8,16,QLatin1Char('0'));
        qlonglong outnum2 = outhex5.toLongLong(0,16); //adding union code instead of punning
        union {
            qlonglong outnum2;
            float array6;
        } xresult = {outnum2};
        qlonglong smallhex1 = xresult.array6 * 128.0;

        //float array6  = *reinterpret_cast<float*>(&outnum2);
        // qlonglong smallhex1 = array6 * 128.0;

        QString outaxo2 = QString("%1").arg(smallhex1,6,16,QLatin1Char('0'));
        ui->lineEdit_14->setText(outaxo2.toUpper()); //3 byte X
        masterx = outaxo2.toUtf8().toUpper(); //send to radius code
        holdmasterx = outaxo2.toUtf8().toUpper();

        ///////////////////////////////X/////////////////////////////////


        QByteArray numinput3 = Teleport_Code_Z;
        qlonglong outnum5 = numinput3.toLongLong(0,16);
        quint32 reversenum7 = ((outnum5 & 0xFF) << 24) | ((outnum5 & 0xFF00) << 8) | ((outnum5 >> 8) & 0xFF00) | (outnum5 >> 24);
        QString outhex7 = QString("%1").arg(reversenum7,8,16,QLatin1Char('0'));
        qlonglong outnum6 = outhex7.toLongLong(0,16);

        union {
            qlonglong outnum6;
            float array8;
        } zresult = {outnum6};

        qlonglong smallhex3 = zresult.array8 * 128.0;
        // float array8  = *reinterpret_cast<float*>(&outnum6);
        //qlonglong smallhex3 = array8 * 128.0;
        QString outazo4 = QString("%1").arg(smallhex3,6,16,QLatin1Char('0'));


        if (smallhex3 < 0)
        {

            QByteArray neg1 = neg1.number(smallhex3,16).toUpper();
            QByteArray neg2 = neg1.mid(neg1.size() - 6,6);
            ui->lineEdit_16->setText(neg2.toUpper()); //3 byte Negative Z
            masterz = neg2.toUpper();
            holdmasterz = QString::fromUtf8(neg2.toUpper()); //save teleport z

        }
        else
        {
            outazo4 = QString("%1").arg(smallhex3,6,16,QLatin1Char('0'));
            ui->lineEdit_16->setText(outazo4.toUpper()); //3 byte Positive Z
            masterz = outazo4.toUtf8().toUpper();
            holdmasterz = outazo4.toUpper(); //save teleport z
        }

        /////////////////////////////////Z//////////////////////////////////




        QByteArray numinput2 = Teleport_Code_Y;
        qlonglong outnum3 = numinput2.toLongLong(0,16);
        quint32 reversenum6 = ((outnum3 & 0xFF) << 24) | ((outnum3 & 0xFF00) << 8) | ((outnum3 >> 8) & 0xFF00) | (outnum3 >> 24);
        QString outhex6 = QString("%1").arg(reversenum6,8,16,QLatin1Char('0'));
        qlonglong outnum4 = outhex6.toLongLong(0,16);

        union {
            qlonglong outnum4;
            float array7;
        } yresult = {outnum4};

        qlonglong smallhex2 = yresult.array7 * 128.0;
        //float array7  = *reinterpret_cast<float*>(&outnum4);
        // qlonglong smallhex2 = array7 * 128.0;
        QString outayo3 = QString("%1").arg(smallhex2,6,16,QLatin1Char('0'));
        ui->lineEdit_15->setText(outayo3.toUpper()); //3 byte Y
        mastery = outayo3.toUtf8().toUpper(); //send to radius code
        holdmastery = outayo3.toUtf8().toUpper();


        qDebug() << "Teleport-X-" << holdmasterx;
        qDebug() << "Teleport-Z-" << holdmasterz;
        qDebug() << "Teleport-Y-" << holdmastery;
        ui->textEdit_5->append(QString("Teleport-X-%1-Z-%2-Y-%3").arg(holdmasterx).arg(holdmasterz).arg(holdmastery));
        //////////////////////////Y///////////4 Byte to 3 Byte///////////////////////////////////////////////////////

        QByteArray newc91 = generator::Players_Map.value("595E0300");
        newc91.replace(12,6,holdmasterx.toUtf8());
        newc91.replace(18,6,holdmasterz.toUtf8());
        newc91.replace(24,6,holdmastery.toUtf8());
        QString chosenworld2 = QString("%1").arg(masterworld,2,16,QLatin1Char('0')).toUpper();
        newc91.replace(34,2,chosenworld2.toUtf8());
        generator::Players_Map.insert("595E0300",newc91); //replace the new coordinates in the map!!!

        /////////////////////////////9007/////////////////////////////////
        //ip
        QByteArray nineoseven = "d37c5ae79dc001bf543213d9bc0d009300fb16f9009007D37C5727280a6cc7030000005ae73b1d69776218";
        QByteArray ourip1 = teleport_ip; //get current ip
        QByteArray first4 =  ourip1.mid(0,2); //reverse it
        QByteArray second4 = ourip1.mid(2,2);
        QByteArray third4 =  ourip1.mid(4,2);
        QByteArray fourth4 = ourip1.mid(6,2);

        QByteArray revip4 = fourth4 += third4 += second4 += first4;
        nineoseven.replace(54,8,revip4); //replace ip

        //ids
        nineoseven.replace(4,4,clientheld_id); //replace ids
        nineoseven.replace(0,4,serverheld_id);

        //packet number
        increment packetcount1; //get class
        QByteArray newcount1 = packetcount1.count(masterserver); //will be masterserver
        nineoseven.replace(30,4,newcount1); //replace new count at 30
        masterserver = newcount1.toUpper(); //replace new count
        ui->lineEdit_5->setText(newcount1.toUpper());

        //opcode number
        increment packetcount2; //get class
        QByteArray newcount2 = packetcount2.count(serveropcode1); //will be server opcode
        nineoseven.replace(38,4,newcount2); //replace new count at 38
        serveropcode1 = newcount2.toUpper(); //replace new count
        ui->lineEdit_10->setText(newcount2.toUpper());

        //server id
        increment packetcount3; //get class
        QByteArray newcount3 = packetcount3.count(C_9007_servidcount); //increment server id
        nineoseven.replace(46,4,newcount3); //replace new count at 46
        C_9007_servidcount = newcount3.toUpper(); //replace new count


        //end count
        increment packetcount4; //get class
        QByteArray newcount4 = packetcount4.count(C_9007_endcount); //end of packet count
        nineoseven.replace(62,4,newcount4); //replace new count at 62
        C_9007_endcount = newcount4.toUpper(); //replace new count


        byte1 = nineoseven;
        elcrc(byte1);
        nineoseven.append(outcrc);
        QByteArray nineosevenout = nineoseven.fromHex(nineoseven);


        QDateTime dateTimet_1 = dateTime.currentDateTime();
        QString dateTimeStringt_1 = dateTimet_1.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Server-9007-" + dateTimeStringt_1);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText(nineoseven.toUpper());
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Server-9007-" + dateTimeStringt_1);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->moveCursor(QTextCursor::End);
        udpsocket3.writeDatagram(nineosevenout, senderheld1, senderPortheld1);
        //////////////////////////////////////////////////////////////////////////////////////////




        if (db.isOpen())
        {
            /////////////////////////Save Location///////////////////////////

            db.transaction();
            QSqlQuery querytlpt2;
            QString tablesel2 = "MAINTOON";
            QString objecttlpt2 = "ObjectID (4)";
            QString xtlpt = "X (3)";
            QString ytlpt = "Y (3)";
            QString ztlpt = "Z (3)";
            QString worldtlpt = "World (1)";
            QString tlptid2 = "595E0300";
            QString chosenworld = QString("%1").arg(masterworld,2,16,QLatin1Char('0')).toUpper(); //get the world
            qDebug() << "chosenworld" << chosenworld;
            QString x1 = QString::fromUtf8(masterx);
            QString y1 = QString::fromUtf8(mastery);
            QString z1 = QString::fromUtf8(masterz);

            querytlpt2.prepare(QStringLiteral("UPDATE %1 SET [%2] = :val2,[%3] = :val3,[%4] = :val4,[%5] = :val5 WHERE [%6] = :val").arg(tablesel2).arg(xtlpt).arg(ytlpt).arg(ztlpt).arg(worldtlpt).arg(objecttlpt2));

            querytlpt2.bindValue(":val",tlptid2);
            querytlpt2.bindValue(":val2",x1.toUpper());
            querytlpt2.bindValue(":val3",y1.toUpper());
            querytlpt2.bindValue(":val4",z1.toUpper());
            querytlpt2.bindValue(":val5",chosenworld.toUpper());

            querytlpt2.exec();
            db.commit();
        }
        ///////////////////////////////////////////////////////////




        ////Also Clear The Channels, Reset keptlist,sendids/////
        QMapIterator<QString,QString> iter (objectpacket::channelmap); //go thru map to get ids and ranges
        while (iter.hasNext())
        {

            iter.next();
            QString chkey1 = iter.key();
            QString mapval1 = "free";
            objectpacket::channelmap.insert(chkey1,mapval1);

        }


        objectpacket::sent_ids_map.clear(); //clear sent ids
        objectpacket::keptlist1.clear(); //clear keptlist
        qDebug() << "objectpacket channel map clear - Teleport" << objectpacket::channelmap;
        teleporting = true; //set teleport flag to true
        teleportmainswitch = true; //turn on fc02d007 sender

    }

}








void MainWindow::on_treeView_clicked(const QModelIndex &index) //get the selection
{
    QStandardItem *item3 = standardModel->itemFromIndex(index); //get index of what was selected
    QString hiddentext = item3->data(Qt::UserRole+1).toString(); //get the hidden userrole data
    int hiddenworld = item3->data(Qt::UserRole+2).toInt();
    QString world = QString("%1").arg(hiddenworld,2,16,QLatin1Char('0')).toUpper(); //get the world
    masterworld = hiddenworld; //save it to use it later
    teleport_destination = item3->data(Qt::UserRole+3).toByteArray();
    ui->textEdit->setText(hiddentext); //show destination
    ui->textEdit_3->setText(world); //show world
    ui->textEdit_6->setText(teleport_destination); //show packet

    if (hiddentext.count()  != 0 && flipped == true)
    {
        ui->pushButton_4->setEnabled(true);
    }
    else
    {
        ui->pushButton_4->setEnabled(false);
        ui->textEdit->clear();
        ui->textEdit_3->clear();
        ui->textEdit_6->clear();
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index != 2)
    {
        ui->pushButton_4->setEnabled(false);
        ui->textEdit->clear();
        ui->textEdit_3->clear();
        ui->textEdit_6->clear();
    }


    if (index == 2) //if  teleport tab is selected
    {
        standardModel->clear(); //clear model if tab clicked on and off
        QStandardItem *rootNode = standardModel->invisibleRootItem();//Set Invisible Root Item again after clearing model
        ////////////////////TREE  & MODEL/////////////////////////////////
        treetop = new QStandardItem("Teleport"); //Set TreeView Header Text
        standardModel->setHorizontalHeaderItem(0,treetop);
        ui->treeView->setUniformRowHeights(true);
        ui->treeView->setModel(standardModel); //Hook View Into Model
        //////////////////////TREE & MODEL////////////////////////////////

        //////////////////Tunaria and subs//////////////////////////

        QString fin1 = "Tunaria";
        one = new QStandardItem(fin1);
        rootNode->appendRow(one);

        QString sub_tun1 = "One";
        two = new QStandardItem(sub_tun1);
        one->appendRow(two);


        QString sub1_1 = "Permafrost";
        three = new QStandardItem(sub1_1);
        three->setData(sub1_1,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Permafrost_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub2_1 = "Zentar's Keep";
        three = new QStandardItem(sub2_1);
        three->setData(sub2_1,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Zentars_Keep_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_1 = "Bogman Village";
        three = new QStandardItem(sub3_1);
        three->setData(sub3_1,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Bogman_Village_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub4_1 = "Mariel Village";
        three = new QStandardItem(sub4_1);
        three->setData(sub4_1,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Mariel_Village_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_1 = "Wyndhaven";
        three = new QStandardItem(sub5_1);
        three->setData(sub5_1,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Wyndhaven_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub6_1 = "Whale Hill";
        three = new QStandardItem(sub6_1);
        three->setData(sub6_1,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Whale_Hill_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub7_1 = "Qeynos";
        three = new QStandardItem(sub7_1);
        three->setData(sub7_1,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Qeynos_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub8_1 = "Qeynos Prison";
        three = new QStandardItem(sub8_1);
        three->setData(sub8_1,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Qeynos_Prison_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_1 = "Highbourne";
        three = new QStandardItem(sub9_1);
        three->setData(sub9_1,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Highbourne_tp,Qt::UserRole+3);
        two->appendRow(three);
        /////////////////////////////////////////////////Two/////////////////////////////
        QString sub_tun2 = "Two";
        two = new QStandardItem(sub_tun2);
        one->appendRow(two);

        QString sub1_2 = "SnowBlind Plains";
        three = new QStandardItem(sub1_2);
        three->setData(sub1_2,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Snowblind_Plains_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub2_2 = "Unkempt North";
        three = new QStandardItem(sub2_2);
        three->setData(sub2_2,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Unkempt_North_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_2 = "Unkempt Glade";
        three = new QStandardItem(sub3_2);
        three->setData(sub3_2,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Unkempt_Glade_tp,Qt::UserRole+3);
        two->appendRow(three);


        QString sub4_2 = "Twisted Tower";
        three = new QStandardItem(sub4_2);
        three->setData(sub4_2,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Twisted_Tower_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_2 = "Jethros Cast";
        three = new QStandardItem(sub5_2);
        three->setData(sub5_2,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Jethros_Cast_tp,Qt::UserRole+3);
        two->appendRow(three);


        QString sub6_2 = "Crethley Manor";
        three = new QStandardItem(sub6_2);
        three->setData(sub6_2,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Crethley_Manor_tp,Qt::UserRole+3);
        two->appendRow(three);


        QString sub7_2 = "Hagley1";
        three = new QStandardItem(sub7_2);
        three->setData(sub7_2,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Hagley1_tp,Qt::UserRole+3);
        two->appendRow(three);


        QString sub8_2 = "Hagley2";
        three = new QStandardItem(sub8_2);
        three->setData(sub8_2,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Hagley2_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_2 = "Druids Watch";
        three = new QStandardItem(sub9_2);
        three->setData(sub9_2,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Druids_Watch_tp,Qt::UserRole+3);
        two->appendRow(three);


        QString sub10_2 = "Stoneclaw";
        three = new QStandardItem(sub10_2);
        three->setData(sub10_2,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Stoneclaw_tp,Qt::UserRole+3);
        two->appendRow(three);

        ///////////////////////////////////////////Three/////////////////////////////

        QString sub_tun3 = "Three";
        two = new QStandardItem(sub_tun3);
        one->appendRow(two);

        QString sub1_3 = "Anu Village";
        three = new QStandardItem(sub1_3);
        three->setData(sub1_3,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Anu_Village_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub2_3 = "North Wilderlands";
        three = new QStandardItem(sub2_3);
        three->setData(sub2_3,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(North_Wilderlands_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_3 = "Castle Lightwolf";
        three = new QStandardItem(sub3_3);
        three->setData(sub3_3,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Castle_Lightwolf_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub4_3 = "Salisearaneen";
        three = new QStandardItem(sub4_3);
        three->setData(sub4_3,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Salisearaneen_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_3 = "Murnf";
        three = new QStandardItem(sub5_3);
        three->setData(sub5_3,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Murnf_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub6_3 = "Surefall Glade";
        three = new QStandardItem(sub6_3);
        three->setData(sub6_3,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Surefall_Glade_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub7_3 = "Fog Marsh";
        three = new QStandardItem(sub7_3);
        three->setData(sub7_3,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Fog_Marsh_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub8_3 = "Honjour";
        three = new QStandardItem(sub8_3);
        three->setData(sub8_3,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Honjour_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_3 = "Bear Cave";
        three = new QStandardItem(sub9_3);
        three->setData(sub9_3,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Bear_Cave_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub10_3 = "Spider Mine";
        three = new QStandardItem(sub10_3);
        three->setData(sub10_3,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Spider_Mine_tp,Qt::UserRole+3);
        two->appendRow(three);


        ///////////////////////////Four///////////////////////////////

        QString sub_tun4 = "Four";
        two = new QStandardItem(sub_tun4);
        one->appendRow(two);

        QString sub1_4 = "Aviak Village";
        three = new QStandardItem(sub1_4);
        three->setData(sub1_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Aviak_Village_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub2_4 = "Frosteye Vally";
        three = new QStandardItem(sub2_4);
        three->setData(sub2_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Frosteye_Vally_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_4 = "Guardian Forest";
        three = new QStandardItem(sub3_4);
        three->setData(sub3_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Guardian_Forest_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub4_4 = "Gramash Ruins";
        three = new QStandardItem(sub4_4);
        three->setData(sub4_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Gramash_Ruins_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_4 = "Spirit Talkers Wood";
        three = new QStandardItem(sub5_4);
        three->setData(sub5_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Spirit_Talkers_Wood_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub6_4 = "Spirit Talkers Wood2";
        three = new QStandardItem(sub6_4);
        three->setData(sub6_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Spirit_Talkers_Wood2_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub7_4 = "Wymondham";
        three = new QStandardItem(sub7_4);
        three->setData(sub7_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Wymondham_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub8_4 = "Al Karad Ruins";
        three = new QStandardItem(sub8_4);
        three->setData(sub8_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Al_Karad_Ruins_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_4 = "Blakedown";
        three = new QStandardItem(sub9_4);
        three->setData(sub9_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Blakedown_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub10_4 = "Mayfly Glade";
        three = new QStandardItem(sub10_4);
        three->setData(sub10_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Mayfly_Glade_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub11_4 = "Urglunts Wall";
        three = new QStandardItem(sub11_4);
        three->setData(sub11_4,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Urglunts_Wall_tp,Qt::UserRole+3);
        two->appendRow(three);


        ///////////////////////////Five/////////////////////////

        QString sub_tun5 = "Five";
        two = new QStandardItem(sub_tun5);
        one->appendRow(two);

        QString sub1_5 = "Halas";
        three = new QStandardItem(sub1_5);
        // three->setForeground(Qt::red);
        three->setData(sub1_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Halas_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub2_5 = "Freezeblood Village";
        three = new QStandardItem(sub2_5);
        three->setData(sub2_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Freezeblood_Village_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_5 = "Diren Village";
        three = new QStandardItem(sub3_5);
        three->setData(sub3_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Diren_Village_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub4_5 = "Mt Hatespike";
        three = new QStandardItem(sub4_5);
        three->setData(sub4_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Mt_Hatespike_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_5 = "Blackburrow";
        three = new QStandardItem(sub5_5);
        three->setData(sub5_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Blackburrow_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub6_5 = "Jareds Blight";
        three = new QStandardItem(sub6_5);
        three->setData(sub6_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Jareds_Blight_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub7_5 = "Darvar Manor";
        three = new QStandardItem(sub7_5);
        three->setData(sub7_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Darvar_Manor_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub8_5 = "Alseops Wall";
        three = new QStandardItem(sub8_5);
        three->setData(sub8_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Alseops_Wall_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_5 = "Forkwatch";
        three = new QStandardItem(sub9_5);
        three->setData(sub9_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Forkwatch_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub10_5 = "South Crossroads";
        three = new QStandardItem(sub10_5);
        three->setData(sub10_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(South_Crossroads_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub11_5 = "Dark Solace";
        three = new QStandardItem(sub11_5);
        three->setData(sub11_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Dark_Solace_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub12_5 = "Widows Peak";
        three = new QStandardItem(sub12_5);
        three->setData(sub12_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Widows_Peak_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub13_5 = "Kelinar";
        three = new QStandardItem(sub13_5);
        three->setData(sub13_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Kelinar_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub14_5 = "Gerntar Mines";
        three = new QStandardItem(sub14_5);
        three->setData(sub14_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Gerntar_Mines_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub15_5 = "Oggok Gate";
        three = new QStandardItem(sub15_5);
        three->setData(sub15_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Oggok_Gate_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub16_5 = "Milas Reef";
        three = new QStandardItem(sub16_5);
        three->setData(sub16_5,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Milas_Reef_tp,Qt::UserRole+3);
        two->appendRow(three);

        ///////////////////////////////Six///////////////////////////////

        QString sub_tun6 = "Six";
        two = new QStandardItem(sub_tun6);
        one->appendRow(two);

        QString sub1_6 = "Snowfist";
        three = new QStandardItem(sub1_6);
        three->setData(sub1_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Snowfist_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub2_6 = "Goldfeather Eyrie";
        three = new QStandardItem(sub2_6);
        three->setData(sub2_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Goldfeather_Eyrie_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_6 = "Moradhim";
        three = new QStandardItem(sub3_6);
        three->setData(sub3_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Moradhim_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub4_6 = "Baga Village";
        three = new QStandardItem(sub4_6);
        three->setData(sub4_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Baga_Village_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_6 = "Merry by Water";
        three = new QStandardItem(sub5_6);
        three->setData(sub5_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Merry_by_Water_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub6_6 = "Bandit Hills";
        three = new QStandardItem(sub6_6);
        three->setData(sub6_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Bandit_Hills_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub7_6 = "Strags Rest";
        three = new QStandardItem(sub7_6);
        three->setData(sub7_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Strags_Rest_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub8_6 = "Salt Mine";
        three = new QStandardItem(sub8_6);
        three->setData(sub8_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Salt_Mine_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_6 = "Centaur Vally";
        three = new QStandardItem(sub9_6);
        three->setData(sub9_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Centaur_Vally_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub10_6 = "Brog Fens";
        three = new QStandardItem(sub10_6);
        three->setData(sub10_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Brog_Fens_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub11_6 = "Blackwater";
        three = new QStandardItem(sub11_6);
        three->setData(sub11_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Blackwater_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub12_6 = "Fort Alliance";
        three = new QStandardItem(sub12_6);
        three->setData(sub12_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Fort_Alliance_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub13_6 = "Elephant Graveyard";
        three = new QStandardItem(sub13_6);
        three->setData(sub13_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Elephant_Graveyard_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub14_6 = "Kerplunk";
        three = new QStandardItem(sub14_6);
        three->setData(sub14_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Kerplunk_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub15_6 = "Cazic Thule";
        three = new QStandardItem(sub15_6);
        three->setData(sub15_6,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Cazic_Thule_tp,Qt::UserRole+3);
        two->appendRow(three);


        //////////////////////////Seven///////////////////////////////////////

        QString sub_tun7 = "Seven";
        two = new QStandardItem(sub_tun7);
        one->appendRow(two);

        QString sub1_7 = "Greyvaxs Caves";
        three = new QStandardItem(sub1_7);
        three->setData(sub1_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Greyvaxs_Caves_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub2_7 = "Snafitzers House";
        three = new QStandardItem(sub2_7);
        three->setData(sub2_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Snafitzers_House_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_7 = "Shon To Monastery";
        three = new QStandardItem(sub3_7);
        three->setData(sub3_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Shon_To_Monastery_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub4_7 = "Misty Thicket";
        three = new QStandardItem(sub4_7);
        three->setData(sub4_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Misty_Thicket_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_7 = "Runnyeye";
        three = new QStandardItem(sub5_7);
        three->setData(sub5_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Runnyeye_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub6_7 = "Highpass";
        three = new QStandardItem(sub6_7);
        three->setData(sub6_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Highpass_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub7_7 = "Trails End";
        three = new QStandardItem(sub7_7);
        three->setData(sub7_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Trails_End_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub8_7 = "Dshinns Redoubt";
        three = new QStandardItem(sub8_7);
        three->setData(sub8_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Dshinns_Redoubt_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_7 = "Wktaans 4th Talon";
        three = new QStandardItem(sub9_7);
        three->setData(sub9_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Wktaans_4th_Talon_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub10_7 = "Serpent Hills";
        three = new QStandardItem(sub10_7);
        three->setData(sub10_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Serpent_Hills_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub11_7 = "Tak Xiz";
        three = new QStandardItem(sub11_7);
        three->setData(sub11_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Tak_Xiz_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub12_7 = "Tak Xiz South";
        three = new QStandardItem(sub12_7);
        three->setData(sub12_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Tak_Xiz_South_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub13_7 = "Lake Noregard";
        three = new QStandardItem(sub13_7);
        three->setData(sub13_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Lake_Noregard_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub14_7 = "Dinbak";
        three = new QStandardItem(sub14_7);
        three->setData(sub14_7,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Dinbak_tp,Qt::UserRole+3);
        two->appendRow(three);



        //////////////////////////////Eight////////////////////////////////////////

        QString sub_tun8 = "Eight";
        two = new QStandardItem(sub_tun8);
        one->appendRow(two);

        QString sub1_8 = "Fayspire Gate";
        three = new QStandardItem(sub1_8);
        three->setData(sub1_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Fayspire_Gate_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub2_8 = "Fayspires";
        three = new QStandardItem(sub2_8);
        three->setData(sub2_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Fayspires_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_8 = "Tethelin";
        three = new QStandardItem(sub3_8);
        three->setData(sub3_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Tethelin_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub4_8 = "Thedruk";
        three = new QStandardItem(sub4_8);
        three->setData(sub4_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Thedruk_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_8 = "Rivervale";
        three = new QStandardItem(sub5_8);
        three->setData(sub5_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Rivervale_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub6_8 = "Moss Mouth Cavern";
        three = new QStandardItem(sub6_8);
        three->setData(sub6_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Moss_Mouth_Cavern_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub7_8 = "Bastable Village";
        three = new QStandardItem(sub7_8);
        three->setData(sub7_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Bastable_Village_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub8_8 = "Ferrans Hope";
        three = new QStandardItem(sub8_8);
        three->setData(sub8_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Ferrans_Hope_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_8 = "Deathfist Horde";
        three = new QStandardItem(sub9_8);
        three->setData(sub9_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Deathfist_Horde_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub10_8 = "Chiktar Hive";
        three = new QStandardItem(sub10_8);
        three->setData(sub10_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Chiktar_Hive_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub11_8 = "Tak Xiv";
        three = new QStandardItem(sub11_8);
        three->setData(sub11_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Tak_Xiv_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub12_8 = "Takish Hiz";
        three = new QStandardItem(sub12_8);
        three->setData(sub12_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Takish_Hiz_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub13_8 = "Burial Mounds";
        three = new QStandardItem(sub13_8);
        three->setData(sub13_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Burial_Mounds_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub14_8 = "Stone Watchers";
        three = new QStandardItem(sub14_8);
        three->setData(sub14_8,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Stone_Watchers_tp,Qt::UserRole+3);
        two->appendRow(three);


        ////////////////////////////////////Nine/////////////////////////////////////


        QString sub_tun9 = "Nine";
        two = new QStandardItem(sub_tun9);
        one->appendRow(two);

        QString sub1_9 = "NE Mountain Boundary";
        three = new QStandardItem(sub1_9);
        three->setData(sub1_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(NE_Mountain_Boundary_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub2_9 = "Kara Village";
        three = new QStandardItem(sub2_9);
        three->setData(sub2_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Kara_Village_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_9 = "Castle Felstar";
        three = new QStandardItem(sub3_9);
        three->setData(sub3_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Castle_Felstar_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub4_9 = "Fort Seriak";
        three = new QStandardItem(sub4_9);
        three->setData(sub4_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Fort_Seriak_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_9 = "North Kithicor";
        three = new QStandardItem(sub5_9);
        three->setData(sub5_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(North_Kithicor_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub6_9 = "Saerk Towers";
        three = new QStandardItem(sub6_9);
        three->setData(sub6_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Saerk_Towers_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub7_9 = "Tomb of Kings";
        three = new QStandardItem(sub7_9);
        three->setData(sub7_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Tomb_of_Kings_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub8_9 = "Deathfist Forge";
        three = new QStandardItem(sub8_9);
        three->setData(sub8_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Deathfist_Forge_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_9 = "Deathfist Citadel";
        three = new QStandardItem(sub9_9);
        three->setData(sub9_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Deathfist_Citadel_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub10_9 = "Box Canyons";
        three = new QStandardItem(sub10_9);
        three->setData(sub10_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Box_Canyons_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub11_9 = "Eternal Desert";
        three = new QStandardItem(sub11_9);
        three->setData(sub11_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Eternal_Desert_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub12_9 = "Oasis";
        three = new QStandardItem(sub12_9);
        three->setData(sub12_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Oasis_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub13_9 = "Sea of Lions";
        three = new QStandardItem(sub13_9);
        three->setData(sub13_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Sea_of_Lions_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub14_9 = "Ant Colonies";
        three = new QStandardItem(sub14_9);
        three->setData(sub14_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Ant_Colonies_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub15_9 = "Brokenskull Rock";
        three = new QStandardItem(sub15_9);
        three->setData(sub15_9,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Brokenskull_Rock_tp,Qt::UserRole+3);
        two->appendRow(three);

        ////////////////////////////////Ten//////////////////////////////////////


        QString sub_tun10 = "Ten";
        two = new QStandardItem(sub_tun10);
        one->appendRow(two);

        QString sub1_10 = "Klick Anon";
        three = new QStandardItem(sub1_10);
        three->setData(sub1_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Klick_Anon_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub2_10 = "Collonridge Cemetary";
        three = new QStandardItem(sub2_10);
        three->setData(sub2_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Collonridge_Cemetary_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_10 = "The Green Rift";
        three = new QStandardItem(sub3_10);
        three->setData(sub3_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(The_Green_Rift_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub4_10 = "Mu Lins Reach";
        three = new QStandardItem(sub4_10);
        three->setData(sub4_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Mu_Lins_Reach_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_10 = "Temple of Light";
        three = new QStandardItem(sub5_10);
        three->setData(sub5_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Temple_of_Light_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub6_10 = "Northwestern Ro";
        three = new QStandardItem(sub6_10);
        three->setData(sub6_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Northwestern_Ro_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub7_10 = "Muniels Tea Garden";
        three = new QStandardItem(sub7_10);
        three->setData(sub7_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Muniels_Tea_Garden_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub8_10 = "Al Farak Ruins";
        three = new QStandardItem(sub8_10);
        three->setData(sub8_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Al_Farak_Ruins_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_10 = "Sycamore Joys Rest";
        three = new QStandardItem(sub9_10);
        three->setData(sub9_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Sycamore_Joys_Rest_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub10_10 = "Great Waste";
        three = new QStandardItem(sub10_10);
        three->setData(sub10_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Great_Waste_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub11_10 = "Silthtar Hive";
        three = new QStandardItem(sub11_10);
        three->setData(sub11_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Silthtar_Hive_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub12_10 = "Sslathis";
        three = new QStandardItem(sub12_10);
        three->setData(sub12_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Sslathis_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub13_10 = "Bashers Enclave";
        three = new QStandardItem(sub13_10);
        three->setData(sub13_10,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Bashers_Enclave_tp,Qt::UserRole+3);
        two->appendRow(three);

        /////////////////////////////////Eleven////////////////////////////////

        QString sub_tun11 = "Eleven";
        two = new QStandardItem(sub_tun11);
        one->appendRow(two);



        QString sub2_11 = "Rogue Clockworks";
        three = new QStandardItem(sub2_11);
        three->setData(sub2_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Rogue_Clockworks_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub3_11 = "Neriak";
        three = new QStandardItem(sub3_11);
        three->setData(sub3_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Neriak_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub4_11 = "Bobble by Water";
        three = new QStandardItem(sub4_11);
        three->setData(sub4_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Bobble_by_Water_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub5_11 = "Hodstock and Temby";
        three = new QStandardItem(sub5_11);
        three->setData(sub5_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Hodstock_and_Temby_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub6_11 = "Freeport";
        three = new QStandardItem(sub6_11);
        three->setData(sub6_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Freeport_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub7_11 = "Northern Ro";
        three = new QStandardItem(sub7_11);
        three->setData(sub7_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Northern_Ro_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub8_11 = "Elemental Towers";
        three = new QStandardItem(sub8_11);
        three->setData(sub8_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Elemental_Towers_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub9_11 = "Hazinak Dock";
        three = new QStandardItem(sub9_11);
        three->setData(sub9_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Hazinak_Dock_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub10_11 = "Hazinak Temple";
        three = new QStandardItem(sub10_11);
        three->setData(sub10_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Hazinak_Temple_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub11_11 = "Guk";
        three = new QStandardItem(sub11_11);
        three->setData(sub11_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Guk_tp,Qt::UserRole+3);
        two->appendRow(three);

        QString sub12_11 = "Grobb";
        three = new QStandardItem(sub12_11);
        three->setData(sub12_11,Qt::UserRole+1); //hide name
        three->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        three->setData(Grobb_tp,Qt::UserRole+3);
        two->appendRow(three);

        ////////////////////////////////////Tunaria End///////////////////////



        //////////////////////Rathe//////////////////////////////////////////

        QString fin2 = "Rathe";
        one = new QStandardItem(fin2);
        rootNode->appendRow(one);


        QString sub_rat1 = "Oggok";
        two = new QStandardItem(sub_rat1);
        two->setData(sub_rat1,Qt::UserRole+1); //Hide Name
        two->setData(0x01,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Oggok_tp,Qt::UserRole+3);
        one->appendRow(two);


        ///////////////////Rathe End///////////////////////////////////////////

        ////////////////////Odus////////////////////////////////////////////////
        QString fin3 = "Odus";
        one = new QStandardItem(fin3);
        rootNode->appendRow(one);

        QString sub_odu1 = "Phantoms";
        two = new QStandardItem(sub_odu1);
        two->setData(sub_odu1,Qt::UserRole+1); //Hide Name
        two->setData(0x02,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Phantoms_tp,Qt::UserRole+3);
        one->appendRow(two);

        ////////////////////Odus End/////////////////////////////////////////////

        ////////////////////Lavastorm////////////////////////////////////////////////

        QString fin4 = "Lavastorm";
        one = new QStandardItem(fin4);
        rootNode->appendRow(one);

        QString sub_lav1 = "Waysender";
        two = new QStandardItem(sub_lav1);
        two->setData(sub_lav1,Qt::UserRole+1); //Hide Name
        two->setData(0x03,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Waysender_tp,Qt::UserRole+3);
        one->appendRow(two);

        ///////////////////Lavastorm End////////////////////////////////////////////////

        ////////////////////Plane of Sky////////////////////////////////////////////////

        QString fin5 = "Plane of Sky";
        one = new QStandardItem(fin5);
        rootNode->appendRow(one);

        QString sub_sky1 = "Main Island";
        two = new QStandardItem(sub_sky1);
        two->setData(sub_sky1,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Pos_Main_Isle,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky2 = "Isle of Wonder";
        two = new QStandardItem(sub_sky2);
        two->setData(sub_sky2,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Isle_of_Wonder,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky3 = "Sun 1 In";
        two = new QStandardItem(sub_sky3);
        two->setData(sub_sky3,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Sun_1_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky4 = "Sun 1 Village";
        two = new QStandardItem(sub_sky4);
        two->setData(sub_sky4,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Sun_1_village,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky5 = "Sun 1 Out";
        two = new QStandardItem(sub_sky5);
        two->setData(sub_sky5,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Sun_1_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky6 = "Sun 2 In";
        two = new QStandardItem(sub_sky6);
        two->setData(sub_sky6,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Sun_2_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky7 = "Sun 2 Ramp";
        two = new QStandardItem(sub_sky7);
        two->setData(sub_sky7,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Sun_2_ramp,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky8 = "Sun 2 Out";
        two = new QStandardItem(sub_sky8);
        two->setData(sub_sky8,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Sun_2_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky9 = "Sun 3 In";
        two = new QStandardItem(sub_sky9);
        two->setData(sub_sky9,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Sun_3_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky10 = "Sun 3 Out";
        two = new QStandardItem(sub_sky10);
        two->setData(sub_sky10,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Sun_3_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky11 = "Sun 4 In";
        two = new QStandardItem(sub_sky11);
        two->setData(sub_sky11,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Sun_4_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky12 = "Sun 4 Out";
        two = new QStandardItem(sub_sky12);
        two->setData(sub_sky12,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Sun_4_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky13 = "Rain 1 In";
        two = new QStandardItem(sub_sky13);
        two->setData(sub_sky13,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Rain_1_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky14 = "Rain 1 Out";
        two = new QStandardItem(sub_sky14);
        two->setData(sub_sky14,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Rain_1_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky15 = "Rain 2 In";
        two = new QStandardItem(sub_sky15);
        two->setData(sub_sky15,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Rain_2_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky16 = "Rain 2 Out";
        two = new QStandardItem(sub_sky16);
        two->setData(sub_sky16,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Rain_2_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky17 = "Rain 3 In";
        two = new QStandardItem(sub_sky17);
        two->setData(sub_sky17,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Rain_3_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky18 = "Rain 3 Out";
        two = new QStandardItem(sub_sky18);
        two->setData(sub_sky18,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Rain_3_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky19 = "Rain 4 In";
        two = new QStandardItem(sub_sky19);
        two->setData(sub_sky19,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Rain_4_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky20 = "Rain 4 Out";
        two = new QStandardItem(sub_sky20);
        two->setData(sub_sky20,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Rain_4_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky21 = "Rain 5 In";
        two = new QStandardItem(sub_sky21);
        two->setData(sub_sky21,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Rain_5_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky22 = "Rain 5 Out";
        two = new QStandardItem(sub_sky22);
        two->setData(sub_sky22,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Rain_5_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky23 = "Night 1 In";
        two = new QStandardItem(sub_sky23);
        two->setData(sub_sky23,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Night_1_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky24 = "Night 1 Out";
        two = new QStandardItem(sub_sky24);
        two->setData(sub_sky24,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Night_1_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky25 = "Night 2 In";
        two = new QStandardItem(sub_sky25);
        two->setData(sub_sky25,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Night_2_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky26 = "Night 2 Out";
        two = new QStandardItem(sub_sky26);
        two->setData(sub_sky26,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Night_2_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky27 = "Night 3 In";
        two = new QStandardItem(sub_sky27);
        two->setData(sub_sky27,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Night_3_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky28 = "Night 3 Out";
        two = new QStandardItem(sub_sky28);
        two->setData(sub_sky28,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Night_3_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky29 = "Night 4 In";
        two = new QStandardItem(sub_sky29);
        two->setData(sub_sky29,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Night_4_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky30 = "Night 4 Out";
        two = new QStandardItem(sub_sky30);
        two->setData(sub_sky30,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Night_4_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky31 = "Night 5 In";
        two = new QStandardItem(sub_sky31);
        two->setData(sub_sky31,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Night_5_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky32 = "Night 5 Out";
        two = new QStandardItem(sub_sky32);
        two->setData(sub_sky32,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Night_5_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky33 = "Snow 1 In";
        two = new QStandardItem(sub_sky33);
        two->setData(sub_sky33,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Snow_1_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky34 = "Snow 1 Center";
        two = new QStandardItem(sub_sky34);
        two->setData(sub_sky34,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Snow_1_center,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky35 = "Snow 1 Bottom";
        two = new QStandardItem(sub_sky35);
        two->setData(sub_sky35,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Snow_1_bottom,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky36 = "Snow 2 In";
        two = new QStandardItem(sub_sky36);
        two->setData(sub_sky36,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Snow_2_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky37 = "Snow 2 Out";
        two = new QStandardItem(sub_sky37);
        two->setData(sub_sky37,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Snow_2_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky38 = "Snow 3 In";
        two = new QStandardItem(sub_sky38);
        two->setData(sub_sky38,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Snow_3_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky39 = "Snow 3 Out";
        two = new QStandardItem(sub_sky39);
        two->setData(sub_sky39,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Snow_3_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky40 = "Snow 4 In";
        two = new QStandardItem(sub_sky40);
        two->setData(sub_sky40,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Snow_4_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky41 = "Snow 4 Pad";
        two = new QStandardItem(sub_sky41);
        two->setData(sub_sky41,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Snow_4_pad,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky42 = "Snow 4 Out";
        two = new QStandardItem(sub_sky42);
        two->setData(sub_sky42,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Snow_4_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky43 = "Deso 1 In";
        two = new QStandardItem(sub_sky43);
        two->setData(sub_sky43,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Deso_1_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky44 = "Deso 1 Out";
        two = new QStandardItem(sub_sky44);
        two->setData(sub_sky44,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Deso_1_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky45 = "Deso 2 In";
        two = new QStandardItem(sub_sky45);
        two->setData(sub_sky45,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Deso_2_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky46 = "Deso 2 Village";
        two = new QStandardItem(sub_sky46);
        two->setData(sub_sky46,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Deso_2_village,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky47 = "Deso 2 Out";
        two = new QStandardItem(sub_sky47);
        two->setData(sub_sky47,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Deso_2_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky48 = "Deso 3 In";
        two = new QStandardItem(sub_sky48);
        two->setData(sub_sky48,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Deso_3_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky49 = "Deso 3 Out";
        two = new QStandardItem(sub_sky49);
        two->setData(sub_sky49,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Deso_3_out,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky50 = "Deso 4 In";
        two = new QStandardItem(sub_sky50);
        two->setData(sub_sky50,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Deso_4_in,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_sky51 = "Deso 4 Out";
        two = new QStandardItem(sub_sky51);
        two->setData(sub_sky51,Qt::UserRole+1); //Hide Name
        two->setData(0x04,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Deso_4_out,Qt::UserRole+3);
        one->appendRow(two);

        ////////////////////Plane of Sky End////////////////////////////////////////////////

        /////////////////Plane of Disease////////////////////////////////////////
        QString fin7= "Plane of Disease";
        one = new QStandardItem(fin7);
        rootNode->appendRow(one);

        QString sub_zao1 = "Last Home";
        two = new QStandardItem(sub_zao1);
        two->setData(sub_zao1,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Last_Home_tp,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zao2 = "Last Home 2";
        two = new QStandardItem(sub_zao2);
        two->setData(sub_zao2,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Last_Home_2,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zao3 = "Pod 1";
        two = new QStandardItem(sub_zao3);
        two->setData(sub_zao3,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Pod_1,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zao4 = "Pod 2";
        two = new QStandardItem(sub_zao4);
        two->setData(sub_zao4,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Pod_2,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zao5 = "Pod 3";
        two = new QStandardItem(sub_zao5);
        two->setData(sub_zao5,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Pod_3,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zao6 = "Pod 4";
        two = new QStandardItem(sub_zao6);
        two->setData(sub_zao6,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Pod_4,Qt::UserRole+3);
        one->appendRow(two);



        ////////////////////////Isle of Dread///////////////////////////////
        QString fin8 = "Isle of Dread";
        one = new QStandardItem(fin8);
        rootNode->appendRow(one);

        QString sub1_isle1 = "Boat";
        two = new QStandardItem(sub1_isle1);
        two->setData(sub1_isle1,Qt::UserRole+1); //hide name
        two->setData(0x00,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Isle_of_Dread_tp,Qt::UserRole+3);
        one->appendRow(two);


        ////////////////////Zp////////////////////////////////////////////////
        QString fin6 = "Zaorean Plateau";
        one = new QStandardItem(fin6);
        rootNode->appendRow(one);

        QString sub_zp1 = "Zp 1";
        two = new QStandardItem(sub_zp1);
        two->setData(sub_zp1,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Zp_1,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zp2 = "Zp 2";
        two = new QStandardItem(sub_zp2);
        two->setData(sub_zp2,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Zp_2,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zp3 = "Zp 3";
        two = new QStandardItem(sub_zp3);
        two->setData(sub_zp3,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Zp_3,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zp4 = "Zp 4";
        two = new QStandardItem(sub_zp4);
        two->setData(sub_zp4,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Zp_4,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zp5 = "Zp 5";
        two = new QStandardItem(sub_zp5);
        two->setData(sub_zp5,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Zp_5,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zp6 = "Zp 6";
        two = new QStandardItem(sub_zp6);
        two->setData(sub_zp6,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Zp_6,Qt::UserRole+3);
        one->appendRow(two);

        QString sub_zp7 = "Zp 7";
        two = new QStandardItem(sub_zp7);
        two->setData(sub_zp7,Qt::UserRole+1); //Hide Name
        two->setData(0x05,Qt::UserRole+2); //Hide World Data Inside Each One :)
        two->setData(Zp_7,Qt::UserRole+3);
        one->appendRow(two);

        //////////////////Zp End/////////////////////////////////////////////


    }


}





void MainWindow::teleportoff()
{
    timer.stop();
    yourmom = true;
    teleporting = false;
    teleport_destination.clear();
    ui->treeView->setDisabled(false);

    //push a 00c9 and still packet over to stop deadlock, and help fix xyz
    QString mainid5 = "595E0300";
    generator packet5;
    QByteArray holdergen2 = packet5.maintoonswitch(mainid5); //send the id we want to get a 00c9
    holdergen2.insert(0,packetcustom2); //add the header

    packetsize newsize5;
    QByteArray sizeresult2 = newsize5.switchsize(holdergen2); //send to get new size
    holdergen2.replace(8,4,sizeresult2);

    holdergen2.replace(4,4,clientid1);
    holdergen2.replace(0,4,C_9007_servidcount);
    holdergen2.replace(34,16,combine1); //add the F8 response

    increment main_c9a; //get class
    QByteArray maintoonc4 = main_c9a.count(masterserver); //packet increment
    holdergen2.replace(30,4,maintoonc4); //replace new count at 30
    masterserver = maintoonc4.toUpper(); //replace new count
    ui->lineEdit_5->setText(maintoonc4.toUpper());

    //channel zero increment

    increment main_chan02; //get class
    QByteArray maintoonchan02 = main_chan02.count(zerocounter); //zerocounter increment
    holdergen2.replace(54,4,maintoonchan02); //replace new count at 54
    zerocounter = maintoonchan02.toUpper(); //replace new count

    //channel zero increment
    byte1 = holdergen2;
    elcrc(byte1);
    holdergen2.append(outcrc);
    QByteArray custom2out2 = holdergen2.fromHex(holdergen2);
    udpsocket3.writeDatagram(custom2out2, senderheld1, senderPortheld1);
    packetcounter1++;

    QDateTime dateTime_l3 = dateTime_l3.currentDateTime();
    QString dateTimeString_l3 = dateTime_l3.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->appendPlainText("Server-00C9-Teleport-" + dateTimeString_l3);
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->appendPlainText(holdergen2.toUpper());
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->appendPlainText("Server-00C9-Teleport-" + dateTimeString_l3);
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for (int de = 0; de < 8; de++)
    {
        QByteArray maindepacket = despawn_map.value(de); //get first map packet

        QByteArray channelx = maindepacket.mid(34,2); // get first channel number
        QByteArray firstchan = objectpacket::incrementmap.value(QString::fromUtf8(channelx)); //get the value for that channel
        increment no1;
        QByteArray firstinc = no1.count(firstchan); //increment
        objectpacket::incrementmap.insert(QString::fromUtf8(channelx),firstinc); //add the new count back in
        maindepacket.replace(38,4,firstinc); //add it to the packet

        QByteArray channely = maindepacket.mid(304,2); //second channel number
        QByteArray secondchan = objectpacket::incrementmap.value(QString::fromUtf8(channely)); //get the value for that channel
        increment no2;
        QByteArray secondinc = no2.count(secondchan); //increment
        objectpacket::incrementmap.insert(QString::fromUtf8(channely),secondinc); //add the new count back in
        maindepacket.replace(308,4,secondinc); //add it to the packet

        if (maindepacket.size() < 600)
        {
            //do nothing
        }
        else
        {
            QByteArray channelz = maindepacket.mid(572,2); //third channel number
            QByteArray thirdchan = objectpacket::incrementmap.value(QString::fromUtf8(channelz)); //get the value for that channel
            increment no3;
            QByteArray thirdinc = no3.count(thirdchan); //increment
            objectpacket::incrementmap.insert(QString::fromUtf8(channelz),thirdinc); //add the new count back in
            maindepacket.replace(576,4,thirdinc); //add it to the packet
        }


        maindepacket.replace(0,4,C_9007_servidcount);
        maindepacket.replace(4,4,clientid1);

        increment despawn1; //get class
        QByteArray despawncount2 = despawn1.count(masterserver); //masterserver
        maindepacket.replace(30,4,despawncount2); //replace new count at 30
        masterserver = despawncount2.toUpper(); //replace new count
        ui->lineEdit_5->setText(despawncount2.toUpper());

        byte1 = maindepacket;
        elcrc(byte1);
        maindepacket.append(outcrc);
        QByteArray despawnout = maindepacket.fromHex(maindepacket);
        udpsocket3.writeDatagram(despawnout, senderheld1, senderPortheld1);

        QDateTime dateTimet_3 = dateTimet_3.currentDateTime();
        QString dateTimeStringt_3 = dateTimet_3.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Server-Despawn-" + dateTimeStringt_3);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText(maindepacket.toUpper());
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Server-Despawn-" + dateTimeStringt_3);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->moveCursor(QTextCursor::End);
    }
    //////////////////////////////////////////////////////////////////////////////////////////////

    QByteArray dummyfc3 = heartbeatfc;
    dummyfc3.replace(4,4,clientid1);
    dummyfc3.replace(0,4,C_9007_servidcount);

    increment fcsend_3; //get class
    QByteArray fcreply_h = fcsend_3.count(masterserver); //packet increment
    dummyfc3.replace(30,4,fcreply_h); //replace new count at 30
    masterserver = fcreply_h.toUpper(); //replace new count
    ui->lineEdit_5->setText(fcreply_h.toUpper());


    byte1 = dummyfc3;
    elcrc(byte1);
    dummyfc3.append(outcrc);
    QByteArray custom3outfc3 = dummyfc3.fromHex(dummyfc3);
    udpsocket3.writeDatagram(custom3outfc3, senderheld1, senderPortheld1);

    QDateTime dateTime_l6 = dateTime_l6.currentDateTime();
    QString dateTimeString_l6 = dateTime_l6.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->appendPlainText("Server-FC02-Despawn-" + dateTimeString_l6);
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->appendPlainText(dummyfc3.toUpper());
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->appendPlainText("Server-FC02-Despawn-" + dateTimeString_l6);
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->moveCursor(QTextCursor::End);



    ////////////////////////////////////////////////////////////////////////////////////////////////


}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == 2) //0 unchecked, 2 checked
    {

        ui->plainTextEdit->setMaximumBlockCount(200); //200 keeps last 50 packets...can change to higher number if needed
        ui->plainTextEdit_2->setMaximumBlockCount(200);
    }
    if (arg1 == 0)
    {
        ui->plainTextEdit->setMaximumBlockCount(0); //full logging
        ui->plainTextEdit_2->setMaximumBlockCount(0);
    }


}

void MainWindow::spellcooldown()
{


    auto* receivedtimer = qobject_cast<QTimer*>(sender()); //get the pointer of the original sender
    if (timers1.contains(receivedtimer)) //remove from unmemorize all list if it arrives here first if not we crash!!!
    {
        timers1.removeOne(receivedtimer);
    }
    QString  ident = receivedtimer->objectName(); //get the name we set
    receivedtimer->deleteLater(); //delete our pointer so we dont leak from using "new"
    int book_pos = ident.indexOf("%%",0);
    QString new_spell_id = ident.mid(book_pos + 2,2);
    int new_slot = spellslot_map.key(new_spell_id.toInt(nullptr,16));

    switch (new_slot)
    {
    case 0:
        slot_0_trigger = false;
        cooldown_0 = false;
        master_spell_count = spell_0_counter;
        break;
    case 1:
        slot_1_trigger = false;
        cooldown_1 = false;
        master_spell_count = spell_1_counter;
        break;
    case 2:
        slot_2_trigger = false;
        cooldown_2 = false;
        master_spell_count = spell_2_counter;
        break;
    case 3:
        slot_3_trigger = false;
        cooldown_3 = false;
        master_spell_count = spell_3_counter;
        break;
    case 4:
        slot_4_trigger = false;
        cooldown_4 = false;
        master_spell_count = spell_4_counter;
        break;
    case 5:
        slot_5_trigger = false;
        cooldown_5 = false;
        master_spell_count = spell_5_counter;
        break;
    case 6:
        slot_6_trigger = false;
        cooldown_6 = false;
        master_spell_count = spell_6_counter;
        break;
    case 7:
        slot_7_trigger = false;
        cooldown_7 = false;
        master_spell_count = spell_7_counter;
        break;
    case 8:
        slot_8_trigger = false;
        cooldown_8 = false;
        master_spell_count = spell_8_counter;
        break;
    case 9:
        slot_9_trigger = false;
        cooldown_9 = false;
        master_spell_count = spell_9_counter;
        break;
    default:
        break;
    }



    QByteArray ungrey = "D67C5AE794C001BF543213D9BC0D032500aaaaaaaafb094C00CD0000010000000000";
    ungrey.replace(0,4,serverid1);
    ungrey.replace(4,4,clientid1);

    ungrey.replace(34,4,last_opcode_messnum);
    ungrey.replace(38,4,client_opcode);

    increment grayid1; //get class second opcode
    QByteArray graycount2 = grayid1.count(masterserver); //main message
    ungrey.replace(30,4,graycount2); //replace new count at 30
    masterserver = graycount2.toUpper(); //replace new count
    ui->lineEdit_5->setText(graycount2.toUpper());

    increment grayid2; //get class second opcode
    QByteArray graycount3 = grayid2.count(serveropcode1); //server opcode
    ungrey.replace(46,4,graycount3); //replace new count at 30 /46
    serveropcode1 = graycount3.toUpper(); //replace new count
    ui->lineEdit_10->setText(graycount3.toUpper());

    ungrey.replace(54,2,new_spell_id.toUtf8()); //spell id ///check this, pcaps are times two like login
    ungrey.replace(56,4,master_spell_count); //send the same number as the spell u just sent

    byte1 =  ungrey;
    elcrc(byte1);
    ungrey.append(outcrc);
    QByteArray ungreyout = ungrey.fromHex(ungrey);


    QDateTime  dateTimet_2 = dateTimet_2.currentDateTime();
    QString dateTimeStringt_2 = dateTimet_2.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->appendPlainText("Server-Ungrey-X-" + dateTimeStringt_2);
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->appendPlainText(ungrey.toUpper());
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->appendPlainText("Server-Ungrey-X-" + dateTimeStringt_2);
    ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    udpsocket3.writeDatagram(ungreyout, senderheld1, senderPortheld1);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//this is just to stop and ungrey any running spells if you remove all or overwrite
void MainWindow::spellunmemorize() //memorize and unmemorize all both come here, only unmemorize all resets the spellslot map
{


    for (int cc = 0; cc < timers1.size(); cc++) //clear all timers
    {

        auto* receivedtimer = timers1.at(cc); //attempting to stop timers on memorize and unmemorize all

        QString  ident = receivedtimer->objectName(); //get the name we set
        int book_pos = ident.indexOf("%%",0);
        QString new_spell_id2 = ident.mid(book_pos + 2,2);
        int new_slot2 = spellslot_map.key(new_spell_id2.toInt(nullptr,16));

        receivedtimer->stop();
        //      ui->textEdit_4->append("timer attempted stop..." + ident + "...done"); //show to ui
        receivedtimer->deleteLater();
        //            ui->textEdit_4->append("new_slot_2..." + QString::number(new_slot2,10));
        slot_0_trigger = false;     slot_1_trigger = false;   slot_2_trigger = false;     slot_3_trigger = false;   slot_4_trigger = false; //to get the old one thats now something new
        slot_5_trigger = false;     slot_6_trigger = false;   slot_7_trigger = false;     slot_8_trigger = false;   slot_9_trigger = false;


        switch (new_slot2)
        {
        case 0:
            slot_0_trigger = false;
            cooldown_0 = false;
            master_spell_count = spell_0_counter;
            break;
        case 1:
            slot_1_trigger = false;
            cooldown_1 = false;
            master_spell_count = spell_1_counter;
            break;
        case 2:
            slot_2_trigger = false;
            cooldown_2 = false;
            master_spell_count = spell_2_counter;
            break;
        case 3:
            slot_3_trigger = false;
            cooldown_3 = false;
            master_spell_count = spell_3_counter;
            break;
        case 4:
            slot_4_trigger = false;
            cooldown_4 = false;
            master_spell_count = spell_4_counter;
            break;
        case 5:
            slot_5_trigger = false;
            cooldown_5 = false;
            master_spell_count = spell_5_counter;
            break;
        case 6:
            slot_6_trigger = false;
            cooldown_6 = false;
            master_spell_count = spell_6_counter;
            break;
        case 7:
            slot_7_trigger = false;
            cooldown_7 = false;
            master_spell_count = spell_7_counter;
            break;
        case 8:
            slot_8_trigger = false;
            cooldown_8 = false;
            master_spell_count = spell_8_counter;
            break;
        case 9:
            slot_9_trigger = false;
            cooldown_9 = false;
            master_spell_count = spell_9_counter;
            break;
        default:
            break;
        }



        QByteArray ungrey = "D67C5AE794C001BF543213D9BC0D032500aaaaaaaafb094C00CD0000010000000000";
        ungrey.replace(0,4,serverid1);
        ungrey.replace(4,4,clientid1);

        ungrey.replace(34,4,last_opcode_messnum);
        ungrey.replace(38,4,client_opcode);

        increment grayid1; //get class second opcode
        QByteArray graycount2 = grayid1.count(masterserver); //main message
        ungrey.replace(30,4,graycount2); //replace new count at 30
        masterserver = graycount2.toUpper(); //replace new count
        ui->lineEdit_5->setText(graycount2.toUpper());

        increment grayid2; //get class second opcode
        QByteArray graycount3 = grayid2.count(serveropcode1); //server opcode
        ungrey.replace(46,4,graycount3); //replace new count at 30 /46
        serveropcode1 = graycount3.toUpper(); //replace new count
        ui->lineEdit_10->setText(graycount3.toUpper());

        ungrey.replace(54,2,new_spell_id2.toUtf8()); //spell id...///check this..pcap is times two so login numbers
        ungrey.replace(56,4,master_spell_count); //send the same number as the spell u just sent

        byte1 =  ungrey;
        elcrc(byte1);
        ungrey.append(outcrc);
        QByteArray ungreyout = ungrey.fromHex(ungrey);


        QDateTime  dateTimet_2 = dateTimet_2.currentDateTime();
        QString dateTimeStringt_2 = dateTimet_2.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Server-Ungrey-Unmemorize-" + dateTimeStringt_2);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText(ungrey.toUpper());
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->appendPlainText("Server-Ungrey-Unmemorize-" + dateTimeStringt_2);
        ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
        ui->plainTextEdit->moveCursor(QTextCursor::End);
        udpsocket3.writeDatagram(ungreyout, senderheld1, senderPortheld1);
    }
    timers1.clear(); //clear the timer list for unmemorize

    if (unmemorize == true) //clear all spellslots in map
    {
        QMapIterator<int, int>  iter1 (spellslot_map); //clearing all spell slots to an unreachable number
        while (iter1.hasNext())
        {
            iter1.next();
            int curr_key = iter1.key();
            spellslot_map.insert(curr_key,0xFF);
        }
        unmemorize = false; //set the bool back
    }

    return; //try returning from this...should be ok
}



void MainWindow::on_actionReset_Main_Coordinates_triggered()
{

    if (db.isOpen())
    {


        QString tablesel3 = "MAINTOON";
        QString objecttlpt3 = "ObjectID (4)";
        QString xtlpt3 = "X (3)";
        QString ytlpt3 = "Y (3)";
        QString ztlpt3 = "Z (3)";
        QString worldtlpt3 = "World (1)";
        QString tlptid3 = "595E0300";
        QString chosenworld2 = "00";

        QString x2 = "317B17";
        QString y2 = "1EC231";
        QString z2 = "001B10";

        db.transaction();
        QSqlQuery querytlpt3;
        querytlpt3.prepare(QStringLiteral("UPDATE %1 SET [%2] = :val2,[%3] = :val3,[%4] = :val4,[%5] = :val5 WHERE [%6] = :val").arg(tablesel3).arg(xtlpt3).arg(ytlpt3).arg(ztlpt3).arg(worldtlpt3).arg(objecttlpt3));

        querytlpt3.bindValue(":val",tlptid3);
        querytlpt3.bindValue(":val2",x2.toUpper());
        querytlpt3.bindValue(":val3",y2.toUpper());
        querytlpt3.bindValue(":val4",z2.toUpper());
        querytlpt3.bindValue(":val5",chosenworld2.toUpper());

        querytlpt3.exec();
        db.commit();
        ui->textEdit_5->append("Reset Main Coordinates...OK");

    }
    else
    {
        ui->textEdit_5->append("Database not open...reset failed...");
    }


}

void MainWindow::coachingoff()
{
    coachtimer.stop();
    on_pushButton_4_clicked(); //send coaching to teleport after filling teleport_destination
}

void MainWindow::on_actionReset_All_Coaches_triggered()
{
    if (db.isOpen())
    {
        db.transaction();
        QString objectidb1 = "ObjectID (4)";
        QString maintoonb1 = "595E0300";
        QSqlQuery queryb1;
        queryb1.prepare(QStringLiteral("SELECT * FROM COACHES WHERE [%1] = '%2'").arg(objectidb1).arg(maintoonb1));
        queryb1.exec();
        queryb1.next();
        QSqlRecord recordb1 = queryb1.record();

        for (int rr = 1; rr < recordb1.count(); rr++) //skip1 so our first coach value is 1 in the list
        {
            positionlist2.append(recordb1.fieldName(rr));
        }
        positionlist2.replace(0,"NULL");

        for (int rt = 1; rt < positionlist2.count(); rt++)
        {
            QString place1 = positionlist2.value(rt);
            QSqlQuery Queryaa;
            Queryaa.prepare(QString("UPDATE COACHES SET [%1] = :val WHERE [%2] = :val2").arg(place1).arg(objectidb1));
            if (rt % 2) //is odd
            {
                writenum = 1;
            }
            else
            {
                writenum = 0;
            }
            Queryaa.bindValue(":val",writenum);
            Queryaa.bindValue(":val2",maintoonb1);
            Queryaa.exec();

        }
        db.commit();
        ui->textEdit_5->append("Reset All Coaches...OK");
        positionlist2.clear();
    }
    else
    {
        ui->textEdit_5->append("Database not open...reset failed...");
    }
}

void MainWindow::loginpause() //this is backup messages for login in case the client decides to not send an opcode
{
    loginpausing.stop();
    if (!tunare.isEmpty()) //make sure its not empty or we will assert and crash
    {
        if (earlypacket_map.contains(tunare.last())) //load the rest with qmap packet 181-263
        {
            if (client_order.toUpper() == tunare.last().toUpper())
            {
                QByteArray firstslice = client_order.mid(0,4); //client order is always incremented after u get it
                QByteArray secondslice = client_order.mid(4,4);
                increment newearly1;
                QByteArray earlyout1 = newearly1.count(firstslice);
                QByteArray earlyout2 = newearly1.count(secondslice);
                client_order = earlyout1 + earlyout2; //incremented to next num //will work up to 1B001C00


                QByteArray holderpmap2 = earlypacket_map.value(tunare.last()).toUpper();
                qDebug() << "HOLDER" <<holderpmap2;
                qDebug() <<"MESSAGEEND" << messageend;

                holderpmap2.replace(0,4,serverid1);
                holderpmap2.replace(4,4,clientid1);
                QByteArray servernoee = holderpmap2.mid(30,4);
                ui->lineEdit_5->setText(servernoee.toUpper());
                byte1 = holderpmap2;
                elcrc(byte1);
                holderpmap2.append(outcrc);
                QByteArray gameoutmap2 = holderpmap2.fromHex(holderpmap2);
                udpsocket3.writeDatagram(gameoutmap2, senderheld1, senderPortheld1);

                QDateTime dateTime = dateTime.currentDateTime();
                QString dateTimeString = dateTime.toString("MM-dd-yy hh:mm:ss.zzzzzzzzz:a");
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-Packet-Map-Messages-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText(holderpmap2.toUpper());
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->appendPlainText("Server-Packet-Map-Messages-" + dateTimeString);
                ui->plainTextEdit->appendPlainText("_________________________________________________________________________");
                ui->plainTextEdit->moveCursor(QTextCursor::End);
            }
        }
    }
}
