/* 该文件用于定义模块所用的数据结构*/
#ifndef SCOPETYPES_H_
#define SCOPETYPES_H_

#include "scopecfg.h"
#include "basic.h"

#define SCOPE_ADDHI             0xAA
#define SCOPE_ADDLO             0xAA

typedef enum BoolType{
    false = 0,
    true = 1
}bool;

typedef enum EvtlevelType {
    customer = 0,
    warning = 1,
    fault = 2
}Evtlevel;


typedef enum ScoResType{
    resOk = 0,
    resRefreshErr = 1,
    resMemFull = 2
}ScoRes;

typedef enum ScoFrameType{
    frameBasic = 0,
    frameName = 1,
    frameEvent = 2,
    frameReal = 3,
    frameEvtDiscript = 4,
    frameEvtData = 5
}ScoFrame;

#define SCOPE_CHN0              0x00000001
#define SCOPE_CHN1              0x00000002
#define SCOPE_CHN2              0x00000004
#define SCOPE_CHN3              0x00000008
#define SCOPE_CHN4              0x00000010
#define SCOPE_CHN5              0x00000020
#define SCOPE_CHN6              0x00000040
#define SCOPE_CHN7              0x00000080
#define SCOPE_CHN8              0x00000100
#define SCOPE_CHN9              0x00000200
#define SCOPE_CHN10             0x00000400
#define SCOPE_CHN11             0x00000800
#define SCOPE_CHN12             0x00001000
#define SCOPE_CHN13             0x00002000
#define SCOPE_CHN14             0x00004000
#define SCOPE_CHN15             0x00008000


#define SCOPE_CHN16             0x00010000
#define SCOPE_CHN17             0x00020000
#define SCOPE_CHN18             0x00040000
#define SCOPE_CHN19             0x00080000
#define SCOPE_CHN20             0x00100000
#define SCOPE_CHN21             0x00200000
#define SCOPE_CHN22             0x00400000
#define SCOPE_CHN23             0x00800000
#define SCOPE_CHN24             0x01000000
#define SCOPE_CHN25             0x02000000
#define SCOPE_CHN26             0x04000000
#define SCOPE_CHN27             0x08000000
#define SCOPE_CHN28             0x10000000
#define SCOPE_CHN29             0x20000000
#define SCOPE_CHN30             0x40000000
#define SCOPE_CHN31             0x80000000


#endif /* SCOPETYPES_H_ */
