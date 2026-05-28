#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QByteArray>
#include <QDebug>
#include <QHostAddress>
#include <QString>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QtCore>
#include <QObject>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QVector>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QStringList>
#include <QMap>
#include <QMapIterator>
#include <QTextEdit>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPixmap>
#include <QTabBar>
#include <QStack>
#include <QTreeView>
#include <QStandardItemModel>
#include <QModelIndexList>
#include <QStandardItem>
#include <QDateTime>
#include <QTimer>
#include <QtConcurrent>
#include <QTreeWidgetItem>
#include <QThread>
#include <QSqlError>
#include "packetparsing.h"
#include "maploading.h"
#include<QListWidgetItem>
#include <QtSql/QtSql>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    maploading *test = new maploading;
//    maploading *test2 = new maploading;

    void UpdateChannelMaps(QByteArray ClientID, QByteArray ClientsIP, QByteArray ClientsPort);

    static bool SetTableSize;
    static QString Spreadsheets;
    static QVector<QString> UiOputVector;
    static QVector <QString> StopMovementVector;
    static QHostAddress PublicMainip;
    static QByteArray MastersClientsID;
    static QByteArray MastersClientsIP;
    static QByteArray MastersClientsPort;
    static QByteArray MastersClientsObjectID;
    static QMap<QString,QString> MobsAggroTargetMap;
    static QString News;
    static QVector <QTimer*> OutgoingTimersVector;
    static QVector <QTimer*> CheckForAggroMovementTimersVector;
    static QVector <QTimer*> GroupTimersVector;
    static QVector <QTimer*> TeleportTimersVector;
    static QVector <QTimer*> CoachTimersVector;
    static QVector <QTimer*> StandingStillTimersVector;
    static QVector <QTimer*> UpdateMapsTimersVector;
    static QVector <QTimer*> LoS_TimersVector;
    static QVector <QTimer*> MySwingTimersVector;
    static QVector <QTimer*> MobSwingTimersVector;
    static QVector <QTimer*> SpawningTimersVector;
    static QVector <QTimer*> reSpawningTimersVector;
    static QVector <QTimer*> RoamingTimersVector;
    static QVector <QTimer*> RegenTimersVector;
    static QVector <QTimer*> reSend42ecTimersVector;
    static QVector <QTimer*> CheckForAggroTimersVector;
    static QVector <QTimer*> reSendFBsTimersVector;
    static QVector <QTimer*> reSendChannelsTimersVector;
    static QVector <QTimer*> StopAllMovementsTimersVector;
    static QVector <QTimer*> ReviveTimersVector;
    static QVector <QTimer*> StandUpTimersVector;
    static QVector <QTimer*> ModelTimersVector;



    bool trip = false;

    int mapCount2 = 0;

    QString SliderUsed = "YES";
    QByteArray byte2;
    QSqlDatabase db;
    QModelIndex index2;
    QSqlDatabase data_b;

    QSqlTableModel  *model;
    QSqlTableModel  *model1;
    QSqlTableModel  *model2;
    QSqlTableModel  *model3;
    QSqlTableModel  *model4;
    QSqlTableModel  *model5;
    QSqlTableModel  *model6;
    QSqlTableModel  *model7;
    QSqlTableModel  *model8;
    QSqlTableModel  *model9;
    QSqlTableModel  *model10;
    QSqlTableModel  *model11;
    QSqlTableModel  *model16;
    QSqlTableModel  *model17;
    QSqlTableModel  *model18;
    QSqlTableModel  *model19;
    QSqlTableModel  *model22;
    QSqlTableModel  *model25;
    QSqlTableModel  *model26;


    QString m_FileName = "";
    static QHostAddress sender3;
    static quint16 senderPort3;
    static int First_Packet_Merchant;


signals:
void MWSheetStart();

    void outgoingdata_dns(QByteArray serverdata,QString sender,quint16 senderPort);
    void outgoingdata_login(QByteArray serverdata,QString sender,quint16 senderPort);
    void outgoingdata_patch(QByteArray serverdata,QString sender,quint16 senderPort);
    void outgoingdata_game1(QByteArray serverdata,QString sender,quint16 senderPort);
    void outgoingdata_game2(QByteArray serverdata,QString sender,quint16 senderPort);

    void stopworldthread();
    void bind_world_socket(QString mainip);

    void stopdnsthread();
    void bind_dns_socket(QString mainip);

    void stopservthread();
    void bind_serv_socket(QString mainip);

    void stoppatchthread();
    void listen_patch_socket(QString mainip);

    void stoploginthread();
    void listen_login_socket(QString mainip);


private slots:


    void MWSheetProgressBarMax1(int Rows);
    void MWSheetProgressBarValue1(int Value);
    void MWTableWidgetRowColumnCounts1(int RowStart,int Rows,int ColStart,int Cols);
    void MWTableWidgetSetItem1(int row, int column, QTableWidgetItem* item);
    void MWTableWidgetResizeColumn1(int column);



    void world_servercall();
    void dns_servercall();
    void serv_servercall();
    void AggroCheck();
    void patch_servercall();
    void login_servercall();

    void gameread2(QByteArray clientdata, QString sender,quint16 senderPort);

    void gameread1(QByteArray clientdata, QString sender,quint16 senderPort);

    void loginread(QByteArray clientdata,QString sender, quint16 senderPort);

    void dnsread(QByteArray clientdata, QString sender,quint16 senderPort);
    void patchread(QByteArray clientdata, QString sender,quint16 senderPort);


    QByteArray GenerateXP(QString ID_IP_PORT);

    void KillYourSelf(QString ID_IP_PORT);


    QVector <int> CheckTheZoneHeight();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();


    void on_actionAbout_Hagley2_triggered();

    void on_actionLoad_Database_triggered();

    void on_pushButton_4_clicked();

    void on_treeView_clicked(const QModelIndex &index);

    void on_tabWidget_currentChanged(int index);

    void teleportoff();

    void coachingoff();

    void spellcooldown();

    void spellunmemorize();

    void on_actionReset_Main_Coordinates_triggered();

    void on_actionReset_All_Coaches_triggered();




    void on_comboBox_currentIndexChanged(int index);

    void on_horizontalSlider_valueChanged(int Index);
    void on_horizontalSlider_2_valueChanged(int Index);
    void on_horizontalSlider_3_valueChanged(int Index);
    void on_horizontalSlider_4_valueChanged(int Index);
    void on_pushButton_49_clicked();
    void on_pushButton_50_clicked();
    void on_pushButton_51_clicked();
    void on_pushButton_52_clicked();

    void on_pushButton_53_clicked();

    void on_pushButton_54_clicked();

    void on_pushButton_62_clicked();

    void on_pushButton_46_clicked();

    void on_pushButton_North_clicked();

    void on_pushButton_West_clicked();

    void on_pushButton_East_clicked();

    void on_pushButton_South_clicked();

    void on_Z_Minus_clicked();

    void on_Z_Plus_clicked();

    void on_dial_valueChanged(int value);

    void on_spinBox_valueChanged(int arg1);

    void on_lineEdit_133_textChanged(const QString &arg1);

    void on_Selection_currentRowChanged(int currentRow);

    void on_comboBox_35_currentIndexChanged(int index);

    void on_comboBox_8_currentIndexChanged(int index);

    void on_comboBox_6_currentIndexChanged(int index);

    void on_pushButton_7_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_12_clicked();

    void on_comboBox_34_currentIndexChanged(int index);

    void on_lineEdit_9_textChanged(const QString &arg1);

    void on_pushButton_13_clicked();

    void on_lineEdit_NPC_Name_textChanged(const QString &arg1);

    void on_pushButton_61_clicked();

    void on_pushButton_47_clicked();

    void on_pushButton_48_clicked();

    void on_pushButton_76_clicked();

    void on_checkBox_7_stateChanged(int arg1);

    void on_checkBox_6_stateChanged(int arg1);


    void on_lineEdit_68_textChanged(const QString &arg1);

    void on_lineEdit_75_textChanged(const QString &arg1);

    void on_lineEdit_78_textChanged(const QString &arg1);

    void on_lineEdit_67_textChanged(const QString &arg1);

    void on_lineEdit_77_textChanged(const QString &arg1);

    void on_lineEdit_82_textChanged(const QString &arg1);

    void on_pushButton_56_clicked();

    void on_pushButton_57_clicked();

    void on_pushButton_58_clicked();

    void on_pushButton_59_clicked();

    void on_pushButton_60_clicked();

    void on_pushButton_63_clicked();

    void on_pushButton_64_clicked();

    void on_pushButton_31_clicked();

    void on_pushButton_28_clicked();

    void on_pushButton_65_clicked();

    void on_radioButton_33_clicked();

    void on_radioButton_34_clicked();

    void on_radioButton_35_clicked();

    void on_radioButton_37_clicked();

    void on_radioButton_38_clicked();

    void on_radioButton_36_clicked();

    void on_radioButton_39_clicked();

    void on_radioButton_40_clicked();

    void on_radioButton_41_clicked();

    void on_pushButton_66_clicked();

    void on_pushButton_67_clicked();

    void on_pushButton_68_clicked();

    void on_pushButton_69_clicked();

    void on_pushButton_70_clicked();

    void on_pushButton_71_clicked();

    void on_pushButton_77_clicked();

    void on_pushButton_78_clicked();

    void on_lineEdit_34_textChanged(const QString &arg1);

    void on_lineEdit_28_textChanged(const QString &arg1);

    void on_lineEdit_52_textChanged(const QString &arg1);

    void on_lineEdit_76_textChanged(const QString &arg1);

    void on_lineEdit_32_textChanged(const QString &arg1);

    void on_lineEdit_41_textChanged(const QString &arg1);

    void on_checkBox_9_stateChanged(int arg1);

    void on_pushButton_80_clicked();

    void on_lineEdit_NPC_Name2_textChanged(const QString &arg1);

    void on_Mob_Models_3_textChanged(const QString &arg1);

    void on_pushButton_14_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_lineEdit_81_textChanged(const QString &arg1);
    void on_pushButton_15_clicked();

    void on_lineEdit_26_textChanged(const QString &arg1);

    void on_pushButton_16_clicked();

    void on_lineEdit_44_textChanged(const QString &arg1);

    void on_pushButton_17_clicked();

    void on_tableView_5_clicked(const QModelIndex &index);

    void on_tableView_6_clicked(const QModelIndex &index);

    void on_NPC_Location_PB_3_clicked();

    void on_pushButton_73_clicked();

    void on_AS_Name_Search_PB_2_clicked();

    void on_pushButton_18_clicked();

    void on_tableView_8_clicked(const QModelIndex &index);

    void on_tableView_7_clicked(const QModelIndex &index);

    void on_pushButton_74_clicked();

    void on_AS_Name_Search_PB_3_clicked();

    void on_pushButton_20_clicked();

    void on_lineEdit_NPC_Name_2_textChanged(const QString &arg1);

    void on_lineEdit_72_textChanged(const QString &arg1);

    void on_pushButton_23_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_24_clicked();

    void on_pushButton_25_clicked();

    void on_pushButton_26_clicked();

    void on_pushButton_27_clicked();

    void on_comboBox_38_currentIndexChanged(int index);

    void on_comboBox_39_currentIndexChanged(int index);

    void on_tabWidget_4_currentChanged();

    void on_lineEdit_92_textChanged(const QString &arg1);

    void on_lineEdit_91_textChanged(const QString &arg1);

    void on_lineEdit_65_textChanged(const QString &arg1);

    void on_lineEdit_69_textChanged(const QString &arg1);

    void on_lineEdit_8_textChanged(const QString &arg1);

    void on_lineEdit_25_textChanged(const QString &arg1);

    void on_Name_Search_LE_3_textChanged(const QString &arg1);

    void on_Name_Search_LE_2_textChanged(const QString &arg1);

    void on_checkBox_5_stateChanged(int arg1);

    void on_checkBox_14_stateChanged(int arg1);

    void on_pushButton_32_clicked();

    void on_pushButton_34_clicked();

    void on_pushButton_33_clicked();

    void on_pushButton_35_clicked();

    void on_pushButton_36_clicked();

    void on_pushButton_38_clicked();

    void on_pushButton_39_clicked();

    void on_pushButton_41_clicked();

    void on_comboBox_40_currentIndexChanged(const QString &arg1);

    void on_lineEdit_104_textChanged(const QString &arg1);

    void on_lineEdit_105_textChanged(const QString &arg1);

    void on_lineEdit_103_textChanged(const QString &arg1);

    void on_pushButton_43_clicked();

    void on_dial_3_valueChanged(int value);

    void on_dial_2_valueChanged(int value);

    void on_pushButton_44_clicked();

    void on_pushButton_45_clicked();

    void on_pushButton_72_clicked();

    void on_pushButton_55_clicked();

    void on_dial_5_valueChanged(int value);

    void on_dial_4_valueChanged(int value);

    void on_pushButton_79_clicked();

    void on_pushButton_83_clicked();

    void on_pushButton_82_clicked();

    void on_pushButton_75_clicked();

    void Mob_DMG_Sent();

    void My_DMG_Sent();
    void on_StandingStill();

    void on_pushButton_88_clicked();
    void on_pushButton_89_clicked();

    void on_pushButton_90_clicked();

    void on_pushButton_91_clicked();

    void on_pushButton_92_clicked();


    void on_pushButton_93_clicked();

    void on_pushButton_94_clicked();


    void on_Regenerate();

    void on_DespawnMob();

    void On_Create_Pet();

    void on_lineEdit_153_textChanged(const QString &arg1);

    void Activate_Roaming();

    void on_pushButton_97_clicked();

    void on_pushButton_98_clicked();

    void on_pushButton_99_clicked();

    void on_Print_XZY_clicked();

    void on_RemoveMarker_clicked();

    void on_SetCurrentRoamerID_clicked();

    void on_pushButton_100_clicked();

    void on_UseThisPath_stateChanged(int arg1);

    void on_pushButton_101_clicked();

    void on_pushButton_102_clicked();

    void on_RemoveALLMarkers_clicked();

    void on_pushButton_103_clicked();

    void on_CheckForMobs_clicked();

    void on_UseThisPath_2_stateChanged(int arg1);

    void on_pushButton_104_clicked();

    void on_pushButton_105_clicked();

    void on_pushButton_106_clicked();

    void on_pushButton_107_clicked();

    void on_lineEdit_73_textChanged(const QString &arg1);

    void on_pushButton_110_clicked();

    void on_pushButton_111_clicked();

    void on_horizontalSlider_5_valueChanged(int value);

    void on_pushButton_113_clicked();

    void on_pushButton_112_clicked();

    void on_pushButton_114_clicked();

    void on_pushButton_115_clicked();

    void on_verticalSlider_valueChanged(int value);

    void on_horizontalSlider_6_valueChanged(int value);

    void on_verticalSlider_2_valueChanged(int value);

    void on_verticalSlider_3_valueChanged(int value);

    void on_verticalSlider_4_valueChanged(int value);

    void on_pushButton_228_clicked();
    void on_pushButton_229_clicked();
    void on_pushButton_230_clicked();
    void on_pushButton_231_clicked();
    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_81_clicked();

    void on_pushButton_116_clicked();

    void on_verticalSlider_5_valueChanged(int value);

    void on_horizontalSlider_7_valueChanged(int value);
    void on_verticalSlider_6_valueChanged(int value);

    void on_pushButton_117_clicked();
    void on_pushButton_118_clicked();
    void on_pushButton_119_clicked();
    void on_pushButton_120_clicked();

    QByteArray SendOutLoS(QPixmap currentImage, QByteArray NPCID,
                          QByteArray AGGRO_Starting_X, QByteArray AGGRO_Starting_Z, QByteArray AGGRO_Starting_Y,
                          QString LoS_Counter_X, QString LoS_Counter_Z, QString LoS_Counter_Y);

    void on_pushButton_121_clicked();

    void on_pushButton_122_clicked();

    void on_pushButton_123_clicked();

    void on_pushButton_124_clicked();


    void on_lineEdit_160_textChanged(const QString &arg1);

    void on_pushButton_126_clicked();

    void on_lineEdit_130_textChanged(const QString &arg1);

    void on_pushButton_127_clicked();

    void on_pushButton_128_clicked();

    void on_pushButton_129_clicked();

    void on_pushButton_108_clicked();

    void on_pushButton_95_clicked();

    void on_pushButton_130_clicked();


    void on_pushButton_84_clicked();
    void Resending42ECs();
    void ResendingFBs();




    void AggroMovement();

    void on_pushButton_42_clicked();

    void on_pushButton_87_clicked();

    QByteArray CheckForLoS(QByteArray NPCID);

    void on_pushButton_86_clicked();

    void on_pushButton_133_clicked();

    void on_pushButton_131_clicked();

    void on_pushButton_132_clicked();

    void on_pushButton_134_clicked();

    void on_pushButton_85_clicked();

    void on_pushButton_135_clicked();

    void on_pushButton_136_clicked();


    void on_X_4_textChanged(const QString &arg1);

    void on_Y_4_textChanged(const QString &arg1);
    void on_pushButton_144_clicked();

    void on_checkBox_15_stateChanged(int arg1);
    void on_pushButton_137_clicked();

    void on_pushButton_Set_Z1_clicked();
    void on_pushButton_Set_Z2_clicked();
    void on_pushButton_Set_Z3_clicked();
    void on_pushButton_Set_Z4_clicked();
    void on_pushButton_Set_Z5_clicked();
    void on_pushButton_Set_Z6_clicked();
    void on_pushButton_Set_Z7_clicked();






    void on_pushButton_North_GZ_clicked();
    void on_pushButton_West_GZ_clicked();
    void on_pushButton_East_GZ_clicked();
    void on_pushButton_South_GZ_clicked();

    void on_pushButton_North_GZ_2_clicked();
    void on_pushButton_West_GZ_2_clicked();
    void on_pushButton_East_GZ_2_clicked();
    void on_pushButton_South_GZ_2_clicked();

    void on_pushButton_North_L1_clicked();
    void on_pushButton_West_L1_clicked();
    void on_pushButton_East_L1_clicked();
    void on_pushButton_South_L1_clicked();

    void on_pushButton_North_L2_clicked();
    void on_pushButton_West_L2_clicked();
    void on_pushButton_East_L2_clicked();
    void on_pushButton_South_L2_clicked();

    void on_pushButton_North_L3_clicked();
    void on_pushButton_West_L3_clicked();
    void on_pushButton_East_L3_clicked();
    void on_pushButton_South_L3_clicked();

    void on_pushButton_North_L4_clicked();
    void on_pushButton_West_L4_clicked();
    void on_pushButton_East_L4_clicked();
    void on_pushButton_South_L4_clicked();

    void on_pushButton_North_L5_clicked();
    void on_pushButton_West_L5_clicked();
    void on_pushButton_East_L5_clicked();
    void on_pushButton_South_L5_clicked();

    void on_pushButton_North_L6_clicked();
    void on_pushButton_West_L6_clicked();
    void on_pushButton_East_L6_clicked();
    void on_pushButton_South_L6_clicked();

    void on_pushButton_North_L7_clicked();
    void on_pushButton_West_L7_clicked();
    void on_pushButton_East_L7_clicked();
    void on_pushButton_South_L7_clicked();



    void on_pushButton_Ground_Zero_Z_clicked();
    void on_pushButton_Ground_Zero_Z_2_clicked();

    void on_pushButton_29_clicked();

    void on_RespawnMob();


    void on_pushButton_30_clicked();
    void on_pushButton_138_clicked();



    void on_checkBox_13_stateChanged(int arg1);

    void ResendingChannels();
    void StopAllMovement();

    void on_pushButton_141_clicked();



    void on_pushButton_149_clicked();

    void on_pushButton_150_clicked();

    void on_pushButton_151_clicked();

    void on_lineEdit_150_textChanged(const QString &arg1);

    void on_A_pushButton_39_clicked();
    void on_A_pushButton_40_clicked();
    void on_A_pushButton_29_clicked();
    void on_A_pushButton_33_clicked();
    void on_A_pushButton_30_clicked();
    void on_A_pushButton_23_clicked();
    void on_A_pushButton_36_clicked();
    void on_A_pushButton_34_clicked();
    void on_A_pushButton_32_clicked();
    void on_A_pushButton_35_clicked();
    void on_A_pushButton_26_clicked();
    void on_A_pushButton_28_clicked();
    void on_A_pushButton_25_clicked();
    void on_A_pushButton_48_clicked();
    void on_A_pushButton_24_clicked();
    void on_A_lineEdit_102_textChanged(const QString &arg1);
    void on_A_lineEdit_103_textChanged(const QString &arg1);
    void on_A_lineEdit_104_textChanged(const QString &arg1);
    void on_A_lineEdit_105_textChanged(const QString &arg1);
    void on_A_lineEdit_106_textChanged(const QString &arg1);
    void on_A_lineEdit_107_textChanged(const QString &arg1);
    void on_A_lineEdit_108_textChanged(const QString &arg1);
    void on_A_lineEdit_109_textChanged(const QString &arg1);
    void on_A_lineEdit_110_textChanged(const QString &arg1);
    void on_A_lineEdit_111_textChanged(const QString &arg1);
    void on_A_lineEdit_112_textChanged(const QString &arg1);
    void on_A_lineEdit_113_textChanged(const QString &arg1);
    void on_A_lineEdit_114_textChanged(const QString &arg1);
    void on_A_lineEdit_115_textChanged(const QString &arg1);
    void on_A_lineEdit_116_textChanged(const QString &arg1);
    void on_A_pushButton_22_clicked();
    void on_A_pushButton_27_clicked();
    void on_A_pushButton_63_clicked();
    void on_A_pushButton_64_clicked();
    void on_A_pushButton_65_clicked();
    void on_A_pushButton_66_clicked();
    void on_A_pushButton_67_clicked();
    void on_A_pushButton_60_clicked();
    void on_A_pushButton_50_clicked();
    void on_A_pushButton_52_clicked();
    void on_A_pushButton_58_clicked();
    void on_A_pushButton_51_clicked();
    void on_A_pushButton_61_clicked();
    void on_A_pushButton_57_clicked();
    void on_A_pushButton_55_clicked();
    void on_A_pushButton_49_clicked();
    void on_A_pushButton_59_clicked();
    void on_A_pushButton_56_clicked();
    void on_A_pushButton_53_clicked();
    void on_A_lineEdit_classBox1_2_textChanged(const QString &arg1);
    void on_A_lineEdit_classBox2_2_textChanged(const QString &arg1);
    void on_A_lineEdit_classBox3_2_textChanged(const QString &arg1);
    void on_A_lineEdit_classBox4_2_textChanged(const QString &arg1);
    void on_A_lineEdit_classBox5_2_textChanged(const QString &arg1);
    void on_A_lineEdit_classBox6_2_textChanged(const QString &arg1);
    void on_A_lineEdit_classBox7_2_textChanged(const QString &arg1);
    void on_A_lineEdit_classBox8_2_textChanged(const QString &arg1);
    void on_A_lineEdit_classBox9_textChanged(const QString &arg1);
    void on_A_lineEdit_classBox10_textChanged(const QString &arg1);



    void on_A_pushButton_31_clicked();
    void on_A_pushButton_62_clicked();

    void on_lineEdit_206_textChanged(const QString &arg1);

    void on_A_tableView_5_clicked(const QModelIndex &index);

    void on_lineEdit_224_textChanged(const QString &arg1);

    void on_pushButton_109_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_lineEdit_151_textChanged(const QString &arg1);

    void on_pushButton_96_clicked();





    void on_pushButton_155_clicked();

    void on_lineEdit_162_textChanged(const QString &arg1);

    void on_lineEdit_230_textChanged(const QString &arg1);

    void on_lineEdit_231_textChanged(const QString &arg1);

    void on_lineEdit_232_textChanged(const QString &arg1);

    void on_lineEdit_233_textChanged(const QString &arg1);

    void on_lineEdit_234_textChanged(const QString &arg1);


    void on_tableView_3_clicked(const QModelIndex &index);

    void on_pushButton_157_clicked();

    void on_pushButton_158_clicked();

    void on_pushButton_164_clicked();

    void on_pushButton_165_clicked();

    void on_pushButton_172_clicked();

    void on_pushButton_173_clicked();






    void on_pushButton_176_clicked();

    void on_pushButton_152_clicked();

    void ReviveYourSelf();
    void StandUp();
    void on_pushButton_177_clicked();

    void on_pushButton_178_clicked();

    void on_lineEdit_248_textChanged(const QString &arg1);

    void on_lineEdit_249_textChanged(const QString &arg1);

    void on_pushButton_179_clicked();

    QByteArray ConvertSession(QString SessionID1);
    void on_pushButton_181_clicked();

    void on_pushButton_180_clicked();



    void on_lineEdit_164_textChanged(const QString &arg1);




    void on_tableView_4_clicked(const QModelIndex &index);

    void on_tableView_9_clicked(const QModelIndex &index);

    void on_pushButton_191_clicked();

    void on_pushButton_166_clicked();

    void on_checkBox_22_stateChanged(int arg1);

    void on_checkBox_23_stateChanged(int arg1);

    void on_checkBox_25_stateChanged(int arg1);

    void on_checkBox_26_stateChanged(int arg1);

    void on_checkBox_28_stateChanged(int arg1);

    void on_checkBox_27_stateChanged(int arg1);

    void on_tableView_10_clicked(const QModelIndex &index);

    void on_pushButton_168_clicked();

    void on_lineEdit_236_textChanged(const QString &arg1);

    void on_pushButton_187_clicked();

    void on_pushButton_167_clicked();

    void on_pushButton_156_clicked();

    void on_pushButton_159_clicked();

    void on_pushButton_160_clicked();


    void on_Icon_ItemsID_5_textChanged(const QString &arg1);

    void on_Icon_ItemsID_6_textChanged(const QString &arg1);

    void on_lineEdit_240_textChanged(const QString &arg1);

    void on_tableView_11_clicked(const QModelIndex &index);

    void on_Icon_tableView_5_clicked(const QModelIndex &index);

    void on_Icon_lineEdit_Items_Color_4_textChanged(const QString &arg1);
    void on_Icon_lineEdit_Item_Name_4_textChanged(const QString &arg1);

    void on_checkBox_38_stateChanged(int arg1);

    void on_checkBox_39_stateChanged(int arg1);

    void on_checkBox_37_stateChanged(int arg1);

    void on_checkBox_36_stateChanged(int arg1);

    void on_checkBox_30_stateChanged(int arg1);

    void on_checkBox_29_stateChanged(int arg1);

    void on_pushButton_161_clicked();

    void on_checkBox_32_stateChanged(int arg1);

    void on_checkBox_33_stateChanged(int arg1);

    void on_checkBox_34_stateChanged(int arg1);

    void on_textEdit_30_textChanged();



    void on_pushButton_162_clicked();

    void on_pushButton_163_clicked();

    void mapprogress(int value);
    void on_pushButton_373_clicked();

    void on_tableView_16_clicked(const QModelIndex &index);

    void on_lineEdit_295_textChanged(const QString &arg1);

    void on_pushButton_193_clicked();

    void on_pushButton_192_clicked();

    void on_pushButton_194_clicked();

    void on_checkBox_53_stateChanged(int arg1);

    void on_checkBox_42_stateChanged(int arg1);


    void on_pushButton_169_clicked();

    void on_checkBox_55_stateChanged(int arg1);

    void on_pushButton_197_clicked();

    void on_pushButton_196_clicked();

    void on_lineEdit_202_textChanged(const QString &arg1);

    void on_lineEdit_201_textChanged(const QString &arg1);

    void on_pushButton_195_clicked();

    void on_tableView_17_clicked(const QModelIndex &index);


    void on_lineEdit_225_textChanged(const QString &arg1);

    void on_lineEdit_204_textChanged(const QString &arg1);

    void on_lineEdit_301_textChanged(const QString &arg1);

    void on_lineEdit_300_textChanged(const QString &arg1);

    void on_lineEdit_237_textChanged(const QString &arg1);

    void on_pushButton_198_clicked();

    void on_checkBox_57_stateChanged(int arg1);

    void on_pushButton_170_clicked();

    void on_lineEdit_275_textChanged(const QString &arg1);

    void on_pushButton_153_clicked();


    void on_lineEdit_297_textChanged(const QString &arg1);

    void on_lineEdit_111_textChanged(const QString &arg1);

    void on_tableView_26_clicked(const QModelIndex &index);

    void on_lineEdit_123_textChanged(const QString &arg1);

    void on_pushButton_3_clicked();

    void on_pushButton_139_clicked();

    void on_lineEdit_347_textChanged(const QString &arg1);

    void on_lineEdit_59_textChanged(const QString &arg1);

    void on_pushButton_140_clicked();

    void on_pushButton_154_clicked();

    void on_lineEdit_348_textChanged(const QString &arg1);

    void on_textEdit_28_textChanged();



    void on_pushButton_171_clicked();

    void on_pushButton_174_clicked();

    void on_pushButton_175_clicked();

    void on_checkBox_44_stateChanged(int arg1);

    void on_checkBox_45_stateChanged(int arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_tableView_18_clicked(const QModelIndex &index);

    void on_tableView_2_clicked(const QModelIndex &index);

    void on_lineEdit_207_textChanged(const QString &arg1);

    void on_checkBox_46_stateChanged(int arg1);

    void on_lineEdit_246_textChanged(const QString &arg1);

    void on_tableView_12_clicked(const QModelIndex &index);


    void on_pushButton_184_clicked();

    void on_pushButton_186_clicked();

    void on_pushButton_188_clicked();

    void on_pushButton_189_clicked();

    void on_lineEdit_AggroTimer_textChanged(const QString &arg1);

    void on_spinBox_5_valueChanged(const QString &arg1);

    void on_pushButton_190_clicked();

    void on_pushButton_200_clicked();

    void on_pushButton_199_clicked();

    void on_pushButton_201_clicked();

    void on_Menu_ListWidget_2_itemDoubleClicked();

    void on_Menu_ListWidget_1_itemDoubleClicked();
    void on_pushButton_204_clicked();

    void on_pushButton_203_clicked();

    void on_pushButton_202_clicked();

    void on_Option_ListWidget_1_itemDoubleClicked();

    void on_Option_ListWidget_2_itemDoubleClicked();

    void on_pushButton_218_clicked();

    void on_pushButton_217_clicked();

    void on_pushButton_206_clicked();

    void on_pushButton_205_clicked();

    void on_pushButton_207_clicked();

    void on_PopUp_ListWidget_1_itemDoubleClicked();

    void on_PopUp_ListWidget_2_itemDoubleClicked();

    void on_pushButton_220_clicked();

    void on_pushButton_219_clicked();

    void on_QuestLog_ListWidget_1_itemDoubleClicked();

    void on_QuestLog_ListWidget_2_itemDoubleClicked();

    void on_pushButton_222_clicked();

    void on_pushButton_221_clicked();

    void on_Description_ListWidget_1_itemDoubleClicked();
    void on_Description_ListWidget_2_itemDoubleClicked();
    void on_pushButton_224_clicked();
    void on_pushButton_223_clicked();
    void on_pushButton_215_clicked();
    void on_pushButton_214_clicked();
    void on_pushButton_216_clicked();
    void on_pushButton_226_clicked();
    void on_pushButton_225_clicked();
    void on_QuestResults_ListWidget_1_itemDoubleClicked();
    void on_QuestResults_ListWidget_2_itemDoubleClicked();
    void on_lineEdit_292_textChanged(const QString &arg1);

    void on_spinBox_6_valueChanged(int arg1);

    void on_pushButton_208_clicked();

    void updateRanges();
    void on_checkBox_52_stateChanged(int arg1);

    void ModelChangeBack();

    void on_lineEdit_14_textChanged(const QString &arg1);

    void on_lineEdit_16_textChanged(const QString &arg1);

    void on_lineEdit_15_textChanged(const QString &arg1);

    void on_lineEdit_17_textChanged(const QString &arg1);

    void on_pushButton_210_clicked();

    void on_pushButton_211_clicked();

    void on_pushButton_209_clicked();

    void on_pushButton_212_clicked();

    void on_spinBox_8_valueChanged(int value);

    void ReturnHomeCoaching(QByteArray ID_IP_PORT);
    void on_lineEdit_317_textChanged(const QString &arg1);

    void on_lineEdit_320_textChanged(const QString &arg1);

    void on_pushButton_125_clicked();
    void incoming_Spread();
    void on_pushButton_147_clicked();
    void on_pushButton_242_clicked();
    void AddingResults(QString Table_Name, QString Text, int PacketID_P);
    void SS_add_Menus_to_db(QString MenusText);
    void SS_add_Options_to_db(int PacketID_M,QString MenusID, QString OptionText);
    void SS_add_popups_to_db(QString PopupText);
    void SS_add_flags_menus_popups_to_db(QVector<QString> FlagsMenusPopupsIDsVector);
    void SS_add_items_tunar_checks_to_db();
    void SS_add_items_tunar_checks_to_Maps(QString NPCsName, QString NPCsID, QString OP, QString Flag, QString Check);
    void on_pushButton_142_clicked();

    void on_pushButton_143_clicked();

    void on_pushButton_145_clicked();

    void on_pushButton_146_clicked();

private:
    Ui::MainWindow *ui;

    QVector <QString> IconBGDColorIndexVector =
    {
        "8BABA7BD0D","8F96CFC607","BEBDA8D003","A7FEABC007","BD8C8BE30B",
        "D4F9E7B504","C2F8ED9903","B6C5B78D0D","B49ABFA303","F5B2C7F606",
        "B8CADBAD09","ABFBCAD70F"
    };

    int CountSize = 0;
    int iconSize = 0;
    int LastIconSize = 0;

    QVector <QString> IconIndexVector =
    {
        "A1F1D4D70F","FC98DDA90F","ECFAFE950F","819BE3CB01","E8929DB60B",
        "F1CEC5D80B","80C5B99D0B","B2ADF6E004","ED8CA4B20C","91959BC604",
        "BFF4F6B20C","B587A3DB06","82EDBAEB0B","D1F6E69F08","FADDE83C",
        "F6EB8DFC04","89B7C5A709","AD8884D904","CBBD89B606","CAAC96D507",
        "D588D0A208","89A29D8C06","81DA9E2F","F7F09AFC0F","808B9DCA06",
        "AF83F69005","A5A3D99902","AFCEB0AB04","BF8AE8BC03","F5E3B1B506",
        "ABB29CB709","B6F9808E06","BC9DA2C80F","91D5D0A706","F093E2A70E",
        "A7E8A19E04","A49BA3A10E","C1A8E54B","EEACC19706","D2DCF88C02",
        "F1D18D9407","E4C0F6B80F"
    };

    int IconBGDColorCount = 0;
    int IconCount = 0;

    int SpellCount = 0;
    int opcodeSent = 0;
    QVector<QString> Popup_Name_ID_OP_Flag_Vector;
    int MenuCounterToDB = 0;

    int MenusOptionCount = 1;

    QMap<QString,QVector<QString>> QGiversID_Row_ChecksVectorMap;

    QMap<int,QVector<QString>> PacketID_ResultsVector;
    QMap<QString,int> OptionText_PacketID;

    QVector<QString> PopUpTextVector2;

    QMap<QString,QString> ItemsNumber_ItemsID_Map =
    {
        {"8061-T101","98F2808000"},{"8061-T301","BCBC808000"},{"8061-T302","A4F3808000"},
        {"8061-T401","CEBE808000"},{"8061-T402","AEF2808000"},{"8061-T403","F0BC808000"},
        {"8061-T404","B8BD808000"},{"8061-T501","BCF2808000"},{"8061-T502","B6F4808000"},
        {"8061-T701","D2F2808000"},{"8061-T702","B8F4808000"},{"8061-T703","BAF4808000"},
        {"8061-T1002","D4F2808000"},{"8061-T1003","BCF4808000"},{"8061-T1004","BEF4808000"},
        {"8061-T1301","CEF4808000"},{"8061-T1302","D8F2808000"},{"8061-T1303","D0F4808000"},
        {"8061-T1304","DCF2808000"},{"8061-T1305","C0F4808000"},{"8061-T1501","E0F2808000"},
        {"8061-T1503","E2F2808000"},{"8061-T1504","E4F2808000"},{"8061-T1505","C2F4808000"},
        {"8061-T1506","C4F4808000"},{"8061-T2001","FCFA808000"},{"8061-T2003","E8F2808000"},
        {"8061-T2004","EAF2808000"},{"8061-T2005","FEFA808000"},{"8061-T2006","CAF4808000"},
        {"8061-T2007","C6F4808000"},{"8061-T2008","CCF4808000"},{"8061-T2009","C8F4808000"}
    };

    QVector<QString>SpiritmastersIDs_Flags =
    {
        "E3F20400","A2630200","279E0D00","18E90000",
        "61E60000","67E70000","7BE70000","2CE80000",
        "32E80000","52E90000","97E80000","DCE80000",
        "F5E80000","FEE80000","6DE90000","0CEA0000",
        "22EA0000","57EA0000","5DEA0000","73EA0000",
        "74EA0000","DDEA0000","03EC0000","2CEC0000",
        "79EC0000","A9EC0000","E7EC0000","E8EC0000",
        "0FED0000","46ED0000","79EE0000","BF031000",
        "37051000","57051000","72051000","E7051000",
        "EB051000","F0051000","F8051000","75061000",
        "17071000","58091000","5F091000","84091000",
        "E2091000","190A1000","410A1000","4D0A1000",
        "580A1000","7A0A1000","840A1000","C10A1000",
        "230B1000","2D0B1000","3C0B1000","490B1000",
        "620B1000","7C0B1000","DB0B1000","F50B1000",
        "1A0C1000","2A0C1000","350C1000","3D0C1000",
        "590C1000","6C0C1000","7C0C1000","E90C1000",
        "F80C1000","0F0D1000","2C0D1000","580D1000",
        "4E111000","75111000","BE141000","18181000",
        "5D291000","792C1000","78381000","06391000",
        "33391000","8D391000","34451000","96451000",
        "B0451000"
    };

    bool M_ids = false;
    bool M_text = false;


    bool O_ids = false;
    bool O_text = false;

    bool P_ids = false;
    bool P_text = false;

    bool QL_ids = false;
    bool QL_text = false;

    bool D_ids = false;
    bool D_text = false;

    bool QR_ids = false;
    bool QR_text = false;

    QString CopyMyGreeting = "";
    QStringList CopyMyList;

    int CurrentRow = 0;
    bool clicked = false;
    int Merchants_PacketID = 0;

    QVector<QString> MerchantFieldNames =
    {
        "ItemsName_01","ItemID_01","ItemsName_02","ItemID_02","ItemsName_03","ItemID_03","ItemsName_04","ItemID_04",
        "ItemsName_05","ItemID_05","ItemsName_06","ItemID_06","ItemsName_07","ItemID_07","ItemsName_08","ItemID_08",
        "ItemsName_09","ItemID_09","ItemsName_10","ItemID_10","ItemsName_11","ItemID_11","ItemsName_12","ItemID_12",
        "ItemsName_13","ItemID_13","ItemsName_14","ItemID_14","ItemsName_15","ItemID_15","ItemsName_16","ItemID_16",
        "ItemsName_17","ItemID_17","ItemsName_18","ItemID_18","ItemsName_19","ItemID_19","ItemsName_20","ItemID_20",
        "ItemsName_21","ItemID_21","ItemsName_22","ItemID_22","ItemsName_23","ItemID_23","ItemsName_24","ItemID_24",
        "ItemsName_25","ItemID_25","ItemsName_26","ItemID_26","ItemsName_27","ItemID_27","ItemsName_28","ItemID_28",
        "ItemsName_29","ItemID_29","ItemsName_30","ItemID_30","ItemsName_31","ItemID_31","ItemsName_32","ItemID_32",
        "ItemsName_33","ItemID_33","ItemsName_34","ItemID_34","ItemsName_35","ItemID_35","ItemsName_36","ItemID_36",
        "ItemsName_37","ItemID_37","ItemsName_38","ItemID_38","ItemsName_39","ItemID_39","ItemsName_40","ItemID_40"
    };

    QVector<QString> ItemsIDs_ItemsNamesVector;
    QVector<QString> LootItemsIDs_LootItemsNamesVector;
    QMap<QString,int> Description_PacketID;
    QString OldDescription = "";
    QString SpawnThisMob = "";
    int ThisLevel = 0;
    int index = 0;
    QByteArray XZYandFacing = "";
    QByteArray NewMobC9 = "";

    QMap <int,int> XPtoNextLevelMap =
    {
        {1,420},{2,2185},{3,6975},{4,17125},{5,35640},
        {6,66195},{7,113150},{8,181520},{9,277000},{10,405955},
        {11,575420},{12,793115},{13,1067415},{14,1407375},{15,1822720},
        {16,2323845},{17,2921830},{18,3628410},{19,4456000},{20,5417685},
        {21,6527220},{22,7799045},{23,9248255},{24,10890625},{25,12742600},
        {26,14821295},{27,17144510},{28,19730700},{29,22599000},{30,25769215},
        {31,29261820},{32,33097975},{33,37299495},{34,41888875},{35,46889280},
        {36,52324545},{37,58219190},{38,64598390},{39,71488000},{40,78914545},
        {41,86905220},{42,95487905},{43,104691135},{44,114544125},{45,125076760},
        {46,136319595},{47,148303870},{48,161061480},{49,174625000},{50,189027675},
        {51,204303420},{52,220486835},{53,237613175},{54,255718375},{55,274839040},
        {56,295012445},{57,316276550},{58,338669970},{59,362232000}
    };

    QVector <QString> ClassTypeValues =
    {
        "00","01","02","03","04",
        "05","06","07","08","09",
        "0A","0B","0C","0D","0E",
    };

    QByteArray NewNPC_c9 = "01xxxxxxxx82310C89001B101C66F6890000000000000000000001FFDF3ECA61000000000000803F00000000000000000000000000000000000000FFFFFFFF000000000000000000000000000000000000000000000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FFFFFFFF00FFFFFFFF00FFFFFFFF00FFFFFFFF00FFFFFFFF00FFFFFFFF00000000FF06000000004E6577204E5043000000000000000000000000000000000002000000060A0000FFFFFFFF00150000010074737271";


    QVector<QString> BlockedList;
    QString curFile;
    QVector <QByteArray> world_networklist;
    QVector <QByteArray> dns_networklist;
    QVector<QByteArray> serv_networklist;
    QVector<QByteArray> patch_networklist;
    QVector<QByteArray> login_networklist;
    QTimer worldtimer;
    QTimer servtimer;
    QTimer logintimer;
    QTimer dnstimer;
    QTimer patchtimer;
    QTimer aggroTimer;

    bool GroupTimerCheck = false;


    QByteArray TestStatus = "Start";

    QByteArray Lock_X = "";
    QByteArray Lock_Z = "";
    QByteArray Lock_Y = "";
    QByteArray Lock_Facing = "";

    QByteArray Start_X = "";
    QByteArray Start_Z = "";
    QByteArray Start_Y = "";
    QByteArray Start_Facing = "";

    QByteArray Lock_Targets_ID = "";

    QVector<QByteArray> ID_IP_PORTvector;

    QVector<QByteArray> GroupInfoVector;

    int tablesize1 = 0;

    int npctablesize = 0;

    const QByteArray Header_AGGRO = "xxc9xxxx01";

    QMap <int,QVector<int>> Levels_OffandDeff_Mods_VectorMap =
    {
        {1,{-1,100,125,133,150,175,183,186,188,189,190,191,192,192,193,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {2,{-1,67,100,125,140,160,167,171,175,178,180,182,183,185,186,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {3,{-1,33,67,100,125,140,150,157,163,167,170,173,175,177,179,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {4,{-1,10,33,75,100,120,133,143,150,156,160,164,167,169,171,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {5,{-1,0,10,50,75,100,117,129,138,144,150,155,158,162,164,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {6,{-1,0,0,25,50,80,100,114,125,133,140,145,150,154,157,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {7,{-1,0,0,0,25,60,83,100,113,122,130,136,142,146,150,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {8,{-1,0,0,0,10,40,67,86,100,111,120,127,133,138,143,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {9,{-1,0,0,0,0,20,50,71,88,100,110,118,125,131,136,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {10,{-1,0,0,0,0,10,33,57,75,89,100,109,117,123,129,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {11,{-1,0,0,0,0,0,17,43,63,78,90,100,108,115,121,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {12,{-1,0,0,0,0,0,0,29,50,67,80,91,100,108,114,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {13,{-1,0,0,0,0,0,0,14,38,56,70,82,92,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {14,{-1,0,0,0,0,0,0,0,25,44,60,73,83,92,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {15,{-1,0,0,0,0,0,0,0,13,33,50,64,75,85,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {16,{-1,0,0,0,0,0,0,0,0,22,40,55,67,77,86,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {17,{-1,0,0,0,0,0,0,0,0,11,30,45,58,69,79,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {18,{-1,0,0,0,0,0,0,0,0,0,20,36,50,62,71,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {19,{-1,0,0,0,0,0,0,0,0,0,10,27,42,54,64,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {20,{-1,0,0,0,0,0,0,0,0,0,0,18,33,46,57,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {21,{-1,0,0,0,0,0,0,0,0,0,0,9,25,38,50,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {22,{-1,0,0,0,0,0,0,0,0,0,0,0,17,31,43,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {23,{-1,0,0,0,0,0,0,0,0,0,0,0,8,23,36,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {24,{-1,0,0,0,0,0,0,0,0,0,0,0,0,15,29,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {25,{-1,0,0,0,0,0,0,0,0,0,0,0,0,8,21,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {26,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,14,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {27,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,7,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {28,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {29,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {30,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {31,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {32,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {33,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {34,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {35,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {36,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {37,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {38,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {39,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {40,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {41,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {42,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {43,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {44,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {45,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {46,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {47,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {48,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {49,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {50,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {51,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {52,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {53,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {54,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {55,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {56,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {57,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {58,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200,200}},
        {59,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200,200}},
        {60,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200,200}},
        {61,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200,200}},
        {62,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200,200}},
        {63,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200,200}},
        {64,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200,200}},
        {65,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200,200}},
        {66,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200,200}},
        {67,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200,200}},
        {68,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200,200}},
        {69,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200,200}},
        {70,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193,200}},
        {71,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187,193}},
        {72,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180,187}},
        {73,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173,180}},
        {74,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167,173}},
        {75,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160,167}},
        {76,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153,160}},
        {77,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147,153}},
        {78,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140,147}},
        {79,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133,140}},
        {80,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127,133}},
        {81,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120,127}},
        {82,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113,120}},
        {83,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107,113}},
        {84,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100,107}},
        {85,{-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,20,27,33,40,47,53,60,67,73,80,87,93,100}}



    };






    QString QuestCode = "";

    QVector<QString> QuestCodeVector =
    {
        "IC_","","TC_","","xp_","","M_","","P_","","IG_","","IR_","","TR_","","TG_","","sp_","","QLa_","","QLr_","","TS_","","MC_",""
    };


    QVector<QString>QuestFieldNames =
    {
        "Quest_ID_00","Description_00","Starting_City_00","Faction_00","Race_00","Class_00","Level_00",
        "Quest_ID_01","Description_01","Starting_City_01","Faction_01","Race_01","Class_01","Level_01",
        "Quest_ID_02","Description_02","Starting_City_02","Faction_02","Race_02","Class_02","Level_02",
        "Quest_ID_03","Description_03","Starting_City_03","Faction_03","Race_03","Class_03","Level_03",
        "Quest_ID_04","Description_04","Starting_City_04","Faction_04","Race_04","Class_04","Level_04",
        "Quest_ID_05","Description_05","Starting_City_05","Faction_05","Race_05","Class_05","Level_05",
        "Quest_ID_06","Description_06","Starting_City_06","Faction_06","Race_06","Class_06","Level_06",
        "Quest_ID_07","Description_07","Starting_City_07","Faction_07","Race_07","Class_07","Level_07",
        "Quest_ID_08","Description_08","Starting_City_08","Faction_08","Race_08","Class_08","Level_08",
        "Quest_ID_09","Description_09","Starting_City_09","Faction_09","Race_09","Class_09","Level_09",
        "Quest_ID_10","Description_10","Starting_City_10","Faction_10","Race_10","Class_10","Level_10",
        "Quest_ID_11","Description_11","Starting_City_11","Faction_11","Race_11","Class_11","Level_11",
        "Quest_ID_12","Description_12","Starting_City_12","Faction_12","Race_12","Class_12","Level_12",
        "Quest_ID_13","Description_13","Starting_City_13","Faction_13","Race_13","Class_13","Level_13",
        "Quest_ID_14","Description_14","Starting_City_14","Faction_14","Race_14","Class_14","Level_14",
        "Quest_ID_15","Description_15","Starting_City_15","Faction_15","Race_15","Class_15","Level_15",
        "Quest_ID_16","Description_16","Starting_City_16","Faction_16","Race_16","Class_16","Level_16",
        "Quest_ID_17","Description_17","Starting_City_17","Faction_17","Race_17","Class_17","Level_17",
        "Quest_ID_18","Description_18","Starting_City_18","Faction_18","Race_18","Class_18","Level_18",
        "Quest_ID_19","Description_19","Starting_City_19","Faction_19","Race_19","Class_19","Level_19",
        "Quest_ID_20","Description_20","Starting_City_20","Faction_20","Race_20","Class_20","Level_20",
        "Quest_ID_21","Description_21","Starting_City_21","Faction_21","Race_21","Class_21","Level_21",
        "Quest_ID_22","Description_22","Starting_City_22","Faction_22","Race_22","Class_22","Level_22",
        "Quest_ID_23","Description_23","Starting_City_23","Faction_23","Race_23","Class_23","Level_23",
        "Quest_ID_24","Description_24","Starting_City_24","Faction_24","Race_24","Class_24","Level_24",
        "Quest_ID_25","Description_25","Starting_City_25","Faction_25","Race_25","Class_25","Level_25",
        "Quest_ID_26","Description_26","Starting_City_26","Faction_26","Race_26","Class_26","Level_26",
        "Quest_ID_27","Description_27","Starting_City_27","Faction_27","Race_27","Class_27","Level_27",
        "Quest_ID_28","Description_28","Starting_City_28","Faction_28","Race_28","Class_28","Level_28",
        "Quest_ID_29","Description_29","Starting_City_29","Faction_29","Race_29","Class_29","Level_29",
        "Quest_ID_30","Description_30","Starting_City_30","Faction_30","Race_30","Class_30","Level_30"
    };

    QVector<float> DistanceVect;
    QVector<QString> NPCsNameVect;
    QVector<QString> PacketIDVect;

    QVector<QString> XYZVect;



    int SessionsValue = 220761;


    int Menu_Option_Counter = 0;

    QString DB_Text = "";
    QString Current_Text_ID = "";

    bool Already_Checked_1 = false;
    bool Already_Checked_2 = false;
    bool Already_Checked_3 = false;
    bool Already_Checked_4 = false;
    bool Already_Checked_5 = false;
    bool Already_Checked_6 = false;
    bool Already_Checked_7 = false;


    QVector<QString> StartingCityVector =
    {
        "Freeport","Surefall Glade","Qeynos","Tethelin",
        "Fayspires","Neriak","Klick'Anon","Moradhim",
        "Grobb","Halas","Rivervale","Highbourne","Oggok"
    };


    QMap<QString,QString> Button_To_TextMap =
    {
        {"Select_Button","%A"},
        {"Start_Button","%a"},
        {"Circle_Button","%c"},
        {"Square_Button","%s"},
        {"Cross_Button","%x"},
        {"Triangle_Button","%t"},
        {"D_Pad_Up_Button","%u"},
        {"D_Pad_Down_Button","%d"},
        {"D_Pad_Left_Button","%l"},
        {"D_Pad_Right_Button","%r"},
        {"L1_Button","%7"},
        {"L2_Button","%&"},
        {"L3_Button","%9"},
        {"R1_Button","%8"},
        {"R2_Button","%*"},
        {"R3_Button","%("},
        {"Players_Name","%p"}
    }; // #Class#

    QMap <QString,QString> ObjectIDsMap;

    QMap <QString,QVector<QString>> NPCsID_DialogVectorMap;

    QVector <QString> All_Text;
    QString Class_string = "";
    QString Race_string = "";
    QMap <QString,QString> Monster_Class_Map;
    QMap <QString,QString> Monster_Race_Map;

    QMap <QString,QString> PacketClassMap;

    QMap <QString,QString> FilterRaceMap;
    QMap <QString,QString> Class_Race_Map =
    {
        {"ALC", "HUM ELF DELF GNO ERU"},
        {"BRD", "HUM ELF"},
        {"CL",  "HUM ELF DELF GNO DWF HLF ERU"},
        {"DRD", "HUM ELF HLF"},
        {"ENC", "HUM ELF DELF GNO ERU"},
        {"MAG", "HUM ELF DELF GNO ERU"},
        {"MNK", "HUM"},
        {"NEC", "HUM DELF GNO ERU OGR"},
        {"PAL", "HUM ELF DWF ERU"},
        {"RAN", "HUM ELF"},
        {"RGE", "HUM ELF DELF GNO DWF BAR HLF"},
        {"SK",  "HUM DELF TRL ERU OGR"},
        {"SHA", "TRL BAR OGR"},
        {"WAR", "HUM ELF DELF GNO DWF TRL BAR HLF OGR"},
        {"WIZ", "HUM ELF DELF GNO ERU"}
    };

    QMap<int,QString> Race_Order =
    {
        {0,"HUM"},{1,"ELF"},{2,"DELF"},{3,"GNO"}, {4,"DWF"}, {5,"TRL"}, {6,"BAR"},{7,"HLF"},
        {8,"ERU"}, {9,"OGR"}
    };

    QMap<int,QString> Race_Order2;


    QMap<int,QString> Class_Order =
    {
        {0,"WAR"},{1,"RAN"},{2,"PAL"},{3,"SK"}, {4,"MNK"}, {5,"BRD"}, {6,"RGE"},{7,"DRD"},
        {8,"SHA"}, {9,"CL"}, {10,"MAG"}, {11,"NEC"}, {12,"ENC"}, {13,"WIZ"}, {14,"ALC"}
    };

    QMap<int,QString> Class_Order2;

    bool SetFilter = false;
    bool tankONLY = false;
    bool priestONLY = false;
    bool meleeONLY = false;
    bool casterONLY = false;

    QByteArray IncThis = "0000";

    bool Flip163 = false;
    QByteArray CharID_5bytes = "";

    QByteArray Xor_Object_ID2 = "";
    QByteArray Xor_Mob_movement2 = "";
    QByteArray Mob_ALL_Values2 = "";
    QByteArray LoS_ALL_Values2 = "";
    QByteArray Xor_LoS_movement2 = "";
    qlonglong LoS_X = 0;
    qlonglong LoS_Z = 0;
    qlonglong LoS_Y = 0;
    QString LoS_Counter_X = "";
    QString LoS_Counter_Z = "";
    QString LoS_Counter_Y = "";

    int indexOf4029 = 0;
    bool isitOpen = false;
    QMap<QString,bool> OpenLootWindowsMap;

    QByteArray ClientsCheckChannelConfirm = "";
    QByteArray ServersCheckChannelConfirm = "";
    bool RemoveResendChannels = false;
    bool ConfirmRemove = false;

    int F901_Counter = 0;
    int F707_Counter = 0;
    int F807_Counter = 0;
    int HeartBeat_Counter = 0;

    QByteArray CheckCentral = "YES";
    QByteArray CheckNorth = "YES";
    QByteArray CheckEast = "YES";
    QByteArray CheckWest = "YES";
    QByteArray CheckSouth = "YES";

    float ZoneX = 0;
    float ZoneY = 0;
    int zoneInt = 0;
    float ZoneXh = 0;
    float ZoneYh = 0;
    float Xh = 0;
    float Zh = 0;
    float Yh = 0;




    bool Opcode4029 = false;

    QByteArray Reply = "";
    float MySwingRadius = 0.0;
    float MobSwingRadius = 0.0;


    QByteArray LastServerOpcode = "";

    QVector<int> SortServers_Last_MessageNumber1;
    QVector<int> SortServers_Last_MessageNumber2;

    QByteArray Clients42EC = "0200";
    QByteArray ClientsLastFBConfirm = "";
    QByteArray ClientLastFB = "";

    QMap<QByteArray,QByteArray> ClientFbMessNumber_OpcodeMap;
    QVector<QByteArray> ClientFbMessNumberTracker;
    int mapCount = 10;

    QByteArray Channel42messnum = "0100";
    bool loader275 = false;
    QByteArray oldclientval = "";
    int packetid = 0;
    QByteArray CurrentCombat = "00";
    QByteArray Servers_Last_MessageNumber1 = "0200";
    QByteArray Servers_Last_MessageNumber2 = "0200";
    QByteArray masterFBmess = "0300";
    QByteArray Client_Mess = "0100";
    QByteArray Client_FB_Mess = "0100";
    QByteArray Old_session = "";
    QByteArray Next4029 = "0100";
    int Counter4029 = 0;



    int TrueReply = 0;
    int F8Reply = 0;
    int FC02Counter = 0;

    int MySwing = 4;
    int MySwingType = 4;

    bool allowAggro = false;






    QMap<QByteArray,QByteArray> Client_incrementmap = {{"00","0000"},{"01","0000"},{"02","0000"},{"03","0000"},{"04","0000"},
                                                       {"05","0000"},{"06","0000"},{"07","0000"},{"08","0000"},
                                                       {"09","0000"},{"0A","0000"},{"0B","0000"},{"0C","0000"},
                                                       {"0D","0000"},{"0E","0000"},{"0F","0000"},{"10","0000"},
                                                       {"11","0000"},{"12","0000"},{"13","0000"},{"14","0000"},
                                                       {"15","0000"},{"16","0000"},{"17","0000"},
                                                       {"42","0000"},{"43","0000"}};





    QVector <QString> Inventory_Vector =
    {
        "00000000","01000000","02000000","03000000",
        "04000000","05000000","06000000","07000000",
        "08000000","09000000","0A000000","0B000000",
        "0C000000","0D000000","0E000000","0F000000",
        "10000000","11000000","12000000","13000000",
        "14000000","15000000","16000000","17000000",
        "18000000","19000000","1A000000","1B000000"
    };

    QVector <QString> Bank_Vector =
    {
        "00000000","01000000","02000000","03000000",
        "04000000","05000000","06000000","07000000",
        "08000000","09000000","0A000000","0B000000",
        "0C000000","0D000000","0E000000","0F000000",
        "10000000","11000000","12000000","13000000",
        "14000000","15000000","16000000","17000000",
        "18000000","19000000","1A000000","1B000000",
        "1C000000","1D000000","1E000000","1F000000",
        "20000000","21000000","22000000","23000000"
    };

    QVector <QString> InventorySlots_ItemIDs =
    {
        "A480808000","A680808000","A880808000","AA80808000",
        "AC80808000","AE80808000","B080808000","B280808000",
        "B480808000","B680808000","B880808000","BA80808000",
        "BA80808000","BC80808000","BE80808000","C080808000",
        "C280808000","C480808000","C680808000","C880808000",
        "CA80808000","CC80808000","CE80808000","D080808000",
        "A280808000","A080808000","9E80808000","9C80808000"
    };

    QVector <QString> BankSlots_ItemIDs =
    {
        "D280808000","D480808000","D680808000","D880808000",
        "9A80808000","DE80808000","8480808000","8280808000",
        "8680808000","8880808000","8A80808000","8C80808000",
        "8E80808000","DC80808000","B680808000","DA80808000",
        "E080808000","E280808000","E280808000","9881808000",
        "E480808000","E680808000","9080808000","E880808000",
        "EA80808000","EC80808000","EE80808000","9280808000",
        "F080808000","F280808000","F480808000","F680808000",
        "F880808000","FA80808000","FC80808000","9480808000"
    };

    QVector <QString> InventoryItem_Quantity =
    {
        "02","02","02","02",
        "02","02","02","02",
        "02","02","02","02",
        "02","02","02","02",
        "02","02","02","02",
        "02","02","02","02",
        "02","06","02","14"
    };

    QVector <QString> InvItem_Type =
    {
        "EQuipment","EQuipment","EQuipment","EQuipment",
        "EQuipment","EQuipment","EQuipment","EQuipment",
        "EQuipment","EQuipment","EQuipment","EQuipment",
        "EQuipment","EQuipment","EQuipment","EQuipment",
        "EQuipment","EQuipment","EQuipment","EQuipment",
        "EQuipment","EQuipment","EQuipment","EQuipment",
        "Other","Other","Other","Other"
    };

    QVector <QString> BankItem_Type =
    {
        "EQuipment","EQuipment","EQuipment","EQuipment","EQuipment","EQuipment",
        "Other","Other","Other","Other","Other","Other","Other","EQuipment",
        "EQuipment","EQuipment","EQuipment","EQuipment","EQuipment","EQuipment",
        "EQuipment","EQuipment","EQuipment","EQuipment","EQuipment","EQuipment",
        "EQuipment","EQuipment","EQuipment","EQuipment","EQuipment","EQuipment",
        "EQuipment","EQuipment","Other","Other"
    };

    QVector <QString> ItemIsEquippedVector =
    {
        "NO","NO","NO","NO",
        "NO","NO","NO","NO",
        "NO","NO","NO","NO",
        "NO","NO","NO","NO",
        "NO","NO","NO","NO",
        "NO","NO","NO","NO",
        "NO","NO","NO","NO"
    };

    QVector <QString> BankItem_Quantity =
    {
        "02","02","02","02",
        "02","02","02","02",
        "02","02","02","02",
        "04","02","02","02",
        "02","02","02","02",
        "02","02","02","02",
        "02","02","02","02",
        "02","02","02","02",
        "02","02","04","02"
    };

    QMap <QString,QString> EquipmentSlotsUsed =
    {
        {"00","FREE"},{"01","FREE"},{"02","FREE"},{"03","FREE"},
        {"04","FREE"},{"05","FREE"},{"06","FREE"},{"07","FREE"},
        {"08","FREE"},{"09","FREE"},{"0A","FREE"},{"0B","FREE"},
        {"0C","FREE"},{"0D","FREE"},{"0E","FREE"},{"0F","FREE"},
        {"10","FREE"},{"11","FREE"},{"12","FREE"},{"13","FREE"},
        {"14","FREE"},{"15","FREE"}
    };

    QVector <QString> EQuipSots_InvSlots =
    {
        "XX","XX","XX","XX",
        "XX","XX","XX","XX",
        "XX","XX","XX","XX",
        "XX","XX","XX","XX",
        "XX","XX","XX","XX",
        "XX","XX","XX","XX",
        "XX","XX","XX","XX"
    };

    QVector <QString> Inv_HP_Vector =
    {
        "D0F001","B0EA01","D0BE01","F0AB01","C0B802","C0B802","C0B802","A4A501","F0AB01",
        "DAAA01","D08604","9ACB01","9ACB01","B0EA01","B0D101","B0EA01","B0EA01","C0BB01",
        "E0C101","E0C101","B0EA01","9ACB01","C8E201","80C801","00","00","00","00"
    };

    QVector <QString> Bank_HP_Vector =
    {
        "88C201","88C201","88c201","88c201","d00f","ac67","00","00","00","00","00","00","00",
        "be8001","daaa01","82bd01","f0c901","b08601","828601","c0bb01","e0c101","b0d101",
        "e0a801","a09902","9acb01","9acb01","c0b802","d00f","90cb01","d0f001","f0c401",
        "eac901","90b201","88d601","00","00"
    };




    QVector <QString> SpellIDsVector =
    {
        "8200","8400","8600","8800","8A00","8C00","8E00","9000","9200","9400","9600",
        "9800","9A00","9C00","9E00","A000","A200","A400","A600","A800","AA00","AC00",
        "AE00","B000","B200","B400","B600","B800","BA00","BC00","BE00","C000","C200",
        "C400","C600","C800","CA00","CC00","CE00","D000","D200","D400","D600","D800",
        "DA00","DC00","DE00","E000","E200","E400","E600","E800","EA00","EC00","EE00",
        "F000","F200","F400","F600","F800","FA00","FC00","FE00","8001","8201","8401"
    };

    QVector <QString> SpellSlotsVector =
    {
        "04","06","00","0A","0E","6E","08","0C","16","14","1C",
        "12","18","1A","2A","42","26","32","3C","30","72","28",
        "74","1E","2C","34","02","66","10","36","44","2E","20",
        "3A","8201","70","50","7A","6C","7C","68","78","54","4E",
        "24","38","52","8001","3E","40","76","7E","6A","22","48",
        "62","56","46","4A","4C","58","5C","5A","60","5E","64"
    };

    QVector <QString> HB_ValuesVector =
    {
        "0001","0210","0001","0001","0001","0001","0001","0001","0001","0001","020E",
        "0001","0001","0001","0001","0001","0001","0001","0001","0001","0001","0204",
        "0001","0001","0001","0001","0001","0001","0001","0206","0001","0001","0001",
        "0001","0001","0001","0001","0001","0202","0001","0001","0001","0208","020C",
        "0200","0001","0212","0001","0001","0001","0001","0001","020A","0001","0001",
        "0001","0001","0001","0001","0001","0001","0001","0001","0001","0001","0001"
    };



    int MobSlider = 0;
    float Magnify_North = 0.0;
    float Magnify_West = 0.0;
    float Magnify_East = 0.0;
    float Magnify_South = 0.0;
    float High_Z = 0.0;
    float Low_Z = 0.0;

    float Entrance_1_North_GZ = 0.0;
    float Entrance_1_West_GZ = 0.0;
    float Entrance_1_East_GZ = 0.0;
    float Entrance_1_South_GZ = 0.0;

    float Entrance_1_GZ = 0.0;

    float Entrance_2_North_GZ = 0.0;
    float Entrance_2_West_GZ = 0.0;
    float Entrance_2_East_GZ = 0.0;
    float Entrance_2_South_GZ = 0.0;

    float Entrance_2_GZ = 0.0;

    QString LevelsNorth_L1 = "";
    QString LevelsWest_L1 = "";
    QString LevelsEast_L1 = "";
    QString LevelsSouth_L1 = "";
    QString Levels_High_Z1 = "";
    QString Levels_Low_Z1 = "";
    QString LevelsName_L1 = "";

    QString LevelsNorth_L2 = "";
    QString LevelsWest_L2 = "";
    QString LevelsEast_L2 = "";
    QString LevelsSouth_L2 = "";
    QString Levels_High_Z2 = "";
    QString Levels_Low_Z2 = "";
    QString LevelsName_L2 = "";

    QString LevelsNorth_L3 = "";
    QString LevelsWest_L3 = "";
    QString LevelsEast_L3 = "";
    QString LevelsSouth_L3 = "";
    QString Levels_High_Z3 = "";
    QString Levels_Low_Z3 = "";
    QString LevelsName_L3 = "";

    QString LevelsNorth_L4 = "";
    QString LevelsWest_L4 = "";
    QString LevelsEast_L4 = "";
    QString LevelsSouth_L4 = "";
    QString Levels_High_Z4 = "";
    QString Levels_Low_Z4 = "";
    QString LevelsName_L4 = "";

    QString LevelsNorth_L5 = "";
    QString LevelsWest_L5 = "";
    QString LevelsEast_L5 = "";
    QString LevelsSouth_L5 = "";
    QString Levels_High_Z5 = "";
    QString Levels_Low_Z5 = "";
    QString LevelsName_L5 = "";

    QString LevelsNorth_L6 = "";
    QString LevelsWest_L6 = "";
    QString LevelsEast_L6 = "";
    QString LevelsSouth_L6 = "";
    QString Levels_High_Z6 = "";
    QString Levels_Low_Z6 = "";
    QString LevelsName_L6 = "";

    QString LevelsNorth_L7 = "";
    QString LevelsWest_L7 = "";
    QString LevelsEast_L7 = "";
    QString LevelsSouth_L7 = "";
    QString Levels_High_Z7 = "";
    QString Levels_Low_Z7 = "";
    QString LevelsName_L7 = "";



    QString BuildingsNextLevel = "Check";
    QString BuildingsLastLevel = "";

    bool MagnifyMap = false;
    bool InTheBuilding = true;

    int tablesize0 = 0;
    int tablesizeQ = 0;
    int tablesizeT = 0;

    int tablesizeZ = 0;













    bool StopSending = false;




    ///////////////////////////////////////////////////////////////////////

    QVector <QString> C9GearLocations =
    {
        "Torso","186","2","Forearms","188","2","Gloves","190","2","Legs","192","2",
        "Feet","194","2","Head","196","2","Robe","298","2"
    };

    QVector <QString> C9GearLocationsC =
    {
        "Torsoc","222","8","Forearmsc","232","8","Glovesc","242","8","Legsc","252","8",
        "Feetc","262","8","Headc","272","8","Robec","282","8"
    };

    QMap <QString,QString> C9replaceGear =
    {
        {"00","Head"},{"01","Gloves"},{"05","Torso"},{"08","Forearms"},
        {"0C","Legs"},{"0D","Feet"},{"15","Robe"}
    };

    QVector <QString> C9WeaponLocations =
    {
        "Primary","134","8","Secondary","142","8","Shield","150","8"
    };

    QMap <QString,QString> C9replaceWeapon =
    {
        {"0E","Primary"},{"0F","Secondary"},{"10","Primary"},{"11","Shield"},
        {"12","Secondary"},{"13","Primary"},{"14","Secondary"}
    };

    QMap <QString,QString> C9replaceArmor =
    {
        {"02","Head"},{"04","Robe"},{"0A","Torso"},{"0E","Forearms"},
        {"14","Legs"},{"16","Feet"},{"26","Gloves"}
    };

    QMap <QString,QString> C9replaceColor =
    {
        {"02","HeadC"},{"04","RobeC"},{"0A","TorsoC"},{"0E","ForearmsC"},
        {"14","LegsC"},{"16","FeetC"},{"26","GlovesC"}
    };

    QMap <QString,QString> C9replaceWeaponEQ =
    {
        {"18","Primary"},{"1A","Shield"},{"1C","Secondary"},{"1E","Primary"},
        {"20","Secondary"},{"22","Secondary"},{"24","Primary"}
    };



    QMap <QString,QString> EQuipTypeToSlot =
    {
        {"02","00"},{"26","01"},{"08","04"},{"0A","05"},
        {"0E","08"},{"12","0B"},{"14","0C"},{"16","0D"},
        {"18","0E"},{"1C","0F"},{"1E","10"},{"1A","11"},
        {"20","12"},{"22","12"},{"24","13"},{"04","15"}
    };

    QMap <QString,QVector <QString>> WeaponCheckIfOpen =
    {
        {"0E",{"10","12","13"}},                //{Primary,{2hand,Bow,HeldPrimary}}
        {"0F",{"10","11","12","13","14"}},      //{Secondary,{2hand,Shield,Bow,HeldSecondary}}
        {"10",{"0E","0F","11","12","13","14"}}, //{2Hand,{Primary,Secondary,Shield,Bow,HeldPrimary,HeldSecondary}}
        {"11",{"0F","10","12","14"}},           //{Shield,{Secondary,2hand,Bow,HeldSecondary}}
        {"12",{"0E","0F","10","11","13","14"}}, //{Bow,{Primary,Secondary,2hand,Shield,HeldPrimary,HeldSecondary}}
        {"13",{"0F","10","12"}},                //{HeldPrimary,{Secondary,2Hand,Bow}}
        {"14",{"0F","10","11","12"}}            //{HeldSecondary,{Secondary,2hand,Shield,Bow}}
    };




    /////////////////////////////////////





    QMap <QString,QVector <QString>> MonsterRaceVectorMap;

    QMap <QString,QVector <QString>> MonsterClassVectorMap;

    ///////////////Coaching///////////////
    QTimer coachtimer;

    QVector <int> StartingCoachVector =
    {
        1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
        1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
        1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
        1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0
    };


    QByteArray NPC_Location_XZY_2 = "";
    const QByteArray NPC_Location_XZY	 = "f60700D1E5974505817642AE819745B0184640000000000000000000000000";

    QByteArray Default_Paste = "Default";
    QString Torso_Value1 = "";
    QString Forearms_Value2 = "";
    QString Gloves_Value3 = "";
    QString Legs_Value4 = "";
    QString Feet_Value5 = "";
    QString Head_Value6 = "";
    QString Robe_Value7 = "";

    QString X_Value8 = "";
    QString Z_Value9 = "";
    QString Y_Value10 = "";
    QString Facing_Value11 = "";
    QString Race_Value12 = "";

    QString Torso_Value14 = "";
    QString Forearms_Value15 = "";
    QString Gloves_Value16 = "";
    QString Legs_Value17 = "";
    QString Feet_Value18 = "";
    QString Head_Value19 = "";
    QString Robe_Value20 = "";

    QString Face_Value21 = "";
    QString HairStyle_Value22 = "";
    QString HairLength_Value23 = "";
    QString HairColor_Value24 = "";
    QString Class_Value25 = "";

    QString NPC_Name_Hex = "";

    QString Level_Value28 = "";
    QString Size_Value29 = "";
    QString ModelID_Value31 = "";
    QString Primary_Value32 = "";
    QString Secondary_Value33 = "";
    QString Shield_Value34 = "";



    const QByteArray NEW42ECxor = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    const QByteArray New_NPC_c9 = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    const QByteArray NewSpawnC9 = "018C1B100082115BE0001B100DF3ECA61001000000000000000001FFDF3ECA61000000000000803F00000000000000000000000000000000000000FFFFFFFF000000000000000000000000000000000000000000000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FFFFFFFF00FFFFFFFF00FFFFFFFF00FFFFFFFF00FFFFFFFF00FFFFFFFF00000000FF06000000004E6577204E504300000000000000000000000000000000000200000006008000FFFFFFFF00150000010074737271";
    bool SpawnClone = false;
    bool SpawnDefault = false;
    bool SpawnDBClone = false;
    QString Current_channel = "";
    QByteArray holderDespawn = "";
    qlonglong Old_value = 0;
    QByteArray New_value = "";
    qlonglong New_value2 = 0;
    qlonglong Output_Xor = 0;
    QString Output_Xor_2 = "";

    QVector <QString> DeletedID;
    QVector <int> DeletedPacketNumber;
    QByteArray Packet_ID = "";

    QMap <QString,QString> Name_PacketNumber_Map;

    QColor color;
    int counter1 = 0;
    int rowCountReset = 0;
    QString RemoveObjectID = "";




    QMap <int,int> Race_Gender_Index =
    {
        {0,0},{1,0},{2,1},{3,1},{4,2},
        {5,2},{6,3},{7,3},{8,4},{9,4},
        {10,5},{11,5},{12,6},{13,6},{14,7},
        {15,7},{16,8},{17,8},{18,9},{19,9}
    };

    QMap <int,QString> NPC_Map;
    QString SizeMylist = "";
    QString New_ID = "";
    QStringList NPC_List;

    QMap <QString,QString> ClassValueToString =
    {
        {"00","WAR"},{"01","RAN"},{"02","PAL"},{"03","SK"},
        {"04","MNK"},{"05","BRD"},{"06","RGE"},{"07","DRD"},
        {"08","SHA"},{"09","CL"},{"0A","MAG"},{"0B","NEC"},
        {"0C","ENC"},{"0D","WIZ"},{"0E","ALC"}
    };

    QMap <QString,QString> RaceValueToString =
    {
        {"00","HUM"},{"01","ELF"},{"02","DELF"},
        {"03","GNO"},{"04","DWF"},{"05","TRL"},
        {"06","BAR"},{"07","HLF"},{"08","ERU"},
        {"09","OGR"}
    };


    QMap <int,QString> NPC_Type =
    {
        {0,"0000"},{1,"8200"},{2,"8800"},{3,"8001"},{4,"8100"},
        {5,"8000"},{6,"8011"},{7,"8001"}
    };

    QMap <int,QString> Race_C9_1Byte_Map =
    {
        {0,"06"},{1,"02"},{2,"04"},{3,"01"},{4,"08"},
        {5,"03"},{6,"07"},{7,"00"},{8,"09"},{9,"05"}
    };

    QMap <int,QString> Race_C9_1Byte_MapX2 =
    {
        {0,"06"},{1,"06"},{2,"02"},{3,"02"},{4,"04"},
        {5,"04"},{6,"01"},{7,"01"},{8,"08"},{9,"08"},
        {10,"03"},{11,"03"},{12,"07"},{13,"07"},{14,"00"},
        {15,"00"},{16,"09"},{17,"09"},{18,"05"},{19,"05"}
    };

    int LastFlagValueUsed = 0;



    QMap <QString,int> Race_AND_Gender =
    {
        {"DF3ECA61",0},{"16D54AC4",1},{"AA9DD87E",2},{"394B9B8C",3},{"47045BAA",4},
        {"BEFD0B35",5},{"F86FD5D8",6},{"F71989E1",7},{"470B0C32",8},{"BE3C1A4F",9},
        {"156B9CA9",10},{"FC84404C",11},{"E23FDBA3",12},{"31A7278A",13},{"C698D870",14},
        {"3573530D",15},{"90748084",16},{"4F0BC0AC",17},{"32A56F4C",18},{"0127E34D",19}
    };

    //////////////////Spells///////////////////
    QMap <int,int> spellslot_map = {
        {0,0x12},
        {1,0x36},
        {2,0x3A},
        {3,0x1B},
        {4,0x2A},
        {5,0x35},
        {6,0x27},
        {7,0x09},
        {8,0x00},
        {9,0x29}};

    bool reDoGreeting = false;

    QString spell_idout;
    QString spell_str1;
    QString spell_graphic;
    int spell_position;
    QByteArray master_spell_count;
    QByteArray another_spell_count;
    QMap <int,QString> spellslot_copy;
    QList <QTimer*> timers1;
    bool unmemorize = false;

    bool slot_0_trigger = false;
    bool slot_1_trigger = false;
    bool slot_2_trigger = false;
    bool slot_3_trigger = false;
    bool slot_4_trigger = false;
    bool slot_5_trigger = false;
    bool slot_6_trigger = false;
    bool slot_7_trigger = false;
    bool slot_8_trigger = false;
    bool slot_9_trigger = false;

    bool cooldown_0 = false;
    bool cooldown_1 = false;
    bool cooldown_2 = false;
    bool cooldown_3 = false;
    bool cooldown_4 = false;
    bool cooldown_5 = false;
    bool cooldown_6 = false;
    bool cooldown_7 = false;
    bool cooldown_8 = false;
    bool cooldown_9 = false;

    int fee = 0x0;
    int fee2 = 0x0;
    int spell_id = 0x0;
    int spell_bookslot = 0x0;
    bool switch1 = false;
    bool switch2 = false;


    QMap <int,QString> spellbook_copy;

    QByteArray spell_0_counter = "0000";
    QByteArray spell_1_counter = "0000";
    QByteArray spell_2_counter = "0000";
    QByteArray spell_3_counter = "0000";
    QByteArray spell_4_counter = "0000";
    QByteArray spell_5_counter = "0000";
    QByteArray spell_6_counter = "0000";
    QByteArray spell_7_counter = "0000";
    QByteArray spell_8_counter = "0000";
    QByteArray spell_9_counter = "0000";



    QMap <int,QString> spellbook_map = {
        {0x00,"Kick%%AA##2E6E280A##6000"}, //add graphic casttime duration
        {0x01,"Fortitude%%AA##0892C0F3##12000"},
        {0x02,"Return_Home%%AA##3382B28F##30000"},
        {0x03,"Quick_Strike%%AA##8B77CD66##30000"},
        {0x04,"Taunt%%AA##2A169ECF##5000"},
        {0x05,"Concentration%%AA##0892C0F3##4000"},
        {0x06,"Call_to_Action%%AA##B6B5AE1D##90000"},
        {0x07,"Furious_Defense%%AA##B7B5AE1D##120000"},
        {0x08,"Bellow%%AA##9A40BE64##300000"},
        {0x09,"Stomp%%AA##2E6E280A##6000"},
        {0x0A,"Elemental_Toughness%%AA##9A40BE64##2000"},
        {0X0B,"Attention%%AA##0892C0F3##60000"},
        {0X0C,"Rapid_Strike%%AA##8B77CD66##30000"},
        {0X0D,"Call_to_Arms%%AA##B6B5AE1D##90000"},
        {0X0E,"Critical_Strike%%AA##CD6D63EA##90000"},
        {0X0F,"Iron_Will%%AA##B6B5AE1D##90000"},
        {0X10,"Roar%%AA##9A40BE64##300000"},
        {0X11,"Minotaurs_Persistence%%AA##B8B5AE1D##6000"},
        {0X12,"Enrage%%AA##E3884247##7000"},
        {0X13,"Provoke%%AA##2A169ECF##5000"},
        {0X14,"Critical_Assault%%AA##CD6D63EA##90000"},
        {0X15,"Call_to_Combat%%AA##B6B5AE1D##90000"},
        {0X16,"Iron_Resolve%%AA##B6B5AE1D##90000"},
        {0X17,"Howl%%AA##9A40BE64##300000"},
        {0X18,"Focus%%AA##0892C0F3##90000"},
        {0X19,"Bait%%AA##2A169ECF##5000"},
        {0X1A,"Iron_Conviction%%AA##B6B5AE1D##90000"},
        {0X1B,"Critical_Barrage%%AA##CD6D63EA##90000"},
        {0X1C,"Werewolf%%AA##8503B84D##5000"},
        {0X1D,"War_Cry%%AA##9A40BE64##300000"},
        {0X1E,"Incite%%AA##2A169ECF##5000"},
        {0X1F,"Feral_Wolf%%AA##8503B84D##5000"},
        {0X20,"Dire_Wolf%%AA##8503B84D##5000"},
        {0X21,"Call_to_War%%AA##B6B5AE1D##90000"},
        {0X22,"Dash%%AA##0D5CABA9##240000"},
        {0X23,"Summon_Badger%%AA##8503B84D##5000"},
        {0X24,"Summon_Undead_Mammoth%%AA##8503B84D##5000"},
        {0X25,"Summon_Squishy_Cube%%AA##8503B84D##5000"},
        {0X26,"Summon_Tiny_Jester%%AA##8503B84D##5000"},
        {0X27,"Boot%%AA##2E6E280A##6000"},
        {0X28,"Goad%%AA##2A169ECF##5000"},
        {0X29,"Bite%%AA##D5DE907B##5000" },
        {0X2A,"Slash%%AA##20AF8B76##30000"},
        {0X2B,"Summon_Baby_Sandswiss%%AA##8503B84D##5000"},
        {0X2C,"Summon_White_Rat%%AA##8503B84D##5000"},
        {0X2D,"Illusion_Gelatinous_Cube%%AA##24E6147B##5000"},
        {0X2E,"Illusion_Rat%%AA##8503B84D##5000"},
        {0X2F,"Sand_Blast_Effect%%AA##AE10C42A##30000"},
        {0X30,"Summon_Sand_Giant%%AA##8503B84D##5000"},
        {0X31,"Summon_Moss_Snake%%AA##8503B84D##5000"},
        {0X32,"Illusion_Shark%%AA##8503B84D##5000"},
        {0X33,"Tough_Skin%%AA##B6B5AE1D##4000"},
        {0X34,"Remove_Illusion%%AA##4800030E##5000"},
        {0X35,"Dropkick%%AA##2E6E280A##6000"},
        {0X36,"Critical_Attack%%AA##CD6D63EA##90000"},
        {0X37,"Sprint%%AA##0D5CABA9##120000"},
        {0X38,"Rend%%AA##20AF8B76##30000"},
        {0X39,"Focused_Attention%%AA##0892C0F3##90000"},
        {0X3A,"Critical_Flurry%%AA##CD6D63EA##90000"},
        {0X3B,"Iron_Resolution%%AA##B6B5AE1D##30000"},
        {0X3C,"Mithril_Skin%%AA##0892C0F3##12000"},
        {0X3D,"Call_to_Victory%%AA##B6B5AE1D##90000"},
        {0X3E,"Diligence%%AA##0892C0F3##36000"},
        {0X3F,"Protection_of_Marr%%AA##B8B5AE1D##30000"},
        {0X40,"Growl_of_Battle%%AA##E3884247##20000"},
        {0X41,"Frantic_Defense%%AA##B7B5AE1D##120000"}};


    ///////////////////////////////////////////


    ////////////////////DB offload to Map//////////
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    QFuture<void> f1;

    QFutureWatcher<void> *watcher2 = new QFutureWatcher<void>();
    QFuture<void> f2;

    QFutureWatcher<void> *watcher4 = new QFutureWatcher<void>();
    QFuture<QByteArray> f4;

    QFutureWatcher<void> *watcher3 = new QFutureWatcher<void>();
    QFuture<QByteArray> f3;



    QString rowline = "";
    QString rowline2 = "";
    QString rowline3 = "";
    int tablesize = 0;
    int tablesize2 = 0;
    int tablesize3 = 0;
    int rotate = 0;
    int rotate2 = 0;
    int rotate3 = 0;
    int rotateQ = 0;
    int rotateT = 0;
    int rotateZ = 0;
    int tablesize4 = 0;
    int rotate4 = 0;
    QByteArray rowline4 = "";


    int QuestsID_PacketID = 0;

    /////////////////////////////////////////////////

    ////////////////////////Map//////////////

    QString directory = "C:/Customize";

    QGraphicsScene *scene;
    QPixmap a = QPixmap (directory + "/Tunaria8.jpg");
    QPixmap b = QPixmap(directory + "/blueball-scale.png");
    QGraphicsPixmapItem *item1;
    QGraphicsPixmapItem *item2;
    int firstmapxone = 0;
    int secondmapyone = 0;
    int NPC_Counter = 0;


    QString Database = "/Master_Database.db";

    QGraphicsScene *scene2;
    QPixmap c = QPixmap (directory + "/ZoneMapsPNG/West Plateau (C)/West Plateau (C).png");
    QPixmap currentImage = QPixmap (directory + "/ZoneMapsPNG/West Plateau (C)/West Plateau (C).png");
    QPixmap d = QPixmap(directory + "/blueball-scale.png");
    QGraphicsPixmapItem *item3;
    QGraphicsPixmapItem *item4;
    int MapsX = 0;
    int MapsY = 0;

    QPixmap Dot_1 = QPixmap(directory + "/Dot_1.png");

    float MobWest = 0;
    float MobEast = 0;
    float MobNorth = 0;
    float MobSouth = 0;



    //////////////////////////Map///////////////







    //////////////////////custom packets////////////////////////

    const QByteArray packetcustom2 = "d37c1234a0c101bf543213"; //00c9 header

    const QByteArray ZeroChanHeader = "d37c1234a0c101bf543213d9bc0d00280000c9030000";

    const QByteArray twosevenone = "0a1f123485c401bf543213";

    QByteArray twosevenoneadd1 =
            "42ec"
            "0100"
            "00"
            ;

    QByteArray twosevenoneadd =
            "42ec"
            "0100"
            "00"
            "20"
            "ad01" //STR
            "22"
            "a601" //STA
            "12"
            "dd"   //AGI
            "23"
            "cc01" //DEX
            "22"
            "0501" //WIS
            "12"
            "46"   //INT
            "13"
            "1e"   //CHA
            "23"
            "591b" //HP
            "22"
            "591b" //HP Max
            "22"
            "7b15" //POW
            "22"
            "7b15" //POW Max
            "12"
            "64"   //??
            "13"
            "d2"   //HoT
            "13"
            "6d"   //PoT
            "23"
            "2a06" //AC
            "811e86136d1345136d1377136d17a313b4133c13c413f11332133223591b220510227b15227b151264138c136d813fff0300"
            "fb2c4b007a0a13000000530068006f0075007400200063006800610074002000760069007300690062006c0065002e00fb034c00c70000"
            "43ff0c010100001002533f9943f316134d116511671161112711731120114d1169116711681174117911201143116111721161117011611163116551ffffffff028902ffffffffffffffff0213018901ffffffffffffffff0212018901ffffffffffffffff022101018901ffffffffffffffff028902ffffffffffffffff02113211018d01ffffffffffffffffecc51bde0d13531170116911721169117411201143116911721163116c116541ffffff0213018901ffffffffffffffff0212018901ffffffffffffffff022101018901ffffffffffffffff028902ffffffffffffffff0213018901ffffffffffffffff0212018901ffffffffffffffff022101018901ffffffffffffffff0212ff00";

    const QByteArray twoseventwo = "0a1f123485c401bf543213";

    const QByteArray twosevenfive = "d37c1234aec301bf543213";



    const QByteArray heartbeatfc = "d37c123487c001bf543213d9bc0d003500FC02D007";

    const QByteArray heartbeatstill = "d37c123487c001bf543213d9bc0d00xxxx";

    QString xout1 = "";
    QString yout1 = "";
    QString zout1 = "";
    QString worldout1 = "";
    QString facingout1 = "";
    QByteArray ecchannel = "0100";
    //////////////////////custom packets////////////////////////

    ////////////////////////////6107 op reply///////////////////////////////

    const QByteArray Con_Packet  = "61070305000000810b80f3d50ff82efebf34061d08085d180ae0a0810b082ffebf000000000000000000000000282ffebf03831c08e0a0810bc0dafc1013000000fb0000005414ff0d2401000080f3d50f13000000c0dafc1013000000482ffebfe1230e0880f3d50fc0dafc10682ffebf460b08082857d30abd000000682ffebf1fe71008000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005000000";
    const QByteArray Self_Packet = "61070302014200650072007300650072006b00650072000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000bd000000e062febfb04a6810e82ffebfa20c0808383e890fbd000000585E03001fe710084c0061007300740020004c006500670069006f006e0020006f006600200045006c006900740065007300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000";
    int Mylevel = 0;
    int Moblevel = 0;
    int difference1 = 0;
    int difference4 = 0;
    QString Face_Color_V = "0";
    QString Con_Color_V = "";
    QByteArray holder_Con = "";

    ///////////////////////////Despawn///////////////////////////////////////////////


    const QByteArray first1 = "d37c123495c301bf543213d9bc0d00123401c90100002001FA11FF418221E190613EDA1D8CBCE611066801FF040CDFBC446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029002017069636B636C6177207368616D616E19091206210804214B78130E120141747372710002c90100002001FB11FF418221E49E613F331D8B3E2E11066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C657218081206244D7822010E120141747372710003c90100002001FC11FF418221E704513EFE1D7D2C12066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C65721808120624517822010E1201417473727100";
    const QByteArray second1 = "d37c123495c301bf543213d9bc0d00123404c90100002001FA11FF418221E190613EDA1D8CBCE611066801FF040CDFBC446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029002017069636B636C6177207368616D616E19091206210804214B78130E120141747372710005c90100002001FB11FF418221E49E613F331D8B3E2E11066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C657218081206244D7822010E120141747372710006c90100002001FC11FF418221E704513EFE1D7D2C12066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C65721808120624517822010E1201417473727100";
    const QByteArray third1 = "d37c123495c301bf543213d9bc0d00123407c90100002001FA11FF418221E190613EDA1D8CBCE611066801FF040CDFBC446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029002017069636B636C6177207368616D616E19091206210804214B78130E120141747372710008c90100002001FB11FF418221E49E613F331D8B3E2E11066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C657218081206244D7822010E120141747372710009c90100002001FC11FF418221E704513EFE1D7D2C12066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C65721808120624517822010E1201417473727100";
    const QByteArray fourth1 = "d37c123495c301bf543213d9bc0d0012340Ac90100002001FA11FF418221E190613EDA1D8CBCE611066801FF040CDFBC446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029002017069636B636C6177207368616D616E19091206210804214B78130E12014174737271000Bc90100002001FB11FF418221E49E613F331D8B3E2E11066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C657218081206244D7822010E12014174737271000Cc90100002001FC11FF418221E704513EFE1D7D2C12066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C65721808120624517822010E1201417473727100";
    const QByteArray fifth1 = "d37c123495c301bf543213d9bc0d0012340Dc90100002001FA11FF418221E190613EDA1D8CBCE611066801FF040CDFBC446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029002017069636B636C6177207368616D616E19091206210804214B78130E12014174737271000Ec90100002001FB11FF418221E49E613F331D8B3E2E11066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C657218081206244D7822010E12014174737271000Fc90100002001FC11FF418221E704513EFE1D7D2C12066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C65721808120624517822010E1201417473727100";
    const QByteArray sixth1 = "d37c123495c301bf543213d9bc0d00123410c90100002001FA11FF418221E190613EDA1D8CBCE611066801FF040CDFBC446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029002017069636B636C6177207368616D616E19091206210804214B78130E120141747372710011c90100002001FB11FF418221E49E613F331D8B3E2E11066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C657218081206244D7822010E120141747372710012c90100002001FC11FF418221E704513EFE1D7D2C12066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C65721808120624517822010E1201417473727100";
    const QByteArray seventh1 = "d37c123495c301bf543213d9bc0d00123413c90100002001FA11FF418221E190613EDA1D8CBCE611066801FF040CDFBC446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029002017069636B636C6177207368616D616E19091206210804214B78130E120141747372710014c90100002001FB11FF418221E49E613F331D8B3E2E11066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C657218081206244D7822010E120141747372710015c90100002001FC11FF418221E704513EFE1D7D2C12066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C65721808120624517822010E1201417473727100";
    const QByteArray eigth1 = "d37c123490c201bf543213d9bc0d00123416c90100002001FA11FF418221E190613EDA1D8CBCE611066801FF040CDFBC446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029002017069636B636C6177207368616D616E19091206210804214B78130E120141747372710017c90100002001FB11FF418221E49E613F331D8B3E2E11066801FF04183796446666663F8413FFFFFFFF9024FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF41FFFFFFFF21020231FFB5029102017069636B636C617720627261776C657218081206244D7822010E1201417473727100";

    QMap<int,QByteArray> despawn_map = {{0,first1},{1,second1},{2,third1},{3,fourth1},{4,fifth1},{5,sixth1},{6,seventh1},{7,eigth1}};



    ////////////////////////////////////////////////////////////////////////
    //////////////////////////Teleport/////////////////////////////////////
//    QByteArray teleport_destination = "";
//    QByteArray clientheld_id = "";
//    QByteArray serverheld_id = "";
//    QHostAddress senderheld1;
//    quint16 senderPortheld1;

//    int masterworld = 0;
//    bool teleporting = false;
//    QByteArray increment7607 = "002E";
//    QString holdmasterx = "";
//    QString holdmastery = "";
//    QString holdmasterz = "";
//    QByteArray mastertelinc = "0000";

//    QByteArray teleport_ip = "";
//    QByteArray C_9007_servidcount = "D37C";
//    QByteArray C_9007_endcount = "0200"; //starts at 0200 because 275 is 0200 (will be incremented)
//    QByteArray C_closer9007_endcount = "0000"; //may not need
//    QByteArray Clients_Last_FB_MessageNumber = "0000";

    const QByteArray packetfc1 = "d37c123487c001bf543213d9bc0d002100fc02d007";
    const QByteArray packetffff = "FFFF930901000000";
    QVector<QByteArray> letmeinagain;
    bool teleportmainswitch = false;
    QTimer timer;
    bool yourmom = true;


    QByteArray levelXP = "2000028080808000";
    QByteArray Correct55550d41 = "0214050100000001000000000000000000000055550d41000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";


    //all
    const QByteArray Xor_Object_ID = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

    const QByteArray Xor_Mob_movement = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

    const QByteArray Mob_ALL_Values = "00000000000000000000";

    const QByteArray LoS_ALL_Values = "00000000000000000000";

    const QByteArray Xor_LoS_movement = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";





    QVector<QByteArray>opvect = {"12","0000","0409","0100","0400","0A00","0B00","0D00","0E00","0F00","1000","1300","1400","1500","1600",
                                 "1800","1900","1A00","1D00","1E00","2000","2100","2B00","2C00","2D00","3200","3300","3400","3500","3A00","3B00",
                                 "3C00","3D00","3E00","3F00","4000","4100","4300","4400","4600","4900","4A00","4B00","4C00","5200","5600",
                                 "5700","6000","6100","6200","6300","6C00","6D00","7300","7C00","7D00","7E00","7F00","B100",
                                 "B400","B700","C500","C700","C800","C900","CA00","CD00","CE00","D700","D800","D900","DB00",
                                 "EC00","6603","6903","D503","6804","7805","7905","2206","2506","9E06","6007","6107","7207",
                                 "7307","7607","7807","9007","9107","A307","B007","B307","D007","D107",
                                 "F507","F607","F707","F807","FE00","2A00",
                                 "0108","4008","0409","B009","7A0A","7B0A","A10A","C40A","010D","410D","010E","020E","0010",
                                 "0510","0111","0512","0612","4D12","5012","5112","5312","5412","5512","0114","0214","0115"};




    QVector <QByteArray> SortChanelsNumbersVector =
    {
        "00","01","02","03","04","05","06","07","08","09",
        "0A","0B","0C","0D","0E","0F","10","11","12",
        "13","14","15","16","17","40","42","43"
    };

    QVector <QByteArray> SortChanelsValuesVector =
    {
        "0000","0000","0000","0000","0000","0000","0000","0000","0000","0000",
        "0000","0000","0000","0000","0000","0000","0000","0000","0000",
        "0000","0000","0000","0000","0000","0000","0000","0000"
    };

    int TestInt = 0;

    QVector <QString> ZonesNamesVector;
    QVector <int> ZonesNumbersVector;

    QMap <QString,int> ZoneNames_NumbersMap;

    QVector <int> PacketIDnewVector;
    QVector <QString> NPCsNewIDVector;
    QVector <QString> NPCsNewNameVector;
    QVector <QString> NPCsNewOrNotVector;
    QVector <int> MergerZoneNumber;
    QVector <QString> ReUseNPCsVector;
    QMap <QByteArray,QVector <QByteArray>> OpcodesAndGamebuf1Map;
    QVector <QString> AlreadyInRoamersVector;
    QVector <QString> WorldNameVector =
    {
        "Tunaria","Rathe Mountians","Odus","Lavastorm Mountians","Plane of Sky","Secrets"
    };

    QVector <QComboBox*> ModelIDsVector;

    QVector <QString> BlockModels =
    {
        "15E0EB26","1E33B6C4","2C35826A","634BF977",
        "CCB2E56D","3E10093B","C698D870","3573530D",
        "F86FD5D8","F71989E1","156B9CA9","FC84404C",
        "DF3ECA61","16D54AC4","E23FDBA3","31A7278A",
        "47045BAA","BEFD0B35","32A56F4C","0127E34D",
        "470B0C32","BE3C1A4F","AA9DD87E","394B9B8C",
        "90748084","4F0BC0AC"
    };

    QMap <int,QString> SpawnMarkersMap;

    QMap <int,QString> Counters_IDsMap;
    QMap <QString, QMap <int,QString>> Multi_PathMaps;

    QMap <int,QString> PathMaker =
    {
        {0,""}
    };

    QMap <QString,int> MarkerLocations;

    int MarkerINC = 0;

    QVector <QByteArray> OutgoingPackets;

    QVector<int> SortResendVector;
    QVector<int> FB_ResendVector;
    int resendingFBCounter = 0;
    int resendingChannelCounter = 0;
    int resending42ecCounter = 0;
    QMap <QByteArray,QByteArray> ResendFBsMap;
    QMap <QByteArray,QByteArray> Resend42ecsMap;
    QMap <QByteArray,QByteArray> ResendAllChannelsMap;
    QVector <QString> IsItNew;
    QStringList StartingIDXZYF;
    QStringList EndingIDXZYF;

    QMap <QByteArray,QByteArray> MasterMap2;



    QByteArray Charm_Lock = "00";
    QByteArray Facing_Lock = "00";
    QByteArray Facing_Lock2 = "00";
    QByteArray Dismiss_Lock = "00";
    QByteArray Spellbinder_counter = "0000";
    QByteArray Pet_Trip = "00";
    QByteArray Pet_Trip2 = "00";
    QByteArray Cant_Charm = "00";
    QByteArray Self_Target = "00";
    QByteArray Out_of_DistanceRange = "00";
    QByteArray Out_of_LevelRange = "00";
    QByteArray Pet_Attack = "00";
    QByteArray Charm_recovery = "00";
    QByteArray REcharm = "00";
    QByteArray Out_Matched = "00";
    QByteArray Remove_This_NPC = "";

    const QByteArray Header_Pet_start = "XXXXXXXXXXXX01bf543213d9bc0d00xxxxfb069a017e00CCE50000xxc9xxxx01";
    QString Pet_ID = "";
    QString Pet_Chan = "";

    QByteArray Pet_Follow = "";
    QByteArray PetConOld = "";



    QString RespawnID = "";
    QString NewspawnID = "";



    QTcpSocket *tcpsocket;
    QTcpSocket *tcpsocket2;
    QTcpSocket *tcpsocket3;
    QTcpServer server2;
    QTcpServer server;
    QTcpServer server3;

    QUdpSocket udpsocket;
    QUdpSocket udpsocket2;
    QUdpSocket udpsocket3;
    QUdpSocket udpsocket4;

    QString Table_Name = "";
    QString Column_Name = "";
    QByteArray Column_value = "";

    QByteArray DB_Trip = "false";

    int currentxor = 0;
    int xorresult1 = 0;
    int xorresult2 = 0;
    int xorresult3 = 0;
    int res1 = 0;
    int res2 = 0;
    int res3 = 0;
    int xdeconumber1 = 0;
    int ydeconumber1 = 0;
    int zdeconumber1 = 0;

    int faceresult1 = 0;
    int res4 = 0;
    int facedeconumber1 = 0;

    QByteArray HolderPacket = "";







    int RandomizerLevel = 0;

    QMap <QString,int> RandomSpawnValues;
    QMap <QString,int> RandomSpawnLevels;

    QMap <QString,QVector<QString>> RandomSpawns =
    {
        {"71E50000",{"71E50000","73E50000"}},
        {"74E50000",{"71E50000","73E50000"}},
        {"75E50000",{"71E50000","73E50000"}},
        {"77E50000",{"71E50000","73E50000"}},
        {"78E50000",{"71E50000","73E50000"}},
        {"79E50000",{"71E50000","73E50000"}},
        {"7AE50000",{"71E50000","73E50000"}},
        {"83E50000",{"71E50000","73E50000"}},
        {"84E50000",{"71E50000","73E50000"}},
        {"AA071000",{"71E50000","73E50000"}},
        {"AB071000",{"71E50000","73E50000"}},
        {"AC071000",{"71E50000","73E50000"}},
        {"AF071000",{"71E50000","73E50000"}},
        {"B0071000",{"71E50000","73E50000"}},
        {"B1071000",{"71E50000","73E50000"}},

        //pickclaw shawman
        {"73E50000",{"71E50000","73E50000"}},
        {"76E50000",{"71E50000","73E50000"}},
        {"7BE50000",{"71E50000","73E50000"}},
        {"7DE50000",{"71E50000","73E50000"}},
        {"7EE50000",{"71E50000","73E50000"}},
        {"81E50000",{"71E50000","73E50000"}},
        {"85E50000",{"71E50000","73E50000"}},
        {"86E50000",{"71E50000","73E50000"}},
        {"AE071000",{"71E50000","73E50000"}},
        {"B2071000",{"71E50000","73E50000"}},
        {"B3071000",{"71E50000","73E50000"}}
    };







    QMap <QString,QString> reSpawnIDsMap;








    QVector <QString> AllPacketDescriptionVector;




    QByteArray CheckTheAggro = "YES";









    QByteArray ItemsFull = "True";
    QByteArray NewItem = "False";
    QString Buy_ItemTotal = "";
    QByteArray NotStackable = "True";
    QString ConvertedValue = "";
    QString CantAffordIt = "True";
    QByteArray InventoryFullHeader = "D700";
    QString RemoveADDQuantity = "";


    QByteArray Plus1 = "";







    QVector <QString> FactionIDVector;
    QVector <int> FactionValueVector;


    QMap <int,int> XPtoNextLevel =
    {
        {1,420},{2,2185},{3,6975},{4,17125},{5,35640},
        {6,66195},{7,113150},{8,181520},{9,277000},{10,405955},
        {11,575420},{12,793115},{13,1067415},{14,1407375},{15,1822720},
        {16,2323845},{17,2921830},{18,3628410},{19,4456000},{20,5417685},
        {21,6527220},{22,7799045},{23,9248255},{24,10890625},{25,12742600},
        {26,14821295},{27,17144510},{28,19730700},{29,22599000},{30,25769215},
        {31,29261820},{32,33097975},{33,37299495},{34,41888875},{35,46889280},
        {36,52324545},{37,58219190},{38,64598390},{39,71488000},{40,78914545},
        {41,86905220},{42,95487905},{43,104691135},{44,114544125},{45,125076760},
        {46,136319595},{47,148303870},{48,161061480},{49,174625000},{50,189027675},
        {51,204303420},{52,220486835},{53,237613175},{54,255718375},{55,274839040},
        {56,295012445},{57,316276550},{58,338669970},{59,362232000}
    };

    QVector <QString> ToltalXPperTPsVector =
    {

        "140","280","420",
        "1148","1876","2605",
        "4930","7255","9580",
        "15288","20996","26705",
        "38585","50465","62345",
        "84410","106475","128540",
        "166256","203972","241690",
        "302196","362702","423210",
        "515543","607876","700210",
        "835528","970846","1106165",
        "1297971","1489777","1681585",//check level 11 to see when you ding from 10
        "1945956","2210327","2474700",
        "2830505","3186310","3542115",
        "4011240","4480365","4949490",

        //5 tps per level
        "5314034","5678578","6043122","6407666","6772210",
        "7236979","7701748","8166517","8631286","9096055",
        "9680421","10264787","10849153","11433519","12017885",
        "12743567","13469249","14194931","14920613","15646295",
        "16537495","17428695","18319895","19211095","20102295",
        "21185832","22269369","23352906","24436443","25519980",
        "26825424","28130868","29436312","30741756","32047200",
        "33607009","35166818","36726627","38286436","39846245",
        "41695896","43545547","45395198","47244849","49094500",
        "51272625","53450750","55628875","57807000","59985125",
        "62533645","65082165","67630685","70179205","72727725",
        "75691984","78656243","81620502","84584761","87549020",
        "90977922","94406824","97835726","101264628","104693530",
        "108639670","112585810","116531950","120478090","124424230",
        "128944030","133463830","137983630","142503430","147023230",

        //9 tps per level
        "149886476","152749722","155612968","158476214","161339460","164202706","167065952","169929198","172792445",
        "176043758","179295071","182546384","185797697","189049010","192300323","195551636","198802949","202054265",
        "205731817","209409369","213086921","216764473","220442025","224119577","227797129","231474681","235152240",
        "239296628","243441016","247585404","251729792","255874180","260018568","264162956","268307344","272451735",
        "277106054","281760373","286414692","291069011","295723330","300377649","305031968","309686287","314340610",
        "319550530","324760450","329970370","335180290","340390210","345600130","350810050","356019970","361229890",
        "367043728","372857566","378671404","384485242","390299080","396112918","401926756","407740594","413554435",
        "420023233","426492031","432960829","439429627","445898425","452367223","458836021","465304819","471773625",
        "478951223","486128821","493306419","500484017","507661615","514839213","522016811","529194409","536372015",
        "544315126","552258237","560201348","568144459","576087570","584030681","591973792","599916903","607860015",
        "616628297","625396579","634164861","642933143","651701425","660469707","669237989","678006271","686774560",
        "696430695","706086830","715742965","725399100","735055235","744711370","754367505","764023640","773679780",
        "784289547","794899314","805509081","816118848","826728615","837338382","847948149","858557916","869167685",
        "880800033","892432381","904064729","915697077","927329425","938961773","950594121","962226469","973858820",
        "986585945","999313070","1012040195","1024767320","1037494445","1050221570","1062948695","1075675820","1088402945",

        //14 tps per level
        "1097336999","1106271053","1115205107","1124139161","1133073215","1142007269","1150941323","1159875377","1168809431","1177743485","1186677539","1195611593","1204545647","1213479705",
        "1223216818","1232953931","1242691044","1252428157","1262165270","1271902383","1281639496","1291376609","1301113722","1310850835","1320587948","1330325061","1340062174","1349799300",
        "1360392433","1370985566","1381578699","1392171832","1402764965","1413358098","1423951231","1434544364","1445137497","1455730630","1466323763","1476916896","1487510029","1498103170",
        "1509607561","1521111952","1532616343","1544120734","1555625125","1567129516","1578633907","1590138298","1601642689","1613147080","1624651471","1636155862","1647660253","1659164650",
        "1671637864","1684111078","1696584292","1709057506","1721530720","1734003934","1746477148","1758950362","1771423576","1783896790","1796370004","1808843218","1821316432","1833789650",
        "1847291626","1860793602","1874295578","1887797554","1901299530","1914801506","1928303482","1941805458","1955307434","1968809410","1982311386","1995813362","2009315338","2022817325",
        "2037410426","2052003527","2066596628","2081189729","2095782830","2110375931","2124969032","2139562133","2154155234","2168748335","2183341436","2197934537","2212527638","2227120745",
        "2242869804","2258618863","2274367922","2290116981","2305866040","2321615099","2337364158","2353113217","2368862276","2384611335","2400360394","2416109453","2431858512","2447607580",
        "2464579949","2481552318","2498524687","2515497056","2532469425","2549441794","2566414163","2583386532","2600358901","2617331270","2634303639","2651276008","2668248377","2685220755",
        "2703486353","2721751951","2740017549","2758283147","2776548745","2794814343","2813079941","2831345539","2849611137","2867876735","2886142333","2904407931","2922673529","2940939130",
        "2960570490","2980201850","2999833210","3019464570","3039095930","3058727290","3078358650","3097990010","3117621370","3137252730","3156884090","3176515450","3196146810","3215778170",
        "3236850487","3257922804","3278995121","3300067438","3321139755","3342212072","3363284389","3384356706","3405429023","3426501340","3447573657","3468645974","3489718291","3510790615",
        "3533381797","3555972979","3578564161","3601155343","3623746525","3646337707","3668928889","3691520071","3714111253","3736702435","3759293617","3781884799","3804475981","3827067165",
        "3851257877","3875448589","3899639301","3923830013","3948020725","3972211437","3996402149","4020592861","4044783573","4068974285","4093164997","4117355709","4141546421","4165737135",
        "4191610849","4217484563","4243358277","4269231991","4295105705","4320979419","4346853133","4372726847","4398600561","4424474275","4450347989","4476221703","4502095417","4527969135"

    };


    QVector <int> TPsPerToltalXPVector =
    {
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35,
        36, 37, 38,
        39, 40, 41,
        42, 43, 44,
        45, 46, 47,
        48, 49, 50,
        51, 52, 53,
        54, 55, 56,
        57, 58, 59,
        60, 61, 62,
        63, 64, 65, 66, 67,
        68, 69, 70, 71, 72,
        73, 74, 75, 76, 77,
        78, 79, 80, 81, 82,
        83, 84, 85, 86, 87,
        88, 89, 90, 91, 92,
        93, 94, 95, 96, 97,
        98, 99, 100, 101, 102,
        103, 104, 105, 106, 107,
        108, 109, 110, 111, 112,
        113, 114, 115, 116, 117,
        118, 119, 120, 121, 122,
        123, 124, 125, 126, 127,
        128, 129, 130, 131, 132,
        133, 134, 135, 136, 137,
        138, 139, 140, 141, 142, 143, 144, 145, 146,
        147, 148, 149, 150, 151, 152, 153, 154, 155,
        156, 157, 158, 159, 160, 161, 162, 163, 164,
        165, 166, 167, 168, 169, 170, 171, 172, 173,
        174, 175, 176, 177, 178, 179, 180, 181, 182,
        183, 184, 185, 186, 187, 188, 189, 190, 191,
        192, 193, 194, 195, 196, 197, 198, 199, 200,
        201, 202, 203, 204, 205, 206, 207, 208, 209,
        210, 211, 212, 213, 214, 215, 216, 217, 218,
        219, 220, 221, 222, 223, 224, 225, 226, 227,
        228, 229, 230, 231, 232, 233, 234, 235, 236,
        237, 238, 239, 240, 241, 242, 243, 244, 245,
        246, 247, 248, 249, 250, 251, 252, 253, 254,
        255, 256, 257, 258, 259, 260, 261, 262, 263,
        264, 265, 266, 267, 268, 269, 270, 271, 272,
        273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286,
        287, 288, 269, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300,
        301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314,
        315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328,
        329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342,
        343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356,
        357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370,
        371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384,
        385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398,
        399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412,
        413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426,
        427, 428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439, 440,
        441, 442, 443, 444, 445, 446, 447, 448, 449, 450, 451, 452, 453, 454,
        455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467, 468,
        469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 481, 482
    };


    QMap <QString,int> ClassSwingCountMap =
    {
        {"00",4},{"01",4},{"02",2},{"03",2},
        {"04",4},{"05",4},{"06",4},{"07",1},
        {"08",1},{"09",1},{"0A",1},{"0B",1},
        {"0C",1},{"0D",1},{"0E",1}
    };






    QMap <QString,QString> AnimationMap =
    {
        {"00","Nothing"},{"01","Walking"},{"02","Walking Bakwards"},{"03","Running"},
        {"04","Turning in place to the right"},{"05","Turning in place to the left"},
        {"06","Falling"},{"07","Crouching or landing"},{"0A","Side step Left"},
        {"0B","Side Step Right"},{"0C","Swim Fast"},{"0D","Swim Slow"},{"0E","Death"},
        {"0F","look left then right"},{"11","Fighting stance"},{"16","left punch"},
        {"17","right punch"},{"18","right back hand"},{"19","right punch"},
        {"1A","2 hand swing from left to right"},{"1B","left hand straight stab"},
        {"1C","Right hand straight stab"},{"1D","Right side 2 hand stab"},{"1E","left hand back hand"},
        {"1F","bow pull"},{"20","left hand punch"},{"21","bolt bow shot left side"},
        {"22","Taking a hit"},{"23","left hand hit"},{"24","pot throw? 2 hand hit"},
        {"25","left hand cast ability"},{"26","2 hands cast ability"},{"27","right hand cast ability"},
        {"28","left hand upper cut"},{"29","right hand chop"},{"2A","2 hand straigh down overhead slash"},
        {"2B","kick"},{"2C","Swimming right hand punch"},{"32","right hand uppercut punch"},
        {"33","right hand lunge punch"},{"34","round house kick"},{"35","High kick"},
        {"38","Wave"},{"39","bow"},{"3C","Point"},{"3D","Cheer"},{"3F","slow over hand right"}
    };

    QMap <QString,QString> ColorMap =
    {
        {"00","Red"},{"01","Yellow"},{"02","White"},{"03","Dark Blue"},{"04","Light Blue"},{"05","Green"}
    };

    ////////////Inventory/////////////////













    QByteArray client_order = "03000400";

    QMap <QString, QString> facing_map =
    {{"00","00000000"},
     {"01","DB0FC93C"},{"02","DB0F493D"},{"03","E4CB963D"},{"04","DB0FC93D"},{"05","D153FB3D"},
     {"06","E4CB163E"},{"07","DFED2F3E"},{"08","DB0F493E"},{"09","D631623E"},{"0A","D1537B3E"},
     {"0B","E63A8A3E"},{"0C","E4CB963E"},{"0D","E15CA33E"},{"0E","DFEDAF3E"},{"0F","DD7EBC3E"},
     {"10","DB0FC93E"},{"11","D8A0D53E"},{"12","D631E23E"},{"13","D4C2EE3E"},{"14","D153FB3E"},
     {"15","67F2033F"},{"16","E63A0A3F"},{"17","6583103F"},{"18","E4CB163F"},{"19","63141D3F"},
     {"1A","E15C233F"},{"1B","60A5293F"},{"1C","DFED2F3F"},{"1D","5E36363F"},{"1E","DD7E3C3F"},
     {"1F","5CC7423F"},{"20","DB0F493F"},{"21","59584F3F"},{"22","D8A0553F"},{"23","57E95B3F"},
     {"24","D631623F"},{"25","557A683F"},{"26","D4C26E3F"},{"27","520B753F"},{"28","D1537B3F"},
     {"29","28CE803F"},{"2A","67F2833F"},{"2B","A716873F"},{"2C","E63A8A3F"},{"2D","255F8D3F"},
     {"2E","6583903F"},{"2F","A4A7933F"},{"30","E4CB963F"},{"31","23F0993F"},{"32","63149D3F"},
     {"33","A238A03F"},{"34","E15CA33F"},{"35","2181A63F"},{"36","60A5A93F"},{"37","A0C9AC3F"},
     {"38","DFEDAF3F"},{"39","1F12B33F"},{"3A","5E36B63F"},{"3B","9D5AB93F"},{"3C","DD7EBC3F"},
     {"3D","1CA3BF3F"},{"3E","5CC7C23F"},{"3F","9BEBC53F"},{"40","DB0FC93F"},{"41","1A34CC3F"},
     {"42","5958CF3F"},{"43","997CD23F"},{"44","D8A0D53F"},{"45","18C5D83F"},{"46","57E9DB3F"},
     {"47","960DDF3F"},{"48","D631E23F"},{"49","1556E53F"},{"4A","557AE83F"},{"4B","949EEB3F"},
     {"4C","D4C2EE3F"},{"4D","13E7F13F"},{"4E","520BF53F"},{"4F","922FF83F"},{"50","D153FB3F"},
     {"51","1178FE3F"},{"52","28CE0040"},{"53","48600240"},{"54","67F20340"},{"55","87840540"},
     {"56","A7160740"},{"57","C6A80840"},{"58","E63A0A40"},{"59","06CD0B40"},{"5A","255F0D40"},
     {"5B","45F10E40"},{"5C","65831040"},{"5D","85151240"},{"5E","A4A71340"},{"5F","C4391540"},
     {"60","E4CB1640"},{"61","035E1840"},{"62","23F01940"},{"63","43821B40"},{"64","63141D40"},
     {"65","82A61E40"},{"66","A2382040"},{"67","C2CA2140"},{"68","E15C2340"},{"69","01EF2440"},
     {"6A","21812640"},{"6B","41132840"},{"6C","60A52940"},{"6D","80372B40"},{"6E","A0C92C40"},
     {"6F","BF5B2E40"},{"70","DFED2F40"},{"71","FF7F3140"},{"72","1F123340"},{"73","3EA43440"},
     {"74","5E363640"},{"75","7EC83740"},{"76","9D5A3940"},{"77","BDEC3A40"},{"78","DD7E3C40"},
     {"79","FD103E40"},{"7A","1CA33F40"},{"7B","3C354140"},{"7C","5CC74240"},{"7D","7B594440"},
     {"7E","9BEB4540"},{"7F","BB7D4740"},{"80","DB0F49C0"},{"81","BB7D47C0"},{"82","9BEB45C0"},
     {"83","7B5944C0"},{"84","5CC742C0"},{"85","3C3541C0"},{"86","1CA33FC0"},{"87","FD103EC0"},
     {"88","DD7E3CC0"},{"89","BDEC3AC0"},{"8A","9D5A39C0"},{"8B","7EC837C0"},{"8C","5E3636C0"},
     {"8D","3EA434C0"},{"8E","1F1233C0"},{"8F","FF7F31C0"},{"90","DFED2FC0"},{"91","BF5B2EC0"},
     {"92","A0C92CC0"},{"93","80372BC0"},{"94","60A529C0"},{"95","411328C0"},{"96","218126C0"},
     {"97","01EF24C0"},{"98","E15C23C0"},{"99","C2CA21C0"},{"9A","A23820C0"},{"9B","82A61EC0"},
     {"9C","63141DC0"},{"9D","43821BC0"},{"9E","23F019C0"},{"9F","035E18C0"},{"A0","E4CB16C0"},
     {"A1","C43915C0"},{"A2","A4A713C0"},{"A3","851512C0"},{"A4","658310C0"},{"A5","45F10EC0"},
     {"A6","255F0DC0"},{"A7","06CD0BC0"},{"A8","E63A0AC0"},{"A9","C6A808C0"},{"AA","A71607C0"},
     {"AB","878405C0"},{"AC","67F203C0"},{"AD","486002C0"},{"AE","28CE00C0"},{"AF","1178FEBF"},
     {"B0","D153FBBF"},{"B1","922FF8BF"},{"B2","520BF5BF"},{"B3","13E7F1BF"},{"B4","D4C2EEBF"},
     {"B5","949EEBBF"},{"B6","557AE8BF"},{"B7","1556E5BF"},{"B8","D631E2BF"},{"B9","960DDFBF"},
     {"BA","57E9DBBF"},{"BB","18C5D8BF"},{"BC","D8A0D5BF"},{"BD","997CD2BF"},{"BE","5958CFBF"},
     {"BF","1A34CCBF"},{"C0","DB0FC9BF"},{"C1","9BEBC5BF"},{"C2","5CC7C2BF"},{"C3","1CA3BFBF"},
     {"C4","DD7EBCBF"},{"C5","9D5AB9BF"},{"C6","5E36B6BF"},{"C7","1F12B3BF"},{"C8","DFEDAFBF"},
     {"C9","A0C9ACBF"},{"CA","60A5A9BF"},{"CB","2181A6BF"},{"CC","E15CA3BF"},{"CD","A238A0BF"},
     {"CE","63149DBF"},{"CF","23F099BF"},{"D0","E4CB96BF"},{"D1","A4A793BF"},{"D2","658390BF"},
     {"D3","255F8DBF"},{"D4","E63A8ABF"},{"D5","A71687BF"},{"D6","67F283BF"},{"D7","28CE80BF"},
     {"D8","D1537BBF"},{"D9","520B75BF"},{"DA","D4C26EBF"},{"DB","557A68BF"},{"DC","D63162BF"},
     {"DD","57E95BBF"},{"DE","D8A055BF"},{"DF","59584FBF"},{"E0","DB0F49BF"},{"E1","5CC742BF"},
     {"E2","DD7E3CBF"},{"E3","5E3636BF"},{"E4","DFED2FBF"},{"E5","60A529BF"},{"E6","E15C23BF"},
     {"E7","63141DBF"},{"E8","E4CB16BF"},{"E9","658310BF"},{"EA","E63A0ABF"},{"EB","67F203BF"},
     {"EC","D153FBBE"},{"ED","D4C2EEBE"},{"EE","D631E2BE"},{"EF","D8A0D5BE"},{"F0","DB0FC9BE"},
     {"F1","DD7EBCBE"},{"F2","DFEDAFBE"},{"F3","E15CA3BE"},{"F4","E4CB96BE"},{"F5","E63A8ABE"},
     {"F6","D1537BBE"},{"F7","D63162BE"},{"F8","DB0F49BE"},{"F9","DFED2FBE"},{"FA","E4CB16BE"},
     {"FB","D153FBBD"},{"FC","DB0FC9BD"},{"FD","E4CB96BD"},{"FE","DB0F49BD"},{"FF","DB0FC9BC"}
    };



    QVector <QString> RH_LH_IndexVector =
    {
        {"00000000","A6B24D02","5B394B70","460E9660","2BB7C0B7",
         "0B7BF220","FA5A260A","4D3B5179","02EC0730","2A6E2E4C",
         "468FBADE","9A87A90D","368A8CEF","9ED31609","3FD2F8FE",
         "A2B41B8F","73B461C8","EA711BF5","A10CDD3B","BA430457",
         "E3301F89","5A85BE83","16BAD08D","CBAAAF4E","9E563809",
         "7CD77A09","9D755D33","42F89E35","F8439CCF","84AC1D36",
         "2D09EC88","D40F342B","4F38A99E","1A99DDF4","A6E4858E",
         "856C40D8","986A11B2","DE4239D0","5A13F4CB","8950DA2E",
         "A655F9DA","A6C24C78","ABEFDC63","A5959903","AF29CBAA",
         "CD5E871D","7CAED45F","58389CFD","05B6828A","CE7A87F2",
         "6D2641F8","696A6611","801EA4F3","297D6BAB","D4A361DD",
         "C83D3CBD","8CF8540D","92BA1D9A","4D22B76E","77D57E86",
         "5A4C03DF","E97C1038","D3352794","967A4EB4","0B87F917",
         "3463B329","A9BE7D7C","2741FE40","707F7E09","29E62F46",
         "2B680BBE","31514A86","8582F37F","FB26DE0C","C381B8AA",
         "3AF8725C","13249D64","6EDADB25","62444F94","2EF3E4CB",
         "53FA6A0A","E2591E1D","615B27E3","D6694721","302AB748",
         "CDB7993B","EA1BCF22","03AF866C","F6242811","1FB48D8E",
         "E40AB467","0CBA465D","035171A7","8C0CFDD2","A5422B3C",
         "FE169F72","7A8F29A6","28E54612","9BFF115C","60F27C09",
         "E9ADD5B4","9F63A231","3D05F023","1023456B","5B80DA16",
         "A8F5CA9C","599EEE11","33AD3736","B2F81866","74650AD4",
         "2588F0FF","E081D947","3F1988F4","95C174BC","3D2FE3FE",
         "16F16A0A","9DDA5C5A","9BF87055","FAA4108F","EC01A793",
         "D2232721","F7ED1500","3A7B7CD1","6A9CACD2","0B9105EC",
         "44FA9436","B1CFE2CB","64B86194","BA4197C1","B3882E43",
         "E9C1FDFE","D1ADDE4C","813FCBCA","F29A9322","369798FB",
         "6264BEC8","903B0766","40748B47","7A7F258C","8D47F9E6",
         "F32D8DED","14214E34","649A06D9","F5F235D3","DF7C64CC",
         "D2CBA9B0","C120FDDC","9FD996FF","B5B40EC0","ED3BF91B",
         "FE8893B7","85350063","C4596478","DBFFC8AD","5BA8679D",
         "E97A9183","2BAF6142","29CD753D","CCA06D02","E9529A4E",
         "F7D146D1","DDEC5F84","954AF508","37837045","B5964A06",
         "38C20F2D","CB7D7EBF","78894386","C0FBEFDF","40AD9D69",
         "A8E42C99","ED74F777","093E55CE","72A5B9A2","DF7669A3",
         "9376BC1E","8134EDDB","8B36C4BE","64D2FAF5","19561749",
         "FD8B89D3","381D6F4A","7C8B080C","7DB1523E","6471B08B",
         "55171856","1D29945B","944849A7","AAD2B4A9"}
    };

    QVector <QString> Shield_IndexVector =
    {
        {"00000000","DFB1236B","73E8A60A","38393159",
         "2960C52E","77F93C34","C4CFB7D1","C5C01ED2",
         "C05657D2","B0BA6A01","291CA522","686552D3",
         "D7A88624"}
    };

    QVector <QString> MobBox1_Index =
    {
        {"09E79C1C","0AE79C1C","0BE79C1C","0CE79C1C","7DA11AD9",
         "01BF1406","02BF1406","03BF1406","04BF1406","50D95A48",
         "51D95A48","52D95A48","53D95A48","D5BC72DA","60AA64AE",
         "61AA64AE","62AA64AE","63AA64AE","23A3EDDB","5E345C01",
         "9E926302","43658088","E1BF0E3E","8FE90163","24871D2C",
         "E2201291","05400206","6E0834E8","5747009E","E08D0912",
         "81073811","AFE65081","7095A682","3DEA4A7E","3EEA4A7E",
         "3FEA4A7E","40EA4A7E","59E1F830","5AE1F830","5BE1F830",
         "5CE1F830","7C513FF2","7D513FF2","7E513FF2","7F513FF2",
         "72FDFA13","73FDFA13","74FDFA13","75FDFA13","A6A9D487",
         "A7A9D487","A8A9D487","A9A9D487","C1DB1715","C2DB1715",
         "C3DB1715","C4DB1715","72391981","73391981","74391981",
         "75391981","A81848AC","A91848AC","AA1848AC","AB1848AC",
         "F0785E12","F1785E12","F2785E12","F3785E12"}
    };

    QVector <QString> MobBox2_Index =
    {
        {"1F528737","20528737","21528737","17B40BD7","ECAAB31E",
         "025315E7","035315E7","045315E7","9202B264","D7F1DC90",
         "3F62418A","4062418A","4162418A","3632BCEB","BB9C48AA",
         "1F3820B6","203820B6","213820B6","04A14E2F","E91FDADB",
         "FB7039ED","F24DCCAB","04066AEC","E3048CEE","368B3214",
         "735FCE87","9258B195","3C352100","4D343BDC","4E343BDC",
         "4F343BDC","50343BDC","8B89BB1B","8C89BB1B","8D89BB1B",
         "AA2F0CF6","8CD846BB","8DD846BB","8ED846BB","0D5E7123",
         "64AA8B19","C7517F19","69A36E55","828AF96A","51F0C9D8",
         "52F0C9D8","53F0C9D8","57F62B58","507C01DC","60C480DC",
         "A150C365","F966BDE3","2A0E5E6E","4D134865","2587406E",
         "25B956EC","667CFA29","96A21D34","A1645AEC"}
    };

    QVector <QString> MobBox3_Index =
    {
        {"7DAEE009","70FC7B09","BCD63F22","D30BBA16","DC50DA2E",
         "EB0FDA17","EEC37A14","F720E316","1C03D558","8E679274",
         "9EAB9B4A","37F2556F","77366D3C","CA63F171","D95E5E58",
         "5E7306CE","615541CF","A54FB9C3","DA2856B1","FC1DB2B7",
         "50A4F3F5"}
    };

    QVector <QString> MobBox4_Index =
    {
        {"15E0EB26","1E33B6C4","2C35826A","634BF977","CCB2E56D",
         "3E10093B","0AA55331","010FD271","59EA25A2","70B67C4B",
         "808AF96A","A360C1AD","75688A82","7F0391DC","D61D89FC",
         "8B89BB1B","F44FEFC1","188C51A2","843B929C","C12CF08A",
         "F366FFD5","F244F508","7C5C9596","1BC5F540","54A46FAD",
         "61D57D7C","B938AAD6","21546842","271BA585","EDA413D0",
         "985B1809","B6AF5B47","71300161","FF6EE82E","A3054F8C",
         "BE4BE4C4","C021872F","CB09E7AA","D6771AA9","F09ADC44"}
    };

    QVector <QString> MobBox5_Index =
    {
        {"02849A7A","5F283D77","271BA585","EDA413D0","15394D05",
         "3B234041","5A0331FE","8CD36801","A5BB1536","EADC0618",
         "0D4BD870","23E7D00E","3AFEFC3B","34BF1729","2D487036",
         "27AB531C","DB4D4DF9","FB859789","9E52D1EE","33167DC2",
         "563F5BD2","5F01C20B","8A0C9D85","2D58E906","41E8FD6A",
         "8ED809BA","D76E7FC3","07266140","2995D465","2A6DB8E8",
         "5664BD14","D656D7F9","D8B2EC13","F391B9CA","BDF1BD15",
         "A84D7615","61BD3D9D","EA881123","79502D38","AE84A21F",
         "AB2D1474","7CE5A967","54CAB966"}
    };

    QVector <QString> MobBox6_Index =
    {
        {"659BF505","CC2708E3","2CEAB1FF","CB0145F3","27C61781",
         "9E6E6C28","3B3017E6","52652A8E","BBFDA872","DF04267F",
         "7269682D","DBDF6ABF","040CDFBC","04183796","42214CF5",
         "7266B93A","D3118E1F","EB8508EC","D47961E9","432FF9A5",
         "54E5A427","6BE54512","524A7C50","22EA3759","E008136C",
         "ADE0B4ED","806A1B58","45767E68","5AB9CB2E","EADF56B6",
         "ED077C6C","BADF1DEE","857B92BE","4A87F5CE","67B8342F",
         "EFF0CD1C","57A1726F","DF28552D","4399D2F6","160F6B47"}
    };

    QVector <QString> MobBox7_Index =
    {
        {"2DA31482","DA3B5801","6777F484","92EB030A","C5D46D65",
         "EFD7FBB2","6AFE74FF","625951DA","08D20502","B1F59831",
         "2F9444E1","A81F6FC7","F913FD37","A7AD86B0","05088BEA",
         "1290230F","0CFE1B14","AB049A42","5E32176F","7DE95F39",
         "F30E45AF","1CEC6316","975D3C06","B5257359","EE78C4CF",
         "16709B6B","2D3CDF9C","175FA131","584FECC3","247074F6",
         "33B9FAF0","3FB1B904","C66EEE95","610CB7D1","35A9070C",
         "78C9A8E0","1BD2904E","9DC9DF4F","5EC21EFD","4AB30A7E"}
    };

    QVector <QString> MobBox8_Index =
    {
        {"021DE70A","CFF4CBC5","A926BA46","C925DCBA","6A83CC0E",
         "815EF79C","00EBABFB","1940B922","BC1B6C32","08F08EC2",
         "6F2210BE","18DB7A09","05CEEF70","BB11F321","D2DD0AB9",
         "261EE7AC","83801BCE","DA0F54D7","0F82D4BF","B727FEA1",
         "E82C6723","806DA1C1","ED9873D7","AA1478DE","904D0270",
         "A8B62D20","36C8639C","56A59326","D7CF4155","F44FEFC1",
         "D55AA166","46EB9DB2","BD1B45CA","05F17955","0BA64644",
         "70DC4381","8D78F3C5","FCD97FF9","9F414883","E8781ED8"}
    };

    QVector <QString> MobBox9_Index =
    {
        {"5747009E","6E0834E8","11FC8D64","B2419F7D","818AF96A",
         "828AF96A","64B0A845","C7517F19","64AA8B19","6F6AF190",
         "69A36E55","8C89BB1B","8CD846BB","8DD846BB","8ED846BB",
         "0D5E7123","8D89BB1B","AA2F0CF6","9910DD75","0942D65C",
         "FC312021","3C8D55A8","9E830F5D","70B67C4B","808AF96A",
         "010FD271","59EA25A2","A360C1AD","8B89BB1B","51F0C9D8",
         "52F0C9D8","53F0C9D8","7D1E1448","21611D5B","A3635AEC",
         "57F62B58","507C01DC","60C480DC","A150C365","F966BDE3"}
    };

    QVector <QString> MobBox10_Index =
    {
        {"022C8469","6544FBBE","2835F3E3","47F6D30A","D1A589E0",
         "3F414B7F","65F7AE9C","A061EB2A","4B346575","A5E04ADF",
         "84275467","E534EA55","44C0958F","5FEB6B75","19FA13D9",
         "EE4A0E0B","B24B2CE4","EA3635C0","9B187B0A","5EEBF88D",
         "BE57EA3F","E8445388","A5D0DBC4","92A63817","9E245410",
         "78EB16A3","3D7CBA74","F5FE21BF","7C424017","8FEE75D2",
         "843B929C","962E8791","A65F8917","F5A17152","1C2F2666",
         "47273946","31834649","52EB7643","6C64857A","CD77BA94"}
    };

    QVector <QString> MobBox11_Index =
    {
        {"DA0F54D7","83801BCE","23A3EDDB","D5BC72DA","7DA11AD9",
         "7095A682","24871D2C","E2201291","249EBB16","1F9F5BD4",
         "A56DBFEE","E7EA6874","5FAAFE39","208FFA78","3CAC5476",
         "80F9E0D8","57BF49B7","E49220B6","53DE00E7","8740DD0D",
         "58621722","EF539C4D","B8E7A777","ABC8405D","EA12FD8E",
         "13DF5131","04B2EEB1","67D13BF8","2A572C26","8527338B",
         "ED5F6B24","00F5EF41","07EC4CC0","3C148555","551064DC",
         "CAE63D3C","4EC2F8FC","EDBB74F2","F02D93BB"}
    };

    QVector <QString> MobBox12_Index =
    {
        {"7F0E049C","4C5FDFD9","74BAFAAD","99DE3BBA","740E32A6",
         "D30B30D1","EFCEA299","F4712E09","5C246BAC","B9408D0A",
         "175BF7D8","92A996D0","616E6729","DA1F5B04","E96E4466",
         "79F58D03","808EF6AC","F35274AC","14341F03","3137ACB7",
         "F8053AED","E39005A2","3AC3D78C","601574D7","9F22675A",
         "76F2D095","65AA8759","5A9566D5","69106CBF","BA4D1CF4",
         "C9AB1063","DF9E1D00","914EC608","B9EEBC9B","B3457C90"}
    };
    QVector <QString> MobBox13_Index =
    {
        {"2F064C08","D99CC9E4","F7AB83F4","FBD53642","223F8FA1",
         "465F2C00","4FE80363","62B76F1B","09AB1BF5","47631F49",
         "7EFF244B","8A33258A","C8ADB0E8","FB884D4A","FCAC7458",
         "5387B096","838AF96A","2AF355ED","7A2A5708","D033F580",
         "71AD2380","9D98ADC6","085AEEC4","FF5E17FA","A0D845F9",
         "822BCB94","EDEC0B93","5C942A4E","32D1D2B4","C8551300",
         "D99F956D","B5BA0493","F6D9C5AE","BE010AFB","0DA59565",
         "18262EFB","4E279A5C","6DF2CC5E","0BEC1784","1F528737"}
    };
    QVector <QString> MobBox14_Index =
    {
        {"C021872F","006A1500","B149C80B","D7653D36","40E6D87F",
         "8B37ABA3","EC215745","BEF2F1A4","FFEF52BA","F88B0444",
         "5CAEC92A","7565DD41","C2B7D85B","3174585F","6864EA8D",
         "145CF35E","0B9E5BAF","422C7B95","7ECDC704","BD1B45CA",
         "05F17955","46EB9DB2","D55AA166","F44FEFC1","60C77652",
         "AE24A100","ABA114D9","83FF5187","914110A7","17B06A61",
         "BEB99CA7","A99F692A","0667BAE7","FF7581A3","36252907",
         "47832D06","31B6F918","D88D2BD3","C6DDE428","51A6C8B5",
         "477746B0"}
    };

    QVector<QVector<QString>> ModelComboBoxIDsVectors =
    {
        MobBox1_Index,MobBox2_Index,MobBox3_Index,MobBox4_Index,MobBox5_Index,MobBox6_Index,MobBox7_Index,
        MobBox8_Index,MobBox9_Index,MobBox10_Index,MobBox11_Index,MobBox12_Index,MobBox13_Index,MobBox14_Index
    };

    QVector <QString> MobFolder =
    {
        "Mob Forms1","Mob Forms2","Mob Forms3","Mob Forms4","Mob Forms5","Mob Forms6","Mob Forms7",
        "Mob Forms8","Mob Forms9","Mob Forms10","Mob Forms11","Mob Forms12","Mob Forms13","Mob Forms14"
    };

    QVector <QString> WeaponHeldShieldFolder =
    {
        "Weapons","Weapons","Shields"
    };

    QVector <QString> RaceAndGenderFolder =
    {
        "Barbarian Male","Barbarian Female","Dark Elf Male","Dark Elf Female",
        "Dwarf Male","Dwarf Female","Elf Male","Elf Female","Erudite Male",
        "Erudite Female","Gnome Male","Gnome Female","Halfling Male","Halfling Female",
        "Human Male","Human Female","Ogre Male","Ogre Female","Troll Male","Troll Female"
    };

    QVector <QString> ArmorFolders =
    {
        "Torso","Forearms","Gloves","Legs","Feet","Head","Robes"
    };

    QVector <int> ArmorPositionVector =
    {
        0,0,0,0,0,0,0
    };


    QVector <QString> GraphicValue;

    QVector <QString> GraphicDescription;








    QByteArray SizeAndSession = "xxxx01bf543213";





    QMap<QByteArray,QString> ConFaceICon =
    {
        {"00","Kill on Sight"},{"02","Neutral"},{"04","Ally"}
    };


    QMap <int,QString> Icon_4_Index_Map =
    {
        {0, "00985CE6"},{1, "01751914"},{2, "0851275A"},{3, "0CCCB2CE"},
        {4, "0D263670"},{5, "14B82D23"},{6, "1B0A7880"},{7, "1D1B6649"},
        {8, "24D2D9F2"},{9, "2E76E1F1"},{10, "31E03EAE"},{11, "38FC955A"},
        {12, "474EBD20"},{13, "49039304"},{14, "49DFCC13"},{15, "4D3DE7BE"},
        {16, "5408D940"},{17, "58F3230F"},{18, "5A91D12F"},{19, "5CAE2AA1"},
        {20, "621089E8"},{21, "65B46C8A"},{22, "664ED091"},{23, "698287E6"},
        {24, "6AB486EB"},{25, "74E6AE19"},{26, "77B9E2BA"},{27, "783C3F34"},
        {28, "7DC9641E"},{29, "7ED53967"},{30, "81338733"},{31, "8E79919D"},
        {32, "93168BCB"},{33, "947409D6"},{34, "9DBD4E62"},{35, "A0EF67EF"},
        {36, "A959739B"},{37, "A985ECC6"},{38, "AF5B2BF3"},{39, "B4ABFFD3"},
        {40, "BA5F28B5"},{41, "BABF5226"},{42, "BB96F032"},{43, "C5751A04"},
        {44, "D530D473"},{45, "DD2438AA"},{46, "EC5F4153"},{47, "ECB87813"},
        {48, "F4DBE746"},{49, "FC756DFC"},{50, "FDDF52CE"},{51, "009093DB"},
        {52, "0649176D"},{53, "12801F99"},{54, "149A12C6"},{55, "1E05401A"},
        {56, "2396F398"},{57, "29487E85"},{58, "35342ACA"},{59, "45B268AE"},
        {60, "52FABFB3"},{61, "5B61D881"},{62, "6CB61696"},{63, "6F570FB0"},
        {64, "7897F021"},{65, "79670725"},{66, "797399BC"},{67, "7AF6BDC3"},
        {68, "8836DA57"},{69, "89D85E3E"},{70, "ABCB4D7A"},{71, "AC9F4D4D"},
        {72, "B3838F32"},{73, "B854C540"},{74, "B9377A66"},{75, "B9FB1122"},
        {76, "BE8F48B4"},{77, "BF78CCE5"},{78, "C906A5DC"},{79, "C90FFC1B"},
        {80, "D23B7A36"},{81, "D2E0760F"},{82, "D5FD6BBE"},{83, "D68A7D40"},
        {84, "DE525064"},{85, "E5F5BD5D"},{86, "EB73FF3C"},{87, "FC2857A0"},
        {88, "2DDADD86"},{89, "3D0D23D6"},{90, "59990D9D"},{91, "92CD5DE2"},
        {92, "930BE499"},{93, "9D61B125"},{94, "B8698C39"},{95, "BFC38B81"},
        {96, "CE752028"},{97, "F96CF480"},{98, "0B852D80"},{99, "1726FCE7"},
        {100, "2C5692D4"},{101, "3010071F"},{102, "33F9185A"},{103, "657E323F"},
        {104, "6BF0FFC2"},{105, "90FC7CE9"},{106, "970B6BCD"},{107, "C8F7A0B2"},
        {108, "0AF18823"},{109, "1B47B31C"},{110, "61DEA90E"},{111, "6D6866A4"},
        {112, "6E485C08"},{113, "70326B38"},{114, "C4C0CF98"},{115, "CA4227C0"},
        {116, "E6354E4A"},{117, "05744EF8"},{118, "1609CE31"},{119, "18390EDC"},
        {120, "1A73DC22"},{121, "2BA88451"},{122, "2BD39B91"},{123, "3848FE9F"},
        {124, "3A3CE3AC"},{125, "3E6FFAE0"},{126, "51068E01"},{127, "52005157"},
        {128, "60C273AD"},{129, "6A423FA6"},{130, "7E3D64B8"},{131, "84D7FE19"},
        {132, "899793F6"},{133, "8E973855"},{134, "9FB7D45E"},{135, "BDFFC57E"},
        {136, "D7136D80"},{137, "D9B20367"},{138, "DC9B24D2"},{139, "E49A32BF"},
        {140, "F470AEE0"},{141, "F4E3142D"},{142, "F7097184"},{143, "FB6FF5CE"},
        {144, "FFDA238A"},{145, "001BB018"},{146, "01E15AA8"},{147, "04438392"},
        {148, "087A1CEC"},{149, "08F2EB95"},{150, "0912AF52"},{151, "09383578"},
        {152, "0E7F8052"},{153, "0EB720C2"},{154, "0EE01F85"},{155, "10BD3209"},
        {156, "11880A64"},{157, "19B90650"},{158, "1AAF5EC4"},{159, "1ACDB7AF"},
        {160, "1F44BA32"},{161, "24E85958"},{162, "257D2298"},{163, "276295D5"},
        {164, "28370CD5"},{165, "2D59D944"},{166, "30FE9977"},{167, "33CEC416"},
        {168, "370990B8"},{169, "3970087E"},{170, "397BE6A3"},{171, "3AF10DB1"},
        {172, "3B86F281"},{173, "3BE9912A"},{174, "3CD9E7B8"},{175, "42C0065E"},
        {176, "45D36C16"},{177, "49DF913E"},{178, "4F3F254C"},{179, "5217AC16"},
        {180, "55447C72"},{181, "5716FC69"},{182, "574FF05E"},{183, "57DE9D28"},
        {184, "58334DD3"},{185, "585BBF4B"},{186, "59EE4EFA"},{187, "5A386821"},
        {188, "5B679415"},{189, "5B91DCA8"},{190, "5CF38100"},{191, "5CF685D5"},
        {192, "5D23FC59"},{193, "5E25E56B"},{194, "5EB29CBA"},{195, "61CECCE1"},
        {196, "66B58604"},{197, "693B97CB"},{198, "6D128A92"},{199, "6D57D8F2"},
        {200, "6F799C0D"},{201, "7237CA8B"},{202, "723C22D5"},{203, "7536FBDD"},
        {204, "7A7DEED9"},{205, "7F5D1674"},{206, "7F8BAF42"},{207, "824A571A"},
        {208, "840F0A6D"},{209, "867795E8"},{210, "8826A7FE"},{211, "8BADC162"},
        {212, "8F8D9142"},{213, "92918F75"},{214, "93A4C29F"},{215, "93F86F21"},
        {216, "96967A37"},{217, "97D1C6FF"},{218, "9CBA49D9"},{219, "9CBE564E"},
        {220, "9E460FB3"},{221, "A006BD66"},{222, "A3BE9693"},{223, "A5A4A0C0"},
        {224, "A5B1BC4E"},{225, "A6D3BA4C"},{226, "A865F0FA"},{227, "A8B84AF3"},
        {228, "AA2697D7"},{229, "AA417C9C"},{230, "AAB6365E"},{231, "B148E3C5"},
        {232, "B1B4B236"},{233, "B9CC3125"},{234, "C007A46E"},{235, "C161F5A6"},
        {236, "C1918EE0"},{237, "C1DB708B"},{238, "C478720B"},{239, "C563FFB6"},
        {240, "C9BC800C"},{241, "CD6BBC3C"},{242, "CE177060"},{243, "CE6F1BC4"},
        {244, "CF93DAE9"},{245, "D001C8DB"},{246, "D1CB0A30"},{247, "D2027F90"},
        {248, "D509ACE5"},{249, "D8417D68"},{250, "DEA786B2"},{251, "DF9C84F9"},
        {252, "E009D524"},{253, "E2124C2D"},{254, "E4057D64"},{255, "E445DA7C"},
        {256, "E5E93053"},{257, "E76A577C"},{258, "E8EE2B2E"},{259, "EFF2048F"},
        {260, "FA63C345"},{261, "FE881D31"},{262, "0DD91D53"},{263, "1488DE46"},
        {264, "225230DC"},{265, "35824954"},{266, "9788E7A2"},{267, "B948C790"},
        {268, "BF127CBB"},{269, "CCFD619D"},{270, "D9877E9C"},{271, "E58E6CB9"},
        {272, "E96E3942"},{273, "0E2166D9"},{274, "0E9DD89E"},{275, "1CD40F9B"},
        {276, "25CCF48B"},{277, "34F168D3"},{278, "5524B3CD"},{279, "5EADA49E"},
        {280, "C5BF6EE1"},{281, "C7EA926C"},{282, "E3F9F2B4"},{283, "FE70D25B"},
        {284, "1195872C"},{285, "14040279"},{286, "1B938503"},{287, "32F3B3E0"},
        {288, "347906F5"},{289, "4EC1DC2C"},{290, "5ED760E0"},{291, "647251C4"},
        {292, "6F333ACC"},{293, "76884FF7"},{294, "9A480D5C"},{295, "9BE7ACB1"},
        {296, "AC0527EE"},{297, "B9B266EF"},{298, "C8183CF6"},{299, "DF126337"},
        {300, "DF69F7FF"},{301, "E9BD127B"},{302, "0269C34A"},{303, "03F12141"},
        {304, "115CC035"},{305, "182AC62E"},{306, "1A3A997B"},{307, "261101B8"},
        {308, "26E67171"},{309, "2958A947"},{310, "30CC996F"},{311, "31BB29A4"},
        {312, "42BFAB1C"},{313, "4620DF6F"},{314, "4662E1A3"},{315, "49020CF8"},
        {316, "49996E73"},{317, "4BE5A2E9"},{318, "4D70160D"},{319, "4E70AB99"},
        {320, "505E36E0"},{321, "52128E1A"},{322, "585B960D"},{323, "59F836F4"},
        {324, "5BBB2DBC"},{325, "681F3651"},{326, "68A07220"},{327, "6BDEE017"},
        {328, "70766C3E"},{329, "71E01290"},{330, "71F6BEDF"},{331, "733B38AC"},
        {332, "770E4164"},{333, "784B2F9D"},{334, "7BEFC9B4"},{335, "7C475717"},
        {336, "7CC6874C"},{337, "7F3F851A"},{338, "80E68B06"},{339, "81681C60"},
        {340, "823B0F77"},{341, "8456E209"},{342, "85CDEC11"},{343, "8D198BA0"},
        {344, "8E010BAF"},{345, "8F7709B6"},{346, "92BDBBBD"},{347, "95D994DC"},
        {348, "96ED07C7"},{349, "9891B59C"},{350, "98FB6C1A"},{351, "9A913485"},
        {352, "9AB8E1C9"},{353, "9B712001"},{354, "9D55725F"},{355, "9FF9FDC8"},
        {356, "A1765DDC"},{357, "A5332F1E"},{358, "AEAD3614"},{359, "AF492C2B"},
        {360, "C1A49A26"},{361, "C57B754C"},{362, "C946DAE5"},{363, "D55345CF"},
        {364, "D5699570"},{365, "D5E23F39"},{366, "DE2BFE7F"},{367, "E728A4C2"},
        {368, "EA3FDE4C"},{369, "EA51A86C"},{370, "EABFFCCA"},{371, "EC19D202"},
        {372, "ECC73EE0"},{373, "ECDA3A24"},{374, "EE5997D4"},{375, "F18E1AC9"},
        {376, "F4617F01"},{377, "F6D9F695"},{378, "F7DC773A"},{379, "F9D360CA"},
        {380, "FF45F5D3"}
    };


    QMap<int,QString> Icon_Map =
    {
        {0, "C:/Customize/Item Icons/00985CE6.jpg"},{1, "C:/Customize/Item Icons/01751914.jpg"},{2, "C:/Customize/Item Icons/0851275A.jpg"},
        {3, "C:/Customize/Item Icons/0CCCB2CE.jpg"},{4, "C:/Customize/Item Icons/0D263670.jpg"},{5, "C:/Customize/Item Icons/14B82D23.jpg"},
        {6, "C:/Customize/Item Icons/1B0A7880.jpg"},{7, "C:/Customize/Item Icons/1D1B6649.jpg"},{8, "C:/Customize/Item Icons/24D2D9F2.jpg"},
        {9, "C:/Customize/Item Icons/2E76E1F1.jpg"},{10, "C:/Customize/Item Icons/31E03EAE.jpg"},{11, "C:/Customize/Item Icons/38FC955A.jpg"},
        {12, "C:/Customize/Item Icons/474EBD20.jpg"},{13, "C:/Customize/Item Icons/49039304.jpg"},{14, "C:/Customize/Item Icons/49DFCC13.jpg"},
        {15, "C:/Customize/Item Icons/4D3DE7BE.jpg"},{16, "C:/Customize/Item Icons/5408D940.jpg"},{17, "C:/Customize/Item Icons/58F3230F.jpg"},
        {18, "C:/Customize/Item Icons/5A91D12F.jpg"},{19, "C:/Customize/Item Icons/5CAE2AA1.jpg"},{20, "C:/Customize/Item Icons/621089E8.jpg"},
        {21, "C:/Customize/Item Icons/65B46C8A.jpg"},{22, "C:/Customize/Item Icons/664ED091.jpg"},{23, "C:/Customize/Item Icons/698287E6.jpg"},
        {24, "C:/Customize/Item Icons/6AB486EB.jpg"},{25, "C:/Customize/Item Icons/74E6AE19.jpg"},{26, "C:/Customize/Item Icons/77B9E2BA.jpg"},
        {27, "C:/Customize/Item Icons/783C3F34.jpg"},{28, "C:/Customize/Item Icons/7DC9641E.jpg"},{29, "C:/Customize/Item Icons/7ED53967.jpg"},
        {30, "C:/Customize/Item Icons/81338733.jpg"},{31, "C:/Customize/Item Icons/8E79919D.jpg"},{32, "C:/Customize/Item Icons/93168BCB.jpg"},
        {33, "C:/Customize/Item Icons/947409D6.jpg"},{34, "C:/Customize/Item Icons/9DBD4E62.jpg"},{35, "C:/Customize/Item Icons/A0EF67EF.jpg"},
        {36, "C:/Customize/Item Icons/A959739B.jpg"},{37, "C:/Customize/Item Icons/A985ECC6.jpg"},{38, "C:/Customize/Item Icons/AF5B2BF3.jpg"},
        {39, "C:/Customize/Item Icons/B4ABFFD3.jpg"},{40, "C:/Customize/Item Icons/BA5F28B5.jpg"},{41, "C:/Customize/Item Icons/BABF5226.jpg"},
        {42, "C:/Customize/Item Icons/BB96F032.jpg"},{43, "C:/Customize/Item Icons/C5751A04.jpg"},{44, "C:/Customize/Item Icons/D530D473.jpg"},
        {45, "C:/Customize/Item Icons/DD2438AA.jpg"},{46, "C:/Customize/Item Icons/EC5F4153.jpg"},{47, "C:/Customize/Item Icons/ECB87813.jpg"},
        {48, "C:/Customize/Item Icons/F4DBE746.jpg"},{49, "C:/Customize/Item Icons/FC756DFC.jpg"},{50, "C:/Customize/Item Icons/FDDF52CE.jpg"},
        {51, "C:/Customize/Item Icons/009093DB.jpg"},{52, "C:/Customize/Item Icons/0649176D.jpg"},{53, "C:/Customize/Item Icons/12801F99.jpg"},
        {54, "C:/Customize/Item Icons/149A12C6.jpg"},{55, "C:/Customize/Item Icons/1E05401A.jpg"},{56, "C:/Customize/Item Icons/2396F398.jpg"},
        {57, "C:/Customize/Item Icons/29487E85.jpg"},{58, "C:/Customize/Item Icons/35342ACA.jpg"},{59, "C:/Customize/Item Icons/45B268AE.jpg"},
        {60, "C:/Customize/Item Icons/52FABFB3.jpg"},{61, "C:/Customize/Item Icons/5B61D881.jpg"},{62, "C:/Customize/Item Icons/6CB61696.jpg"},
        {63, "C:/Customize/Item Icons/6F570FB0.jpg"},{64, "C:/Customize/Item Icons/7897F021.jpg"},{65, "C:/Customize/Item Icons/79670725.jpg"},
        {66, "C:/Customize/Item Icons/797399BC.jpg"},{67, "C:/Customize/Item Icons/7AF6BDC3.jpg"},{68, "C:/Customize/Item Icons/8836DA57.jpg"},
        {69, "C:/Customize/Item Icons/89D85E3E.jpg"},{70, "C:/Customize/Item Icons/ABCB4D7A.jpg"},{71, "C:/Customize/Item Icons/AC9F4D4D.jpg"},
        {72, "C:/Customize/Item Icons/B3838F32.jpg"},{73, "C:/Customize/Item Icons/B854C540.jpg"},{74, "C:/Customize/Item Icons/B9377A66.jpg"},
        {75, "C:/Customize/Item Icons/B9FB1122.jpg"},{76, "C:/Customize/Item Icons/BE8F48B4.jpg"},{77, "C:/Customize/Item Icons/BF78CCE5.jpg"},
        {78, "C:/Customize/Item Icons/C906A5DC.jpg"},{79, "C:/Customize/Item Icons/C90FFC1B.jpg"},{80, "C:/Customize/Item Icons/D23B7A36.jpg"},
        {81, "C:/Customize/Item Icons/D2E0760F.jpg"},{82, "C:/Customize/Item Icons/D5FD6BBE.jpg"},{83, "C:/Customize/Item Icons/D68A7D40.jpg"},
        {84, "C:/Customize/Item Icons/DE525064.jpg"},{85, "C:/Customize/Item Icons/E5F5BD5D.jpg"},{86, "C:/Customize/Item Icons/EB73FF3C.jpg"},
        {87, "C:/Customize/Item Icons/FC2857A0.jpg"},{88, "C:/Customize/Item Icons/2DDADD86.jpg"},{89, "C:/Customize/Item Icons/3D0D23D6.jpg"},
        {90, "C:/Customize/Item Icons/59990D9D.jpg"},{91, "C:/Customize/Item Icons/92CD5DE2.jpg"},{92, "C:/Customize/Item Icons/930BE499.jpg"},
        {93, "C:/Customize/Item Icons/9D61B125.jpg"},{94, "C:/Customize/Item Icons/B8698C39.jpg"},{95, "C:/Customize/Item Icons/BFC38B81.jpg"},
        {96, "C:/Customize/Item Icons/CE752028.jpg"},{97, "C:/Customize/Item Icons/F96CF480.jpg"},{98, "C:/Customize/Item Icons/0B852D80.jpg"},
        {99, "C:/Customize/Item Icons/1726FCE7.jpg"},{100, "C:/Customize/Item Icons/2C5692D4.jpg"},{101, "C:/Customize/Item Icons/3010071F.jpg"},
        {102, "C:/Customize/Item Icons/33F9185A.jpg"},{103, "C:/Customize/Item Icons/657E323F.jpg"},{104, "C:/Customize/Item Icons/6BF0FFC2.jpg"},
        {105, "C:/Customize/Item Icons/90FC7CE9.jpg"},{106, "C:/Customize/Item Icons/970B6BCD.jpg"},{107, "C:/Customize/Item Icons/C8F7A0B2.jpg"},
        {108, "C:/Customize/Item Icons/0AF18823.jpg"},{109, "C:/Customize/Item Icons/1B47B31C.jpg"},{110, "C:/Customize/Item Icons/61DEA90E.jpg"},
        {111, "C:/Customize/Item Icons/6D6866A4.jpg"},{112, "C:/Customize/Item Icons/6E485C08.jpg"},{113, "C:/Customize/Item Icons/70326B38.jpg"},
        {114, "C:/Customize/Item Icons/C4C0CF98.jpg"},{115, "C:/Customize/Item Icons/CA4227C0.jpg"},{116, "C:/Customize/Item Icons/E6354E4A.jpg"},
        {117, "C:/Customize/Item Icons/05744EF8.jpg"},{118, "C:/Customize/Item Icons/1609CE31.jpg"},{119, "C:/Customize/Item Icons/18390EDC.jpg"},
        {120, "C:/Customize/Item Icons/1A73DC22.jpg"},{121, "C:/Customize/Item Icons/2BA88451.jpg"},{122, "C:/Customize/Item Icons/2BD39B91.jpg"},
        {123, "C:/Customize/Item Icons/3848FE9F.jpg"},{124, "C:/Customize/Item Icons/3A3CE3AC.jpg"},{125, "C:/Customize/Item Icons/3E6FFAE0.jpg"},
        {126, "C:/Customize/Item Icons/51068E01.jpg"},{127, "C:/Customize/Item Icons/52005157.jpg"},{128, "C:/Customize/Item Icons/60C273AD.jpg"},
        {129, "C:/Customize/Item Icons/6A423FA6.jpg"},{130, "C:/Customize/Item Icons/7E3D64B8.jpg"},{131, "C:/Customize/Item Icons/84D7FE19.jpg"},
        {132, "C:/Customize/Item Icons/899793F6.jpg"},{133, "C:/Customize/Item Icons/8E973855.jpg"},{134, "C:/Customize/Item Icons/9FB7D45E.jpg"},
        {135, "C:/Customize/Item Icons/BDFFC57E.jpg"},{136, "C:/Customize/Item Icons/D7136D80.jpg"},{137, "C:/Customize/Item Icons/D9B20367.jpg"},
        {138, "C:/Customize/Item Icons/DC9B24D2.jpg"},{139, "C:/Customize/Item Icons/E49A32BF.jpg"},{140, "C:/Customize/Item Icons/F470AEE0.jpg"},
        {141, "C:/Customize/Item Icons/F4E3142D.jpg"},{142, "C:/Customize/Item Icons/F7097184.jpg"},{143, "C:/Customize/Item Icons/FB6FF5CE.jpg"},
        {144, "C:/Customize/Item Icons/FFDA238A.jpg"},{145, "C:/Customize/Item Icons/001BB018.jpg"},{146, "C:/Customize/Item Icons/01E15AA8.jpg"},
        {147, "C:/Customize/Item Icons/04438392.jpg"},{148, "C:/Customize/Item Icons/087A1CEC.jpg"},{149, "C:/Customize/Item Icons/08F2EB95.jpg"},
        {150, "C:/Customize/Item Icons/0912AF52.jpg"},{151, "C:/Customize/Item Icons/09383578.jpg"},{152, "C:/Customize/Item Icons/0E7F8052.jpg"},
        {153, "C:/Customize/Item Icons/0EB720C2.jpg"},{154, "C:/Customize/Item Icons/0EE01F85.jpg"},{155, "C:/Customize/Item Icons/10BD3209.jpg"},
        {156, "C:/Customize/Item Icons/11880A64.jpg"},{157, "C:/Customize/Item Icons/19B90650.jpg"},{158, "C:/Customize/Item Icons/1AAF5EC4.jpg"},
        {159, "C:/Customize/Item Icons/1ACDB7AF.jpg"},{160, "C:/Customize/Item Icons/1F44BA32.jpg"},{161, "C:/Customize/Item Icons/24E85958.jpg"},
        {162, "C:/Customize/Item Icons/257D2298.jpg"},{163, "C:/Customize/Item Icons/276295D5.jpg"},{164, "C:/Customize/Item Icons/28370CD5.jpg"},
        {165, "C:/Customize/Item Icons/2D59D944.jpg"},{166, "C:/Customize/Item Icons/30FE9977.jpg"},{167, "C:/Customize/Item Icons/33CEC416.jpg"},
        {168, "C:/Customize/Item Icons/370990B8.jpg"},{169, "C:/Customize/Item Icons/3970087E.jpg"},{170, "C:/Customize/Item Icons/397BE6A3.jpg"},
        {171, "C:/Customize/Item Icons/3AF10DB1.jpg"},{172, "C:/Customize/Item Icons/3B86F281.jpg"},{173, "C:/Customize/Item Icons/3BE9912A.jpg"},
        {174, "C:/Customize/Item Icons/3CD9E7B8.jpg"},{175, "C:/Customize/Item Icons/42C0065E.jpg"},{176, "C:/Customize/Item Icons/45D36C16.jpg"},
        {177, "C:/Customize/Item Icons/49DF913E.jpg"},{178, "C:/Customize/Item Icons/4F3F254C.jpg"},{179, "C:/Customize/Item Icons/5217AC16.jpg"},
        {180, "C:/Customize/Item Icons/55447C72.jpg"},{181, "C:/Customize/Item Icons/5716FC69.jpg"},{182, "C:/Customize/Item Icons/574FF05E.jpg"},
        {183, "C:/Customize/Item Icons/57DE9D28.jpg"},{184, "C:/Customize/Item Icons/58334DD3.jpg"},{185, "C:/Customize/Item Icons/585BBF4B.jpg"},
        {186, "C:/Customize/Item Icons/59EE4EFA.jpg"},{187, "C:/Customize/Item Icons/5A386821.jpg"},{188, "C:/Customize/Item Icons/5B679415.jpg"},
        {189, "C:/Customize/Item Icons/5B91DCA8.jpg"},{190, "C:/Customize/Item Icons/5CF38100.jpg"},{191, "C:/Customize/Item Icons/5CF685D5.jpg"},
        {192, "C:/Customize/Item Icons/5D23FC59.jpg"},{193, "C:/Customize/Item Icons/5E25E56B.jpg"},{194, "C:/Customize/Item Icons/5EB29CBA.jpg"},
        {195, "C:/Customize/Item Icons/61CECCE1.jpg"},{196, "C:/Customize/Item Icons/66B58604.jpg"},{197, "C:/Customize/Item Icons/693B97CB.jpg"},
        {198, "C:/Customize/Item Icons/6D128A92.jpg"},{199, "C:/Customize/Item Icons/6D57D8F2.jpg"},{200, "C:/Customize/Item Icons/6F799C0D.jpg"},
        {201, "C:/Customize/Item Icons/7237CA8B.jpg"},{202, "C:/Customize/Item Icons/723C22D5.jpg"},{203, "C:/Customize/Item Icons/7536FBDD.jpg"},
        {204, "C:/Customize/Item Icons/7A7DEED9.jpg"},{205, "C:/Customize/Item Icons/7F5D1674.jpg"},{206, "C:/Customize/Item Icons/7F8BAF42.jpg"},
        {207, "C:/Customize/Item Icons/824A571A.jpg"},{208, "C:/Customize/Item Icons/840F0A6D.jpg"},{209, "C:/Customize/Item Icons/867795E8.jpg"},
        {210, "C:/Customize/Item Icons/8826A7FE.jpg"},{211, "C:/Customize/Item Icons/8BADC162.jpg"},{212, "C:/Customize/Item Icons/8F8D9142.jpg"},
        {213, "C:/Customize/Item Icons/92918F75.jpg"},{214, "C:/Customize/Item Icons/93A4C29F.jpg"},{215, "C:/Customize/Item Icons/93F86F21.jpg"},
        {216, "C:/Customize/Item Icons/96967A37.jpg"},{217, "C:/Customize/Item Icons/97D1C6FF.jpg"},{218, "C:/Customize/Item Icons/9CBA49D9.jpg"},
        {219, "C:/Customize/Item Icons/9CBE564E.jpg"},{220, "C:/Customize/Item Icons/9E460FB3.jpg"},{221, "C:/Customize/Item Icons/A006BD66.jpg"},
        {222, "C:/Customize/Item Icons/A3BE9693.jpg"},{223, "C:/Customize/Item Icons/A5A4A0C0.jpg"},{224, "C:/Customize/Item Icons/A5B1BC4E.jpg"},
        {225, "C:/Customize/Item Icons/A6D3BA4C.jpg"},{226, "C:/Customize/Item Icons/A865F0FA.jpg"},{227, "C:/Customize/Item Icons/A8B84AF3.jpg"},
        {228, "C:/Customize/Item Icons/AA2697D7.jpg"},{229, "C:/Customize/Item Icons/AA417C9C.jpg"},{230, "C:/Customize/Item Icons/AAB6365E.jpg"},
        {231, "C:/Customize/Item Icons/B148E3C5.jpg"},{232, "C:/Customize/Item Icons/B1B4B236.jpg"},{233, "C:/Customize/Item Icons/B9CC3125.jpg"},
        {234, "C:/Customize/Item Icons/C007A46E.jpg"},{235, "C:/Customize/Item Icons/C161F5A6.jpg"},{236, "C:/Customize/Item Icons/C1918EE0.jpg"},
        {237, "C:/Customize/Item Icons/C1DB708B.jpg"},{238, "C:/Customize/Item Icons/C478720B.jpg"},{239, "C:/Customize/Item Icons/C563FFB6.jpg"},
        {240, "C:/Customize/Item Icons/C9BC800C.jpg"},{241, "C:/Customize/Item Icons/CD6BBC3C.jpg"},{242, "C:/Customize/Item Icons/CE177060.jpg"},
        {243, "C:/Customize/Item Icons/CE6F1BC4.jpg"},{244, "C:/Customize/Item Icons/CF93DAE9.jpg"},{245, "C:/Customize/Item Icons/D001C8DB.jpg"},
        {246, "C:/Customize/Item Icons/D1CB0A30.jpg"},{247, "C:/Customize/Item Icons/D2027F90.jpg"},{248, "C:/Customize/Item Icons/D509ACE5.jpg"},
        {249, "C:/Customize/Item Icons/D8417D68.jpg"},{250, "C:/Customize/Item Icons/DEA786B2.jpg"},{251, "C:/Customize/Item Icons/DF9C84F9.jpg"},
        {252, "C:/Customize/Item Icons/E009D524.jpg"},{253, "C:/Customize/Item Icons/E2124C2D.jpg"},{254, "C:/Customize/Item Icons/E4057D64.jpg"},
        {255, "C:/Customize/Item Icons/E445DA7C.jpg"},{256, "C:/Customize/Item Icons/E5E93053.jpg"},{257, "C:/Customize/Item Icons/E76A577C.jpg"},
        {258, "C:/Customize/Item Icons/E8EE2B2E.jpg"},{259, "C:/Customize/Item Icons/EFF2048F.jpg"},{260, "C:/Customize/Item Icons/FA63C345.jpg"},
        {261, "C:/Customize/Item Icons/FE881D31.jpg"},{262, "C:/Customize/Item Icons/0DD91D53.jpg"},{263, "C:/Customize/Item Icons/1488DE46.jpg"},
        {264, "C:/Customize/Item Icons/225230DC.jpg"},{265, "C:/Customize/Item Icons/35824954.jpg"},{266, "C:/Customize/Item Icons/9788E7A2.jpg"},
        {267, "C:/Customize/Item Icons/B948C790.jpg"},{268, "C:/Customize/Item Icons/BF127CBB.jpg"},{269, "C:/Customize/Item Icons/CCFD619D.jpg"},
        {270, "C:/Customize/Item Icons/D9877E9C.jpg"},{271, "C:/Customize/Item Icons/E58E6CB9.jpg"},{272, "C:/Customize/Item Icons/E96E3942.jpg"},
        {273, "C:/Customize/Item Icons/0E2166D9.jpg"},{274, "C:/Customize/Item Icons/0E9DD89E.jpg"},{275, "C:/Customize/Item Icons/1CD40F9B.jpg"},
        {276, "C:/Customize/Item Icons/25CCF48B.jpg"},{277, "C:/Customize/Item Icons/34F168D3.jpg"},{278, "C:/Customize/Item Icons/5524B3CD.jpg"},
        {279, "C:/Customize/Item Icons/5EADA49E.jpg"},{280, "C:/Customize/Item Icons/C5BF6EE1.jpg"},{281, "C:/Customize/Item Icons/C7EA926C.jpg"},
        {282, "C:/Customize/Item Icons/E3F9F2B4.jpg"},{283, "C:/Customize/Item Icons/FE70D25B.jpg"},{284, "C:/Customize/Item Icons/1195872C.jpg"},
        {285, "C:/Customize/Item Icons/14040279.jpg"},{286, "C:/Customize/Item Icons/1B938503.jpg"},{287, "C:/Customize/Item Icons/32F3B3E0.jpg"},
        {288, "C:/Customize/Item Icons/347906F5.jpg"},{289, "C:/Customize/Item Icons/4EC1DC2C.jpg"},{290, "C:/Customize/Item Icons/5ED760E0.jpg"},
        {291, "C:/Customize/Item Icons/647251C4.jpg"},{292, "C:/Customize/Item Icons/6F333ACC.jpg"},{293, "C:/Customize/Item Icons/76884FF7.jpg"},
        {294, "C:/Customize/Item Icons/9A480D5C.jpg"},{295, "C:/Customize/Item Icons/9BE7ACB1.jpg"},{296, "C:/Customize/Item Icons/AC0527EE.jpg"},
        {297, "C:/Customize/Item Icons/B9B266EF.jpg"},{298, "C:/Customize/Item Icons/C8183CF6.jpg"},{299, "C:/Customize/Item Icons/DF126337.jpg"},
        {300, "C:/Customize/Item Icons/DF69F7FF.jpg"},{301, "C:/Customize/Item Icons/E9BD127B.jpg"},{302, "C:/Customize/Item Icons/0269C34A.jpg"},
        {303, "C:/Customize/Item Icons/03F12141.jpg"},{304, "C:/Customize/Item Icons/115CC035.jpg"},{305, "C:/Customize/Item Icons/182AC62E.jpg"},
        {306, "C:/Customize/Item Icons/1A3A997B.jpg"},{307, "C:/Customize/Item Icons/261101B8.jpg"},{308, "C:/Customize/Item Icons/26E67171.jpg"},
        {309, "C:/Customize/Item Icons/2958A947.jpg"},{310, "C:/Customize/Item Icons/30CC996F.jpg"},{311, "C:/Customize/Item Icons/31BB29A4.jpg"},
        {312, "C:/Customize/Item Icons/42BFAB1C.jpg"},{313, "C:/Customize/Item Icons/4620DF6F.jpg"},{314, "C:/Customize/Item Icons/4662E1A3.jpg"},
        {315, "C:/Customize/Item Icons/49020CF8.jpg"},{316, "C:/Customize/Item Icons/49996E73.jpg"},{317, "C:/Customize/Item Icons/4BE5A2E9.jpg"},
        {318, "C:/Customize/Item Icons/4D70160D.jpg"},{319, "C:/Customize/Item Icons/4E70AB99.jpg"},{320, "C:/Customize/Item Icons/505E36E0.jpg"},
        {321, "C:/Customize/Item Icons/52128E1A.jpg"},{322, "C:/Customize/Item Icons/585B960D.jpg"},{323, "C:/Customize/Item Icons/59F836F4.jpg"},
        {324, "C:/Customize/Item Icons/5BBB2DBC.jpg"},{325, "C:/Customize/Item Icons/681F3651.jpg"},{326, "C:/Customize/Item Icons/68A07220.jpg"},
        {327, "C:/Customize/Item Icons/6BDEE017.jpg"},{328, "C:/Customize/Item Icons/70766C3E.jpg"},{329, "C:/Customize/Item Icons/71E01290.jpg"},
        {330, "C:/Customize/Item Icons/71F6BEDF.jpg"},{331, "C:/Customize/Item Icons/733B38AC.jpg"},{332, "C:/Customize/Item Icons/770E4164.jpg"},
        {333, "C:/Customize/Item Icons/784B2F9D.jpg"},{334, "C:/Customize/Item Icons/7BEFC9B4.jpg"},{335, "C:/Customize/Item Icons/7C475717.jpg"},
        {336, "C:/Customize/Item Icons/7CC6874C.jpg"},{337, "C:/Customize/Item Icons/7F3F851A.jpg"},{338, "C:/Customize/Item Icons/80E68B06.jpg"},
        {339, "C:/Customize/Item Icons/81681C60.jpg"},{340, "C:/Customize/Item Icons/823B0F77.jpg"},{341, "C:/Customize/Item Icons/8456E209.jpg"},
        {342, "C:/Customize/Item Icons/85CDEC11.jpg"},{343, "C:/Customize/Item Icons/8D198BA0.jpg"},{344, "C:/Customize/Item Icons/8E010BAF.jpg"},
        {345, "C:/Customize/Item Icons/8F7709B6.jpg"},{346, "C:/Customize/Item Icons/92BDBBBD.jpg"},{347, "C:/Customize/Item Icons/95D994DC.jpg"},
        {348, "C:/Customize/Item Icons/96ED07C7.jpg"},{349, "C:/Customize/Item Icons/9891B59C.jpg"},{350, "C:/Customize/Item Icons/98FB6C1A.jpg"},
        {351, "C:/Customize/Item Icons/9A913485.jpg"},{352, "C:/Customize/Item Icons/9AB8E1C9.jpg"},{353, "C:/Customize/Item Icons/9B712001.jpg"},
        {354, "C:/Customize/Item Icons/9D55725F.jpg"},{355, "C:/Customize/Item Icons/9FF9FDC8.jpg"},{356, "C:/Customize/Item Icons/A1765DDC.jpg"},
        {357, "C:/Customize/Item Icons/A5332F1E.jpg"},{358, "C:/Customize/Item Icons/AEAD3614.jpg"},{359, "C:/Customize/Item Icons/AF492C2B.jpg"},
        {360, "C:/Customize/Item Icons/C1A49A26.jpg"},{361, "C:/Customize/Item Icons/C57B754C.jpg"},{362, "C:/Customize/Item Icons/C946DAE5.jpg"},
        {363, "C:/Customize/Item Icons/D55345CF.jpg"},{364, "C:/Customize/Item Icons/D5699570.jpg"},{365, "C:/Customize/Item Icons/D5E23F39.jpg"},
        {366, "C:/Customize/Item Icons/DE2BFE7F.jpg"},{367, "C:/Customize/Item Icons/E728A4C2.jpg"},{368, "C:/Customize/Item Icons/EA3FDE4C.jpg"},
        {369, "C:/Customize/Item Icons/EA51A86C.jpg"},{370, "C:/Customize/Item Icons/EABFFCCA.jpg"},{371, "C:/Customize/Item Icons/EC19D202.jpg"},
        {372, "C:/Customize/Item Icons/ECC73EE0.jpg"},{373, "C:/Customize/Item Icons/ECDA3A24.jpg"},{374, "C:/Customize/Item Icons/EE5997D4.jpg"},
        {375, "C:/Customize/Item Icons/F18E1AC9.jpg"},{376, "C:/Customize/Item Icons/F4617F01.jpg"},{377, "C:/Customize/Item Icons/F6D9F695.jpg"},
        {378, "C:/Customize/Item Icons/F7DC773A.jpg"},{379, "C:/Customize/Item Icons/F9D360CA.jpg"},{380, "C:/Customize/Item Icons/FF45F5D3.jpg"}
    };

    QMap <QString,QString> Icon_5Byte_Map =
    {

        {"BF78CCE5", "819D9CA303"},{"BE8F48B4", "83C1BBBB09"},{"B9FB1122", "F2EE8FA104"},{"B9377A66", "F2DED1E70C"},
        {"B854C540", "F0D2AA8C08"},{"B3838F32", "E68EFCA806"},{"AC9F4D4D", "D8FEECD409"},{"ABCB4D7A", "D6AEEEA40F"},
        {"89D85E3E", "92E2F6E507"},{"8836DA57", "90DAD1FD0A"},{"7AF6BDC3", "8BA690C407"},{"797399BC", "8DB2B4B608"},
        {"79670725", "F29DBBD004"},{"7897F021", "F0DD849F04"},{"6F570FB0", "A1A285FF09"},{"6CB61696", "A7A6CA9E0D"},
        {"5B61D881", "C9FABCE20F"},{"52FABFB3", "DB9680C409"},{"45B268AE", "F5B6BA990A"},{"35342ACA", "95AFAEDD06"},
        {"29487E85", "ADDF8DA80F"},{"2396F398", "B9A7E3F00C"},{"1E05401A", "BC9480A403"},{"149A12C6", "D797EB9E07"},
        {"12801F99", "DBFF83EE0C"},{"0649176D", "8CA4BAD10D"},{"009093DB", "FFBFE3C604"},{"FDDF52CE", "8580E99A06"},
        {"FC756DFC", "87A89439"},{"F4DBE746", "E8EFBEEE08"},{"ECB87813", "D8E3C5B702"},{"EC5F4153", "D8FF8AB40A"},
        {"DD2438AA", "C5ECBEDC0A"},{"D530D473", "AAC3A1BD0E"},{"C5751A04", "8AD7D341"},{"BB96F032", "F6DA84AF06"},
        {"BABF5226", "F4FE95E504"},{"BA5F28B5", "8B81BDAD09"},{"B4ABFFD3", "97D182C005"},{"AF5B2BF3", "A191A5CD01"},
        {"A985ECC6", "ADE99B9107"},{"A959739B", "AD99E5C80C"},{"A0EF67EF", "BFC1C08902"},{"9DBD4E62", "BAF6F5A40C"},
        {"947409D6", "D7ADB49F05"},{"93168BCB", "D9A5A7C706"},{"8E79919D", "E399F4A60C"},{"81338733", "82CEB9B806"},
        {"7ED53967", "FCD5CEF30C"},{"7DC9641E", "FAA5A6E603"},{"783C3F34", "F0F1F9C306"},{"77B9E2BA", "919AEAD108"},
        {"74E6AE19", "E899F79A03"},{"6AB486EB", "ABAECAC702"},{"698287E6", "ADF6C39703"},{"664ED091", "B3C6FDE20D"},
        {"65B46C8A", "B5AE9AD90E"},{"621089E8", "BBBEB7F702"},{"5CAE2AA1", "C7C6AAED0B"},{"5A91D12F", "B4C58CFD05"},
        {"58F3230F", "B0CD9FF201"},{"4D3DE7BE", "E58AC69108"},{"5408D940", "A8A1C88D08"},{"49DFCC13", "92FDE6BC02"},
        {"49039304", "928D9849"},{"474EBD20", "8EB9EA8B04"},{"38FC955A", "F0F0AFA90B"},{"31E03EAE", "9DFF889C0A"},
        {"2E76E1F1", "A3A7F4E101"},{"24D2D9F2", "B7B7B1D201"},{"0B852D80", "E9EB93FD0F"},{"F96CF480", "8DCCDCF00F"},
        {"1D1B6649", "BAECB09609"},{"1B0A7880", "C9D7BFF80F"},{"14B82D23", "A8E0EDB204"},{"0D263670", "9A98B1830E"},
        {"0CCCB2CE", "E7CFE99406"},{"0851275A", "90C4BAA20B"},{"01751914", "82D4CBC102"},{"00985CE6", "FF9F9B9A03"},
        {"CE752028", "9CD7838205"},{"BFC38B81", "81F1A1E70F"},{"B8698C39", "F0A6E39807"},{"9D61B125", "BA868BDB04"},
        {"930BE499", "D9D1DFE10C"},{"92CD5DE2", "DBC991DA03"},{"59990D9D", "CD9A93AF0C"},{"3D0D23D6", "85CBE79D05"},
        {"2DDADD86", "A59791920F"},{"FC2857A0", "87DCC6FA0B"},{"EB73FF3C", "D6CFFBCF07"},{"E5F5BD5D", "CAD7EFDB0B"},
        {"DE525064", "BCCB82C50C"},{"D68A7D40", "ACABEC8708"},{"D5FD6BBE", "D588A09908"},{"D2E0760F", "A483B7F701"},
        {"D23B7A36", "A4EFD1E706"},{"C90FFC1B", "92BFE0BF03"},{"C906A5DC", "EDE4D7B504"},{"FFDA238A", "8194E1DD0E"},
        {"FB6FF5CE", "89C0D49006"},{"F7097184", "91D8F7B80F"},{"F4E3142D", "E88FA7D105"},{"F470AEE0", "97BC8CF503"},
        {"E49A32BF", "B794EB8C08"},{"DC9B24D2", "C790DBDD05"},{"D9B20367", "B2CB9DF00C"},{"D7136D80", "D1B097F90F"},
        {"BDFFC57E", "FAFEAFEC0F"},{"9FB7D45E", "BEDEA5ED0B"},{"8E973855", "9CDEC4D30A"},{"899793F6", "EDA1E39601"},
        {"84D7FE19", "88DEF69F03"},{"7E3D64B8", "838ADEF908"},{"6A423FA6", "ABF6859C0B"},{"60C273AD", "BFF6E1A80A"},
        {"52005157", "A48188F50A"},{"51068E01", "A299F018"},{"3E6FFAE0", "83C3ACF003"},{"3A3CE3AC", "8B8FE6B10A"},
        {"3848FE9F", "8FDF8D800C"},{"2BD39B91", "A9B3A1E60D"},{"2BA88451", "D6A0A5980A"},{"1A73DC22", "B4CCE3AD04"},
        {"18390EDC", "CF9B8EBF04"},{"1609CE31", "ACA4F09C06"},{"05744EF8", "F5AF8C7B"},{"E6354E4A", "CCD7F1A409"},
        {"CA4227C0", "EBF4C5FD07"},{"C4C0CF98", "F7FC81F30C"},{"70326B38", "E0C9D98607"},{"6E485C08", "DCA1E28501"},
        {"6D6866A4", "A5DECCB90B"},{"61DEA90E", "C2F9CEEA01"},{"1B47B31C", "B69C9ACB03"},{"0AF18823", "94C4C7B804"},
        {"C8F7A0B2", "EFA0F8D509"},{"970B6BCD", "D1D1A7A906"},{"90FC7CE9", "DF8D98E802"},{"6BF0FFC2", "A9BE80D007"},
        {"657E323F", "CAF993F307"},{"33F9185A", "E6E4C7A10B"},{"3010071F", "E0C0B8F003"},{"2C5692D4", "A7A7EDB605"},
        {"1726FCE7", "D1E79E8003"},{"1F44BA32", "BE90D2AB06"},{"1ACDB7AF", "CBCBC1840A"},{"1AAF5EC4", "CBC38ABA07"},
        {"19B90650", "B2E4B5800A"},{"11880A64", "A2A0D4C00C"},{"10BD3209", "A0F4959301"},{"0EE01F85", "E3FF80AE0F"},
        {"0EB720C2", "E3A3FADD07"},{"0E7F8052", "9CFC83A80A"},{"09383578", "92E0A9830F"},{"0912AF52", "92C8F8AA0A"},
        {"08F2EB95", "EFB7A0A10D"},{"087A1CEC", "EF979CBE02"},{"04438392", "F7F3E5D70D"},{"01E15AA8", "FDFBA8FA0A"},
        {"001BB018", "80EC808B03"},{"FF45F5D3", "81E8D5C005"},{"F9D360CA", "8DB0F9D906"},{"F7DC773A", "EEF3BEA707"},
        {"F6D9F695", "9398C9A00D"},{"F4617F01", "E887FB17"},{"F18E1AC9", "9DC4ABEE06"},{"EE5997D4", "A398C5B605"},
        {"ECDA3A24", "D8EBD6C304"},{"ECC73EE0", "A7E089FC03"},{"EC19D202", "D8E7902D"},{"EABFFCCA", "AB809AD006"},
        {"EA51A86C", "D4C7C2CA0D"},{"EA3FDE4C", "D4FFF1CD09"},{"E728A4C2", "B1DCDED507"},{"DE2BFE7F", "BCAFF1FF0F"},
        {"D5E23F39", "AA8BFF9307"},{"D5699570", "AAA7AB890E"},{"D55345CF", "D5B0D58B06"},{"C946DAE5", "EDE4ADA203"},
        {"C57B754C", "8AEFABC709"},{"C1A49A26", "8293D5E904"},{"AF492C2B", "DEA6E2B205"},{"AEAD3614", "DCB6B5C302"},
        {"A5332F1E", "CACEF9E203"},{"A1765DDC", "BDA594BA04"},{"9FF9FDC8", "C19990F006"},{"9D55725F", "BAD692F70B"},
        {"9B712001", "B6C68312"},{"9AB8E1C9", "CB9DF2E106"},{"9A913485", "CBB9DBAC0F"},{"98FB6C1A", "B0EEE7A603"},
        {"9891B59C", "CFB9D3B40C"},{"96ED07C7", "D3C9C08F07"},{"95D994DC", "D599D9B604"},{"92BDBBBD", "DB89A2A408"},
        {"8F7709B6", "E1A1B49F09"},{"8E010BAF", "E3F9A78F0A"},{"8D198BA0", "E599A7F70B"},{"85CDEC11", "8AB6E69E02"},
        {"8456E209", "88DA929E01"},{"823B0F77", "84EEF9F00E"},{"81681C60", "82A2E3810C"},{"80E68B06", "809ADF68"},
        {"7F3F851A", "FEFDA9A803"},{"7CC6874C", "F899BEC809"},{"7C475717", "F89DBAF502"},{"7BEFC9B4", "89C2B0B309"},
        {"784B2F9D", "8FD285AD0C"},{"770E4164", "EEB988C40C"},{"733B38AC", "9992BEBC0A"},{"71F6BEDF", "9DA6888404"},
        {"71E01290", "9DFEE8FE0D"},{"70766C3E", "E0D9E3E607"},{"6BDEE017", "D6F986FE02"},{"68A07220", "D081958704"},
        {"681F3651", "D0FDB0930A"},{"5BBB2DBC", "C99292BD08"},{"59F836F4", "CD9EC8BC01"},{"585B960D", "B0EDB2D901"},
        {"52128E1A", "A4C9F0A803"},{"505E36E0", "DF86CDFC03"},{"4E70AB99", "E3BEA4E50C"},{"4D70160D", "9AC1B3D101"},
        {"4BE5A2E9", "E9EAE8E502"},{"49996E73", "92E5F4B60E"},{"49020CF8", "EDF69F7F"},{"4662E1A3", "F3F6F4C10B"},
        {"4620DF6F", "8C81F9FD0D"},{"42BFAB1C", "84FDDDCA03"},{"31BB29A4", "9D93B2BD0B"},{"30CC996F", "E0B0CEF90D"},
        {"2958A947", "D2E0CAFA08"},{"26E67171", "CC988F970E"},{"261101B8", "B3BBF7FF08"},{"1A3A997B", "B4E8C9B90F"},
        {"182AC62E", "B0A8B1EC05"},{"115CC035", "A2F082DC06"},{"03F12141", "86C48F9208"},{"0269C34A", "84A49BAC09"},
        {"E9BD127B", "D2F795B10F"},{"DF69F7FF", "C1D844"},{"DF126337", "BECB98F606"},{"C8183CF6", "EF9C9F9C01"},
        {"B9B266EF", "8DB5CA8902"},{"AC0527EE", "A7E9C79D02"},{"9BE7ACB1", "C9E198E509"},{"9A480D5C", "B4A2EAC00B"},
        {"76884FF7", "93DE838B01"},{"6F333ACC", "A1B2AEBC06"},{"647251C4", "B7B6F4BA07"},{"5ED760E0", "C3A2F9F903"},
        {"4EC1DC2C", "9C85E6CD05"},{"347906F5", "979BCCAF01"},{"32F3B3E0", "9BB3E0F403"},{"1B938503", "B6CCAC38"},
        {"14040279", "A89090900F"},{"1195872C", "A2D4BCC805"},{"FE70D25B", "FCC393BD0B"},{"E3F9F2B4", "B998E8B009"},
        {"C7EA926C", "8EAB97C90D"},{"C5BF6EE1", "F5808AE903"},{"5EADA49E", "C3CADA950C"},{"5524B3CD", "D5EEE6A406"},
        {"34F168D3", "97BBB8C905"},{"25CCF48B", "B5CFD9C00E"},{"1CD40F9B", "C7AF81CF0C"},{"0E9DD89E", "E38BBB920C"},
        {"0E2166D9", "E3FBCEE904"},{"E96E3942", "D2BBCBA308"},{"E58E6CB9", "B5C49BE908"},{"D9877E9C", "CDE08BB80C"},
        {"CCFD619D", "E788F0A90C"},{"BF127CBB", "81B59FC808"},{"B948C790", "8DDDC5F30D"},{"9788E7A2", "D1DDC3D10B"},
        {"35824954", "EA88CCC40A"},{"225230DC", "BBB7FDBC04"},{"1488DE46", "A8A0F4ED08"},{"0DD91D53", "9AE4EEB10A"},
        {"FE881D31", "FCA3EC9106"},{"FA63C345", "F48F9BDC08"},{"EFF2048F", "A1B4D88F0E"},{"E8EE2B2E", "D0BBDFE205"},
        {"E76A577C", "CEABBBC50F"},{"E5E93053", "CAA787B30A"},{"E445DA7C", "C897D2CD0F"},{"E4057D64", "C897E8C70C"},
        {"E2124C2D", "C4CBE0D405"},{"E009D524", "C0A7A8CD04"},{"DF9C84F9", "C18CDB67"},{"DEA786B2", "C3E0CAD709"},
        {"D8417D68", "B087EA870D"},{"D509ACE5", "D5D89FA503"},{"D2027F90", "DBF487F80D"},{"D1CB0A30", "A2AFD68006"},
        {"D001C8DB", "DFF8BFC304"},{"CF93DAE9", "E1B0ABE202"},{"CE6F1BC4", "E3C0A4BE07"},{"CE177060", "9CDF80870C"},
        {"CD6BBC3C", "9AAFE3CB07"},{"C9BC800C", "92F385C801"},{"C563FFB6", "F5F0849009"},{"C478720B", "88E393B701"},
        {"C1DB708B", "FD90F9C80E"},{"C1918EE0", "FDB88BF703"},{"C161F5A6", "FDF8D4900B"},{"C007A46E", "809FA0EA0D"},
        {"B9CC3125", "F2B28ED304"},{"B1B4B236", "E2D295EB06"},{"B148E3C5", "9DDDE5A107"},{"AAB6365E", "D4DAB5E30B"},
        {"AA417C9C", "ABF99DB80C"},{"AA2697D7", "ABE5C68605"},{"A8B84AF3", "AF9DAACB01"},{"A865F0FA", "AFE9FC50"},
        {"A6D3BA4C", "CCCED6CB09"},{"A5B1BC4E", "CAC6E5EB09"},{"A5A4A0C0", "B5EDFAF507"},{"A3BE9693", "B985CAC60D"},
        {"A006BD66", "C09AE8EB0C"},{"9E460FB3", "C3E585CF09"},{"9CBE564E", "B8FAB5E509"},{"9CBA49D9", "C795B2EB04"},
        {"97D1C6FF", "D1B9C903"},{"96967A37", "ACDAD4F706"},{"93F86F21", "A6E2FF9604"},{"93A4C29F", "D9EDEA830C"},
        {"92918F75", "A4C6FCD80E"},{"8F8D9142", "9EB68CA908"},{"8BADC162", "96B68DAC0C"},{"8826A7FE", "EFE5C615"},
        {"867795E8", "F3A1D4F602"},{"840F0A6D", "88BED0D00D"},{"824A571A", "84AABAA503"},{"7F8BAF42", "FEADFCAA08"},
        {"7F5D1674", "FEF5B2C10E"},{"7A7DEED9", "8B8A8CE104"},{"7536FBDD", "95A6A6A004"},{"723C22D5", "9B8EEEAD05"},
        {"7237CA8B", "9BA2AEC30E"},{"6F799C0D", "DEE5E3D901"},{"6D57D8F2", "A5A2BDD201"},{"6D128A92", "A5B6AFD70D"},
        {"693B97CB", "AD92C6C606"},{"66B58604", "CCD5B548"},{"61CECCE1", "BDC699E303"},{"5EB29CBA", "C3B69AD608"},
        {"5E25E56B", "BC95A9BE0D"},{"5D23FC59", "BA8DE19F0B"},{"5CF685D5", "C7A6D0A705"},{"5CF38100", "B8CD8F08"},
        {"5B91DCA8", "C9BA9BF20A"},{"5B679415", "B69DA3D902"},{"5A386821", "B4E1C19604"},{"59EE4EFA", "CDC6885B"},
        {"585BBF4B", "B0EDFABB09"},{"58334DD3", "CFB296CB05"},{"57DE9D28", "AEF9EE8905"},{"574FF05E", "AEBD82EF0B"},
        {"5716FC69", "AED9E09F0D"},{"55447C72", "AA91E2A70E"},{"5217AC16", "A4DDE0EA02"},{"4F3F254C", "9EFDA9C209"},
        {"49DF913E", "92FD8EE907"},{"45D36C16", "8ACDE6E602"},{"42C0065E", "8481B6E00B"},{"3CD9E7B8", "879BC1F108"},
        {"3BE9912A", "F6A48FA905"},{"3B86F281", "89E7EBE00F"},{"3AF10DB1", "8BBB90EF09"},{"397BE6A3", "8D93CCC10B"},
        {"3970087E", "F2C0C3E00F"},{"370990B8", "91DBFFF608"},{"33CEC416", "E6B8A6EC02"},{"30FE9977", "E0F8CFF90E"},
        {"2D59D944", "DAE4CACD08"},{"28370CD5", "AFA39EAF05"},{"276295D5", "B1F7D4A605"},{"257D2298", "B58BECFD0C"},
        {"24E85958", "C8A0CF850B"}
    };





    QMap<QString,int> Number_Of_NPCsMap;

    QMap <float,QVector<QString>> Distance_Names;


    QHostAddress mainip;


    QString FileName = "C:/PCSX2/" + QString::number(0) + ".txt";


    QTimer TeleportTimer;
    QTimer CoachTimer;




};


#endif // MAINWINDOW_H
