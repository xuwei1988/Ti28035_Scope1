#ifndef SCOPE_SCORECV_H_
#define SCOPE_SCORECV_H_

#include "scopetypes.h"

#define SCORECV_MAX_LEN     30

/* 定义用于接收数据的结构体 ------------------------------------------- */
typedef struct ScoRecvTag {
    Uint8 addHi;
    Uint8 addLo;
    Uint8 totLen;
    Uint8 data[SCORECV_MAX_LEN];
}ScoRecv;

typedef struct {
    Uint8 totLen;
    Uint8 type;
    Uint8 data[SCORECV_MAX_LEN - 1];
}ScoRecvBlk;


typedef struct ScoRecvFrameTag{
    struct ScoRecvFrameTag *pNext;
    ScoRecv recvData;
}ScoRecvFrame;

void ScoRecv_init(void);
void ScoRecv_byteRecving(Uint8 data);
void ScoRecv_frameRecving(Uint8 *pdata, Uint16 len);
ScoRecvFrame *ScoRecv_frameGet(void);

extern ScoRecvFrame *scoRecvHead;


#endif /* SCOPE_SCORECV_H_ */
