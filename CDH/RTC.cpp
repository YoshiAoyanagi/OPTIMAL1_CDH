// 
// 
// 

#include "RTC.h"

RTC_RX8900 rtc;
RTC_TIME rtc_time;

int RTC_init(void)
{
	int i_ret;
	i_ret = rtc.initialized();

	RTC_get_UNIX_TIME();
	RTC_Debug_message();

	return (i_ret);
}

int RTC_UTC_SET(UTC_STRUCT utc)
{
	rtc.adjust(DateTime((uint16_t)utc.year, (uint8_t)utc.month, (uint8_t)utc.day, (uint8_t)utc.hour,
		(uint8_t)utc.min, (uint8_t)utc.sec));

	return 0;
}

int RTC_UTIME_SET(uint32_t unix_time)
{
	UTC_STRUCT utc = Convert_UnixTime2UTC(unix_time);
	RTC_UTC_SET(utc);
	return 0;
}

int RTC_get_UTC(UTC_STRUCT *utc)
{
	DateTime now = rtc.now();
	utc->year = now.year();
	utc->month = now.month();
	utc->day = now.day();
	utc->hour = now.hour();
	utc->min = now.minute();
	utc->sec = (float)now.second();
	return 0;
}

int RTC_get_UNIX_TIME(void)
{
	RTC_get_UTC(&rtc_time.utc);
	rtc_time.unix = Convert_Utc2UnixTime(&rtc_time.utc);
	return 0;
}

void RTC_Debug_message(void)
{
	Serial.print("RTC: "); Serial.print(rtc_time.unix);
	Serial.print(", ");
	Serial.print(rtc_time.utc.year); Serial.print("/");
	Serial.print(rtc_time.utc.month); Serial.print("/");
	Serial.print(rtc_time.utc.day); Serial.print(" ");
	Serial.print(rtc_time.utc.hour); Serial.print(":");
	Serial.print(rtc_time.utc.min); Serial.print(":");
	Serial.print(rtc_time.utc.sec); Serial.println("");

}