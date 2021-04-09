// 
// 
// 

#include "SafeSL.h"


#include "BlockCommandTable.h"
#include "cmd_item.h"

void BC_load_any_to_safe(size_t pos)
{
	CTCP temp;

	BCT_clear_block(pos);

	temp = adcs_mode_standby();
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(2));
	BCT_register_cmd(&temp);
	
	temp = cmd_stx_off();
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(4));
	BCT_register_cmd(&temp);

	temp = pwr_off_safe();
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(8)); //SF‚Ípwr_off_safe‚Åˆê’UOFF(RESET)‚·‚é
	BCT_register_cmd(&temp);

	temp = cmd_stx_power(1);
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(20));
	BCT_register_cmd(&temp);

	temp = cmd_stx_bitrate(STX_BITRATE_INITIAL);
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(22));
	BCT_register_cmd(&temp);

	temp = mux_change_sf();
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(24));
	BCT_register_cmd(&temp);

	temp = cmd_stx_set_port(0);
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(26));
	BCT_register_cmd(&temp);

	temp = auto_control_execute();
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(28));
	BCT_register_cmd(&temp);

	temp = finish_transition();
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(30));
	BCT_register_cmd(&temp);

	BCT_activate_block();
}
