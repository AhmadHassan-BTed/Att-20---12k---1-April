#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QObject>
#include <QUdpSocket>
#include <QString>
#include <QByteArray>
#include <QHostAddress>
#include <QThread>

class worldthread : public QObject
{
    Q_OBJECT
public:
    explicit worldthread(QObject *parent = nullptr);
    static QUdpSocket *udpsocket3;

private:

signals:
    void finished();
    void senddata_world(QByteArray clientdata, QString sender,quint16 senderPort);

public slots:

    void start_world_thread();
    void incoming_world();
    void outgoing_world(QByteArray serverdata,QString sender,quint16 senderPort);
    void stop_world_thread();
    void binding_world_socket(QString mainip);
};

#endif // WORKERTHREAD_H
