// SerialTransmitter.h

#ifndef _SERIALTRANSMITTER_h
#define _SERIALTRANSMITTER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define AD_STRX_FIFO_SIZE	256//512	//Byte
#define AD_STRX_RTS_TIMEOUT	1000

void SerialTransmit_init(void);
int	SerialTransmit(unsigned char* data, unsigned int len);
int	SerialDummyTransmit(void);

#endif

