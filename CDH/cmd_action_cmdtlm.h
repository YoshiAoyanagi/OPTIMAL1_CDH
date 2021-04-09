// cmd_action_cmdtlm.h

#ifndef _CMD_ACTION_CMDTLM_h
#define _CMD_ACTION_CMDTLM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CommonTlmCmdPacket.h"
#include "CommonSerialReceiver.h"


int Cmd_TlmRequest_via_SF(const CTCP* packet);
int Cmd_Set_Fwd_TlmID_via_SF(const CTCP* packet);

int Cmd_TlmRequest(const CTCP* packet);
int Cmd_SET_TLM_INTERVAL(const CTCP* packet);
int Cmd_TLM_SET_MODE(const CTCP* packet);

int Cmd_MUX_UART_SET(const CTCP* packet);

int Cmd_OBC_DR_REPLAY_PARAM_SET(const CTCP* packet);
int Cmd_OBC_DR_REPLAY(const CTCP* packet);
int Cmd_OBC_DR_ERASE(const CTCP* packet);
int Cmd_OBC_DR_Replay_Stop(const CTCP* packet);
int Cmd_OBC_DR_RECORD(const CTCP* packet);
int Cmd_OBC_DR_INITIALIZE(const CTCP* packet);


int Cmd_MSN_DR_REPLAY_PARAM_SET(const CTCP* packet);
int Cmd_MSN_DR_REPLAY(const CTCP* packet);
int Cmd_MSN_DR_ERASE(const CTCP* packet);
int Cmd_MSN_DR_Replay_Stop(const CTCP* packet);
int Cmd_MSN_DR_RECORD_ENABLE(const CTCP* packet);
int Cmd_MSN_DR_INITIALIZE(const CTCP* packet);

int Cmd_OBC_DR_DIRECT_REPLAY(const CTCP* packet);
int Cmd_MSN_DR_DIRECT_REPLAY(const CTCP* packet);

//STRX
int Cmd_STRX_SET_BR(const CTCP* packet);
int Cmd_STRX_SET_TX_PWR(const CTCP* packet);
int Cmd_STRX_TX_ON(const CTCP* packet);
int Cmd_STRX_OTC_ENABLE(const CTCP* packet);
int Cmd_STRX_OTC_SET_THRESHOLD(const CTCP* packet);


int Cmd_SET_STX_ONOFF_DURATION(const CTCP* packet);
int Cmd_SET_STX_BURST_DURATION(const CTCP* packet);

int Cmd_OBC_BIN_DATA_UPLOAD(const CTCP* packet);

int Cmd_STX_SET_OUTPORT(const CTCP* packet);
int Cmd_STX_SET_MOD(const CTCP* packet);
int Cmd_STX_SET_PN_TEST(const CTCP* packet);


int Cmd_SET_STRX_LOCK_REPLY_TIME(const CTCP* packet);

//G2
int Cmd_SET_OBCAM_DATA_INTERVAL(const CTCP* packet);

int Cmd_OBC_BIN_DATA_UPLOAD_BYTE_SHIFT(const CTCP* packet);

int Cmd_SET_TLMD_ANOMALY_ENABLE(const CTCP* packet);
#endif