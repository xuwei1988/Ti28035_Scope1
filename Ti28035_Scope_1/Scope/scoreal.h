/* ���ļ������ṩ����ʵʱ�ϴ����ݵĽӿ� */
#ifndef SCOPE_SCOREAL_H_
#define SCOPE_SCOREAL_H_

#include "scopetypes.h"

/* �������ڱ�����Ҫʵʱ�ϴ������ݽṹ --------------------------------------------*/

typedef struct ScoRealTag{
    Uint8 chnNum;                       //��ǰ�ϴ���ͨ����
    Uint8 chnPoints;                    //ÿ��ͨ����Frame�еĵ��������chnNum���Լ������Ч���ݳ��ȣ�
    Uint16 data[SCOPE_REALDATA_NUM];    //���ϴ�����Ч����
}ScoReal;

typedef struct ScoRealFrameTag{
    struct ScoRealFrameTag *pNext;
    ScoReal realData;
}ScoRealFrame;


/* ���建�漰���ƿ� --------------------------------------------------------*/
extern ScoRealFrame scoRealBuffSto[SCOPE_REALDATA_FNUM];
extern MemPool scoRealPool;
extern ScoRealFrame *scoRealHead;

void ScoReal_poolInit(void *poolSto, Uint16 poolSize, Uint16 blockSize);
void ScoReal_add(ScoRealFrame **root, ScoRealFrame *pf);
ScoRealFrame *ScoReal_pop(ScoRealFrame **root);
void ScoReal_fresh(Uint16 *pdata, Uint16 used);



#endif /* SCOPE_SCOREAL_H_ */
