// 
// 
// 

#include "cmd_action_cmdtlm.h"
#include "cmd_analyze_obc.h"
#include "Stored_TlmGenerator.h"
#include "STRX_update.h"
#include "TlmDispatcher.h"
#include "LoRaCmd_handler.h"

int Cmd_TlmRequest_via_SF(const CTCP* packet)
{
	unsigned char id = packet->data[CMD_POSITION_PARAM];
	int i_ret = LoRa_ObcTlmSend(id);
	return i_ret;
}

int Cmd_Set_Fwd_TlmID_via_SF(const CTCP* packet)
{
	lora_fwd_tlm.fwd_enabled = packet->data[CMD_POSITION_PARAM];
	lora_fwd_tlm.fwd_tlm_id[0] = packet->data[CMD_POSITION_PARAM + 1];
	lora_fwd_tlm.fwd_tlm_id[1] = packet->data[CMD_POSITION_PARAM + 2];
	lora_fwd_tlm.fwd_tlm_id[2] = packet->data[CMD_POSITION_PARAM + 3];
	lora_fwd_tlm.fwd_tlm_id[3] = packet->data[CMD_POSITION_PARAM + 4];

	return CMD_ERROR_NONE;
}


int Cmd_TlmRequest(const CTCP* packet)
{
	unsigned char id = packet->data[CMD_POSITION_PARAM];
	int i_ret = OBC_TlmRegister(id);
	return i_ret;
}

int Cmd_SET_TLM_INTERVAL(const CTCP* packet)
{
	unsigned int interval_sec;
	
	interval_sec = (packet->data[CMD_POSITION_PARAM] << 8);
	interval_sec += packet->data[CMD_POSITION_PARAM + 1];
	OBC_Tlm_Set_tlm_interval(interval_sec);
	return CMD_ERROR_NONE;
}

int Cmd_TLM_SET_MODE(const CTCP* packet)
{
	return CMD_ERROR_NONE;
}

int Cmd_MUX_UART_SET(const CTCP* packet)
{
	unsigned char port = packet->data[CMD_POSITION_PARAM];
	Serial_MUX_change((SERIAL_MUX)port);

	return CMD_ERROR_NONE;
}



///Cmd
int Cmd_OBC_DR_REPLAY_PARAM_SET(const CTCP* packet)
{
	int i;
	int ofs = 0;

	unsigned int start_sector;
	unsigned int start_packet_address;

	for (i = 0; i < 2; i++)
	{
		start_sector <<= 8;
		start_sector += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		start_packet_address <<= 8;
		start_packet_address += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;
	hk_dr.replay_pointer = DR_Sector_Init(start_sector, start_packet_address);

	return CMD_ERROR_NONE;
}

int Cmd_OBC_DR_REPLAY(const CTCP* packet)
{
	int i;
	int ofs = 0;

	unsigned char _enable;
	unsigned int _num_packet;
	unsigned int _interval_time;

	_enable = packet->data[CMD_POSITION_PARAM + ofs];
	ofs += 1;

	for (i = 0; i < 2; i++)
	{
		_num_packet <<= 8;
		_num_packet += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		_interval_time <<= 8;
		_interval_time += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	hk_dr.replay_interval_time = _interval_time;
	hk_dr.replay_num_packet = _num_packet;
	hk_dr.replay_enabled = _enable;

	return CMD_ERROR_NONE;
}

//Add 2019/11/13
int Cmd_OBC_DR_DIRECT_REPLAY(const CTCP* packet)
{
	int i;
	int ofs = 0;

	unsigned char _enable;
	unsigned int _num_packet;
	unsigned int _interval_time;
	unsigned int start_sector;
	unsigned int start_packet_address;

	for (i = 0; i < 2; i++)
	{
		start_sector <<= 8;
		start_sector += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		start_packet_address <<= 8;
		start_packet_address += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		_num_packet <<= 8;
		_num_packet += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		_interval_time <<= 8;
		_interval_time += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	hk_dr.replay_pointer = DR_Sector_Init(start_sector, start_packet_address);
	hk_dr.replay_interval_time = _interval_time;
	hk_dr.replay_num_packet = _num_packet;
	hk_dr.replay_enabled = 1;

	return CMD_ERROR_NONE;
}


int Cmd_OBC_DR_ERASE(const CTCP* packet)
{
	int i;
	int ofs = 0;

	unsigned int _start_sector;
	unsigned int _end_sector;

	for (i = 0; i < 2; i++)
	{
		_start_sector <<= 8;
		_start_sector += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		_end_sector <<= 8;
		_end_sector += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	st_tlm_erase(_start_sector, _end_sector);

	return CMD_ERROR_NONE;
}

int Cmd_OBC_DR_Replay_Stop(const CTCP* packet)
{
	hk_dr.replay_enabled = 0;
	return CMD_ERROR_NONE;
}

int Cmd_OBC_DR_RECORD(const CTCP* packet)
{
	int i;
	int ofs = 0;

	unsigned char _enable;
	unsigned int _interval = 0;

	_enable = packet->data[CMD_POSITION_PARAM + ofs];
	ofs++;

	for (i = 0; i < 2; i++)
	{
		_interval <<= 8;
		_interval += packet->data[CMD_POSITION_PARAM + ofs + i];
	}

	if (_enable > 1)
		return CMD_ERROR_ILLEGAL_PARAMETER;

	if (_interval == 0)
		_enable = 0;

	hk_dr.enabled = _enable;
	st_tlm_set_interval(_interval);
	return CMD_ERROR_NONE;
}


//mission recorder
int Cmd_MSN_DR_REPLAY_PARAM_SET(const CTCP* packet)
{
	int i;
	int ofs = 0;

	unsigned int start_sector;
	unsigned int start_packet_address;

	for (i = 0; i < 2; i++)
	{
		start_sector <<= 8;
		start_sector += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		start_packet_address <<= 8;
		start_packet_address += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;
	msn_dr.replay_pointer = DR_Sector_Init(start_sector, start_packet_address);

	return CMD_ERROR_NONE;
}

int Cmd_MSN_DR_REPLAY(const CTCP* packet)
{
	int i;
	int ofs = 0;

	unsigned char _enable;
	unsigned int _num_packet;
	unsigned int _interval_time;

	_enable = packet->data[CMD_POSITION_PARAM + ofs];
	ofs += 1;

	for (i = 0; i < 2; i++)
	{
		_num_packet <<= 8;
		_num_packet += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		_interval_time <<= 8;
		_interval_time += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	msn_dr.replay_interval_time = _interval_time;
	msn_dr.replay_num_packet = _num_packet;
	msn_dr.replay_enabled = _enable;

	return CMD_ERROR_NONE;
}

int Cmd_MSN_DR_ERASE(const CTCP* packet)
{
	int i;
	int ofs = 0;

	unsigned short _start_sector;
	unsigned short _end_sector;

	for (i = 0; i < 2; i++)
	{
		_start_sector <<= 8;
		_start_sector += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		_end_sector <<= 8;
		_end_sector += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	MSN_DR_Set_EraseMode(_start_sector, _end_sector);//
													 //MSN_DR_Erase(_start_sector, _end_sector); //

	return CMD_ERROR_NONE;
}

int Cmd_MSN_DR_Replay_Stop(const CTCP* packet)
{
	msn_dr.replay_enabled = 0;
	return CMD_ERROR_NONE;
}

int Cmd_MSN_DR_RECORD_ENABLE(const CTCP* packet)
{
	int i;
	int ofs = 0;

	unsigned char _enable;

	_enable = packet->data[CMD_POSITION_PARAM + ofs];
	ofs++;

	if (_enable > 1)
		return CMD_ERROR_ILLEGAL_PARAMETER;

	msn_dr.enabled = _enable;
	return CMD_ERROR_NONE;
}

int Cmd_OBC_DR_INITIALIZE(const CTCP* packet)
{
	OBC_DR_Initialize();
	return CMD_ERROR_NONE;
}

int Cmd_MSN_DR_INITIALIZE(const CTCP* packet)
{
	MSN_DR_Initialize();
	return CMD_ERROR_NONE;
}

//Add 2019/11/13
int Cmd_MSN_DR_DIRECT_REPLAY(const CTCP* packet)
{
	int i;
	int ofs = 0;

	unsigned char _enable;
	unsigned int _num_packet;
	unsigned int _interval_time;
	unsigned int start_sector;
	unsigned int start_packet_address;

	for (i = 0; i < 2; i++)
	{
		start_sector <<= 8;
		start_sector += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		start_packet_address <<= 8;
		start_packet_address += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		_num_packet <<= 8;
		_num_packet += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	for (i = 0; i < 2; i++)
	{
		_interval_time <<= 8;
		_interval_time += packet->data[CMD_POSITION_PARAM + i + ofs];
	}
	ofs += 2;

	msn_dr.replay_pointer = DR_Sector_Init(start_sector, start_packet_address);
	msn_dr.replay_interval_time = _interval_time;
	msn_dr.replay_num_packet = _num_packet;
	msn_dr.replay_enabled = 1;

	return CMD_ERROR_NONE;
}


//STRX
int Cmd_STRX_SET_BR(const CTCP* packet)
{
	unsigned char param = packet->data[CMD_POSITION_PARAM ];

	STX_set_bitrate(param);
	return CMD_ERROR_NONE;
}
int Cmd_STRX_SET_TX_PWR(const CTCP* packet)
{
	unsigned char param = packet->data[CMD_POSITION_PARAM];
	STX_set_power(param);

	return CMD_ERROR_NONE;
}
int Cmd_STRX_TX_ON(const CTCP* packet)
{
	unsigned char param = packet->data[CMD_POSITION_PARAM];

	if (param == 0)
		STX_set_OFF();
	else if (param == 1)
		STX_set_ON();
	else
		return CMD_ERROR_ILLEGAL_PARAMETER;

	return CMD_ERROR_NONE;
}

//add
int Cmd_STX_SET_OUTPORT(const CTCP* packet)
{
	unsigned char param = packet->data[CMD_POSITION_PARAM];
	STX_set_outport(param);

	return CMD_ERROR_NONE;
}

int Cmd_STX_SET_MOD(const CTCP* packet)
{
	unsigned char param = packet->data[CMD_POSITION_PARAM];
	STX_set_mod(param);

	return CMD_ERROR_NONE;
}

int Cmd_STX_SET_PN_TEST(const CTCP* packet)
{
	unsigned char param = packet->data[CMD_POSITION_PARAM];
	STX_set_pn(param);

	return CMD_ERROR_NONE;
}

int Cmd_SET_STX_ONOFF_DURATION(const CTCP* packet)
{
	int i;
	int ofs = 0;
	unsigned int _on_dur = 0;
	unsigned int _off_dur = 0;

	for (i = 0; i < 2; i++)
	{
		_on_dur <<= 8;
		_on_dur += packet->data[CMD_POSITION_PARAM + ofs + i];
	}
	ofs+=2;
	for (i = 0; i < 2; i++)
	{
		_off_dur <<= 8;
		_off_dur += packet->data[CMD_POSITION_PARAM + ofs + i]; 
	}
	ofs+=2;

	STX_set_transmit_duration(_on_dur, _off_dur);

	return CMD_ERROR_NONE;
}
int Cmd_SET_STX_BURST_DURATION(const CTCP* packet)
{
	int i;
	int ofs = 0;
	unsigned int _on_dur = 0;
	
	for (i = 0; i < 2; i++)
	{
		_on_dur <<= 8;
		_on_dur += packet->data[CMD_POSITION_PARAM + ofs + i];
	}
	if (_on_dur > (30 * 60))	//30•ªˆÈã‚Ì˜A‘±ON‚Í”F‚ß‚È‚¢
		return CMD_ERROR_ILLEGAL_PARAMETER;
	STX_set_burst_duration(_on_dur);

	return CMD_ERROR_NONE;
}

int Cmd_STRX_OTC_ENABLE(const CTCP* packet)
{
	unsigned char tmp = packet->data[CMD_POSITION_PARAM];
	if(tmp > 1)
		return CMD_ERROR_ILLEGAL_PARAMETER;

	STRX_set_OTC_Enable(tmp);
	return CMD_ERROR_NONE;
}
int Cmd_STRX_OTC_SET_THRESHOLD(const CTCP* packet)
{
	unsigned char tmp = packet->data[CMD_POSITION_PARAM];
	if (tmp < STRX_OTC_LOW_TEMPERATURE)
		return CMD_ERROR_ILLEGAL_PARAMETER;

	STRX_set_OTC_Threshold(tmp);
	return CMD_ERROR_NONE;
}

int Cmd_OBC_BIN_DATA_UPLOAD(const CTCP* packet)
{
	static unsigned char count;
	CTCP tlm_packet = CCP_initialize(packet->data, packet->length);

	CCP_set_tlm_id(&tlm_packet, 0xA0);
	CCP_set_tlm_count(&tlm_packet, count);
	if (count == 255)
		count = 0;
	else
		count++;

	OBC_DR_PacketWrite(&tlm_packet);
	return CMD_ERROR_NONE;
}

int Cmd_OBC_BIN_DATA_UPLOAD_BYTE_SHIFT(const CTCP* packet)
{
	static unsigned char prev_cmd_count;

	static unsigned char count;
	unsigned char data[256];
	size_t length;
	int i;

	for (i = 0; i < (packet->length); i++)
	{
		if(i >= TLM_POSITION_MAIN)
			data[i] = packet->data[i+1];
		else
			data[i] = packet->data[i];
	}
	data[TLM_POSITION_LENGTH] = data[TLM_POSITION_LENGTH] - 1;

	if (prev_cmd_count != 0)
	{
		if (prev_cmd_count == data[CMD_POSITION_COUNT])
		{
			return CMD_ERROR_CONFLICT;
		}
	}
	prev_cmd_count = data[CMD_POSITION_COUNT];

	CTCP tlm_packet = CCP_initialize(data, (packet->length - 1));

	CCP_set_tlm_id(&tlm_packet, 0xA0);
	CCP_set_tlm_count(&tlm_packet, count);
	if (count == 255)
		count = 0;
	else
		count++;

	OBC_DR_PacketWrite(&tlm_packet);
	return CMD_ERROR_NONE;
}


//G2
int Cmd_SET_STRX_LOCK_REPLY_TIME(const CTCP* packet)
{
	int i;
	int ofs = 0;
	unsigned int c_lock_time = 0;
	unsigned int s_lock_time = 0;
	float		 lock_rssi = 0.0;
	unsigned char raw_val = 0;

	for (i = 0; i < 2; i++)
	{
		c_lock_time <<= 8;
		c_lock_time += packet->data[CMD_POSITION_PARAM + ofs + i];
	}
	ofs += 2;
	for (i = 0; i < 2; i++)
	{
		s_lock_time <<= 8;
		s_lock_time += packet->data[CMD_POSITION_PARAM + ofs + i];
	}
	ofs += 2;

	lock_rssi = packet->data[CMD_POSITION_PARAM + ofs];

	STRX_set_lock_reply_time(c_lock_time, s_lock_time, lock_rssi);

	return CMD_ERROR_NONE;
}


//G2
int Cmd_SET_OBCAM_DATA_INTERVAL(const CTCP* packet)
{
	unsigned int val;
	int i;
	for (i = 0; i < 4; ++i)
	{
		val <<= 8;
		val += packet->data[CMD_POSITION_PARAM + i];
	}

	OBCAM_Set_interval(val);
	return CMD_ERROR_NONE;
}

int Cmd_SET_TLMD_ANOMALY_ENABLE(const CTCP* packet)
{
	unsigned char ena = packet->data[CMD_POSITION_PARAM];
	FRAM_Write_tlmd_anomaly_ena(ena);

	FRAM_Read_tlmd_anomaly_ena(&tlmd_anomaly_detect_enable);

	return CMD_ERROR_NONE;
}


