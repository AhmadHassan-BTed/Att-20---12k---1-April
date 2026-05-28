#include "increment.h"



increment::increment(QObject *parent) : QObject(parent)
{

}

QByteArray increment::count(QByteArray holdingcount)
{

    QByteArray pullt4 = holdingcount; //get current number to increment
    QByteArray frontt4 = pullt4.mid(0,2);
    QByteArray backt4 = pullt4.mid(2,2);
    QByteArray topt4 = QString("%1").arg(frontt4.toInt(0,16) + 1,2,16, QLatin1Char( '0' )).toUtf8(); //increment
    QByteArray bottomt4 = QString("%1").arg(backt4.toInt(0,16),2,16,QLatin1Char('0')).toUtf8();

    if (topt4 == "100")
    {
    topt4 = "00";
    bottomt4 = QString("%1").arg(backt4.toInt(0,16) + 1,2,16,QLatin1Char('0')).toUtf8(); //increment
    }

    if (bottomt4 == "100")
    {
    bottomt4 = "00";
    }

    QByteArray result_t4 = topt4 += bottomt4;
    return result_t4.toUpper(); //return new count


}
