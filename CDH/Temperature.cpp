// 
// 
// 

// 
// 
// 

#include "Temperature.h"


TempSens temp_sens;

static void TempSens_Ch_change(void);
unsigned char TempSens_convert8bit(float fl_val);


#ifdef TEMP_TASK_NEW

AppInfo temperature_ch_change(void)
{
	return create_app_info("TEMPCH", 10, NULL, TempSens_Ch_change);
}

static void TempSens_Ch_change(void)
{
	if (temp_sens.ch > TEMP_CH_MAX_NUMBER)
		temp_sens.ch = TEMP_CH_MAX_NUMBER;

	TempSens_select(temp_sens.ch);
}

AppInfo temperature_update(void)
{
	return create_app_info("TEMPSENS", 10, TempSens_init, TempSens_Task);
}

void TempSens_init(void)
{
	int i;
	temp_sens.ch = 0;
	for (i = 0; i < 16; i++)
	{
		temp_sens.an_tlm[i].byte_value = 0;
		temp_sens.an_tlm[i].float_value = 0.0;
	}

	pinMode(PORT_AN_EN, OUTPUT);
	pinMode(PORT_TEMP_AN0, OUTPUT);
	pinMode(PORT_TEMP_AN1, OUTPUT);
	pinMode(PORT_TEMP_AN2, OUTPUT);
	pinMode(PORT_TEMP_AN3, OUTPUT);
	digitalWrite(PORT_AN_EN, HIGH);  // Analog Curcuit Power ON
	analogReference(EXTERNAL);       //Enable Vref+ Enable (2.048V))


	TempSens_select(15);
	delay(5);
	float tmp = TempSens_channel_read();
	temp_sens.an_tlm[15].float_value = TempSens_Calibration(tmp);
	temp_sens.an_tlm[15].byte_value = TempSens_convert8bit(temp_sens.an_tlm[15].float_value);

	TempSens_select(0);
	for (i = 0; i < 14; i++)
	{
		delay(1);
		TempSens_Task();
	}

	return;
}

float TempSens_channel_read(void)
{
	float mv;
	float degc;

	mv = analogRead(PORT_TEMP_RTD) * 2;
	degc = (mv / TEMP_GAIN - (100 + TEMP_OFFSET)) / 0.3851; //RTD resistance - (100deg with Offset) / RTD coefficient

	return degc;
}

float TempSens_Calibration(float temp)
{
	float cal_temp;
	cal_temp = TEMP_CAL_FACTOR * temp + TEMP_CAL_BIAS;
	return cal_temp;
}

void TempSens_select(unsigned char ch)
{
	uint8_t b = 0;
	ch = ch & 0x0F;
	digitalWrite(PORT_TEMP_AN0, ch & 0x01);
	ch = ch >> 1;
	digitalWrite(PORT_TEMP_AN1, ch & 0x01);
	ch = ch >> 1;
	digitalWrite(PORT_TEMP_AN2, ch & 0x01);
	ch = ch >> 1;
	digitalWrite(PORT_TEMP_AN3, ch & 0x01);
}

unsigned char TempSens_convert8bit(float fl_val)
{
	unsigned char byte_val;

	byte_val = (unsigned char)(fl_val * TEMP_GAIN_CONV_8BIT + TEMP_OFFSET_CONV_8BIT);

	return byte_val;
}

void TempSens_Task(void)
{
	float tmp;
	float ref_temp;

	static unsigned char ch;

	tmp = TempSens_channel_read();
	if (ch == 15)
	{
		temp_sens.an_tlm[ch].float_value = TempSens_Calibration(tmp);
		temp_sens.an_tlm[ch].byte_value = TempSens_convert8bit(temp_sens.an_tlm[ch].float_value);
	}
	else
	{
		temp_sens.an_tlm[ch].float_value = TempSens_Calibration(tmp) - temp_sens.an_tlm[15].float_value;
		temp_sens.an_tlm[ch].byte_value = TempSens_convert8bit(temp_sens.an_tlm[ch].float_value);
	}


	if (ch == 0)
		ch = 15;
	else
		ch--;

	TempSens_select(ch);

	return;
}

void TempSens_DebugMessage(void)
{
	Serial.println("TEMP SENSOR: ");
	//Serial.print("[01, ");
	//Serial.print("[CAM, ");
	Serial.print(temp_sens.an_tlm[0].float_value); Serial.print("] ");

	Serial.print("[02, ");
	//Serial.print("[MOBC, ");
	Serial.print(temp_sens.an_tlm[1].float_value); Serial.print("] ");

	Serial.print("[03, ");
	//Serial.print("[BAT, ");
	Serial.print(temp_sens.an_tlm[2].float_value); Serial.print("] ");

	Serial.print("[04, ");
	//Serial.print("[PZ, ");
	Serial.print(temp_sens.an_tlm[3].float_value); Serial.print("] ");
	Serial.println("");

	Serial.print("[05, ");
	//Serial.print("[MZ, ");
	Serial.print(temp_sens.an_tlm[4].float_value); Serial.print("] ");

	Serial.print("[06, ");
	//Serial.print("[PY, ");
	Serial.print(temp_sens.an_tlm[5].float_value); Serial.print("] ");

	Serial.print("[07, ");
	Serial.print(temp_sens.an_tlm[6].float_value); Serial.print("] ");
	//Serial.print("[MX, ");

	Serial.print("[08, ");
	//Serial.print("[STRx1, ");
	Serial.print(temp_sens.an_tlm[7].float_value); Serial.print("] ");
	Serial.println("");

	Serial.print("[09, ");
	//Serial.print("[STRx2, ");
	Serial.print(temp_sens.an_tlm[8].float_value); Serial.print("] ");

	Serial.print("[15, ");
	//Serial.print("[OBC, ");
	Serial.print(temp_sens.an_tlm[14].float_value); Serial.print("] ");

	Serial.print("[16, ");
	//Serial.print("[REF, ");
	Serial.print(temp_sens.an_tlm[15].float_value); Serial.print("] ");
	Serial.println("");

	return;
}


#else


AppInfo temperature_ch_change(void)
{
	return create_app_info("TEMPCH", 10, NULL, TempSens_Ch_change);
}

static void TempSens_Ch_change(void)
{
	if (temp_sens.ch > TEMP_CH_MAX_NUMBER)
		temp_sens.ch = TEMP_CH_MAX_NUMBER;

	TempSens_select(temp_sens.ch);
}


AppInfo temperature_update(void)
{
	return create_app_info("TEMPSENS", 10, TempSens_init, TempSens_Task);
}

void TempSens_init(void)
{
	int i;
	temp_sens.ch = 0;
	for (i = 0; i < 16; i++)
	{
		temp_sens.an_tlm[i].byte_value = 0;
		temp_sens.an_tlm[i].float_value = 0.0;
	}

	pinMode(PORT_AN_EN, OUTPUT);
	pinMode(PORT_TEMP_AN0, OUTPUT);
	pinMode(PORT_TEMP_AN1, OUTPUT);
	pinMode(PORT_TEMP_AN2, OUTPUT);
	pinMode(PORT_TEMP_AN3, OUTPUT);
	digitalWrite(PORT_AN_EN, HIGH);  // Analog Curcuit Power ON
	analogReference(EXTERNAL);       //Enable Vref+ Enable (2.048V))

	return;
}

float TempSens_channel_read(unsigned char ch)
{
	float mv;
	float degc;

	TempSens_select(ch);
	delay(1);
	//delayMicroseconds(500);
	mv = analogRead(PORT_TEMP_RTD) * 2;
	degc = (mv / TEMP_GAIN - (100 + TEMP_OFFSET)) / 0.3851; //RTD resistance - (100deg with Offset) / RTD coefficient

	return degc;
}

float TempSens_Calibration(float temp)
{
	float cal_temp;
	cal_temp = TEMP_CAL_FACTOR * temp + TEMP_CAL_BIAS;
	return cal_temp;
}

void TempSens_select(unsigned char ch)
{
	uint8_t b = 0;
	ch = ch & 0x0F;
	digitalWrite(PORT_TEMP_AN0, ch & 0x01);
	ch = ch >> 1;
	digitalWrite(PORT_TEMP_AN1, ch & 0x01);
	ch = ch >> 1;
	digitalWrite(PORT_TEMP_AN2, ch & 0x01);
	ch = ch >> 1;
	digitalWrite(PORT_TEMP_AN3, ch & 0x01);
}

unsigned char TempSens_convert8bit(float fl_val)
{
	unsigned char byte_val;

	byte_val = (unsigned char)(fl_val * TEMP_GAIN_CONV_8BIT + TEMP_OFFSET_CONV_8BIT);

	return byte_val;
}

void TempSens_Task(void)
{
	static unsigned int elapsed_time;
	float tmp;
	float ref_temp;

	tmp = TempSens_channel_read(15);
	ref_temp = TempSens_Calibration(tmp);

	int i;
	for (i = 0; i < 15; i++)	//0.5*15=7.5msec delay”­¶
	{
		tmp = TempSens_channel_read(i);
		temp_sens.an_tlm[i].float_value = TempSens_Calibration(tmp) - ref_temp;
		temp_sens.an_tlm[i].byte_value = TempSens_convert8bit(temp_sens.an_tlm[i].float_value);
	}
	return;
}

void TempSens_DebugMessage(void)
{
	Serial.println("TEMP SENSOR: ");
	//Serial.print("[01, "); 
	Serial.print("[CAM, "); Serial.print(temp_sens.an_tlm[0].float_value); Serial.print("] ");

	//Serial.print("[02, "); 
	Serial.print("[MOBC, "); Serial.print(temp_sens.an_tlm[1].float_value); Serial.print("] ");

	//Serial.print("[03, "); 
	Serial.print("[BAT, "); Serial.print(temp_sens.an_tlm[2].float_value); Serial.print("] ");

	//Serial.print("[04, "); 
	Serial.print("[PZ, "); Serial.print(temp_sens.an_tlm[3].float_value); Serial.print("] ");
	Serial.println("");

	//Serial.print("[05, "); 
	Serial.print("[MZ, "); Serial.print(temp_sens.an_tlm[4].float_value); Serial.print("] ");

	//Serial.print("[06, "); 
	Serial.print("[PY, "); Serial.print(temp_sens.an_tlm[5].float_value); Serial.print("] ");

	//Serial.print("[07, "); Serial.print(temp_sens.an_tlm[6].float_value); Serial.print("] ");
	Serial.print("[MX, ");

	//Serial.print("[08, ");
	Serial.print("[STRx1, "); Serial.print(temp_sens.an_tlm[7].float_value); Serial.print("] ");
	Serial.println("");

	//Serial.print("[09, "); 
	Serial.print("[STRx2, "); Serial.print(temp_sens.an_tlm[8].float_value); Serial.print("] ");

	//Serial.print("[15, "); 
	Serial.print("[OBC, "); Serial.print(temp_sens.an_tlm[14].float_value); Serial.print("] ");

	//Serial.print("[16, "); 
	Serial.print("[REF, "); Serial.print(temp_sens.an_tlm[15].float_value); Serial.print("] ");
	Serial.println("");

	return;
}


#endif
