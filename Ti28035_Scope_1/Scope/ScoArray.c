#include <String.h>
#include "ScoArray.h"
#include "scopeport.h"
#include "assertEx.h"
DEFINE_THIS_MODULE(ScoArray.c)

/* 用于实时保存数据的全局数组 -------------------------------------------------*/
Uint16 scopeArray[SCOPE_MAX_CHN][SCOPE_CHN_PTS];

/* 用于操作全局数组的控制块 ---------------------------------------------------*/
ScoArrayPCB scpAryPcb;

/* 初始化函数，主要用于将内存置为0 --------------------------------------------*/
void ScoArray_init(void) {
    bzero((Uint8 *)&scpAryPcb, sizeof(ScoArrayPCB));
    scpAryPcb.isFulled = false;
    scpAryPcb.curPos = 0;
    scpAryPcb.pToPcb = (ScoEvtPCBFrame *)0;

    int i;
    for(i = 0; i < SCOPE_MAX_CHN; i++) {
        bzero((Uint8 *)scopeArray[i], sizeof(scopeArray[i]));
    }
}

Uint16 ScoArray_refresh(Uint16 *pdata, Uint8 len) {
    /* step1: 先判断传入的数据长度是否合法 */
    if(len != SCOPE_MAX_CHN) {
        return resRefreshErr;
    }

    /* step2: 为每个通道更新数据*/
    int i;
    for(i = 0; i < SCOPE_MAX_CHN; i++){
        scopeArray[i][scpAryPcb.curPos] = *(pdata + i);
    }

    /* step3: 更新数据并且检测是否需要环回 */
    if(++scpAryPcb.curPos == SCOPE_CHN_PTS){
        scpAryPcb.curPos = 0;
        scpAryPcb.isFulled = true;
    }

    /* step4: 检测是否有事件发生并检测是否在此刻需要保存 */


    return 0;
}


void ScoArray_channelLog(Uint8 chnID, Uint16 len) {
    /* step1 先判断截取长度是否超限 */
    ASSERT_EX(len <= SCOPE_CHN_PTS);

    /* step2 拷贝相应通道数据 */
    /* 为了防止数据回绕，应当分两部分COPY */
    Uint16 part1 = 0, part2 = 0;
    if(scpAryPcb.curPos < len) {
        part1 = len - scpAryPcb.curPos;
        part2 = scpAryPcb.curPos;

        memcpy(scoEvtChnLog, &(scopeArray[chnID][SCOPE_CHN_PTS - part1]), part1);
        memcpy(&scoEvtChnLog[part1], &(scopeArray[chnID][0]), part2);
    }else {
        memcpy(scoEvtChnLog, &(scopeArray[chnID][scpAryPcb.curPos - len]), len);
    }
}























