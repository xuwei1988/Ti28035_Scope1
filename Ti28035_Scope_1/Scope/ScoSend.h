#ifndef SCOPE_SCOSEND_H_
#define SCOPE_SCOSEND_H_

#include "scopetypes.h"

void ScoSend_init(void);
bool ScoSend_byteGet(Uint8 *pdata);

void ScoSend_basicImfor(void);
void ScoSend_channelName(Uint8 id);
void ScoSend_eventImfor(Uint8 id);
void SciSend_realData(void);

#endif /* SCOPE_SCOSEND_H_ */
