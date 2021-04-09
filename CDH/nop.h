// nop.h

#ifndef _NOP_h
#define _NOP_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "CommonTlmCmdPacket.h"

AppInfo create_nop_task(void);

int Cmd_NOP(const CTCP* packet);

#endif // NOP_H_
