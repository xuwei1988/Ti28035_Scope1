/* 该文件用于用户配置一些设备信息，模块根据配置分配内存、条件编译 */
#ifndef SCOPE_SCOPEPORT_H_
#define SCOPE_SCOPEPORT_H_


/*
 * step1: 和设备直接相关信息
 * 必须手动配置
 */
    /* 定义设备最大采样通道 --------------------------------------------------------------------------------*/
    #define SCOPE_MAX_CHN               3

    /* 定义设备支持的最大事件数量 --------------------------------------------------------------------------*/
    #define SCOPE_EVENT_CNT             40
    /* 以下由用户定义每个事件的名称和ID ---------------------------------------------------------------------*/
    #define A_PHRASE_HIGH               0
    #define B_PHRASE_LOW                1
    #define C_PHRASE_HIGH               2

/* 定义单通道保存周期数量 ------------------------------------------------------------------------------*/
#define SCOPE_CHN_CYC               3

/* 定义20ms(一个工频周期) 采样点数 ---------------------------------------------------------------------*/
#define SCOPE_CYC_NUM               200


/* 定义单通道保存的所有采样点数 ------------------------------------------------------------------------*/
#define SCOPE_CHN_PTS               SCOPE_CHN_CYC * SCOPE_CYC_NUM

/* 定义故障处理周期调用时间 ----------------------------------------------------------------------------*/
#define SCOPE_EVTPRQ_1MS            20
#define SCOPE_EVTPRO_2MS            10
#define SCOPE_EVTPRO_5MS            4
#define SCOPE_EVTPRO_10MS           2
#define SCOPE_EVTPRO_20MS           1

/* 注：调用频率越高，数据的抖动越小，但是会增加系统时间消耗*/
#define SCOPE_EVTPRO_FRQ            SCOPE_EVTPRQ_1MS



#endif /* SCOPE_SCOPEPORT_H_ */
