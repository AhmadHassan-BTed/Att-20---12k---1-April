#ifndef PATCH_H
#define PATCH_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QVector>
#include <QCoreApplication>
#include <QMessageBox>

class patch : public QObject
{
    Q_OBJECT
public:
    explicit patch(QObject *parent = nullptr);

signals:

public slots:
    QVector <QByteArray> patch_main(QByteArray patch_input);

};

#endif // PATCH_H
