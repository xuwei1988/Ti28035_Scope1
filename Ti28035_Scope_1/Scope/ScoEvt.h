/* 此文件用于提供操作上传事件发生时、通道采样数据的接口 */
#ifndef SCOPE_SCOEVT_H_
#define SCOPE_SCOEVT_H_

#include "scopetypes.h"

/* 定义用于保存需要上传的数据结构 --------------------------------------------*/

typedef struct ScoEvtTag{
    Uint8 chnID;                        //通道ID号
    Uint8 points;                       //有效数据长度（采样点数）
    Uint16 data[SCOPE_EVTDATA_NUM];     //待上传的有效数据
}ScoEvt;

typedef struct ScoEvtFrameTag{
    struct ScoEvtFrameTag *pNext;
    ScoEvt eventData;
}ScoEvtFrame;

/* 定义缓存及控制块 --------------------------------------------------------*/
extern ScoEvtFrame scoEvtBuffSto[SCOPE_EVTDATA_FNUM];
extern MemPool scoEvtPool;

void ScoEvt_poolInit(void *poolSto, Uint16 poolSize, Uint16 blockSize);
void ScoEvt_add(ScoEvtFrame **root, ScoEvtFrame *pf);
ScoEvtFrame *ScoEvt_pop(ScoEvtFrame **root);
ScoRes ScoEvt_chainFrame(ScoEvtFrame **root, Uint8 chnID, Uint16 *pdata, Uint16 len);


#endif /* SCOPE_SCOEVT_H_ */
