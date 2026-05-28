#ifndef LOGIN_H
#define LOGIN_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QDebug>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QtCore>
#include <QTcpSocket>

class login : public QObject
{
    Q_OBJECT
public:
    explicit login(QObject *parent = nullptr);




    QString directory = QCoreApplication::applicationDirPath();

    QString Database = "/Master_Database.db";

    QSqlDatabase db = QSqlDatabase::database();



    const QByteArray login_packet = "000000A8000000250000000000000000000000000000000000"
                                    "00000000000000000000000000000000000000000000000000"
                                    "00000000000000000000000000000000000000000000000000"
                                    "000000000026C15D5A00000003001E7F4F001E7F4F716C4432"
                                    "36636878726B53314A74677600000000000000000000000000"
                                    "00000000000000000000000000000000000000000000000000"
                                    "000000000000000000000000000100000003";


signals:

public slots:
    QVector <QByteArray> login_main(QByteArray ClientsIP,QByteArray ClientsPort,QByteArray ClientsPacket);

};

#endif // LOGIN_H
