#include "dnsthread.h"

dnsthread::dnsthread(QObject *parent) : QObject(parent)
{

}


void dnsthread::start_dns_thread()
{
    qDebug() <<"Started DNS Thread" << thread()->currentThread();
}

void dnsthread::binding_dns_socket(QString mainip)
{
    udpsocket = new QUdpSocket();
    QHostAddress address;
    address.setAddress(mainip);
    udpsocket->bind(QHostAddress::Any,53);
    connect(udpsocket, SIGNAL(readyRead()),this,SLOT(incoming_dns()));

    qDebug() <<"binding_dns_socket";

}

void dnsthread::incoming_dns() //reads
{
    while (udpsocket->hasPendingDatagrams())
    {

        QByteArray datagram;
        datagram.resize(udpsocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpsocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);




        QByteArray hold = datagram.toHex().toUpper();
        QString changedatatype = sender.toString(); //had to change, signal didnt like hostaddress in native form

        if(changedatatype.contains("::ffff:"))
        {
            changedatatype.remove(0,7);
        }

        emit senddata_dns(hold,changedatatype,senderPort);

        qDebug() <<"incoming_dns" << "senderIP" << sender << "changedatatype" << changedatatype << "senderPort" << senderPort;
        qDebug() << "incoming_dns reads" << hold;

    }
}


void dnsthread::outgoing_dns(QByteArray serverdata,QString sender,quint16 senderPort) //writes
{
    QHostAddress newsender = QHostAddress(sender);
    if (serverdata != "NULL")
    {
        qDebug() <<"outgoing_dns" << "senderIP" << sender << "senderPort" << senderPort;

        udpsocket->writeDatagram(serverdata,newsender,senderPort);
        qDebug() << "outgoing_dns writes" << serverdata;
    }
}


void dnsthread::stop_dns_thread()
{

    udpsocket->close();
    thread()->quit();
    emit finished();
    //thread gets deleted by connect line in mainwindow calling deletelater directly after finished signal
    qDebug() <<thread()->currentThread() <<"deleted thread in dnsthread.cpp";


}
