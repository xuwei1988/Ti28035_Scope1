#include "ScoEvtPcb.h"
#include "assertEx.h"

/* ���建�漰���ƿ� --------------------------------------------------------*/
ScoEvtPCBFrame scoEvtPcbBuffSto[SCOPE_EVTPCB_NUM];
MemPool scoEvtPcbPool;
ScoEvtPCBFrame *scoEvtPCBHead = 0;
//���ڱ��浥��ͨ���Ĺ�������
Uint16 scoEvtChnLog[SCOPE_CHN_PTS];

/* ���ݿ��ʼ�����������ʼ�������е��� --------------------------------------*/
void ScoEvtPCB_poolInit(void *poolSto, Uint16 poolSize, Uint16 blockSize) {
    scoEvtPCBHead = 0;
    bzero((Uint8 *)scoEvtPcbBuffSto, sizeof(scoEvtPcbBuffSto));
    MemPool_init(&scoEvtPcbPool, poolSto, poolSize, blockSize);
}

/* ���ݿ���� --------------------------------------------------------------*/
void ScoEvtPCB_add(ScoEvtPCBFrame **root, ScoEvtPCBFrame *pf) {
    while(*root){
        root = &(*root)->pNext;
    }
    *root = pf;
    pf->pNext = (ScoEvtPCBFrame *)0;
}

/* ���ݿ鵯�� --------------------------------------------------------------*/
ScoEvtPCBFrame *ScoEvtPCB_pop(ScoEvtPCBFrame **root) {
    ScoEvtPCBFrame *ptemp = (ScoEvtPCBFrame *)0;
    if(*root){
        ptemp = *root;
        *root = (*root)->pNext;
    }
    return ptemp;
}



/* ˢ��scoEvtPCB�������������ĳ��Ԫ���Ѿ������ȡ���ݵ�ʵ�ʣ���Ҫ�ر��� */
extern void ScoArray_channelLog(Uint8 chnID, Uint16 len);

void ScoEvtPCB_refresh(ScoEvtPCBFrame **root) {
    ScoEvtPCBFrame *ptemp;

    while(*root) {
        ptemp = *root;
        /* step1:����ÿ��ƿ��Ѿ��ﵽ����Ҫ��ȡ���ݵ����� */
        if(ptemp->pcb.leftLoop-- == 0) {
            *root = ptemp->pNext;

            /* step2: ������ͨ��ѡȡ��������ұ���ͨ������ */
            Uint32 id = 0;
            for(id = 0; id < SCOPE_MAX_CHN; id++) {
                if(ptemp->pcb.selectedChn & (1 << (id))) {
                    ScoArray_channelLog(id, ptemp->pcb.chnPoints);
                    /* step3: ��ͨ������ת�������������� */
                    ScoEvt_chainFrame(&ptemp->pcb.pToData, id, scoEvtChnLog, ptemp->pcb.chnPoints);
                }
            }

            /* step3: ��ptempת��ֵ�������¼��������� */
            ScoEvtPCB_add(&scoEvtPCBHead, ptemp);

        }else{
            root = &ptemp->pNext;
            ptemp = *root;
        }

    }
}





