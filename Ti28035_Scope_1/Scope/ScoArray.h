/* ���ļ��ṩ�û���������ʵʱ���ݵĽӿ� */
#ifndef SCOPE_SCOARRAY_H_
#define SCOPE_SCOARRAY_H_

#include "scopetypes.h"
#include "ScoEvtPcb.h"

typedef struct ScoArrayPCBTag{
    bool isFulled;                      //���������Ƿ������ϣ����鱻ѭ����䣩
    Uint16 curPos;                      //��ǰ������λ��

    ScoEvtPCBFrame *pToPcb;
}ScoArrayPCB;


/* ���ڲ���ȫ������Ŀ��ƿ� ---------------------------------------------------*/
extern ScoArrayPCB scpAryPcb;

void ScoArray_init(void);
Uint16 ScoArray_refresh(Uint16 *pdata, Uint8 len);


#endif /* SCOPE_SCOARRAY_H_ */
