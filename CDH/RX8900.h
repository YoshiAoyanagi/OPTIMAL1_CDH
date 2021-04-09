// RX8900.h

#ifndef _RX8900_h
#define _RX8900_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DTWI.h"
#include "InternalDevice.h"
#include "dWire0.h"

class TimeSpan;

#define RX8900_ADDRESS  0x32

#define RX8900_TEMP		0x17
#define RX8900_BACKUP	0x18
#define RX8900_TIMER_COUNTER0		0x0B
#define RX8900_TIMER_COUNTER1		0x0C
#define RX8900_EXTENTION	0x0D
#define RX8900_FLAG		0x0E
#define RX8900_CONTROL  0x0F

#define SECONDS_PER_DAY 86400L

#define SECONDS_FROM_1970_TO_2000 946684800



// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
class DateTime {
public:
	DateTime(uint32_t t = 0);
	DateTime(uint16_t year, uint8_t month, uint8_t day,
		uint8_t hour = 0, uint8_t min = 0, uint8_t sec = 0);
	DateTime(const DateTime& copy);
	DateTime(const char* date, const char* time);
	DateTime(const __FlashStringHelper* date, const __FlashStringHelper* time);
	uint16_t year() const { return 2000 + yOff; }
	uint8_t month() const { return m; }
	uint8_t day() const { return d; }
	uint8_t hour() const { return hh; }
	uint8_t minute() const { return mm; }
	uint8_t second() const { return ss; }
	uint8_t dayOfTheWeek() const;

	// 32-bit times as seconds since 1/1/2000
	long secondstime() const;
	// 32-bit times as seconds since 1/1/1970
	uint32_t unixtime(void) const;

	DateTime operator+(const TimeSpan& span);
	DateTime operator-(const TimeSpan& span);
	TimeSpan operator-(const DateTime& right);

protected:
	uint8_t yOff, m, d, hh, mm, ss;
};

// Timespan which can represent changes in time with seconds accuracy.
class TimeSpan {
public:
	TimeSpan(int32_t seconds = 0);
	TimeSpan(int16_t days, int8_t hours, int8_t minutes, int8_t seconds);
	TimeSpan(const TimeSpan& copy);
	int16_t days() const { return _seconds / 86400L; }
	int8_t  hours() const { return _seconds / 3600 % 24; }
	int8_t  minutes() const { return _seconds / 60 % 60; }
	int8_t  seconds() const { return _seconds % 60; }
	int32_t totalseconds() const { return _seconds; }

	TimeSpan operator+(const TimeSpan& right);
	TimeSpan operator-(const TimeSpan& right);

protected:
	int32_t _seconds;
};

// RTC based on the RX8900 chip connected via I2C and the Wire library

class RTC_RX8900 {
public:
	boolean begin(void);
	void adjust(const DateTime& dt);
	boolean initialized(void);
	static DateTime now();
	uint16_t TimerINT_Second(uint16_t count);
};

// RTC using the internal millis() clock, has to be initialized before use
// NOTE: this clock won't be correct once the millis() timer rolls over (>49d?)
class RTC_Millis {
public:
	static void begin(const DateTime& dt) { adjust(dt); }
	static void adjust(const DateTime& dt);
	static DateTime now();

protected:
	static long offset;
};
#endif

