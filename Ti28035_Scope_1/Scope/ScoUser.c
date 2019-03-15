#include "ScoUser.h"
#include "assertEx.h"
#include <String.h>
DEFINE_THIS_MODULE(ScoUser.c)

/* ����һЩȫ�ֱ���   --------------------------------------------*/
Uint16 scoUserbuff[SCOPE_MAX_CHN];

/* ��ʼ������  --------------------------------------------------*/
void ScoUser_init(void) {
    /* step0: ��ʼ���������鼰������ƿ�*/
    ScoArray_init();

    /* step1: ��ʼ�����ڴ���ƿ� */
    ScoReal_poolInit(scoRealBuffSto, sizeof(scoRealBuffSto), sizeof(scoRealBuffSto[0]));
    ScoEvt_poolInit(scoEvtBuffSto, sizeof(scoEvtBuffSto), sizeof(scoEvtBuffSto[0]));
    ScoEvtPCB_poolInit(scoEvtPcbBuffSto, sizeof(scoEvtPcbBuffSto), sizeof(scoEvtPcbBuffSto[0]));
}


/* ����ʹ�ܺ��� ---------------------------------------------------*/
ScoRes ScoUser_eventTriger(Uint16 forwardCyc,                 //ǰ���¼������
                         Uint16 afterwardCyc,               //�����¼������
                         char *description,                 //�¼�����
                         Evtlevel level,                    //�¼��ȼ�
                         Uint32 chnIDs) {                   //ͨ��ID

    /* step1: ���Ⱥ�ʵ�����ĺϷ���*/
    ASSERT_EX(forwardCyc + afterwardCyc <= SCOPE_CHN_CYC);
    ASSERT_EX(strlen(description) < SCOPE_EVTDESCRIPTION_NUM - 1);

    /* step1.1: ���chnIDS�Ƿ�Ϊ0����ʾû��ѡ���κ�ͨ����*/
    if(chnIDs == 0)
        return resOk;

    /* step2: ��ȡһ���¼����ƿ飬����д�����Ϣ */
    ScoEvtPCBFrame *ptemp = (ScoEvtPCBFrame *)MemPool_get(&scoEvtPcbPool);
    if(!ptemp)
        return resMemFull;
    /* д�������Ϣ*/
    ptemp->pcb.chnPoints = (forwardCyc + afterwardCyc) * SCOPE_CYC_NUM;
    strcpy(ptemp->pcb.decription, description);
    ptemp->pcb.eventLevel = level;
    ptemp->pcb.isPCBImforSended = false;
    ptemp->pcb.leftLoop = afterwardCyc * SCOPE_EVTPRO_FRQ;
    ptemp->pcb.pToData = (ScoEvtFrame *)0;
    ptemp->pcb.selectedChn = chnIDs;

    ScoEvtPCB_add(&scpAryPcb.pToPcb, ptemp);
    return resOk;
}

/* ����ˢ���û��ӿ� -----------------------------------------------------*/
void ScoUser_eventPro(void) {
    ScoEvtPCB_refresh(&scpAryPcb.pToPcb);
}

































