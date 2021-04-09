// TlmDispatcher.h

#ifndef _TLMDISPATCHER_h
#define _TLMDISPATCHER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CommonTlmCmdPacket.h"
extern unsigned char tlmd_anomaly_detect_enable;

void TLMD_initialize(void);
void TLMD_execute(void);

#endif

