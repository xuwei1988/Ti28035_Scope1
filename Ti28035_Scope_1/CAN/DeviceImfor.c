#include "DeviceImfor.h"

const Uint8 ECUSerialNumber[10] = {'A', 'B','C','D','E','F','G','H','I','J'};
const Uint8 ECUSoftWareVersion[6] = {'7', '7', '7', '7', '7', '7'};
Uint8 PrimarySideSoftwareVersion[6];

const Uint8 ECUReleaseDate[8] = {'2', '0', '1', '9', '1', '1', '1', '4'};


DeviceGenImfor deviceGenImfor;
Uint16 deviceRunImfor[DEVICE_RUNIMFOR_MAXNUM];
Uint16 deviceControlImfor[DEVICE_SET_MAXID];


#pragma DATA_SECTION(bootData, "GlobalData");
BootData bootData;

void bezero(Uint8 *ptr, Uint16 len){
    while (len-- != (Uint16)0) {
        *ptr++ = (Uint8)0;
    }
}

void Device_init(void) {
    bezero((Uint8 *)&deviceGenImfor, sizeof(deviceGenImfor));
    bezero((Uint8 *)deviceRunImfor, sizeof(deviceRunImfor));
    bezero((Uint8 *)deviceControlImfor, sizeof(deviceControlImfor));
    bezero((Uint8 *)PrimarySideSoftwareVersion, sizeof(PrimarySideSoftwareVersion));
}

Uint8 Device_setConfigureValue(Uint16 *pdata, Uint8 len) {
    Uint8 i = 0;
    if(len != DEVICE_SET_MAXID)
        return 1;
    for(i = 0; i < len; i++) {
        deviceControlImfor[i] = pdata[i];
    }
    return 0;
}

void Device_setPFCSideSWVersion(Uint8 *pVersion, Uint8 len) {
    if(len != 6)
        return;
    PrimarySideSoftwareVersion[0] = pVersion[0];
    PrimarySideSoftwareVersion[1] = pVersion[1];
    PrimarySideSoftwareVersion[2] = pVersion[2];
    PrimarySideSoftwareVersion[3] = pVersion[3];
    PrimarySideSoftwareVersion[4] = pVersion[4];
    PrimarySideSoftwareVersion[5] = pVersion[5];
}

void Device_setTotalCharger(Uint16 num) {
    deviceGenImfor.totalChargers = num;
}

void Device_setCompleteCharger(Uint16 num) {
    deviceGenImfor.completeChargers = num;
}

void Device_setInterruptedCharger(Uint16 num) {
    deviceGenImfor.interruptedChargers = num;
}

void Device_setChargerHours(Uint16 hours) {
    deviceGenImfor.chargerHours = hours;
}

void Device_setChargerAH(Uint16 ahs) {
    deviceGenImfor.totalAH = ahs;
}


void Device_updateStatus(Uint8 ID, Uint16 value) {
    //如果写入的状态量ID超范围，直接返回
    if(ID > DEVICE_STATUS_MAXID)
        return;
    deviceRunImfor[ID] = value;
}

void Device_updateAlarm(Uint8 ID, Uint8 state){
    Uint16 *pState = 0;
    if(ID > DEVICE_MAX_STATE)
        return;

    if(ID <= DEVICE_SCI_FAIL)
        pState = &deviceRunImfor[DEVICE_STATE1];
    else
        pState = &deviceRunImfor[DEVICE_STATE2];

    switch(ID) {
    /* state1 */
        case DEVICE_CHARGING_STATUS:{
            ((Device_AlarmNStates1 *)pState)->CHARGING_STATUS = state;
            break;
        }
        case DEVICE_AC_OV:{
            ((Device_AlarmNStates1 *)pState)->AC_OV = state;
            break;
        }
        case DEVICE_AC_UV:{
            ((Device_AlarmNStates1 *)pState)->AC_UV = state;
            break;
        }
        case DEVICE_AC_OC:{
            ((Device_AlarmNStates1 *)pState)->AC_OC = state;
            break;
        }
        case DEVICE_BUS_OV:{
            ((Device_AlarmNStates1 *)pState)->BUS_OV = state;
            break;
        }
        case DEVICE_BUS_UV:{
            ((Device_AlarmNStates1 *)pState)->BUS_UV = state;
            break;
        }

        case DEVICE_DC_OV_ALARM:{
            ((Device_AlarmNStates1 *)pState)->DC_OV_ALARM = state;
            break;
        }
        case DEVICE_DC_OV_CLOSE:{
            ((Device_AlarmNStates1 *)pState)->DC_OV_CLOSE = state;
            break;
        }
        case DEVICE_DC_UV:{
            ((Device_AlarmNStates1 *)pState)->DC_UV = state;
            break;
        }
        case DEVICE_ENV_TEMP_HIGH:{
            ((Device_AlarmNStates1 *)pState)->ENV_TEMP_HIGH = state;
            break;
        }
        case DEVICE_ENV_TEMP_LOW:{
            ((Device_AlarmNStates1 *)pState)->ENV_TEMP_LOW = state;
            break;
        }
        case DEVICE_PFC_TEMP_HIGH:{
            ((Device_AlarmNStates1 *)pState)->PFC_TEMP_HIGH = state;
            break;
        }
        case DEVICE_DC_TEMP_HIGH:{
            ((Device_AlarmNStates1 *)pState)->DC_TEMP_HIGH = state;
            break;
        }
        case DEVICE_SCI_FAIL:{
            ((Device_AlarmNStates1 *)pState)->SCI_FAIL = state;
            break;
        }

    /* state2 */
        case DEVICE_PFC_SIDE_FAIL:{
            ((Device_AlarmNStates2 *)pState)->PFC_SIDE_FAIL = state;
            break;
        }
        case DEVICE_PFC_ROM_FAIL:{
            ((Device_AlarmNStates2 *)pState)->PFC_ROM_FAIL = state;
            break;
        }
        case DEVICE_DC_ROM_FAIL:{
            ((Device_AlarmNStates2 *)pState)->DC_ROM_FAIL = state;
            break;
        }
        case DEVICE_BATTERY_SOC: {
            ((Device_AlarmNStates2 *)pState)->BATERY_SOC = state;
            break;
        }
        default: break;
    }
}

Uint16 Device_GetSetingValue(Uint8 ID) {
    if(ID > DEVICE_SET_MAXID)
        return 0xFFFF;
    return deviceControlImfor[ID];
}

Uint16 Device_GetSetingState(Uint8 ID) {
    Uint16 *pState = 0;
    Uint16 res = 0xFFFF;
    if(ID < 16)
        pState = &deviceControlImfor[DEVICE_STATE1_SET];
    switch(ID) {
        case DEVICE_POWEROFF_SET:{
            res = ((Device_SetState1 *)pState)->POWER_OFF;
            break;
        }
        case DEVICE_LOCK_SET:{
            res = ((Device_SetState1 *)pState)->LOCKED;
            break;
        }
        default: break;
    }

    return res;
}

