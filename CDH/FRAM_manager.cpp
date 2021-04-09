// 
// 
// 

#include "FRAM_manager.h"

FRAM com_fram;

ack FRAM_Begin(void)
{
	unsigned char data[1];
	data[0] = FRAM_VERIFY_CODE;
	com_fram.Begin(FRAM1_CS);
	com_fram.WriteData(data, FRAM_ADDRESS_VERIFY, 1);
	ack i_ret = FRAM_Read_verify();
	return i_ret;
}


ack FRAM_Write(unsigned char *data, unsigned int address)
{
	com_fram.WriteData(data, address, sizeof(data));

	return Success;
}

ack FRAM_Write_rtc_sync_ena(ENABLE_STS ena)
{
	unsigned char data[1];
	data[0] = (unsigned char)ena;
	com_fram.WriteData(data, FRAM_ADDRESS_RTC_SYNC_ENA, sizeof(data));

	return Success;
}

ack FRAM_Write_uvc_ena(ENABLE_STS ena)
{
	unsigned char data[1];
	data[0] = (unsigned char)ena;
	com_fram.WriteData(data, FRAM_ADDRESS_UVC_ENA, sizeof(data));

	return Success;
}

ack FRAM_Write_uvc_threshold(unsigned char v1, unsigned char v2, unsigned char v3)
{
	unsigned char data[1];
	data[0] = v1;
	com_fram.WriteData(data, FRAM_ADDRESS_UVC1_THR, sizeof(data));

	data[0] = v2;
	com_fram.WriteData(data, FRAM_ADDRESS_UVC2_THR, sizeof(data));

	data[0] = v3;
	com_fram.WriteData(data, FRAM_ADDRESS_UVC3_THR, sizeof(data));
	return Success;
}

ack FRAM_Write_serial_mux(unsigned char port)
{
	unsigned char data[1];
	data[0] = port;
	com_fram.WriteData(data, FRAM_ADDRESS_SERIAL_MUX, sizeof(data));

	return Success;
}
ack FRAM_Write_ops_mode(ModeManagerInfo mode)
{
	unsigned char data[1];
	data[0] = (unsigned char)mode.current_id;
	com_fram.WriteData(data, FRAM_ADDRESS_OPS_MODE, sizeof(data));

	data[0] = (unsigned char)mode.previous_id;
	com_fram.WriteData(data, FRAM_ADDRESS_OPS_PREV_MODE, sizeof(data));

	return Success;
}

ack FRAM_Write_PWR_STS(unsigned int address, unsigned char pwr_sts)
{
	unsigned char data[1];
	data[0] = pwr_sts;
	com_fram.WriteData(data, address, sizeof(data));
	return Success;
}

ack FRAM_Write_master_cycle(unsigned int time)
{
	unsigned char data[4];

	data[0] = (time >> 24) & 0xFF;
	data[1] = (time >> 16) & 0xFF;
	data[2] = (time >> 8) & 0xFF;
	data[3] = (time & 0xFF);

	com_fram.WriteData(data, FRAM_ADDRESS_MASTER_CYCLE, sizeof(data));

	return Success;
}
ack FRAM_Write_reboot_count(unsigned int count)
{
	unsigned char data[4];
	data[0] = (count >> 24) & 0xFF;
	data[1] = (count >> 16) & 0xFF;
	data[2] = (count >> 8) & 0xFF;
	data[3] = (count & 0xFF);
	com_fram.WriteData(data, FRAM_ADDRESS_REBOOT_COUNT, sizeof(data));
	return Success;
}

ack FRAM_Write_stx_on_off_duration(unsigned int on_dur, unsigned int off_dur)
{
	unsigned char data[2];
	data[0] = (on_dur >> 8) & 0xFF;
	data[1] = (on_dur & 0xFF);
	com_fram.WriteData(data, FRAM_ADDRESS_STX_ON_DURATION, sizeof(data));

	data[0] = (off_dur >> 8) & 0xFF;
	data[1] = (off_dur & 0xFF);
	com_fram.WriteData(data, FRAM_ADDRESS_STX_OFF_DURATION, sizeof(data));
	return Success;
}

ack FRAM_Write_strx_otc_ena(unsigned char ena)
{
	unsigned char data[1];
	data[0] = ena;
	if (ena > 1)
		return ValueError;
	com_fram.WriteData(data, FRAM_ADDRESS_STRX_OTC_ENA, sizeof(data));
	return Success;
}

ack FRAM_Write_strx_otc_thr(unsigned char thr)
{
	unsigned char data[1];
	data[0] = thr;
	com_fram.WriteData(data, FRAM_ADDRESS_STRX_OTC_THR, sizeof(data));
	return Success;
}

ack FRAM_Write_strx_power(unsigned char pwr)
{
	unsigned char data[1];
	data[0] = pwr;
	com_fram.WriteData(data, FRAM_ADDRESS_STRX_POWER, sizeof(data));
	return Success;
}


ack FRAM_Write_strx_lock_reply_time(unsigned int c_lock, unsigned int s_lock, unsigned char lock_rssi)
{
	unsigned char data[2];
	data[0] = (c_lock >> 8) & 0xFF;
	data[1] = (c_lock & 0xFF);
	com_fram.WriteData(data, FRAM_ADDRESS_STRX_C_LOCK_REPLY_TIME, sizeof(data));

	data[0] = (s_lock >> 8) & 0xFF;
	data[1] = (s_lock & 0xFF);
	com_fram.WriteData(data, FRAM_ADDRESS_STRX_S_LOCK_REPLY_TIME, sizeof(data));

	data[0] = lock_rssi;
	com_fram.WriteData(data, FRAM_ADDRESS_STRX_LOCK_RSSI, 1);

	return Success;
}


ack FRAM_Write_tlmd_anomaly_ena(unsigned char ena)
{
	unsigned char data[1];
	data[0] = ena;
	if (ena > 1)
		return ValueError;
	com_fram.WriteData(data, FRAM_ADDRESS_TLMD_ANOMALY_ENA, sizeof(data));
	return Success;
}

ack FRAM_Write_dr_obc_timetag_ena(unsigned char ena)
{
	unsigned char data[1];
	data[0] = ena;
	if (ena > 1)
		return ValueError;
	com_fram.WriteData(data, FRAM_ADDRESS_DR_OBC_TIME_TAG, sizeof(data));
	return Success;
}

ack FRAM_Write_dr_msn_timetag_ena(unsigned char ena) 
{
	unsigned char data[1];
	data[0] = ena;
	if (ena > 1)
		return ValueError;
	com_fram.WriteData(data, FRAM_ADDRESS_DR_MSN_TIME_TAG, sizeof(data));
	return Success;
}

//Read process
ack FRAM_Read(unsigned char *data, unsigned int address, unsigned int size)
{
	com_fram.ReadData(data, address, size);

	return Success;
}

//Read process

ack FRAM_Read_verify(void)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_VERIFY, 1);

	if (data[0] == FRAM_VERIFY_CODE)
		return Success;

	else
		return ValueError;
}

ack FRAM_Read_ops_mode(MD_ModeID *current_mode, MD_ModeID *prev_mode)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_OPS_MODE, 1);
	*current_mode = (MD_ModeID)data[0];

	com_fram.ReadData(data, FRAM_ADDRESS_OPS_PREV_MODE, 1);
	*prev_mode = (MD_ModeID)data[0];

	return Success;
}

ack FRAM_Read_serial_mux(unsigned char *port)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_SERIAL_MUX, 1);

	*port = data[0];

	return Success;
}

ack FRAM_Read_rtc_sync_ena(ENABLE_STS *ena)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_RTC_SYNC_ENA, 1);

	*ena = (ENABLE_STS)data[0];

	if (*ena > 1)
		return ValueError;

	return Success;
}

ack FRAM_Read_uvc_ena(ENABLE_STS *ena)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_UVC_ENA, 1);

	*ena = (ENABLE_STS)data[0];

	Serial.print("UVC reboot: ");
	if (*ena == ENABLE)
		Serial.println("Enable");
	else
		Serial.println("Disable");

	if (*ena > 1)
		return ValueError;

	return Success;
}

ack FRAM_Read_uvc_threshold(unsigned char *v1, unsigned char *v2, unsigned char *v3)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_UVC1_THR, 1);
	*v1 = data[0];

	com_fram.ReadData(data, FRAM_ADDRESS_UVC2_THR, 1);
	*v2 = data[0];

	com_fram.ReadData(data, FRAM_ADDRESS_UVC3_THR, 1);
	*v3 = data[0];

	return Success;
}

ack FRAM_Read_PWR_STS(unsigned int address, unsigned char *pwr_sts)
{
	unsigned char data[1];
	com_fram.ReadData(data, address, 1);

	*pwr_sts = data[0];

	Serial.print("FRAM_PWR_STS: ");
	Serial.print(address);
	if (data[0] == 0x01)
		Serial.println(", ON_STS");
	else
		Serial.println(", OFF_STS");

	return Success;
}
ack FRAM_Read_master_cycle(unsigned int *time)
{
	int i;
	unsigned int tmp = 0;
	unsigned char data[4];
	com_fram.ReadData(data, FRAM_ADDRESS_MASTER_CYCLE, sizeof(data));

	for (i = 0; i<4; ++i)
	{
		tmp <<= 8;
		tmp += data[i];
	}
	*time = tmp;

	Serial.print("OBC_SHUTDOWN_TIME: ");
	Serial.println(tmp);

	return Success;
}

ack FRAM_Read_reboot_count(unsigned int *val)
{
	int i;
	unsigned int tmp = 0;
	unsigned char data[4];
	com_fram.ReadData(data, FRAM_ADDRESS_REBOOT_COUNT, sizeof(data));

	for (i = 0; i<4; ++i)
	{
		tmp <<= 8;
		tmp += data[i];
	}
	*val = tmp;

	Serial.print("COM REBOOT COUNT: ");
	Serial.println(tmp);

	return Success;
}

ack FRAM_Read_stx_on_off_duration(unsigned int *on_dur, unsigned int *off_dur)
{
	int i;
	unsigned int tmp = 0;
	unsigned char data[2];
	com_fram.ReadData(data, FRAM_ADDRESS_STX_ON_DURATION, sizeof(data));

	for (i = 0; i<2; ++i)
	{
		tmp <<= 8;
		tmp += data[i];
	}
	*on_dur = tmp;
	Serial.print("STX_ON duration: "); Serial.println(tmp);

	com_fram.ReadData(data, FRAM_ADDRESS_STX_OFF_DURATION, sizeof(data));

	tmp = 0;
	for (i = 0; i<2; ++i)
	{
		tmp <<= 8;
		tmp += data[i];
	}
	*off_dur = tmp;
	Serial.print("STX_OFF duration: "); Serial.println(tmp);
	Serial.println(tmp);

	return Success;
}

ack FRAM_Read_strx_otc_ena(unsigned char *ena)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_STRX_OTC_ENA, 1);

	*ena = data[0];

	if (*ena > 1)
		return ValueError;

	return Success;
}

ack FRAM_Read_strx_otc_thr(unsigned char *thr)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_STRX_OTC_THR, 1);

	*thr = data[0];

	return Success;
}

ack FRAM_Read_strx_power(unsigned char *pwr)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_STRX_POWER, 1);

	*pwr = data[0];

	return Success;
}


ack FRAM_Read_strx_lock_reply_time(unsigned int* c_lock, unsigned int* s_lock, unsigned char* lock_rssi)
{
	int i;
	unsigned int tmp = 0;
	unsigned char data[2];
	com_fram.ReadData(data, FRAM_ADDRESS_STRX_C_LOCK_REPLY_TIME, sizeof(data));

	for (i = 0; i < 2; ++i)
	{
		tmp <<= 8;
		tmp += data[i];
	}
	*c_lock = tmp;

	com_fram.ReadData(data, FRAM_ADDRESS_STRX_S_LOCK_REPLY_TIME, sizeof(data));

	tmp = 0;
	for (i = 0; i < 2; ++i)
	{
		tmp <<= 8;
		tmp += data[i];
	}
	*s_lock = tmp;


	com_fram.ReadData(data, FRAM_ADDRESS_STRX_LOCK_RSSI, 1);
	*lock_rssi = data[0];

	return Success;
}


ack FRAM_Read_tlmd_anomaly_ena(unsigned char* ena)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_TLMD_ANOMALY_ENA, 1);

	*ena = data[0];

	return Success;
}

ack FRAM_Read_dr_obc_timetag_ena(unsigned char* ena)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_DR_OBC_TIME_TAG, 1);

	*ena = data[0];

	return Success;
}

ack FRAM_Read_dr_msn_timetag_ena(unsigned char* ena)
{
	unsigned char data[1];
	com_fram.ReadData(data, FRAM_ADDRESS_DR_MSN_TIME_TAG, 1);

	*ena = data[0];

	return Success;
}

ack FRAM_All_Initialize(void)
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

		com_fram.WriteData(data, head, small_size);
		head += small_size;
	}

	Serial.println("FRAM ALL RESET");

	return Success;
}


//////////////////////////////////////////////////////
//add 20201207
ack FRAM_Write_restart_count(unsigned short count)
{
	unsigned char data[2];
	data[0] = (count >> 8) & 0xFF;
	data[1] = (count & 0xFF);
	com_fram.WriteData(data, FRAM_ADDRESS_RESTART_COUNT, sizeof(data));
	return Success;
}

ack FRAM_Write_auto_ctrl_info(unsigned char auto_ctrl_id, unsigned char bc_id, unsigned char activate, unsigned short exec_times)
{
	unsigned char c_data[1];
	unsigned char data[2];

	if (auto_ctrl_id > FRAM_AC_HEADER_MAX)
		return ValueError;

	c_data[0] = bc_id;
	com_fram.WriteData(c_data, auto_ctrl_id * FRAM_LENGTH_AC_HEADER + 
		FRAM_ADDRESS_AC_HEADER + FRAM_POSITION_AC_BC_ID, sizeof(c_data));

	c_data[0] = activate;
	com_fram.WriteData(c_data, auto_ctrl_id * FRAM_LENGTH_AC_HEADER +
		FRAM_ADDRESS_AC_HEADER + FRAM_POSITION_AC_BC_ACTIVATED, sizeof(c_data));

	data[0] = (exec_times >> 8) & 0xFF;
	data[1] = (exec_times & 0xFF);
	com_fram.WriteData(data, auto_ctrl_id * FRAM_LENGTH_AC_HEADER +
		FRAM_ADDRESS_AC_HEADER + FRAM_POSITION_AC_EXEC_TIMES, sizeof(data));

	return Success;
}

ack FRAM_Read_restart_count(unsigned short* times)
{
	int i;
	unsigned int tmp = 0;
	unsigned char data[2];
	com_fram.ReadData(data, FRAM_ADDRESS_RESTART_COUNT, sizeof(data));

	for (i = 0; i < 2; ++i)
	{
		tmp <<= 8;
		tmp += data[i];
	}
	*times = tmp;

	Serial.print("OBC RESTART COUNT: ");
	Serial.println(tmp);

	return Success;
}

ack FRAM_Read_auto_ctrl_info(unsigned char auto_ctrl_id, unsigned char* bc_id, unsigned char* activate, unsigned short* exec_times)
{
	int i;
	unsigned int tmp = 0;
	unsigned char c_data[1];
	unsigned char data[2];

	com_fram.ReadData(c_data, auto_ctrl_id * FRAM_LENGTH_AC_HEADER +
		FRAM_ADDRESS_AC_HEADER + FRAM_POSITION_AC_BC_ID, sizeof(c_data));

	*bc_id = c_data[0];

	com_fram.ReadData(c_data, auto_ctrl_id * FRAM_LENGTH_AC_HEADER +
		FRAM_ADDRESS_AC_HEADER + FRAM_POSITION_AC_BC_ACTIVATED, sizeof(c_data));

	*activate = c_data[0];

	com_fram.ReadData(data, auto_ctrl_id * FRAM_LENGTH_AC_HEADER +
		FRAM_ADDRESS_AC_HEADER + FRAM_POSITION_AC_EXEC_TIMES, sizeof(data));


	for (i = 0; i < 2; ++i)
	{
		tmp <<= 8;
		tmp += data[i];
	}

	*exec_times = tmp;

	return Success;
}