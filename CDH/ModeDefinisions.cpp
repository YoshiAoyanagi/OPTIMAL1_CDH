// 
// 
// 

#include "ModeDefinisions.h"


#include "BlockCommandDefinisions.h"

void MD_load_mode_list(size_t list[MODE_MAX])
{
	list[SAFE]			= BC_TL_NORMAL;
	list[NORMAL]		= BC_TL_NORMAL;
	list[SPINSUN]		= BC_TL_NORMAL;
	list[SUN_POINTING]	= BC_TL_NORMAL;
	list[EARTH_POINTING]= BC_TL_NORMAL;
	list[BDOT]			= BC_TL_NORMAL;
	list[INITIAL]		= BC_TL_NORMAL;
}

void MD_load_transition_table(size_t table[MODE_MAX][MODE_MAX])
{
	int from, to;

	for (from = 0; from<MODE_MAX; ++from)
	{
		for (to = 0; to<MODE_MAX; ++to)
		{
			table[from][to] = MM_NOT_DEFINED;
		}
	}
	for (from = 0; from<MODE_MAX; ++from)
	{
		table[from][SAFE] = BC_SL_ANY_TO_SAFE;
	}

	for (from = 0; from<MODE_MAX; ++from)
	{
		table[from][INITIAL] = BC_SL_ANY_TO_INITIAL;
	}

	for (from = 0; from<MODE_MAX; ++from)
	{
		table[from][NORMAL]			= BC_SL_ANY_TO_NORMAL;
		table[from][SPINSUN]		= BC_SL_ANY_TO_NORMAL;
		table[from][SUN_POINTING]	= BC_SL_ANY_TO_NORMAL;
		table[from][EARTH_POINTING] = BC_SL_ANY_TO_NORMAL;
		table[from][BDOT]			= BC_SL_ANY_TO_NORMAL;
	}
}
