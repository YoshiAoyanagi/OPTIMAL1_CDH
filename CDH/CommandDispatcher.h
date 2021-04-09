// CommandDispatcher.h

#ifndef _COMMANDDISPATCHER_h
#define _COMMANDDISPATCHER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "OBCTime.h"
#include "PacketList.h"

typedef struct
{
	OBCTime time;
	int code;
	int sts;
} CDIS_EXEC_INFO;

typedef struct
{
	CDIS_EXEC_INFO prev;
	CDIS_EXEC_INFO prev_err;
	unsigned int error_counter;
	int lockout;
	int stop_on_error;
	PL_Info *pli;
} CDIS;

CDIS CDIS_init(PL_Info* pli);

void CDIS_dispatch_command(CDIS* cdis);

void CDIS_clear_command_list(CDIS* cdis);

void CDIS_clear_error_status(CDIS* cdis);

#endif // COMMAND_DISPATCHER_H_

