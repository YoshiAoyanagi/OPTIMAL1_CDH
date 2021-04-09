// 
// 
// 

#include "AppManager.h"


#include "AppManager.h"

#include <string.h> 
#include "AnomalyLogger.h"
#include "TimeManager.h"
#include "cmd_analyze_obc.h"
#include "nop.h"

static AMInfo ami_;
const AMInfo *ami;

void AM_initialize(void)
{
	int i;
	for (i = 0; i<AM_MAX_APPS; ++i)
	{
		ami_.ais[i] = create_nop_task();
	}

	ami = &ami_;
}

AM_ACK AM_register_ai(size_t id,
	const AppInfo* ai)
{
	if (id >= AM_MAX_APPS)
	{
		AL_add_anomaly(AL_APP_MANAGER, AM_INVALID_ID);
		return AM_INVALID_ID;
	}

	ami_.ais[id] = *ai;
	return AM_SUCCESS;
}

void AM_initialize_all_apps(void)
{
	size_t i;
	for (i = 0; i<AM_MAX_APPS; ++i) { AM_initialize_app(i);}
}

AM_ACK AM_initialize_app(size_t id)
{
	if (id >= AM_MAX_APPS)
	{
		AL_add_anomaly(AL_APP_MANAGER, AM_INVALID_ID);
		return AM_INVALID_ID;
	}
	else if (ami_.ais[id].initializer == NULL)
	{
		return AM_NOT_REGISTERED;
	}
	ami_.ais[id].initializer();
	return AM_SUCCESS;
}

AM_ACK AM_execute_app(size_t id)
{
	OBCTime start, finish;

	if (id >= AM_MAX_APPS)
	{
		AL_add_anomaly(AL_APP_MANAGER, AM_INVALID_ID);
		return AM_INVALID_ID;
	}
	else if (ami_.ais[id].entry_point == NULL)
	{
		return AM_NOT_REGISTERED;
	}

	start = *master_clock;
	ami_.ais[id].entry_point();
	finish = *master_clock;

	// 処理時間情報アップデート
	ami_.ais[id].prev = OBCT_diff_in_step(&start, &finish);
	if (ami_.ais[id].max < ami_.ais[id].prev) { ami_.ais[id].max = ami_.ais[id].prev; }
	if (ami_.ais[id].min > ami_.ais[id].prev) { ami_.ais[id].min = ami_.ais[id].prev; }

	return AM_SUCCESS;
}

int Cmd_REGISTER_APP(const CTCP* packet)
{
	const unsigned char* param;
	size_t id;
	AppInfo ai;

	// コマンド長確認 4x4Bytes (id, proc_time, initializer, entry_point)
	if (CCP_get_length(packet) != 11 + 16)
	{
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	param = CCP_get_cmd_param(packet);
	memcpy(&id, param, 4);
	memcpy(&ai.proc_time, param + 4, 4);
	memcpy(&ai.initializer, param + 8, 4);
	memcpy(&ai.entry_point, param + 12, 4);

	ai.name = "SPECIAL";
	ai.prev = 0;
	ai.max = 0;
	ai.min = 0xffffffff;

	return AM_register_ai(id, &ai);
}

int Cmd_INITIALIZE_APP(const CTCP* packet)
{
	size_t id = AM_MAX_APPS;
	AM_ACK ack = AM_UNKNOWN;

	// コマンドデータ長確認
	if (CCP_get_length(packet) != (11 + sizeof(size_t)))
	{
		// データ長はヘッダ＋4Byte
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	memcpy(&id, CCP_get_cmd_param(packet), sizeof(size_t));

	ack = AM_initialize_app(id);
	if (ack == AM_SUCCESS) { return CMD_ERROR_NONE; }
	else { return CMD_ERROR_ILLEGAL_CONTEXT; }
}

int Cmd_EXECUTE_APP(const CTCP* packet)
{
	size_t id = AM_MAX_APPS;
	AM_ACK ack = AM_UNKNOWN;

	// コマンドデータ長確認
	if (CCP_get_length(packet) != (11 + sizeof(size_t)))
	{
		// データ長はヘッダ＋4Byte
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	memcpy(&id, CCP_get_cmd_param(packet), sizeof(size_t));

	ack = AM_execute_app(id);
	if (ack == AM_SUCCESS) { return CMD_ERROR_NONE; }
	else { return CMD_ERROR_ILLEGAL_CONTEXT; }
}
