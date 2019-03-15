/* 该文件主要给客户做底层配置*/
#ifndef SCOPE_SCOPECFG_H_
#define SCOPE_SCOPECFG_H_

/* 定义实时上传数据内存块保存数据的数量 ---------------------------------------*/
#define SCOPE_REALDATA_NUM          100
#define SCOPE_REALDATA_FNUM         4

/* 定义异步上传数据内存块保存数据的数量 ---------------------------------------*/
#define SCOPE_EVTDATA_NUM           30
#define SCOPE_EVTDATA_FNUM          45

///* 定义事件标记数据块数量 ----------------------------------------------------*/
//#define SCOPE_EVTRECORD_FNUM        4

/* 定义事件控制块的数量，用于控制事件数据发送 ----------------------------------*/
#define SCOPE_EVTPCB_NUM            1

/* 定义事件描述数组的大小 -----------------------------------------------------*/
#define SCOPE_EVTDESCRIPTION_NUM    50




#endif /* SCOPE_SCOPECFG_H_ */
