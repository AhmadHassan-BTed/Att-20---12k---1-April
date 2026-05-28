#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QDateTime dateTime = dateTime.currentDateTime();
    QString dateTimeString = dateTime.toString("V.MM.dd.yy.hh.mm.ss");

    w.setWindowTitle("Multi-Server: By Devin Kathman and Dustin Faxon. " + dateTimeString);
    w.show();

    return a.exec();
}
