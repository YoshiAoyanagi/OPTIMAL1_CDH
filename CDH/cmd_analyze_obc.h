#ifndef CMD_ANALYZE_INCLUDE_
#define CMD_ANALYZE_INCLUDE_

#include "cmd_code_obc.h"
#include "Utility.h"
#include "CommonTlmCmdPacket.h"
#include "PacketHandler.h"
#include "AnomalyHandler.h"
#include "AppStatus.h"

#include "nop.h"
#include "ModeManager.h"
#include "AppManager.h"
#include "TaskDispatcher.h"

#include "cmd_action.h"
#include "cmd_action_cmdtlm.h"
#include "cmd_action_power.h"
#include "cmd_action_adcs.h"

#define		CMD_REC_STATUS_0	0
#define		CMD_REC_STATUS_1	1
#define		CMD_REC_STATUS_2	2
#define		CMD_REC_STATUS_3	3

#define		CMD_ERROR_NONE					0x00
#define		CMD_ERROR_NOT_DEFINED			0xff
#define		CMD_ERROR_CRC					0xfe
#define		CMD_ERROR_ILLEGAL_LENGTH		0xfd
#define		CMD_ERROR_ILLEGAL_PARAMETER		0xfc
#define		CMD_ERROR_ILLEGAL_CONTEXT		0xe0
#define		CMD_ERROR_CONFLICT				0xe1
#define		CMD_ERROR_TO_ERROR				0xe2
#define		CMD_ERROR_TLMCMD_ERROR			0xe3

#define		CMD_STATUS_NONE					0x00
#define		CMD_STATUS_ACC					0x01	//ACPT
#define		CMD_STATUS_START				0x02	//EXEC
#define		CMD_STATUS_END					0x03	//CMPL
#define		CMD_STATUS_ERROR_CMD			0xFE
#define		CMD_STATUS_ERROR_EXE			0xFF

#define CMD_DATA_MAX 256//128

int cmdExec(const CTCP *packet);

extern  int       CmdSts;
extern  int       CmdCode;
extern  int       CmdCount;
extern  int       CmdErrorSts;
extern  int       CmdLength;
extern  int       CmdTo;
extern  int       CmdTlmCmd;

#endif // CMD_ANALYZE_INCLUDE_
