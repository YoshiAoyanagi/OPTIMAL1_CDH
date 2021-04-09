// LoRaCmd_handler.h

#ifndef _LORACMD_HANDLER_h
#define _LORACMD_HANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CommonTlmCmdPacket.h"

//#define	LORA_DEBUG_MSG

#define SF_ID_FULL_CMD	0xFF
#define SF_ID_SHORT_CMD	0xFA
#define SF_ID_SHORT_TLC	0xFB

//Position
#define SF_CMD_LENGTH_POSITION	0
#define SF_CMD_ID_POSITION		1
#define SF_CMD_TO_ID_POSITION	2
#define SF_CMD_DATA_POSITION	3

//SEQ
#define SF_CMD_SEQ_ID_POSITION		3
#define SF_CMD_SEQ_CNT_POSITION		4
#define SF_CMD_SEQ_DATA_POSITION	5

//SHORT
#define SF_CMD_SHORT_PBYTE_POSITION		3
#define SF_CMD_SHORT_TO_ID_POSITION		4
#define SF_CMD_SHORT_CMD_ID_POSITION	5
#define SF_CMD_SHORT_PARAM_POSITION		6

//SHORT TLC
#define SF_CMD_SHORT_TLC_TIME_POSITION		6
#define SF_CMD_SHORT_TLC_PARAM_POSITION		10

typedef struct
{
	unsigned char length;
	unsigned char id;
	unsigned char seq_id;
	unsigned char seq_count;
	unsigned char error_sts;
}LoRaCmdSts;

typedef enum
{
	LR_PROCESSING,
	LR_SUCCESS,
	LR_LENGTH_ERROR,
	LR_CRC_ERROR,
	LR_INVALID_SEQ_ID
}LoRaError;

typedef struct
{
	unsigned char fwd_tlm_id[4];
	unsigned char fwd_enabled;
	unsigned char fwd_count;
}LORA_FWD_STRUCT;


extern LoRaCmdSts lora_cmd_sts;
extern LORA_FWD_STRUCT lora_fwd_tlm;

void LoRaCmdProcess(unsigned char* data, unsigned char length);
int LoRa_ObcTlmSend(unsigned char id);
void LoRa_TlmForward(CTCP packet);

#endif

