// 
// 
// 

#include "STRX.h"

STRX_STS_STRUCT strx_sts;

void STRX_engineering_convert(unsigned char *data);
unsigned char STRX_CheckSum(unsigned char *data, unsigned char size);

int STRX_rec(void)
{
	int i_ret = 0;
	int size = 0;
	unsigned char data[256];
	if ((size = STRX_Serial.available()) > 0)
	{
		for (int i = 0; i < size; i++)
		{
			data[i] = STRX_Serial.read();
			//Serial.print(data[i], HEX); Serial.print(" ");
		}
		STRX_engineering_convert(data);
	}
	return i_ret;
}

void STRX_engineering_convert(unsigned char *strx_rx_data)
{
	unsigned short	short_tmp;
	unsigned char	c_hex;
	unsigned short  s_hex;

	if (strx_rx_data[0] != 0x21)
		return;

	if (strx_rx_data[23] != 0x0D)
		return;

	short_tmp = 0;
	short_tmp |= (strx_rx_data[1] << 8);
	short_tmp |= (strx_rx_data[2] << 0);
	strx_sts.spc_counter = ascii2hex(short_tmp);

	short_tmp = 0;
	short_tmp |= (strx_rx_data[3] << 8);
	short_tmp |= (strx_rx_data[4] << 0);
	strx_sts.wdt_counter = ascii2hex(short_tmp);

	short_tmp = 0;
	short_tmp |= (strx_rx_data[5] << 8);
	short_tmp |= (strx_rx_data[6] << 0);
	c_hex = ascii2hex(short_tmp);
	strx_sts.rssi = (c_hex + 10);	//地上工学値変換でマイナス処理
	
	s_hex = 0x0000;
	short_tmp = 0;
	short_tmp |= (strx_rx_data[7] << 8);
	short_tmp |= (strx_rx_data[8] << 0);
	s_hex |= (ascii2hex(short_tmp) << 8);
	short_tmp = 0;
	short_tmp |= (strx_rx_data[9] << 8);
	short_tmp |= (strx_rx_data[10] << 0);
	s_hex |= (ascii2hex(short_tmp) << 0);
	strx_sts.rx_freq = float(s_hex) * 100;

	short_tmp = 0;
	short_tmp |= (strx_rx_data[11] << 8);
	short_tmp |= (strx_rx_data[12] << 0);
	c_hex = ascii2hex(short_tmp);
	strx_sts.tempe = ((char)c_hex) + 100;

	short_tmp = 0;
	short_tmp |= (strx_rx_data[13] << 8);
	short_tmp |= (strx_rx_data[14] << 0);
	strx_sts.volt = ascii2hex(short_tmp);

	short_tmp = 0;
	short_tmp |= (strx_rx_data[15] << 8);
	short_tmp |= (strx_rx_data[16] << 0);
	strx_sts.tx_power = ascii2hex(short_tmp);

	short_tmp = 0;
	short_tmp |= (strx_rx_data[17] << 8);
	short_tmp |= (strx_rx_data[18] << 0);
	c_hex = ascii2hex(short_tmp);
	strx_sts.others = c_hex;

	strx_sts.carrier_lock = (c_hex >> 7) & 0b00000001;
	strx_sts.sub_carrier_lock = (c_hex >> 6) & 0b00000001;
	strx_sts.pwr_level	= (c_hex >> 2) & 0b00001111;
	strx_sts.bitrate	= (c_hex >> 0) & 0b00000011;

}

void STRX_set_cmd(unsigned char pwr_level, unsigned char bit_rate)
{
	int i;
	unsigned char data[7];
	unsigned char d1 = 0x03;
	unsigned char d2 = 0x00;

	data[0] = STRX_STX;

	d1 += bit_rate << 2;
	d2 = pwr_level;

	data[1] = (unsigned char)hex2ascii(d2);
	data[2] = (unsigned char)hex2ascii(d1);
	
	unsigned char checksum = STRX_CheckSum(data, 3);
	
	data[3] = (unsigned char)hex2ascii(checksum >> 4);
	data[4] = (unsigned char)hex2ascii(checksum & 0x0F);

	data[5] = STRX_ETX1;
	data[6] = STRX_ETX2;

	for (i = 0; i < 7; i++)
	{
		STRX_Serial.write(data[i]);
	}
}

unsigned char STRX_CheckSum(unsigned char *data, unsigned char size)
{
	unsigned short checksum = 0;
	int i;
	for (i = 0; i < size; i++)
	{
		checksum += data[i];
	}
	checksum += 1;
	return checksum;
}

void STRX_DebugMessage(void)
{
	DEBUG_Serial.print("STRX_WDT: "); DEBUG_Serial.print(strx_sts.wdt_counter); DEBUG_Serial.print(", ");

	DEBUG_Serial.print("RX_FREQ : "); DEBUG_Serial.print(strx_sts.rx_freq, 2); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("RSSI : "); DEBUG_Serial.print(strx_sts.rssi); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("POWER : "); DEBUG_Serial.print(strx_sts.tx_power); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("TMP : "); DEBUG_Serial.print(strx_sts.tempe); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("Volt : "); DEBUG_Serial.print(strx_sts.volt); DEBUG_Serial.println("");

	DEBUG_Serial.print("Others : "); DEBUG_Serial.print(strx_sts.others); DEBUG_Serial.println(" ");
	
	DEBUG_Serial.print("CarrierLock : "); DEBUG_Serial.print(strx_sts.carrier_lock); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("SubCarrierLock : "); DEBUG_Serial.print(strx_sts.sub_carrier_lock); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("Bitrate : "); DEBUG_Serial.print(strx_sts.bitrate); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("PWR_LEVEL : "); DEBUG_Serial.print(strx_sts.pwr_level); DEBUG_Serial.println(" ");
}