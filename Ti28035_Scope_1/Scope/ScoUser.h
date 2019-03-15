/* 本文件主要提供给用户在应用层直接调用的接口 */
#ifndef SCOPE_SCOUSER_H_
#define SCOPE_SCOUSER_H_

#include "scopeport.h"
#include "scoreal.h"
#include "ScoEvt.h"
#include "ScoEvtPcb.h"
#include "ScoArray.h"
#include "scopetypes.h"

extern Uint16 scoUserbuff[SCOPE_MAX_CHN];

/* 初始化函数  --------------------------------------------------*/
void ScoUser_init(void);
/* 故障使能函数 ---------------------------------------------------*/
ScoRes ScoUser_eventTriger(Uint16 forwardCyc,                  //前相记录周期数
                         Uint16 afterwardCyc,                  //后相记录周期数
                         char *description,                    //事件描述
                         Evtlevel level,                       //事件等级
                         Uint32 chnIDs);                       //通道ID

/* 故障刷新用户接口 -----------------------------------------------------*/
void ScoUser_eventPro(void);



#endif /* SCOPE_SCOUSER_H_ */
