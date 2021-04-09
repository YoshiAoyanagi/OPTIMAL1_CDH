// AppManager.h

#ifndef _APPMANAGER_h
#define _APPMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <stddef.h> // for size_t

#include "AppInfo.h"
#include "CommonTlmCmdPacket.h"

#define AM_MAX_APPS (128)

typedef struct
{
	AppInfo ais[AM_MAX_APPS];
} AMInfo;

typedef enum
{
	AM_SUCCESS,
	AM_INVALID_ID,
	AM_NOT_REGISTERED,
	AM_UNKNOWN
} AM_ACK;

extern const AMInfo *ami;

void AM_initialize(void);

AM_ACK AM_register_ai(size_t id,
	const AppInfo* ai);

void AM_initialize_all_apps(void);

AM_ACK AM_initialize_app(size_t id);

AM_ACK AM_execute_app(size_t id);

int Cmd_REGISTER_APP(const CTCP* packet);
int Cmd_INITIALIZE_APP(const CTCP* packet);
int Cmd_EXECUTE_APP(const CTCP* packet);

#endif // APP_MANAGER_H_
