// 
// 
// 

#include "TimeManager.h"
#include "FRAM_manager.h"
#include "RTC.h"

static OBCTime master_clock_;
const OBCTime* master_clock;

void TMGR_init(void)
{
	OBCT_clear(&master_clock_);
	master_clock = &master_clock_;

	ENABLE_STS ena;
	unsigned int reboot_cycle;

	FRAM_Read_rtc_sync_ena(&ena);

	if (ena == ENABLE)
	{
		TMGR_sync_OBC_time(rtc_time.unix);
		DEBUG_Serial.print("TMGR_sync_OBC_time(RTC): "); DEBUG_Serial.println(master_clock_.utime);
	}
	else
	{
		FRAM_Read_master_cycle(&reboot_cycle);
		TMGR_set_master_cycle((cycle_t)reboot_cycle + OBCT_REBOOT_OFFSET);
	}
}

void TMGR_clear(void)
{
	master_clock_.master = 0;
	master_clock_.utime = 0;
	master_clock_.initial_utime = 0;
}

void TMGR_clear_mode_cycle(void)
{
	master_clock_.mode = 0;
}

void TMGR_count_up(void)
{
	OBCT_count_up(&master_clock_);
}

unsigned int TMGR_get_master_in_msec(void)
{
	return OBCT_get_master_in_msec(&master_clock_);
}

unsigned int TMGR_get_mode_in_msec(void)
{
	return OBCT_get_mode_in_msec(&master_clock_);
}

cycle_t TMGR_get_master_cycle(void)
{
	return OBCT_get_master_cycle(&master_clock_);
}

step_t TMGR_get_step(void)
{
	return OBCT_get_step(&master_clock_);
}


utime_t TMGR_get_utime(void)
{
	return OBCT_get_utime(&master_clock_);
}
OBCTime TMGR_get_time(void)
{
	return master_clock_;
}

void TMGR_set_master_cycle(cycle_t master)
{
	master_clock_.master = master;
}

void TMGR_set_initial_utime(utime_t initial_utime)
{
	master_clock_.initial_utime = initial_utime;
}

int TMGR_sync_OBC_time(utime_t sync_time)
{
	unsigned int sec = OBCT_cycle2sec(master_clock_.master);

	if (sync_time < sec)
		return (-1);

	utime_t initial_utime = sync_time - sec;
	TMGR_set_initial_utime(initial_utime);
	master_clock_.utime =  (OBCT_STEP_IN_MSEC*OBCT_STEPS_PER_CYCLE * master_clock_.master) / 1000 + master_clock_.initial_utime;
	return 1;
}

unsigned int TMGR_ElapsedTime_in_cycle(cycle_t old_time_cycle)
{
	unsigned int elapsed_cycle = 0;
	
	cycle_t now_cycle = TMGR_get_master_cycle();

	if (now_cycle < old_time_cycle)
		return 0;

	elapsed_cycle = (unsigned int)(now_cycle - old_time_cycle);
	return elapsed_cycle;
}
