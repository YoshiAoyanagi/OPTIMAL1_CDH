// 
// 
// 

#include "FRAM_TL_CmdDispatcher.h"


#include <string.h> // for memcpy

#include "utility.h"
#include "PacketHandler.h"
#include "TimeManager.h"
#include "FRAM_cmd_manager.h"
#include "PacketList.h"

static void fram_tl_cmd_dispatcher_init_(void);
static void fram_tl_cmd_dispatcher_(void);

AppInfo fram_tl_cmd_dispatcher(void)
{
	return create_app_info("ftlcmd", 5, fram_tl_cmd_dispatcher_init_, fram_tl_cmd_dispatcher_);
}

static void fram_tl_cmd_dispatcher_init_(void)
{
	FRAM_TLC_Read_queue();
	FRAM_TLC_Read_tlc_time();
}

static void fram_tl_cmd_dispatcher_(void)
{
	unsigned char rec_data[CMD_DATA_MAX];
	int  rec_len = CMD_DATA_MAX;
	int i;
	static PE_Info cmd_pei;

	if (FRAM_TLC_search_tlc(TMGR_get_utime(), rec_data) != PL_TLC_NOT_YET)
	{
		PH_extract_packet(rec_data, rec_len, &cmd_pei);
	}
}
