#ifndef DNSTHREAD_H
#define DNSTHREAD_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QByteArray>
#include <QHostAddress>
#include <QThread>

class dnsthread : public QObject
{
    Q_OBJECT
public:
    explicit dnsthread(QObject *parent = nullptr);
    QUdpSocket *udpsocket;

signals:
    void finished();
    void senddata_dns(QByteArray clientdata, QString sender,quint16 senderPort);

public slots:
    void start_dns_thread();
    void incoming_dns();
    void outgoing_dns(QByteArray serverdata,QString sender,quint16 senderPort);
    void stop_dns_thread();
    void binding_dns_socket(QString mainip);
};

#endif // DNSTHREAD_H
