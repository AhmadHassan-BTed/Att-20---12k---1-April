#ifndef CONVERT_H
#define CONVERT_H

#include <QObject>
#include <QDebug>

class Convert : public QObject
{
    Q_OBJECT
public:
    explicit Convert(QObject *parent = nullptr);

signals:

public slots:
    QString ConvertTo(QString Dec_value);
    QString Bytes4to5(QString fourBytes);

private:
    long manipulate1 = 0;
    long manipulate2 = 0;
    long manipulate3 = 0;
    long manipulate4 = 0;
    long manipulate5 = 0;
    QString theresult = "";
    QByteArray Plus1 = "";


};

#endif // CONVERT_H
