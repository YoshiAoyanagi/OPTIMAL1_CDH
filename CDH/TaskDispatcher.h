// TaskDispatcher.h

#ifndef _TASKDISPATCHER_h
#define _TASKDISPATCHER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#include "PacketHandler.h"
#include "PacketList.h"
#include "BlockCommandTable.h"
#include "CommandDispatcher.h"
#include "CommonTlmCmdPacket.h"

#define TDSP_TASK_MAX BCT_MAX_BLOCK_LENGTH

typedef struct
{
	CDIS tskd;
	size_t task_list_id;
	cycle_t activated_at;
} TDSP_Info;

typedef enum
{
	TDSP_SUCCESS,
	TDSP_DEPLOY_FAILED,
	TDSP_CYCLE_OVERRUN,
	TDSP_STEP_OVERRUN,
	TDSP_TASK_EXEC_FAILED,
	TDSP_UNKNOWN
} TDSP_ACK;

extern const TDSP_Info *TDSP_info;

void TDSP_initialize(void);

TDSP_ACK TDSP_set_task_list_id(size_t id);

void TDSP_execute_pl_as_task_list(void);

void TDSP_resync_internal_counter(void);

int Cmd_TDSP_SET_TASK_LIST_ID(const CTCP* packet);
#endif // TASK_DISPATCHER_H_
