// 
// 
// 

#include "GNSS_receiver.h"

position_t gnss_pot;
UTC_STRUCT gnss_utc;
float gnss_hdop;
unsigned char gnss_quality;
unsigned char gnss_visible_sat;

void GNSS_init_(void);
void GNSS_receiver_(void);

float NMEA2DD(float val);

AppInfo GNSS_receiver(void)
{
	return create_app_info("GNSS", 10, GNSS_init_, GNSS_receiver_);
}

void GNSS_init_(void)
{
	gnss_pot.eci.x = 0;
	gnss_pot.eci.y = 0;
	gnss_pot.eci.z = 0;

	gnss_pot.ecr.x = 0;
	gnss_pot.ecr.y = 0;
	gnss_pot.ecr.z = 0;

	gnss_pot.geo.lat = 0;
	gnss_pot.geo.lon = 0;
	gnss_pot.geo.alt = 0;
	
	gnss_utc.year  = 2018;
	gnss_utc.month = 1;
	gnss_utc.day   = 1;
	gnss_utc.hour  = 0;
	gnss_utc.min   = 0;
	gnss_utc.sec   = 0;
	
	gnss_hdop = 0;
}

void GNSS_receiver_(void)
{
	//GNSS_REC()
	if (GNSSR_Serial.available() > 0)
	{
		String line = GNSSR_Serial.readStringUntil('\n');

		if (line != "") {
			int i;
			int index = 0;
			int len = line.length();
			String str = "";

			// StringListÇÃê∂ê¨(ä»à’)
			String list[64];
			for (i = 0; i < 64; i++) {
				list[i] = "";
			}

			// Åu,ÅvÇãÊêÿÇËï∂éöÇ∆ÇµÇƒï∂éöóÒÇîzóÒÇ…Ç∑ÇÈ
			for (i = 0; i < len; i++) {
				if (line[i] == ',') {
					list[index++] = str;
					str = "";
					continue;
				}
				str += line[i];
			}

			if (list[0] == "$GPGGA")
			{
				gnss_pot.geo.lat = NMEA2DD(list[2].toFloat());
				if (list[3] == "N")
					gnss_pot.geo.lat = -1 * gnss_pot.geo.lat;

				gnss_pot.geo.lon = NMEA2DD(list[4].toFloat());
				if (list[3] != "E")
					gnss_pot.geo.lon = -1 * gnss_pot.geo.lon;

				gnss_pot.geo.alt = NMEA2DD(list[9].toFloat());
			}
			else if (list[0] == "$GPRMC")
			{
				gnss_utc.hour = list[1].substring(0, 2).toInt();
				gnss_utc.min  = list[1].substring(2, 4).toInt();
				gnss_utc.sec  = list[1].substring(4, 6).toInt();

				int tmp_day = list[9].substring(0, 2).toInt();
				int tmp_month = list[9].substring(2, 4).toInt();
				int tmp_year = list[9].substring(4, 6).toInt();
				if (tmp_day > 0 && tmp_month > 0 && tmp_year > 0)
				{
					gnss_utc.day = tmp_day;
					gnss_utc.month = tmp_month;
					gnss_utc.year = tmp_year;
				}
			}
		}
	}
}

float NMEA2DD(float val) 
{
	int d = val / 100;
	int m = (((val / 100.0) - d) * 100.0) / 60;
	float s = (((((val / 100.0) - d) * 100.0) - m) * 60) / (60 * 60);
	return d + m + s;
}

void GNSS_DebugMessage(void)
{
	DEBUG_Serial.print("GNSS_TIME: ");
	DEBUG_Serial.print(gnss_utc.year); DEBUG_Serial.print("/");
	DEBUG_Serial.print(gnss_utc.month); DEBUG_Serial.print("/");
	DEBUG_Serial.print(gnss_utc.day); DEBUG_Serial.print(" ");

	DEBUG_Serial.print(gnss_utc.hour); DEBUG_Serial.print(":");
	DEBUG_Serial.print(gnss_utc.min); DEBUG_Serial.print(":");
	DEBUG_Serial.print(gnss_utc.sec); DEBUG_Serial.println("");

	DEBUG_Serial.print("HDOP: "); DEBUG_Serial.print(gnss_hdop, 2); DEBUG_Serial.print(",");
	DEBUG_Serial.print("Lat: "); DEBUG_Serial.print(gnss_pot.geo.lat, 4); DEBUG_Serial.print(",");
	DEBUG_Serial.print("Lon: "); DEBUG_Serial.print(gnss_pot.geo.lon, 4); DEBUG_Serial.print(",");
	DEBUG_Serial.print("Alt: "); DEBUG_Serial.println(gnss_pot.geo.alt, 4); DEBUG_Serial.println("");
}