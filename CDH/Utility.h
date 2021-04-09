#ifndef  UTILITY_H
#define  UTILITY_H

#include "arduino.h"
#include"stdlib.h"
#include "utility.h"
#include "time.h"

typedef enum
{
  Success     =  1,
  OtherProcess = 0,
  ValueError  = -1,
  GetError    = -2,
  Error       = -3,
  ValueNull   = -4
}ack;

typedef enum
{
	DISABLE	= 0,
	ENABLE	= 1
}ENABLE_STS;

typedef struct
{
	float			millivoltage;
	float			float_value;
	unsigned char	byte_value;
	float			conversion_coeff;
	float			conversion_offset;
	float			conversion_coeff_to_byte;
	float			conversion_offset_to_byte;
}AN_TLM;

typedef struct
{
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
}UTC_STRUCT;

typedef struct xyz_s
{
	float x;
	float y;
	float z;
} xyz_t;

typedef struct geo_t
{
	float lat;
	float lon;
	float alt;
} geo_t;

typedef struct position_t
{
	geo_t geo;
	xyz_t ecr;
	xyz_t eci;
	xyz_t v_eci;
	xyz_t v_ecr;

}position_t;

#define		PWR_RESET	0x02
#define		PWR_ON		0x01
#define		PWR_OFF		0x00
#define		PWR_ERROR	0xFE

#define  MAX_PACKET_LENGTH  256
#define CRCPOLY1  0x1021U
#define CHAR_BIT  8

//math
#define	DEG2RAD	(3.14159265359 / 180)
#define	RAD2DEG	(180 / 3.14159265359)

int adr_printf(char *str, ...);
unsigned int crc(unsigned char *c, int n);
bool IntervalTimer(long long interval_time, bool start, unsigned int old_time, unsigned int *_old_time);
float raw2float(unsigned char raw_val[4]);
float Uint2Float(unsigned int val);
uint32_t Convert_Utc2UnixTime(UTC_STRUCT *my_utc);
UTC_STRUCT Convert_UnixTime2UTC(uint32_t unix_time);
unsigned char ascii2hex(unsigned short ascii);
unsigned short hex2ascii(unsigned char hex);


void Debug_print(const char* a);
void Debug_println(const char* a);
#endif
