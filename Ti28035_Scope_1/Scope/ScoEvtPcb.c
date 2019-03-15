#include "ScoEvtPcb.h"
#include "assertEx.h"

/* 定义缓存及控制块 --------------------------------------------------------*/
ScoEvtPCBFrame scoEvtPcbBuffSto[SCOPE_EVTPCB_NUM];
MemPool scoEvtPcbPool;
ScoEvtPCBFrame *scoEvtPCBHead = 0;
//用于保存单个通道的故障数据
Uint16 scoEvtChnLog[SCOPE_CHN_PTS];

/* 数据块初始化，在整体初始化函数中调用 --------------------------------------*/
void ScoEvtPCB_poolInit(void *poolSto, Uint16 poolSize, Uint16 blockSize) {
    scoEvtPCBHead = 0;
    bzero((Uint8 *)scoEvtPcbBuffSto, sizeof(scoEvtPcbBuffSto));
    MemPool_init(&scoEvtPcbPool, poolSto, poolSize, blockSize);
}

/* 数据块插入 --------------------------------------------------------------*/
void ScoEvtPCB_add(ScoEvtPCBFrame **root, ScoEvtPCBFrame *pf) {
    while(*root){
        root = &(*root)->pNext;
    }
    *root = pf;
    pf->pNext = (ScoEvtPCBFrame *)0;
}

/* 数据块弹出 --------------------------------------------------------------*/
ScoEvtPCBFrame *ScoEvtPCB_pop(ScoEvtPCBFrame **root) {
    ScoEvtPCBFrame *ptemp = (ScoEvtPCBFrame *)0;
    if(*root){
        ptemp = *root;
        *root = (*root)->pNext;
    }
    return ptemp;
}



/* 刷新scoEvtPCB链表，如果链表中某个元素已经到达存取数据的实际，需要特别处理 */
extern void ScoArray_channelLog(Uint8 chnID, Uint16 len);

void ScoEvtPCB_refresh(ScoEvtPCBFrame **root) {
    ScoEvtPCBFrame *ptemp;

    while(*root) {
        ptemp = *root;
        /* step1:如果该控制块已经达到了需要存取数据的条件 */
        if(ptemp->pcb.leftLoop-- == 0) {
            *root = ptemp->pNext;

            /* step2: 逐个检查通道选取情况，并且保存通道数据 */
            Uint32 id = 0;
            for(id = 0; id < SCOPE_MAX_CHN; id++) {
                if(ptemp->pcb.selectedChn & (1 << (id))) {
                    ScoArray_channelLog(id, ptemp->pcb.chnPoints);
                    /* step3: 将通道数据转存至待发送链表 */
                    ScoEvt_chainFrame(&ptemp->pcb.pToData, id, scoEvtChnLog, ptemp->pcb.chnPoints);
                }
            }

            /* step3: 将ptemp转移值待发送事件数据链表 */
            ScoEvtPCB_add(&scoEvtPCBHead, ptemp);

        }else{
            root = &ptemp->pNext;
            ptemp = *root;
        }

    }
}





