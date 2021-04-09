// Stored_TlmDispatcher.h

#ifndef _STORED_TLMDISPATCHER_h
#define _STORED_TLMDISPATCHER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "AppInfo.h"
#include "CommonTlmCmdPacket.h"
#include "DataRecorderManager.h"

extern DR_STRUCT hk_dr;
extern unsigned int st_tlm_send_interval;

AppInfo st_tlm_generator(void);
void st_tlm_erase(unsigned short start_sector, unsigned short num_sector);
void st_tlm_set_interval(unsigned int interval_sec);
void OBC_DR_Initialize(void);
void OBC_DR_PacketWrite(CTCP *packet);
#endif

