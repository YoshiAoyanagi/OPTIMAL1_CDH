#ifndef PACKET_DESCRIPTION_H_
#define PACKET_DESCRIPTION_H_

#include <stddef.h> // for size_t
#include <stdlib.h>

#include <string.h> // for memcpy
#include "Config.h"
#include "Utility.h"
#include "TimeManager.h"
#include "AppRegistry.h"

#define CCP_MAX_PACKET_LENGTH (210)
#define CCP_MAX_PARAM_LENGTH (192)
#define CCP_STX_ETX_LENGTH (6)

#define		CMD_STX1	0xEB
#define		CMD_STX2	0x90
#define		CMD_ETX1	0xC5
#define		CMD_ETX2	0x79

#define		CMD_POSITION_STX1		0
#define		CMD_POSITION_STX2		1
#define		CMD_POSITION_LENGTH		2
#define		CMD_POSITION_TLMCMD		3
#define		CMD_POSITION_FROM		4
#define		CMD_POSITION_TO			5
#define		CMD_POSITION_EXE_TYPE	6
#define		CMD_POSITION_TIME		7
#define		CMD_POSITION_TYPE		11
#define		CMD_POSITION_COUNT		12
#define		CMD_POSITION_CODE		13
#define		CMD_POSITION_PARAM		14

//GenerateCmd
#define		CMD_POSITION_CRC_H		15
#define		CMD_POSITION_CRC_L		16
#define		CMD_POSITION_ETX1		17
#define		CMD_POSITION_ETX2		18
#define		CMD_BYTE_COUNT			(CMD_POSITION_ETX2 + 1)

typedef enum
{
  TLMCMD_CMD = 0x00,
  TLMCMD_SC_CMD= SC_CMD_ID,
  TLMCMD_TLM = 0xFF,
  TLMCMD_DR = 0xFE,
  TLMCMD_VIA_SF = 0x0A
} CCP_TC_DSC;

typedef enum
{
	TLM_DSC_HK  = 0x00,
	TLM_DSC_ACK = 0x01
} CCP_TLM_DSC;

typedef enum
{
	FROM_TO_COMM = 0x04,
	FROM_TO_ADCS = 0x05,
	FROM_TO_AQUA = 0x01,
	FROM_TO_SF = 0x02,
	FROM_TO_SUBCAM = 0x03,
	FROM_TO_OVCO = 0x06,
	FROM_TO_GS = 0xFF
} CCP_FT_ID;

typedef enum
{
	CCP_REALTIME		= 0x00,
	CCP_TIMELINE		= 0x11,
	CCP_FRAM_TLC		= 0x20,
	PKT_DIFF_STORED		= 0x21	//update: 2017/10/31: OBCÇÃëäëŒéûçèStoredCommand//ñ¢é¿ëï
} CCP_CMD_EXE_TYPE;

typedef enum
{
  CCP_SINGLE = 0x00,
  CCP_BLOCK = 0x10,
  CCP_STORED_BLOCK = 0x12
} CCP_CMD_TYPE;

typedef enum
{
	CCP_OBC_TLM_ID		= 1,
	CCP_AQU_TLM_ID_1 = 16,
	CCP_AQU_TLM_ID_2 = 17,
	CCP_AQU_TLM_ID_3 = 18,
	CCP_AQU_TLM_ID_4 = 19,
	CCP_SUBCAM_TLM_ID_1		= 20,
	CCP_SUBCAM_TLM_ID_2		= 21,
	CCP_SUBCAM_TLM_ID_3		= 22,
	CCP_SUBCAM_TLM_ID_4		= 23,
	CCP_SF_TLM_ID_1 = 32,
	CCP_SF_TLM_ID_2		= 33,
	CCP_SF_ROUTING_ID	= 34,
	CCP_OVCO_TLM_ID_1 = 50,
	CCP_OVCO_TLM_ID_2 = 51,
	CCP_OVCO_TLM_ID_3 = 59
} CCP_DATA_ID;

typedef struct
{
  size_t length;
  unsigned char data[CCP_MAX_PACKET_LENGTH];
} CTCP;

CTCP CCP_initialize(const unsigned char* data,
	size_t length);

CTCP CCP_form_rt_cmd(unsigned char cmd_id,
	const unsigned char* param,
	size_t length);

CTCP CCP_form_tl_cmd(cycle_t time,
	unsigned char cmd_id,
	const unsigned char* param,
	size_t length);

void CCP_convert_rt_cmd_to_tl_cmd(CTCP* rt_cmd,
	cycle_t time);

CTCP CCP_form_app_cmd(cycle_t time, AR_APP_ID id);

void CCP_set_stx(CTCP* packet);

size_t CCP_get_length(const CTCP* packet);

void CCP_set_length(CTCP* packet,
	size_t length);

CCP_TC_DSC CCP_get_tlm_cmd_discrimination(const CTCP* packet);

void CCP_set_tlm_cmd_discrimination(CTCP* packet,
	CCP_TC_DSC discriminator);

CCP_FT_ID CCP_get_cmd_from_id(const CTCP* packet);

void CCP_set_cmd_from_id(CTCP* packet,
	CCP_FT_ID id);

CCP_FT_ID CCP_get_cmd_to_id(const CTCP* packet);

void CCP_set_cmd_to_id(CTCP* packet,
	CCP_FT_ID id);

CCP_CMD_EXE_TYPE CCP_get_cmd_exe_type(const CTCP* packet);

void CCP_set_cmd_exe_type(CTCP* packet,
	CCP_CMD_EXE_TYPE exe_type);

cycle_t CCP_get_cmd_time(const CTCP* packet);

void CCP_set_cmd_time(CTCP* packet, cycle_t time);

CCP_CMD_TYPE CCP_get_cmd_type_id(const CTCP* packet);

void CCP_set_cmd_type_id(CTCP* packet,
	CCP_CMD_TYPE id);

size_t CCP_get_cmd_count(const CTCP* packet);

void CCP_set_cmd_count(CTCP* packet,
	size_t count);

unsigned int CCP_get_cmd_id(const CTCP* packet);

void CCP_set_cmd_id(CTCP* packet,
	unsigned int id);

const unsigned char* CCP_get_cmd_param(const CTCP* packet);

void CCP_set_cmd_param(CTCP* packet,
	const unsigned char* param,
	size_t length);

unsigned int CCP_get_tlm_id(const CTCP* packet);

int CCP_get_tlm_count(const CTCP* packet);

int CCP_get_tlm_time(const CTCP* packet);
void CCP_set_tlm_time(CTCP* packet, cycle_t time);

void CCP_set_cmd_time_l(CTCP* packet, cycle_t time);

void CCP_set_utime2cycle(CTCP* packet);

void CCP_set_tlm_id(CTCP* packet, unsigned int id);
void CCP_set_tlm_count(CTCP* packet, unsigned char count);

#endif // HODO_UNI_PACKET_H_
