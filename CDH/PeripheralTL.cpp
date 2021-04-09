// 
// 
// 

#include "PeripheralTL.h"
#include "BlockCommandTable.h"
#include "BlockCommandDefinisions.h"
#include "AppRegistry.h"
#include "cmd_code_obc.h"

void BC_load_peripheral_task(size_t pos)
{
	CTCP temp;
	unsigned char param;

	BCT_clear_block(pos);
	
	temp = CCP_form_app_cmd(0, AR_STRX_UPDATE);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(2, AR_IMU_UPDATE);
	BCT_register_cmd(&temp);

	temp = CCP_form_app_cmd(4, AR_ANOMALY_HANDLER);
	BCT_register_cmd(&temp);
	
	temp = CCP_form_app_cmd(5, AR_UVC);
	BCT_register_cmd(&temp);
	
	temp = CCP_form_app_cmd(6, AR_RTC_UPDATE);
	BCT_register_cmd(&temp);
	
	temp = CCP_form_app_cmd(7, AR_TEMPERATURE_UPDATE);
	BCT_register_cmd(&temp);
	
	temp = CCP_form_app_cmd(8, AR_PRINT_DBG_MESSAGE);
	BCT_register_cmd(&temp);

	BCT_activate_block();
}