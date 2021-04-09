// cmd_action_power.h

#ifndef _CMD_ACTION_POWER_h
#define _CMD_ACTION_POWER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CommonTlmCmdPacket.h"
#include "cmd_analyze_obc.h"

int Cmd_EPS_RESET(const CTCP* packet);
int Cmd_EPS_SELECT(const CTCP *packet);
int Cmd_EPS_UVC_ENABLE(const CTCP* packet);
int Cmd_EPS_UVC_SET_BAT_V(const CTCP* packet);
int Cmd_ADCS_POWER(const CTCP* packet);
int Cmd_RW_POWER(const CTCP* packet);
int Cmd_HEATER_POWER(const CTCP* packet);
int Cmd_RW_ENABLE(const CTCP* packet);
int Cmd_MSN_unreg_ENABLE(const CTCP* packet);
int Cmd_MSN_5V_POWER(const CTCP* packet);
int Cmd_SUBCAM_POWER(const CTCP* packet);
int Cmd_SF_POWER(const CTCP* packet);
int Cmd_MTQ_POWER_OBC(const CTCP* packet);
int Cmd_POWER_SAFE_OFF(const CTCP* packet);
int Cmd_UNREG_POWER(const CTCP* packet);
int Cmd_POWER_REBOOT(const CTCP* packet);

int Cmd_STRX_POWER(const CTCP* packet);
int Cmd_SRX_RESET(const CTCP* packet);

int Cmd_AQU_5V_POWER(const CTCP* packet);
int Cmd_OVCO_5V_POWER(const CTCP* packet);

#endif

