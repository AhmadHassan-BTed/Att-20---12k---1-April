#include "login.h"
#include "opcodes.h"
#include "packetparsing.h"
#include <QSqlError>

bool packetparsing::SkipKey = false;

login::login(QObject *parent) : QObject(parent)
{

}

QVector <QByteArray> login::login_main(QByteArray ClientsIP, QByteArray ClientsPort, QByteArray ClientsPacket)
{
    qDebug() << "";
    qDebug() << "";
    qDebug() << "Inside login_main";

    if(db.isOpen())
    {
        db.close();
        db.removeDatabase(QSqlDatabase::defaultConnection);
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(Opcodes::m_FileName);
    db.open();



    packetparsing::packetvars IN_llogin;

    if(packetparsing::IPandPort_AllvariablesMap.contains(ClientsIP))
    {
        qDebug() << "Already in";

        IN_llogin = packetparsing::IPandPort_AllvariablesMap.value(ClientsIP);
    }
    else
    {
        qDebug() << "Not in yet";

        IN_llogin = packetparsing::pX;
    }



    IN_llogin.CharSelectID = ClientsIP;

    qDebug() << "IN_llogin.CharSelectID" << IN_llogin.CharSelectID;



    QVector <QByteArray> loginvector; //this is just returning the login packet back to mainwindow...change later to your code
    QByteArray hold4 = ClientsPacket.toHex().toUpper(); //client packet...not used for now
    QByteArray logrsp = "";
    QString PasswordDB = "";
    QString Column_Name = "";
    int packetid = 0;

    //removes the extra 00's---------------------------------
    QByteArray User_Name2 = hold4.mid(328,64);
    QString textout = User_Name2.fromHex(User_Name2);
    QByteArray User_Name = textout.toUtf8().toHex().toUpper();
    //removes the extra 00's---------------------------------

    IN_llogin.Pass_Word = hold4.mid(392,16);

    qDebug() << "IN_llogin.Pass_Word" << IN_llogin.Pass_Word;

    if(User_Name.isEmpty() == false)
    {
        IN_llogin.UserName = User_Name;
    }

    qDebug() << "IN_llogin.UserName" << IN_llogin.UserName;

    if(Opcodes::UserName_PasswordMap.contains(IN_llogin.UserName))
    {
        PasswordDB = Opcodes::UserName_PasswordMap.value(IN_llogin.UserName);
    }


    if(PasswordDB.isEmpty() == true)
    {
        PasswordDB = "XXXXXXXX";// so it dont match IN_llogin.Pass_Word and go into the wrong if statement
    }

    qDebug() << "PasswordDB" << PasswordDB;

    IN_llogin.NewAccount1 = hold4.mid(956,20);

    qDebug() << "IN_llogin.NewAccount1" << IN_llogin.NewAccount1;

    QVector <QString> NewAccountKeysVector;
    QByteArray Activate = hold4.mid(160,8);

    qDebug() << "Activate" << Activate;



    if(PasswordDB == IN_llogin.Pass_Word && IN_llogin.NewAccount1 != "01DC0000003800000000" && IN_llogin.AccountKeyReply != "Finished")//Normal Login
    {
        if(User_Name != "")
        {
            IN_llogin.UserName = User_Name;
        }

        logrsp = login_packet.toUpper();
        QByteArray logdone = logrsp.fromHex(logrsp);

        loginvector.append(logdone);
        loginvector.append
                (
                    "Login Normal.. Done... "
                    "User_Name " + IN_llogin.UserName.toUtf8() +
                    " PasswordDB " + PasswordDB.toUtf8() +
                    " IN_llogin.Pass_Word " + IN_llogin.Pass_Word +
                    " NewAccount1 " + IN_llogin.NewAccount1 +
                    " Activate " + Activate +
                    " AccountKeyReply " + IN_llogin.AccountKeyReply +
                    " hold4 " + hold4);
    }

    if(PasswordDB != IN_llogin.Pass_Word && IN_llogin.NewAccount1 != "01DC0000003800000000" && Activate != "45514F41" && IN_llogin.AccountKeyReply != "Finished")//No Login Account found
    {
        logrsp = login_packet.toUpper();

        logrsp.replace(174,2,"04");

        QByteArray logdone = logrsp.fromHex(logrsp);

        loginvector.append(logdone);
        loginvector.append("Login No Account.. Error4... "
                           "UserName " + IN_llogin.UserName.toUtf8() +
                           " PasswordDB " + PasswordDB.toUtf8() +
                           " IN_llogin.Pass_Word " + IN_llogin.Pass_Word +
                           " NewAccount1 " + IN_llogin.NewAccount1 +
                           " Activate " + Activate +
                           " AccountKeyReply " + IN_llogin.AccountKeyReply +
                           " hold4 " + hold4);
    }


    if(IN_llogin.NewAccount1 == "01DC0000003800000000" && PasswordDB != IN_llogin.Pass_Word)//New Account
    {
        qDebug() << "Inside New Account";

        QByteArray User_Name = hold4.mid(976,64);
        QString textout = User_Name.fromHex(User_Name);
        User_Name = textout.toUtf8().toHex().toUpper();
        IN_llogin.UserName = User_Name;
        qDebug() << "IN_llogin.UserName" << IN_llogin.UserName;
        IN_llogin.PassWord = hold4.mid(1040,16);
        qDebug() << "IN_llogin.PassWord" << IN_llogin.PassWord;
        IN_llogin.Pass_Word = hold4.mid(1040,16);
        qDebug() << "IN_llogin.Pass_Word" << IN_llogin.Pass_Word;
        logrsp = login_packet.toUpper();

        logrsp.replace(174,2,"02");

        IN_llogin.AccountKeyReply = "Finished";

        qDebug() << "IN_llogin.AccountKeyReply" << IN_llogin.AccountKeyReply;

        QByteArray logdone = logrsp.fromHex(logrsp);

        loginvector.append(logdone);
        loginvector.append("Login Account Key Used... NewAccount1 " +
                           IN_llogin.NewAccount1 +
                           " Activate " + Activate +
                           " AccountKeyReply " + IN_llogin.AccountKeyReply +
                           " hold4 " + hold4);



        Opcodes::UserName_PasswordMap.insert(IN_llogin.UserName,IN_llogin.PassWord);

    }

    QString AccountKey = hold4.mid(192,-1);

    qDebug() << "AccountKey" << AccountKey;

    if(Activate == "45514F41" && IN_llogin.AccountKeyReply == "Finished")
    {
        qDebug() << "Inside Account db update";


        db.transaction();
        QString Table_Name = "ACCOUNT_KEYS";
        QSqlQuery query;
        query.prepare(QString("SELECT * FROM %1").arg(Table_Name));
        query.exec();

        while(query.next())
        {
            QString NewACCountKeys = query.value(0).toString();
            NewAccountKeysVector.append(NewACCountKeys);
        }

        db.commit();

        if(NewAccountKeysVector.contains(AccountKey) || packetparsing::SkipKey == true)
        {
            qDebug() << "packetparsing::SkipKey" << packetparsing::SkipKey;

            int index = NewAccountKeysVector.indexOf(AccountKey);

            qDebug() << "index" << index;

            if(index != -1)
            {
                NewAccountKeysVector.removeAt(index);
            }

            packetparsing::SkipKey = false;


            db.transaction();

            if(index != -1)
            {
                QSqlQuery query0;
                query0.prepare(QString("DELETE FROM ACCOUNT_KEYS WHERE KEY = :val"));
                query0.bindValue(":val",AccountKey);
                query0.exec();

                logrsp = login_packet.toUpper();
                QByteArray logdone = logrsp.fromHex(logrsp);

                loginvector.append(logdone);
                loginvector.append("Login Done.. Key Consumed... Activate " + Activate + " AccountKeyReply " + IN_llogin.AccountKeyReply + " hold4 " + hold4);
            }

            QSqlQuery query1 ("SELECT COUNT (*) FROM ACCOUNTS");

            while(query1.next())
            {
                packetid = query1.value(0).toInt();
            }


            packetid++;

            Table_Name = "ACCOUNTS";
            Column_Name = "PacketID";
            query.prepare(QString("INSERT OR IGNORE INTO [%1] ([%2])""VALUES(:id)").arg(Table_Name).arg(Column_Name));
            query.bindValue(":id",packetid);
            query.exec();

            if (!query.exec())
            {
                loginvector.append("Fail_ACCOUNTSX = " + IN_llogin.CharSelectID.toUtf8());
                return loginvector;
            }

            Column_Name = "USERNAME";
            query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
            query.bindValue(":val",IN_llogin.UserName);
            query.bindValue(":id",packetid);
            query.exec();

            if (!query.exec())
            {
                loginvector.append("Fail_ACCOUNTS_USERNAME = " + IN_llogin.CharSelectID.toUtf8());
                return loginvector;
            }

            Column_Name = "PASSWORD";
            query.prepare(QString("UPDATE [%1] SET [%2] = :val WHERE ROWID = :id").arg(Table_Name).arg(Column_Name));
            query.bindValue(":val",IN_llogin.PassWord);
            query.bindValue(":id",packetid);
            query.exec();

            if (!query.exec())
            {
                loginvector.append("Fail_ACCOUNTS_PASSWORD = " + IN_llogin.CharSelectID.toUtf8());
                return loginvector;
            }

            IN_llogin.AccountKeyReply = "";
            db.commit();

        }
        else
        {
            qDebug() << "Inside Else Login Error";

            QString Vkey = "";
            if(NewAccountKeysVector.isEmpty() == false)
            {
                Vkey = NewAccountKeysVector.at(0);
            }
            logrsp = login_packet.toUpper();

            logrsp.replace(174,2,"04");

            QByteArray logdone = logrsp.fromHex(logrsp);

            loginvector.append(logdone);
            loginvector.append("Login Error... Activate " +
                               Activate +
                               " Vkey " + Vkey.toUtf8() +
                               " AccountKeyReply " + IN_llogin.AccountKeyReply +
                               " AccountKey " + AccountKey.toUtf8() +
                               " hold4 " + hold4);

        }
    }

    qDebug() << "IN_llogin.CharSelectID" << IN_llogin.CharSelectID;
    qDebug() << "";

    packetparsing::IPandPort_AllvariablesMap.insert(ClientsIP,IN_llogin);

    return loginvector;

}
