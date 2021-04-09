// CDHTime.h

#ifndef _OBCTIME_h
#define _OBCTIME_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#define OBCT_REBOOT_OFFSET	(0)

#define OBCT_STEP_IN_MSEC	(1)
#define OBCT_STEPS_PER_CYCLE (50)//
#define OBCT_CYCLES_PER_SEC			(1000/OBCT_STEP_IN_MSEC/OBCT_STEPS_PER_CYCLE)		//20Hz
#define OBCT_CYCLES_IN_MSEC	(1000 / OBCT_CYCLES_PER_SEC)								//50msec 
#define OBCT_CYCLES_PER_SEC_IN_BC	(1000/OBCT_STEP_IN_MSEC/OBCT_STEPS_PER_CYCLE/10)	//=100
#define OBCT_MAX_CYCLE (0xfffffff0u)


typedef unsigned int cycle_t;
typedef unsigned int step_t;
typedef unsigned int utime_t;

typedef struct
{
	cycle_t master;
	cycle_t mode;
	step_t step;
	utime_t utime;
	utime_t initial_utime;
} OBCTime;

OBCTime OBCT_create(cycle_t master, cycle_t mode, step_t step);
OBCTime OBCT_get_max(void);
void	OBCT_clear(OBCTime* time);
void	OBCT_count_up(OBCTime* time);

step_t	OBCT_get_step(const OBCTime* time);
cycle_t OBCT_get_master_cycle(const OBCTime* time);
cycle_t OBCT_get_mode_cycle(const OBCTime* time);
utime_t OBCT_get_utime(const OBCTime* time);

cycle_t OBCT_sec2cycle(unsigned int sec);
unsigned int OBCT_cycle2sec(cycle_t cycle);

unsigned int OBCT_get_master_in_msec(const OBCTime* time);
unsigned int OBCT_get_mode_in_msec(const OBCTime* time);

OBCTime OBCT_diff(const OBCTime* before,
	const OBCTime* after);
step_t OBCT_diff_in_step(const OBCTime* before,
	const OBCTime* after);
float OBCT_diff_in_sec(const OBCTime* before,
	const OBCTime* after);

utime_t OBCT_cycle2utime(cycle_t cycle, utime_t initial_utime);
cycle_t OBCT_utime2cycle(utime_t utime, utime_t initial_utime);

#endif

