#include "PacketHandler.h"
#include "FRAM_TL_CmdDispatcher.h"
#include "DataRecorderManager.h"
#include "tlm_frame.h"
#include "LoRaCmd_handler.h"

PL_Info rt_tlm_list;
PL_Info tl_cmd_list;
PL_Info rt_cmd_list;
PL_Info to_msn_cmd_list;
PL_Info to_adcs_cmd_list;

int GsCmdSts;
PH_ACK GsCmdErrorSts;
int GsCmdCode;
int GsCmdToId;
int GsCmdCount;
int GsTlmDsc;
int CdhCmdCount;
int CdhCmdCode;

static PL_Node rt_cmd_stock_[RT_CMD_LIST_MAX];
static PL_Node rt_tlm_stock_[RT_TLM_LIST_MAX];
static PL_Node tl_cmd_stock_[TL_CMD_LIST_MAX];
static PL_Node to_adcs_cmd_stock_[TO_ADCS_CMD_LIST_MAX];
static PL_Node to_msn_cmd_stock_[TO_MSN_CMD_LIST_MAX];

static PH_ACK analyze_block_cmd_(const CTCP* packet);
static PH_ACK analyze_single_cmd_(CTCP* packet);
static PH_ACK analyze_cmd_(CTCP* packet);

static PH_ACK add_tl_cmd_(const CTCP* packet, size_t now);
static PH_ACK add_adcs_cmd_(const CTCP* packet);
static PH_ACK add_msn_cmd_(const CTCP* packet);
static PH_ACK add_fram_tlc(const CTCP* packet, utime_t now);
static unsigned char check_msn_packet(const CTCP* packet);

PE_Info PE_initialize(void)
{
	PE_Info pei;
	pei.packet = CCP_initialize(NULL, 0);
	pei.rec_status = CMD_REC_STATUS_0;
	pei.rec_count = 0;

	return pei;
}

void PH_init(void)
{
	rt_tlm_list = PL_initialize(rt_tlm_stock_, RT_TLM_LIST_MAX);
	rt_cmd_list = PL_initialize(rt_cmd_stock_, RT_CMD_LIST_MAX);
	tl_cmd_list = PL_initialize(tl_cmd_stock_, TL_CMD_LIST_MAX);
	to_msn_cmd_list = PL_initialize(to_msn_cmd_stock_, TO_MSN_CMD_LIST_MAX);
	to_adcs_cmd_list = PL_initialize(to_adcs_cmd_stock_, TO_ADCS_CMD_LIST_MAX);

	// 地上コマンド処理ステータスの初期化
	GsCmdSts = CMD_STATUS_NONE;
	GsCmdErrorSts = PH_UNKNOWN;
	GsCmdCode = 0;
	GsCmdToId = 0;
	GsTlmDsc = 0;
	CdhCmdCount = 0;
	CdhCmdCode = 0;

	return;
}
static int PH_check_packet_error(CTCP* packet)
{
	unsigned int	crc_calc;
	unsigned int	crc_cmd;

	unsigned char id = packet->data[TLM_POSITION_TLM_MODE];

	//	ETX Check
	if (packet->data[packet->length - 1] != CMD_ETX2) {
		return(-1);
	}
	
	if (packet->data[packet->length - 2] != CMD_ETX1) {
		return(-1);
	}

	//	CRC Check
	crc_calc = crc(&(packet->data[CMD_POSITION_TLMCMD]), (packet->length - 2 - 1 - 2 - 2));	// -STX*2 - LENGTH   -ETX*2 - CRC*2 
	crc_calc &= 0xffff;
	crc_cmd = packet->data[packet->length - 4];
	crc_cmd <<= 8;
	crc_cmd |= packet->data[packet->length - 3];

	//if (crc_calc != crc_cmd)
		//return(-2);

	return(0);
}

PH_ACK PH_extract_packet(unsigned char* rec_buffer, int rec_byte, PE_Info* pei)
{
	PH_ACK ack = PH_NOW_PROCESSING;//PH_UNKNOWN;

	while (rec_byte > 0)
	{
		if (pei->rec_status == CMD_REC_STATUS_0) // STX
		{
			if (*rec_buffer == CMD_STX1)
			{
				pei->packet.length = 0;
				pei->packet.data[pei->packet.length] = *rec_buffer;
				pei->packet.length++;
				pei->rec_status = CMD_REC_STATUS_1;
			}
		}
		else if (pei->rec_status == CMD_REC_STATUS_1) // STX
		{
			if (*rec_buffer == CMD_STX2)
			{
				pei->packet.data[pei->packet.length] = *rec_buffer;
				pei->packet.length++;
				pei->rec_status = CMD_REC_STATUS_2;
			}
		}
		else if (pei->rec_status == CMD_REC_STATUS_2) // Length
		{
			pei->packet.data[pei->packet.length] = *rec_buffer;
			pei->packet.length++;
			pei->rec_count = *rec_buffer;
			pei->rec_count += 4; // IncludeCRC and ETX

			pei->rec_status = CMD_REC_STATUS_3;
		}
		else if (pei->rec_status == CMD_REC_STATUS_3) // CMD_DATA
		{
			pei->packet.data[pei->packet.length] = *rec_buffer;
			pei->packet.length++;
			pei->rec_count--;
			if (pei->rec_count <= 0)
			{
				int ret = 0;
				ret = PH_check_packet_error(&(pei->packet));
				if (ret == 0)
				{
					ack = PH_analyze_packet(&(pei->packet));
				}
				else if (ret == -1)
				{
					ack = PH_INVALID_LENGTH;
#ifdef PRINT_DEBUG
					Serial.println("length error");
#endif
				}
				else if (ret == -2)
				{
					ack = PH_INVALID_CRC;
#ifdef PRINT_DEBUG
					Serial.println("crc error");
#endif
				}
				pei->rec_status = CMD_REC_STATUS_0;
			}
		}
		rec_buffer++;
		rec_byte--;
	}
	return ack;
}

PH_ACK PH_analyze_packet(CTCP* packet)
{
	switch (CCP_get_tlm_cmd_discrimination(packet))
	{
	case TLMCMD_CMD:
		//UTIMEをcycleに変換
		return analyze_cmd_(packet);
	case TLMCMD_SC_CMD:
		CCP_set_tlm_cmd_discrimination(packet, TLMCMD_CMD);
		return analyze_cmd_(packet);
	case TLMCMD_TLM:
		return add_rt_tlm(packet);
	case TLMCMD_DR:
		return add_rt_tlm(packet);
	default:
		return PH_INVALID_DISCRIMINATOR;
	}
	return PH_UNKNOWN;
}

static PH_ACK analyze_cmd_(CTCP* packet)
{
	switch (CCP_get_cmd_type_id(packet))
	{
	case CCP_SINGLE:
		return analyze_single_cmd_(packet);
	case CCP_BLOCK:
		if (CCP_get_cmd_from_id(packet) == FROM_TO_GS)
		{
			CCP_set_utime2cycle(packet);
		}
		return analyze_block_cmd_(packet);
	case CCP_STORED_BLOCK:
		if (FRAM_SBC_Write_packet(packet) == Success)
		{
			return PH_BC_REGISTERED;
		}
		else
		{
			return PH_BC_CMD_COUNT_CONFLICT;
		}
	default:
		return PH_UNKNOWN;
	}
}

static PH_ACK analyze_block_cmd_(const CTCP* packet)
{
	switch (BCT_register_cmd(packet))
	{
	case BCT_SUCCESS:
		++(rt_cmd_list.pl_public.executed_nodes);	//BC, TLCでもcmd_countを上昇
		return PH_BC_REGISTERED;
	case BCT_INVALID_BLOCK_NO:
		return PH_BC_INVALID_BLOCK_NO;
	case BCT_INVALID_CMD_NO:
		return PH_BC_INVALID_CMD_NO;
	case BCT_ISORATED_CMD:
		return PH_BC_ISORATED_CMD;
	default:
		return PH_UNKNOWN;
	}
}

PH_ACK PH_Cmd_Router(const CTCP* packet)
{
	switch (CCP_get_cmd_to_id(packet))
	{
	case FROM_TO_COMM:
		return PH_despatch_command(packet);
	case FROM_TO_ADCS:
		return add_adcs_cmd_(packet);
	case FROM_TO_SF:
		return add_msn_cmd_(packet);
	case FROM_TO_SUBCAM:
		return add_msn_cmd_(packet);
	case FROM_TO_OVCO:
		return add_msn_cmd_(packet);
	case FROM_TO_AQUA:
		return add_msn_cmd_(packet);
	default:return PH_FORWARDED;
	}
	return PH_UNKNOWN;
}

static PH_ACK analyze_single_cmd_(CTCP* packet)
{
	switch (CCP_get_cmd_exe_type(packet))
	{
	case CCP_REALTIME:
		return PH_Cmd_Router(packet);
	case CCP_TIMELINE:
		if (CCP_get_cmd_from_id(packet) == FROM_TO_GS)
		{
			CCP_set_utime2cycle(packet);
		}
		// コマンド発行元情報をOBCに上書き
		CCP_set_cmd_from_id(packet, FROM_TO_COMM);
		++(rt_cmd_list.pl_public.executed_nodes);		//BC, TLCでもcmd_countを上昇
		return add_tl_cmd_(packet, (size_t)master_clock->master);
	case CCP_FRAM_TLC:
		// コマンド発行元情報をOBCに上書き
		CCP_set_cmd_from_id(packet, FROM_TO_COMM);
		CCP_set_cmd_exe_type(packet, CCP_REALTIME);
		++(rt_cmd_list.pl_public.executed_nodes);		//BC, TLCでもcmd_countを上昇
		return add_fram_tlc(packet, master_clock->utime);
	default:
		return PH_UNKNOWN;
	}
}
PH_ACK PH_despatch_command(const CTCP* packet)
{
	// CMD以外のパケットが来たら異常判定
	int tcd = CCP_get_tlm_cmd_discrimination(packet);
	if ((tcd != TLMCMD_CMD))
	{
		return PH_INVALID_DISCRIMINATOR;
	}

	// Singleコマンドの場合
	if (CCP_get_cmd_type_id(packet) == CCP_SINGLE)
	{
		PH_ACK ack;
		if (CCP_get_cmd_to_id(packet) == FROM_TO_COMM)
		{
			ack = (PH_ACK)cmdExec(packet);
		}
		else
		{
			ack = PH_Cmd_Router(packet);
		}
		return ack;
	}
	// Single以外の場合は異常判定
	else { return PH_INVALID_CMD_TYPE; }
}

PH_ACK PH_send_realtime_telemetry(CTCP* packet)
{
	int i;
	int i_ret;

	if (CCP_get_tlm_cmd_discrimination(packet) == TLMCMD_CMD)
	{
		return PH_INVALID_DISCRIMINATOR;
	}
	for (i = 0; i < packet->length; i++)
		TlmTransferFrame.Packet.packet_data[i] = packet->data[i];

	i_ret = TlmSend((int)((packet->length) - TLM_BYTE_COUNT));

	if (i_ret == -1)
	{
		//Serial.println("PH_NOW_PROCESSING");
		return PH_NOW_PROCESSING;
	}

	return PH_SUCCESS;
}

PH_ACK add_rt_tlm(CTCP* packet)
{
	int i;
	unsigned char _dr_tlm = packet->data[CMD_POSITION_TLMCMD];

	//Forward via SF
	if (lora_fwd_tlm.fwd_enabled)
	{
		for (i = 0; i < sizeof(lora_fwd_tlm.fwd_tlm_id); i++)
		{
			if (CCP_get_tlm_id(packet) == lora_fwd_tlm.fwd_tlm_id[i])
			{
				return PH_FORWARDED_VIA_SF;
			}
		}
	}
	//

	if (msn_dr.enabled && _dr_tlm == (unsigned char)TLMCMD_TLM)
	{

		if (check_msn_packet(packet) == 1)
		{
			return PH_FORWARDED;
		}
	}

	//ST-DR
	if (hk_dr.enabled && _dr_tlm == (unsigned char)TLMCMD_TLM)
	{
		if ((CCP_get_tlm_id(packet) != CCP_OBC_TLM_ID) && (check_msn_packet(packet) != 1))//OBC定期テレメ以外，ミッションテレメ以外
		{
			OBC_DR_PacketWrite(packet);
		}
	}

	PL_ACK ack = PL_push_back(&rt_tlm_list, packet);
	if (ack != PL_SUCCESS) {
#ifdef MSG_DEBUG
		Serial.println("ERROR: PH_PL_LIST_FULL");
#endif
		rt_tlm_list = PL_initialize(rt_tlm_stock_, RT_TLM_LIST_MAX);
		return PH_PL_LIST_FULL;
	}

	return PH_REGISTERED;
}

static PH_ACK add_tl_cmd_(const CTCP* packet,
	size_t now)
{
	PL_ACK ack = PL_insert_tl_cmd(&tl_cmd_list, packet, now);
	switch (ack)
	{
	case PL_SUCCESS:
		return PH_TLC_REGISTERD;
	case PL_LIST_FULL:
		return PH_PL_LIST_FULL;
	case PL_TLC_PAST_TIME:
		return PH_TLC_PAST_TIME;
	case PL_TLC_ALREADY_EXISTS:
		return PH_TLC_ALREADY_EXISTS;
	default:
		return PH_UNKNOWN;
	}
}

PH_ACK add_rt_cmd(const CTCP* packet)
{
	PL_ACK ack = PL_push_back(&rt_cmd_list, packet);
	if (ack != PL_SUCCESS) { return PH_PL_LIST_FULL; }
	return PH_REGISTERED;
}

static PH_ACK add_adcs_cmd_(const CTCP* packet)
{
	PL_ACK ack = PL_push_back(&to_adcs_cmd_list, packet);
	if (ack != PL_SUCCESS) { return PH_PL_LIST_FULL; }
	return PH_REGISTERED;
}

static PH_ACK add_msn_cmd_(const CTCP* packet)
{
	PL_ACK ack = PL_push_back(&to_msn_cmd_list, packet);
	if (ack != PL_SUCCESS) { return PH_PL_LIST_FULL; }
	return PH_REGISTERED;
}

static PH_ACK add_fram_tlc(const CTCP* packet, utime_t now_utime)
{
	utime_t time = CCP_get_cmd_time(packet);
	if (now_utime > time)
		return PH_FRAM_TLC_PAST_TIME;
	if (fram_tlc_queue == FRAM_TLC_MAX)
		return PH_PL_LIST_FULL;

	if (FRAM_TLC_Write_packet(packet) != Success)
		return PH_FRAM_TLC_ALREADY_EXISTS;

	return PH_FRAM_TLC_REGISTERD;
}

static unsigned char check_msn_packet(const CTCP* packet)
{
	switch (CCP_get_tlm_id(packet))	
	{
	case CCP_AQU_TLM_ID_1:return (1);
	case CCP_AQU_TLM_ID_2:return (1);
	case CCP_AQU_TLM_ID_3:return (1);
	case CCP_AQU_TLM_ID_4:return (1);
	case CCP_SUBCAM_TLM_ID_1:return (1);
	case CCP_SUBCAM_TLM_ID_2:return (1);
	case CCP_SUBCAM_TLM_ID_3:return (1);
	case CCP_SUBCAM_TLM_ID_4:return (1);
	case CCP_SF_TLM_ID_1:return (1);
	case CCP_SF_TLM_ID_2:return (1);
	case CCP_OVCO_TLM_ID_1:return (1);
	case CCP_OVCO_TLM_ID_2:return (1);
	case CCP_OVCO_TLM_ID_3:return (1);

	}
	return 0;	//未該当
}