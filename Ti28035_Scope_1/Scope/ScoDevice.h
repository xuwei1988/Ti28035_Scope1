/* 此文件提供描述设备的结构体，已经操作这些结构体的指针 */
#ifndef SCOPE_SCODEVICE_H_
#define SCOPE_SCODEVICE_H_

#include "scopetypes.h"
#include "scopeport.h"

typedef struct {
    bool isSelected;
    char *name;
}Channel;

typedef struct {
    Uint8 number;
    Channel channels[SCOPE_MAX_CHN];
}ChannelImfor;

/* 2018.04.16添加事件描述结构体 --*/
typedef struct {
    char *discription;
    Evtlevel level;

    Uint8 cycBefore;
    Uint8 cycAfter;
    Uint32 selectedChns;
}Event;

typedef struct EventsTag{
    Uint16 used;
    Event list[SCOPE_EVENT_CNT];
}Events;


typedef enum ScoStateType {
    ScoDisconnect,
    ScoNameUploading,
    ScoEventUploading,
    ScoWaitSetting,
    ScoRunning
}ScoState;

typedef struct ScoDeviceTag{
    Uint16 cycPoints;               //工频周期(20ms)采样点数
    ChannelImfor channelImfor;      //通道名称及使能信息
    Events events;                  //事件信息

    Uint8 updateInterval;           //实时量上传间隔次数
    ScoState state;                 //当前模块运行状态
}ScoDevice;


extern ScoDevice scoDevice;





#define EVENT_ENABLE            1
#define EVENT_DISABLE           0

#define CUSTOMER                0
#define WARNING                 1
#define ERROR                   2

#define ScoDevice_setEvent(id, lv, bf, af, chns)    \
    scoDevice.events.used++;\
    scoDevice.events.list[id].discription = #id;\
    scoDevice.events.list[id].level = (Evtlevel)lv;\
    scoDevice.events.list[id].cycBefore = bf;\
    scoDevice.events.list[id].cycAfter = af;\
    scoDevice.events.list[id].selectedChns = chns;



void ScoDevice_init(void);
void ScoDevice_handle(void);
void ScoDevice_setCycPts(Uint16 points);
void ScoDevice_setName(char *name);
void ScoDevice_setUpdateInterval(Uint8 interval);
void ScoDevice_selectChannel(Uint8 *pIds, Uint8 len);
void ScoDevice_refresh(void);
void ScoDevice_eventHappen(Uint8 id);

void ScoDevice_check(void);

inline void ScoDevice_setState(ScoState state) {
    scoDevice.state = state;
}

inline ScoState ScoDevice_getState(void) {
    return scoDevice.state;
}

void SciDevice_cleanChannel(void);


/* 获取设备基础信息函数  ------------------------------------------------*/
Uint16 SciDevice_getBaseImfor(Uint8 *dst);
/* 获取指定通道ID的名称 ------------------------------------------------*/
Uint16 SciDevice_getChannelID(Uint8 *dst, Uint8 id);
/* 获取指定事件ID的相关信息 --------------------------------------------*/
Uint16  SciDevice_getEventImfor(Uint8 *dst, Uint8 id);
/* 获取当前实时信息（在通讯过程中产生的示波数据和故障数据）---------------*/
Uint16 SciDevice_getRunningData(Uint8 *dst);


#endif /* SCOPE_SCODEVICE_H_ */
