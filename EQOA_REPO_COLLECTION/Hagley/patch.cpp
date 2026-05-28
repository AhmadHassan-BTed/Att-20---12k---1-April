#include "patch.h"

patch::patch(QObject *parent) : QObject(parent)
{

}


QVector <QByteArray> patch::patch_main(QByteArray patch_input)
{

    QByteArray patches = patch_input.toHex().toUpper(); //make sure its upper before contains (!!udp sockets do tohex in the socket code so u dont have to!!)

    if (patches.contains("70617463682F65716F612F65716F612D66726F6E74696572736C6976652D7570646174652E786D6C2E677A"))
        //get/patch/eqoa/eqoa-frontierslive-update.xml.gz
    {

        QString filename = "/eqoa-frontierslive-update.xml.gz";
        QString directory = QCoreApplication::applicationDirPath();
        directory += filename;
        QFile masterfile(directory);
        if (!masterfile.open(QIODevice::ReadOnly)) //Open File
        {
            QMessageBox msgBox1; //something bad happened
            msgBox1.setText(masterfile.errorString());
            msgBox1.exec();
            QVector <QByteArray> nofile;
            nofile.append("NULL");
            return nofile;
        }

        int sizenew = masterfile.size();
        QByteArray nextsize = QByteArray::number(sizenew,10);

        QByteArray httphead ="HTTP/1.1 200 OK\r\n";
        httphead += "Content-Type: application/octet-stream\r\n";
        httphead += "Content-Length: " + nextsize + "\r\n"; //space after length:
        httphead += "Accept-Ranges: bytes\r\n";
        httphead += "Content-Disposition: attachment; filename=\"eqoa-frontierslive-update.xml.gz\";\r\n\r\n";
        httphead += masterfile.readAll();
        masterfile.close();

        QVector<QByteArray> updatereturn;
        updatereturn.append(httphead);
        updatereturn.append("Patch Done...");

        qDebug() << "httphead" << httphead;
        qDebug() << "Patch Done...";

        return updatereturn;

    }


    if (patches.contains("70617463682F65716F612F66726F6E74696572736C6976652F6368616E6765732F4368616E6765732E747874"))
        //get patch/eqoa/frontierslive/changes/Changes.txt
    {

        QString filename3 = "/Changes.txt";
        QString directory3 = QCoreApplication::applicationDirPath();
        directory3 += filename3;

        QFile masterfile3(directory3);
        if (!masterfile3.open(QIODevice::ReadOnly)) //Open File
        {
            QMessageBox msgBox3; //something bad happened
            msgBox3.setText(masterfile3.errorString());
            msgBox3.exec();
            QVector <QByteArray> nofile;
            nofile.append("NULL");

            qDebug() << "nofile1" << nofile;
            qDebug() << "nofile1 Done...";

            return nofile;
        }

        int sizenew3 = masterfile3.size();
        QByteArray nextsize3 = QByteArray::number(sizenew3,10);

        QByteArray chnghead = "HTTP/1.1 200 OK\r\n";
        chnghead += "Content-Type: text/plain \r\n";
        chnghead += "Content-Length: " + nextsize3 + "\r\n"; //space after length:
        chnghead += "Accept-Ranges: bytes\r\n";
        chnghead += "Content-Disposition: attachment; filename=\"Changes.txt\";\r\n\r\n";
        chnghead += masterfile3.readAll();
        masterfile3.close();

        QVector<QByteArray> updatereturn;
        updatereturn.append(chnghead);
        updatereturn.append("Changes Done...");

        qDebug() << "chnghead" << chnghead;
        qDebug() << "Changes Done...";

        return updatereturn;
    }


    if (patches.contains("70617463682F7765622F65716F612F6D6F74642F6E6577732E68746D6C"))
        //get/patch/web/eqoa/motd/news.html
    {

        QString filename2 = "/news.html";
        QString directory2 = QCoreApplication::applicationDirPath();
        directory2 += filename2;


        QFile masterfile2(directory2);
        if (!masterfile2.open(QIODevice::ReadOnly)) //Open File
        {
            QMessageBox msgBox2; //something bad happened
            msgBox2.setText(masterfile2.errorString());
            msgBox2.exec();
            QVector <QByteArray> nofile;
            nofile.append("NULL");

            qDebug() << "nofile2" << nofile;
            qDebug() << "nofile2 Done...";

            return nofile;
        }

        int sizenew2 = masterfile2.size();
        QByteArray nextsize2 = QByteArray::number(sizenew2,10);

        QByteArray newshead = "HTTP/1.1 200 OK\r\n";
        newshead += "Content-Type: text/html\r\n";
        newshead += "Content-Length: " + nextsize2 + "\r\n"; //space after length:
        newshead += "Accept-Ranges: bytes\r\n";
        newshead += "Content-Disposition: attachment; filename=\"news.html\";\r\n\r\n";
        newshead += masterfile2.readAll();
        masterfile2.close();

        QVector<QByteArray> updatereturn;
        updatereturn.append(newshead);
        updatereturn.append("News Done...");

        qDebug() << "newshead" << newshead;
        qDebug() << "News Done...";

        return updatereturn;

    }

    QVector <QByteArray> null;
    null.append("NULL");

    qDebug() << "nofile3" << "NULL";
    qDebug() << "nofile3 Done...";

    return null;




}
