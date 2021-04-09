// OBC_TlmGenerator.h

#ifndef _OBC_TLMGENERATOR_h
#define _OBC_TLMGENERATOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "CommonTlmCmdPacket.h"

extern unsigned int tlm_send_interval;
#define	TLM_INITAL_SEND_INTERVAL	2

AppInfo OBC_tlm_generator(void);
int OBC_TlmSend(unsigned char id);
int OBC_TlmRegister(unsigned char id);
ack OBC_Tlm_Set_tlm_interval(unsigned int interval_sec);
#endif

