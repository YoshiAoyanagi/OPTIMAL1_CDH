// ModeManager.h

#ifndef _MODEMANAGER_h
#define _MODEMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "OBCTime.h"
#include "ModeDefinisions.h"
#include "CommonTlmCmdPacket.h"
#include "cmd_code_obc.h"

typedef enum
{
	MM_SUCCESS,
	MM_BAD_ID,
	MM_BAD_CYCLE,
	MM_BAD_BC_INDEX,
	MM_OVERWRITE,
	MM_ILLEGAL_MOVE,
	MM_NOT_IN_PROGRESS,
	MM_TL_LOAD_FAILED
} MM_ACK;

typedef enum
{
	MM_FINISHED,
	MM_IN_PROGRESS
} MM_Status;

typedef struct
{
	size_t mode_list[MODE_MAX];
	size_t transition_table[MODE_MAX][MODE_MAX];
	MM_Status stat;
	MD_ModeID previous_id;
	MD_ModeID current_id;
} ModeManagerInfo;

extern const ModeManagerInfo* ops_mode;

void MM_initialize(void);

MM_ACK MM_set_mode_list(MD_ModeID mode,
	size_t bc_index);

MM_ACK MM_set_transition_table(MD_ModeID from,
	MD_ModeID to,
	size_t bc_index);

MM_ACK MM_start_transition(MD_ModeID id);

MM_ACK MM_finish_transition(void);

MM_Status MM_get_status(void);

MD_ModeID MM_get_previous_mode_id(void);

MD_ModeID MM_get_current_mode_id(void);

int Cmd_START_TRANSITION(const CTCP* packet);
int Cmd_FINISH_TRANSITION(const CTCP* packet);
int Cmd_SET_MODE_LIST(const CTCP* packet);
int Cmd_SET_TRANSITION_TABLE(const CTCP* packet);

#endif // MODE_MANAGER_H_
