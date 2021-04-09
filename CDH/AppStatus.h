// AppStatus.h

#ifndef _APPSTATUS_h
#define _APPSTATUS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "CommonTlmCmdPacket.h"
typedef enum
{
	AS_SUCCESS,
	AS_INVALID_ID,
	AS_UNKNOWN
} AS_ACK;

typedef struct
{
	size_t app_id;
} ASInfo;

extern const ASInfo *asi;

AppInfo AppStatus(void);
int Cmd_AS_set_id(const CTCP* packet);

#endif // APP_STATUS_H_

