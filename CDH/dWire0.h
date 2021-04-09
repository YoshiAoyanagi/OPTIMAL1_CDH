// dWire0.h.h

#ifndef _DWIRE0.H_h
#define _DWIRE0.H_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DTWI.h"

extern DTWI0 dtwi0;

void			Wire0_Init(void);
bool			Wire0_beginTransmission(unsigned char address);
bool			Wire0_endTransmission(void);
int				Wire0_write(unsigned char data);
unsigned char	Wire0_read(void);
unsigned char	Wire0_requestFrom(unsigned char address, unsigned char quantity);
unsigned char	Wire0_available(void);

#endif

