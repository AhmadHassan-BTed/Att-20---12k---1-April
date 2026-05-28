#include "crc.h"

crc::crc(QObject *parent) : QObject(parent)
{

}

QByteArray crc::elcrc(QByteArray byte1)
{

    quint32 crc32 = 0;
    qint64 i = 0;
    QByteArray test1 = byte1.fromHex(byte1);

    crc32 = 0xFFFFFFFF;

    for (i = 0; i < test1.size(); i++)
    {
        crc32 = (crc32 >> 8) ^ crc32_tab[(crc32 & 0xff) ^ (test1.at(i) & 0xff)];
    }


    crc32 ^= 0x11F19ED3;

    QString hexvalue = QString("%1").arg(crc32, 8, 16, QLatin1Char( '0' ));
    QByteArray originalcrc = hexvalue.toUtf8();
    QByteArray first =  originalcrc.mid(0,2);
    QByteArray second = originalcrc.mid(2,2);
    QByteArray third =  originalcrc.mid(4,2);
    QByteArray fourth = originalcrc.mid(6,2);
    QByteArray outcrc = fourth+= third += second += first;
    return outcrc.toUpper();

}
