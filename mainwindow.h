#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QTcpSocket>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define ZW_PACKET_MAX_BYTES 992

typedef struct
{
    uint16_t index;
    uint16_t len;
    QByteArray Data;
}binblock;

typedef struct
{
    uint16_t blockNum;
    uint16_t curBlock;
}ST_IAP_INFO;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

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
};

#endif // MAINWINDOW_H
