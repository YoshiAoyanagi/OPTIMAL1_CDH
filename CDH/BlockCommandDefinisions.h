// BlockCommandDefinisions.h

#ifndef _BLOCKCOMMANDDEFINISIONS_h
#define _BLOCKCOMMANDDEFINISIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "NormalTL.h"
#include "TlmCmdTL.h"
#include "PeripheralTL.h"
#include "SafeSL.h"
#include "NormalSL.h"
#include "InitialSL.h"
#include "cmd_item.h"

typedef enum
{
	BC_SL_ANY_TO_SAFE,
	BC_SL_ANY_TO_INITIAL,
	BC_SL_ANY_TO_NORMAL,
	BC_TL_NORMAL,
	BC_AR_TLMCMD,
	BC_AR_PERIPHERAL,
	BC_UVC_LV1_ACTION,
	BC_UVC_LV2_ACTION,
	BC_UVC_RTN_ACTION
} BC_DEFAULTS;

void BC_load_defaults(void);

#endif // BLOCK_COMMAND_DEFINISIONS_H_