// 
// 
// 

#include "cmd_action_adcs.h"
#include "cmd_analyze_obc.h"


int Cmd_OBC_MTQ_X(const CTCP *packet)
{
	char sts = packet->data[CMD_POSITION_PARAM];
	mtq_sts.x = sts;
	MTQ_OutputUpdate();
	return CMD_ERROR_NONE;
}

int Cmd_OBC_MTQ_Y(const CTCP *packet)
{
	char sts = packet->data[CMD_POSITION_PARAM];
	mtq_sts.y = sts;
	MTQ_OutputUpdate();
	return CMD_ERROR_NONE;
}

int Cmd_OBC_MTQ_Z(const CTCP *packet)
{
	char sts = packet->data[CMD_POSITION_PARAM];
	mtq_sts.z = sts;
	MTQ_OutputUpdate();
	return CMD_ERROR_NONE;
}