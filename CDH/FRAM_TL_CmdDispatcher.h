// FRAM_TL_CmdDispatcher.h

#ifndef _FRAM_TL_CMDDISPATCHER_h
#define _FRAM_TL_CMDDISPATCHER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CommonTlmCmdPacket.h"
#include "PacketHandler.h"
#include "AppInfo.h"

AppInfo fram_tl_cmd_dispatcher(void);

#endif

