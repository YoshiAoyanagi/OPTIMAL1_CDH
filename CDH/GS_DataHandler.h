// GS_DataHandler.h

#ifndef _GS_DATAHANDLER_h
#define _GS_DATAHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "Config.h"
#include "CommonSerialReceiver.h"
#include "RT_TlmDispatcher.h"
#include "PacketHandler.h"
#include "tlm_frame.h"
#include "OBC_TlmGenerator.h"

AppInfo gs_data_handler(void);

#endif