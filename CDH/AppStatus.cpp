// 
// 
// 

#include "AppStatus.h"


#include "utility.h"
#include "cmd_analyze_obc.h"
#include "AppManager.h"

static ASInfo asi_;
const ASInfo *asi;

static void AppStatus_init_(void);
static AS_ACK set_app_id_(size_t id);

AppInfo AppStatus(void)
{
	return create_app_info("as", 5, AppStatus_init_, NULL);
}

static void AppStatus_init_(void)
{
	asi = &asi_;
	asi_.app_id = 0;
}


int Cmd_AS_set_id(const CTCP* packet)
{
	AS_ACK ack = AS_UNKNOWN;

	if (CCP_get_length(packet) != 11 + 1)
	{
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	ack = set_app_id_((size_t)CCP_get_cmd_param(packet)[0]);
	switch (ack)
	{
	case AS_SUCCESS:
		return CMD_ERROR_NONE;
	case AS_INVALID_ID:
		return CMD_ERROR_ILLEGAL_PARAMETER;
	default:
		return CMD_ERROR_ILLEGAL_PARAMETER;
	}
}

static AS_ACK set_app_id_(size_t id)
{
	if (id >= AM_MAX_APPS) { return AS_INVALID_ID; }

	asi_.app_id = id;
	return AS_SUCCESS;
}
