// 
// 
// 

#include "AppRegistry.h"
#include "AppManager.h"
#include "AppHeaders.h"

static AM_ACK add_application_(size_t id,
	AppInfo(*app_creator)(void));

void AR_load_initial_settings(void)
{
	add_application_(AR_NOP, AH_create_app);
	add_application_(AR_ANOMALY_HANDLER, AH_create_app);
	add_application_(AR_APP_STATU, AppStatus);
	add_application_(AR_UVC, UVC_create_app);
	add_application_(AR_TEMPERATURE_UPDATE, temperature_update);
	add_application_(AR_GS_DATA_HANDLER, gs_data_handler);
	add_application_(AR_TLM_GENERATOR, OBC_tlm_generator);
	add_application_(AR_RT_CMD, rt_cmd_dispatcher);
	add_application_(AR_TL_CMD, tl_cmd_dispatcher);
	add_application_(AR_FRAM_TLC, fram_tl_cmd_dispatcher);
	add_application_(AR_RT_TLM, rt_tlm_transmitter);
	add_application_(AR_ADCS_PACKET_HANDLER, ADCS_packet_handler);
	add_application_(AR_ADCS_CMD_FORWARD, ADCS_cmd_forward);
	add_application_(AR_MSN_PACKET_HANDLER, MSN_packet_handler);
	add_application_(AR_MSN_CMD_FORWARD, MSN_cmd_forward);
	add_application_(AR_STRX_UPDATE, STRX_update);
	add_application_(AR_EPS_UPDATE, EPS_update);
	add_application_(AR_IMU_UPDATE, IMU_update);
	add_application_(AR_RTC_UPDATE, RTC_update);
	add_application_(AR_ST_TLM, st_tlm_generator);
	add_application_(AR_PRINT_DBG_MESSAGE, print_debug_message);
	add_application_(AR_STX_TRANSMIT, STX_transmit);
	add_application_(AR_MSN_DR_ERASE_HANDLER, MSN_DR_erase_handler);
	add_application_(AR_TEMPERATURE_CH_CHANGE, temperature_ch_change);

}

static AM_ACK add_application_(size_t id, AppInfo(*app_creator)(void))
{
	AppInfo ai = app_creator();
	return AM_register_ai(id, &ai);
}