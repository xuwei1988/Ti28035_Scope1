#ifndef CAN_CANDSP_CANBASIC_H_
#define CAN_CANDSP_CANBASIC_H_
#include "DSP28x_Project.h"
void CanBasic_init(void);
Uint8 CanBasic_sendMbox(Uint8 id, Uint16 *pdata);
Uint8 CanBasic_recieveMBox(Uint16 *pbuff);




#endif /* CAN_CANDSP_CANBASIC_H_ */
