/**************************************************************************************************
 * Author:  ChenFeiyang
 * Date:    2022-8-21
 * SoftName:智微Morphogo固件升级平台
 * Version: 202208231339
 * ************************************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "hardcmd.h"
#include "quihelperdata.h"
#include <winsock.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    iapinfo = { 0 };
    iapinfo.isMainFinish = 1;
    iapinfo.isExpFinish = 1;
    binblocks = new QList<binblock>;
    connect(this, SIGNAL(getMainSch()), this, SLOT(getMainBoardIapSchedule()));
    connect(this, SIGNAL(resetMain()), this, SLOT(resetMainBoardSystem()));
    connect(this, SIGNAL(setMainIapData()), this, SLOT(setMainBoardIapData()));
    connect(this, SIGNAL(synExpSch()), this, SLOT(synExpBoardIapSchedule()));
    connect(this, SIGNAL(resetExp()), this, SLOT(resetExpBoardSystem()));
    connect(this, SIGNAL(setExpIapData()), this, SLOT(setExpBoardIapData()));
//    iapaction = new IapAction;
//    iapaction->moveToThread(&iapThread);

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
    ui->btn_write->setDisabled(true);
    ui->btn_write_2->setDisabled(true);
    ui->progressBar->setTextVisible(false);
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

    ui->progressBar->setValue(0);
    ui->progressBar->setTextVisible(false);
}

void MainWindow::error()
{
    statusBar()->showMessage(socket->errorString(), 0);
}

void MainWindow::readData()
{
    QByteArray data = socket->readAll();
    if (data.length() <= 0) {
        return;
    }

    quint8 cmdid;
    QByteArray cmdcontent;
    if(HardCmd::parseBoardResponse(data, cmdid, cmdcontent))
    {
        if(EnumBoardId_setIAPFirmwareInfo == cmdid)
        {
            /*pMsg 内容：iapSumBytes(U32) + iapSumBlock(U16) + checkData(U8[8])*/
            /*pReturnMsg 内容：bIsSucceed(U8)*/
            iapinfo.isMainBoardIapInfoSet = cmdcontent[0];
            if(iapinfo.isMainBoardIapInfoSet)
            {
                ui->statusBar->showMessage(QStringLiteral("正在更新......"), 1000);
                ui->progressBar->setTextVisible(true);
                ui->progressBar->setValue(0);
                emit getMainSch();
            }
            else
            {
                qDebug() << "main iapinfo set Err";
                QMessageBox::critical(this, tr("Main IAP"), tr("set iapinfo Err!"));
                return;
            }
        }
        else if(EnumBoardId_getIAPSchedule == cmdid)
        {
            /*pMsg 内容：空 */
            /*pReturnMsg 内容：curBlock(U16)+ isFinsh(U8)*/
            iapinfo.mainCurBlock = QUIHelperData::byteToUShort(cmdcontent.mid(0, 2));
            iapinfo.isMainFinish = cmdcontent[2];
            if(iapinfo.isMainFinish)
            {
                ui->progressBar->setValue(100);
                emit resetMain();
            }
            else
            {
                quint8 progress = iapinfo.mainCurBlock*100/iapinfo.blockNum;
                ui->progressBar->setValue(progress);
                emit setMainIapData();
            }
        }
        else if(EnumBoardId_setIAPFirmwareData == cmdid)
        {
            /*pMsg 内容：curBlock(U16) + lenght(U16) + curBlockCrc(U16) + iapDtat(U8[N])*/
            /*pReturnMsg 内容：bIsSucceed(U8)*/
            if(cmdcontent[0])
            {
                emit getMainSch();
            }
            else
            {
                qDebug() << "main iap block[%d] write Err" << iapinfo.mainCurBlock;
                QMessageBox::critical(this, tr("Main IAP"), tr("Update Fail, Block[%d] write Err!"));
                return;
            }
        }
        else if(EnumBoardId_setExpIAPFirmwareInfo == cmdid)
        {
            /*pMsg 内容：iapSumBytes(U32) + iapSumBlock(U16) + checkData(U8[8])(ignore)*/
            /*pReturnMsg 内容：bIsExpBoard_Receive(U8)*/
            if(cmdcontent[0])
            {
                emit synExpSch();
            }
            else
            {
                qDebug() << "main not send iapinfo to Exp" << iapinfo.mainCurBlock;
                QMessageBox::critical(this, tr("Exp IAP"), tr("Main not send iapinfo to Exp!"));
                return;
            }
        }
        else if(EnumBoardId_getExpIAPSchedule == cmdid)
        {
            /*pMsg 内容：空 */
            /*pReturnMsg 内容：m_exp_curBlock(U16)+ g_isExpIapInfoSet(U8)+ g_isCurBlockDataSet(U8)+ m_exp_isFinsh(U8)*/
            iapinfo.expCurBlock = QUIHelperData::byteToUShort(cmdcontent.mid(0, 2));
            iapinfo.isExpBoardIapInfoSet = cmdcontent[2];
            iapinfo.isExpCurBlockDataSet = cmdcontent[3];
            iapinfo.isExpFinish = cmdcontent[4];

            if(2 == iapinfo.isExpBoardIapInfoSet)
            {
                qDebug() << "exp set iapinfo Err";
                QMessageBox::critical(this, tr("Exp IAP"), tr("Exp set iapinfo Err!"));
                return;
            }
            else if(0 == iapinfo.isExpBoardIapInfoSet)
            {//继续同步
                emit synExpSch();
            }
            else
            {
                if(2 == iapinfo.isExpCurBlockDataSet)
                {
                    qDebug() << "exp write iapdata Err";
                    QMessageBox::critical(this, tr("Exp IAP"), tr("Exp write iapdata Err!"));
                    return;
                }
                else if(0 == iapinfo.isExpCurBlockDataSet)
                {
                    emit synExpSch();
                }
                else
                {
                    if( (iapinfo.isExpFinish) || (iapinfo.expCurBlock == iapinfo.blockNum) )
                    {
                        qDebug() << "exp iap finish";
                        emit resetExp();
                    }
                    else
                    {
                        qDebug() << "pc send exp iapdata,block[%d]" <<iapinfo.expCurBlock;
                        emit setExpIapData();
                    }
                }
            }
        }
        else if(EnumBoardId_setExpIAPFirmwareData == cmdid)
        {
            /*pMsg 内容：curBlock(U16) + lenght(U16) + curBlockCrc(U16) + iapDtat(U8[N])*/
            /*pReturnMsg 内容：bIsSucceed(U8)*/
            if(cmdcontent[0])
            {
                qDebug() << "syn exp iapdata write status[%d]" << iapinfo.expCurBlock;
                emit synExpSch();
            }
            else
            {
                qDebug() << "main not send iapdata to Exp" << iapinfo.mainCurBlock;
                QMessageBox::critical(this, tr("Exp IAP"), tr("Main not send iapdata to Exp!"));
                return;
            }
        }
    }
    else
    {
        qDebug() << "board response parse Err";
    }
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

        binblocks->clear();
        for(quint16 i =0; i < blocknum; i++)
        {
            binblock* curblock = new binblock;
            curblock->index = i;
            if( (remainbytes) && ((blocknum - 1) == i))
                curblock->len = remainbytes;
            else
                curblock->len = ZW_PACKET_MAX_BYTES;
            curblock->Data.append(binByteArray.sliced(i*ZW_PACKET_MAX_BYTES, curblock->len));
            curblock->crcValue = QUIHelperData::getModbus16( (quint8*)curblock->Data.data(), curblock->len );

            binblocks->append(*curblock);
            delete curblock;
        }
        iapinfo.byteNum = binSize;
        iapinfo.blockNum = binblocks->size();

        ui->textBrowser->insertPlainText( tr("%1\t%2字节\n").arg( binInfo.fileName(), QString::number(binSize) ) );
        ui->btn_write->setEnabled(true);
        ui->btn_write_2->setEnabled(true);
    }
    else
    {
        ui->btn_write->setDisabled(true);
        ui->btn_write_2->setDisabled(true);
    }
}


void MainWindow::on_btn_write_clicked()
{
    /*pMsg 内容：iapSumBytes(U32) + iapSumBlock(U16) + checkData(U8[8])(ignore)*/
    /*pReturnMsg 内容：bIsSucceed(U8)*/
    if(!net_status)
    {
        QMessageBox::information(this, tr("开始更新主板"), tr("主板未连接！"));
        return;
    }
    QByteArray pMsg;
    pMsg.append( QUIHelperData::intToByte(iapinfo.byteNum) );
    pMsg.append( QUIHelperData::ushortToByte(iapinfo.blockNum) );
    iapinfo.isMainBoardIapInfoSet = 0;
    ui->statusBar->showMessage(QStringLiteral("正在写入更新信息......"), 0);
    socket->write(HardCmd::formatBoardCmd(EnumBoardId_setIAPFirmwareInfo, pMsg));
}

void MainWindow::getMainBoardIapSchedule()
{
    /*pMsg 内容：空 */
    /*pReturnMsg 内容：curBlock(U16)+ isFinsh(U8)*/
    QByteArray pMsg;
    socket->write(HardCmd::formatBoardCmd(EnumBoardId_getIAPSchedule, pMsg));
}

void MainWindow::resetMainBoardSystem()
{
    ui->statusBar->showMessage(QStringLiteral("正在重启主板......"), 1000);
    /*pMsg 内容：checkData(U8[8])*/
    /*pReturnMsg 内容：空 */
    QByteArray pMsg(8, 0);
    socket->write(HardCmd::formatBoardCmd(EnumBoardId_codeSystemReset, pMsg));
}

void MainWindow::setMainBoardIapData()
{
    /*pMsg 内容：curBlock(U16) + lenght(U16) + curBlockCrc(U16) + iapDtat(U8[N])*/
    /*pReturnMsg 内容：bIsSucceed(U8)*/
    QByteArray pMsg;
    pMsg.append( QUIHelperData::ushortToByte( iapinfo.mainCurBlock ) );
    pMsg.append( QUIHelperData::ushortToByte( binblocks->at(iapinfo.mainCurBlock).len ) );
    pMsg.append( QUIHelperData::ushortToByteRec( binblocks->at(iapinfo.mainCurBlock).crcValue ) );
    pMsg.append( binblocks->at(iapinfo.mainCurBlock).Data );
    socket->write(HardCmd::formatBoardCmd(EnumBoardId_setIAPFirmwareData, pMsg));
}


void MainWindow::on_btn_cleaar_clicked()
{
    ui->textBrowser->clear();
}


void MainWindow::on_btn_write_2_clicked()
{
    /*pMsg 内容：iapSumBytes(U32) + iapSumBlock(U16) + checkData(U8[8])(ignore)*/
    /*pReturnMsg 内容：bIsSucceed(U8)*/
    if(!net_status)
    {
        QMessageBox::information(this, tr("开始更新副板"), tr("主板未连接！"));
        return;
    }
    QByteArray pMsg;
    pMsg.append( QUIHelperData::intToByte(iapinfo.byteNum) );
    pMsg.append( QUIHelperData::ushortToByte(iapinfo.blockNum) );
    ui->statusBar->showMessage(QStringLiteral("正在写入更新信息......"), 0);
    socket->write(HardCmd::formatBoardCmd(EnumBoardId_setExpIAPFirmwareInfo, pMsg));
}

void MainWindow::synExpBoardIapSchedule()
{
    /*pMsg 内容：空 */
    /*pReturnMsg 内容：m_exp_curBlock(U16)+ m_exp_isFinsh(U8)+ g_isExpIapInfoSet(U8)+ g_isExpReadyWrite(U8)*/
    QByteArray pMsg;
    socket->write(HardCmd::formatBoardCmd(EnumBoardId_getExpIAPSchedule, pMsg));
}

void MainWindow::resetExpBoardSystem()
{
    ui->statusBar->showMessage(QStringLiteral("正在重启副板......"), 1000);
    /*pMsg 内容：checkData(U8[8])*/
    /*pReturnMsg 内容：空 */
    QByteArray pMsg(8, 0);
    socket->write(HardCmd::formatBoardCmd(EnumBoardId_ExpcodeSystemReset, pMsg));
}

void MainWindow::setExpBoardIapData()
{
    /*pMsg 内容：curBlock(U16) + lenght(U16) + curBlockCrc(U16) + iapDtat(U8[N])*/
    /*pReturnMsg 内容：bIsSucceed(U8)*/
    QByteArray pMsg;
    pMsg.append( QUIHelperData::ushortToByte( iapinfo.expCurBlock ) );
    pMsg.append( QUIHelperData::ushortToByte( binblocks->at(iapinfo.expCurBlock).len ) );
    pMsg.append( QUIHelperData::ushortToByteRec( binblocks->at(iapinfo.expCurBlock).crcValue ) );
    pMsg.append( binblocks->at(iapinfo.expCurBlock).Data );
    socket->write(HardCmd::formatBoardCmd(EnumBoardId_setExpIAPFirmwareData, pMsg));
}

