// 
// 
// 

#include "OBCTime.h"

OBCTime OBCT_create(cycle_t master, cycle_t mode, step_t step)
{
	OBCTime temp;
	temp.master = master;
	temp.mode = mode;
	temp.step = step;
	return temp;
}

OBCTime OBCT_get_max(void)
{
	OBCTime max = { OBCT_MAX_CYCLE - 1, OBCT_MAX_CYCLE - 1, OBCT_STEPS_PER_CYCLE - 1 };
	return max;
}

void OBCT_clear(OBCTime* time)
{
	time->master = 0;
	time->mode = 0;
	time->step = 0;
	time->utime = 0;
	time->initial_utime = 0;
}

void OBCT_count_up(OBCTime* time)
{
	++(time->step);
	if (time->step == OBCT_STEPS_PER_CYCLE)
	{
		++(time->master);
		if (time->master == OBCT_MAX_CYCLE) { time->master = 0; }

		++(time->mode);
		if (time->mode == OBCT_MAX_CYCLE) { time->mode = 0; }

		time->step = 0;

		time->utime = (OBCT_STEP_IN_MSEC*OBCT_STEPS_PER_CYCLE * time->master) / 1000 + time->initial_utime;
	}
}

step_t OBCT_get_step(const OBCTime* time) { return (step_t)time->step; }

cycle_t OBCT_get_master_cycle(const OBCTime* time)
{
	return (cycle_t)time->master;
}

cycle_t OBCT_get_mode_cycle(const OBCTime* time)
{
	return (cycle_t)time->mode;
}

utime_t OBCT_get_utime(const OBCTime* time)
{
	return (cycle_t)time->utime;
}
unsigned int OBCT_get_master_in_msec(const OBCTime* time)
{
	return OBCT_STEP_IN_MSEC * (OBCT_STEPS_PER_CYCLE * time->master + time->step);
}

unsigned int OBCT_get_mode_in_msec(const OBCTime* time)
{
	return OBCT_STEP_IN_MSEC * (OBCT_STEPS_PER_CYCLE * time->mode + time->step);
}

cycle_t OBCT_sec2cycle(unsigned int sec) { return (1000 * sec) / (OBCT_STEP_IN_MSEC*OBCT_STEPS_PER_CYCLE); }

unsigned int OBCT_cycle2sec(cycle_t cycle) { return (OBCT_STEP_IN_MSEC*OBCT_STEPS_PER_CYCLE*cycle) / 1000; }

OBCTime OBCT_diff(const OBCTime* before,
	const OBCTime* after)
{
	OBCTime diff;

	// まずcycleの差分を計算
	diff.master = after->master - before->master;
	diff.mode = after->mode - before->mode;

	// stepのみで差分を考えればよい場合
	if (after->step > before->step)
	{
		diff.step = after->step - before->step;
	}
	// cycleからの桁借りが必要な場合
	else
	{
		diff.step = OBCT_STEPS_PER_CYCLE - before->step + after->step;
		--diff.master;
		--diff.mode;
	}

	return diff;
}

step_t OBCT_diff_in_step(const OBCTime* before,
	const OBCTime* after)
{
	cycle_t c_diff = after->master - before->master;
	step_t s_diff;
	if (after->step > before->step)
	{
		s_diff = after->step - before->step;
	}
	else
	{
		s_diff = OBCT_STEPS_PER_CYCLE - before->step + after->step;
		--c_diff;
	}

	return OBCT_STEPS_PER_CYCLE*c_diff + s_diff;
}

unsigned int OBCT_diff_in_msec(const OBCTime* before,
	const OBCTime* after)
{
	return OBCT_STEP_IN_MSEC * OBCT_diff_in_step(before, after);
}

float OBCT_diff_in_sec(const OBCTime* before,
	const OBCTime* after)
{
	return 0.001f * OBCT_diff_in_msec(before, after);
}

utime_t OBCT_cycle2utime(cycle_t cycle, utime_t initial_utime)
{
	cycle_t sec = (OBCT_STEP_IN_MSEC*OBCT_STEPS_PER_CYCLE * cycle) / 1000;
	utime_t utime = sec + initial_utime;
	return utime;
}

cycle_t OBCT_utime2cycle(utime_t utime, utime_t initial_utime)
{
	if (utime < initial_utime)
		return 0;

	cycle_t sec = utime - initial_utime;
	cycle_t cycle = (1000 * sec) / (OBCT_STEP_IN_MSEC*OBCT_STEPS_PER_CYCLE);
	return cycle;
}