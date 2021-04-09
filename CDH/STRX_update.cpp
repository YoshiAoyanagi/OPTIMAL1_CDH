//
#include "STRX_update.h"
#include "cmd_item.h"
#include "FRAM_manager.h"
#include "PacketHandler.h"

AD_STX_STS_STRUCT ad_stx_sts;
AD_SRX_STS_STRUCT ad_srx_sts;

static void STRX_init_(void);
static void STRX_update_(void);
static void STX_tx_init_(void);
static void STX_transmit_(void);
static void STX_sequence_ON(void);

unsigned int strx_carrierlock_reply_time;
unsigned int strx_subcarrierlock_reply_time;
float		 strx_rssi_threshold;

unsigned int stx_transmit_on_duration;
unsigned int stx_transmit_off_duration;
unsigned int stx_burst_duration;
unsigned int stx_transmit_count;
unsigned char stx_transmit_on;

unsigned int stx_on_remaining;
unsigned char strx_otc_enable;
unsigned char strx_otc_threshold;


void STRX_carrierlock_handler(void);
void STRX_subcarrierlock_handler(void);

AppInfo STRX_update(void)
{
	return create_app_info("STRXU", 10, STRX_init_, STRX_update_);
}

static void STRX_init_(void)
{
	unsigned char tmp = 0;

	strx_otc_enable = 1;
	strx_otc_threshold = STRX_OTC_DEFAULT_TEMPERATURE;

	strx_carrierlock_reply_time = DEFAULT_STRX_CLOCK_REPLY_TIME;
	strx_subcarrierlock_reply_time = DEFAULT_STRX_SLOCK_REPLY_TIME;

	strx_rssi_threshold = DEFAULT_STRX_CLOCK_RSSI_THR;

	PWR_SRX_RESET();
	PWR_AD_STX(PWR_OFF);

	AD_SRX_init(&ad_srx_sts);
	AD_STX_init(&ad_stx_sts);

	//設定読み出し
	FRAM_Read_strx_otc_ena(&strx_otc_enable);

	FRAM_Read_strx_otc_thr(&strx_otc_threshold);
	if (strx_otc_threshold == 0)
	{
		strx_otc_threshold = STRX_OTC_DEFAULT_TEMPERATURE;
	}

	FRAM_Read_strx_lock_reply_time(&strx_carrierlock_reply_time, &strx_subcarrierlock_reply_time, &tmp);

	if (strx_carrierlock_reply_time == 0 && strx_subcarrierlock_reply_time == 0)
	{
		strx_carrierlock_reply_time = DEFAULT_STRX_CLOCK_REPLY_TIME;
		strx_subcarrierlock_reply_time = DEFAULT_STRX_SLOCK_REPLY_TIME;
	}
	strx_rssi_threshold = -1 * (float)tmp;

	if (strx_rssi_threshold == 0)
	{
		strx_rssi_threshold = DEFAULT_STRX_CLOCK_RSSI_THR;
	}
}

static void STRX_update_(void)
{
	AD_SRX_rec(&ad_srx_sts);
	AD_STX_rec(&ad_stx_sts);

	STRX_OTC_Detect();

	STRX_carrierlock_handler();
	STRX_subcarrierlock_handler();

}

void STX_set_bitrate(unsigned char br)
{
	if (br == 0xFF)
	{
		AD_STX_Cmd_set_bitrate(&ad_stx_sts, ad_stx_sts.set_bitrate);
	}
	else
	{
		AD_STX_Cmd_set_bitrate(&ad_stx_sts, br);
	}
}

void STX_set_power(unsigned char pwr_level)
{
	if (pwr_level == 0xFF)
	{
		AD_STX_Cmd_set_power(&ad_stx_sts, ad_stx_sts.set_power);
	}
	else
	{
		AD_STX_Cmd_set_power(&ad_stx_sts, pwr_level);
		FRAM_Write_strx_power(pwr_level);
	}
}

void STX_set_outport(unsigned char ch)
{
	if (ch == 0xFF)
	{
		AD_STX_Cmd_set_outport(&ad_stx_sts, ad_stx_sts.set_outport);
	}
	else
	{
		AD_STX_Cmd_set_outport(&ad_stx_sts, ch);
	}
}

void STX_set_mod(unsigned char ch)
{
	AD_STX_Cmd_set_mod(&ad_stx_sts, ch);
}

void STX_set_pn(unsigned char ch)
{
	AD_STX_Cmd_set_pn_test(&ad_stx_sts, ch);
}

AppInfo STX_transmit(void)
{
	return create_app_info("STXT", 10, STX_tx_init_, STX_transmit_);
}

static void STX_tx_init_(void)
{
	unsigned int tmp_on_dur;
	unsigned int tmp_off_dur;

	stx_burst_duration = 0;

	stx_transmit_on_duration = DEFAULT_STX_TRANSMIT_ON_DURATION;
	stx_transmit_off_duration = DEFAULT_STX_TRANSMIT_OFF_DURATION;

	FRAM_Read_stx_on_off_duration(&tmp_on_dur, &tmp_off_dur);

	if (tmp_on_dur != 0 && tmp_off_dur != 0)
	{
		stx_transmit_on_duration = tmp_on_dur;
		stx_transmit_off_duration = tmp_off_dur;
	}

	if (stx_transmit_on_duration == 0)
	{
		STX_set_OFF();
	}
	else
	{
		STX_sequence_ON();
	}
}

static void STX_transmit_(void)
{

	if (stx_burst_duration > 0)
	{
		if (stx_transmit_count >= (stx_burst_duration * OBCT_CYCLES_PER_SEC))
		{
			STX_set_OFF();
			stx_burst_duration = 0;
			stx_on_remaining = 0;
		}
		else
			stx_on_remaining = stx_burst_duration - (stx_transmit_count / OBCT_CYCLES_PER_SEC);
	}
	else
	{
		if (stx_transmit_on == 0)
		{
			if (stx_transmit_count >= (stx_transmit_off_duration * OBCT_CYCLES_PER_SEC))
			{
				STX_sequence_ON();
			}
		}
		else
		{
			if (stx_transmit_count >= (stx_transmit_on_duration * OBCT_CYCLES_PER_SEC))
			{
				STX_set_OFF();
				stx_on_remaining = 0;
			}
			else
				stx_on_remaining = stx_transmit_on_duration - (stx_transmit_count / OBCT_CYCLES_PER_SEC);
		}
	}

	stx_transmit_count++;

	/*
	static cycle_t tmp_cycle;
	cycle_t diff_cycle;

	if (tmp_cycle != 0)
	{
		diff_cycle = TMGR_get_master_cycle() - tmp_cycle;
		stx_transmit_count += diff_cycle;
	}
	tmp_cycle = TMGR_get_master_cycle();
	*/
}

//setting
void STX_sequence_ON(void)
{
	CTCP packet;
	unsigned char param;

	STX_set_ON();

	//設定は時間差おきたいので，コマンド登録して実行
	param = ad_stx_sts.set_power;
	packet = CCP_form_rt_cmd(CODE_Cmd_STRX_SET_TX_PWR, &param, 1u);
	add_rt_cmd(&packet);

	param = ad_stx_sts.set_outport;
	packet = CCP_form_rt_cmd(CODE_Cmd_STX_SET_OUTPORT, &param, 1u);
	add_rt_cmd(&packet);

	param = ad_stx_sts.set_bitrate;
	packet = CCP_form_rt_cmd(CODE_Cmd_STRX_SET_BR, &param, 1u);
	add_rt_cmd(&packet);
}

void STX_set_ON(void)
{
	CTCP packet;
	unsigned char param;

	stx_transmit_on = 1;
	stx_transmit_count = 0;

	PWR_AD_STX(PWR_ON);

}
void STX_set_OFF(void)
{
	stx_transmit_on = 0;
	stx_transmit_count = 0;
	stx_burst_duration = 0;
	stx_on_remaining = 0;

	PWR_AD_STX(PWR_OFF);
}


void STX_set_transmit_duration(unsigned int on_duration, unsigned int off_duration)
{
	stx_transmit_on_duration = on_duration;
	stx_transmit_off_duration = off_duration;

	if (on_duration > (30 * 60))
		return;
	if (off_duration == 0)
		return;

	FRAM_Write_stx_on_off_duration(on_duration, off_duration);

	STX_sequence_ON();
}

void STX_set_burst_duration(unsigned int duration)
{
	stx_burst_duration = duration;
	STX_sequence_ON();
}

void STRX_OTC_Detect(void)
{
	static int otc_count;
	CTCP packet;
	unsigned char param = (unsigned char)PWR_OFF;

	unsigned char tmp = (unsigned char)(ad_stx_sts.tmpe);//ここダメ

	if (strx_otc_enable && (temp_sens.an_tlm[3].float_value > (float)strx_otc_threshold))
	{
		otc_count++;
		if (otc_count > STRX_OTC_THR_COUNT)
		{
			otc_count = 0;
			packet = CCP_form_rt_cmd(CODE_Cmd_STRX_TX_ON, &param, 1u);
			RT_Cmd_Register(packet);
		}
	}
	else
	{
		otc_count = 0;
	}
}

void STRX_set_OTC_Enable(unsigned char en)
{
	if (en > 1)
		return;

	strx_otc_enable = en;
	FRAM_Write_strx_otc_ena(en);
}

void STRX_set_OTC_Threshold(unsigned char val)
{
	strx_otc_threshold = val;
	FRAM_Write_strx_otc_thr(val);
}


//Sweep対応
void STRX_carrierlock_handler(void)
{
	static unsigned char sts_carrier_lock;
	static unsigned char raise_carrier_lock;
	static unsigned int lock_count;
	static unsigned int unlock_count;

	if (strx_carrierlock_reply_time == 0)
		return;

	//DEBUG_Serial.print("raise_carrier_lock="); DEBUG_Serial.println(raise_carrier_lock);
	//DEBUG_Serial.print("ad_srx_sts.carrier_lock="); DEBUG_Serial.println(ad_srx_sts.carrier_lock);
	//DEBUG_Serial.print("ad_srx_sts.agc_inc="); DEBUG_Serial.println(ad_srx_sts.agc_inc);

	if (raise_carrier_lock == 0 && ad_srx_sts.carrier_lock && ad_srx_sts.agc_inc > strx_rssi_threshold)
	{
		lock_count++;
		if (lock_count > 5)
		{
			sts_carrier_lock = 1;
			raise_carrier_lock = 1;
			lock_count = 0;

			if (raise_carrier_lock == 1 && sts_carrier_lock == 1)//
			{
#ifdef PRINT_DEBUG
				DEBUG_Serial.println("Detect: SRX_CarrierLock");
#endif
				//TLM応答
				if (stx_on_remaining < strx_carrierlock_reply_time)
				{
					STX_set_burst_duration(strx_carrierlock_reply_time);//default5s応答
				}
			}

		}
		unlock_count = 0;
	}
	else if (ad_srx_sts.carrier_lock == 0 || ad_srx_sts.agc_inc < strx_rssi_threshold)
	{
		unlock_count++;
		if (unlock_count > 5)
		{
			sts_carrier_lock = 0;
			raise_carrier_lock = 0;
			unlock_count = 0;
		}
		lock_count = 0;
	}
	else
	{
		lock_count = 0;
		unlock_count = 0;
	}
}

void STRX_subcarrierlock_handler(void)
{
	static unsigned char sts_lock;
	static unsigned char raise_lock;
	static unsigned int lock_count;
	static unsigned int unlock_count;

	if (strx_carrierlock_reply_time == 0)
		return;

	if (raise_lock == 0 && ad_srx_sts.carrier_lock && ad_srx_sts.sub_carrier_lock && ad_srx_sts.agc_inc > strx_rssi_threshold)
	{
		lock_count++;
		if (lock_count > 5)
		{
			sts_lock = 1;
			raise_lock = 1;
			lock_count = 0;

			if (raise_lock == 1 && sts_lock == 1)//
			{
#ifdef PRINT_DEBUG
				DEBUG_Serial.println("Detect: SRX_SubCarrierLock");
#endif
				//TLM応答
				if (stx_on_remaining < strx_subcarrierlock_reply_time)
				{
					STX_set_burst_duration(strx_subcarrierlock_reply_time);//default5s応答
				}
			}

		}
		unlock_count = 0;
	}
	else if (ad_srx_sts.sub_carrier_lock == 0 || ad_srx_sts.agc_inc < strx_rssi_threshold)
	{
		unlock_count++;
		if (unlock_count > 5)
		{
			sts_lock = 0;
			raise_lock = 0;
			unlock_count = 0;
		}
		lock_count = 0;
	}
	else
	{
		lock_count = 0;
		unlock_count = 0;
	}
}

void STRX_set_lock_reply_time(unsigned int c_lock_time, unsigned int s_lock_time, unsigned char lock_rssi)
{
	strx_carrierlock_reply_time = c_lock_time;
	strx_subcarrierlock_reply_time = s_lock_time;
	strx_rssi_threshold = -1 * (float)lock_rssi;

	FRAM_Write_strx_lock_reply_time(c_lock_time, s_lock_time, lock_rssi);
}


void	STRX_debug_message(void)
{
	DEBUG_Serial.print("[SRX]: ");
	DEBUG_Serial.print("C_LOCK: "); DEBUG_Serial.print(ad_srx_sts.carrier_lock); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("SC_LOCK: "); DEBUG_Serial.print(ad_srx_sts.sub_carrier_lock); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("RSSI : "); DEBUG_Serial.print(ad_srx_sts.agc_inc, 4); DEBUG_Serial.print(", ");

	DEBUG_Serial.println("");
	//DEBUG_Serial.print("AGC_COH : "); DEBUG_Serial.print(ad_srx_sts.agc_coh, 4); DEBUG_Serial.print(", ");
	//DEBUG_Serial.print("FREQ_ERR : "); DEBUG_Serial.print(ad_srx_sts.freq_err, 4); DEBUG_Serial.print(", ");

	//DEBUG_Serial.println("");

	/*
	DEBUG_Serial.println("[STX]: ");
	DEBUG_Serial.print("PORT: "); DEBUG_Serial.print(ad_stx_sts.use_port); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("BR: "); DEBUG_Serial.print(ad_stx_sts.bitrate); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("SET_BR: "); DEBUG_Serial.print(ad_stx_sts.set_bitrate); DEBUG_Serial.print(", ");

	DEBUG_Serial.println("");

	DEBUG_Serial.print("SET_PORT: "); DEBUG_Serial.print(ad_stx_sts.set_outport); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("SET_POWER: "); DEBUG_Serial.print(ad_stx_sts.set_power); DEBUG_Serial.print(", ");
	DEBUG_Serial.print("TMPE: "); DEBUG_Serial.print(ad_stx_sts.tmpe); DEBUG_Serial.print(", ");

	DEBUG_Serial.println("");
	*/

}
