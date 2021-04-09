#include "CommonTlmCmdPacket.h"
#include "tlm_frame.h"

#include "CommonTlmCmdPacket.h"

#include <string.h> // for memcpy


static void CCP_update_crc_(CTCP* packet);
static void CCP_set_etx_(CTCP* packet);
static void CCP_setup_common_cmd_header_(CTCP* packet);


CTCP CCP_initialize(const unsigned char* data,
	size_t length)
{
	CTCP packet;

	if ((data == NULL) || (length > CCP_MAX_PACKET_LENGTH))
	{
		packet.length = 0;
	}
	else
	{
		packet.length = length;
		memcpy(packet.data, data, length);
	}

	return packet;
}

CTCP CCP_form_rt_cmd(unsigned char cmd_id,
	const unsigned char* param,
	size_t length)
{
	CTCP packet;
	cycle_t dummy = 0;
	CCP_setup_common_cmd_header_(&packet);
	packet.data[CMD_POSITION_EXE_TYPE] = CCP_REALTIME;
	memcpy(&(packet.data[CMD_POSITION_TIME]), &dummy, 4);
	packet.data[CMD_POSITION_CODE] = cmd_id;
	CCP_set_cmd_param(&packet, param, length);

	return packet;
}

CTCP CCP_form_tl_cmd(cycle_t time,
	unsigned char cmd_id,
	const unsigned char* param,
	size_t length)
{
	CTCP packet;
	CCP_setup_common_cmd_header_(&packet);
	packet.data[CMD_POSITION_EXE_TYPE] = CCP_TIMELINE;
	memcpy(&(packet.data[CMD_POSITION_TIME]), &time, 4);
	packet.data[CMD_POSITION_CODE] = cmd_id;
	CCP_set_cmd_param(&packet, param, length);

	return packet;
}

void CCP_convert_rt_cmd_to_tl_cmd(CTCP* rt_cmd,
	cycle_t time)
{
	CCP_set_cmd_exe_type(rt_cmd, CCP_TIMELINE);
	CCP_set_cmd_time(rt_cmd, time);
}
CTCP CCP_form_app_cmd(cycle_t time,
	AR_APP_ID id)
{
	return CCP_form_tl_cmd(time, CODE_Cmd_EXECUTE_APP, (unsigned char*)&(id), sizeof(AR_APP_ID));
}

void CCP_set_stx(CTCP* packet)
{
	packet->data[CMD_POSITION_STX1] = CMD_STX1;
	packet->data[CMD_POSITION_STX2] = CMD_STX2;
}

size_t CCP_get_length(const CTCP* packet)
{
	return (size_t)packet->data[CMD_POSITION_LENGTH];
}

void CCP_set_length(CTCP* packet,
	size_t length)
{
	packet->data[CMD_POSITION_LENGTH] = (unsigned char)length;
}

CCP_TC_DSC CCP_get_tlm_cmd_discrimination(const CTCP* packet)
{
	return (CCP_TC_DSC)packet->data[CMD_POSITION_TLMCMD];
}

void CCP_set_tlm_cmd_discrimination(CTCP* packet,
	CCP_TC_DSC discriminator)
{
	packet->data[CMD_POSITION_TLMCMD] = (unsigned char)discriminator;
	CCP_update_crc_(packet);
}

CCP_FT_ID CCP_get_cmd_from_id(const CTCP* packet)
{
	return (CCP_FT_ID)packet->data[CMD_POSITION_FROM];
}

void CCP_set_cmd_from_id(CTCP* packet,
	CCP_FT_ID id)
{
	packet->data[CMD_POSITION_FROM] = (unsigned char)id;
	CCP_update_crc_(packet);
}

CCP_FT_ID CCP_get_cmd_to_id(const CTCP* packet)
{
	return (CCP_FT_ID)packet->data[CMD_POSITION_TO];
}

void CCP_set_cmd_to_id(CTCP* packet,
	CCP_FT_ID id)
{
	packet->data[CMD_POSITION_TO] = (unsigned char)id;
	CCP_update_crc_(packet);
}

CCP_CMD_EXE_TYPE CCP_get_cmd_exe_type(const CTCP* packet)
{
	return (CCP_CMD_EXE_TYPE)packet->data[CMD_POSITION_EXE_TYPE];
}

void CCP_set_cmd_exe_type(CTCP* packet,
	CCP_CMD_EXE_TYPE exe_type)
{
	packet->data[CMD_POSITION_EXE_TYPE] = (unsigned char)exe_type;
	CCP_update_crc_(packet);
}

cycle_t CCP_get_cmd_time(const CTCP* packet)
{
	cycle_t time = 0;
	int i;

	for (i = 0; i < 4; ++i)
	{
		time <<= 8;
		time += packet->data[CMD_POSITION_TIME + i];
	}
	return time;
}

void CCP_set_cmd_time(CTCP* packet, cycle_t time)
{
	
	unsigned char* ptr = (unsigned char*)&time;
	int i;
	for (i = 0; i<4; ++i)
	{
		packet->data[CMD_POSITION_TIME + i] = ptr[i];
	}

	CCP_update_crc_(packet);
}

void CCP_set_cmd_time_l(CTCP* packet, cycle_t time)
{
	packet->data[CMD_POSITION_TIME + 0] = (unsigned char)((time >> 24) & 0xFF);
	packet->data[CMD_POSITION_TIME + 1] = (unsigned char)((time >> 16) & 0xFF);
	packet->data[CMD_POSITION_TIME + 2] = (unsigned char)((time >> 8) & 0xFF);
	packet->data[CMD_POSITION_TIME + 3] = (unsigned char)((time) & 0xFF);

	CCP_update_crc_(packet);
}

CCP_CMD_TYPE CCP_get_cmd_type_id(const CTCP* packet)
{
	return (CCP_CMD_TYPE)packet->data[CMD_POSITION_TYPE];
}

void CCP_set_cmd_type_id(CTCP* packet,
	CCP_CMD_TYPE id)
{
	packet->data[CMD_POSITION_TYPE] = (unsigned char)id;
	CCP_update_crc_(packet);
}

size_t CCP_get_cmd_count(const CTCP* packet)
{
	return (size_t)packet->data[CMD_POSITION_COUNT];
}

void CCP_set_cmd_count(CTCP* packet,
	size_t count)
{
	packet->data[CMD_POSITION_COUNT] = (unsigned char)count;
	CCP_update_crc_(packet);
}

unsigned int CCP_get_cmd_id(const CTCP* packet)
{
	return (unsigned int)packet->data[CMD_POSITION_CODE];
}

void CCP_set_cmd_id(CTCP* packet,
	unsigned int id)
{
	packet->data[CMD_POSITION_CODE] = (unsigned char)id;
	CCP_update_crc_(packet);
}

const unsigned char* CCP_get_cmd_param(const CTCP* packet)
{
	return (packet->data) + CMD_POSITION_PARAM;
}

void CCP_set_cmd_param(CTCP* packet,
	const unsigned char* param,
	size_t param_length)
{
	if (param_length > CCP_MAX_PARAM_LENGTH) { param_length = CCP_MAX_PARAM_LENGTH; }

	packet->length = 18 + param_length;
	CCP_set_length(packet, 11 + param_length);
	memcpy(&(packet->data[CMD_POSITION_PARAM]), param, param_length);
	CCP_update_crc_(packet);
	CCP_set_etx_(packet);
}

unsigned int CCP_get_tlm_id(const CTCP* packet)
{
	return (unsigned int)packet->data[TLM_POSITION_TLM_MODE];
}

//add 20190911
void CCP_set_tlm_id(CTCP* packet, unsigned int id)
{
	packet->data[TLM_POSITION_TLM_MODE] = (unsigned char)id;
	CCP_update_crc_(packet);
}

void CCP_set_tlm_count(CTCP* packet, unsigned char count)
{
	packet->data[TLM_POSITION_TLM_COUNT] = (unsigned char)count;
	CCP_update_crc_(packet);
}

//

static void CCP_update_crc_(CTCP* packet)
{
	size_t len = CCP_get_length(packet);
	unsigned int crc_calc = crc(&(packet->data[CMD_POSITION_TLMCMD]), len);

	packet->data[3 + len] = (unsigned char)(0xff & (crc_calc >> 8));
	packet->data[4 + len] = (unsigned char)(0xff & crc_calc);
}

static void CCP_set_etx_(CTCP* packet)
{
	size_t len = CCP_get_length(packet);

	packet->data[5 + len] = (unsigned char)CMD_ETX1;
	packet->data[6 + len] = (unsigned char)CMD_ETX2;
}

static void CCP_setup_common_cmd_header_(CTCP* packet)
{
	packet->data[CMD_POSITION_STX1] = CMD_STX1;
	packet->data[CMD_POSITION_STX2] = CMD_STX2;
	packet->data[CMD_POSITION_TLMCMD] = TLMCMD_CMD;
	packet->data[CMD_POSITION_FROM] = FROM_TO_COMM;
	packet->data[CMD_POSITION_TO] = FROM_TO_COMM;
	packet->data[CMD_POSITION_TYPE] = CCP_SINGLE;
	packet->data[CMD_POSITION_COUNT] = 0x00;
}

void CCP_set_utime2cycle(CTCP* packet)
{
	cycle_t time = 0;
	utime_t utime = 0;

	int i;

	for (i = 0; i<4; ++i)
	{
		utime <<= 8;
		utime += packet->data[CMD_POSITION_TIME + i];
	}
	time = OBCT_utime2cycle(utime, master_clock->initial_utime);

	for (i = 0; i<4; ++i)
	{
		packet->data[CMD_POSITION_TIME + i] = (unsigned char)((time >> (24 - i * 8)) & 0xff);
	}

	CCP_update_crc_(packet);
}

void CCP_set_tlm_time(CTCP* packet, cycle_t time)
{
	packet->data[TLM_POSITION_TLM_TIME + 0] = (unsigned char)((time >> 24) & 0xFF);
	packet->data[TLM_POSITION_TLM_TIME + 1] = (unsigned char)((time >> 16) & 0xFF);
	packet->data[TLM_POSITION_TLM_TIME + 2] = (unsigned char)((time >> 8) & 0xFF);
	packet->data[TLM_POSITION_TLM_TIME + 3] = (unsigned char)((time) & 0xFF);

	CCP_update_crc_(packet);
}