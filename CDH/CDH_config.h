#ifndef COM_config_H
#define COM_config_H

//#define		UART_DEBUG

//#define		MSG_DEBUG

#define _COM

#ifndef _COM
#define _GROUND_STATION
#endif

//pheripheral
#define BITRATE_DEBUG 115200  //bps
#define BITRATE_MOBC  115200  //bps
#define BITRATE_IRDA   57600  //bps
#define BITRATE_ACS    38400  //bps
#define BITRATE_SF     38400  //bps

#define Debug_SERIAL	Serial
#define MOBC_SERIAL		Serial1
#define IrDA_SERIAL		Serial2
#define ACS_SERIAL		Serial3
#define SF_SERIAL		Serial3

#endif