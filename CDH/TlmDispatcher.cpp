// 
// 
// 

#include "TlmDispatcher.h"

#include <string.h> // for memcmp

#include "utility.h"
#include "PacketHandler.h"
#include "cmd_analyze_obc.h"
#include "tlm_frame.h"
#include "BlockCommandDefinisions.h"
#include "STRX_update.h"
#include "SerialTransmitter.h"

unsigned char tlmd_anomaly_detect_enable;

void TLMD_anomaly_detect(void);

void TLMD_initialize(void)
{
	tlmd_anomaly_detect_enable = 1;

	FRAM_Read_tlmd_anomaly_ena(&tlmd_anomaly_detect_enable);
	PL_clear_list(&rt_tlm_list);
}


void TLMD_execute(void)
{

	static unsigned int step;
	static unsigned int cycle;

	unsigned int dif;

	CTCP* tpkt = NULL;

	TLMD_anomaly_detect();	//add 202012/25

	if (TMGR_get_step() < step)
		cycle = 0;

	step = TMGR_get_step();

	if (step > (10 * cycle))//if (step > (10 * cycle))//step > 10ms
	{
#ifndef UART_DEBUG
		if (PORTEbits.RE8 == LOW)	//アクティブロー(20200831)
#endif
		{
			if (PL_is_empty(&rt_tlm_list))
			{
#ifndef UART_DEBUG
				SerialDummyTransmit();
#endif
				return;
			}
			else
			{
				tpkt = &(PL_get_head(&rt_tlm_list)->packet);
				//Serial.println("PH_send_realtime_telemetry(tpkt)");
				if (PH_send_realtime_telemetry(tpkt) == PH_SUCCESS)
				{
					PL_drop_executed(&rt_tlm_list);
				}
			}
		}
		cycle++;
	}
}

void TLMD_anomaly_detect(void)
{
	static unsigned int start_cycle;
	static unsigned int cycle;

	if (tlmd_anomaly_detect_enable == 0)
		return;

	if (stx_transmit_count <= 1)	//0 or 1
	{
		cycle = TMGR_get_master_cycle() - start_cycle;

		if (cycle > 10 * OBCT_CYCLES_PER_SEC)	//10秒間連続でtransmit_countがゼロの場合
		{
			STX_set_OFF();
#ifdef MSG_DEBUG
			Serial.println("Detect: TLMD_anomaly");
#endif
			start_cycle = TMGR_get_master_cycle();	//start_cycle reset
		}
	}
	else
	{
		start_cycle = TMGR_get_master_cycle();
	}
}

