// ModeDefinisions.h

#ifndef _MODEDEFINISIONS_h
#define _MODEDEFINISIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <stddef.h> // for size_t

#include "BlockCommandTable.h"

#define MM_NOT_DEFINED (BCT_MAX_BLOCKS)

typedef enum
{
	SAFE,			//0
	NORMAL,			//1
	SPINSUN,		//2
	SUN_POINTING,	//3
	EARTH_POINTING,	//4
	BDOT,			//5
	INITIAL,		//6
	RESERVE1,
	RESERVE2,
	MODE_MAX
} MD_ModeID;


void MD_load_mode_list(size_t list[MODE_MAX]);
void MD_load_transition_table(size_t table[MODE_MAX][MODE_MAX]);

#endif // MODE_DEFINISIION_H_