// 
// 
// 

#include "InitialSL.h"

#include "BlockCommandTable.h"
#include "cmd_item.h"

void BC_load_any_to_initial(size_t pos)
{

	CTCP temp;

	BCT_clear_block(pos);

	temp = pwr_off_safe();
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(1));
	BCT_register_cmd(&temp);

	temp = cmd_stx_set_duty(1, 1800);	//dutyïœçX(1, 1800)
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(2));
	BCT_register_cmd(&temp);
	
	temp = cmd_stx_bitrate(STX_BITRATE_INITIAL);
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(66));
	BCT_register_cmd(&temp);

	temp = cmd_stx_set_duty(5, 60);
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(1800));
	BCT_register_cmd(&temp);

	temp = finish_transition();
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(1805));
	BCT_register_cmd(&temp);

	//Transition to Safe mode
	temp = start_transition(SAFE);
	CCP_convert_rt_cmd_to_tl_cmd(&temp, OBCT_sec2cycle(1810));
	BCT_register_cmd(&temp);

	BCT_activate_block();
}

