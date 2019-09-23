#ifndef DEVICEIMFOR_H_
#define DEVICEIMFOR_H_
#include "DSP2803x_Device.h"

extern const Uint8 ECUSerialNumber[10];
extern const Uint8 ECUSoftWareVersion[6];
extern const Uint8 ECUReleaseDate[8];
extern Uint8 PrimarySideSoftwareVersion[6];

typedef struct{
    Uint32 isNeed;
}BootData;

extern BootData bootData;
#define REQURE_FLAG     0x55555555

/* 宏定义机器运行相关模拟量 ID */
#define AC_VOLT_IN                      0
#define AC_CURR_IN                      1
#define AC_GRID_FREQ                    2
#define DC_VOLT_OUT                     3
#define DC_VOLT_DESIRED                 4
#define DC_VOLT_CLOSE_POINT             5
#define DC_CURR_OUT                     6
#define DC_CURR_MAX                     7
#define BUS_VOLT                        8
#define MAX_POWER_OUTPUT                9
#define PFC_SIDE_TEMP                   10
#define LLC_SIDE_TEMP                   11
#define TRANSFORM_TEMP                  12
#define DEVICE_STATUS_MAXID             TRANSFORM_TEMP

#define RSVD0                           13
#define RSVD1                           14
#define RSVD2                           15
#define DEVICE_STATE1                   16
#define DEVICE_STATE2                   17
#define RSVD3                           18
#define RSVD4                           19
#define DEVICE_RUNIMFOR_MAXNUM          RSVD4 + 1

/* 宏定义机器运行时的状态量ID */
/* State1 */
#define DEVICE_CHARGING_STATUS          0
#define DEVICE_AC_OV                    1
#define DEVICE_AC_UV                    2
#define DEVICE_AC_OC                    3
#define DEVICE_BUS_OV                   4
#define DEVICE_BUS_UV                   5

#define DEVICE_DC_OV_ALARM              6
#define DEVICE_DC_OV_CLOSE              7
#define DEVICE_DC_UV                    8
#define DEVICE_ENV_TEMP_HIGH            9
#define DEVICE_ENV_TEMP_LOW             10
#define DEVICE_PFC_TEMP_HIGH            11
#define DEVICE_DC_TEMP_HIGH             12
#define DEVICE_SCI_FAIL                 13

/* State2 */
#define DEVICE_PFC_SIDE_FAIL            14
#define DEVICE_PFC_ROM_FAIL             15
#define DEVICE_DC_ROM_FAIL              16
#define DEVICE_BATTERY_SOC              17
#define DEVICE_MAX_STATE                DEVICE_BATTERY_SOC

/* 宏定义，机器的设定量ID  */
#define DEVICE_DC_VOLT_SET              0
#define DEVICE_DC_VOLT_DEFAULT_SET      1
#define DEVICE_DC_OV_SET                2
#define DEVICE_DC_CURR_SET              3
#define DEVICE_DC_CURR_DEFAULT_SET      4
#define DEVICE_BUS_VOLT_SET             5
#define DEVICE_MAX_POWER_SET            6
#define DEVICE_DCMAX_VOLT_CLA_SET       7
#define DEVICE_DCMIN_VOLT_CLA_SET       8
#define DEVICE_DCMAX_CURR_CLA_SET       9
#define DEVICE_DCMIN_CURR_CLA_SET       10
#define DEVICE_DCSETEDMAX_VOLT_CLA_SET  11
#define DEVICE_DCSETEDMIN_VOLT_CLA_SET  12
#define DEVICE_ACMAX_CURR_CLA_SET       13
#define DEVICE_ACMIN_CURR_CLA_SET       14
#define RSVD0_SET                       15
#define RSVD1_SET                       16
#define RSVD2_SET                       17
#define RSVD3_SET                       18
#define RSVD4_SET                       19
#define RSVD5_SET                       20
#define RSVD6_SET                       21
#define DEVICE_STATE1_SET               22
#define RSVD7_SET                       23
#define DEVICE_SET_MAXID                RSVD7_SET + 1

/* 宏定义  机器设定状态ID */
#define DEVICE_POWEROFF_SET             0
#define DEVICE_LOCK_SET                 1

/* 定义机器运行相关告警、状态量 */
typedef struct  {
    Uint16 CHARGING_STATUS  : 3;
    Uint16 AC_OV            : 1;
    Uint16 AC_UV            : 1;
    Uint16 AC_OC            : 1;
    Uint16 BUS_OV           : 1;
    Uint16 BUS_UV           : 1;

    Uint16 DC_OV_ALARM      : 1;
    Uint16 DC_OV_CLOSE      : 1;
    Uint16 DC_UV            : 1;
    Uint16 ENV_TEMP_HIGH    : 1;
    Uint16 ENV_TEMP_LOW     : 1;
    Uint16 PFC_TEMP_HIGH    : 1;
    Uint16 DC_TEMP_HIGH     : 1;
    Uint16 SCI_FAIL         : 1;
}Device_AlarmNStates1;

typedef struct {
    Uint16 PFC_SIDE_FAIL    : 1;
    Uint16 PFC_ROM_FAIL     : 1;
    Uint16 DC_ROM_FAIL      : 1;
    Uint16 BATERY_SOC       : 3;
    Uint16 RSVD             : 10;
}Device_AlarmNStates2;

typedef struct {
    Uint16 POWER_OFF        : 1;
    Uint16 LOCKED           : 1;
    Uint16 DEVICE_IMFOR     : 1;
    Uint16 DC_MODULE_BOOT   : 1;
    Uint16 PFC_MODULE_BOOT  : 1;
    Uint16 RSVD             : 11;
}Device_SetState1;

typedef struct {
    Uint16 totalChargers;               //总充电次数
    Uint16 completeChargers;            //完整充电次数
    Uint16 interruptedChargers;         //被中断的充电次数
    Uint16 chargerHours;                //充电总时长
    Uint16 totalAH;                     //充电安时数
}DeviceGenImfor;


extern Uint16 deviceRunImfor[DEVICE_RUNIMFOR_MAXNUM];
extern DeviceGenImfor deviceGenImfor;
extern Uint16 deviceControlImfor[DEVICE_SET_MAXID];

void bezero(Uint8 *ptr, Uint16 len);

void Device_init(void);
Uint8 Device_setConfigureValue(Uint16 *pdata, Uint8 len);
void Device_setTotalCharger(Uint16 num);
void Device_setCompleteCharger(Uint16 num);
void Device_setInterruptedCharger(Uint16 num);
void Device_setChargerHours(Uint16 hours);
void Device_setChargerAH(Uint16 ahs);
void Device_updateStatus(Uint8 ID, Uint16 value);
void Device_updateAlarm(Uint8 ID, Uint8 state);
Uint16 Device_GetSetingValue(Uint8 ID);
Uint16 Device_GetSetingState(Uint8 ID);
void Device_setPFCSideSWVersion(Uint8 *pVersion, Uint8 len);

#endif /* DEVICEIMFOR_H_ */
