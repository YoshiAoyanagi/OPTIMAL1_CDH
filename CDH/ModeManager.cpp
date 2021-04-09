// 
// 
// 

#include "ModeManager.h"

#include "TimeManager.h"
#include "TaskDispatcher.h"
#include "AnomalyLogger.h"
#include "CommonTlmCmdPacket.h"
#include "PacketHandler.h"
#include "FRAM_manager.h"

static void deploy_block_cmd_(size_t index);

static ModeManagerInfo ops_mode_;
const ModeManagerInfo* ops_mode;

void MM_initialize(void)
{
	MD_load_mode_list(ops_mode_.mode_list);
	MD_load_transition_table(ops_mode_.transition_table);

	MD_ModeID current_mode;
	MD_ModeID prev_mode;

	FRAM_Read_ops_mode(&current_mode, &prev_mode);

	ops_mode_.stat = MM_FINISHED;
	ops_mode_.previous_id = prev_mode;
	ops_mode_.current_id = current_mode;

	Serial.print("OPS_MODE: "); Serial.println(ops_mode_.current_id);

	if (ops_mode_.previous_id > MODE_MAX || ops_mode_.transition_table[SAFE][ops_mode_.current_id] == MM_NOT_DEFINED)
	{
		DEBUG_Serial.println("MM_NOT_DEFINED_MODE");
		ops_mode_.previous_id = SAFE;
		ops_mode_.current_id = SAFE;
	}
	MM_start_transition(ops_mode_.current_id);

	ops_mode = &ops_mode_;
}

MM_ACK MM_set_mode_list(MD_ModeID mode,
	size_t bc_index)
{
	if (mode >= MODE_MAX)
	{
		return MM_BAD_ID;
	}

	ops_mode_.mode_list[mode] = bc_index;

	return MM_SUCCESS;
}

MM_ACK MM_set_transition_table(MD_ModeID from,
	MD_ModeID to,
	size_t bc_index)
{
	if ((from >= MODE_MAX) || (to >= MODE_MAX))
	{
		return MM_BAD_ID;
	}
	else if (bc_index > MM_NOT_DEFINED)
	{
		return MM_BAD_BC_INDEX;
	}

	ops_mode_.transition_table[from][to] = bc_index;

	return MM_SUCCESS;
}

MM_ACK MM_start_transition(MD_ModeID id)
{
	size_t bc_index = MM_NOT_DEFINED;

	if (id >= MODE_MAX)
	{
		AL_add_anomaly(AL_MODE_MANAGER, MM_BAD_ID);
		return MM_BAD_ID;
	}
	else if (ops_mode_.stat != MM_FINISHED)
	{
		AL_add_anomaly(AL_MODE_MANAGER, MM_OVERWRITE);
		return MM_OVERWRITE;
	}

	bc_index = ops_mode_.transition_table[ops_mode_.current_id][id];
	if (bc_index != MM_NOT_DEFINED)
	{
		TMGR_clear_mode_cycle();
		deploy_block_cmd_(ops_mode_.transition_table[ops_mode_.current_id][id]);
		ops_mode_.previous_id = ops_mode_.current_id;
		ops_mode_.current_id = id;
		ops_mode_.stat = MM_IN_PROGRESS;
		FRAM_Write_ops_mode(ops_mode_);	//FRAMにモード保存
	}
	else
	{
		AL_add_anomaly(AL_MODE_MANAGER, MM_ILLEGAL_MOVE);
		return MM_ILLEGAL_MOVE;
	}

	return MM_SUCCESS;
}

MM_ACK MM_finish_transition(void)
{
	TDSP_ACK ack = TDSP_UNKNOWN;

	if (ops_mode_.stat != MM_IN_PROGRESS)
	{
		AL_add_anomaly(AL_MODE_MANAGER, MM_NOT_IN_PROGRESS);
		return MM_NOT_IN_PROGRESS;
	}

	TMGR_clear_mode_cycle();
	ack = TDSP_set_task_list_id(ops_mode_.mode_list[ops_mode_.current_id]);
	switch (ack)
	{
	case TDSP_SUCCESS:
		break;
	default:
		AL_add_anomaly(AL_MODE_MANAGER, MM_TL_LOAD_FAILED);
		break;
	}
	ops_mode_.stat = MM_FINISHED;

	return MM_SUCCESS;
}

MM_Status MM_get_status(void) { return ops_mode_.stat; }

MD_ModeID MM_get_previous_mode_id(void) { return ops_mode_.previous_id; }

MD_ModeID MM_get_current_mode_id(void) { return ops_mode_.current_id; }

static void deploy_block_cmd_(size_t bc_index)
{
	unsigned char param = (unsigned char)bc_index;
	CTCP packet = CCP_form_rt_cmd(CODE_Cmd_DEPLOY_BLOCK, &param, 1u);
	PH_despatch_command(&packet);
}

int Cmd_START_TRANSITION(const CTCP* packet)
{
	MD_ModeID id = (MD_ModeID)packet->data[CMD_POSITION_PARAM];

	if (id >= MODE_MAX)
	{
		DEBUG_Serial.println("MODE: Invalid mode!");
		return CMD_ERROR_ILLEGAL_PARAMETER;
	}
	else
	{
		MM_start_transition(id);
	}
	return CMD_ERROR_NONE;
}

int Cmd_FINISH_TRANSITION(const CTCP* packet)
{
	ops_mode_.stat = MM_FINISHED;

	MM_ACK ack = MM_SUCCESS;
	if (CCP_get_length(packet) != 11) // コマンドデータ長確認
	{
		// コマンドはパケットヘッダのみ、パラメータなし
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	ack = MM_finish_transition();
	if (ack != MM_SUCCESS)
	{
		return CMD_ERROR_ILLEGAL_CONTEXT;
	}
	return CMD_ERROR_NONE;
}

int Cmd_SET_MODE_LIST(const CTCP* packet)
{
	MM_ACK ack;

	unsigned char mode, bc_index;
	// パラメータはパケットヘッダとuint8_t 2個（mode, index)。
	if (CCP_get_length(packet) != (11 + 2))
	{
		return CMD_ERROR_ILLEGAL_LENGTH;
	}
	mode = CCP_get_cmd_param(packet)[0];
	bc_index = CCP_get_cmd_param(packet)[1];

	ack = MM_set_mode_list((MD_ModeID)mode, bc_index);
	if (ack != MM_SUCCESS) { CMD_ERROR_ILLEGAL_PARAMETER; }
	return CMD_ERROR_NONE;
}

int Cmd_SET_TRANSITION_TABLE(const CTCP* packet)
{
	unsigned char from, to, bc_index;
	MM_ACK ack = MM_SUCCESS;

	if (CCP_get_length(packet) != (11 + 3))
	{
		// コマンドはパケットヘッダとuint8_t 3個（from, to, index)。
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	from = CCP_get_cmd_param(packet)[0];
	to = CCP_get_cmd_param(packet)[1];
	bc_index = CCP_get_cmd_param(packet)[2];

	ack = MM_set_transition_table((MD_ModeID)from,
		(MD_ModeID)to,
		(size_t)bc_index);
	if (ack != MM_SUCCESS)
	{
		return CMD_ERROR_ILLEGAL_PARAMETER;
	}

	return CMD_ERROR_NONE;
}
