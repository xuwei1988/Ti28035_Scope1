#ifndef SCOPE_SCOEVTPCB_H_
#define SCOPE_SCOEVTPCB_H_
#include "scopeport.h"
#include "scopetypes.h"
#include "ScoEvt.h"

typedef struct ScoEvtPCBTag{
    /* һ�������������ݼ�¼��ʱ���õ� */
    Uint16 leftLoop;            //��ʣ��δ�����õĴ���
    Uint32 selectedChn;         //��ѡȡ��ͨ��

    /* �����������ݷ��͵�ʱ���õ� */
    bool isPCBImforSended;      //PCB������Ϣ�Ƿ��Ѿ��ϴ���PCB���������һЩ�¼���Ϣ����Щ��ϢҲ��Ҫ�ϴ���
    Evtlevel eventLevel;        //�¼��ȼ�
    Uint16 chnPoints;           //ÿͨ���ϴ�����
    char decription[SCOPE_EVTDESCRIPTION_NUM];
                                //���������¼����ַ���
    ScoEvtFrame *pToData;        //ָ����Ч���ݵ�ָ��
}ScoEvtPCB;

typedef struct ScoEvtPCBSendTag{
    Uint32 selectedChn;         //��ѡȡ��ͨ��
    Evtlevel eventLevel;        //�¼��ȼ�
    Uint16 chnPoints;           //ÿͨ���ϴ�����
    char decription[SCOPE_EVTDESCRIPTION_NUM];
                                //���������¼����ַ���
}ScoEvtPCBSend;


typedef struct ScoEvtPCBFrameTag {
    struct ScoEvtPCBFrameTag *pNext;
    ScoEvtPCB pcb;
}ScoEvtPCBFrame;

/* ���建�漰���ƿ� --------------------------------------------------------*/
extern ScoEvtPCBFrame scoEvtPcbBuffSto[SCOPE_EVTPCB_NUM];
extern MemPool scoEvtPcbPool;
extern ScoEvtPCBFrame *scoEvtPCBHead;       //����ָʾ��ǰ�Ƿ��д����͵��¼�����
extern Uint16 scoEvtChnLog[SCOPE_CHN_PTS];  //���ڱ��浥��ͨ���Ĺ�������

void ScoEvtPCB_poolInit(void *poolSto, Uint16 poolSize, Uint16 blockSize);
void ScoEvtPCB_add(ScoEvtPCBFrame **root, ScoEvtPCBFrame *pf);
ScoEvtPCBFrame *ScoEvtPCB_pop(ScoEvtPCBFrame **root);
/* �¼����ƿ�ˢ�º��������ڼ�ʱ�������������͵��¼�*/
void ScoEvtPCB_refresh(ScoEvtPCBFrame **root);

#endif /* SCOPE_SCOEVTPCB_H_ */
