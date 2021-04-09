// cmd_item.h

#ifndef _CMD_ITEM_h
#define _CMD_ITEM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CommonTlmCmdPacket.h"
#include "cmd_code_obc.h"
#include "ModeDefinisions.h"

typedef enum
{
	ADCS_UNC
}ADCS_MODE;

void RT_Cmd_Register(CTCP packet);
CTCP start_transition(MD_ModeID mode);
CTCP finish_transition(void);

CTCP adcs_on(void);
CTCP adcs_off(void);
CTCP pwr_off_safe(void);
CTCP cmd_adcs_set_mode(ADCS_MODE mode);
CTCP cmd_power_reboot(void);
CTCP cmd_stx_on(void);
CTCP cmd_stx_off(void);
CTCP cmd_stx_set_duty(unsigned short on_dur, unsigned short off_dur);
CTCP cmd_stx_power(unsigned char pwr);
CTCP cmd_stx_bitrate(unsigned char bps);
CTCP cmd_stx_set_port(unsigned char port);

CTCP msn_unreg_on(void);
CTCP msn_unreg_off(void);

CTCP sf_on(void);
CTCP sf_off(void);
CTCP subcam_on(void);
CTCP subcam_off(void);
CTCP msn5v_on(void);
CTCP msn5v_off(void);

CTCP st_block_cmd_deploy(unsigned char id);
CTCP cmd_obc_dr_initialize(void);
CTCP cmd_obc_dr_record_start(unsigned short interval);

//ADCSŒü‚¯ƒRƒ}ƒ“ƒh
CTCP adcs_rw_stop(unsigned char ch);
CTCP adcs_rw_off(void);
CTCP adcs_mtq_off(void);
CTCP adcs_mode_standby(void);


CTCP ah_activate_rule(unsigned char id);

//auto execute
CTCP auto_control_execute(void);

CTCP mux_change_sf(void);

CTCP send_obc_utime_to_any(CCP_FT_ID to_id, unsigned char cmd_id);

#endif

