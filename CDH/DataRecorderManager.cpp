// 
// 
// 

#include "DataRecorderManager.h"
#include "LoRaCmd_handler.h"


ack DR_initialize(SerialFlash serial_flash, DR_STRUCT *comp_dr, unsigned char cs)
{
	if (serial_flash.Begin(cs) == FLASH_STS_FAILED)
		return Error;

	DR_UpdateRecordSector(serial_flash, comp_dr); //update‚ÍƒRƒ}ƒ“ƒh‚ÅŽÀŽ{‚·‚é‚±‚Æ‚É‚·‚é

	comp_dr->enabled = 0;//disable

	comp_dr->replay_pointer = DR_Sector_Init(0, 0);
	comp_dr->replay_interval_time = 1000;
	comp_dr->replay_num_packet = 0;
	comp_dr->replay_enabled = 0;

	return (Success);
}

void DR_UpdateRecordSector(SerialFlash serial_flash, DR_STRUCT *comp_dr)
{
	*comp_dr = DR_Comp_Init(serial_flash, DR_BEGIN_SECTOR, DR_SECTOR_RANGE);
#ifdef MSG_DEBUG
	Debug_Serial.print("DR Init: ");
	Debug_Serial.print(comp_dr->write_pointer.sector); Debug_Serial.print(", "); Debug_Serial.println(comp_dr->written_packet_size);
#endif
	comp_dr->enabled = 0;//disable

	comp_dr->replay_pointer = DR_Sector_Init(0, 0);
	comp_dr->replay_interval_time = 1000;
	comp_dr->replay_num_packet = 0;
	comp_dr->replay_enabled = 0;
	return;
}
DR_STRUCT	DR_Comp_Init(SerialFlash serial_flash, unsigned short begin_sector, unsigned short range)
{
	DR_STRUCT comp_dr;

	comp_dr.begin_sector = DR_Sector_Init(begin_sector, 0);
	comp_dr.end_sector = DR_Sector_Init(begin_sector + range, 0);

	DR_GenWrittenPointer(serial_flash, &comp_dr);

	return comp_dr;
}

FLASH_SECTOR_STRUCT DR_Sector_Init(unsigned short begin_sector, unsigned short begin_packet_address)
{
	unsigned short begin_inner_address;
	FLASH_SECTOR_STRUCT init_sector;

	init_sector.sector = begin_sector;
	init_sector.packet_address = begin_packet_address;

	begin_inner_address = begin_packet_address * 256;
	init_sector.inner_address = begin_inner_address;

	return init_sector;
}

void DR_GenWrittenPointer(SerialFlash serial_flash, DR_STRUCT *comp_dr)
{
	unsigned short begin_sector = comp_dr->begin_sector.sector;
	unsigned short begin_address = comp_dr->begin_sector.inner_address;
	unsigned short end_sector = comp_dr->end_sector.sector;
	unsigned short end_address = comp_dr->end_sector.inner_address;

	unsigned short sector = begin_sector;
	unsigned short inner_address = begin_address;

	unsigned char read_buf[256];
	unsigned short read_size = 256;
	int empty_sector_count = 0;
	int empty_address_count = 0;

	while (1)
	{
		serial_flash.ReadData(read_buf, sector, inner_address, read_size);

		int i;
		empty_address_count = 0;
		for (i = 0; i < read_size; i++)
		{
			if (read_buf[i] == 0xFF || read_buf[i] == 0x00)
				empty_address_count++;
			else
				empty_sector_count = 0;	//reset
		}
		if (empty_address_count == read_size)
		{
			empty_sector_count++;
			if (empty_sector_count > DR_EMPTY_SECTOR_THR)
			{
				inner_address -= (DR_EMPTY_SECTOR_THR * read_size);
				break;
			}
		}

		unsigned int tmp_address = read_size + inner_address;

		if (tmp_address >= 0xFFFF)
		{
			inner_address = 0;
			sector++;
		}
		else
		{
			inner_address += read_size;
		}

		if (sector == end_sector && inner_address >= end_address)
		{
#ifdef MSG_DEBUG
			Debug_Serial.println("Full Stored data\n");
#endif
			break;
		}

	}
	comp_dr->write_pointer.sector = sector;
	comp_dr->write_pointer.inner_address = inner_address;
	comp_dr->write_pointer.packet_address = inner_address / 256;

	DR_GenWrittenSize(comp_dr);
	return;
}
void DR_GenWrittenSize(DR_STRUCT *comp_dr)
{
	unsigned short sector_size;
	unsigned short packet_size;
	unsigned short begin_sector = comp_dr->begin_sector.sector;
	unsigned short sector = comp_dr->write_pointer.sector;
	unsigned short inner_address = comp_dr->write_pointer.inner_address;

	if (begin_sector > sector)
		return;

	sector_size = sector - begin_sector;
	packet_size = (inner_address) / 256;

	comp_dr->written_sector_size = sector_size;
	comp_dr->written_packet_size = packet_size;
	return;
}


void DR_PacketWrite(SerialFlash serial_flash, DR_STRUCT *comp_dr, CTCP *packet)
{
	if (comp_dr->ena_timetag == 1)
	{
		utime_t now_time = TMGR_get_utime();
		CCP_set_tlm_time(packet, (cycle_t)now_time);
	}
	_DR_PacketWrite(serial_flash, comp_dr, packet);

	return;
}

void DR_EnableTimeTag(DR_STRUCT* comp_dr, unsigned char ena)
{
	comp_dr->ena_timetag = ena;
	return;
}

void _DR_PacketWrite(SerialFlash serial_flash, DR_STRUCT *comp_dr, CTCP *packet)
{
	unsigned char write_buf[256];
	unsigned int  write_size = 256;
	CCP_set_tlm_cmd_discrimination(packet, TLMCMD_DR);

	if (comp_dr->write_pointer.sector > 255)
	{
#ifdef MSG_DEBUG
		Debug_Serial.println("Flash ROM overflow");
#endif
		return;
	}

	int i;
	for (i = 0; i < write_size; i++)
	{
		if (i > packet->length)
			write_buf[i] = 0;

		else
		{
			write_buf[i] = packet->data[i];
		}
	}
	serial_flash.PageWrite(write_buf, comp_dr->write_pointer.sector,
		comp_dr->write_pointer.inner_address, write_size);

	unsigned int tmp_address = write_size + comp_dr->write_pointer.inner_address;
	
	if ((tmp_address + (write_size - 1)) > 0xFFFF)//if ((tmp_address + 0xFF) >= 0xFFFF)
	{
		comp_dr->write_pointer.inner_address = 0;
		(comp_dr->write_pointer.sector)++;
	}
	else
	{
		comp_dr->write_pointer.inner_address = tmp_address;
	}
	comp_dr->write_pointer.packet_address = (comp_dr->write_pointer.inner_address) / 256;
	DR_GenWrittenSize(comp_dr);

	return;
}

void DR_Replay(SerialFlash serial_flash, DR_STRUCT *comp_dr)
{
	if (comp_dr->replay_enabled != 1)
		return;

	static unsigned int num_read_packet;
	unsigned int replay_packet_size =
		DR_PacketRead(serial_flash, comp_dr->replay_pointer.sector, comp_dr->replay_pointer.inner_address);

	unsigned int tmp_address = comp_dr->replay_pointer.inner_address;

	if (tmp_address >= 0xFF00)
	{
		comp_dr->replay_pointer.inner_address = 0;
		comp_dr->replay_pointer.packet_address = 0;
		comp_dr->replay_pointer.sector += 1;
	}
	else
	{
		comp_dr->replay_pointer.inner_address += replay_packet_size;
		comp_dr->replay_pointer.packet_address++;
	}
	num_read_packet++;

	if (num_read_packet >= comp_dr->replay_num_packet)
	{
		comp_dr->replay_enabled = 0;
		num_read_packet = 0;
	}
	return;
}
unsigned int DR_PacketRead(SerialFlash serial_flash, unsigned short sector, unsigned short inner_address)
{
	PE_Info dr_pei;
	dr_pei = PE_initialize();

	int i;
	int i_ret;
	unsigned char read_buf[256];
	unsigned int read_size = 256;
	serial_flash.ReadData(read_buf, sector, inner_address, read_size);

	i_ret = PH_extract_packet(read_buf, read_size, &dr_pei);


	return (read_size);
}

void DR_SectorErase(SerialFlash serial_flash, DR_STRUCT *comp_dr, unsigned short start_sector, unsigned short num_sector)
{
	int tar;
	for (tar = 0; tar < num_sector; tar++)
	{
#ifdef MSG_DEBUG
		Debug_Serial.print("Sector Erase: ");
		Debug_Serial.println(tar + start_sector);
#endif
		serial_flash.EraseSector(start_sector);
		delay(1000);
	}
	return;
}

void DR_Single_SectorErase(SerialFlash serial_flash, DR_STRUCT *comp_dr, unsigned short tar_sector)
{
#ifdef MSG_DEBUG
	Debug_Serial.print("Sector Erase: ");
	Debug_Serial.println(tar_sector);
#endif

	serial_flash.EraseSector(tar_sector);
	delay(1000);
	return;
}