// MSN_PacketHandler.h

#ifndef _MSN_PACKETHANDLER_h
#define _MSN_PACKETHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "PacketHandler.h"
#include "PacketList.h"
#include "DataRecorderManager.h"

extern unsigned short obcam_tlm_interval;

AppInfo MSN_packet_handler(void);
AppInfo MSN_cmd_forward(void);
AppInfo MSN_DR_erase_handler(void);

void MSN_DR_Erase(unsigned short start_sector, unsigned short num_sector);
void MSN_DR_Replay(void);
void MSN_DR_Initialize(void);

void MSN_DR_Set_EraseMode(unsigned short start_sector, unsigned short num_sector);
void MSN_DR_Stop_EraseMode(void);
int MSN_TlmRegister(unsigned char id);
void OBCAM_Set_interval(unsigned int interval_sec);

#endif

