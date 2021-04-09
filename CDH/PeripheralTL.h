// PeripheralTL.h

#ifndef _PERIPHERALTL_h
#define _PERIPHERALTL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <stddef.h> // for size_t

void BC_load_peripheral_task(size_t pos);


#endif

