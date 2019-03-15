#include "ScoUser.h"
#include "assertEx.h"
#include <String.h>
DEFINE_THIS_MODULE(ScoUser.c)

/* 定义一些全局变量   --------------------------------------------*/
Uint16 scoUserbuff[SCOPE_MAX_CHN];

/* 初始化函数  --------------------------------------------------*/
void ScoUser_init(void) {
    /* step0: 初始化环回数组及数组控制块*/
    ScoArray_init();

    /* step1: 初始化各内存控制块 */
    ScoReal_poolInit(scoRealBuffSto, sizeof(scoRealBuffSto), sizeof(scoRealBuffSto[0]));
    ScoEvt_poolInit(scoEvtBuffSto, sizeof(scoEvtBuffSto), sizeof(scoEvtBuffSto[0]));
    ScoEvtPCB_poolInit(scoEvtPcbBuffSto, sizeof(scoEvtPcbBuffSto), sizeof(scoEvtPcbBuffSto[0]));
}


/* 故障使能函数 ---------------------------------------------------*/
ScoRes ScoUser_eventTriger(Uint16 forwardCyc,                 //前相记录周期数
                         Uint16 afterwardCyc,               //后相记录周期数
                         char *description,                 //事件描述
                         Evtlevel level,                    //事件等级
                         Uint32 chnIDs) {                   //通道ID

    /* step1: 首先核实参数的合法性*/
    ASSERT_EX(forwardCyc + afterwardCyc <= SCOPE_CHN_CYC);
    ASSERT_EX(strlen(description) < SCOPE_EVTDESCRIPTION_NUM - 1);

    /* step1.1: 检测chnIDS是否为0（表示没有选择任何通道）*/
    if(chnIDs == 0)
        return resOk;

    /* step2: 获取一块事件控制块，并填写相关信息 */
    ScoEvtPCBFrame *ptemp = (ScoEvtPCBFrame *)MemPool_get(&scoEvtPcbPool);
    if(!ptemp)
        return resMemFull;
    /* 写各域的信息*/
    ptemp->pcb.chnPoints = (forwardCyc + afterwardCyc) * SCOPE_CYC_NUM;
    strcpy(ptemp->pcb.decription, description);
    ptemp->pcb.eventLevel = level;
    ptemp->pcb.isPCBImforSended = false;
    ptemp->pcb.leftLoop = afterwardCyc * SCOPE_EVTPRO_FRQ;
    ptemp->pcb.pToData = (ScoEvtFrame *)0;
    ptemp->pcb.selectedChn = chnIDs;

    ScoEvtPCB_add(&scpAryPcb.pToPcb, ptemp);
    return resOk;
}

/* 故障刷新用户接口 -----------------------------------------------------*/
void ScoUser_eventPro(void) {
    ScoEvtPCB_refresh(&scpAryPcb.pToPcb);
}

































