// EPS_update.h

#ifndef _EPS_UPDATE_h
#define _EPS_UPDATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "AppHeaders.h"
#include "EPS.h"


#define	EPS_TASK_INTERVAL_SEC			0.5//sec

AppInfo EPS_update(void);
void	EPS_DebugMessage(void);
void	EPS_PWR_DebugMessage(void);
#endif

