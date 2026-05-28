#include "patchthread.h"

patchthread::patchthread(QObject *parent): QObject(parent)
{

}
void patchthread::start_patch_thread()
{
    qDebug() <<"Started Patch Thread" << thread()->currentThread();
}


void patchthread::listen_patch_socket(QString mainip)
{

    QHostAddress address;
    address.setAddress(mainip);
    server = new QTcpServer();
    server->listen(QHostAddress::Any,7000);
    connect(server,SIGNAL(newConnection()),this,SLOT(acceptconnection()));

    qDebug() <<"listen_patch_socket";

    if(server->isListening())
    {
        qDebug() << "Patch Server started.";
//        server.start(500);
    }
    else
    {
        qDebug() << "PatchServer did not start!";
    }

    address.setAddress(mainip);
    server4 = new QTcpServer();
    server4->listen(QHostAddress::Any,0);


    qDebug() <<"listen_patch_socket";

    if(server4->isListening())
    {
        connect(server4,SIGNAL(newConnection()),this,SLOT(acceptconnection()));
        qDebug() << "Patch Server4 started.";
//        server.start(500);
    }
    else
    {
        qDebug() << "PatchServer4 did not start!";
    }

}

void patchthread::acceptconnection()
{


    socket = server->nextPendingConnection();
    connect(socket,SIGNAL(readyRead()),this,SLOT(incoming_patch()));

    qDebug() <<"acceptconnection";

}

void patchthread::incoming_patch() //reads..this is different than udp since we send the same thing to everyone
{
    QByteArray patchtcp1 = "";
    patchtcp1.resize(socket->bytesAvailable());
    patchtcp1.append(socket->read(socket->bytesAvailable()));
    QHostAddress sender = socket->peerAddress();
    quint16 senderPort = socket->peerPort();

    QString changedatatype = sender.toString();

    if(changedatatype.contains("::ffff:"))
    {
        changedatatype.remove(0,7);
    }

    qDebug() <<"incoming_patch" << "senderIP" << sender << "changedatatype" << changedatatype << "senderPort" << senderPort;
    qDebug() << "outgoing_patch reads" << patchtcp1;

    emit senddata_patch(patchtcp1,changedatatype,senderPort);
}

void patchthread::outgoing_patch(QByteArray serverdata, QString sender, quint16 senderPort) //will prolly take off sender and port for writes since it doesnt use them
{
    QHostAddress address;
    address.setAddress(sender);
    quint16 port = senderPort;

    if (serverdata != "NULL")
    {
        qDebug() <<"outgoing_patch" << "senderIP" << sender << "senderPort" << senderPort;

        socket->write(serverdata); //write for tcpsocket doesnt use ip port cause it spawns a socket for each pendingconnection

        qDebug() << "outgoing_patch writes" << serverdata;
    }
}


void patchthread::stop_patch_thread()
{
    qDebug() <<"stop_patch_thread";

    server->disconnect();
    server->close();
    thread()->quit();
    emit finished();
    //thread gets deleted by connect line in mainwindow calling deletelater directly after finished signal
    qDebug() <<thread()->currentThread() <<"deleted thread in patchthread.cpp";
}
