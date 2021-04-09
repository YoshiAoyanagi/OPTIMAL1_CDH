// DataRecorderManager.h

#ifndef _DATARECORDERMANAGER_h
#define _DATARECORDERMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "SerialFlash.h"
#include "Utility.h"
#include "port_config.h"
#include "CommonTlmCmdPacket.h"
#include "PacketHandler.h"
#include "FRAM_manager.h"

#define TARGET_DR_HK	0
#define TARGET_DR_B3	1

typedef	struct {
	unsigned short	sector;
	unsigned short	inner_address;
	unsigned short	packet_address;
}	FLASH_SECTOR_STRUCT;

typedef	struct {
	FLASH_SECTOR_STRUCT	begin_sector;
	FLASH_SECTOR_STRUCT	end_sector;
	FLASH_SECTOR_STRUCT	write_pointer;
	unsigned char		enabled;				//0->disable, 1->enable
	unsigned short		written_sector_size;
	unsigned short		written_packet_size;
	FLASH_SECTOR_STRUCT	replay_pointer;			//initial: 0
	unsigned short		replay_interval_time;	//initial: 50ms
	unsigned short		replay_num_packet;		//initial: 0
	unsigned char		replay_enabled;			//0->disable, 1->enable
	unsigned char		ena_timetag;			//1->OBC UTIME‚ðTLM_TIME‚É“ü‚ê‚é
}	DR_STRUCT;

#define DR_EMPTY_SECTOR_THR		3

//16MB x 2
#define	DR_SECTOR_RANGE		255	//255*64KB = 16MB
#define	DR_BEGIN_SECTOR		0

extern DR_STRUCT			hk_dr;
extern DR_STRUCT			msn_dr;


ack DR_initialize(SerialFlash serial_flash, DR_STRUCT *comp_dr, unsigned char cs);
DR_STRUCT	DR_Comp_Init(SerialFlash serial_flash, unsigned short begin_sector, unsigned short range);
FLASH_SECTOR_STRUCT DR_Sector_Init(unsigned short begin_sector, unsigned short begin_address);
void DR_UpdateRecordSector(SerialFlash serial_flash, DR_STRUCT *comp_dr);

void DR_GenWrittenPointer(SerialFlash serial_flash, DR_STRUCT *comp_dr);
void DR_GenWrittenSize(DR_STRUCT *comp_dr);

void DR_PacketWrite(SerialFlash serial_flash, DR_STRUCT *comp_dr, CTCP *packet);
void _DR_PacketWrite(SerialFlash serial_flash, DR_STRUCT *comp_dr, CTCP *packet);

void DR_Replay(SerialFlash serial_flash, DR_STRUCT *comp_dr);
unsigned int  DR_PacketRead(SerialFlash serial_flash, unsigned short sector, unsigned short inner_address);
void DR_SectorErase(SerialFlash serial_flash, DR_STRUCT *comp_dr, unsigned short start_sector, unsigned short num_sector);
void DR_Single_SectorErase(SerialFlash serial_flash, DR_STRUCT *comp_dr, unsigned short tar_sector);

void DR_EnableTimeTag(DR_STRUCT* comp_dr, unsigned char ena);

#endif

