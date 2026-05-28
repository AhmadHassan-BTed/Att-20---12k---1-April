#ifndef GENERATOR_H
#define GENERATOR_H

#include <QObject>
#include <QtSql>
#include <QSqlQuery>
#include <QByteArray>
#include <QDebug>
#include <QVector>
#include <QSqlRecord>
#include <QMap>
#include <QStringList>
#include <QString>

class generator : public QObject
{
    Q_OBJECT
public:
    explicit generator(QObject *parent = 0);
    QSqlDatabase db = QSqlDatabase::database();
    QByteArray maintoonarray = "";
    QString output2 = "";
    QByteArray encryptpacket2 = "";
    static QMap<QByteArray,QByteArray> Players_Map;


public slots:
    QByteArray maintoonswitch(QString mainid, QString Change1F, QString CharSelectID_Main);



private:
    QByteArray encom4 = "";
    int enhigh2 = 0;
    int enlow2 = 0;
    int holdit2 = 0;
    QByteArray hold5 = "";
    int encarat3 = 0;
    QByteArray encom5 = "";
    QByteArray encom6 = "";
    QByteArray hold6 = "";
    int zerocounter2 = 0;


signals:




};

#endif // GENERATOR_H
