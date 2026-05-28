#ifndef INCREMENT_H
#define INCREMENT_H

#include <QObject>
#include<QString>
#include<QByteArray>

class increment : public QObject
{
    Q_OBJECT
public:
    explicit increment(QObject *parent = 0);

signals:

public slots:
    QByteArray count(QByteArray holdingcount);

};

#endif // INCREMENT_H
