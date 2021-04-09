// 
// 
// 

#include "NormalTL.h"


#include "BlockCommandTable.h"
#include "BlockCommandDefinisions.h"
#include "AppRegistry.h"
#include "cmd_code_obc.h"

void BC_load_normal_mode(size_t pos)
{
	CTCP temp;
	unsigned char param;
	int N = 0;

	BCT_clear_block(pos);

	temp = CCP_form_app_cmd(0, AR_TLM_GENERATOR);//
	BCT_register_cmd(&temp);

	param = BC_AR_TLMCMD;
	temp = CCP_form_tl_cmd(1, CODE_Cmd_ROTATE_BLOCK, &param, 1u);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(2, AR_FRAM_TLC);//
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(3, AR_TL_CMD);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(4, AR_STX_TRANSMIT);
	BCT_register_cmd(&temp);

	param = BC_AR_PERIPHERAL;
	temp = CCP_form_tl_cmd(5, CODE_Cmd_ROTATE_BLOCK, &param, 1u);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(6, AR_EPS_UPDATE);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(7, AR_GS_DATA_HANDLER);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(8, AR_ST_TLM);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(9, AR_MSN_PACKET_HANDLER);
	BCT_register_cmd(&temp);

	BCT_activate_block();
}