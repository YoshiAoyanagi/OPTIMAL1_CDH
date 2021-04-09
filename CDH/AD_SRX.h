// AD_SRX.h

#ifndef _AD_SRX_h
#define _AD_SRX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "port_config.h"
#include "Config.h"

typedef struct
{
	float		  agc_inc;//RSSI
	float		  agc_coh;//ƒLƒƒƒŠƒAƒŒƒxƒ‹
	float		  freq_err;
	unsigned char carrier_lock;
	unsigned char sub_carrier_lock;
	unsigned char rx_on;
}AD_SRX_STS_STRUCT;



void	AD_SRX_init(AD_SRX_STS_STRUCT *srx);
void	AD_SRX_rec(AD_SRX_STS_STRUCT *srx);

void	AD_SRX_RX_ON(void);
void	AD_SRX_RX_OFF(void);
void	AD_SRX_RX_RESET(void);
//void	AD_SRX_debug_message(void);

#endif

