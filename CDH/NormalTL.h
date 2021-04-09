// NormalTL.h

#ifndef _NORMALTL_h
#define _NORMALTL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <stddef.h> // for size_t

void BC_load_normal_mode(size_t pos);

#endif // NORMAL_TL_H_

