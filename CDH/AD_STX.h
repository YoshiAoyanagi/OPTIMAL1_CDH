// AD_STX.h

#ifndef _AD_STX_h
#define _AD_STX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Config.h"
#include "port_config.h"

#define AD_STX_AD590_SCALE	1.0
#define AD_STX_AD590_OFFSET	0.0


typedef struct
{
	unsigned char use_port;
	unsigned char bitrate;
	unsigned char set_bitrate;
	unsigned char set_outport;
	unsigned char set_power;
	float		  tmpe;
}AD_STX_STS_STRUCT;

extern AD_STX_STS_STRUCT ad_stx_sts;

void AD_STX_init(AD_STX_STS_STRUCT* stx);
void AD_STX_rec(AD_STX_STS_STRUCT* stx);

void AD_STX_Cmd_set_bitrate(AD_STX_STS_STRUCT* stx, unsigned char ch);
void AD_STX_Cmd_set_power(AD_STX_STS_STRUCT* stx, unsigned char ch);
void AD_STX_Cmd_set_outport(AD_STX_STS_STRUCT* stx, unsigned char tmp);
void AD_STX_Cmd_set_mod(AD_STX_STS_STRUCT* stx, unsigned char tmp);
void AD_STX_Cmd_set_pn_test(AD_STX_STS_STRUCT* stx, unsigned char tmp);



#endif

