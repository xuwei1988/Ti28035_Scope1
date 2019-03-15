/* 此文件提供用户更新所有实时数据的接口 */
#ifndef SCOPE_SCOARRAY_H_
#define SCOPE_SCOARRAY_H_

#include "scopetypes.h"
#include "ScoEvtPcb.h"

typedef struct ScoArrayPCBTag{
    bool isFulled;                      //采样数组是否被填充完毕（数组被循环填充）
    Uint16 curPos;                      //当前操作的位置

    ScoEvtPCBFrame *pToPcb;
}ScoArrayPCB;


/* 用于操作全局数组的控制块 ---------------------------------------------------*/
extern ScoArrayPCB scpAryPcb;

void ScoArray_init(void);
Uint16 ScoArray_refresh(Uint16 *pdata, Uint8 len);


#endif /* SCOPE_SCOARRAY_H_ */
