// RTC.h

#ifndef _RTC_h
#define _RTC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include"RX8900.h"
#include "Utility.h"

typedef struct
{
	UTC_STRUCT utc;
	uint32_t unix;
}RTC_TIME;

extern RTC_TIME rtc_time;

int RTC_init(void);

int RTC_UTC_SET(UTC_STRUCT utc);
int RTC_UTIME_SET(uint32_t unix_time);
int RTC_get_UTC(UTC_STRUCT *utc);
int RTC_get_UNIX_TIME(void);
void RTC_Debug_message(void);

#endif

