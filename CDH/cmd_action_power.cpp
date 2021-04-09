// 
// 
// 

#include "cmd_action_power.h"
#include "tlm_frame.h"
#include "EPS.h"
#include "OBC_TlmGenerator.h"
#include "UVC_app.h"

int Cmd_EPS_RESET(const CTCP *packet)
{
	GsTlmDsc = TLM_DSC_ACK;
	OBC_TlmSend(TLM_ID_01);
	if (EPS_RESET() != Success)
		return CMD_ERROR_ILLEGAL_CONTEXT;

	return CMD_ERROR_NONE;
}

int Cmd_EPS_SELECT(const CTCP *packet)
{
	unsigned char ch = packet->data[CMD_POSITION_PARAM];
	int ofs;

	if (ch > 2) { return CMD_ERROR_ILLEGAL_PARAMETER; }

	EPS_SELECT(ch);
	return CMD_ERROR_NONE;
}
int Cmd_EPS_UVC_ENABLE(const CTCP* packet)
{
	unsigned char en = packet->data[CMD_POSITION_PARAM];
	UVC_enable(en);
	return CMD_ERROR_NONE;
}

int Cmd_EPS_UVC_SET_BAT_V(const CTCP* packet)
{
	unsigned char v1, v2, v3;
	unsigned char raw_val[4];
	float v;
	int i, j;
	int ofs = 0;

	v1 = packet->data[CMD_POSITION_PARAM + ofs]; ofs++;
	v2 = packet->data[CMD_POSITION_PARAM + ofs]; ofs++;
	v3 = packet->data[CMD_POSITION_PARAM + ofs]; ofs++;

	if(UVC_set(v1, v2, v3) != 0)
		return CMD_ERROR_ILLEGAL_PARAMETER;
	
	return CMD_ERROR_NONE;
}
int Cmd_POWER_SAFE_OFF(const CTCP* packet)
{
	PWR_SAFE_OFF();
	return CMD_ERROR_NONE;
}
int Cmd_ADCS_POWER(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_ADCS(pwr_onoff);
	return CMD_ERROR_NONE;
}

int Cmd_UNREG_POWER(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_UNREG(pwr_onoff);
	return CMD_ERROR_NONE;
}

int Cmd_RW_POWER(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_RW(pwr_onoff);
	return CMD_ERROR_NONE;
}
int Cmd_HEATER_POWER(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_HEATER(pwr_onoff);
	return CMD_ERROR_NONE;
}
int Cmd_RW_ENABLE(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_RW_ENABLE(pwr_onoff);
	return CMD_ERROR_NONE;
}
int Cmd_MSN_unreg_ENABLE(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_MSN_UNREG_ENABLE(pwr_onoff);
	return CMD_ERROR_NONE;
}
int Cmd_MSN_5V_POWER(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_MSN5V(pwr_onoff);
	return CMD_ERROR_NONE;
}

int Cmd_AQU_5V_POWER(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_AQU_5V(pwr_onoff);
	return CMD_ERROR_NONE;
}

int Cmd_OVCO_5V_POWER(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_OVCO_5V(pwr_onoff);
	return CMD_ERROR_NONE;
}


int Cmd_SUBCAM_POWER(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_SUBCAM(pwr_onoff);
	return CMD_ERROR_NONE;
}
int Cmd_SF_POWER(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	PWR_SF(pwr_onoff);
	return CMD_ERROR_NONE;
}
int Cmd_MTQ_POWER_OBC(const CTCP* packet)
{
	unsigned char pwr_onoff = packet->data[CMD_POSITION_PARAM];
	//PWR_MTQ(pwr_onoff);
	return CMD_ERROR_NONE;
}

int Cmd_STRX_POWER(const CTCP* packet)
{
	PWR_SRX_RESET();
	return CMD_ERROR_NONE;
}

int Cmd_SRX_RESET(const CTCP* packet)
{
	PWR_SRX_RESET();
	return CMD_ERROR_NONE;
}


int Cmd_POWER_REBOOT(const CTCP* packet)
{
	unsigned char pwr_onoff;

	if(FRAM_Read_PWR_STS(FRAM_ADDRESS_PWR_UNREG, &pwr_onoff) == Success)
		PWR_UNREG(pwr_onoff);
	else
		PWR_UNREG(PWR_OFF);

	if (FRAM_Read_PWR_STS(FRAM_ADDRESS_PWR_HEATER, &pwr_onoff) == Success)
		PWR_HEATER(pwr_onoff);
	else
		PWR_HEATER(PWR_OFF);

	if (FRAM_Read_PWR_STS(FRAM_ADDRESS_PWR_RW, &pwr_onoff) == Success)
		PWR_RW(pwr_onoff);
	else
	{
		PWR_RW(PWR_OFF);
		Serial.println("FRAM_PWR_REBOOT ERROR");
	}
		
	if (FRAM_Read_PWR_STS(FRAM_ADDRESS_PWR_ADCS, &pwr_onoff) == Success)
		PWR_ADCS(pwr_onoff);
	else
		PWR_ADCS(PWR_OFF);

	if (FRAM_Read_PWR_STS(FRAM_ADDRESS_PWR_MSN5V, &pwr_onoff) == Success)
		PWR_MSN5V(pwr_onoff);
	else
		PWR_MSN5V(PWR_OFF);

	if (FRAM_Read_PWR_STS(FRAM_ADDRESS_PWR_SUBCAM, &pwr_onoff) == Success)
		PWR_SUBCAM(pwr_onoff);
	else
		PWR_SUBCAM(PWR_OFF);

	if (FRAM_Read_PWR_STS(FRAM_ADDRESS_PWR_SF, &pwr_onoff) == Success)
		PWR_SF(pwr_onoff);
	else
		PWR_SF(PWR_OFF);

	if (FRAM_Read_PWR_STS(FRAM_ADDRESS_PWR_MSN_UNREG_EN, &pwr_onoff) == Success)
		PWR_MSN_UNREG_ENABLE(pwr_onoff);
	else
		PWR_MSN_UNREG_ENABLE(PWR_OFF);

	if (FRAM_Read_PWR_STS(FRAM_ADDRESS_PWR_RW_EN, &pwr_onoff) == Success)
		PWR_RW_ENABLE(pwr_onoff);
	else
		PWR_RW_ENABLE(PWR_OFF);

	return CMD_ERROR_NONE;
}


