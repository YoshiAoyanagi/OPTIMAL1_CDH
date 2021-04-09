// 
// 
// 

#include "nop.h"
#include <stddef.h> // for NULL
#include "cmd_analyze_obc.h"

static void nop_(void);

AppInfo create_nop_task(void)
{
	return create_app_info("nop", 1, NULL, nop_);
}

static void nop_() {}

int Cmd_NOP(const CTCP* packet)
{
	(void)packet;
	return CMD_ERROR_NONE;
}
