// 
// 
// 

#include "EPS_update.h"

static void EPS_init_(void);
static void EPS_update_(void);

AppInfo EPS_update(void)
{
	return create_app_info("EPSU", 10, EPS_init_, EPS_update_);
}

static void EPS_init_(void)
{
	EPS_init();
}

static void EPS_update_(void)
{
	EPS_GetStatus();
}

void EPS_DebugMessage(void)
{
	PCU_DebugMessage();
	//PDU_DebugMessage();
	//EPS_PWR_DebugMessage();

	return;
}
void EPS_PWR_DebugMessage(void)
{
	Serial.print("BAT_V: "); Serial.print(eps_bat_v, 2); Serial.print(", ");

	Serial.print("UNREG_PWR: ");	if (pwr_sts.msn_unreg == PWR_ON) { Serial.print("ON, "); }
	else { Serial.print("OFF, "); }
	Serial.print("HEATER_PWR: ");	if (pwr_sts.heater == PWR_ON) { Serial.print("ON, "); }
	else { Serial.print("OFF, "); }
	Serial.print("RW_PWR: ");		if (pwr_sts.rw == PWR_ON) { Serial.print("ON, "); }
	else { Serial.print("OFF, "); }
	Serial.print("ADCS_PWR: ");		if (pwr_sts.adcs == PWR_ON) { Serial.print("ON, "); }
	else { Serial.print("OFF, "); }
	Serial.print("MSN5V_PWR: ");	if (pwr_sts.msn5v == PWR_ON) { Serial.print("ON, "); }
	else { Serial.print("OFF, "); }
	Serial.print("SUBCAM_PWR: ");	if (pwr_sts.subcam == PWR_ON) { Serial.print("ON, "); }
	else { Serial.print("OFF, "); }
	Serial.print("SF_PWR: ");		if (pwr_sts.sf == PWR_ON) { Serial.print("ON, "); }
	else { Serial.print("OFF, "); }
	Serial.print("MSN_UNREG_PWR(EN): ");if (pwr_sts.msn_unreg_en == PWR_ON) { Serial.print("ON, "); }
	else { Serial.print("OFF, "); }
	Serial.print("RW_PWR(EN): ");	if (pwr_sts.rw_en == PWR_ON) { Serial.print("ON, "); }
	else { Serial.print("OFF, "); }
	Serial.print("IMU_PWR: ");		if (pwr_sts.imu == PWR_ON) { Serial.print("ON, "); }
	else { Serial.print("OFF, "); }

	Serial.println("");
	return;
}