// FRAM_cmd_manager.h

#ifndef _FRAM_CMD_MANAGER_h
#define _FRAM_CMD_MANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "FRAM.h"
#include "TimeManager.h"
#include "Utility.h"
#include "ModeManager.h"
#include "port_config.h"
#include "PacketList.h"
#include "CommonSerialReceiver.h"
#include "CommonTlmCmdPacket.h"
#include "cmd_analyze_obc.h"

#define FRAM_TLC_MAX	62

/*
#define FRAM_CMD_VERIFY_CODE					0xAA
#define FRAM_CMD_ADDRESS_VERIFY					0x0000
#define FRAM_CMD_ADDRESS_TLC_QUEUE				0x0010

#define FRAM_CMD_ADDRESS_TLC_TIME_HEAD			0x0020
#define FRAM_CMD_ADDRESS_TLC_START				0x0200
*/

#define FRAM_CMD_VERIFY_CODE					0xAA
#define FRAM_CMD_ADDRESS_VERIFY					0x0100
#define FRAM_CMD_ADDRESS_TLC_QUEUE				0x0110

#define FRAM_CMD_ADDRESS_TLC_TIME_HEAD			0x0120
#define FRAM_CMD_ADDRESS_TLC_START				0x0300

extern unsigned int fram_tlc_exec_count;
extern unsigned int fram_tlc_queue;	//FRAM2Ç…ìoò^Ç≥ÇÍÇƒÇ¢ÇÈTLCêî
extern unsigned int fram_tlc_time[FRAM_TLC_MAX];

ack FRAM_TLC_Begin(void);
ack FRAM_TLC_Read_verify(void);

ack FRAM_TLC_Write_queue(unsigned int queue);
ack FRAM_TLC_Write_packet(const CTCP *packet);
ack FRAM_TLC_drop_time(unsigned int address);

PL_ACK FRAM_TLC_search_tlc(utime_t utime, unsigned char *data);

ack FRAM_TLC_Read_queue(void);
ack FRAM_TLC_Read_tlc_time(void);
ack FRAM_TLC_Read_packet(unsigned int p, unsigned char *raw_data);

ack FRAM_TLC_Clear_At_Address(unsigned int p);
ack FRAM_TLC_Clear_At_Time(utime_t utime);
ack FRAM_TLC_Clear_All(void);


//FRAM-BC
#define FRAM_S_BCT_MAX_BLOCK_LENGTH				14
#define FRAM_S_BCT_MAX_BLOCKS					32

#define FRAM_CMD_ADDRESS_SBC_IS_ACTIVE_HEAD		0x4100
#define FRAM_CMD_ADDRESS_SBC_TO_ID_HEAD			FRAM_CMD_ADDRESS_SBC_IS_ACTIVE_HEAD + 1 * FRAM_S_BCT_MAX_BLOCKS
#define FRAM_CMD_ADDRESS_SBC_CMD_ID_HEAD		FRAM_CMD_ADDRESS_SBC_TO_ID_HEAD + 1 * FRAM_S_BCT_MAX_BLOCKS  * FRAM_S_BCT_MAX_BLOCK_LENGTH
#define FRAM_CMD_ADDRESS_SBC_CYCLE_HEAD			FRAM_CMD_ADDRESS_SBC_CMD_ID_HEAD + 1 * FRAM_S_BCT_MAX_BLOCKS * FRAM_S_BCT_MAX_BLOCK_LENGTH
#define FRAM_CMD_ADDRESS_SBC_START				FRAM_CMD_ADDRESS_SBC_CYCLE_HEAD + 4 * FRAM_S_BCT_MAX_BLOCKS * FRAM_S_BCT_MAX_BLOCK_LENGTH

#define	SBC_ACTIVE		1
#define	SBC_INACTIVE	0


typedef struct
{
	unsigned char is_active;
	unsigned char bc_id;
	unsigned char to_id[FRAM_S_BCT_MAX_BLOCK_LENGTH];
	unsigned char cmd_id[FRAM_S_BCT_MAX_BLOCK_LENGTH];
	unsigned int cycle[FRAM_S_BCT_MAX_BLOCK_LENGTH];
}S_BC_PARAM;

extern S_BC_PARAM Stored_BC_Param;
extern unsigned int Stored_BC_Number;
extern unsigned int Stored_BC_Position;
extern unsigned char Stored_BC_Data[128];

ack FRAM_SBC_Write_packet(CTCP *packet);
ack FRAM_SBC_Set_Stored_BC_Number(unsigned int number, unsigned int pos);
ack FRAM_SBC_Read_BlockInfo(unsigned char bc_id);
ack FRAM_SBC_Read_packet(unsigned int bc_number, unsigned int pos, unsigned char *raw_data);
ack FRAM_SBC_Deploy_Block(unsigned char bc_id);
ack FRAM_SBC_ACTIVATE(unsigned char bc_id, unsigned char activate);

ack FRAM_SBC_ExtractData(unsigned char bc_id, unsigned char pos);

ack FRAM_SBC_Remove_Block(unsigned char bc_id);
ack FRAM_SBC_Remove_Block_At_PS(unsigned char bc_id, unsigned char pos);
ack FRAM_SBC_Remove_ALL_Block(void);

ack FRAM_TLC_Write_Test(void);
ack FRAM_TLC_All_Initialize(void);
#endif
