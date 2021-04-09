// 
// 
// 

#include "cmd_item.h"
#include "PacketHandler.h"


void RT_Cmd_Register(CTCP packet)
{
	PH_analyze_packet(&packet);
}

CTCP start_transition(MD_ModeID mode)
{
	unsigned char param[1];
	param[0] = (unsigned char)mode;
	return CCP_form_rt_cmd(CODE_Cmd_START_TRANSITION, param, 1u);
}

CTCP finish_transition(void)
{
	return CCP_form_rt_cmd(CODE_Cmd_FINISH_TRANSITION, NULL, 0u);
}

CTCP adcs_on(void)
{
	unsigned char param[1];
	param[0] = PWR_ON;
	return CCP_form_rt_cmd(CODE_Cmd_ADCS_POWER, param, 1u);
}
CTCP adcs_off(void)
{
	unsigned char param[1];
	param[0] = PWR_OFF;
	return CCP_form_rt_cmd(CODE_Cmd_ADCS_POWER, param, 1u);
}
//
CTCP sf_on(void)
{
	unsigned char param[1];
	param[0] = PWR_ON;
	return CCP_form_rt_cmd(CODE_Cmd_SF_POWER, param, 1u);
}
CTCP sf_off(void)
{
	unsigned char param[1];
	param[0] = PWR_OFF;
	return CCP_form_rt_cmd(CODE_Cmd_SF_POWER, param, 1u);
}

CTCP subcam_on(void)
{
	unsigned char param[1];
	param[0] = PWR_ON;
	return CCP_form_rt_cmd(CODE_Cmd_SUBCAM_POWER, param, 1u);
}
CTCP subcam_off(void)
{
	unsigned char param[1];
	param[0] = PWR_OFF;
	return CCP_form_rt_cmd(CODE_Cmd_SUBCAM_POWER, param, 1u);
}

CTCP msn5v_on(void)
{
	unsigned char param[1];
	param[0] = PWR_ON;
	return CCP_form_rt_cmd(CODE_Cmd_MSN_5V_POWER, param, 1u);
}
CTCP msn5v_off(void)
{
	unsigned char param[1];
	param[0] = PWR_OFF;
	return CCP_form_rt_cmd(CODE_Cmd_MSN_5V_POWER, param, 1u);
}


CTCP pwr_off_safe(void)
{
	return CCP_form_rt_cmd(CODE_Cmd_POWER_SAFE_OFF, NULL, 0u);
}
CTCP cmd_adcs_set_mode(ADCS_MODE mode)
{
	return CCP_form_rt_cmd(CODE_Cmd_FINISH_TRANSITION, NULL, 0u);
}

CTCP cmd_power_reboot(void)
{
	return CCP_form_rt_cmd(CODE_Cmd_POWER_REBOOT, NULL, 0u);
}

CTCP cmd_stx_on(void)
{
	unsigned char param[1];
	param[0] = PWR_ON;
	return CCP_form_rt_cmd(CODE_Cmd_STRX_TX_ON, param, 1u);
}

CTCP cmd_stx_set_duty(unsigned short on_dur, unsigned short off_dur)
{
	unsigned char param[4];
	param[0] = (on_dur >> 8) & 0xFF;
	param[1] = (on_dur >> 0) & 0xFF;

	param[2] = (off_dur >> 8) & 0xFF;
	param[3] = (off_dur >> 0) & 0xFF;

	return CCP_form_rt_cmd(CODE_Cmd_SET_STX_ONOFF_DURATION, param, 4u);
}


CTCP cmd_stx_off(void)
{
	unsigned char param[1];
	param[0] = PWR_OFF;
	return CCP_form_rt_cmd(CODE_Cmd_STRX_TX_ON, param, 1u);
}

CTCP cmd_stx_power(unsigned char pwr)
{
	unsigned char param[1];
	param[0] = pwr;
	return CCP_form_rt_cmd(CODE_Cmd_STRX_SET_TX_PWR, param, 1u);
}

CTCP cmd_stx_set_port(unsigned char port)
{
	unsigned char param[1];
	param[0] = port;
	return CCP_form_rt_cmd(CODE_Cmd_STX_SET_OUTPORT, param, 1u);
}

CTCP cmd_stx_bitrate(unsigned char bps)
{
	unsigned char param[1];
	param[0] = bps;
	return CCP_form_rt_cmd(CODE_Cmd_STRX_SET_BR, param, 1u);
}


CTCP st_block_cmd_deploy(unsigned char id)
{
	unsigned char param[1];
	param[0] = id;
	return CCP_form_rt_cmd(CODE_Cmd_FRAM_DEPLOY_BLOCK, param, 1u);
}

CTCP cmd_obc_dr_initialize(void)
{
	return CCP_form_rt_cmd(CODE_Cmd_OBC_DR_INITIALIZE, NULL, 0u);
}

CTCP cmd_obc_dr_record_start(unsigned short interval)
{
	unsigned char param[3];
	param[0] = 1;
	param[1] = (interval >> 8) & 0xff;
	param[2] = (interval >> 0) & 0xff;

	return CCP_form_rt_cmd(CODE_Cmd_OBC_DR_RECORD, param, 3u);
}

CTCP adcs_rw_stop(unsigned char ch)
{
	unsigned char param[5];
	CTCP packet;
	param[0] = ch;

	param[1] = 0;
	param[2] = 0;
	param[3] = 0;
	param[4] = 0;

	packet = CCP_form_rt_cmd(0x20, param, 5u);
	CCP_set_cmd_to_id(&packet, FROM_TO_ADCS);
	return packet;
}

CTCP adcs_rw_off(void)
{
	unsigned char param[1];
	CTCP packet;
	param[0] = 0;

	packet = CCP_form_rt_cmd(0x16, param, 1u);
	CCP_set_cmd_to_id(&packet, FROM_TO_ADCS);
	return packet;
}

CTCP adcs_mtq_off(void)
{
	unsigned char param[1];
	CTCP packet;
	param[0] = 0;

	packet = CCP_form_rt_cmd(0x14, param, 1u);
	CCP_set_cmd_to_id(&packet, FROM_TO_ADCS);
	return packet;
}

CTCP adcs_mode_standby(void)
{
	unsigned char param[1];
	CTCP packet;
	param[0] = 0;

	packet = CCP_form_rt_cmd(0x30, param, 1u);
	CCP_set_cmd_to_id(&packet, FROM_TO_ADCS);
	return packet;
}

CTCP ah_activate_rule(unsigned char id)
{
	unsigned char param[1];
	param[0] = id;
	return CCP_form_rt_cmd(CODE_Cmd_AH_activate_rule, param, 1u);
}

CTCP auto_control_execute(void)
{
	return CCP_form_rt_cmd(CODE_Cmd_AutoControl_execute, NULL, 0u);
}

CTCP mux_change_sf(void)
{
	unsigned char param[1];
	param[0] = (unsigned char)MUX_SF;
	return CCP_form_rt_cmd(CODE_Cmd_MUX_UART_SET, param, 1U);
}


CTCP send_obc_utime_to_any(CCP_FT_ID to_id, unsigned char cmd_id)
{
	unsigned char param[4];
	CTCP packet;
	
	unsigned int now_time = TMGR_get_utime();

	param[0] = (unsigned char)((now_time >> 24) & 0xff);
	param[1] = (unsigned char)((now_time >> 16) & 0xff);
	param[2] = (unsigned char)((now_time >> 8) & 0xff);
	param[3] = (unsigned char)(now_time & 0xff);

	packet = CCP_form_rt_cmd(cmd_id, param, 4u);
	CCP_set_cmd_to_id(&packet, to_id);
	return packet;
}

