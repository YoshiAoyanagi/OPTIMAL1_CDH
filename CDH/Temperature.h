// Temperature.h

#ifndef _TEMPERATURE_h
#define _TEMPERATURE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Config.h"
#include "Utility.h"
#include "port_config.h"

#define TEMP_TASK_NEW

#define TEMP_OFFSET					3.29		// Board#2 ADG706 On resistance 
#define TEMP_GAIN					11.0176		//amp gain

#define TEMP_GAIN_CONV_8BIT			1	//0degC‚Å128dn, 1degC/1dn
#define TEMP_OFFSET_CONV_8BIT		128	//0degC‚Å128dn, 1degC/1dn

#define TEMP_CAL_FACTOR				1
#define TEMP_CAL_BIAS				0

#define TEMP_CH_MAX_NUMBER	15

#include "AppInfo.h"

typedef struct
{
	AN_TLM an_tlm[16];
	unsigned char ch;
}TempSens;

extern TempSens temp_sens;
AppInfo temperature_update(void);
AppInfo temperature_ch_change(void);

void TempSens_init(void);
void TempSens_select(unsigned char ch);

#ifdef TEMP_TASK_NEW
float TempSens_channel_read(void);
#else
float TempSens_channel_read(unsigned char ch);
#endif

float TempSens_Calibration(float temp);
void TempSens_Task(void);
void TempSens_DebugMessage(void);
#endif

