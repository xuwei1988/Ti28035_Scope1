#include "ScoSend.h"
#include "ScoDevice.h"

Uint8 sendBuff[200];
Uint8 sendTotlen;              //������Ч���ݳ���
Uint8 sendPos;              //��ǰ�������λ��

typedef struct {
    Uint16 addHi;
    Uint16 addLo;
    Uint16 len;
    Uint16 ftype;
}Framehead;

void ScoSend_init(void) {
    sendTotlen = 0;
    sendPos = 0;
}


/* �����ֽڻ�ȡ���˺�����Ҫ��Ϊ���ڷ��ͽӿ� --------------------------------------------------------*/
bool ScoSend_byteGet(Uint8 *pdata) {
    bool res = false;
    /* step1.2: ���ݻ�����ǰ״̬�������Ƿ��ȡ��һ֡���� */
    switch(ScoDevice_getState()) {
        case ScoDisconnect:
        case ScoNameUploading:
        case ScoEventUploading:
        case ScoWaitSetting:{
            if(sendTotlen && sendPos != sendTotlen) {
                *pdata = sendBuff[sendPos++];
                res = true;
            }
            /* step1.1: ��������Ѿ�������� */
            if(sendPos == sendTotlen) {
                sendPos = 0;
                sendTotlen = 0;
            }
            break;
        }
		case ScoRunning: {
			if (sendTotlen == 0) {
				SciSend_realData();
			}
			if (sendTotlen && sendPos != sendTotlen) {
			    Framehead *fh = (Framehead *)sendBuff;
			    switch (fh->ftype) {
                    case frameEvtDiscript: {
                        *pdata = sendBuff[sendPos++];
                        break;
                    }
                    case frameReal:
                    case frameEvtData: {

                        if (sendPos < 6) {
                            *pdata = sendBuff[sendPos++];
                        }
                        else {
                            if (sendPos % 2 ) {
                                *pdata = (sendBuff[6 + (sendPos - 6) / 2] & 0x00FF);
                            }
                            else {
                                *pdata = ((sendBuff[6 + (sendPos - 6) / 2] & 0xFF00) >> 8);
                            }
                            sendPos++;
                        }
                        break;
                    }
			    }

				res = true;
			}

			if (sendPos == sendTotlen) {
				sendPos = 0;
				sendTotlen = 0;
			}

			break;
		}
    }
    return res;
}

void ScoSend_basicImfor(void) {
    sendPos = 0;
    sendTotlen = 0;
    sendBuff[sendTotlen++] = SCOPE_ADDHI;
    sendBuff[sendTotlen++] = SCOPE_ADDLO;
    sendTotlen += SciDevice_getBaseImfor(&sendBuff[2]);
}

void ScoSend_channelName(Uint8 id) {
    sendPos = 0;
    sendTotlen = 0;
    sendBuff[sendTotlen++] = SCOPE_ADDHI;
    sendBuff[sendTotlen++] = SCOPE_ADDLO;
    sendTotlen += SciDevice_getChannelID(&sendBuff[2], id);
}

void ScoSend_eventImfor(Uint8 id) {
    sendPos = 0;
    sendTotlen = 0;
    sendBuff[sendTotlen++] = SCOPE_ADDHI;
    sendBuff[sendTotlen++] = SCOPE_ADDLO;

    sendTotlen += SciDevice_getEventImfor(&sendBuff[2], id);
}

void SciSend_realData(void) {
    Uint16 len = 0;
    sendPos = 0;
    sendTotlen = 0;

    len = SciDevice_getRunningData(&sendBuff[2]);
    if(len) {
        sendBuff[sendTotlen++] = SCOPE_ADDHI;
        sendBuff[sendTotlen++] = SCOPE_ADDLO;
        sendTotlen += len;
    }

}























































