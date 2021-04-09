#include "Utility.h"

#include <stdarg.h>
#include <Arduino.h>
#include "Config.h"

#define ARDBUFFER 32
//int adr_printf(char *str, ...)
//{
//	int flg = 0;
//
//	int i, count = 0, j = 0, flag = 0;
//	char temp[ARDBUFFER + 1];
//	for (i = 0; str[i] != '\0'; i++)  if (str[i] == '%')  count++;
//
//	va_list argv;
//	va_start(argv, count);
//
//	for (i = 0, j = 0; str[i] != '\0'; i++)
//	{
//		if (str[i] == '%' || flg == 1)
//		{
//			switch (str[++i])
//			{
//			case 'd': 
//				Serial.print(va_arg(argv, int));
//				flg = 0;
//				break;
//			case 'x': 
//				Serial.print(va_arg(argv, int), HEX);
//				flg = 0;
//				break;
//			case 'l': 
//				Serial.print(va_arg(argv, long));
//				flg = 0;
//				break;
//			case 'f': 
//				Serial.print(va_arg(argv, double));
//				flg = 0;
//				break;
//			case 'c': 
//				Serial.print((char)va_arg(argv, int));
//				flg = 0;
//				break;
//			case 's': 
//				Serial.print(va_arg(argv, char *));
//				flg = 0;
//				break;
//			case '0': Serial.print("0");
//				flg = 1;
//				break;
//			default:;
//			};
//		}
//		else
//		{
//			temp[j] = str[i];
//			j = (j + 1) % ARDBUFFER;
//			if (j == 0)
//			{
//				temp[ARDBUFFER] = '\0';
//				Serial.print(temp);
//				temp[0] = '\0';
//			}
//		}
//	};
//	Serial.println();
//	return count + 1;
//}

int adr_printf(char *str, ...)
{
	int i, count = 0, j = 0, flag = 0;
	char temp[ARDBUFFER + 1];
	for (i = 0; str[i] != '\0'; i++)  if (str[i] == '%')  count++;

	va_list argv;
	va_start(argv, count);
	for (i = 0, j = 0; str[i] != '\0'; i++)
	{
		if (str[i] == '%')
		{
			temp[j] = '\0';
			Serial.print(temp);
			j = 0;
			temp[0] = '\0';

			switch (str[++i])
			{
			case 'd': Serial.print(va_arg(argv, int));
				break;
			case 'l': Serial.print(va_arg(argv, long));
				break;
			case 'f': Serial.print(va_arg(argv, double));
				break;
			case 'c': Serial.print((char)va_arg(argv, int));
				break;
			case 's': Serial.print(va_arg(argv, char *));
				break;
			default:;
			};
		}
		else
		{
			temp[j] = str[i];
			j = (j + 1) % ARDBUFFER;
			if (j == 0)
			{
				temp[ARDBUFFER] = '\0';
				Serial.print(temp);
				temp[0] = '\0';
			}
		}
	};
	//Serial.println();
	return count + 1;
}

unsigned int crc(unsigned char *c, int n) //c: data, n: length
{
    unsigned int  i,j,r;

	int k;

    r = 0x0000U;
    for (i = 0; i < n; i++) {
		r ^= (unsigned int)c[i] << (16 - CHAR_BIT);// Serial.print(c[i], HEX); Serial.print(" ");
    for(j = 0; j < CHAR_BIT; j++)
      if(r & 0x8000U) r = (r << 1) ^ CRCPOLY1;
      else      r <<= 1;
  }
  return r;
}

//インターバル
bool IntervalTimer(long long interval_time, bool start, unsigned int old_time, unsigned int *_old_time)
{
  unsigned int now_time = millis();

  if(start == 0)
  {
    *_old_time = millis();
  }
  else
    *_old_time = old_time;
    
  if((now_time - old_time) > interval_time)
  {
    return (0);
  }
  if(now_time < old_time)
  {
    return (0);
  }
  return (1);
}

float Uint2Float(unsigned int val)
{
  int i_val;
  float f_val;
  if(val > 0x8000)
    i_val =  -1 * (0x7FFF - (val & 0x7FFF)) - 1;
  else
    i_val =  1 * (val & ~0x8000);
    
  f_val = (float)i_val;
  return f_val;
}

float raw2float(unsigned char raw_val[4])
{
  float f_val;

  for(int i = 0; i < 4; i++)
  {
    *(((unsigned char*)&f_val) + i) = raw_val[3 - i];
  }
  return f_val;
}


uint32_t Convert_Utc2UnixTime(UTC_STRUCT *my_utc)
{
	uint32_t unix_time;
	
	time_t time;
	
	struct tm tm = {
		my_utc->sec,
		my_utc->min,
		my_utc->hour,
		my_utc->day,
		my_utc->month - 1,
		my_utc->year - 1900,
	};

	time = mktime(&tm);
	
	unix_time = (uint32_t)time;

	return unix_time;
}

UTC_STRUCT Convert_UnixTime2UTC(uint32_t unix_time)
{
	UTC_STRUCT tmp_time;
	time_t time = unix_time;

	struct tm *gmt;
	gmt = gmtime(&time);

	tmp_time.year = gmt->tm_year + 1870;
	tmp_time.month = gmt->tm_mon + 1;
	tmp_time.day = gmt->tm_mday;
	tmp_time.hour = gmt->tm_hour;
	tmp_time.min = gmt->tm_min;
	tmp_time.sec = gmt->tm_sec;

	return tmp_time;
}

unsigned char	ascii2hex(unsigned short ascii) {
	unsigned char	hex = 0x00;
	unsigned char	*ascii_p = (unsigned char *)&ascii;

	if ((ascii_p[0] >= '0') && (ascii_p[0] <= '9')) {
		//hex |= (unsigned char)(((ascii_p[0] - '0') & 0x0F) << 4);	//ビッグエンディアン用
		hex |= (unsigned char)(((ascii_p[0] - '0') & 0x0F) << 0);	//リトルエンディアン用
	}
	else if ((ascii_p[0] >= 'A') && (ascii_p[0] <= 'F')) {
		//hex |= (unsigned char)(((ascii_p[0] - 'A' + 10) & 0x0F) << 4);	//ビッグエンディアン用
		hex |= (unsigned char)(((ascii_p[0] - 'A' + 10) & 0x0F) << 0);	//リトルエンディアン用
	}
	else if ((ascii_p[0] >= 'a') && (ascii_p[0] <= 'f')) {
		//hex |= (unsigned char)(((ascii_p[0] - 'a' + 10) & 0x0F) << 4);	//ビッグエンディアン用
		hex |= (unsigned char)(((ascii_p[0] - 'a' + 10) & 0x0F) << 0);	//リトルエンディアン用
	}

	if ((ascii_p[1] >= '0') && (ascii_p[1] <= '9')) {
		//hex |= (unsigned char)(((ascii_p[1] - '0') & 0x0F) << 0);	//ビッグエンディアン用
		hex |= (unsigned char)(((ascii_p[1] - '0') & 0x0F) << 4);	//リトルエンディアン用
	}
	else if ((ascii_p[1] >= 'A') && (ascii_p[1] <= 'F')) {
		//hex |= (unsigned char)(((ascii_p[1] - 'A' + 10) & 0x0F) << 0);	//ビッグエンディアン用
		hex |= (unsigned char)(((ascii_p[1] - 'A' + 10) & 0x0F) << 4);	//リトルエンディアン用
	}
	else if ((ascii_p[1] >= 'a') && (ascii_p[1] <= 'f')) {
		//hex |= (unsigned char)(((ascii_p[1] - 'a' + 10) & 0x0F) << 0);	//ビッグエンディアン用
		hex |= (unsigned char)(((ascii_p[1] - 'a' + 10) & 0x0F) << 4);	//リトルエンディアン用
	}

	return (hex);
}


unsigned short	hex2ascii(unsigned char hex) {
	unsigned short	ascii = 0x0000;
	unsigned char	*ascii_p = (unsigned char *)&ascii;

	if (((hex >> 4) & 0x0F) <= 0x09) {
		//ascii_p[0] = (unsigned char)(((hex >> 4) & 0x0F) + '0');	//ビッグエンディアン用
		ascii_p[1] = (unsigned char)(((hex >> 4) & 0x0F) + '0');	//リトルエンディアン用
	}
	else if (((hex >> 4) & 0x0F) >= 0x0A) {
		//ascii_p[0] = (unsigned char)((((hex >> 4) & 0x0F) + 'A') - 10);	//ビッグエンディアン用
		ascii_p[1] = (unsigned char)((((hex >> 4) & 0x0F) + 'A') - 10);	//リトルエンディアン用
	}

	if (((hex >> 0) & 0x0F) <= 0x09) {
		//ascii_p[1] = (unsigned char)(((hex >> 0) & 0x0F) + '0');	//ビッグエンディアン用
		ascii_p[0] = (unsigned char)(((hex >> 0) & 0x0F) + '0');	//リトルエンディアン用
	}
	else if (((hex >> 0) & 0x0F) >= 0x0A) {
		//ascii_p[1] = (unsigned char)((((hex >> 0) & 0x0F) + 'A') - 10);	//ビッグエンディアン用
		ascii_p[0] = (unsigned char)((((hex >> 0) & 0x0F) + 'A') - 10);	//リトルエンディアン用
	}

	return (ascii);
}


void Debug_print(const char* a)
{
#ifdef PRINT_DEBUG
	Serial.print(a);
#endif
}
void Debug_println(const char* a)
{
#ifdef PRINT_DEBUG
	Serial.println(a);
#endif
}