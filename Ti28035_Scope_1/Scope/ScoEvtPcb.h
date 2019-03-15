#ifndef SCOPE_SCOEVTPCB_H_
#define SCOPE_SCOEVTPCB_H_
#include "scopeport.h"
#include "scopetypes.h"
#include "ScoEvt.h"

typedef struct ScoEvtPCBTag{
    /* 一下两个域在数据记录的时候用到 */
    Uint16 leftLoop;            //还剩余未被调用的次数
    Uint32 selectedChn;         //被选取的通道

    /* 以下域在数据发送的时候用到 */
    bool isPCBImforSended;      //PCB自身信息是否已经上传（PCB自身包含了一些事件信息，这些信息也需要上传）
    Evtlevel eventLevel;        //事件等级
    Uint16 chnPoints;           //每通道上传点数
    char decription[SCOPE_EVTDESCRIPTION_NUM];
                                //用于描述事件的字符串
    ScoEvtFrame *pToData;        //指向有效数据的指针
}ScoEvtPCB;

typedef struct ScoEvtPCBSendTag{
    Uint32 selectedChn;         //被选取的通道
    Evtlevel eventLevel;        //事件等级
    Uint16 chnPoints;           //每通道上传点数
    char decription[SCOPE_EVTDESCRIPTION_NUM];
                                //用于描述事件的字符串
}ScoEvtPCBSend;


typedef struct ScoEvtPCBFrameTag {
    struct ScoEvtPCBFrameTag *pNext;
    ScoEvtPCB pcb;
}ScoEvtPCBFrame;

/* 定义缓存及控制块 --------------------------------------------------------*/
extern ScoEvtPCBFrame scoEvtPcbBuffSto[SCOPE_EVTPCB_NUM];
extern MemPool scoEvtPcbPool;
extern ScoEvtPCBFrame *scoEvtPCBHead;       //用于指示当前是否有待发送的事件数据
extern Uint16 scoEvtChnLog[SCOPE_CHN_PTS];  //用于保存单个通道的故障数据

void ScoEvtPCB_poolInit(void *poolSto, Uint16 poolSize, Uint16 blockSize);
void ScoEvtPCB_add(ScoEvtPCBFrame **root, ScoEvtPCBFrame *pf);
ScoEvtPCBFrame *ScoEvtPCB_pop(ScoEvtPCBFrame **root);
/* 事件控制块刷新函数。用于及时触发须立即发送的事件*/
void ScoEvtPCB_refresh(ScoEvtPCBFrame **root);

#endif /* SCOPE_SCOEVTPCB_H_ */
