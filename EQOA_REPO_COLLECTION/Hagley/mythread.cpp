#include "mythread.h"
#include <QtCore/qglobal.h>

MyThread::MyThread(QObject *parent) :
    QThread(parent)
{

}

void MyThread::run()
{
    //Thread starts here
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::DirectConnection);
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);

    qDebug() << socketDescriptor << " Client Connected";

    exec();
}

void MyThread::readyRead()
{
    qDebug() << "";
    qDebug() << socketDescriptor << " Incomming from client";

    QByteArray patchtcp1 = "";
    patchtcp1.resize(socket->bytesAvailable());
    patchtcp1.append(socket->read(socket->bytesAvailable()));
    QByteArray hold3 = patchtcp1.toHex().toUpper();

    qDebug() << "patchtcp1" << hold3;
    QByteArray middle2 = hold3.mid(208,20);
    QByteArray check1 = middle2.toUpper();

    if(check1 == "474554202F7061746368") //GET PATCH Request
    {

        QByteArray httphead ="HTTP/1.1 200 OK\r\n";
        httphead += "Content-Type: application/octet-stream\r\n";
        httphead += "Content-Length: 145\r\n";
        httphead += "Accept-Ranges: bytes\r\n";
        httphead += "Content-Disposition: attachment; filename=\"eqoa-frontierslive-update.xml.gz\";\r\n\r\n";

        QString filename = "/eqoa-frontierslive-update.xml.gz";
        QString directory = QCoreApplication::applicationDirPath();
        directory += filename;
        //qDebug() <<"file" <<directory;


        QFile masterfile(directory);
        if(!masterfile.open(QIODevice::ReadOnly)) //Open File
        {
            QMessageBox msgBox1; //something bad happened
            msgBox1.setText(masterfile.errorString());
            msgBox1.exec();
            qDebug() << masterfile.errorString();
            return;
        }


        httphead += masterfile.readAll();
        masterfile.close();
        //socket->write(httphead); //write the data out
        qDebug() << "httphead" <<httphead;
    }


    QByteArray middle3 = hold3.mid(220,28);
    QByteArray motd1 = middle3.toUpper();
    //qDebug() << "motd1" << motd1;

    if(motd1 == "6D6F74642F6E6577732E68746D6C") //motd news
    {

        QByteArray newshead = "HTTP/1.1 200 OK\r\n";
        newshead += "Content-Type: text/html\r\n";
        newshead += "Content-Length: 43\r\n";
        newshead += "Accept-Ranges: bytes\r\n";
        newshead += "Content-Disposition: attachment; filename=\"news.html\";\r\n\r\n";
        QString filename2 = "/news.html";
        QString directory2 = QCoreApplication::applicationDirPath();
        directory2 += filename2;
        //qDebug() <<"file2" << directory2;

        QFile masterfile2(directory2);
        if(!masterfile2.open(QIODevice::ReadOnly)) //Open File
        {
            QMessageBox msgBox2; //something bad happened
            msgBox2.setText(masterfile2.errorString());
            msgBox2.exec();
            //qDebug() << masterfile2.errorString();
            return;
        }


        newshead += masterfile2.readAll();
        masterfile2.close();
        //socket->write(newshead);
        qDebug() << "newshead" <<newshead;
    }
}

void MyThread::disconnected()
{
    socket->deleteLater();
    exit(0);
}



