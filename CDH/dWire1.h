// dWire1.h.h

#ifndef _DWIRE1.H_h
#define _DWIRE1.H_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DTWI.h"

extern DTWI1 dtwi1;

void			Wire1_Init(void);
bool			Wire1_beginTransmission(unsigned char address);
bool			Wire1_endTransmission(void);
int				Wire1_write(unsigned char data);
unsigned char	Wire1_read(void);
unsigned char	Wire1_requestFrom(unsigned char address, unsigned char quantity);
unsigned char	Wire1_available(void);

#endif

