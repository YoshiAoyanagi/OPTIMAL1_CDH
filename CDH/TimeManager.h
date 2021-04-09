// TimeManager.h

#ifndef _TIMEMANAGER_h
#define _TIMEMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#include "OBCTime.h"

extern const OBCTime* master_clock;

void TMGR_init(void);
void TMGR_clear(void);
void TMGR_clear_mode_cycle(void);
void TMGR_count_up(void);

unsigned int TMGR_get_master_in_msec(void);
unsigned int TMGR_get_mode_in_msec(void);

cycle_t TMGR_get_master_cycle(void);
utime_t TMGR_get_utime(void);
step_t TMGR_get_step(void);
OBCTime TMGR_get_time(void);

void TMGR_set_master_cycle(cycle_t master);
void TMGR_set_initial_utime(utime_t initial_utime);
int TMGR_sync_OBC_time(utime_t sync_time);

unsigned int TMGR_ElapsedTime_in_cycle(cycle_t old_time_cycle);

#endif

