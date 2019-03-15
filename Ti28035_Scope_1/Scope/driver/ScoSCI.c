#include "ScoSCI.h"
#include "ScoRecv.h"
#include "ScoSend.h"

void ScoSCI_init(void) {
    ScoRecv_init();
    ScoSend_init();
    /* SCI 相关的初始化 －－*/
    InitSciGpio();
    InitSci();
}

void ScoSCI_recv(void) {
    if(SciaRegs.SCIRXST.bit.RXRDY) {
        Uint16 data = SciaRegs.SCIRXBUF.all;
        ScoRecv_byteRecving((Uint8)data);
    }
}

void ScoSCI_send(void) {
    Uint16 data = 0;
    /* step1: 判断发送寄存器是否已经ready*/
    if(SciaRegs.SCICTL2.bit.TXRDY) {
        if(ScoSend_byteGet((Uint8 *)&data) == true){
            SciaRegs.SCITXBUF = data;
        }
    }
}

