#ifndef CDH_config_H
#define CDH_config_H

#include "arduino.h"

//Hardware

#define CPU_TICKS_CORRECT		1.041567829

#define CPU_TICKS_PER_SECOND 480 * CPU_TICKS_CORRECT

//#define		UART_DEBUG
#define		MSG_DEBUG
#define		PRINT_DEBUG

#define OPTIMAL1
//#define GSAT
//#define OTHER_SAT

#ifdef GSAT
#define SC_CMD_ID	0x10
#elif defined OPTIMAL1
#define SC_CMD_ID	0x11
#else
#define SC_CMD_ID	0x00
#endif

//STX
#define STX_BITRATE_4KBPS	2
#define STX_BITRATE_32KBPS	1
#define STX_BITRATE_64KBPS	0

#define STX_BITRATE_INITIAL	STX_BITRATE_4KBPS

//pheripheral
#define BITRATE_DEBUG 115200  //bps
#define BITRATE_STRX  115200	//19200   //bps
#define BITRATE_DATA  115200  //57600  //bps
#define BITRATE_ADCS  115200  //bps
#define BITRATE_MSN	  115200  //bps
#define BITRATE_GNSSR 9600  //bps

#define DEBUG_Serial	Serial
#define Debug_Serial	Serial

#define DATA_Serial		Serial5
#define STRX_Serial		Serial2
#define ADCS_Serial		Serial3 
#define MSN_Serial		Serial4	
#define GNSSR_Serial	Serial1	

#endif