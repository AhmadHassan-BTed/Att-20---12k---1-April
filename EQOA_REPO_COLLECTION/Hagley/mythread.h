#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QtCore>
#include <QMessageBox>
#include <QHostAddress>
#include <QRunnable>
#include <QThreadPool>

class MyThread : public QThread, public QRunnable
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
    void run();

    QTcpSocket *socket;
    int socketDescriptor;

    QHostAddress PublicMainip;
    QHostAddress mainip;
    QByteArray ClientsID = "";
    QByteArray ClientsIP = "";
    QByteArray ClientsPort = "";
    QByteArray ClientsPacket = "";

signals:
    void error(QTcpSocket::SocketError socketerror);



public slots:
    void readyRead();
    void disconnected();


};

#endif // MYTHREAD_H
