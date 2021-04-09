// cmd_action.h

#ifndef _CMD_ACTION_h
#define _CMD_ACTION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CommonTlmCmdPacket.h"
#include "CommonSerialReceiver.h"
#include "cmd_analyze_obc.h"
#include "RTC.h"
#include "FRAM_manager.h"
#include "FRAM_cmd_manager.h"

int Cmd_SYNC_TIME(const CTCP* packet);
int Cmd_SYNC_OBC_TIME(const CTCP* packet);
int Cmd_SYNC_RTC_TIME(const CTCP* packet);
int Cmd_FRAM_SYNC_RTC_ENABLE(const CTCP* packet);

int Cmd_SYNC_GPS_TIME(const CTCP* packet);	//**

int Cmd_SET_OBC_TIME(const CTCP* packet);
int Cmd_SOFTWARE_RESET(const CTCP* packet);
int Cmd_FRAM_INITIALIZE(const CTCP* packet);
int Cmd_FRAM_CMD_INITIALIZE(const CTCP* packet);


int Cmd_FRAM_TLC_CLEAR_ALL(const CTCP* packet);
int Cmd_FRAM_TLC_CLEAR_AT_TIME(const CTCP* packet);
int Cmd_FRAM_TLC_CLEAR_AT_NUMBER(const CTCP* packet);

int Cmd_FRAM_DEPLOY_BLOCK(const CTCP* packet);
int Cmd_FRAM_SET_BLOCK_POSITION(const CTCP* packet);
int Cmd_FRAM_ACTIVATE_BLOCK(const CTCP* packet);
int Cmd_FRAM_REQUEST_BC_INFO(const CTCP* packet);
int Cmd_FRAM_REQUEST_BC_PARAM(const CTCP* packet);

int Cmd_FRAM_REMOVE_BLOCK(const CTCP* packet);
int Cmd_FRAM_REMOVE_BLOCK_AT_POSITION(const CTCP* packet);
int Cmd_FRAM_REMOVE_ALL_BLOCK(const CTCP* packet);



//Autonoumas
int Cmd_AutoControl_set_restart_counter(const CTCP* packet);
int Cmd_AutoControl_set_action(const CTCP* packet);
int Cmd_AutoControl_remove_action(const CTCP* packet);
int Cmd_AutoControl_remove_all_action(const CTCP* packet);
int Cmd_AutoControl_execute(const CTCP* packet);
int Cmd_ClearRebootCount(const CTCP* packet);

//add(2021/03/11
int Cmd_RETURN_CMD_OBC_UTIME(const CTCP* packet);
int Cmd_MSN_DR_ENABLE_OBC_TIMETAG(const CTCP* packet);

#endif