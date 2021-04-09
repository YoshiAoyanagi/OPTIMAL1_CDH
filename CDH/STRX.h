// STRX.h

#ifndef _STRX_h
#define _STRX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "port_config.h"
#include "Config.h"
#include "Utility.h"

/**
* @brief STRXの受信ステータスを格納するための構造体です。
*/

#define	STRX_STX	0x2A
#define	STRX_ETX1	0x0D
#define	STRX_ETX2	0x0A

#define STX_BR_64KBPS	0x03
#define STX_BR_32KBPS	0x02
#define STX_BR_16KBPS	0x01
#define STX_BR_4KBPS	0x00

typedef struct
{
	unsigned char spc_counter;
	unsigned char spc_obc_reset_counter;
	unsigned char spc_pcu_reset_counter;
	unsigned char spc_tx_suspension_counter;
	unsigned char spc_tx_resumption_counter;
	unsigned char wdt_counter;
	short		  rssi;
	float		  rx_freq;
	short		  tempe;
	unsigned char volt;
	unsigned char carrier_lock;
	unsigned char sub_carrier_lock;
	unsigned char tx_power;
	unsigned char pwr_level;
	unsigned char bitrate;
	unsigned char others;
}STRX_STS_STRUCT;

extern STRX_STS_STRUCT strx_sts;

int STRX_rec(void);

void STRX_set_cmd(unsigned char pwr_level, unsigned char bit_rate);
void STRX_DebugMessage(void);
#endif

