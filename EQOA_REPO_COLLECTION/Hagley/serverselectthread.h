#ifndef SERVERSELECTTHREAD_H
#define SERVERSELECTTHREAD_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QByteArray>
#include <QHostAddress>
#include <QThread>

class serverselectthread : public QObject
{
    Q_OBJECT
public:
    explicit serverselectthread(QObject *parent = nullptr);
    QUdpSocket *udpsocket2;

signals:
    void finished();
    void senddata_serv(QByteArray clientdata, QString sender,quint16 senderPort);

public slots:
    void start_serv_thread();
    void incoming_serv();
    void outgoing_serv(QByteArray serverdata,QString sender,quint16 senderPort);
    void stop_serv_thread();
    void binding_serv_socket(QString mainip);


};

#endif // SERVERSELECTTHREAD_H
