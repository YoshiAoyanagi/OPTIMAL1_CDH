// 
// 
// 

#include "TL_CmdDispatcher.h"
#include <string.h> // for memcpy

#include "utility.h"
#include "PacketHandler.h"
#include "TimeManager.h"

static CTCP null_packet_;
const CTCP* tl_list_for_tlm[TL_CMD_LIST_MAX];

static void tl_cmd_dispatcher_init_(void);
static void tl_cmd_dispatcher_(void);
static PH_ACK drop_tl_cmd_at_(cycle_t time);
static void update_tl_list_for_tlm_(void);

AppInfo tl_cmd_dispatcher(void)
{
	return create_app_info("tlcmd", 5, tl_cmd_dispatcher_init_, tl_cmd_dispatcher_);
}

static void tl_cmd_dispatcher_init_(void)
{
	PL_clear_list(&tl_cmd_list);

	memset(&null_packet_, 0, sizeof(null_packet_));
	update_tl_list_for_tlm_();
}

static void tl_cmd_dispatcher_(void)
{
	CTCP packet;
	PL_ACK ack = PL_check_tl_cmd(&tl_cmd_list,
		(size_t)master_clock->master);
	switch (ack)
	{
	case PL_TLC_ON_TIME: /* FALL THROUGH */
	case PL_TLC_PAST_TIME:
		packet = PL_get_head(&tl_cmd_list)->packet;
		PL_drop_executed(&tl_cmd_list);
		CCP_set_cmd_exe_type(&packet, CCP_REALTIME); // TimeLine -> RealTime
		PH_despatch_command(&packet);
		break;
	case PL_TLC_NOT_YET:
		break;
	default:
		break;
	}
}

int Cmd_CLEAR_ALL_TIMELINE(const CTCP* packet)
{
	if (CCP_get_length(packet) != 11) // コマンドデータ長確認
	{
		// コマンドはパケットヘッダのみ、パラメータなし
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	PL_clear_list(&tl_cmd_list);

	return CMD_ERROR_NONE;
}

int Cmd_CLEAR_TIMELINE_AT(const CTCP* packet)
{
	cycle_t time;
	if (CCP_get_length(packet) != 11 + 4) // コマンドデータ長確認
	{
		// パラメータはTime Indicator 4Bytes.
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	// 書き込み先アドレスをコピー（パラメータの先頭4Bytes）
	memcpy(&time, CCP_get_cmd_param(packet), sizeof(time));

	if (drop_tl_cmd_at_(time) == PH_SUCCESS) { return CMD_ERROR_NONE; }
	else { return CMD_ERROR_ILLEGAL_PARAMETER; }
}

static PH_ACK drop_tl_cmd_at_(cycle_t time)
{
	PL_Node *pos = PL_get_head(&tl_cmd_list);

	if (pos == NULL) { return PH_TLC_NOT_FINED; }

	while (CCP_get_cmd_time(&(pos->packet)) != time)
	{
		if (pos->next == NULL) { return PH_TLC_NOT_FINED; }
		pos = pos->next;
	}
	PL_drop_node(&tl_cmd_list, pos);
	return PH_SUCCESS;
}

int Cmd_UPDATE_TIMELINE_TLM(const CTCP* packet)
{
	if (CCP_get_length(packet) != 11) // コマンドデータ長確認
	{
		// コマンドはパケットヘッダのみ、パラメータなし
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	update_tl_list_for_tlm_();
	return CMD_ERROR_NONE;
}

static void update_tl_list_for_tlm_(void)
{
	PL_Node *pos = PL_get_head(&tl_cmd_list);
	int i;

	// 全リスト内容をクリア
	for (i = 0; i<TL_CMD_LIST_MAX; ++i)
	{
		tl_list_for_tlm[i] = &null_packet_;
	}

	// 登録されているTLコマンドをリストに書き込み
	for (i = 0; pos != NULL; ++i)
	{
		tl_list_for_tlm[i] = &(pos->packet);
		pos = pos->next;
	}
}

int Cmd_DEPLOY_BLOCK(const CTCP* packet)
{
	size_t block_id = packet->data[CMD_POSITION_PARAM];
	PL_ACK ack = PL_deploy_block_cmd(&tl_cmd_list, BCT_get_bc(block_id), (size_t)master_clock->master);

	if (ack != PL_SUCCESS)
	{
		DEBUG_Serial.println("PL: Deploy failed!");
		return ack;
	}
	return CMD_ERROR_NONE;
}


