// RTC_update.h

#ifndef _RTC_UPDATE_h
#define _RTC_UPDATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "RTC.h"
#include "AppInfo.h"
#include "AppHeaders.h"

AppInfo RTC_update(void);
void RTC_Debug_Message(void);

#endif

