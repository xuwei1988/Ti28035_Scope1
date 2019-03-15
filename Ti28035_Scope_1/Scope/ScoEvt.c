#include "scoEvt.h"
#include "assertEx.h"

/* 定义缓存及控制块 --------------------------------------------------------*/
ScoEvtFrame scoEvtBuffSto[SCOPE_EVTDATA_FNUM];
MemPool scoEvtPool;


void ScoEvt_poolInit(void *poolSto, Uint16 poolSize, Uint16 blockSize) {
    bzero((Uint8 *)scoEvtBuffSto, sizeof(scoEvtBuffSto));
    MemPool_init(&scoEvtPool, poolSto, poolSize, blockSize);
}

void ScoEvt_add(ScoEvtFrame **root, ScoEvtFrame *pf) {
    while(*root){
        root = &(*root)->pNext;
    }
    *root = pf;
    pf->pNext = (ScoEvtFrame *)0;
}

ScoEvtFrame *ScoEvt_pop(ScoEvtFrame **root) {
    ScoEvtFrame *ptemp = (ScoEvtFrame *)0;
    if(*root){
        ptemp = *root;
        *root = (*root)->pNext;
    }
    return ptemp;
}

ScoRes ScoEvt_chainFrame(ScoEvtFrame **root, Uint8 chnID, Uint16 *pdata, Uint16 len) {
    ScoEvtFrame *ptemp = 0;
    Uint16 pos = 0;

    /* step1: 将pdata数组的数据分割并保存至链表 */
    while(pos < len) {
        ptemp = (ScoEvtFrame *)MemPool_get(&scoEvtPool);
        if(!ptemp)
            return resMemFull;

        ptemp->eventData.chnID = chnID;
        /* step2: 判断待COPY的数据的长度 */
        if(len - pos >= SCOPE_EVTDATA_NUM) {
            ptemp->eventData.points = SCOPE_EVTDATA_NUM;
            memcpy(ptemp->eventData.data, &pdata[pos], ptemp->eventData.points);

        }else{
            ptemp->eventData.points = len - pos;
            memcpy(ptemp->eventData.data, &pdata[pos], ptemp->eventData.points);
        }
        pos += ptemp->eventData.points;

        ScoEvt_add(root, ptemp);
    }
    return resOk;
}


