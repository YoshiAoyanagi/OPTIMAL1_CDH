// 
// 
// 

#include "TlmCmdTL.h"

#include "BlockCommandTable.h"
#include "BlockCommandDefinisions.h"
#include "AppRegistry.h"
#include "cmd_code_obc.h"

void BC_load_tlmcmd_task(size_t pos)
{
	CTCP temp;
	unsigned char param;

	BCT_clear_block(pos);

	temp = CCP_form_app_cmd(1, AR_RT_CMD);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(2, AR_MSN_CMD_FORWARD);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(4, AR_ADCS_PACKET_HANDLER);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(5, AR_ADCS_CMD_FORWARD);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(7, AR_MSN_DR_ERASE_HANDLER);
	BCT_register_cmd(&temp);

	BCT_activate_block();
}