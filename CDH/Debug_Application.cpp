// 
// 
// 

#include "Debug_Application.h"
#include "Temperature.h"
#include "STRX_update.h"
#include "RTC.h"

static cycle_t debug_interval_cycle;
void 	print_debug_message_(void);
void	_tlmcmd_debug_message(void);
void debug_message(void);

AppInfo print_debug_message(void)
{
	return create_app_info("dbg_app", 30, NULL, print_debug_message_);
}

void print_debug_message_(void)
{
	static unsigned int interval;
	static unsigned int cycle;

	if (TMGR_get_master_cycle() < cycle)
		interval = 0;

	cycle = TMGR_get_master_cycle();

	if (cycle > ((DEBUG_TASK_INTERVAL_SEC * OBCT_CYCLES_PER_SEC) * interval))
	{
#ifdef MSG_DEBUG
		debug_message();
#endif
		interval++;
	}
}

void debug_message(void)
{
	//DEBUG_Serial.println("\r\nTRICOM2 Satndard Satellite BUS:");
	DEBUG_Serial.println("\r\nOPTIMAL-1:");
	DEBUG_Serial.print("clock: "); DEBUG_Serial.print(master_clock->master); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("utime: "); DEBUG_Serial.print(master_clock->utime); DEBUG_Serial.print("\r\n");
	DEBUG_Serial.print("millis: "); DEBUG_Serial.print(millis()); DEBUG_Serial.print("\r\n");

	RTC_Debug_message();
	_tlmcmd_debug_message();
	EPS_DebugMessage();
	//TempSens_DebugMessage();
	STRX_debug_message();
	//lora_debug_message();
	DEBUG_Serial.println("");
}

void	_tlmcmd_debug_message(void)
{
	//DEBUG_Serial.print("TlmCount[01]: "); DEBUG_Serial.print(TlmCount[TLM_ID_01]); DEBUG_Serial.println(", ");

	DEBUG_Serial.print("GS : cmdCount: "); DEBUG_Serial.print(GsCmdCount); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("CmdCode: "); DEBUG_Serial.print(GsCmdCode); DEBUG_Serial.print(", ");
	//DEBUG_Serial.print("CmdErrorSts: "); DEBUG_Serial.print(GsCmdErrorSts); DEBUG_Serial.print(", ");
	//DEBUG_Serial.print("CmdSts: "); DEBUG_Serial.print(GsCmdSts); 
	//DEBUG_Serial.print("\r\n");

	//DEBUG_Serial.print("CDH: cmdCount: "); DEBUG_Serial.print(CdhCmdCount); DEBUG_Serial.print(", ");
	//DEBUG_Serial.print("CmdCode: "); DEBUG_Serial.print(CdhCmdCode); DEBUG_Serial.println("");

	DEBUG_Serial.print("RT_TLM: "); DEBUG_Serial.print(rt_tlm_list.pl_public.active_nodes); DEBUG_Serial.print("/");  DEBUG_Serial.print(rt_tlm_list.pl_public.total_nodes);
	DEBUG_Serial.println("");

}


void	debug_cmd_menue_stx(void)
{
	Debug_println("=====================");
	Debug_println("==DEBUG_CONSOLE_STX==");
	Debug_println(" on: STX on");
	Debug_println("off: STX off");
	Debug_println(" 64: Set 64kbps");
	Debug_println(" 32: Set 32kbps");
	Debug_println("  4: Set 4kbps");
	Debug_println(" cw: Set CW mode");
	Debug_println("mod: Set Modulation");
	Debug_println(" p1: Set Port 1");
	Debug_println(" p2: Set Port 2");
	Debug_println("low: Set Low power");
	Debug_println("high: Set High power");
	Debug_println("d  : display debug message");
	Debug_println("=====================");

}
void	debug_cmd_console_stx(void)
{
	unsigned char dbg_len;
	dbg_len = Serial.available();
	if (dbg_len > 0)
	{
		String str = Serial.readStringUntil('\r');
		if (str == "on")
		{
			Debug_println("STX on");
			STX_set_ON();
		}
		if (str == "off")
		{
			Debug_println("STX off");
			STX_set_OFF();
		}
		if (str == "64")
		{
			Debug_println("Set 64kbps");
			STX_set_bitrate(0);
		}
		if (str == "32")
		{
			Debug_println("Set 32kbps");
			STX_set_bitrate(1);
		}
		if (str == "4")
		{
			Debug_println("Set 4kbps");
			STX_set_bitrate(2);
		}
		if (str == "cw")
		{
			Debug_println("Set CW");
			STX_set_mod(0);
		}
		if (str == "mod")
		{
			Debug_println("Set MOD");
			STX_set_mod(1);
		}
		if (str == "p1")
		{
			Debug_println("Set PORT 1");
			STX_set_outport(0);
		}
		if (str == "p2")
		{
			Debug_println("Set PORT 2");
			STX_set_outport(1);
		}
		if (str == "low")
		{
			Debug_println("Set LOW power");
			STX_set_power(0);
		}
		if (str == "high")
		{
			Debug_println("Set HIGH power");
			STX_set_power(1);
		}
	}
}