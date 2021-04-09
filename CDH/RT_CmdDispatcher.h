// RT_CmdDispatcher.h

#ifndef _RT_CMDDISPATCHER_h
#define _RT_CMDDISPATCHER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "CommonTlmCmdPacket.h"
#include "PacketHandler.h"

AppInfo rt_cmd_dispatcher(void);

int Cmd_CLEAR_ALL_REALTIME(const CTCP* packet);

#endif // REALTIME_COMMAND_DISPATCHER_H_