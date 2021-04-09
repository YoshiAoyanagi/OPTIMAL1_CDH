// Debug_Application.h

#ifndef _DEBUG_APPLICATION_h
#define _DEBUG_APPLICATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "AppHeaders.h"
#include "Config.h"
#include "TimeManager.h"
#include "tlm_frame.h"
#include "Utility.h"
#include "PacketHandler.h"
#include "EPS_update.h"
#include "GNSS_receiver.h"
#include "STRX.h"

#define DEBUG_TASK_INTERVAL_SEC		2

AppInfo print_debug_message(void);

void	debug_cmd_menue_stx(void);
void	debug_cmd_console_stx(void);
#endif

