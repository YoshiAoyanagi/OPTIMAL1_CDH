// 
// 
// 

#include "RT_TlmDispatcher.h"

#include <string.h> // for memcmp

#include "utility.h"
#include "PacketHandler.h"
#include "cmd_analyze_obc.h"
#include "tlm_frame.h"
#include "BlockCommandDefinisions.h"
#include "STRX_update.h"
#include "SerialTransmitter.h"

static void rt_tlm_transmitter_init_(void);
static void rt_tlm_transmitter_(void);

AppInfo rt_tlm_transmitter(void)
{
	return create_app_info("rttlm", 1, rt_tlm_transmitter_init_, rt_tlm_transmitter_);
	//return create_app_info("rttlm", 1, rt_tlm_transmitter_init_, rt_tlm_transmitter_);
}

static void rt_tlm_transmitter_init_(void)
{
	PL_clear_list(&rt_tlm_list);
}

void rt_tlm_transmitter_(void)
{
	CTCP* tpkt = NULL;
	if (PL_is_empty(&rt_tlm_list))
	{
#ifndef UART_DEBUG
		SerialDummyTransmit();
#endif
		return;
	}

	tpkt = &(PL_get_head(&rt_tlm_list)->packet);

	if (PH_send_realtime_telemetry(tpkt) == PH_SUCCESS)
	{
		PL_drop_executed(&rt_tlm_list);
	}
}


void rt_tlm_transmitter_irq(void)
{

	static unsigned int step;
	static unsigned int cycle;

	unsigned int dif;

	CTCP* tpkt = NULL;

	if (TMGR_get_step() < step)
		cycle = 0;

	step = TMGR_get_step();

	if (step > (10 * cycle))
	{
		
		if (PL_is_empty(&rt_tlm_list))
		{
#ifndef UART_DEBUG
			SerialDummyTransmit();
#endif
			return;
		}

		tpkt = &(PL_get_head(&rt_tlm_list)->packet);

		if (PH_send_realtime_telemetry(tpkt) == PH_SUCCESS)
		{
			PL_drop_executed(&rt_tlm_list);
		}
		
		cycle++;
	}
}
