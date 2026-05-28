#ifndef DNS_H
#define DNS_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QDebug>
#include <QHostAddress>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>

class dns : public QObject
{
    Q_OBJECT
public:
    explicit dns(QObject *parent = nullptr);

    const QByteArray gatedns = "8180000100010000000005676174653102757304646e61730b706c617973746174696f6e036f72670000010001c00c00010001000002560004";
    const QByteArray patchdns = "81800001000100000000057061746368196576657271756573746F6E6C696E65616476656E747572657303636F6D0000010001c00c00010001000002560004";


    QSqlDatabase db = QSqlDatabase::database();

signals:

public slots:
    QVector <QByteArray> dns_main(QByteArray dns_input,QString mainip,QString sender,QByteArray senderPort);
};

#endif // DNS_H
