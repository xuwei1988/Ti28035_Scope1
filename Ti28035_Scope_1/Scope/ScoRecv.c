#include "ScoRecv.h"
#include "assertEx.h"
DEFINE_THIS_MODULE(ScoRecv.c)

static struct {
    ScoRecvFrame *pb;
    Uint16 currPos;
    Uint16 endPos;
}byteRecvPcb;

static void ByteRecvPcb_clear(void) {
    bzero((Uint8 *)&byteRecvPcb, sizeof(byteRecvPcb));
}
static void ScoRecv_add(ScoRecvFrame **root, ScoRecvFrame *pf);

/* 接收到的数据链表表头 ------------------------------------------------------------*/
ScoRecvFrame *scoRecvHead;

void ScoRecv_init(void) {
    /* step1: 初始化内存簇 */
    bzero((Uint8 *)clusterbuffSto, sizeof(clusterbuffSto));
    cluster_PoolInit(clusterbuffSto, sizeof(clusterbuffSto), sizeof(clusterbuffSto[0]));

    /* step2: 初始化接收链表 */
    scoRecvHead = (ScoRecvFrame *)0;

    /* step3: 初始化字节接收控制块 */
    bzero((Uint8 *)&byteRecvPcb, sizeof(byteRecvPcb));
}

/* 此函数用于按字节接收数据的情况， 主要供SCI/UART等串口使用 -------------------------*/
void ScoRecv_byteRecving(Uint8 data) {
    /* step1: 检查是否需要重新开辟缓存 */
    if(!byteRecvPcb.pb) {
        byteRecvPcb.pb = (ScoRecvFrame *)MemPool_get(&clusterPool);
        ASSERT_EX(byteRecvPcb.pb);
    }

    ((Uint8 *)&byteRecvPcb.pb->recvData)[byteRecvPcb.currPos++] = data;

    /* step2: 检查帧头 */
    switch(byteRecvPcb.currPos) {
        case 1: {
            if(data != SCOPE_ADDHI) {
                MemPool_put(&clusterPool, byteRecvPcb.pb);
                ByteRecvPcb_clear();
            }
            return;
        }
        case 2: {
            if(data != SCOPE_ADDLO) {
                MemPool_put(&clusterPool, byteRecvPcb.pb);
                ByteRecvPcb_clear();
            }
            return;
        }
        case 3: {
            byteRecvPcb.endPos = 3 + data;
            return;
        }
    }

    if(byteRecvPcb.currPos == byteRecvPcb.endPos) {
        ScoRecv_add(&scoRecvHead, byteRecvPcb.pb);
        INT_LOCK();
        ByteRecvPcb_clear();
        INT_UNLOCK();
    }
}

/* 此函数用于按块接收数据的情况， 主要供Ethernet/CAN等使用 -----------------------------*/
void ScoRecv_frameRecving(Uint8 *pdata, Uint16 len) {
    ScoRecvFrame *pb = (ScoRecvFrame *)MemPool_get(&clusterPool);
    ASSERT_EX(pb);

    memcpy(&pb->recvData, pdata, len);
    INT_LOCK();
    ScoRecv_add(&scoRecvHead, byteRecvPcb.pb);
    INT_UNLOCK();
}

ScoRecvFrame *ScoRecv_frameGet(void) {
    ScoRecvFrame *pb;
    if(!scoRecvHead)
        return (ScoRecvFrame *)0;
    INT_LOCK();
    pb = scoRecvHead;
    scoRecvHead = pb->pNext;
    INT_UNLOCK();

    return pb;
}


/* 接收数据块插入 --------------------------------------------------------------------*/
static void ScoRecv_add(ScoRecvFrame **root, ScoRecvFrame *pf) {
    while(*root){
        root = &(*root)->pNext;
    }
    *root = pf;
    pf->pNext = (ScoRecvFrame *)0;
}
