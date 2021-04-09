// TlmCmdTL.h

#ifndef _TLMCMDTL_h
#define _TLMCMDTL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <stddef.h> // for size_t

void BC_load_tlmcmd_task(size_t pos);

#endif

