// 
// 
// 

#include "cmd_action.h"
#include "tlm_frame.h"
#include "OBC_TlmGenerator.h"
#include "AutonomousControl.h"
#include "cmd_item.h"

int Cmd_SET_OBC_TIME(const CTCP* packet)
{
	int i;
	unsigned int cycle;

	for (i = 0; i<4; ++i)
	{
		cycle <<= 8;
		cycle += packet->data[CMD_POSITION_PARAM + i];
	}
	TMGR_set_master_cycle((cycle_t)cycle);
	TMGR_set_initial_utime(0);
	TDSP_resync_internal_counter();

	return CMD_ERROR_NONE;
}

int Cmd_SYNC_TIME(const CTCP* packet)
{
	int i;
	unsigned int gs_utime;

	for (i = 0; i<4; ++i)
	{
		gs_utime <<= 8;
		gs_utime += packet->data[CMD_POSITION_PARAM + i];
	}
	RTC_UTIME_SET(gs_utime);
	if (TMGR_sync_OBC_time(gs_utime) == -1)
		return CMD_ERROR_ILLEGAL_PARAMETER;
	return CMD_ERROR_NONE;
}

int Cmd_SYNC_OBC_TIME(const CTCP* packet)
{
	int i;
	unsigned int gs_utime;

	for (i = 0; i<4; ++i)
	{
		gs_utime <<= 8;
		gs_utime += packet->data[CMD_POSITION_PARAM + i];
	}
	if (TMGR_sync_OBC_time(gs_utime) == -1)
		return CMD_ERROR_ILLEGAL_PARAMETER;
	return CMD_ERROR_NONE;
}

int Cmd_SYNC_RTC_TIME(const CTCP* packet)
{
	int i;
	unsigned int gs_utime;

	for (i = 0; i<4; ++i)
	{
		gs_utime <<= 8;
		gs_utime += packet->data[CMD_POSITION_PARAM + i];
	}

	RTC_UTIME_SET(gs_utime);
	return CMD_ERROR_NONE;
}

int Cmd_SYNC_GPS_TIME(const CTCP* packet)
{
	return CMD_ERROR_NONE;
}

int Cmd_FRAM_SYNC_RTC_ENABLE(const CTCP* packet)
{
	unsigned char ena = packet->data[CMD_POSITION_PARAM];
	FRAM_Write_rtc_sync_ena((ENABLE_STS)ena);
	return CMD_ERROR_NONE;
}

int Cmd_SOFTWARE_RESET(const CTCP* packet)
{
	return CMD_ERROR_NONE;
}
int Cmd_FRAM_INITIALIZE(const CTCP* packet)
{
	static unsigned char d;
	
	if (d > 0)
	{
		FRAM_All_Initialize();
		d = 0;
	}
	else
		d++;

	return CMD_ERROR_NONE;
}

int Cmd_FRAM_CMD_INITIALIZE(const CTCP* packet)
{
	FRAM_TLC_All_Initialize();
	return CMD_ERROR_NONE;
}

int Cmd_FRAM_TLC_CLEAR_ALL(const CTCP* packet)
{
	FRAM_TLC_Clear_All();
	return CMD_ERROR_NONE;
}
int Cmd_FRAM_TLC_CLEAR_AT_TIME(const CTCP* packet)
{
	int i;
	unsigned int utime;

	for (i = 0; i<4; ++i)
	{
		utime <<= 8;
		utime += packet->data[CMD_POSITION_PARAM + i];
	}
	if(FRAM_TLC_Clear_At_Time(utime) == Error)
		return CMD_ERROR_ILLEGAL_PARAMETER;

	return CMD_ERROR_NONE;
}
int Cmd_FRAM_TLC_CLEAR_AT_NUMBER(const CTCP* packet)
{
	unsigned char p = packet->data[CMD_POSITION_PARAM];
	FRAM_TLC_Clear_At_Address((unsigned int)p);

	return CMD_ERROR_NONE;
}


int Cmd_FRAM_DEPLOY_BLOCK(const CTCP* packet)
{
	unsigned char bc_id = packet->data[CMD_POSITION_PARAM];
	FRAM_SBC_Deploy_Block(bc_id);
	return CMD_ERROR_NONE;
}
int Cmd_FRAM_REMOVE_BLOCK(const CTCP* packet)
{
	unsigned char bc_id = packet->data[CMD_POSITION_PARAM];
	FRAM_SBC_Remove_Block(bc_id);
	return CMD_ERROR_NONE;
}
int Cmd_FRAM_REMOVE_BLOCK_AT_POSITION(const CTCP* packet)
{
	unsigned char bc_id = packet->data[CMD_POSITION_PARAM];
	unsigned char pos = packet->data[CMD_POSITION_PARAM + 1];
	FRAM_SBC_Remove_Block_At_PS(bc_id, pos);
	return CMD_ERROR_NONE;
}
int Cmd_FRAM_REMOVE_ALL_BLOCK(const CTCP* packet)
{
	FRAM_SBC_Remove_ALL_Block();
	return CMD_ERROR_NONE;
}
int Cmd_FRAM_SET_BLOCK_POSITION(const CTCP* packet)
{
	unsigned char number = packet->data[CMD_POSITION_PARAM];
	unsigned char pos = packet->data[CMD_POSITION_PARAM + 1];

	if (FRAM_SBC_Set_Stored_BC_Number(number, pos) != Success)
		return CMD_ERROR_ILLEGAL_PARAMETER;

	return CMD_ERROR_NONE;
}

int Cmd_FRAM_ACTIVATE_BLOCK(const CTCP* packet)
{
	unsigned char bc_id = packet->data[CMD_POSITION_PARAM];
	unsigned char activate = packet->data[CMD_POSITION_PARAM + 1];

	FRAM_SBC_ACTIVATE(bc_id, activate);
	return CMD_ERROR_NONE;
}

int Cmd_FRAM_REQUEST_BC_INFO(const CTCP* packet)
{
	unsigned char p = packet->data[CMD_POSITION_PARAM];
	FRAM_SBC_Read_BlockInfo(p);
	
	OBC_TlmRegister(TLM_SBC);
	return CMD_ERROR_NONE;
}

int Cmd_FRAM_REQUEST_BC_PARAM(const CTCP* packet)
{
	unsigned char id = packet->data[CMD_POSITION_PARAM];
	unsigned char pos = packet->data[CMD_POSITION_PARAM + 1];

	FRAM_SBC_ExtractData(id, pos);

	OBC_TlmRegister(TLM_SBC_PARAM);
	return CMD_ERROR_NONE;
}


//AutonomousControl
int Cmd_AutoControl_set_restart_counter(const CTCP* packet)
{
	int i;
	unsigned short count;

	for (i = 0; i < 2; ++i)
	{
		count <<= 8;
		count += packet->data[CMD_POSITION_PARAM + i];
	}
	AutoControl_set_restart_non_cmd_counter(count);
	return CMD_ERROR_NONE;
}

int Cmd_AutoControl_set_action(const CTCP* packet)
{
	int i_ret;
	int i;
	unsigned short count;

	unsigned char id		= packet->data[CMD_POSITION_PARAM + 0];
	unsigned char bc_id		= packet->data[CMD_POSITION_PARAM + 1];
	unsigned char activate	=  packet->data[CMD_POSITION_PARAM + 2];

	for (i = 0; i < 2; ++i)
	{
		count <<= 8;
		count += packet->data[CMD_POSITION_PARAM + 3 + i];
	}


	i_ret = AutoControl_set_action(id, bc_id, activate, count);

	if(i_ret != 0)
		return CMD_ERROR_ILLEGAL_PARAMETER;
	return CMD_ERROR_NONE;
}

int Cmd_AutoControl_remove_action(const CTCP* packet)
{
	int i_ret;
	unsigned char id = packet->data[CMD_POSITION_PARAM + 0];
	
	i_ret = AutoControl_remove_action(id);

	if (i_ret != 0)
		return CMD_ERROR_ILLEGAL_PARAMETER;
	return CMD_ERROR_NONE;
}

int Cmd_AutoControl_remove_all_action(const CTCP* packet)
{
	AutoControl_remove_all_action();
	return CMD_ERROR_NONE;
}

int Cmd_AutoControl_execute(const CTCP* packet)
{
	AutoControl_execute();
	return CMD_ERROR_NONE;
}

int Cmd_ClearRebootCount(const CTCP* packet)
{
	auto_ctrl.restart_counter = 0;
	FRAM_Write_reboot_count(auto_ctrl.restart_counter);
	return CMD_ERROR_NONE;
}

int Cmd_RETURN_CMD_OBC_UTIME(const CTCP* packet)
{
	unsigned char cmd_id = packet->data[CMD_POSITION_PARAM];

	CCP_FT_ID to_id = CCP_get_cmd_from_id(packet);
	CTCP rtn_packet = send_obc_utime_to_any(to_id, cmd_id);

	RT_Cmd_Register(rtn_packet);

	return CMD_ERROR_NONE;
}

int Cmd_MSN_DR_ENABLE_OBC_TIMETAG(const CTCP* packet)
{
	unsigned char p1 = packet->data[CMD_POSITION_PARAM];
	unsigned char p2 = packet->data[CMD_POSITION_PARAM + 1];

	hk_dr.ena_timetag = p1;
	msn_dr.ena_timetag = p2;

	FRAM_Write_dr_obc_timetag_ena(p1);
	FRAM_Write_dr_msn_timetag_ena(p2);
	
	return CMD_ERROR_NONE;
}