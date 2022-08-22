#include "hardcmd.h"
#include "quihelperdata.h"

QByteArray HardCmd::formatBoardCmd(emBoardCmdId cmdid, QByteArray cmdconetent)
{
    QByteArray boardCmdArry;
    ushort crcValue = 0xFFFF;

    boardCmdArry.append( QUIHelperData::ushortToByte(WK_BOARD_COMMAND_HEAD) );      // 指令头
    boardCmdArry.append( QUIHelperData::ushortToByte(10 + cmdconetent.size()) );    // 指令长度
    boardCmdArry.append(ENUM_BOARD_NO_ENCRYPT_VER_SERIAL_PORT);                     // 类型标识
    boardCmdArry.append(ENUM_BOARD_ASK_TABLE);                                      // 请求标识
    boardCmdArry.append('\0');                                                      // 预留
    boardCmdArry.append(cmdid);                                                     // 指令ID
    boardCmdArry.append( QUIHelperData::intToByte(WK_BOARD_COMMAND_FLAG));          // 指令FLAG
    boardCmdArry.append(cmdconetent);
    crcValue = QUIHelperData::getModbus16( (quint8*)&boardCmdArry.data()[2], (10 + cmdconetent.size()) );
    boardCmdArry.append( QUIHelperData::ushortToByteRec(crcValue) );                // CRC校验
    boardCmdArry.append( QUIHelperData::ushortToByte(WK_BOARD_COMMAND_END));        // 指令尾

    return boardCmdArry;
}

bool HardCmd::parseBoardResponse(QByteArray &boardcmd, quint8 &cmdid, QByteArray &cmdconetent)
{
    ushort crcValue, crcCnt;
    ushort head = QUIHelperData::byteToUShort(boardcmd.mid(0,2));
    ushort tail;
    ushort cmdlen =boardcmd.size();
    ushort datalen;

    if(WK_BOARD_COMMAND_HEAD == head)
    {
        datalen = QUIHelperData::byteToUShort(boardcmd.mid(2,2));
        if((datalen + 6) <= cmdlen)
        {
            tail = QUIHelperData::byteToUShort(boardcmd.mid(datalen + 4,2));
            if(WK_BOARD_COMMAND_END == tail)
            {
                crcCnt = QUIHelperData::getModbus16((quint8*)&boardcmd.data()[2], datalen);
                crcValue = QUIHelperData::byteToUShortRec(boardcmd.mid(datalen + 2, 2));
                if(crcValue == crcCnt)
                {
                    cmdid = boardcmd[7];
                    cmdconetent = boardcmd.mid(12, datalen - 10);
                    return true;
                }
            }
        }
    }
    return false;
}
