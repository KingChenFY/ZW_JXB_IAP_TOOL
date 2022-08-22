#ifndef HARDCMD_H
#define HARDCMD_H

#include <QObject>

/* 跟硬件主板的通讯封装协议定义  高位在前 低位在后  总长度16+N*/
typedef enum
{
    ENUM_BOARD_CMD_VERID_START					= 0 ,
    ENUM_BOARD_NO_ENCRYPT_VER_SERIAL_PORT		= 0xA1, // 不加密串口类型
    ENUM_BOARD_NO_ENCRYPT_VER_WAN				= 0xA2, // 不加密网口类型
    ENUM_BOARD_CMD_VERID_END
}WK_BOARD_CMD_VERID_ENUM;

typedef enum
{
    ENUM_BOARD_CMD_TABLEID_START					= 0 ,
    ENUM_BOARD_ASK_TABLE							= 0x30, // 指令请求类型
    ENUM_BOARD_ANSWER_TABLE							= 0x31, // 指令回复类型
    ENUM_BOARD_CMD_TABLEID_END
}WK_BOARD_CMD_TABLEID_ENUM;

#define WK_BOARD_COMMAND_HEAD           0xEA9D
#define WK_BOARD_COMMAND_END            0x1206
#define WK_BOARD_COMMAND_FLAG           0x00000000
#define WK_BOARD_COMMAND_MAX_LEN        1024
#define WK_BOARD_COMMAND_ID_INDEX		7

#define WK_BOARD_CMD_HEAD_LENGTH        (12)
#define WK_BOARD_CMD_END_LENGTH        (4)

#define WK_COMMAND_BOARD_SAVE_CFG_SECTION_LEN        200

/*  |       2         |     2    |     1    |     1    |  1   |   1    |     4    |     N    |    2      |    2             | */
/*  协议头 0xEA 0x9D   | 指令长度 | 类型标识 | 请求标识 | 预留 | 指令ID | 指令FLAG | 指令内容 |   | 协议尾 0x12 0x06   */

/*  注 :
    1、指令长度包含的内容(10+N) = 指令长度 + 类型标识 + 请求标识 + 预留 + 指令ID + 指令FLAG + 指令内容（总长度-6）
    2、类型标识 : WK_BOARD_CMD_VERID_ENUM (本文件内)
    3、请求标识 : WK_BOARD_CMD_TABLEID_ENUM (本文件内)
    4、指令ID : CmdPcToBoard / CmdBoardToPc (文件HardCmd.h内)
    5、CRC计算包含        容 : 		  指令长度 + 类型标识 + 请求标识 + 预留 + 指令ID + 指令FLAG + 指令内容
*/

typedef enum
{
    EnumBoardId_START = 0, // 未知

    /* 请求： */
    /* 返回：SerialID_part1(U32) + SerialID_part2(U32) + SerialID_part3(U32) + hard_ver(U32) + soft_ver(U32)+ soft_setup_address(U32)
    +SerialID_part1(U32) + SerialID_part2(U32) + SerialID_part3(U32) + hard_ver(U32) + soft_ver(U32)+ soft_setup_address(U32) */
    EnumBoardId_getBaseInfo = 1, // 获取基础信息

    /* 请求： */
    /* 返回：IsBoard2Valid(U8 WK_TRUE/WK_FALSE) PCB板1与板2的连接状态是否有效
        + eSensors(U8[emSensorId_End] emSensorStatus) 各传感器状态 + 最后一个-旋转仓玻片盒存在*/
    EnumBoardId_getAllSensors = 2, // 获取各位置的传感器当前状态信息

    /*****************************************************滴油模块********************************************************/
    /* 请求：eTskType(U8 emTskDDropOilType) + uTaskId(U8) */
    /* 返回：*/
    EnumBoardId_setOilTask = 3, // 设置滴油模块任务

    /* 请求：*/
    /* 返回：eTskType(U8 emTskDDropOilType) + uTaskId(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDDropOil) */
    EnumBoardId_getOilTaskInfo = 4, // 获取滴油模块任务信息

    /*****************************************************光源模块********************************************************/
    /* 请求：eTskType(U8 emTskDBrightType) + LedSelect(U8) + IsTri(U8) + BrightValue(U16) + TriTime(U16)*/
    /* 返回：*/
    EnumBoardId_setBrightTask = 5,

    /* 请求： */
    /* 返回：eTskType(U8 emTskDBrightType) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDBright)
        + LedSelect(U8) + IsTri(U8) + BrightLevel(U16) + TriDis(U16) */
    EnumBoardId_getBrightTaskInfo = 6, // 获取光源模块任务信息

    /*****************************************************玻片盒翻转模块*************************************************/
    /* 请求： eTskType(U8 emTskDUpenderType) + uTaskId(U8)*/
    /* 返回： */
    EnumBoardId_setUpenderTask = 7,//设置玻片盒翻转任务

    /* 请求：*/
    /* 返回：eTskType(U8 emTskDUpenderType) + uTaskId(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDUpender) + eCurPos(U8 emPositionType) + m_iIsMove(U8) */
    EnumBoardId_getUpenderTaskInfo = 8,//获取玻片盒翻转任务状态

    /*****************************************************二维码扫描模块*************************************************/
    /* 请求：eTskType(U8 emTskDQRScannerType) + uScannerId(U8) */
    /* 返回： */
    EnumBoardId_setQRScannerTask = 9, // 设置扫码任务

    /* 请求： */
    /* 返回：eTskType(U8 emTskDQRScannerType) + uScannerId(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDQRScanner) + Connect(U8[12]) */
    EnumBoardId_getQRScannerTaskInfo = 10, // 获取二维码扫描功能结果信息

    /*******************************************************夹爪模块****************************************************/
    /* 请求：eTskType(U8 emTskDGripperType) + uTskId(U8)+ uTskStep(U8) */
    /* 返回：*/
    EnumBoardId_setGripperTask = 11, // 设置夹爪任务

    /* 请求： */
    /* 返回：eTskType(U8 emTskDGripperType) + uTskId(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDGripper)
    + IsClamp(U8 WK_TRUE/WK_FALSE) 夹爪当前方向+ uTskStep(U8)+ nowStep(U8)+ TotalLength(U16)+ IsEdgeCheck(U8)+ SlideExist(U8) */
    EnumBoardId_getGripperTaskInfo = 12, // 获取夹爪任务信息

    /*****************************************************入舱皮带模块*************************************************/
    /* 请求： eTskType(U8 emTskDBeltInType) + uTskId(U8) + uIsScan(U8)*/
     /* 返回： */
     EnumBoardId_setBeltInTask = 13,//设置入舱皮带任务

     /* 请求：*/
     /* 返回：eTskType(U8 emTskDBeltInType)  + uTskId(U8) + uIsScan(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDBelt)
      + IsWorking(U8 WK_TRUE/WK_FALSE) 皮带是否在运动
      + IsDirectionInSide(U8 WK_TRUE/WK_FALSE) 皮带方向是否向里
      + QRScanContent(U8[12]) 二维码扫描内容*/
     EnumBoardId_getBeltInTaskInfo = 14,//获取入舱皮带任务状态
    /*****************************************************入舱推杆模块*************************************************/
    /* 请求： eTskType(U8 emTskDPoleInType) + uTaskId(U8)*/
    /* 返回： */
    EnumBoardId_setPoleInTask = 15,//设置入舱推杆任务

    /* 请求：*/
    /* 返回：eTskType(U8 emTskDPoleInType) + uTaskId(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDPole) + eCurPos(U8 emPositionType) */
    EnumBoardId_getPoleInTaskInfo = 16,//获取入舱推杆任务状态

    /*****************************************************出舱推杆模块*************************************************/
    /* 请求： eTskType(U8 emTskDPoleOutType) + uTaskId(U8)*/
    /* 返回： */
    EnumBoardId_setPoleOutTask = 17,//设置出舱推杆任务

    /* 请求：*/
    /* 返回：eTskType(U8 emTskDPoleOutType) + uTaskId(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDPole) + eCurPos(U8 emPositionType) + eBoxNum(U8)*/
    EnumBoardId_getPoleOutTaskInfo = 18,//获取出舱推杆任务状态

    /*****************************************************出舱皮带模块*************************************************/
    /* 请求： eTskType(U8 emTskDBeltOutType) + uTskId(U8)*/
    /* 返回： */
    EnumBoardId_setBeltOutTask = 19,//设置出舱皮带任务

    /* 请求：*/
    /* 返回：eTskType(U8 emTskDBeltOutType) + uTskId(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDBelt)
        + IsWorking(U8 WK_TRUE/WK_FALSE) 皮带是否在运动*/
    EnumBoardId_getBeltOutTaskInfo = 20,//获取出舱皮带任务状态

    /**************************************************XYZ模块**************************************************************/
    /* 请求：eTskType(U8 emTskDXYZType) + PosX(I32) + PosY(I32) + PosZ(I32) + Time(U32 ms)+ id(U16) + PosXStart(I32) + PosYStart(I32) + PosZStart(I32)*/
    /* 返回：*/
    EnumBoardId_setXYZTask = 21, // 设置 XYZ 模块任务

    /* 请求：*/
    /* 返回：eTskType(U8 emTskDXYZType) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDMoveXYZ)
        + PosX(I32) + PosY(I32) + PosZ(I32) + Time(U32 ms) + curPosX(I32) + curPosY(I32) + curPosZ(I32) + rdTmStamp(U64)
        + xMin(I32) + xMax(I32) + yMin(I32) + yMax(I32) + zMin(I32) + zMax(I32) + id(U16) + YInOut(U8) + ZUpDown(U8)
        + PosXStart(I32) + PosYStart(I32) + PosZStart(I32)*/
    EnumBoardId_getXYZTaskInfo = 22, // 获取 XYZ 模块任务信息

    /***********************************************玻片盒垂直扫描模块*****************************************************/
    /* 请求：eTskType(U8 emTskDBoxVType) + iPos(I32) + iStartPos(I32) + iEndPos(I32)*/
    /* 返回：*/
    EnumBoardId_setBoxVTask = 23,//设置垂直扫描玻片盒任务

    /* 请求：*/
    /* 返回：eTskType(U8 emTskDBoxVType) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDBoxV) + Errflag(U16)
        + m_iTaskPos(I32) + m_iPosStartPhy(I32) + m_iPosEndPhy(I32) + m_iCurPosPhy(I32)
         + m_eCurPosType(U8 emBoxVPosType) + m_iMinLine(I32) + m_iMaxLine(I32)+m_iBoxIsMove(U8) */
    EnumBoardId_getBoxVTaskInfo = 24,//获取玻片盒垂直扫描任务状态

    /* 请求：AskNum(U8) + BgnIdx(U8) */
    /* 返回：AskNum(U8) + BgnIdx(U8) + TotalNum(U8) + GetNum(U8)
        + { uFlag(U8 SmearBoxColIdWait/SmearBoxColIdFinish)完成列/等待列
        + uFlag(U8 SmearCoverFlagUpperEdge/SmearCoverFlagLowerEdge)玻片上沿/玻片下沿，
        + rdPos(I32) }  位置 */
    EnumBoardId_getBoxVRecords = 25, // 获取玻片盒垂直扫描记录

    /*****************************************************配置信息模块**************************************************/
    /******** ABC层配置参数指令 *********/
    /* 请求：uCfgId(U16) + uCfgLen(U8) + cfgData(U8[N]) */
    /* 返回：uCfgId(U16) +    bIsSucceed(U8)*/
    EnumBoardId_saveCfgABC = 26, // 保存 ABC 上层使用的配置数据到设备flash里 (长度 N <= WK_COMMAND_BOARD_SAVE_CFG_SECTION_LEN )

    /* 请求：uCfgId(U16)    */
    /* 返回：uCfgId(U16) +    bIsSucceed(U8) + uCfgLen(U8) + cfgData(U8[N])    */
    EnumBoardId_readCfgABC = 27, // 设备flash里读取 ABC 上层使用的配置数据

    /**************************************************************/
    /******** D层配置参数指令 *********/
    /* 请求 见HardCfg.h 结构体cfgDHardware*/
    /* 返回：   bIsSucceed(U8)    */
    EnumBoardId_saveCfgDActn = 28, // 保存设备里 D 层 功能使用的配置参数信息

    /* 请求： */
    /* 返回：   bIsSucceed(U8) + 见HardCfg.h 结构体cfgDHardware*/
    EnumBoardId_readCfgDActn = 29, // 读取设备里 D 层 功能使用的配置参数信息

    /*****************************************************编码模块********************************************************/
    /* 请求：ch(U8)(编码切换：X通道(0),Y通道(1),Z通道(2))+dis(U16)+trigElc(U8 1：外触发电子目镜；0：外触发测距) */
    /* 返回：*/
    EnumBoardId_setEncodeCh = 30,

    /* 请求：*/
    /* 返回：ch(U8)+dis(U16)+trigElc(U8)*/
    EnumBoardId_getEncodeCh = 31,

    /* 请求：bgnIdx(U32 默认值0xFFFFFFFF表示使用当前起始) + askNum(U8) */
    /* 返回：curIdx(U32) + bgnIdx(U32) + askNum(U8) + getNum(U8) + { rdPosX(I32) + rdPosY(I32) + rdPosZ(I32) + rdTmStamp(U64) } */
    EnumBoardId_synXYZPosRecords = 32, // 同步 XYZ 模块的运行轨迹数据

    /* 请求： */
    /* 返回：sizeDCircle(U32 D层环形缓存空间大小)    + curIdx(U32) + rdPosX(I32) + rdPosY(I32) + rdPosZ(I32) + rdTmStamp(U64) */
    EnumBoardId_synFirstXYZRecords = 33, // 获取 XYZ 模块的最新一条运行轨迹数据

    /* 请求：*/
    /* 返回：BoardInfo /硬件两个主板间通讯使用，与PC端无关/ */
    EnumBoardId_getBoardInfo = 34,

    /* 请求：id(U16)+totalNum(U16)+startId(U16)+num(U8)+{pos1(I32)+pos2(I32)}*/
    /* 返回：id(U16)+totalNum(U16)+startId(U16)+num(U8)*/
    EnumBoardId_setXYZReady = 35,//下发移动轨迹数据

    /* 请求：id(U16)+startId(U16)*/
    /* 返回：id(U16)+totalNum(U16)+startId(U16)+num(U8)+{pos1(I32)+pos2(I32) */
    EnumBoardId_getXYZReady = 36,

    /* 请求：bModefy(U8) + flagGripType(I32)*/
    /* 返回：bReadOk(U8) + flagGripType(I32) */
    EnumBoardId_synGripTypeFlag = 37, // 同步夹爪取玻片动作流程当前执行状态(需要修改到硬件存储中，断电不丢失)

    /* 请求：bModefy(U8) + uCfgLen(U16) + cfgData(U8[N]) */
    /* 返回：bReadOk(U8) + uCfgLen(U16) + cfgData(U8[N]) */
    EnumBoardId_saveSlotInfos = 38, // 保存玻片盒的扫描数据到设备flash里 (长度 N <= 990 )

    /* 请求：curCol(U8) + curRow(U8)*/
    /* 返回： bIsSucceed(U8)*/
    EnumBoardId_setMatrixLEDPos = 39, // 设置LED矩阵那个位置灯亮

    /* 请求：uPeriod(U16) + uCycleDelay(U8) + uPercent(U8) + uErrCount(U8) */
    /* 返回：*/
    EnumBoardId_setFollowParam = 40, // 设置跟随移动运动控制相关参数

    /* 请求： */
    /* 返回：uPeriod(U16) + uSpeed(U16) + uCycleDelay(U8) + uPercent(U8) + uErrCount(U8)*/
    EnumBoardId_getFollowParam = 41, // 获取跟随移动运动控制相关参数

    /* 请求：uControlID(U8) + Step(I32) + */
    /* 返回：*/
    EnumBoardId_setXTest = 42, // X轴步进电机直接控制移动指令

    /* 请求：bSetBoxVLedOn(U8 1表示亮，0表示关)*/
    /* 返回：*/
    EnumBoardId_setBoxVLedOn = 43,// 设置玻片盒垂直轴的激光打开指令

    /* 请求：type(U8 控制项) + bOpen(U8 0关闭\1打开)*/
    /* 返回：*/
    EnumBoardId_hardControl = 44,// 硬件零件控制

    /*****************************************************夹爪翻转模块*************************************************/
    /* 请求： eTskType(U8 emTskDRotateType) + uTskId(U8)+iStep(i16)*/
    /* 返回： */
    EnumBoardId_setRotateTask = 45,//设置夹爪翻转任务

    /* 请求：*/
    /* 返回： eTskType(U8 emTskDRotateType) + uTskId(U8)+iStep(i32) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDrotate)+iNowStep(i32)*/
    EnumBoardId_getRotateTaskInfo = 46,//获取夹爪翻转任务状态

    /* 请求：u16 start + u8 needNum*/
    /* 返回：u16 start + u8 num + xyz(U8 目标轴) + AimPos(I32目标位置) + xyzMoveTime(I32移动时间) + TotalNum(I16) + realStart(U16) + realNum(u16) + triPoses{I32} */
    EnumBoardId_getExitTrigInfo = 47,

    /* 请求：eTskType(U8 emTskDCleanOilType) + uTaskId(U8) */
    /* 返回：*/
    EnumBoardId_setCleanOilTask = 48, // 设置去油模块任务

    /* 请求：*/
    /* 返回：eTskType(U8 emTskDCleanOilType) + uTaskId(U8) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDDropOil) */
    EnumBoardId_getCleanOilTaskInfo = 49, // 获取去油模块任务信息

    /* 请求：eTskType(U8 emTskDGalvoType) + uTskId(U8) + i_SetPoint(i16) + i_SetAmp_Up(i16) + i_SetAmp_Down(i16) + i_SetCycle(u16)*/
    /* 返回：*/
    EnumBoardId_setGalvoTask = 50,

    /* 请求：*/
    /* 返回：eTskType(U8 emTskDGalvoType) + eStatus(U8 emWorkStatus) + eSelute(U8 emSeluteDGalvo) + uTskId(U8) + i_SetPoint(i16) + i_SetAmp_Up(i16) + i_SetAmp_Down(i16) + i_SetCycle(u16)*/
    EnumBoardId_getGalvoTaskInfo = 51,

    /* 请求：id(U16)+totalNum(U16)+{param(double)}*/
    /* 返回：isSuccess(U8)+id(U16)+totalNum(U16)+{param(double)}*/
    EnumBoardId_setEquationParam = 52, //设置跟随系数

    /* 请求：mode(U8)(mode:0 获取移动 mode:1 获取跟随)*/
    /* 返回：isSuccess(U8)读取是否成功 + mode(U8) + moveErr(U8)移动偏差 + maxErr(U32)跟随最大偏差 + avgErr(I16)跟随平均偏差 + rangErr(U8)跟随偏差跨度 + sumErr(I32)跟随偏差累计和 + errCnt(U32)总偏差个数 + errNum(U8)记录偏差组数 + {err(I8)偏差值 + cnt(U16)该偏差值计数}*/
    EnumBoardId_getZAxisAdptErr = 53, //获取移动或跟随自适应误差结果

    /* 请求：isFollowTime(U8)(0:跟随轴 1:跟随时间) mode(U8)(mode:0 正常跟随 mode:1 直线跟随 mode:2 正弦跟随)*/
    /* 返回：isSuccess(U8)*/
    EnumBoardId_setFollowMode = 54, //设置跟随模式

    /* 请求：AxisNum(U8) */
    /* 返回：IsSucceed(U8) + AxisNum(U8 实际) */
    EnumBoardId_ResetBoardRunLog = 55,//清空XYZ移动总量计数

    /* 请求： */
    /* 返回：AxisX(U64) + AxisY(U64) + AxisZ(U64) */
    EnumBoardId_GetBoardRunLog = 56,//获取XYZ移动总量计数

    /******** IIT固件升级指令 *********/
    EnumBoardId_setIAPFirmwareInfo = 57,//设置iap信息

    /* 请求： */
    /*pReturnMsg 内容：curBlock(U16)+ isFinsh(U8)*/
    EnumBoardId_getIAPSchedule = 58,//获取iap进度

    EnumBoardId_setIAPFirmwareData = 59,//传输bin数据包

    EnumBoardId_codeSystemReset = 60,//复位

    /******** ZIT固件升级指令 *********/
    EnumBoardId_setExpIAPFirmwareInfo = 61,//设置iap信息

    /*pReturnMsg 内容：exp_curBlock(U16)+ exp_isFinsh(U8)*/
    EnumBoardId_getExpIAPSchedule = 62,//获取iap进度

    EnumBoardId_setExpIAPFirmwareData = 63,//传输bin数据包

    EnumBoardId_ExpcodeSystemReset = 64,//复位

    EnumBoardId_END  //
}emBoardCmdId;

class HardCmd
{
public:
    //Board Data Format&&Parser
    static QByteArray formatBoardCmd(emBoardCmdId cmdid, QByteArray cmdconetent);
    static bool parseBoardResponse(QByteArray &boardcmd, quint8 &cmdid, QByteArray &cmdconetent);
};

#endif // HARDCMD_H
