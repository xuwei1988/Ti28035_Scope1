#include "CANApp.h"
#include "DeviceImfor.h"
#include "CANBasic.h"

/* 定义CANApp模块全局数据 ------------------------------------------------------------*/
CanBuffer buffer[CAN_BUFFER_SIZE];              //存储所有待发送数据信息
CanHelper helper;

static void CanApp_statusRefresh(void);
static void CanApp_writeSN(void);
static void CanApp_writeSWVersion(void);
static void CanApp_writeReleaseDate(void);
static void CanApp_writeDeviceImfor(void);
static void CanApp_Tx(void);
static void CanApp_Rx(void);
static void CanApp_ReceiveCheck(void);
static void CanApp_BootFunction(void);
static void CanApp_writePFCSWVersion(void);
static void CanApp_WriteConfigure(void);

void CanApp_init(void){
    bezero((Uint8 *)&helper, sizeof(helper));
    bezero((Uint8 *)buffer, sizeof(buffer));
    helper.currPos = 0;
    helper.endPos = CAN_TX_RUNIMFOR_MAXPOS;
    helper.bootState = NO_BOOT_REQ;

    CanApp_writeSN();
    CanApp_writeSWVersion();
    CanApp_writeReleaseDate();
    CanApp_writeDeviceImfor();
    CanApp_writePFCSWVersion();
    CanApp_WriteConfigure();
}

void CanApp_MainFunction(void) {
    /* step1: 刷新运行是数据 */
    CanApp_statusRefresh();

    /* step2: 发送数据 */
    CanApp_Tx();
    /* step3: 接收数据 */
    CanApp_Rx();

    /* step4: 检查和本模块逻辑有关的接收数据 */
    CanApp_ReceiveCheck();

    /* step5: 检擦bootload状态机 */
    CanApp_BootFunction();
}

/* 接收状态获取函数，用于指示是否有新的数据被接收
 * 注：此函数调用完毕会自动清除接收状态(用户需及时处理接收数据 )
 */
Uint8 CanApp_GetRecieveIndicator(void) {
    Uint8 res = helper.recvIndicator;
    helper.recvIndicator = 0;
    return res;
}

Uint8 CanApp_IsRecieved(void) {
    return helper.recvIndicator ? 1 : 0;
}

static void CanApp_ReceiveCheck(void) {
    Uint16 *p = &deviceControlImfor[DEVICE_STATE1_SET];
    Uint8 isDCBootNeed = 0;
    Uint8 isPFCBootNeed = 0;
    /* step1: 检查是否需要发送完整设备信息状态 */
    if(((Device_SetState1 *)p)->DEVICE_IMFOR) {
        helper.endPos = CAN_TX_ALLIMFOR_MAXPOS;
        ((Device_SetState1 *)p)->DEVICE_IMFOR = 0;
    }
    isDCBootNeed = ((Device_SetState1 *)p)->DC_MODULE_BOOT;
    ((Device_SetState1 *)p)->DC_MODULE_BOOT = 0;
    isPFCBootNeed = ((Device_SetState1 *)p)->PFC_MODULE_BOOT;
    ((Device_SetState1 *)p)->PFC_MODULE_BOOT = 0;
    helper.bootState = isDCBootNeed + (isPFCBootNeed<<1);
}


static void CanApp_Tx(void) {
    /* step1: 获取待发送数据的buffer ID 及数据指针 */
    Uint8 id = buffer[helper.currPos].boxOffset;
    Uint16 *pdata = buffer[helper.currPos].data;

    /* step2: 尝试发送 */
    if(CanBasic_sendMbox(id, pdata)){
        /* step3: 如果数据已装载到MBOX，更新下一次数据发送的位置  */
        helper.currPos++;
        if(helper.currPos > helper.endPos) {
            helper.endPos = CAN_TX_RUNIMFOR_MAXPOS;

            helper.currPos = 0;
        }

    }
}

static void CanApp_Rx(void) {
    Uint8 res = 0;
    res = CanBasic_recieveMBox(deviceControlImfor);
    helper.recvIndicator |= res;
}

static void CanApp_statusRefresh(void) {
    /* step1: 写Status1邮箱buffer */
    CanBuffer *pb = &buffer[0];

    pb->boxOffset = CAN_TX_STATUS1_OFFSET;
    pb->byteNum = 8;
    pb->data[0] = deviceRunImfor[0];
    pb->data[1] = deviceRunImfor[1];
    pb->data[2] = deviceRunImfor[2];
    pb->data[3] = deviceRunImfor[3];

    /* step2: 写Status2邮箱buffer */
    pb = &buffer[1];

    pb->boxOffset = CAN_TX_STATUS2_OFFSET;
    pb->byteNum = 8;
    pb->data[0] = deviceRunImfor[4];
    pb->data[1] = deviceRunImfor[5];
    pb->data[2] = deviceRunImfor[6];
    pb->data[3] = deviceRunImfor[7];

    /* step3: 写Status3邮箱buffer */
    pb = &buffer[2];

    pb->boxOffset = CAN_TX_STATUS3_OFFSET;
    pb->byteNum = 8;
    pb->data[0] = deviceRunImfor[8];
    pb->data[1] = deviceRunImfor[9];
    pb->data[2] = deviceRunImfor[10];
    pb->data[3] = deviceRunImfor[11];

    /* step4: 写Status3邮箱buffer */
    pb = &buffer[3];

    pb->boxOffset = CAN_TX_STATUS4_OFFSET;
    pb->byteNum = 8;
    pb->data[0] = deviceRunImfor[12];
    pb->data[1] = deviceRunImfor[13];
    pb->data[2] = deviceRunImfor[14];
    pb->data[3] = deviceRunImfor[15];

    /* step4: 写Alarm邮箱buffer */
    pb = &buffer[4];

    pb->boxOffset = CAN_TX_ALARMS_OFFSET;
    pb->byteNum = 8;
    pb->data[0] = deviceRunImfor[16];
    pb->data[1] = deviceRunImfor[17];
    pb->data[2] = deviceRunImfor[18];
    pb->data[3] = deviceRunImfor[19];
}


static void CanApp_writeSN(void) {
    CanBuffer *pb = &buffer[5];
    CanDeviceImfor *pImf = (CanDeviceImfor *)pb->data;

    pb->boxOffset = CAN_TX_DEVIMFOR_OFFSET;
    pb->byteNum = 8;
    pImf->CategoryImfor.bit.categoty = CAN_SN_ID;
    pImf->CategoryImfor.bit.totalSeq = 2;
    pImf->CategoryImfor.bit.currSeq = 1;

    pImf->data[0] = ECUSerialNumber[0];
    pImf->data[0] <<= 8;
    pImf->data[0] += ECUSerialNumber[1];

    pImf->data[1] = ECUSerialNumber[2];
    pImf->data[1] <<= 8;
    pImf->data[1] += ECUSerialNumber[3];

    pImf->data[2] = ECUSerialNumber[4];
    pImf->data[2] <<= 8;
    pImf->data[2] += ECUSerialNumber[5];

    pb = &buffer[6];        //偏移到buffer的下一个元素
    pImf = (CanDeviceImfor *)pb->data;
    pb->boxOffset = CAN_TX_DEVIMFOR_OFFSET;
    pb->byteNum = 6;
    pImf->CategoryImfor.bit.categoty = CAN_SN_ID;
    pImf->CategoryImfor.bit.totalSeq = 2;
    pImf->CategoryImfor.bit.currSeq = 2;

    pImf->data[0] = ECUSerialNumber[6];
    pImf->data[0] <<= 8;
    pImf->data[0] += ECUSerialNumber[7];

    pImf->data[1] = ECUSerialNumber[8];
    pImf->data[1] <<= 8;
    pImf->data[1] += ECUSerialNumber[9];
}

static void CanApp_writeSWVersion(void) {
    CanBuffer *pb = &buffer[7];
    CanDeviceImfor *pImf = (CanDeviceImfor *)pb->data;

    pb->boxOffset = CAN_TX_DEVIMFOR_OFFSET;
    pb->byteNum = 7;
    pImf->CategoryImfor.bit.categoty = CAN_SWVERSION_ID;
    pImf->CategoryImfor.bit.totalSeq = 1;
    pImf->CategoryImfor.bit.currSeq = 1;

    pImf->data[0] = ECUSoftWareVersion[0];
    pImf->data[0] <<= 8;
    pImf->data[0] += ECUSoftWareVersion[1];

    pImf->data[1] = ECUSoftWareVersion[2];
    pImf->data[1] <<= 8;
    pImf->data[1] += ECUSoftWareVersion[3];

    pImf->data[2] = ECUSoftWareVersion[4];
    pImf->data[2] <<= 8;
    pImf->data[2] += ECUSoftWareVersion[5];
}

static void CanApp_writePFCSWVersion(void) {
    CanBuffer *pb = &buffer[8];
    CanDeviceImfor *pImf = (CanDeviceImfor *)pb->data;

    pb->boxOffset = CAN_TX_DEVIMFOR_OFFSET;
    pb->byteNum = 7;
    pImf->CategoryImfor.bit.categoty = CAN_PFCVERSION_ID;
    pImf->CategoryImfor.bit.totalSeq = 1;
    pImf->CategoryImfor.bit.currSeq = 1;

    pImf->data[0] = PrimarySideSoftwareVersion[0];
    pImf->data[0] <<= 8;
    pImf->data[0] += PrimarySideSoftwareVersion[1];

    pImf->data[1] = PrimarySideSoftwareVersion[2];
    pImf->data[1] <<= 8;
    pImf->data[1] += PrimarySideSoftwareVersion[3];

    pImf->data[2] = PrimarySideSoftwareVersion[4];
    pImf->data[2] <<= 8;
    pImf->data[2] += PrimarySideSoftwareVersion[5];

}

static void CanApp_writeReleaseDate(void) {
    CanBuffer *pb = &buffer[9];
    CanDeviceImfor *pImf = (CanDeviceImfor *)pb->data;

    pb->boxOffset = CAN_TX_DEVIMFOR_OFFSET;
    pb->byteNum = 8;
    pImf->CategoryImfor.bit.categoty = CAN_RELEASEDATE_ID;
    pImf->CategoryImfor.bit.totalSeq = 2;
    pImf->CategoryImfor.bit.currSeq = 1;

    pImf->data[0] = ECUReleaseDate[0];
    pImf->data[0] <<= 8;
    pImf->data[0] += ECUReleaseDate[1];

    pImf->data[1] = ECUReleaseDate[2];
    pImf->data[1] <<= 8;
    pImf->data[1] += ECUReleaseDate[3];

    pImf->data[2] = ECUReleaseDate[4];
    pImf->data[2] <<= 8;
    pImf->data[2] += ECUReleaseDate[5];

    pb = &buffer[10];        //偏移到buffer的下一个元素
    pImf = (CanDeviceImfor *)pb->data;
    pb->boxOffset = CAN_TX_DEVIMFOR_OFFSET;
    pb->byteNum = 4;
    pImf->CategoryImfor.bit.categoty = CAN_RELEASEDATE_ID;
    pImf->CategoryImfor.bit.totalSeq = 2;
    pImf->CategoryImfor.bit.currSeq = 2;

    pImf->data[0] = ECUReleaseDate[6];
    pImf->data[0] <<= 8;
    pImf->data[0] += ECUReleaseDate[7];
}

static void CanApp_writeDeviceImfor(void) {
    CanBuffer *pb = &buffer[11];
    CanDeviceImfor *pImf = (CanDeviceImfor *)pb->data;

    pb->boxOffset = CAN_TX_DEVIMFOR_OFFSET;
    pb->byteNum = 8;
    pImf->CategoryImfor.bit.categoty = CAN_DEVICEIMFOR_ID;
    pImf->CategoryImfor.bit.totalSeq = 2;
    pImf->CategoryImfor.bit.currSeq = 1;

    pImf->data[0] = deviceGenImfor.totalChargers;
    pImf->data[1] = deviceGenImfor.completeChargers;
    pImf->data[2] = deviceGenImfor.interruptedChargers;

    pb = &buffer[12];        //偏移到buffer的下一个元素
    pImf = (CanDeviceImfor *)pb->data;
    pb->boxOffset = CAN_TX_DEVIMFOR_OFFSET;
    pb->byteNum = 6;
    pImf->CategoryImfor.bit.categoty = CAN_DEVICEIMFOR_ID;
    pImf->CategoryImfor.bit.totalSeq = 2;
    pImf->CategoryImfor.bit.currSeq = 2;

    pImf->data[0] = deviceGenImfor.chargerHours;
    pImf->data[1] = deviceGenImfor.totalAH;
}

static void CanApp_WriteConfigure(void) {
    CanBuffer *pb = &buffer[13];
    pb->boxOffset = CAN_TX_CONFIGURE_OFFSET;

    pb->byteNum = 8;
    pb->data[0] = deviceControlImfor[0];
    pb->data[1] = deviceControlImfor[1];
    pb->data[2] = deviceControlImfor[2];
    pb->data[3] = deviceControlImfor[3];

    pb = &buffer[14];
    pb->boxOffset = CAN_TX_CONFIGURE_OFFSET;

    pb->byteNum = 8;
    pb->data[0] = deviceControlImfor[4];
    pb->data[1] = deviceControlImfor[5];
    pb->data[2] = deviceControlImfor[6];
    pb->data[3] = deviceControlImfor[7];

    pb = &buffer[15];
    pb->boxOffset = CAN_TX_CONFIGURE_OFFSET;

    pb->byteNum = 8;
    pb->data[0] = deviceControlImfor[8];
    pb->data[1] = deviceControlImfor[9];
    pb->data[2] = deviceControlImfor[10];
    pb->data[3] = deviceControlImfor[11];

    pb = &buffer[16];
    pb->boxOffset = CAN_TX_CONFIGURE_OFFSET;

    pb->byteNum = 8;
    pb->data[0] = deviceControlImfor[12];
    pb->data[1] = deviceControlImfor[13];
    pb->data[2] = deviceControlImfor[14];
    pb->data[3] = deviceControlImfor[15];

    pb = &buffer[17];
    pb->boxOffset = CAN_TX_CONFIGURE_OFFSET;

    pb->byteNum = 8;
    pb->data[0] = deviceControlImfor[16];
    pb->data[1] = deviceControlImfor[17];
    pb->data[2] = deviceControlImfor[18];
    pb->data[3] = deviceControlImfor[19];

    pb = &buffer[18];
    pb->boxOffset = CAN_TX_CONFIGURE_OFFSET;

    pb->byteNum = 8;
    pb->data[0] = deviceControlImfor[20];
    pb->data[1] = deviceControlImfor[21];
    pb->data[2] = deviceControlImfor[22];
    pb->data[3] = deviceControlImfor[23];

}

static void CanApp_BootFunction(void) {
    Uint8 a = 0;
    switch(helper.bootState) {
        case NO_BOOT_REQ:{
            a = NO_BOOT_REQ;
            break;
        }
        case DC_BOOT_REQ:{
            a = DC_BOOT_REQ;
            bootData.isNeed = REQURE_FLAG;
            asm(" LB #0x3F7FF6");
            break;
        }
        case PFC_BOOT_REQ:{
            a = PFC_BOOT_REQ;
            break;
        }
        case ALL_BOOT_REQ:{
            a = ALL_BOOT_REQ;
            break;
        }
        default: break;
    }
}















