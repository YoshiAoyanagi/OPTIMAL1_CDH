// 
// 
// 

#include "AppInfo.h"
#include <stddef.h> // for NULL

AppInfo create_app_info(const char* name,
	step_t proc_time,
	void(*initializer)(void),
	void(*entry_point)(void))
{
	AppInfo ai;

	ai.name = name;
	ai.proc_time = proc_time;
	ai.prev = 0;
	ai.max = 0;
	ai.min = 0xffffffff;
	ai.initializer = initializer;
	ai.entry_point = entry_point;

	return ai;
}
