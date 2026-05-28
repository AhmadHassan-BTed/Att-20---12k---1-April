#ifndef LOGINTHREAD_H
#define LOGINTHREAD_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QHostAddress>
#include <QThread>

class loginthread : public QObject
{
    Q_OBJECT
public:
    explicit loginthread(QObject *parent = nullptr);
    QTcpServer *server2;
    QTcpSocket *socket2;

signals:
    void finished();
    void senddata_login(QByteArray clientdata, QString sender,quint16 senderPort);

public slots:
    void start_login_thread();
    void incoming_login();
    void outgoing_login(QByteArray serverdata,QString sender,quint16 senderPort);
    void stop_login_thread();
    void listen_login_socket(QString mainip);
    void acceptconnection2();
};

#endif // LOGINTHREAD_H
