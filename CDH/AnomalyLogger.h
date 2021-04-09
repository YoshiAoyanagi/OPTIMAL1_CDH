// AnomalyLogger.h

#ifndef _ANOMALYLOGGER_h
#define _ANOMALYLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#include <stddef.h> // for size_t

#include "OBCTime.h"

#define AL_RECORD_MAX (100)

typedef enum
{
	AL_ANOMALY_LOGGER,
	AL_MODE_MANAGER,
	AL_TASK_MANAGER,
	AL_TASK_DISPATCHER,
	AL_APP_MANAGER,
	AL_CORE_GROUP_MAX
} AL_CORE_GROUP;

typedef enum
{
	AL_UVC = AL_CORE_GROUP_MAX,
	AL_SSD
} AL_GROUP;

typedef enum
{
	AL_SUCCESS,
	AL_FULL
} AL_ACK;

typedef struct
{
	unsigned int group;
	unsigned int local;
} AnomalyCode;

typedef struct
{
	OBCTime time;
	AnomalyCode code;
	size_t run_length;
} AnomalyRecord;

typedef struct
{
	size_t counter;
	size_t header;
	AnomalyRecord records[AL_RECORD_MAX];
} AnomalyLogger;

extern const AnomalyLogger* al;

void AL_initialize(void);

void AL_add_anomaly(unsigned int group,
	unsigned int local);

size_t AL_get_counter(void);

size_t AL_get_latest_pos(void);

const AnomalyRecord* AL_get_record(size_t pos);

const AnomalyRecord* AL_get_latest_record(void);

void AL_clear(void);

#endif // ANOMALY_LOGGER_H_
