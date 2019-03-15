#include "scoreal.h"
#include "assertEx.h"
DEFINE_THIS_MODULE(scoreal.c)

/* 定义缓存及控制块 --------------------------------------------------------*/
ScoRealFrame scoRealBuffSto[SCOPE_REALDATA_FNUM];
ScoRealFrame *scoRealHead;
MemPool scoRealPool;

/* 用来接收实时数据的函数实现 -----------------------------------------------*/
static struct{
    ScoRealFrame *pb;
    Uint16 maxUse;
    Uint16 pos;

    Uint16 currLen;
}control = {0, 30, 0};

/* 数据块初始化，在整体初始化函数中调用 --------------------------------------*/
void ScoReal_poolInit(void *poolSto, Uint16 poolSize, Uint16 blockSize) {
    control.pb = 0;
    control.maxUse = 30;
    control.pos = 0;

    scoRealHead = 0;
    bzero((Uint8 *)scoRealBuffSto, sizeof(scoRealBuffSto));
    MemPool_init(&scoRealPool, poolSto, poolSize, blockSize);
}

/* 数据块插入 --------------------------------------------------------------*/
void ScoReal_add(ScoRealFrame **root, ScoRealFrame *pf) {
    while(*root){
        root = &(*root)->pNext;
    }
    *root = pf;
    pf->pNext = (ScoRealFrame *)0;
}

/* 数据块弹出 --------------------------------------------------------------*/
ScoRealFrame *ScoReal_pop(ScoRealFrame **root) {
    ScoRealFrame *ptemp = (ScoRealFrame *)0;
    if(*root){
        ptemp = *root;
        *root = (*root)->pNext;
    }
    return ptemp;
}




void ScoReal_fresh(Uint16 *pdata, Uint16 used) {
    if(control.pb == (ScoRealFrame *)0) {
        control.pb = (ScoRealFrame *)MemPool_get(&scoRealPool);
        ASSERT_EX(control.pb);
        control.pos = 0;
        control.pb->realData.chnNum = used;
        control.pb->realData.chnPoints = 0;
    }

    /* 如果剩余容量还足够存放本次数据 --*/
    if(control.pos + used <= control.maxUse) {
        int i = 0;
        for(i = 0; i < used; i++) {
            control.pb->realData.data[control.pos + i] = pdata[i];
        }

        control.pos += used;
        control.pb->realData.chnPoints++;
    }

    /* 如果剩余容量不足下一次存储，直接挂到链表，准备传输 */
    if((control.pos + used) > control.maxUse) {
        ScoReal_add(&scoRealHead, control.pb);
        control.pb = (ScoRealFrame *)0;
    }
}








