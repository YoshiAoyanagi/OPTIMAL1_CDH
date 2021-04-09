// SafeSL.h

#ifndef _SAFESL_h
#define _SAFESL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <stddef.h>
#include "cmd_item.h"

void BC_load_any_to_safe(size_t pos);

#endif

