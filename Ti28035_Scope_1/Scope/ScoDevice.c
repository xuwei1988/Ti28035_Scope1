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

    /* step1: ������ͨ���Ƿ�������ȷ */
    if(scoDevice.channelImfor.number != SCOPE_MAX_CHN) {
        for(;;);
    }

    /* step2: ����¼��б��Ƿ�������ȷ*/
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
    /* step1: ���»��λ������� */
    ScoArray_refresh(scoUserbuff, sizeof(scoUserbuff));

    /* step2: ���ݵ�ǰ״̬���ж��Ƿ���Ҫ����ʵʱ�ϴ����� */
    if(scoDevice.state == ScoRunning) {
        /* step3: �����ϴ�Ƶ�� */
        static Uint8 interval = 0;
        if(interval++ % scoDevice.updateInterval)
            return;
        else
            interval = 1;

        /* step4: ����ͨ�� */
        Uint16 data[SCOPE_MAX_CHN];
        Uint16 used = 0, i;

        for(i = 0; i < SCOPE_MAX_CHN; i++){
            if(scoDevice.channelImfor.channels[i].isSelected == true) {
                data[used++] = scoUserbuff[i];
            }
        }

        /* step5: �����ݼ�������ͻ��� */
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


/* 2018.04.03 ������ݷ��ͺͽ��յ��ⲿ�ӿ� ------------------------------*/
Uint16 SciDevice_getBaseImfor(Uint8 *dst) {
    Uint16 len = 1;

    dst[len++] = frameBasic;
    dst[len++] = ((scoDevice.cycPoints & 0xFF00) >> 8);
    dst[len++] =  (scoDevice.cycPoints & 0x00FF);
    dst[len++] = scoDevice.channelImfor.number;
    dst[len++] = scoDevice.events.used;

    /* �û�������Ϣ���ϴ� */


    dst[len++] = SCOPE_CHN_CYC;         //ѭ����������������
    dst[len++] = ((SCOPE_EVTPRO_FRQ & 0xFF00) >> 8);         //ѭ����������������
    dst[len++] = (SCOPE_EVTPRO_FRQ & 0x00FF);

    dst[len++] = ((SCOPE_REALDATA_NUM & 0xFF00) >> 8);         //ѭ����������������
    dst[len++] = (SCOPE_REALDATA_NUM & 0x00FF);
    dst[len++] = scoRealPool.nTot;      //ʵʱ�ϴ�ģ����

    dst[len++] = ((SCOPE_EVTDATA_NUM & 0xFF00) >> 8);         //ѭ����������������
    dst[len++] = (SCOPE_EVTDATA_NUM & 0x00FF);
    dst[len++] = scoEvtPool.nTot;       //�¼��ϴ�ģ����

    dst[len++] = scoEvtPcbPool.nTot;    //�¼����ƿ�����

    dst[0] = len - 1;
    return len;
}


/* ��ȡָ��ͨ��ID������ ------------------------------------------------*/
Uint16 SciDevice_getChannelID(Uint8 *dst, Uint8 id) {
    const char *src = scoDevice.channelImfor.channels[id].name;
    strcpy((char *)(dst + 2), src);
    dst[1] = frameName;
    dst[0] = strlen(src) + 1;
    return dst[0] + 1;
}

/* ��ȡָ���¼�ID�������Ϣ --------------------------------------------*/
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

/* ��ȡ��ǰʵʱ��Ϣ����ͨѶ�����в�����ʾ�����ݺ͹������ݣ�---------------*/
Uint16 SciDevice_getRunningData(Uint8 *dst) {
    /* step1: ����ʵʱ�������ȵ�ԭ��ɨ�����������ͷ */
    /* 1��scoRealHead
     * 2��scoEvtPCBHead
     */

    Uint16 len = 1;

    /* step2: ��������ȼ����ߵ�ʵʱ���� */
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

        /* step2.1 ��pbָ����ڴ淵�� */
        MemPool_put(&scoRealPool, pb);

        return len;
    }

    /* step3: ������¼����� */
    if(scoEvtPCBHead) {
//        ScoEvtPCBFrame *pb = ScoEvtPCB_pop(&scoEvtPCBHead);
        ScoEvtPCBFrame *pb = scoEvtPCBHead;
        /* step3.1: ��⵱ǰPCB���ƿ��Ƿ��Ѿ��ϴ���������Ϣ */
        if(pb->pcb.isPCBImforSended == false) {
            pb->pcb.isPCBImforSended = true;

            dst[len++] = frameEvtDiscript;
            /* step3.1.1 д��ͨ����Ϣ */
            dst[len++] = ((pb->pcb.selectedChn & 0xFF000000) >> 24);
            dst[len++] = ((pb->pcb.selectedChn & 0x00FF0000) >> 16);
            dst[len++] = ((pb->pcb.selectedChn & 0x0000FF00) >> 8);
            dst[len++] =  (pb->pcb.selectedChn & 0x000000FF);

            /* step3.1.2 д���¼��ȼ� */
            dst[len++] = pb->pcb.eventLevel;

            /* step3.1.3 д�뵥ͨ���ϴ����� */
            dst[len++] = ((pb->pcb.chnPoints & 0xFF00) >> 8);
            dst[len++] = (pb->pcb.chnPoints & 0x00FF);

            /* step3.1.4 д��������Ϣ */
            strcpy((char *)&dst[len], pb->pcb.decription);
            len += strlen(pb->pcb.decription);

            dst[0] = len - 1;

            return len;
        }else {         /* step3.2: �����ǰPCB���ƿ���Ϣ�Ѿ��ϴ�*/
            /* step3.2.1 ����Ƿ���ͨ������δ���� */
            if(pb->pcb.pToData) {
                ScoEvtFrame *pEvt = ScoEvt_pop(&pb->pcb.pToData);

                dst[len++] = frameEvtData;

                /* step3.2.2 д�뵱ǰͨ��ID */
                dst[len++] = pEvt->eventData.chnID;

                /* step3.2.3 д�뵱ǰ֡�������ݳ��� */
                dst[len++] = pEvt->eventData.points;

                /* step3.2.4 д��ͨ���������� */
//                int i = 0;
//                for(i = 0; i < pEvt->eventData.points; i++) {
//                    dst[len++] = ((pEvt->eventData.data[i] & 0xFF00) >> 8);
//                    dst[len++] =  (pEvt->eventData.data[i] & 0x00FF);
//                }
                memcpy(&dst[len], pEvt->eventData.data, pEvt->eventData.points);
                len += (pEvt->eventData.points * 2);


                dst[0] = len - 1;
                /* step3.2.5�����ڴ�� */
                MemPool_put(&scoEvtPool, pEvt);

                /* step3.2.6 !!!�����ǰ�¼�������Ϣ�Ѿ�ȫ����������Ҫ����PCB�ڴ��*/
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
































