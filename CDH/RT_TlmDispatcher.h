// RT_TlmDispatcher.h

#ifndef _RT_TLMDISPATCHER_h
#define _RT_TLMDISPATCHER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "CommonTlmCmdPacket.h"

AppInfo rt_tlm_transmitter(void);
void rt_tlm_transmitter_irq(void);

#endif
