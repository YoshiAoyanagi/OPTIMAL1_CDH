// 
// 
// 

#include "RT_CmdDispatcher.h"
#include "PacketHandler.h"

static void rt_cmd_dispatcher_init_(void);
static void rt_cmd_dispatcher_(void);

AppInfo rt_cmd_dispatcher(void)
{
	return create_app_info("rtcmd", 5, rt_cmd_dispatcher_init_, rt_cmd_dispatcher_);
}

static void rt_cmd_dispatcher_init_(void)
{
	PL_clear_list(&rt_cmd_list);
}

static void rt_cmd_dispatcher_(void)
{
	// 実行すべきコマンドなし
	if (PL_is_empty(&rt_cmd_list)) { return; }

	PH_despatch_command(&(PL_get_head(&rt_cmd_list)->packet));
	PL_drop_executed(&rt_cmd_list);
}

int Cmd_CLEAR_ALL_REALTIME(const CTCP* packet)
{
	if (CCP_get_length(packet) != 11) // コマンドデータ長確認
	{
		// コマンドはパケットヘッダのみ、パラメータなし
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	PL_clear_list(&rt_cmd_list);

	return CMD_ERROR_NONE;
}
