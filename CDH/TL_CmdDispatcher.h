// TL_CmdDispatcher.h

#ifndef _TL_CMDDISPATCHER_h
#define _TL_CMDDISPATCHER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "PacketHandler.h"
#include "CommonTlmCmdPacket.h"
#include "AppInfo.h"

extern const CTCP* tl_list_for_tlm[TL_CMD_LIST_MAX];

AppInfo tl_cmd_dispatcher(void);

int Cmd_CLEAR_ALL_TIMELINE(const CTCP* packet);
int Cmd_CLEAR_TIMELINE_AT(const CTCP* packet);
int Cmd_UPDATE_TIMELINE_TLM(const CTCP* packet);
int Cmd_DEPLOY_BLOCK(const CTCP* packet);

#endif

