#include "ScoUser.h"
#include "ScoDevice.h"
#include "ScoSCI.h"
/*2019.03.15 */
/* Master Branch test*/
/* dev test!*/
/* dev2 test! */

int sin[200] = { 0,9,19,28,38,48,57,67,76,85,95,104,113,122,131,139,
                 148,156,165,173,181,188,196,203,210,217,224,231,237,243,249,254,
                 260,265,269,274,278,282,286,289,292,295,298,300,302,304,305,306,
                 307,307,308,307,307,306,305,304,302,300,298,295,292,289,286,282,
                 278,274,269,265,260,254,249,243,237,231,224,217,210,203,196,188,
                 181,173,165,156,148,139,131,122,113,104,95,85,76,67,57,48,
                 38,28,19,9,0,-9,-19,-28,-38,-48,-57,-67,-76,-85,-95,-104,
                 -113,-122,-131,-139,-148,-156,-165,-173,-181,-188,-196,-203,-210,-217,-224,-231,
                 -237,-243,-249,-254,-260,-265,-269,-274,-278,-282,-286,-289,-292,-295,-298,-300,
                 -302,-304,-305,-306,-307,-307,-308,-307,-307,-306,-305,-304,-302,-300,-298,-295,
                 -292,-289,-286,-282,-278,-274,-269,-265,-260,-254,-249,-243,-237,-231,-224,-217,
                 -210,-203,-196,-188,-181,-173,-165,-156,-148,-139,-131,-122,-113,-104,-95,-85,
                 -76,-67,-57,-48,-38,-28,-19,-9};

static interrupt void cpu_timer0_isr(void){

    ScoSCI_recv();
    ScoSCI_send();

    static int16 cnt = 0;
    static int16 cnt1 = 49;
    static int16 cnt2 = 99;

    if(cnt < 200) {
        scoUserbuff[0] = sin[cnt++];
        scoUserbuff[1] = sin[cnt1++];
        scoUserbuff[2] = sin[cnt2++];
        if(cnt == 200)
            cnt = 0;
        if(cnt1 == 200)
            cnt1 = 0;
        if(cnt2 == 200)
            cnt2 = 0;
    }

    ScoDevice_refresh();



    static Uint32 loop = 0;
    if(loop++ == 30000) {
        loop = 0;
        static int n = 0;
        ScoDevice_eventHappen(n++ % 3);
    }

    static int loop1 = 0;
    if(loop1++ == 9) {
        loop1 = 0;
        ScoUser_eventPro();
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


void main(void) {

    InitSysCtrl();
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;

    /* 这里允许中断 －－*/
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

    EALLOW;
    /* Timer0 中断 －－*/
    PieVectTable.TINT0 = &cpu_timer0_isr;
    EDIS;

    InitCpuTimers();

    /* step1: 初始化各相关模块 */
    ScoUser_init();
    ScoDevice_init();
    ScoSCI_init();


    ConfigCpuTimer(&CpuTimer0, 60, 100);      //100us
    CpuTimer0Regs.TCR.all = 0x4001;

    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    IER |= M_INT1;
    EINT;
    ERTM;

    /* step2: 输入设备基本信息 */
    ScoDevice_setName("A Phrase V");
    ScoDevice_setName("B Phrase V");
    ScoDevice_setName("C Phrase V");

    ScoDevice_setEvent(A_PHRASE_HIGH, CUSTOMER, 1, 1, SCOPE_CHN0);
    ScoDevice_setEvent(B_PHRASE_LOW, WARNING, 1, 1, SCOPE_CHN0|SCOPE_CHN1);
    ScoDevice_setEvent(C_PHRASE_HIGH, ERROR, 1, 1, SCOPE_CHN0|SCOPE_CHN1|SCOPE_CHN2);

    /* step3: 检查设备相关设置是否正确 */
    ScoDevice_check();

    while(1) {
       ScoDevice_handle();
    }

}



































