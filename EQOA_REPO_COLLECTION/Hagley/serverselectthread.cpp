#include "serverselectthread.h"

serverselectthread::serverselectthread(QObject *parent) : QObject(parent)
{

}

void serverselectthread::start_serv_thread()
{
    qDebug() <<"Started Server Select Thread" << thread()->currentThread();
}

void serverselectthread::binding_serv_socket(QString mainip)
{
    udpsocket2 = new QUdpSocket();
    QHostAddress address;
    address.setAddress(mainip);
    udpsocket2->bind(QHostAddress::Any,10070);
    connect(udpsocket2, SIGNAL(readyRead()),this,SLOT(incoming_serv()));
}

void serverselectthread::incoming_serv() //reads
{
    while (udpsocket2->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpsocket2->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpsocket2->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        QByteArray hold = datagram.toHex().toUpper();
        QString changedatatype = sender.toString(); //had to change, signal didnt like hostaddress in native form

        if(changedatatype.contains("::ffff:"))
        {
            changedatatype.remove(0,7);
        }

        qDebug() <<"incoming_server" << "senderIP" << sender << "changedatatype" << changedatatype << "senderPort" << senderPort;
        qDebug() << "server reads" << hold;

        emit senddata_serv(hold,changedatatype,senderPort);

    }
}

void serverselectthread::outgoing_serv(QByteArray serverdata,QString sender,quint16 senderPort) //writes
{
    QHostAddress newsender = QHostAddress(sender);
    if (serverdata != "NULL")
    {
        udpsocket2->writeDatagram(serverdata,newsender,senderPort);

        qDebug() <<"outgoing_server" << "senderIP" << sender << "senderPort" << senderPort;

        qDebug() << "server writes" << serverdata;
    }
}


void serverselectthread::stop_serv_thread()
{
    udpsocket2->close();
    thread()->quit();
    emit finished();
    //thread gets deleted by connect line in mainwindow calling deletelater directly after finished signal
    qDebug() <<thread()->currentThread() <<"deleted thread in serverselectthread.cpp";
}


