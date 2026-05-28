#ifndef MYSERVER_H
#define MYSERVER_H

#include <QObject>

class MyServer : public QObject
{
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = nullptr);

signals:

};

#endif // MYSERVER_H
