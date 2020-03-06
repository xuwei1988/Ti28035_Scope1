#include "ScoUser.h"
#include "ScoDevice.h"
#include "ScoSCI.h"

#include "CANBasic.h"
#include "CANApp.h"
#include "DeviceImfor.h"
#include "basic.h"
/*2019.03.15 */

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




Uint16 chargerCfg[DEVICE_SET_MAXID] =
{
 24500,     /* 输出电压给定 */
 24000,     /* 输出电压默认值 */
 26000,     /* 输出电压过压值 */
 26500,     /* 输出电流 */
 27000,     /* 输出电流默认值 */
 4000,      /* 母线电压给定 */
 700,       /* 最大输出功率 */
 27900,     /* 输出电压高点校准 */
 10000,     /* 输出电压低点校准 */
 30000,     /* 输出电流高点校准 */
 10000,     /* 输出电流低点校准 */
 27900,     /* 输出电压给定高点校准 */
 10000,     /* 输出电压给定低点校准 */
 2800,      /* AC电压高点校准 */
 1000,      /* AC电压低点校准 */
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0,
 0
};

Uint16 acVolt[3] =      {2303, 2230, 2410};
Uint16 testData1[3] =   {9, 10, 11};
Uint16 testData2[3] =   {19, 20, 21};
Uint16 dcVolt[3] =      {25500, 24800, 25700};
Uint16 dcVoltSet[3] =   {25500, 26000, 26500};
Uint16 dcVoltClose[3] = {28400, 28000, 28200};
Uint16 dcCurr[3] =      {27100, 26900, 27000};
Uint16 dcCurrMax[3] =   {30000, 31000, 32000};
Uint16 busVolt[3] =     {4000, 4100, 3900};
Uint16 testData3[3] =   {29, 30, 31};
Uint16 pfcTemp[3] =     {80, 79, 81};
Uint16 llcTemp[3] =     {60, 59, 61};
Uint16 transTemp[3] =   {80, 70, 90};
Uint16 testData4[3] =   {39, 40, 41};
Uint16 testData5[3] =   {49, 50, 51};
Uint16 testData6[3] =   {59, 60, 61};

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


    /* CAN Task */
    static Uint16 CANcnt = 0;
    if(CANcnt++ >= 200) {
        CANcnt = 0;
        static Uint16 cnt = 0;
        cnt++;
        Device_updateStatus(AC_VOLT_IN, acVolt[cnt%3]);
        Device_updateStatus(TEST_DATA_1, testData1[cnt%3]);
        Device_updateStatus(TEST_DATA_2, testData2[cnt%3]);
        Device_updateStatus(DC_VOLT_OUT, dcVolt[cnt%3]);
        Device_updateStatus(DC_VOLT_DESIRED, dcVoltSet[cnt%3]);
        Device_updateStatus(DC_VOLT_CLOSE_POINT, dcVoltClose[cnt%3]);
        Device_updateStatus(DC_CURR_OUT, dcCurr[cnt%3]);
        Device_updateStatus(DC_CURR_MAX, dcCurrMax[cnt%3]);
        Device_updateStatus(BUS_VOLT, busVolt[cnt%3]);
        Device_updateStatus(TEST_DATA_3, testData3[cnt%3]);
        Device_updateStatus(PFC_SIDE_TEMP, pfcTemp[cnt%3]);
        Device_updateStatus(LLC_SIDE_TEMP, llcTemp[cnt%3]);
        Device_updateStatus(TRANSFORM_TEMP, transTemp[cnt%3]);

        Device_updateStatus(TEST_DATA_4, testData4[cnt%3]);
        Device_updateStatus(TEST_DATA_5, testData5[cnt%3]);
        Device_updateStatus(TEST_DATA_6, testData6[cnt%3]);

//        Device_updateStatus(AC_VOLT_IN, 1);
//        Device_updateStatus(AC_CURR_IN, 2);
//        Device_updateStatus(AC_GRID_FREQ, 3);
//        Device_updateStatus(DC_VOLT_OUT, 4);
//        Device_updateStatus(DC_VOLT_DESIRED, 5);
//        Device_updateStatus(DC_VOLT_CLOSE_POINT, 6);
//        Device_updateStatus(DC_CURR_OUT, 7);
//        Device_updateStatus(DC_CURR_MAX, 8);
//        Device_updateStatus(BUS_VOLT, 9);
//        Device_updateStatus(MAX_POWER_OUTPUT, 10);
//        Device_updateStatus(PFC_SIDE_TEMP, 11);
//        Device_updateStatus(LLC_SIDE_TEMP, 12);
//        Device_updateStatus(TRANSFORM_TEMP, 13);


        Device_updateAlarm(DEVICE_AC_OV, (cnt+1)%2);
        Device_updateAlarm(DEVICE_AC_UV, cnt%2);
        Device_updateAlarm(DEVICE_AC_OC, (cnt+1)%2);
        Device_updateAlarm(DEVICE_BUS_OV, cnt%2);
        Device_updateAlarm(DEVICE_BUS_UV, cnt%2);
        Device_updateAlarm(DEVICE_BATTERY_SOC, 6);

        CanApp_MainFunction();
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


    Device_init();
    CanBasic_init();

    /* Notice: 在CanApp_init前 必须先写完deviceImfor */
    Device_setTotalCharger(100);
    Device_setCompleteCharger(77);
    Device_setInterruptedCharger(23);
    Device_setChargerHours(620);
    Device_setChargerAH(999);

    Uint8 primerVersion[6] = {'7','7','7','7','7','7'};
    Device_setPFCSideSWVersion(primerVersion, 6);

    if(Device_setConfigureValue(chargerCfg, DEVICE_SET_MAXID)) {
        while(1){
            //配置信息长度超范围
            ;
        }
    }

    CanApp_init();


    Device_updateAlarm(DEVICE_CHARGING_STATUS, 7);
    Device_updateAlarm(DEVICE_AC_OV, 1);
    Device_updateAlarm(DEVICE_AC_UV, 1);
    Device_updateAlarm(DEVICE_AC_OC, 1);
    Device_updateAlarm(DEVICE_BUS_OV, 1);
    Device_updateAlarm(DEVICE_BUS_UV, 1);

    Device_updateAlarm(DEVICE_DC_OV_ALARM, 1);
    Device_updateAlarm(DEVICE_DC_OV_CLOSE, 1);
    Device_updateAlarm(DEVICE_DC_UV, 1);
    Device_updateAlarm(DEVICE_ENV_TEMP_HIGH, 1);
    Device_updateAlarm(DEVICE_ENV_TEMP_LOW, 1);
    Device_updateAlarm(DEVICE_PFC_TEMP_HIGH, 1);
    Device_updateAlarm(DEVICE_DC_TEMP_HIGH, 1);
    Device_updateAlarm(DEVICE_SCI_FAIL, 1);

    Device_updateAlarm(DEVICE_PFC_SIDE_FAIL, 1);
    Device_updateAlarm(DEVICE_PFC_ROM_FAIL, 1);
    Device_updateAlarm(DEVICE_DC_ROM_FAIL, 1);
//    Device_updateAlarm(DEVICE_BATTERY_SOC, 7);


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



































