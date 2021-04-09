// AnomalyHandler.h

#ifndef _ANOMALYHANDLER_h
#define _ANOMALYHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <stddef.h> // for size_t

#include "AnomalyLogger.h"
#include "AppInfo.h"
#include "CommonTlmCmdPacket.h"

#define AH_MAX_RULES (16)

typedef enum
{
	AH_SINGLE,
	AH_CONTINUOUS,
	AH_CUMULATE
} AHCondition;

typedef struct
{
	AnomalyCode code;
	AHCondition cond;
	size_t threshold;
	size_t bc_id;
} AHRule;

typedef struct
{
	int is_active;
	AHRule rule;
	size_t counter;
}AHElement;

typedef struct
{
	size_t count;
	size_t pos;
}AHPointer;

typedef struct
{
	AHElement elements[AH_MAX_RULES];
	AHPointer al_pointer;
	OBCTime respond_at;
	size_t latest_id;
	size_t action_counter;
}AHInfo;

extern const AHInfo *ahi;

AppInfo AH_create_app(void);

void print_ah_status_(void);
AppInfo print_ah_status(void);

int Cmd_AH_register_rule(const CTCP* packet);

int Cmd_AH_activate_rule(const CTCP* packet);

int Cmd_AH_inactivate_rule(const CTCP* packet);

int Cmd_AH_clear_log(const CTCP* packet);

#endif // ANOMALY_HANDLER_H_
