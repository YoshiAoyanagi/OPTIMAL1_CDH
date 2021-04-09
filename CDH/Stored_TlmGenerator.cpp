// 
// 
// 

#include "Stored_TlmGenerator.h"


#include <string.h> // for memcmp

#include "utility.h"
#include "PacketHandler.h"
#include "cmd_analyze_obc.h"
#include "tlm_frame.h"
#include "BlockCommandDefinisions.h"
#include "MSN_PacketHandler.h"

void st_tlm_initialize_(void);
void st_tlm_generator_(void);
unsigned int st_tlm_send_interval;

SerialFlash serial_flash_hk(FLASH1_CS);
DR_STRUCT hk_dr;


static void ST_DR_erase_handler_(void);
unsigned char st_dr_erase_enabled;
unsigned short st_dr_erase_tar_sector;
unsigned short st_dr_erase_start_sector;
unsigned short st_dr_erase_stop_sector;

AppInfo st_tlm_generator(void)
{
	return create_app_info("ST_TLM_GEN", 2, st_tlm_initialize_, st_tlm_generator_);
}

void st_tlm_initialize_(void)
{
	st_tlm_send_interval = TLM_INITAL_SEND_INTERVAL;

	st_dr_erase_enabled = 0;
	st_dr_erase_tar_sector = 0;
	st_dr_erase_start_sector = 0;
	st_dr_erase_stop_sector = 0;

	unsigned char ena = 0;
	FRAM_Read_dr_obc_timetag_ena(&ena);
	DR_EnableTimeTag(&hk_dr, ena);
}

void st_tlm_generator_(void)
{
	int ofs;

	static unsigned int c;
	if (hk_dr.enabled)
	{
		if (c >= (st_tlm_send_interval * OBCT_CYCLES_PER_SEC))
		{
			c = 0;
			ofs = MakeTlmPacket(TLM_ID_01, TLM_DR);
			if (ofs > 0)
			{
				CTCP tlm_packet = CCP_initialize(TlmTransferFrame.Packet.packet_data, (size_t)(TLM_BYTE_COUNT + ofs));
				DR_PacketWrite(serial_flash_hk, &hk_dr, &tlm_packet);
			}
		}
		c++;
	}

	static unsigned int d;
	if (hk_dr.replay_enabled)
	{
		if (d >= (hk_dr.replay_interval_time / OBCT_CYCLES_IN_MSEC))
		{
			d = 0;
			DR_Replay(serial_flash_hk, &hk_dr);
		}
		d++;
	}

	static unsigned int m;
	if (msn_dr.replay_enabled)
	{
		if (m >= (msn_dr.replay_interval_time / OBCT_CYCLES_IN_MSEC))
		{
			m = 0;
			MSN_DR_Replay();
		}
		m++;
	}

	ST_DR_erase_handler_();
}

void st_tlm_erase(unsigned short start_sector, unsigned short num_sector)
{
	//DR_SectorErase(serial_flash_hk, &hk_dr, start_sector, num_sector);

	st_dr_erase_tar_sector = start_sector + num_sector;
	st_dr_erase_start_sector = start_sector;
	st_dr_erase_stop_sector = start_sector + num_sector;

	st_dr_erase_enabled = 1;

}

static void ST_DR_erase_handler_(void)
{
	static unsigned int m;
	if (st_dr_erase_enabled)
	{
		if (m >= (1500 / OBCT_CYCLES_IN_MSEC))
		{
			m = 0;
			DR_Single_SectorErase(serial_flash_hk, &hk_dr, st_dr_erase_tar_sector);
			DR_UpdateRecordSector(serial_flash_hk, &hk_dr);
			if (st_dr_erase_tar_sector == 0 || st_dr_erase_tar_sector == st_dr_erase_start_sector)
			{
				st_dr_erase_enabled = 0;
				return;
			}
			else
			{
				st_dr_erase_tar_sector--;
			}
		}
		m++;
	}
}

void st_tlm_set_interval(unsigned int interval_sec)
{
	st_tlm_send_interval = interval_sec;
}

void OBC_DR_PacketWrite(CTCP *packet)
{
	CCP_set_tlm_cmd_discrimination(packet, TLMCMD_DR);

	//timetag

	if (hk_dr.enabled)
	{
		DR_PacketWrite(serial_flash_hk, &hk_dr, packet);
	}
}

void OBC_DR_Initialize(void)
{
	static int p;
	if (p == 0)
	{
		DR_initialize(serial_flash_hk, &hk_dr, FLASH1_CS);
		p = 1;
	}
}