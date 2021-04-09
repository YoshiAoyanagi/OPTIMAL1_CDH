// cmd_action_adcs.h

#ifndef _CMD_ACTION_ADCS_h
#define _CMD_ACTION_ADCS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CommonTlmCmdPacket.h"
#include "MTQ_OBC.h"

int Cmd_OBC_MTQ_X(const CTCP *packet);
int Cmd_OBC_MTQ_Y(const CTCP *packet);
int Cmd_OBC_MTQ_Z(const CTCP *packet);

#endif

