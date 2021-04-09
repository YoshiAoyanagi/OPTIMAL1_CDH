// AppInfo.h

#ifndef _APPINFO_h
#define _APPINFO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "OBCTime.h"

typedef struct
{
	const char *name;
	step_t proc_time;
	step_t prev;
	step_t min;
	step_t max;
	void(*initializer)(void);
	void(*entry_point)(void);
} AppInfo;

AppInfo create_app_info(const char* name,
	step_t proc_time,
	void(*initializer)(void),
	void(*entry_point)(void));

#endif // APP_INFO_H_

