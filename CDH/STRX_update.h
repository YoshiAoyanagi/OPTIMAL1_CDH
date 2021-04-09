// STRX_update.h

#ifndef _STRX_UPDATE_h
#define _STRX_UPDATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "AppHeaders.h"
#include "AD_STX.h"
#include "AD_SRX.h"

#define DEFAULT_STRX_CLOCK_REPLY_TIME		5
#define DEFAULT_STRX_SLOCK_REPLY_TIME		30

#define	DEFAULT_STX_TRANSMIT_ON_DURATION	10
#define	DEFAULT_STX_TRANSMIT_OFF_DURATION	60

#define	STRX_OTC_DEFAULT_TEMPERATURE		60
#define	STRX_OTC_LOW_TEMPERATURE			30
#define STRX_OTC_THR_COUNT					10

#define DEFAULT_STRX_CLOCK_RSSI_THR			-110

extern unsigned int strx_carrierlock_reply_time;
extern unsigned int strx_subcarrierlock_reply_time;
extern unsigned int stx_transmit_on_duration;
extern unsigned int stx_transmit_off_duration;
extern unsigned char stx_transmit_on;
extern unsigned int stx_transmit_count;
extern unsigned int stx_burst_duration;
extern unsigned int stx_on_remaining;
extern unsigned char strx_otc_enable;
extern unsigned char strx_otc_threshold;
extern float		 strx_rssi_threshold;

extern AD_STX_STS_STRUCT ad_stx_sts;
extern AD_SRX_STS_STRUCT ad_srx_sts;

AppInfo STRX_update(void);
AppInfo STX_transmit(void);

void STX_set_bitrate(unsigned char bps);
void STX_set_power(unsigned char pwr_level);
void STX_set_outport(unsigned char ch);
void STX_set_mod(unsigned char ch); //test
void STX_set_pn(unsigned char ch); //test

void STX_set_ON(void);
void STX_set_OFF(void);

void STRX_OTC_Detect(void);

void STRX_set_OTC_Enable(unsigned char en);
void STRX_set_OTC_Threshold(unsigned char val);
void STX_set_transmit_duration(unsigned int on_duration, unsigned int off_duration);
void STX_set_burst_duration(unsigned int duration);
void STRX_set_lock_reply_time(unsigned int c_lock_time, unsigned int s_lock_time, unsigned char lock_rssi);

void	STRX_debug_message(void);

#endif

