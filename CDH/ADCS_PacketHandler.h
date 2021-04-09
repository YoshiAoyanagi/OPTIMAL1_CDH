// ADCS_PacketHandler.h

#ifndef _ADCS_PACKETHANDLER_h
#define _ADCS_PACKETHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "PacketHandler.h"
#include "PacketList.h"

AppInfo ADCS_packet_handler(void);
AppInfo ADCS_cmd_forward(void);

#endif

