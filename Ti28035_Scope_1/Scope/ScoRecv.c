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

/* ���յ������������ͷ ------------------------------------------------------------*/
ScoRecvFrame *scoRecvHead;

void ScoRecv_init(void) {
    /* step1: ��ʼ���ڴ�� */
    bzero((Uint8 *)clusterbuffSto, sizeof(clusterbuffSto));
    cluster_PoolInit(clusterbuffSto, sizeof(clusterbuffSto), sizeof(clusterbuffSto[0]));

    /* step2: ��ʼ���������� */
    scoRecvHead = (ScoRecvFrame *)0;

    /* step3: ��ʼ���ֽڽ��տ��ƿ� */
    bzero((Uint8 *)&byteRecvPcb, sizeof(byteRecvPcb));
}

/* �˺������ڰ��ֽڽ������ݵ������ ��Ҫ��SCI/UART�ȴ���ʹ�� -------------------------*/
void ScoRecv_byteRecving(Uint8 data) {
    /* step1: ����Ƿ���Ҫ���¿��ٻ��� */
    if(!byteRecvPcb.pb) {
        byteRecvPcb.pb = (ScoRecvFrame *)MemPool_get(&clusterPool);
        ASSERT_EX(byteRecvPcb.pb);
    }

    ((Uint8 *)&byteRecvPcb.pb->recvData)[byteRecvPcb.currPos++] = data;

    /* step2: ���֡ͷ */
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

/* �˺������ڰ���������ݵ������ ��Ҫ��Ethernet/CAN��ʹ�� -----------------------------*/
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


/* �������ݿ���� --------------------------------------------------------------------*/
static void ScoRecv_add(ScoRecvFrame **root, ScoRecvFrame *pf) {
    while(*root){
        root = &(*root)->pNext;
    }
    *root = pf;
    pf->pNext = (ScoRecvFrame *)0;
}
