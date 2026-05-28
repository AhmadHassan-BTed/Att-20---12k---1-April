#ifndef PATCHTHREAD_H
#define PATCHTHREAD_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QHostAddress>
#include <QThread>

class patchthread : public QObject
{
    Q_OBJECT
public:
    explicit patchthread(QObject* parent = nullptr);
    QTcpServer *server;
    QTcpSocket *socket;
    QTcpServer *server4;
    //qintptr m_descriptor;

signals:
    void finished();
    void senddata_patch(QByteArray clientdata, QString sender,quint16 senderPort);

public slots:
    void start_patch_thread();
    void incoming_patch();
    void outgoing_patch(QByteArray serverdata,QString sender,quint16 senderPort);
    void stop_patch_thread();
    void listen_patch_socket(QString mainip);
    void acceptconnection();

    //protected:
    //   void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
};

#endif // PATCHTHREAD_H
