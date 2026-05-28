#include "worldthread.h"


worldthread::worldthread(QObject *parent) : QObject(parent)
{

}


void worldthread::start_world_thread()
{

    qDebug() <<"Started World Thread" << thread()->currentThread();

}

void worldthread::binding_world_socket(QString mainip)
{
    udpsocket3 = new QUdpSocket();
    QHostAddress address;
    address.setAddress(mainip);
    udpsocket3->bind(QHostAddress::Any,10071);
    connect(udpsocket3, SIGNAL(readyRead()),this,SLOT(incoming_world()));


}

void worldthread::incoming_world() //reads
{


    while (udpsocket3->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpsocket3->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpsocket3->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        QByteArray hold = datagram.toHex().toUpper();
        QString changedatatype = sender.toString(); //had to change, signal didnt like hostaddress in native form

        if(changedatatype.contains("::ffff:"))
        {
            changedatatype.remove(0,7);
        }

        qDebug() <<"incoming_world" << "senderIP" << sender << "changedatatype" << changedatatype << "senderPort" << senderPort;
        qDebug() << "incoming_world reads" << hold;
        emit senddata_world(hold,changedatatype,senderPort);

    }

}

void worldthread::outgoing_world(QByteArray serverdata,QString sender,quint16 senderPort) //writes
{
    QHostAddress newsender = QHostAddress(sender);
    if (serverdata != "NULL")
    {
        qDebug() <<"outgoing_world" << "senderIP" << sender << "senderPort" << senderPort;

        udpsocket3->writeDatagram(serverdata,newsender,senderPort);
        qDebug() << "outgoing_world writes" << serverdata;
    }
}

void worldthread::stop_world_thread()
{
    udpsocket3->close();
    thread()->quit();
    emit finished();
    //thread gets deleted by connect line in mainwindow calling deletelater directly after finished signal
    qDebug() <<thread()->currentThread() <<"deleted thread in worldthread.cpp";
}
