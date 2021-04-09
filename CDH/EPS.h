// EPS.h

#ifndef _EPS_h
#define _EPS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Config.h"
#include "TimeManager.h"
#include "EPS_config.h"
#include "PDU.h"
#include "PCU.h"
#include "Utility.h"
#include "FRAM_manager.h"

#define GNSS_ON_PORT	PDU_IOE_CHA_ON_1
#define GNSS_OFF_PORT	PDU_IOE_CHA_OFF_1

#define RW_ON_PORT		PDU_IOE_CHA_OFF_1//PDU_IOE_CHA_ON_1
#define RW_OFF_PORT		PDU_IOE_CHA_ON_1//PDU_IOE_CHA_OFF_1

#define ADCS_ON_PORT	PDU_IOE_CHA_ON_2
#define ADCS_OFF_PORT	PDU_IOE_CHA_OFF_2

#define MSN5V_ON_PORT	PDU_IOE_CHA_ON_3
#define MSN5V_OFF_PORT	PDU_IOE_CHA_OFF_3

#define SUBCAM_ON_PORT	PDU_IOE_CHA_ON_4
#define SUBCAM_OFF_PORT	PDU_IOE_CHA_OFF_4

#define SF_ON_PORT		PDU_IOE_CHA_ON_5
#define SF_OFF_PORT		PDU_IOE_CHA_OFF_5

#define UNREG_ON_PORT	PDU_IOE_CHA_OFF_6//PDU_IOE_CHA_ON_6
#define UNREG_OFF_PORT	PDU_IOE_CHA_ON_6//PDU_IOE_CHA_OFF_6

typedef struct
{
	unsigned char msn_unreg;
	unsigned char heater;
	unsigned char rw;
	unsigned char adcs;
	unsigned char msn5v;
	unsigned char subcam;
	unsigned char sf;
	unsigned char msn_unreg_en;
	unsigned char rw_en;
	unsigned char imu;
	unsigned char strx;
	unsigned char stx;
	unsigned char srx;
	unsigned char srxif;
	unsigned char aqu_5v;	//opt1
	unsigned char ovco;		//opt1
}PWR_STS;

extern PWR_STS	pwr_sts;
extern unsigned char pcu_select;
extern ack eps_access_sts;
extern ack pcu_access_sts;
extern ack pdu_access_sts;

extern float eps_bat_v;

ack		EPS_init(void);
void	EPS_GetStatus(void);
ack		EPS_RESET(void);
void	EPS_SELECT(unsigned char sel);


void	EPS_DebugMessage(void);

ack		PWR_PDU_SET(unsigned char pwr_onoff, unsigned char on_port, unsigned char off_port);
ack		PWR_PDU_UNREG_SET(unsigned char pwr_onoff, unsigned char on_port, unsigned char off_port);
ack		PWR_SAFE_OFF(void);
ack		PWR_UNREG_OBC_SET(unsigned char pwr_onoff);

ack		PWR_UNREG(unsigned char pwr_onoff);
ack		PWR_RW(unsigned char pwr_onoff);
ack		PWR_ADCS(unsigned char pwr_onoff);
ack		PWR_MSN5V(unsigned char pwr_onoff);
ack		PWR_SUBCAM(unsigned char pwr_onoff);
ack		PWR_SF(unsigned char pwr_onoff);

ack		PWR_MSN_UNREG_ENABLE(unsigned char pwr_onoff);
ack		PWR_RW_ENABLE(unsigned char pwr_onoff);
ack		PWR_IMU(unsigned char pwr_onoff);
ack		PWR_HEATER(unsigned char pwr_onoff);

ack		PWR_AD_STX(unsigned char pwr_onoff);
ack		PWR_AD_SRX(unsigned char pwr_onoff);
ack		PWR_SRXIF(unsigned char pwr_onoff);

//OPT1
ack		PWR_AQU_5V(unsigned char pwr_onoff);
ack		PWR_OVCO_5V(unsigned char pwr_onoff);


void		PWR_SRX_RESET(void);
#endif

