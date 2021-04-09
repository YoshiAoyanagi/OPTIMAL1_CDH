#ifndef PACKET_HANDLER_H_
#define PACKET_HANDLER_H_

#include "CommonTlmCmdPacket.h"
#include "BlockCommandTable.h"
#include "PacketList.h"
//#include "tlm_frame.h"
#include "cmd_analyze_obc.h"
#include "Utility.h"
#include "FRAM_cmd_manager.h"

#define RT_CMD_LIST_MAX (4)
#define RT_TLM_LIST_MAX (12)
#define TL_CMD_LIST_MAX (32)
#define TO_ADCS_CMD_LIST_MAX (4)
#define TO_MSN_CMD_LIST_MAX (4)

typedef enum
{
	PH_SUCCESS,
	PH_REGISTERED,
	PH_FORWARDED,
	PH_INVALID_LENGTH,
	PH_INVALID_CRC,
	PH_INVALID_TO_ID,
	PH_INVALID_DISCRIMINATOR,
	PH_INVALID_CMD_TYPE,
	PH_PL_LIST_FULL,
	PH_TLC_REGISTERD,
	PH_TLC_PAST_TIME,
	PH_TLC_ALREADY_EXISTS,
	PH_TLC_NOT_EXECUTED,
	PH_TLC_NOT_FINED,
	PH_BC_REGISTERED,
	PH_BC_INVALID_BLOCK_NO,
	PH_BC_INVALID_CMD_NO,
	PH_BC_ISORATED_CMD,
	PH_BC_INACTIVE_BLOCK,
	PH_BC_COMBINE_FAILED,
	PH_FRAM_TLC_REGISTERD,
	PH_FRAM_TLC_PAST_TIME,
	PH_FRAM_TLC_ALREADY_EXISTS,
	PH_NOW_PROCESSING,
	PH_FORWARDED_VIA_SF,
	PH_BC_CMD_COUNT_CONFLICT,
	PH_UNKNOWN
}PH_ACK;

typedef struct
{
	CTCP packet;
	int rec_status;
	int rec_count;
} PE_Info;

extern PL_Info rt_tlm_list;
extern PL_Info tl_cmd_list;
extern PL_Info rt_cmd_list;
extern PL_Info to_msn_cmd_list;
extern PL_Info to_adcs_cmd_list;

extern int GsCmdSts;
extern PH_ACK GsCmdErrorSts;
extern int GsCmdCode;
extern int GsCmdToId;
extern int GsCmdCount;
extern int GsTlmDsc;
extern int CdhCmdCount;
extern int CdhCmdCode;

PE_Info PE_initialize(void);
void PH_init(void);
static int PH_check_packet_error(CTCP* packet);

PH_ACK PH_extract_packet(unsigned char* rec_buffer, int rec_byte, PE_Info* pei);
PH_ACK PH_analyze_packet(CTCP* packet);
PH_ACK PH_Cmd_Router(const CTCP* packet);
PH_ACK PH_send_realtime_telemetry(CTCP* packet);
PH_ACK PH_despatch_command(const CTCP* packet);

PH_ACK add_rt_tlm(CTCP* packet);
PH_ACK add_rt_cmd(const CTCP* packet);

#endif