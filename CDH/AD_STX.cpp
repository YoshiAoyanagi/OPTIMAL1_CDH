// 
// 
// 

#include "AD_STX.h"

float AD_STX_get_temp(AD_STX_STS_STRUCT* stx);
void AD_STX_hk_convert(AD_STX_STS_STRUCT* stx, unsigned char* data);


void AD_STX_init(AD_STX_STS_STRUCT* stx)
{
	stx->set_bitrate = STX_BITRATE_INITIAL;// 0; //0:64, 1:32k, 2:4k
	stx->set_outport = 0;//
	stx->set_power	 = 1; //0:low, 1:High

	AD_STX_get_temp(stx);
}

void AD_STX_rec(AD_STX_STS_STRUCT* stx)
{
	int size = 0;
	unsigned char data[256];
	if ((size = STRX_Serial.available()) > 0)
	{
		for (int i = 0; i < size; i++)
		{
			if (i > sizeof(data))
				break;
			data[i] = STRX_Serial.read();
			AD_STX_hk_convert(stx, data);
		}
	}

	AD_STX_get_temp(stx);
}

void AD_STX_Cmd_set_bitrate(AD_STX_STS_STRUCT* stx, unsigned char bps)
{
	stx->set_bitrate = bps;

	if (bps == 0)
		STRX_Serial.print("BR0\r\n");
	else if (bps == 1)
		STRX_Serial.print("BR1\r\n");
	else if (bps == 2)
		STRX_Serial.print("BR2\r\n");
	else if (bps == 3)
		STRX_Serial.print("BR3\r\n");
	else if (bps == 4)
		STRX_Serial.print("BR4\r\n");
	else if (bps == 5)
		STRX_Serial.print("BR5\r\n");
	else if (bps == 6)
		STRX_Serial.print("BR6\r\n");
	else if (bps == 7)
		STRX_Serial.print("BR7\r\n");
	else
		return;
}

void AD_STX_Cmd_set_power(AD_STX_STS_STRUCT* stx, unsigned char power)
{
	stx->set_power = power;

	if (power == 0)
		STRX_Serial.print("PWL\r\n");
	else if (power == 1)
		STRX_Serial.print("PWH\r\n");
	else
		return;
}

void AD_STX_Cmd_set_outport(AD_STX_STS_STRUCT* stx, unsigned char tmp)
{
	stx->set_outport = tmp;

	if (tmp == 0)
		STRX_Serial.print("AT0\r\n");
	else
		STRX_Serial.print("AT1\r\n");
}

void AD_STX_Cmd_set_mod(AD_STX_STS_STRUCT* stx, unsigned char tmp)
{
	if (tmp == 0)
		STRX_Serial.print("MOF\r\n");
	else
		STRX_Serial.print("MON\r\n");
}

void AD_STX_Cmd_set_pn_test(AD_STX_STS_STRUCT* stx, unsigned char tmp)
{
	if (tmp == 0)
		STRX_Serial.print("PNN\r\n");
	else
		STRX_Serial.print("PNF\r\n");
}


float AD_STX_get_temp(AD_STX_STS_STRUCT* stx)
{
	float tmp_mv;
	tmp_mv = analogRead(PORT_AN_AD_STX_TEMP);
	stx->tmpe = AD_STX_AD590_SCALE * tmp_mv + AD_STX_AD590_OFFSET;
}


void AD_STX_hk_convert(AD_STX_STS_STRUCT* stx, unsigned char* data)
{
	if (data[0] == 'A')
	{
		if (data[1] == 'F')
			stx->use_port = 1;
		else
			stx->use_port = 0;
	}

	if (data[3] == 'R')
	{
		if (data[4] == '0')
			stx->bitrate = 0;
		else if (data[4] == '1')
			stx->bitrate = 1;
		else if (data[4] == '2')
			stx->bitrate = 2;
	}
}