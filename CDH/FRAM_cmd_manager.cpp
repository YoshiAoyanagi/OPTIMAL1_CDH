// 
// 
// 

#include "FRAM_cmd_manager.h"

FRAM tlc_fram;
unsigned int fram_tlc_exec_count;
unsigned int fram_tlc_queue;
unsigned int fram_tlc_time[FRAM_TLC_MAX];

S_BC_PARAM Stored_BC_Param;
unsigned int Stored_BC_Number;
unsigned int Stored_BC_Position;
unsigned char Stored_BC_Data[128];

ack FRAM_TLC_Begin(void)
{
	fram_tlc_queue = 0;
	fram_tlc_exec_count = 0;
	unsigned char data[1];
	data[0] = FRAM_CMD_VERIFY_CODE;
	tlc_fram.Begin(FRAM2_CS);
	tlc_fram.WriteData(data, FRAM_CMD_ADDRESS_VERIFY, 1);

	ack i_ret = FRAM_TLC_Read_verify();

	if (i_ret == Success)
	{
		FRAM_TLC_Read_queue();
		FRAM_TLC_Read_tlc_time();
	}

	Stored_BC_Number = 0;
	Stored_BC_Position = 0;

	return i_ret;
}

ack FRAM_TLC_Read_verify(void)
{
	unsigned char data[1];
	tlc_fram.ReadData(data, FRAM_CMD_ADDRESS_VERIFY, 1);
	if (data[0] == FRAM_CMD_VERIFY_CODE)
		return Success;

	else
		return ValueError;
}

ack FRAM_TLC_Read_queue(void)
{
	FRAM_TLC_Read_tlc_time();

	return Success;
}

PL_ACK FRAM_TLC_search_tlc(utime_t utime, unsigned char *data)
{
	int i;
	unsigned int cnt;
	unsigned char raw_data[CMD_DATA_MAX];
	unsigned int pkt_time = 0;

	for (cnt = 0; cnt < FRAM_TLC_MAX; cnt++)
	{
		if (utime >= fram_tlc_time[cnt] && fram_tlc_time[cnt] != 0)
		{
			FRAM_TLC_Read_packet(cnt, raw_data);
			for (i = 0; i < CMD_DATA_MAX; i++)
			{
				data[i] = raw_data[i];
			}
			fram_tlc_exec_count++;
			fram_tlc_queue--;

			//drop
			FRAM_TLC_Write_queue(fram_tlc_queue);
			FRAM_TLC_drop_time(cnt);
			FRAM_TLC_Read_tlc_time();
			if (utime == fram_tlc_time[cnt]) { return PL_TLC_ON_TIME; }
			else if (utime > fram_tlc_time[cnt]) { return PL_TLC_PAST_TIME; }

		}
	}

	return PL_TLC_NOT_YET;
}

ack FRAM_TLC_Read_tlc_time(void)
{
	int i;
	unsigned int cnt = 0;
	unsigned int time = 0;
	unsigned char data[4];
	fram_tlc_queue = 0;

	for (cnt = 0; cnt < FRAM_TLC_MAX; cnt++)
	{
		tlc_fram.ReadData(data, FRAM_CMD_ADDRESS_TLC_TIME_HEAD + (cnt * 4), sizeof(data));

		time = 0;
		for (i = 0; i<4; ++i)
		{
			time <<= 8;
			time += data[i];
		}
		fram_tlc_time[cnt] = time;
		if (time != 0)
			fram_tlc_queue++;
	}

	return Success;
}

ack FRAM_TLC_Read_packet(unsigned int p, unsigned char *raw_data)
{
	int i;
	unsigned char data[CMD_DATA_MAX];
	unsigned int cnt = 0;
	unsigned int time = 0;

	tlc_fram.ReadData(data, FRAM_CMD_ADDRESS_TLC_START + (p * CMD_DATA_MAX), CMD_DATA_MAX);

	for (i = 0; i < CMD_DATA_MAX; i++)
	{
		raw_data[i] = data[i];
	}
	return Success;
}

ack FRAM_TLC_Write_packet(const CTCP *packet)
{
	utime_t utime = CCP_get_cmd_time(packet);
	unsigned char data[4];
	unsigned char dummy_data[CMD_DATA_MAX];
	int i;
	for (i = 0; i < CMD_DATA_MAX; i++)
	{
		if (i < packet->length)
			dummy_data[i] = packet->data[i];
		else
			dummy_data[i] = 0;
	}

	unsigned int cnt;
	unsigned int empty_address = 0;

	data[0] = (unsigned char)((utime >> 24) & 0xff);
	data[1] = (unsigned char)((utime >> 16) & 0xff);
	data[2] = (unsigned char)((utime >> 8) & 0xff);
	data[3] = (unsigned char)(utime & 0xff);

	//ãÛÇ´ÉAÉhÉåÉXÇíTçı
	FRAM_TLC_Read_tlc_time();
	for (cnt = 0; cnt < FRAM_TLC_MAX; cnt++)
	{
		if (fram_tlc_time[cnt] == 0)
			break;
		empty_address++;
	}

	for (cnt = 0; cnt < FRAM_TLC_MAX; cnt++)
	{
		if (fram_tlc_time[cnt] == utime)
			return Error;
	}

	//í«â¡(20201223)
	if (empty_address >= FRAM_TLC_MAX)
		return Error;

	tlc_fram.WriteData(data, FRAM_CMD_ADDRESS_TLC_TIME_HEAD + (empty_address * 4), 4);
	tlc_fram.WriteData(dummy_data, FRAM_CMD_ADDRESS_TLC_START + (empty_address * CMD_DATA_MAX), CMD_DATA_MAX);

	fram_tlc_queue++;
	FRAM_TLC_Write_queue(fram_tlc_queue);
	FRAM_TLC_Read_tlc_time();

	return Success;
}


ack FRAM_TLC_Write_Test(void)
{
	unsigned char dummy_data[CMD_DATA_MAX];
	int i;
	for (i = 0; i < CMD_DATA_MAX; i++)
	{
		dummy_data[i] = 0xAA;
	}

	unsigned int cnt;
	unsigned int empty_address = 0;

	tlc_fram.WriteData(dummy_data, FRAM_CMD_ADDRESS_TLC_START + (empty_address * CMD_DATA_MAX), CMD_DATA_MAX);

	unsigned char raw_data[CMD_DATA_MAX];

	tlc_fram.ReadData(raw_data, FRAM_CMD_ADDRESS_TLC_START + (empty_address * CMD_DATA_MAX), CMD_DATA_MAX);
	for (i = 0; i < CMD_DATA_MAX; i++)
	{
		Serial.print(raw_data[i], HEX); Serial.print(" ");
	}
	return Success;
}



ack FRAM_TLC_Write_queue(unsigned int queue)
{
	unsigned char data[1];

	data[0] = queue;
	tlc_fram.WriteData(data, FRAM_CMD_ADDRESS_TLC_QUEUE, 1);

	return Success;
}

ack FRAM_TLC_drop_time(unsigned int p)
{
	unsigned int address = 4 * p + FRAM_CMD_ADDRESS_TLC_TIME_HEAD;
	unsigned char data[4] = { 0, 0, 0, 0 };
	unsigned int time;
	unsigned char r_data[4];
	unsigned int i;

	tlc_fram.ReadData(r_data, address, sizeof(r_data));
	time = 0;
	for (i = 0; i<4; ++i)
	{
		time <<= 8;
		time += r_data[i];
	}
	if (time == 0)
		return Error;

	tlc_fram.WriteData(data, address, 4);
	return Success;
}

//Clear F-TLC
ack FRAM_TLC_Clear_At_Address(unsigned int p)
{
	ack i_ret;

	if ((i_ret = FRAM_TLC_drop_time(p)) == Success)
	{
		fram_tlc_queue--;
		FRAM_TLC_Write_queue(fram_tlc_queue);
		FRAM_TLC_Read_tlc_time();

	}
	return i_ret;
}

ack FRAM_TLC_Clear_At_Time(utime_t utime)
{
	int cnt;

	for (cnt = 0; cnt < FRAM_TLC_MAX; cnt++)
	{
		if (utime == fram_tlc_time[cnt])
		{
			return FRAM_TLC_Clear_At_Address(cnt);
		}
	}
	return Error;
}

ack FRAM_TLC_Clear_All(void)
{
	int cnt;

	for (cnt = 0; cnt < FRAM_TLC_MAX; cnt++)
	{
		FRAM_TLC_Clear_At_Address(cnt);
	}
	return Error;
}

ack FRAM_TLC_All_Initialize(void)
{

	unsigned char data[1024];
	unsigned int size = FRAM_CAPACITY_MAX;
	unsigned int small_size = 1024;
	unsigned int head = 0;

	int i, j;

	for (j = 0; j < (FRAM_CAPACITY_MAX / small_size); j++)
	{
		for (i = 0; i < small_size; i++)
			data[i] = 0;

		tlc_fram.WriteData(data, head, small_size);
		head += small_size;
	}

	FRAM_TLC_Read_tlc_time();
	fram_tlc_queue = 0;

	Serial.println("FRAM TLC_ALL RESET");

	return Success;
}


/////////////////////
//FRAM-Block Command
/////////////////////

ack FRAM_SBC_Set_Stored_BC_Number(unsigned int number, unsigned int pos)
{
	Stored_BC_Position = 0;
	if (number < FRAM_S_BCT_MAX_BLOCKS && pos < FRAM_S_BCT_MAX_BLOCK_LENGTH)
	{
		Stored_BC_Number = number;
		Stored_BC_Position = pos;
	}
	else
		return Error;

	return Success;
}

ack FRAM_SBC_ACTIVATE(unsigned char bc_id, unsigned char activate)
{
	unsigned char data[1];
	data[0] = activate;
	unsigned int address_is_active = FRAM_CMD_ADDRESS_SBC_IS_ACTIVE_HEAD + bc_id * 1;
	tlc_fram.WriteData(data, address_is_active, 1);

	FRAM_SBC_Read_BlockInfo(bc_id);

	return Success;
}

ack FRAM_SBC_Write_packet(CTCP *packet)
{
	unsigned char id_data[1];
	unsigned char cycle_data[4];
	unsigned char cmd_data[CMD_DATA_MAX];

	unsigned char to_id = (unsigned char)CCP_get_cmd_to_id(packet);
	unsigned char cmd_id = (unsigned char)CCP_get_cmd_id(packet);
	unsigned int cycle = (unsigned int)CCP_get_cmd_time(packet);

	static unsigned char prev_cmd_count;
	if (prev_cmd_count != 0)
	{
		if (prev_cmd_count == (unsigned char)CCP_get_cmd_count(packet))
		{
			return ValueError;	//2èdìoò^ÇâÒîÇ∑ÇÈ
		}
	}
	prev_cmd_count = (unsigned char)CCP_get_cmd_count(packet);

	if (Stored_BC_Position >= FRAM_S_BCT_MAX_BLOCK_LENGTH)
		return Error;

	unsigned int address_to_id = FRAM_CMD_ADDRESS_SBC_TO_ID_HEAD + Stored_BC_Number * FRAM_S_BCT_MAX_BLOCK_LENGTH + Stored_BC_Position * 1;
	unsigned int address_cmd_id = FRAM_CMD_ADDRESS_SBC_CMD_ID_HEAD + Stored_BC_Number * FRAM_S_BCT_MAX_BLOCK_LENGTH + Stored_BC_Position * 1;
	unsigned int address_cycle = FRAM_CMD_ADDRESS_SBC_CYCLE_HEAD + Stored_BC_Number * 4 * FRAM_S_BCT_MAX_BLOCK_LENGTH + Stored_BC_Position * 4;
	unsigned int address_packet = FRAM_CMD_ADDRESS_SBC_START + Stored_BC_Number * CMD_DATA_MAX * FRAM_S_BCT_MAX_BLOCK_LENGTH + Stored_BC_Position * CMD_DATA_MAX;

	id_data[0] = to_id;
	tlc_fram.WriteData(id_data, address_to_id, 1);

	id_data[0] = cmd_id;
	tlc_fram.WriteData(id_data, address_cmd_id, 1);

	cycle_data[0] = (unsigned char)((cycle >> 24) & 0xff);
	cycle_data[1] = (unsigned char)((cycle >> 16) & 0xff);
	cycle_data[2] = (unsigned char)((cycle >> 8) & 0xff);
	cycle_data[3] = (unsigned char)(cycle & 0xff);
	tlc_fram.WriteData(cycle_data, address_cycle, 4);

	int i;
	for (i = 0; i < CMD_DATA_MAX; i++)
	{
		if (i < packet->length)
			cmd_data[i] = packet->data[i];
		else
			cmd_data[i] = 0;
	}
	tlc_fram.WriteData(cmd_data, address_packet, CMD_DATA_MAX);
	Stored_BC_Position++;

	return Success;
}

//Remove
ack FRAM_SBC_Remove_Block(unsigned char bc_id)
{
	int i;

	for (i = 0; i < FRAM_S_BCT_MAX_BLOCK_LENGTH; i++)
	{
		FRAM_SBC_Remove_Block_At_PS(bc_id, i);
	}
	/*
	unsigned char id_data[1] = { 0 };
	unsigned char cycle_data[4] = { 0 };
	unsigned char cmd_data[CMD_DATA_MAX] = { 0 };

	unsigned int address_to_id = FRAM_CMD_ADDRESS_SBC_TO_ID_HEAD + bc_id * FRAM_S_BCT_MAX_BLOCK_LENGTH;
	unsigned int address_cmd_id = FRAM_CMD_ADDRESS_SBC_CMD_ID_HEAD + bc_id * FRAM_S_BCT_MAX_BLOCK_LENGTH;
	unsigned int address_cycle = FRAM_CMD_ADDRESS_SBC_CYCLE_HEAD + bc_id * 4 * FRAM_S_BCT_MAX_BLOCK_LENGTH;
	unsigned int address_packet = FRAM_CMD_ADDRESS_SBC_START + bc_id * CMD_DATA_MAX * FRAM_S_BCT_MAX_BLOCK_LENGTH;

	int i, j;
	for (i = 0; i < FRAM_S_BCT_MAX_BLOCK_LENGTH; i++)
	{
	address_to_id	+= i;
	address_cmd_id	+= i;
	address_cycle	+= i*4;
	address_packet	+= i*CMD_DATA_MAX;

	tlc_fram.WriteData(id_data, address_to_id, 1);
	tlc_fram.WriteData(id_data, address_cmd_id, 1);
	tlc_fram.WriteData(cycle_data, address_cycle, 4);
	tlc_fram.WriteData(cmd_data, address_packet, CMD_DATA_MAX);
	}
	*/
	return Success;
}

ack FRAM_SBC_Remove_Block_At_PS(unsigned char bc_id, unsigned char pos)
{
	unsigned char id_data[1] = { 0 };
	unsigned char cycle_data[4] = { 0 };
	unsigned char cmd_data[CMD_DATA_MAX] = { 0 };

	unsigned int address_to_id = FRAM_CMD_ADDRESS_SBC_TO_ID_HEAD + bc_id * FRAM_S_BCT_MAX_BLOCK_LENGTH;
	unsigned int address_cmd_id = FRAM_CMD_ADDRESS_SBC_CMD_ID_HEAD + bc_id * FRAM_S_BCT_MAX_BLOCK_LENGTH;
	unsigned int address_cycle = FRAM_CMD_ADDRESS_SBC_CYCLE_HEAD + bc_id * 4 * FRAM_S_BCT_MAX_BLOCK_LENGTH;
	unsigned int address_packet = FRAM_CMD_ADDRESS_SBC_START + bc_id * CMD_DATA_MAX * FRAM_S_BCT_MAX_BLOCK_LENGTH;

	address_to_id += pos;
	address_cmd_id += pos;
	address_cycle += pos * 4;
	address_packet += pos * CMD_DATA_MAX;

	tlc_fram.WriteData(id_data, address_to_id, 1);
	tlc_fram.WriteData(id_data, address_cmd_id, 1);
	tlc_fram.WriteData(cycle_data, address_cycle, 4);
	tlc_fram.WriteData(cmd_data, address_packet, CMD_DATA_MAX);

	return Success;
}
ack FRAM_SBC_Remove_ALL_Block(void)
{
	int i;
	for (i = 0; i < FRAM_S_BCT_MAX_BLOCKS; i++)
	{
		FRAM_SBC_Remove_Block(i);
	}
	return Success;
}


ack FRAM_SBC_Read_BlockInfo(unsigned char bc_id)
{
	unsigned char id_data[1];
	unsigned char cmd_id_data[1];
	unsigned char cycle_data[4];

	unsigned int address_is_active;
	unsigned int address_to_id;
	unsigned int address_cmd_id;
	unsigned int address_cycle;
	unsigned int address_param;
	unsigned char bc_pos = 0;
	unsigned int cycle = 0;

	Stored_BC_Param.bc_id = bc_id;

	address_is_active = FRAM_CMD_ADDRESS_SBC_IS_ACTIVE_HEAD + bc_id * 1;
	tlc_fram.ReadData(id_data, address_is_active, sizeof(id_data));
	Stored_BC_Param.is_active = id_data[0];

	int i, j;
	for (i = 0; i < FRAM_S_BCT_MAX_BLOCK_LENGTH; i++)
	{
		address_to_id = FRAM_CMD_ADDRESS_SBC_TO_ID_HEAD + bc_id * FRAM_S_BCT_MAX_BLOCK_LENGTH + i * 1;
		address_cmd_id = FRAM_CMD_ADDRESS_SBC_CMD_ID_HEAD + bc_id * FRAM_S_BCT_MAX_BLOCK_LENGTH + i * 1;
		address_cycle = FRAM_CMD_ADDRESS_SBC_CYCLE_HEAD + bc_id * 4 * FRAM_S_BCT_MAX_BLOCK_LENGTH + i * 4;

		tlc_fram.ReadData(id_data, address_to_id, sizeof(id_data));
		Stored_BC_Param.to_id[i] = id_data[0];

		tlc_fram.ReadData(cmd_id_data, address_cmd_id, sizeof(cmd_id_data));
		Stored_BC_Param.cmd_id[i] = cmd_id_data[0];

		tlc_fram.ReadData(cycle_data, address_cycle, sizeof(cycle_data));
		cycle = 0;
		for (j = 0; j<4; j++)
		{
			cycle <<= 8;
			cycle += cycle_data[j];
		}
		Stored_BC_Param.cycle[i] = cycle;

	}

	return Success;
}


ack FRAM_SBC_Read_packet(unsigned int bc_number, unsigned int pos, unsigned char *raw_data)
{
	int i;
	unsigned char data[CMD_DATA_MAX];
	unsigned int address_packet = FRAM_CMD_ADDRESS_SBC_START + bc_number * CMD_DATA_MAX * FRAM_S_BCT_MAX_BLOCK_LENGTH + pos * CMD_DATA_MAX;

	tlc_fram.ReadData(data, address_packet, CMD_DATA_MAX);

	for (i = 0; i < CMD_DATA_MAX; i++)
	{
		raw_data[i] = data[i];
	}
	return Success;
}

ack FRAM_SBC_ExtractData(unsigned char bc_id, unsigned char pos)
{
	int i;
	unsigned char data[CMD_DATA_MAX];

	Stored_BC_Number = bc_id;
	Stored_BC_Position = pos;

	unsigned int address_to_id = FRAM_CMD_ADDRESS_SBC_TO_ID_HEAD + bc_id * FRAM_S_BCT_MAX_BLOCK_LENGTH + i * 1;
	unsigned int address_cmd_id = FRAM_CMD_ADDRESS_SBC_CMD_ID_HEAD + bc_id * FRAM_S_BCT_MAX_BLOCK_LENGTH + i * 1;
	unsigned int address_cycle = FRAM_CMD_ADDRESS_SBC_CYCLE_HEAD + bc_id * 4 * FRAM_S_BCT_MAX_BLOCK_LENGTH + i * 4;
	unsigned int address_packet = FRAM_CMD_ADDRESS_SBC_START + bc_id * CMD_DATA_MAX * FRAM_S_BCT_MAX_BLOCK_LENGTH + pos * CMD_DATA_MAX;

	tlc_fram.ReadData(data, address_packet, CMD_DATA_MAX);

	for (i = 0; i < 128; i++)
	{
		Stored_BC_Data[i] = data[i];
	}

	return Success;
}



ack FRAM_SBC_Deploy_Block(unsigned char bc_id)
{
	CTCP packet;
	unsigned char id_data[1];
	unsigned char cmd_id_data[1];
	unsigned char cycle_data[4];
	unsigned char raw_data[CMD_DATA_MAX];

	unsigned int address_is_active;
	unsigned int address_to_id;
	unsigned int address_cmd_id;
	unsigned int address_cycle;
	unsigned char bc_pos = 0;
	unsigned int cycle = 0;

	Stored_BC_Param.bc_id = bc_id;

	address_is_active = FRAM_CMD_ADDRESS_SBC_IS_ACTIVE_HEAD + bc_id * 1;
	tlc_fram.ReadData(id_data, address_is_active, sizeof(id_data));
	Stored_BC_Param.is_active = id_data[0];

	if (Stored_BC_Param.is_active != SBC_ACTIVE)
		return Error;

	int i, j;
	for (i = 0; i < FRAM_S_BCT_MAX_BLOCK_LENGTH; i++)
	{
		address_to_id = FRAM_CMD_ADDRESS_SBC_TO_ID_HEAD + bc_id * FRAM_S_BCT_MAX_BLOCK_LENGTH + i * 1;
		address_cmd_id = FRAM_CMD_ADDRESS_SBC_CMD_ID_HEAD + bc_id * FRAM_S_BCT_MAX_BLOCK_LENGTH + i * 1;
		address_cycle = FRAM_CMD_ADDRESS_SBC_CYCLE_HEAD + bc_id * 4 * FRAM_S_BCT_MAX_BLOCK_LENGTH + i * 4;

		tlc_fram.ReadData(id_data, address_to_id, sizeof(id_data));
		Stored_BC_Param.to_id[i] = id_data[0];

		tlc_fram.ReadData(cmd_id_data, address_cmd_id, sizeof(cmd_id_data));
		Stored_BC_Param.cmd_id[i] = cmd_id_data[0];

		tlc_fram.ReadData(cycle_data, address_cycle, sizeof(cycle_data));
		cycle = 0;
		for (j = 0; j<4; j++)
		{
			cycle <<= 8;
			cycle += cycle_data[j];
		}
		Stored_BC_Param.cycle[i] = cycle;

		if (cycle != 0)
		{
			FRAM_SBC_Read_packet(bc_id, i, raw_data);
			int k;

			for (k = 0; k < CCP_MAX_PACKET_LENGTH; k++)
			{
				packet.data[k] = raw_data[k];
			}
			packet.length = CCP_MAX_PACKET_LENGTH;

			CCP_set_cmd_exe_type(&packet, CCP_FRAM_TLC);
			CCP_set_cmd_type_id(&packet, CCP_SINGLE);
			CCP_set_cmd_time_l(&packet, master_clock->utime + cycle);
			PH_analyze_packet(&packet);
		}
	}

	return Success;
}