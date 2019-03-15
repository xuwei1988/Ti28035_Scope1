/* 此文件用于提供操作实时上传数据的接口 */
#ifndef SCOPE_SCOREAL_H_
#define SCOPE_SCOREAL_H_

#include "scopetypes.h"

/* 定义用于保存需要实时上传的数据结构 --------------------------------------------*/

typedef struct ScoRealTag{
    Uint8 chnNum;                       //当前上传总通道数
    Uint8 chnPoints;                    //每个通道在Frame中的点数（配合chnNum可以计算出有效数据长度）
    Uint16 data[SCOPE_REALDATA_NUM];    //待上传的有效数据
}ScoReal;

typedef struct ScoRealFrameTag{
    struct ScoRealFrameTag *pNext;
    ScoReal realData;
}ScoRealFrame;


/* 定义缓存及控制块 --------------------------------------------------------*/
extern ScoRealFrame scoRealBuffSto[SCOPE_REALDATA_FNUM];
extern MemPool scoRealPool;
extern ScoRealFrame *scoRealHead;

void ScoReal_poolInit(void *poolSto, Uint16 poolSize, Uint16 blockSize);
void ScoReal_add(ScoRealFrame **root, ScoRealFrame *pf);
ScoRealFrame *ScoReal_pop(ScoRealFrame **root);
void ScoReal_fresh(Uint16 *pdata, Uint16 used);



#endif /* SCOPE_SCOREAL_H_ */
