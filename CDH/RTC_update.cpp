// 
// 
// 

#include "RTC_update.h"

void RTC_update_init_(void);
void RTC_update_(void);


AppInfo RTC_update(void)
{
	return create_app_info("RTC_UD", 10, RTC_update_init_, RTC_update_);
}

void RTC_update_init_(void)
{
	RTC_get_UNIX_TIME();
}

void RTC_update_(void)
{
	static unsigned int interval;
	static unsigned int cycle;

	if (TMGR_get_master_cycle() < cycle)
		interval = 0;

	cycle = TMGR_get_master_cycle();

	if (cycle > ((1 * OBCT_CYCLES_PER_SEC) * interval))
	{
		RTC_get_UNIX_TIME();
		interval++;
	}
}