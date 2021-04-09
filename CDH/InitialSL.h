// InitialSL.h

#ifndef _INITIALSL_h
#define _INITIALSL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <stddef.h>
#include "cmd_item.h"

void BC_load_any_to_initial(size_t pos);

#endif

