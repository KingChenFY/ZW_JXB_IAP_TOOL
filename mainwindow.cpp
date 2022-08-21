#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //实例化Socket对象并绑定网络信号槽
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(error()));
#else
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
#endif

    //Ui配置
    setFixedSize ( 660, 460 );
    setWindowTitle(QStringLiteral("智微Morphogo固件升级平台V1.0"));
    ui->combo_ip->addItem("10.10.10.150");
    ui->combo_ip->addItem("192.168.3.61");
    ui->combo_port->addItem("8888");
    ui->statusBar->showMessage(QStringLiteral("就绪"), 1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btn_net_clicked()
{
    if (!socket){
        statusBar()->showMessage(QStringLiteral("Socket构造失败"), 5000);
        return;
    }
    if((ui->combo_ip->currentText().isEmpty()) || (ui->combo_port->currentText().isEmpty()))
    {
        statusBar()->showMessage(QStringLiteral("未填写ip/端口"), 5000);
        return;
    }
    statusBar()->clearMessage();

    if(net_status == false)
    {
        socket->connectToHost(ui->combo_ip->currentText(),  ui->combo_port->currentText().toUInt());
    }
    else
    {
        socket->abort();
    }
}

void MainWindow::connected()
{
    net_status = true;
    ui->btn_net->setText("断开主板");
}

void MainWindow::disconnected()
{
    net_status = false;
    ui->btn_net->setText("连接主板");
}

void MainWindow::error()
{
    statusBar()->showMessage(socket->errorString(), 0);
}

void MainWindow::readData()
{
}


void MainWindow::on_btn_file_clicked()
{
    QString binPath = QFileDialog::getOpenFileName(this, QStringLiteral("选择bin文件"), ".",QStringLiteral("文件 (*.bin)"));

    if(!binPath.isNull())
    {
        ui->lineEdit->setText(binPath);

        QFile binFile(binPath);
        QFileInfo binInfo(binPath);
        const int binSize = binInfo.size();

        if(!binSize)
        {
            QMessageBox::warning(this, tr("BinSize"), tr("Bin File Empty"));
            return;
        }
        if(!binFile.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, tr("Open BinFile"), tr("Cannot open bin:\n%1").arg(binPath));
            return;
        }
        binByteArray = binFile.readAll();

        quint16 blocknum = (binSize + ZW_PACKET_MAX_BYTES - 1) / ZW_PACKET_MAX_BYTES;
        quint16 remainbytes = binSize % ZW_PACKET_MAX_BYTES;
        binblocks = new QList<binblock>;
        for(quint16 i =0; i < blocknum; i++)
        {
            binblock* curblock = new binblock;
            curblock->index = i;
            if( (remainbytes) && ((blocknum - 1) == i))
                curblock->len = remainbytes;
            else
                curblock->len = ZW_PACKET_MAX_BYTES;
            curblock->Data.append(binByteArray.sliced(i*ZW_PACKET_MAX_BYTES, curblock->len));
            binblocks->append(*curblock);
        }
        iapinfo.blockNum = binblocks->size();
        iapinfo.curBlock = 0;



        ui->textBrowser->insertPlainText(binInfo.fileName());
        ui->textBrowser->insertPlainText("      ");
        ui->textBrowser->insertPlainText(QString::number(binSize));
        ui->textBrowser->insertPlainText(QStringLiteral("字节"));
        ui->textBrowser->moveCursor(QTextCursor::End);//接收框始终定为在末尾一行
//        ui->btn_menu->setEnabled(true);
//        ui->btn_erase->setEnabled(true);
//        ui->btn_update->setEnabled(true);
//        ui->btn_run->setEnabled(true);
//        ui->btn_write->setEnabled(true);
//        ui->btn_copy->setEnabled(true);
    }
    else
    {
//        ui->btn_menu->setDisabled(true);
//        ui->btn_erase->setDisabled(true);
//        ui->btn_update->setDisabled(true);
//        ui->btn_run->setDisabled(true);
//        ui->btn_write->setDisabled(true);
    }
}

