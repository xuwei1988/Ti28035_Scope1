#ifndef CAN_CANDSP_CANAPP_H_
#define CAN_CANDSP_CANAPP_H_

#include "DSP2803x_Device.h"

#define CAN_TX_RUNIMFOR_MAXPOS              4
#define CAN_TX_ALLIMFOR_MAXPOS              18

/* �������������ַ�������ƫ�� -----------------------------------------------------*/
//#define CAN_BASE_ID                         0x2FF           //�����������ID

#define CAN_TX_STATUS1_OFFSET               0x0             //STATYS1��ַƫ��
#define CAN_TX_STATUS2_OFFSET               0x1             //STATYS2��ַƫ��
#define CAN_TX_STATUS3_OFFSET               0x2             //STATYS3��ַƫ��
#define CAN_TX_STATUS4_OFFSET               0x3             //STATYS3��ַƫ��
#define CAN_TX_ALARMS_OFFSET                0x4             //STATE��ַƫ��
#define CAN_TX_DEVIMFOR_OFFSET              0x5             //�豸��Ϣ��ص�ַƫ��
#define CAN_TX_CONFIGURE_OFFSET             0x6             //�豸������Ϣ��ַƫ��


/* ����ģ�����ʹ�õ�CAN buffer���� -------------------------------------------------*/
#define CAN_REFRESH_BUFFER_NUM              CAN_TX_ALARMS_OFFSET
#define CAN_STATIC_BUFFER_NUM               16
#define CAN_BUFFER_SIZE                     (CAN_REFRESH_BUFFER_NUM + CAN_STATIC_BUFFER_NUM)

/* ����Category ������ --------------------------------------------------------------*/
#define CAN_SN_ID                           0x0
#define CAN_SWVERSION_ID                    0x1
#define CAN_PFCVERSION_ID                   0x2
#define CAN_RELEASEDATE_ID                  0x3
#define CAN_DEVICEIMFOR_ID                  0x4


/* CANApp �����ݽṹ���� ------------------------------------------------------------*/
typedef struct {
    Uint8   boxOffset;                  //��ַƫ��
    Uint8   byteNum;                    //��Ч�ֽ���
    Uint16  data[4];
}CanBuffer;

typedef struct {
    union CATEGORY{
        Uint16 all;
        struct {
            Uint16 categoty : 8;
            Uint16 totalSeq : 4;
            Uint16 currSeq  : 4;
        }bit;
    }CategoryImfor;
    Uint16 data[3];
}CanDeviceImfor;

#define NO_BOOT_REQ             0
#define DC_BOOT_REQ             1
#define PFC_BOOT_REQ            2
#define ALL_BOOT_REQ            3

typedef struct {
    Uint8 recvIndicator;
    Uint8 currPos;
    Uint8 endPos;
    Uint8 bootState;
}CanHelper;

void CanApp_init(void);
void CanApp_MainFunction(void);
Uint8 CanApp_IsRecieved(void);
Uint8 CanApp_GetRecieveIndicator(void);

#endif /* CAN_CANDSP_CANAPP_H_ */
