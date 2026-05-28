#include "loginthread.h"

loginthread::loginthread(QObject *parent) : QObject(parent)
{

}


void loginthread::start_login_thread()
{
    qDebug() <<"Started Login Thread" << thread()->currentThread();
}

void loginthread::listen_login_socket(QString mainip)
{
    qDebug() <<"listen_login_socket";

    QHostAddress address;
    address.setAddress(mainip);
    server2 = new QTcpServer();
    server2->listen(QHostAddress::Any,9735);
    connect(server2, SIGNAL(newConnection()), this, SLOT(acceptconnection2()));

}

void loginthread::acceptconnection2()
{
    qDebug() <<"acceptconnection2";

    socket2 = server2->nextPendingConnection();
    connect(socket2,SIGNAL(readyRead()),this,SLOT(incoming_login()));

}

void loginthread::incoming_login() //reads..this is different than udp since we send the same thing to everyone
{
    QByteArray login1 = "";
    login1.resize(socket2->bytesAvailable());
    login1.append(socket2->read(socket2->bytesAvailable()));
    QHostAddress sender = socket2->peerAddress();

    quint16 senderPort = socket2->peerPort();

    QString changedatatype = sender.toString();

    if(changedatatype.contains("::ffff:"))
    {
        changedatatype.remove(0,7);
    }

    qDebug() <<"incoming_login" << "senderIP" << sender << "changedatatype" << changedatatype << "senderPort" << senderPort;
    qDebug() << "incoming_login reads" << login1;


    emit senddata_login(login1,changedatatype,senderPort);
}

void loginthread::outgoing_login(QByteArray serverdata, QString sender, quint16 senderPort) //will prolly take off sender and port for writes since it doesnt use them
{
    QHostAddress address;
    address.setAddress(sender);
    if (serverdata != "NULL")
    {
        qDebug() <<"outgoing_login" << "senderIP" << sender << "senderPort" << senderPort;

        socket2->write(serverdata); //write for tcpsocket doesnt use ip port cause it spawns a socket for each pendingconnection

        qDebug() << "outgoing_login writes" << serverdata;
    }
}


void loginthread::stop_login_thread()
{
    qDebug() <<"stop_login_thread";

    server2->disconnect();
    server2->close();
    thread()->quit();
    emit finished();
    //thread gets deleted by connect line in mainwindow calling deletelater directly after finished signal
    qDebug() <<thread()->currentThread() <<"deleted thread in loginthread.cpp";
}





