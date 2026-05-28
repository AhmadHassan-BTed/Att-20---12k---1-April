#ifndef SPREADTHREAD_H
#define SPREADTHREAD_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QThread>
#include <QTableWidgetItem>

class SpreadThread : public QObject
{
    Q_OBJECT
public:
    explicit SpreadThread(QObject *parent = nullptr);

signals:
    void finished();
    void SheetProgressBarMax1(int Rows);
    void SheetProgressBarValue1(int Value);
    void TableWidgetRowColumnCounts1(int RowStart,int Rows,int ColStart,int Cols);
    void TableWidgetSetItem1(int row, int column, QTableWidgetItem*item);
    void TableWidgetResizeColumn1(int column);




public slots:
    void start_Spread_thread();
    void incoming_Spread();
    void stop_Spread_thread();
};

#endif // SPREADTHREAD_H
