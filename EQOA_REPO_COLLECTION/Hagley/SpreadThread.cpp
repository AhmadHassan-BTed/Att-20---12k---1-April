#include "SpreadThread.h"
#include "maploading.h"
#include "mainwindow.h"
#include <ActiveQt/qaxobject.h>
#include <ActiveQt/qaxbase.h>

SpreadThread::SpreadThread(QObject *parent) : QObject(parent)
{

}


void SpreadThread::start_Spread_thread()
{
    //qDebug() <<"Started Spread Thread" << thread()->currentThread();
}

void SpreadThread::incoming_Spread() //reads
{
    QThread::currentThread()->setObjectName("Spread_Thread");
    //qDebug() << "ReadExcel" << QThread::currentThread();

    maploading::SpreadSheet_RowitemsVectorMap.clear();
    maploading::Sheet_IntVector.clear();

    QAxObject* excel = new QAxObject("Excel.Application", this);
    QAxObject* workbooks = excel->querySubObject("Workbooks");
    QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", MainWindow::Spreadsheets);
    excel->dynamicCall("SetVisible(bool", false);

    QAxObject* worksheet = workbook->querySubObject("Worksheets(int)", 1);

    QAxObject* usedrange = worksheet->querySubObject("UsedRange");
    QAxObject* rows = usedrange->querySubObject("Rows");
    QAxObject* columns = usedrange->querySubObject("Columns");

    int Cols = columns->property("Count").toInt();
    int Rows = rows->property("Count").toInt();
    int RowStart = usedrange->property("Row").toInt();
    int ColStart = usedrange->property("Columns").toInt();

    emit SheetProgressBarMax1(75120);

    emit TableWidgetRowColumnCounts1(RowStart,Rows,ColStart,Cols);

    //qDebug() << "IN_ReadExcel.intRows" << Rows;
    //qDebug() << "IN_ReadExcel.intCols" << Cols;
    //qDebug() << "IN_ReadExcel.intRowStart" << RowStart;
    //qDebug() << "IN_ReadExcel.intColStart" << ColStart;

    //qDebug() << "Started";

    QVector<QString> ABCvect =
    {
        "A","B","C","D","E","F","G","H","I","J","K","L","M",
        "N","O","P","Q","R","S","T","U","V","W","X","Y","Z",
        "AA","AB","AC","AD","AE","AF","AG","AH","AI","AL","AK","AL","AM",
        "AN","AO","AP","AQ","AR","AS","AT","AU","AV","AW","AX","AY","AZ"
    };

    int count = 0;

    for(int row = 0; row < Rows+1; row++ )
    {
        count++;

        emit SheetProgressBarValue1(count);

        for(int col = 0; col < 11; col++)
        {
            count++;

            emit SheetProgressBarValue1(count);

            QString cellName = ABCvect.at(col);

            QAxObject* cell = worksheet->querySubObject("Cells(int,string)", row+1, cellName);
            cell->setObjectName(cellName);
            QVariant value = cell->dynamicCall("Value()");

            QString Text = value.toString();

            QTableWidgetItem* item = new QTableWidgetItem(Text);

            emit TableWidgetSetItem1(row,col,item);

            if(col != 0)
            {
                emit TableWidgetResizeColumn1(col);
            }
        }
    }

    //qDebug() << "IN_ReadExcel.Finished" << count;

    workbook->dynamicCall("Close");
    excel->dynamicCall("Quit()");

    stop_Spread_thread();
}




void SpreadThread::stop_Spread_thread()
{

    emit finished();
    //thread gets deleted by connect line in mainwindow calling deletelater directly after finished signal
    //qDebug() <<thread()->currentThread() <<"deleted thread in SpreadThread.cpp";


}
