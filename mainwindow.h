#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QTcpSocket>
#include <QFileDialog>
#include <QThread>

#include "iapaction.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define ZW_PACKET_MAX_BYTES 992

typedef struct
{
    uint16_t index;
    uint16_t len;
    uint16_t crcValue;
    QByteArray Data;
}binblock;

typedef struct
{
    uint32_t byteNum;
    uint16_t blockNum;
    uint8_t isMainBoardIapInfoSet;
    uint16_t mainCurBlock;
    uint16_t expCurBlock;
    uint8_t isMainFinish;
    uint8_t isExpFinish;
}ST_IAP_INFO;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public:
    QThread iapThread;
    IapAction *iapaction;
private:
    QTcpSocket *socket;
    bool net_status = false;
    QByteArray binByteArray;
    QList<binblock> *binblocks;
    ST_IAP_INFO iapinfo;

private slots:
    void connected();
    void disconnected();
    void error();
    void readData();
    void on_btn_net_clicked();
    void on_btn_file_clicked();
    void on_btn_write_clicked();
    void getMainBoardIapSchedule();
    void resetMainBoardSystem();
    void setMainBoardIapData();


    void on_btn_cleaar_clicked();

signals:
    void getMainSch();
    void resetMain();
    void setMainIapData();
};

#endif // MAINWINDOW_H
