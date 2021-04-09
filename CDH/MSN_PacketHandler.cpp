// 
// 
// 

#include "MSN_PacketHandler.h"
#include "tlm_frame.h"
#include "LoRaCmd_handler.h"

SerialFlash serial_flash_msn(FLASH2_CS);
DR_STRUCT msn_dr;

static void MSN_cmd_forward_init_(void);
static void MSN_cmd_forward_(void);

static PE_Info msn_pei_;

static void MSN_packet_handler_init_(void);
static void MSN_packet_handler_(void);

static void MSN_DR_erase_init_(void);
static void MSN_DR_erase_handler_(void);
unsigned char msn_dr_erase_enabled;
unsigned short msn_dr_erase_tar_sector;
unsigned short msn_dr_erase_start_sector;
unsigned short msn_dr_erase_stop_sector;

unsigned short obcam_tlm_interval;

AppInfo MSN_packet_handler(void)
{
	return create_app_info("MSN_PH", 10, MSN_packet_handler_init_, MSN_packet_handler_);
}

static void MSN_packet_handler_init_(void)
{
	obcam_tlm_interval = 0;
	msn_pei_ = PE_initialize();

	unsigned char ena = 0;
	FRAM_Read_dr_msn_timetag_ena(&ena);
	DR_EnableTimeTag(&msn_dr, ena);
}

static void MSN_packet_handler_(void)
{
	unsigned char rec_data[256];
	int  rec_len = 0;
	int i;
	int i_ret = 0;
	
	ack result = Serial_DataReceive(PORT_MSN, rec_data, 0xFFFF, &rec_len);

	if (result == Success)
	{
		msn_pei_ = PE_initialize();
		i_ret = PH_extract_packet(rec_data, rec_len, &msn_pei_);
		if (i_ret == PH_FORWARDED)
		{
			DR_PacketWrite(serial_flash_msn, &msn_dr, &msn_pei_.packet);
		}

		if (i_ret == PH_FORWARDED_VIA_SF)
		{
			LoRa_TlmForward(msn_pei_.packet);
		}
	}
	else if (result == OtherProcess)
	{
#ifdef PRINT_DEBUG
		Serial.println("MSN_packet_handler_OtherProcess");
#endif
		LoRaCmdProcess(rec_data, rec_len);
	}

}

int MSN_TlmRegister(unsigned char id)
{
	// ”ÍˆÍŠO‚ÌTLM ID‚ðœŠO
	if (id > 0xff) { return CMD_ERROR_ILLEGAL_PARAMETER; }
	// ‘¶Ý‚µ‚È‚¢TLM ID‚ðœŠO
	if (tlm_packet_ptr[id] == NULL) { return CMD_ERROR_ILLEGAL_CONTEXT; }

	int ofs = MakeTlmPacket(id, TLM_REAL);
	if (ofs > 0)
	{
		CTCP tlm_packet = CCP_initialize(TlmTransferFrame.Packet.packet_data, (size_t)(TLM_BYTE_COUNT + ofs));
		if (PH_analyze_packet(&tlm_packet) == PH_FORWARDED)
		{
			DR_PacketWrite(serial_flash_msn, &msn_dr, &tlm_packet);
		}
	}
	return CMD_ERROR_NONE;
}

AppInfo MSN_cmd_forward(void)
{
	return create_app_info("MSN_PT", 1, MSN_cmd_forward_init_, MSN_cmd_forward_);
}

static void MSN_cmd_forward_init_(void)
{
	PL_clear_list(&to_msn_cmd_list);
}

static void MSN_cmd_forward_(void)
{
	CTCP *packet = NULL;
	if (PL_is_empty(&to_msn_cmd_list)) { return; }
	packet = &(PL_get_head(&to_msn_cmd_list)->packet);
	
	int i;
	for (i = 0; i < packet->length; i++)
	{
		MSN_Serial.write(packet->data[i]);
	}

#ifdef MSG_DEBUG
	DEBUG_Serial.println("MSN_CMD");
#endif

	PL_drop_head(&to_msn_cmd_list);
}

AppInfo MSN_DR_erase_handler(void)
{
	return create_app_info("MSN_ER", 1, MSN_DR_erase_init_, MSN_DR_erase_handler_);
}

static void MSN_DR_erase_init_(void)
{
	msn_dr_erase_enabled = 0;
	msn_dr_erase_tar_sector = 0;
	msn_dr_erase_start_sector = 0;
	msn_dr_erase_stop_sector = 0;
}

static void MSN_DR_erase_handler_(void)
{
	static unsigned int m;
	if (msn_dr_erase_enabled)
	{
		if (m >= (1500 / OBCT_CYCLES_IN_MSEC))
		{
			m = 0;
			DR_Single_SectorErase(serial_flash_msn, &msn_dr, msn_dr_erase_tar_sector);
			DR_UpdateRecordSector(serial_flash_msn, &msn_dr);
			if (msn_dr_erase_tar_sector == 0 || msn_dr_erase_tar_sector == msn_dr_erase_start_sector)
			{
				msn_dr_erase_enabled = 0;
				return;
			}
			else
			{
				msn_dr_erase_tar_sector--;
			}
		}
		m++;
	}
}

void MSN_DR_Set_EraseMode(unsigned short start_sector, unsigned short num_sector)
{
	msn_dr_erase_tar_sector = start_sector + num_sector;
	msn_dr_erase_start_sector = start_sector;
	msn_dr_erase_stop_sector = start_sector + num_sector;

	msn_dr_erase_enabled = 1;
}

void MSN_DR_Stop_EraseMode(void)
{
	msn_dr_erase_enabled = 0;
}


void MSN_DR_Erase(unsigned short start_sector, unsigned short num_sector)
{
	DR_SectorErase(serial_flash_msn, &msn_dr, start_sector, num_sector);
	DR_UpdateRecordSector(serial_flash_msn, &msn_dr);
}

void MSN_DR_Replay(void)
{
	DR_Replay(serial_flash_msn, &msn_dr);
}

void MSN_DR_Initialize(void)
{
	static int p;
	if (p == 0)
	{
		DR_initialize(serial_flash_msn, &msn_dr, FLASH2_CS);
		p = 1;
	}
}

void OBCAM_Set_interval(unsigned int interval_sec)
{
	obcam_tlm_interval = interval_sec;
}