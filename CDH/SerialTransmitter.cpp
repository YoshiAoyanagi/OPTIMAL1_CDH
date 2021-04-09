// 
// 
// 

#include "SerialTransmitter.h"
#include "port_config.h"
#include "Config.h"

const unsigned char pn_code[28] =
{ 0xFF,0x48,0x0E,0xC0,0x9A,0x0D,0x70,0xBC,0x8E,0x2C,0x93,0xAD,0xA7,0xB7,  //1
 0x46,0xCE,0x5A,0x97,0x7D,0xCC,0x32,0xA2,0xBF,0x3E,0x0A,0x10,0xF1,0x88,  //2
};

void SerialTransmit_init(void)
{
	pinMode(PORT_AD_STX_RTS, INPUT);
	pinMode(PORT_AD_STX_EMPTY, INPUT);
}


int	SerialTransmit(unsigned char* data, unsigned int len)
{
	int i;

	uint8_t dummy = 0x00;
	uint8_t empty = 0x00;

	unsigned char pn_count = 0;

	if (PORTEbits.RE8 == LOW)	//アクティブロー(20200831)//G2はRE9
	{
		for (i = 0; i < AD_STRX_FIFO_SIZE; i++)
		{
			if (i > len)
			{
				if (pn_count > 27)
					pn_count = 0;

				dummy = dummy ^ pn_code[pn_count];
				pn_count++;
				DATA_Serial.write(dummy);	//ゼロフィル(PN)する
			}
			else
			{
				DATA_Serial.write(data[i]);
			}
		}
		return 0;
	}
	return(-1);
}

int	SerialDummyTransmit(void)
{
	int i;
	int t = 0;

	uint8_t dummy = 0x00;
	uint8_t empty = 0x00;

	unsigned char pn_count = 0;

	if (PORTEbits.RE8 == LOW)	//アクティブロー(20200831)//G2はRE9
	{
		
		DATA_Serial.write(0x1A);
		DATA_Serial.write(0xCF);
		DATA_Serial.write(0xFC);
		DATA_Serial.write(0x1D);
		
		for (i = 0; i < (AD_STRX_FIFO_SIZE - 4); i++)
		{
			if (pn_count > 27)
				pn_count = 0;

			dummy = dummy ^ pn_code[pn_count];
			pn_count++;
			DATA_Serial.write(dummy);	//ゼロフィル(PN)する
		}
	}
	return 0;
}
