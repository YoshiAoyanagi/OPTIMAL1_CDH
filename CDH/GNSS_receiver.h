// GNSS_PacketHandler.h

#ifndef _GNSS_PACKETHANDLER_h
#define _GNSS_PACKETHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "AppHeaders.h"
#include "Utility.h"

AppInfo GNSS_receiver(void);
void GNSS_DebugMessage(void);

extern position_t gnss_pot;
extern UTC_STRUCT gnss_utc;
extern float gnss_hdop;
extern unsigned char gnss_quality;
extern unsigned char gnss_visible_sat;

#endif

