// 
// 
// 

#include "LoRaCmd_handler.h"
#include "cmd_item.h"
#include "PacketHandler.h"
#include "tlm_frame.h"

//LoRa cmd router
LoRaCmdSts lora_cmd_sts;
static PE_Info lora_pei_;
LORA_FWD_STRUCT lora_fwd_tlm;

void LoRaCmd_analyze_shortcmd(unsigned char* data);
void LoRaCmd_analyze_shorttlc(unsigned char* data);
void LoRaCmd_analyze_fullcmd(unsigned char* data);
int LoRa_crc_check(unsigned char* data);

void LoRaTlmSend(unsigned char* data, unsigned char size);

void LoRaCmdInit(void)
{
	lora_pei_ = PE_initialize();

	lora_cmd_sts.length		= 0;
	lora_cmd_sts.id			= 0;
	lora_cmd_sts.seq_id		= 0;
	lora_cmd_sts.seq_count = 0;
	lora_cmd_sts.error_sts = 0;

	lora_fwd_tlm.fwd_count = 0;
	lora_fwd_tlm.fwd_enabled = 0;

	lora_fwd_tlm.fwd_tlm_id[0] = 0xF0;
	lora_fwd_tlm.fwd_tlm_id[1] = 22;
	lora_fwd_tlm.fwd_tlm_id[2] = 34;
	lora_fwd_tlm.fwd_tlm_id[3] = 35;

}

void LoRaCmdProcess(unsigned char* data, unsigned char length)
{
	int i, j;

	CTCP packet;

	//CRCチェック
	if (LoRa_crc_check(data) != 0)
	{
		lora_cmd_sts.error_sts = LR_CRC_ERROR;
		return;
	}
	//

	if (data[SF_CMD_ID_POSITION] == SF_ID_SHORT_CMD)	//短縮コマンド
	{
#ifdef LORA_DEBUG_MSG
		Serial.println("REC_SF_SHORT_CMD");
#endif
		LoRaCmd_analyze_shortcmd(data);
	}
	else if (data[SF_CMD_ID_POSITION] == SF_ID_SHORT_TLC)	//短縮TLCコマンド
	{
#ifdef LORA_DEBUG_MSG
		Serial.println("REC_SF_SHORT_TLC");
#endif
		LoRaCmd_analyze_shorttlc(data);
	}
	else if (data[SF_CMD_ID_POSITION] == SF_ID_FULL_CMD)	//フルコマンド
	{
#ifdef LORA_DEBUG_MSG
		Serial.println("REC_SF_FULL_CMD");
#endif
		LoRaCmd_analyze_fullcmd(data);
	}
}

void LoRaCmd_analyze_shortcmd(unsigned char* data)
{
	int i, j;
	unsigned char pbyte;
	unsigned char to_id;
	unsigned char cmd_id;
	unsigned char param[32];
	CTCP packet;

	lora_cmd_sts.length = data[SF_CMD_LENGTH_POSITION];
	lora_cmd_sts.id = data[SF_CMD_ID_POSITION];

	pbyte = data[SF_CMD_SHORT_PBYTE_POSITION];
	to_id = data[SF_CMD_SHORT_TO_ID_POSITION];
	cmd_id = data[SF_CMD_SHORT_CMD_ID_POSITION];

	for (i = 0; i < pbyte; i++)
	{
		param[i] = data[SF_CMD_SHORT_PARAM_POSITION + i];
	}

	packet = CCP_form_rt_cmd(cmd_id, param, pbyte);
	CCP_set_cmd_to_id(&packet, (CCP_FT_ID)to_id);
	CCP_set_cmd_from_id(&packet, FROM_TO_SF);
	RT_Cmd_Register(packet);
	lora_cmd_sts.error_sts = LR_SUCCESS;
}

void LoRaCmd_analyze_shorttlc(unsigned char* data)
{
	int i, j;
	unsigned int time;
	unsigned char pbyte;
	unsigned char to_id;
	unsigned char cmd_id;
	unsigned char param[32];
	CTCP packet;

	lora_cmd_sts.length = data[SF_CMD_LENGTH_POSITION];
	lora_cmd_sts.id = data[SF_CMD_ID_POSITION];

	pbyte = data[SF_CMD_SHORT_PBYTE_POSITION];
	to_id = data[SF_CMD_SHORT_TO_ID_POSITION];
	cmd_id = data[SF_CMD_SHORT_CMD_ID_POSITION];

	for (i = 0; i < 4; i++)
	{
		time <<= 8;
		time += data[SF_CMD_SHORT_TLC_TIME_POSITION + i];
	}

	for (i = 0; i < pbyte; i++)
	{
		param[i] = data[SF_CMD_SHORT_TLC_PARAM_POSITION + i];
	}

	packet = CCP_form_tl_cmd(time, cmd_id, param, pbyte);
	CCP_set_cmd_exe_type(&packet, CCP_FRAM_TLC);
	CCP_set_cmd_to_id(&packet, (CCP_FT_ID)to_id);
	CCP_set_cmd_from_id(&packet, FROM_TO_SF);
	RT_Cmd_Register(packet);
	lora_cmd_sts.error_sts = LR_SUCCESS;
}

void LoRaCmd_analyze_fullcmd(unsigned char* data)
{
	int i, j;
	//Full command
	//static unsigned char sf_cmd[256];
	unsigned char rec_data[256];
	unsigned char rec_length;
	static unsigned char sf_cmd[16][256];
	static unsigned char sf_length[16];
	static unsigned char tmp_seq_id;
	static unsigned char tmp_seq_cnt;
	static unsigned char pkt_count;
	static unsigned char pkt_sts;

	lora_cmd_sts.length = data[SF_CMD_LENGTH_POSITION];
	lora_cmd_sts.id = data[SF_CMD_ID_POSITION];

	tmp_seq_id = data[SF_CMD_SEQ_ID_POSITION];
	tmp_seq_cnt = data[SF_CMD_SEQ_CNT_POSITION];

	if (tmp_seq_cnt == 0x00)
	{
		lora_cmd_sts.seq_id = tmp_seq_id;
		lora_cmd_sts.error_sts = LR_PROCESSING;
#ifdef LORA_DEBUG_MSG
		Serial.print("SEQ_CNT="); Serial.println(tmp_seq_cnt);
#endif

		//reset
		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 256; j++)
			{
				sf_cmd[i][j] = 0x00;
			}
		}
		pkt_sts = 0;
		pkt_count = 0;

		sf_length[tmp_seq_cnt] = (lora_cmd_sts.length - SF_CMD_SEQ_DATA_POSITION - 2);
		for (i = 0; i < (lora_cmd_sts.length - SF_CMD_SEQ_DATA_POSITION - 2); i++)
		{
			sf_cmd[tmp_seq_cnt][i] = data[SF_CMD_SEQ_DATA_POSITION + i];
		}

	}
	else
	{
		if (tmp_seq_id == lora_cmd_sts.seq_id)
		{
			lora_cmd_sts.error_sts = LR_PROCESSING;
#ifdef LORA_DEBUG_MSG
			Serial.print("SEQ_CNT="); Serial.println(tmp_seq_cnt);
#endif

			sf_length[tmp_seq_cnt] = (lora_cmd_sts.length - SF_CMD_SEQ_DATA_POSITION - 2);
			for (i = 0; i < (lora_cmd_sts.length - SF_CMD_SEQ_DATA_POSITION - 2); i++)
			{
				sf_cmd[tmp_seq_cnt][i] = data[SF_CMD_SEQ_DATA_POSITION + i];
			}
		}
		else
		{
			lora_cmd_sts.error_sts = LR_INVALID_SEQ_ID;
#ifdef LORA_DEBUG_MSG
			Serial.println("LR_INVALID_SEQ_ID");
#endif
		}

	}

	//sf_cmd[][]からパケット生成
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < sf_length[i]; j++)
		{
			rec_data[pkt_count] = sf_cmd[i][j];
			Serial.print(rec_data[pkt_count], HEX); Serial.print(" ");

			if (rec_data[pkt_count] == CMD_STX1)
			{
				pkt_sts = 1;
			}
			else if (pkt_sts == 1 && rec_data[pkt_count] == CMD_STX2)
			{
				pkt_sts = 2;
			}
			else if (pkt_sts == 2 && rec_data[pkt_count] == CMD_ETX1)
			{
				pkt_sts = 3;
			}
			else if (pkt_sts == 3 && rec_data[pkt_count] == CMD_ETX2)
			{
				pkt_sts = 4;
			}
			pkt_count++;
		}
	}
	if (pkt_sts == 4)
	{
		PH_extract_packet(rec_data, pkt_count, &lora_pei_);
		lora_cmd_sts.error_sts = LR_SUCCESS;
	}
}


int LoRa_crc_check(unsigned char* data)
{
	unsigned int	crc_calc;
	unsigned int	crc_cmd;
	int i;

	if (data[SF_CMD_LENGTH_POSITION] < 2)
		return(-1);

	//	CRC Check
	crc_calc = crc(&(data[SF_CMD_LENGTH_POSITION]), (data[SF_CMD_LENGTH_POSITION] - 2));	// -STX*2 - LENGTH   -ETX*2 - CRC*2 

	crc_calc &= 0xffff;
	crc_cmd = data[data[SF_CMD_LENGTH_POSITION] - 2];
	crc_cmd <<= 8;
	crc_cmd |= data[data[SF_CMD_LENGTH_POSITION] - 1];

	if (crc_calc != crc_cmd)
	{
#ifdef LORA_DEBUG_MSG
		Serial.print("CRC ERROR: "); Serial.print(crc_calc, HEX); Serial.println(" ");
#endif
		return(-2);
	}

	return(0);
}

//add20200904
int LoRa_ObcTlmSend(unsigned char id)
{
	// 範囲外のTLM IDを除外
	if (id > 0xff) { return CMD_ERROR_ILLEGAL_PARAMETER; }
	// 存在しないTLM IDを除外
	if (tlm_packet_ptr[id] == NULL) { return CMD_ERROR_ILLEGAL_CONTEXT; }

	int ofs = MakeTlmPacket(id, TLM_REAL);
	if (ofs > 0)
	{
		CTCP tlm_packet = CCP_initialize(TlmTransferFrame.Packet.packet_data, (size_t)(TLM_BYTE_COUNT + ofs));
		CCP_set_tlm_cmd_discrimination(&tlm_packet, TLMCMD_VIA_SF);
		LoRaTlmSend(tlm_packet.data, tlm_packet.length);



	}
	return CMD_ERROR_NONE;
}

void LoRaTlmSend(unsigned char* data, unsigned char size)
{
	int i;
	Serial.println("SEND: ");
	SERIAL_MUX prev_port = serial_mux_enable_port;

	Serial_MUX_change(MUX_SF);

	for (i = 0; i < size; i++)
	{
		MSN_Serial.write(data[i]);
		Serial.print(data[i], HEX); Serial.print(" ");
	}
	Serial.println(" ");
	
	Serial_MUX_change(prev_port);
}

void LoRa_TlmForward(CTCP packet)
{
	CCP_set_tlm_cmd_discrimination(&packet, TLMCMD_VIA_SF);
	LoRaTlmSend(packet.data, packet.length);

	if (lora_fwd_tlm.fwd_count == 0xFF)
		lora_fwd_tlm.fwd_count = 0;
	else
		lora_fwd_tlm.fwd_count++;

	return;
}
