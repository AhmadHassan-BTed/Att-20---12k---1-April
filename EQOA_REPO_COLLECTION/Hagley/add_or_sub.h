#ifndef ADD_OR_SUB_H
#define ADD_OR_SUB_H

#include <QObject>
#include <QDebug>

class ADD_OR_SUB : public QObject
{
    Q_OBJECT
public:
    explicit ADD_OR_SUB(QObject *parent = nullptr);




signals:

public slots:

    QString Withdraw(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString Currency1, QByteArray Servers_Last_FB_MessageNumber);
    QString Deposit(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString Currency2, QByteArray Servers_Last_FB_MessageNumber);
    QString AddtoWallet(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QString Currency3,QByteArray Servers_Last_FB_MessageNumber);
    QString SubfromWallet(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort, QString Currency4, QByteArray Servers_Last_FB_MessageNumber);
    QString SubforNeg(QString Neg_val);
    QString TextSize(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QString CurrentText, bool Trimm,QString Opcode,QByteArray Servers_Last_FB_MessageNumber);
    QString FBSize(QString CurrentFB);
    QString ConvertFormat(int Randomizer);
    QString BackToDec(QString Old_Val);
    QByteArray TextSize1(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QString CurrentText, QByteArray serveropcode, QByteArray Red_Or_White_Text);

    QString TextSizeNo_00s(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort,QString CurrentText,QString Opcode);

    float DistanceCheck(QByteArray MasterX, QByteArray MasterZ, QByteArray MasterY, QByteArray PLayerX, QByteArray PLayerZ, QByteArray PLayerY);
private slots:
private:
    QString Total_Dec2 = "";
    QByteArray First_Lootcheck1 = "fb50870b7a0a";
    QByteArray Text_Size = "";
    const QByteArray You_receive  = "59006f007500200072006500630065006900760065002000";
          QByteArray tunar_amount = "";
    const QByteArray tunar_from_the_corpse = "2000740075006e00610072002000660072006f006d002000740068006500200063006f0072007000730065002e00";

    QByteArray OpcodeColor = "";
    QByteArray FBText = "";

    QVector<QByteArray>opvect = {"12","0100","0400","0A00","0B00","0D00","0E00","0F00","1000","1300","1400","1500","1600",
                                 "1800","1900","1A00","1D00","2000","2100","2B00","2C00","2D00","3200","3300","3400","3500","3A00","3B00",
                                 "3C00","3D00","3E00","3F00","4000","4100","4600","4900","4A00","4B00","4C00","5200","5600",
                                 "5700","6000","6100","6200","6300","6C00","6D00","7300","7C00","7D00","7E00","7F00","B100",
                                 "B400","B700","C500","C700","C800","C900","CA00","CD00","CE00","D700","D800","D900","DB00",
                                 "EC00","6603","6903","D503","6804","7805","7905","2206","2506","9E06","6007","6107","7207",
                                 "7307","7607","7807","9007","9107","A307","B007","B307","D007","D107",
                                 "F507","F607","F707","F807","FE00",
                                 "0108","4008","0409","B009","7A0A","7B0A","A10A","C40A","010D","410D","010E","020E","0010",
                                 "0510","0111","0512","0612","4D12","5012","5112","5312","5412","5512","0114","0214","0115"};

};

#endif // ADD_OR_SUB_H
