#include "ScoDevice.h"
#include "ScoUser.h"
#include "ScoRecv.h"
#include "ScoSend.h"
#include <String.h>
#include "assertEx.h"
DEFINE_THIS_MODULE(ScoDevice.c)


enum{
    ask = 0,
    connecting = 1,
    nameloading = 2,
    eventloading = 3,
    selecting = 4,
    disconnecting = 5,

    eventSetting
};


#pragma DATA_SECTION(scoDevice, "GlobalData1");
ScoDevice scoDevice;

static void ScoDevice_disconnectJob(ScoRecvFrame *pb);
static void ScoDevice_nameUploadingJob(ScoRecvFrame *pb);
static void ScoDevice_eventUploadingJob(ScoRecvFrame *pb);
static void ScoDevice_WaitSettingJob(ScoRecvFrame *pb);
static void ScoDevice_RunningJob(ScoRecvFrame *pb);
static void ScoDevice_selfReset(void);
static void ScoDevice_reset(void);
static void ScoDevice_evtSettingPro(ScoRecvBlk *p);

void ScoDevice_init(void) {
    bzero((Uint8 *)&scoDevice, sizeof(scoDevice));
    scoDevice.state = ScoDisconnect;
    scoDevice.updateInterval = 1;
    ScoDevice_setCycPts(SCOPE_CYC_NUM);
}

void ScoDevice_check(void) {
    int i;

    /* step1: 检查采样通道是否设置正确 */
    if(scoDevice.channelImfor.number != SCOPE_MAX_CHN) {
        for(;;);
    }

    /* step2: 检查事件列表是否设置正确*/
    for(i = 0; i < scoDevice.events.used; i++) {
        if(scoDevice.events.list[i].discription == 0)
            for(;;);
    }

}

void ScoDevice_handle(void) {
    ScoRecvFrame *pb;

    pb = ScoRecv_frameGet();
    if(pb) {
        switch(scoDevice.state) {
            case ScoDisconnect:{
                ScoDevice_disconnectJob(pb);
                break;
            }
            case ScoNameUploading:{
                ScoDevice_nameUploadingJob(pb);
                break;
            }
            case ScoEventUploading:{
                ScoDevice_eventUploadingJob(pb);
                break;
            }
            case ScoWaitSetting:{
                ScoDevice_WaitSettingJob(pb);
                break;
            }
            case ScoRunning:{
                ScoDevice_RunningJob(pb);
                break;
            }

        }

        MemPool_put(&clusterPool, pb);
    }
}

static void ScoDevice_disconnectJob(ScoRecvFrame *pb) {
    ScoRecvBlk *p = (ScoRecvBlk *)&pb->recvData.totLen;
    if(p->type == connecting){
        ScoSend_basicImfor();
    }

    if(p->type == ask){
        scoDevice.state = ScoNameUploading;
    }
}

static void ScoDevice_nameUploadingJob(ScoRecvFrame *pb) {
    ScoRecvBlk *p = (ScoRecvBlk *)&pb->recvData.totLen;
    if(p->type == nameloading) {
        ScoSend_channelName(p->data[0]);
    }

    if(p->type == ask) {
        scoDevice.state = ScoEventUploading;
    }
}

static void ScoDevice_eventUploadingJob(ScoRecvFrame *pb) {
    ScoRecvBlk *p = (ScoRecvBlk *)&pb->recvData.totLen;
    if(p->type == eventloading){
        ScoSend_eventImfor(p->data[0]);
    }

    if(p->type == ask) {
        scoDevice.state = ScoWaitSetting;
    }

}

static void ScoDevice_WaitSettingJob(ScoRecvFrame *pb) {
    ScoRecvBlk *p = (ScoRecvBlk *)&pb->recvData.totLen;
    if(p->type == selecting) {
        ScoDevice_selectChannel(p->data, p->totLen - 2);
        ScoDevice_setUpdateInterval(p->data[p->totLen - 2]);
        scoDevice.state = ScoRunning;
    }

    if(p->type == eventSetting) {
        ScoDevice_evtSettingPro(p);
    }

    if(p->type == disconnecting) {
        ScoDevice_reset();
    }
}

static void ScoDevice_RunningJob(ScoRecvFrame *pb) {
    ScoRecvBlk *p = (ScoRecvBlk *)&pb->recvData.totLen;
    if(p->type == disconnecting) {
        ScoDevice_reset();
    }

    if(p->type == eventSetting){
        ScoDevice_evtSettingPro(p);
    }
}

static void ScoDevice_evtSettingPro(ScoRecvBlk *p) {
    Uint8 id = p->data[0];
    Uint8 cycBefor = p->data[1];
    Uint8 cycAfter = p->data[2];
    Uint32 selectedChn = (((Uint32)p->data[3]) << 24) +
            (((Uint32)p->data[4]) << 16) +
            (((Uint32)p->data[5]) << 8) +
            ((Uint32)p->data[6]);

    scoDevice.events.list[id].cycBefore = cycBefor;
    scoDevice.events.list[id].cycAfter = cycAfter;
    scoDevice.events.list[id].selectedChns = selectedChn;
}

static void ScoDevice_reset(void) {
//    ScoDevice_init();

    ScoDevice_selfReset();

    ScoUser_init();
    ScoRecv_init();
    ScoSend_init();
}

static void ScoDevice_selfReset(void) {
        scoDevice.state = ScoDisconnect;
        scoDevice.updateInterval = 1;
        ScoDevice_setCycPts(SCOPE_CYC_NUM);

        int i = 0;
        for(i = 0; i < scoDevice.channelImfor.number; i++) {
            scoDevice.channelImfor.channels[i].isSelected = false;
        }
}


void ScoDevice_setCycPts(Uint16 points) {
    scoDevice.cycPoints = points;
}

void ScoDevice_setName(char *name) {
    ASSERT_EX(scoDevice.channelImfor.number < SCOPE_MAX_CHN);
    scoDevice.channelImfor.channels[scoDevice.channelImfor.number++].name = name;
}

void ScoDevice_setUpdateInterval(Uint8 interval) {
    scoDevice.updateInterval = interval;
}

void ScoDevice_selectChannel(Uint8 *pIds, Uint8 len) {
    REQUIRE(len <= SCOPE_MAX_CHN);
    SciDevice_cleanChannel();

    Uint16 i = 0;
    Uint16 id;
    for(i = 0; i < len; i++) {
        id = *(pIds + i);
        REQUIRE(id < SCOPE_MAX_CHN);
        scoDevice.channelImfor.channels[id].isSelected = true;
    }
}

void ScoDevice_refresh(void) {
    /* step1: 更新环形缓存数组 */
    ScoArray_refresh(scoUserbuff, sizeof(scoUserbuff));

    /* step2: 根据当前状态，判断是否需要更新实时上传数据 */
    if(scoDevice.state == ScoRunning) {
        /* step3: 过滤上传频率 */
        static Uint8 interval = 0;
        if(interval++ % scoDevice.updateInterval)
            return;
        else
            interval = 1;

        /* step4: 过滤通道 */
        Uint16 data[SCOPE_MAX_CHN];
        Uint16 used = 0, i;

        for(i = 0; i < SCOPE_MAX_CHN; i++){
            if(scoDevice.channelImfor.channels[i].isSelected == true) {
                data[used++] = scoUserbuff[i];
            }
        }

        /* step5: 将数据计入待发送缓存 */
        ScoReal_fresh(data, used);
    }
}

void ScoDevice_eventHappen(Uint8 id){
    Event *p = &scoDevice.events.list[id];
    ScoUser_eventTriger(p->cycBefore, p->cycAfter, p->discription, p->level, p->selectedChns);
}


void SciDevice_cleanChannel(void) {
    int i;
    for(i = 0; i < SCOPE_MAX_CHN; i++) {
        scoDevice.channelImfor.channels[i].isSelected = false;
    }
}


/* 2018.04.03 添加数据发送和接收的外部接口 ------------------------------*/
Uint16 SciDevice_getBaseImfor(Uint8 *dst) {
    Uint16 len = 1;

    dst[len++] = frameBasic;
    dst[len++] = ((scoDevice.cycPoints & 0xFF00) >> 8);
    dst[len++] =  (scoDevice.cycPoints & 0x00FF);
    dst[len++] = scoDevice.channelImfor.number;
    dst[len++] = scoDevice.events.used;

    /* 用户配置信息的上传 */


    dst[len++] = SCOPE_CHN_CYC;         //循环采样保留周期数
    dst[len++] = ((SCOPE_EVTPRO_FRQ & 0xFF00) >> 8);         //循环采样保留周期数
    dst[len++] = (SCOPE_EVTPRO_FRQ & 0x00FF);

    dst[len++] = ((SCOPE_REALDATA_NUM & 0xFF00) >> 8);         //循环采样保留周期数
    dst[len++] = (SCOPE_REALDATA_NUM & 0x00FF);
    dst[len++] = scoRealPool.nTot;      //实时上传模块数

    dst[len++] = ((SCOPE_EVTDATA_NUM & 0xFF00) >> 8);         //循环采样保留周期数
    dst[len++] = (SCOPE_EVTDATA_NUM & 0x00FF);
    dst[len++] = scoEvtPool.nTot;       //事件上传模块数

    dst[len++] = scoEvtPcbPool.nTot;    //事件控制块数量

    dst[0] = len - 1;
    return len;
}


/* 获取指定通道ID的名称 ------------------------------------------------*/
Uint16 SciDevice_getChannelID(Uint8 *dst, Uint8 id) {
    const char *src = scoDevice.channelImfor.channels[id].name;
    strcpy((char *)(dst + 2), src);
    dst[1] = frameName;
    dst[0] = strlen(src) + 1;
    return dst[0] + 1;
}

/* 获取指定事件ID的相关信息 --------------------------------------------*/
Uint16  SciDevice_getEventImfor(Uint8 *dst, Uint8 id) {
    Uint16 len = 1;
    const char *src = scoDevice.events.list[id].discription;

    dst[len++] = frameEvent;
    dst[len++] = scoDevice.events.list[id].level;
    dst[len++] = scoDevice.events.list[id].cycBefore;
    dst[len++] = scoDevice.events.list[id].cycAfter;

    dst[len++] = ((scoDevice.events.list[id].selectedChns & 0xFF000000) >> 24);
    dst[len++] = ((scoDevice.events.list[id].selectedChns & 0x00FF0000) >> 16);
    dst[len++] = ((scoDevice.events.list[id].selectedChns & 0x0000FF00) >> 8);
    dst[len++] = ((scoDevice.events.list[id].selectedChns & 0x000000FF) >> 0);
    strcpy((char *)(dst + len), src);

    dst[0] = strlen(src) + len - 1;
    return dst[0] + 1;
}

/* 获取当前实时信息（在通讯过程中产生的示波数据和故障数据）---------------*/
Uint16 SciDevice_getRunningData(Uint8 *dst) {
    /* step1: 按照实时数据优先的原则扫描两个链表表头 */
    /* 1、scoRealHead
     * 2、scoEvtPCBHead
     */

    Uint16 len = 1;

    /* step2: 如果有优先级更高的实时数据 */
    if(scoRealHead){
        ScoRealFrame *pb = ScoReal_pop(&scoRealHead);

        dst[len++] = frameReal;
        dst[len++] = pb->realData.chnNum;
        dst[len++] = pb->realData.chnPoints;

//        int i, j;
//        for(i = 0; i < pb->realData.chnPoints; i++) {
//            for(j = 0; j < pb->realData.chnNum; j++) {
//                dst[len++] = ((pb->realData.data[i*pb->realData.chnNum + j] & 0xFF00) >> 8);
//                dst[len++] = (pb->realData.data[i*pb->realData.chnNum + j] & 0x00FF);
//            }
//        }



        memcpy(&dst[len], pb->realData.data, pb->realData.chnPoints * pb->realData.chnNum);
        len += (pb->realData.chnPoints * pb->realData.chnNum * 2);


        dst[0] = len - 1;

        /* step2.1 将pb指向的内存返回 */
        MemPool_put(&scoRealPool, pb);

        return len;
    }

    /* step3: 如果有事件数据 */
    if(scoEvtPCBHead) {
//        ScoEvtPCBFrame *pb = ScoEvtPCB_pop(&scoEvtPCBHead);
        ScoEvtPCBFrame *pb = scoEvtPCBHead;
        /* step3.1: 检测当前PCB控制块是否已经上传过自身信息 */
        if(pb->pcb.isPCBImforSended == false) {
            pb->pcb.isPCBImforSended = true;

            dst[len++] = frameEvtDiscript;
            /* step3.1.1 写入通道信息 */
            dst[len++] = ((pb->pcb.selectedChn & 0xFF000000) >> 24);
            dst[len++] = ((pb->pcb.selectedChn & 0x00FF0000) >> 16);
            dst[len++] = ((pb->pcb.selectedChn & 0x0000FF00) >> 8);
            dst[len++] =  (pb->pcb.selectedChn & 0x000000FF);

            /* step3.1.2 写入事件等级 */
            dst[len++] = pb->pcb.eventLevel;

            /* step3.1.3 写入单通道上传点数 */
            dst[len++] = ((pb->pcb.chnPoints & 0xFF00) >> 8);
            dst[len++] = (pb->pcb.chnPoints & 0x00FF);

            /* step3.1.4 写入描述信息 */
            strcpy((char *)&dst[len], pb->pcb.decription);
            len += strlen(pb->pcb.decription);

            dst[0] = len - 1;

            return len;
        }else {         /* step3.2: 如果当前PCB控制块信息已经上传*/
            /* step3.2.1 检查是否还有通道数据未传完 */
            if(pb->pcb.pToData) {
                ScoEvtFrame *pEvt = ScoEvt_pop(&pb->pcb.pToData);

                dst[len++] = frameEvtData;

                /* step3.2.2 写入当前通道ID */
                dst[len++] = pEvt->eventData.chnID;

                /* step3.2.3 写入当前帧包含数据长度 */
                dst[len++] = pEvt->eventData.points;

                /* step3.2.4 写入通道采样数据 */
//                int i = 0;
//                for(i = 0; i < pEvt->eventData.points; i++) {
//                    dst[len++] = ((pEvt->eventData.data[i] & 0xFF00) >> 8);
//                    dst[len++] =  (pEvt->eventData.data[i] & 0x00FF);
//                }
                memcpy(&dst[len], pEvt->eventData.data, pEvt->eventData.points);
                len += (pEvt->eventData.points * 2);


                dst[0] = len - 1;
                /* step3.2.5回收内存块 */
                MemPool_put(&scoEvtPool, pEvt);

                /* step3.2.6 !!!如果当前事件所有信息已经全部发出，需要回收PCB内存块*/
                if(pb->pcb.pToData == (ScoEvtFrame *)0){
                    ScoEvtPCB_pop(&scoEvtPCBHead);
                    MemPool_put(&scoEvtPcbPool, pb);
                }

                return len;
            }

        }

    }

    return 0;
}
































