// FRAM_manager.h

#ifndef _FRAM_MANAGER_h
#define _FRAM_MANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "FRAM.h"
#include "TimeManager.h"
#include "Utility.h"
#include "ModeManager.h"
#include "port_config.h"
#include "CommonSerialReceiver.h"

#define FRAM_VERIFY_CODE					0xAA
#define FRAM_ADDRESS_VERIFY					0x0000
#define FRAM_ADDRESS_OPS_MODE				0x0010
#define FRAM_ADDRESS_OPS_PREV_MODE			0x0011
#define FRAM_ADDRESS_MASTER_CYCLE			0x0012
#define FRAM_ADDRESS_REBOOT_COUNT			0x0016

#define FRAM_ADDRESS_PWR_UNREG				0x001A
#define FRAM_ADDRESS_PWR_HEATER				0x001B
#define FRAM_ADDRESS_PWR_RW					0x001C
#define FRAM_ADDRESS_PWR_ADCS				0x001D
#define FRAM_ADDRESS_PWR_MSN5V				0x001E
#define FRAM_ADDRESS_PWR_SUBCAM				0x001F
#define FRAM_ADDRESS_PWR_SF					0x0020
#define FRAM_ADDRESS_PWR_MSN_UNREG_EN		0x0021
#define FRAM_ADDRESS_PWR_RW_EN				0x0022

#define FRAM_ADDRESS_UVC_ENA				0x0023
#define FRAM_ADDRESS_SERIAL_MUX				0x0024
#define FRAM_ADDRESS_RTC_SYNC_ENA			0x0025
#define FRAM_ADDRESS_STX_ON_DURATION		0x0026
#define FRAM_ADDRESS_STX_OFF_DURATION		0x0028
#define FRAM_ADDRESS_UVC1_THR				0x002A
#define FRAM_ADDRESS_UVC2_THR				0x002B
#define FRAM_ADDRESS_UVC3_THR				0x002C
#define FRAM_ADDRESS_STRX_OTC_ENA			0x002D
#define FRAM_ADDRESS_STRX_OTC_THR			0x002E
#define FRAM_ADDRESS_STRX_POWER				0x002F
#define FRAM_ADDRESS_STRX_C_LOCK_REPLY_TIME				0x0030
#define FRAM_ADDRESS_STRX_S_LOCK_REPLY_TIME				0x0032
#define FRAM_ADDRESS_STRX_LOCK_RSSI						0x0034 //1Byte

#define FRAM_ADDRESS_TLMD_ANOMALY_ENA		0x0035	//1Byte

#define FRAM_ADDRESS_PWR_AQU_5V				0x0036
#define FRAM_ADDRESS_PWR_OVCO_5V			0x0037

#define FRAM_ADDRESS_DR_OBC_TIME_TAG		0x0038
#define FRAM_ADDRESS_DR_MSN_TIME_TAG		0x0039

//add 20201207
#define FRAM_ADDRESS_RESTART_COUNT			0x0040
#define FRAM_ADDRESS_AC_HEADER				0x0042


#define FRAM_POSITION_AC_BC_ID				0
#define FRAM_POSITION_AC_BC_ACTIVATED		1
#define FRAM_POSITION_AC_EXEC_TIMES			2
#define FRAM_LENGTH_AC_HEADER				4
#define FRAM_AC_HEADER_MAX					16
//


ack FRAM_Begin(void);
ack FRAM_Write(unsigned char *data, unsigned int address);

ack FRAM_Write_rtc_sync_ena(ENABLE_STS ena);
ack FRAM_Write_uvc_ena(ENABLE_STS ena);
ack FRAM_Write_uvc_threshold(unsigned char v1, unsigned char v2, unsigned char v3);
ack FRAM_Write_ops_mode(ModeManagerInfo mode);
ack FRAM_Write_master_cycle(unsigned int time);
ack FRAM_Write_reboot_count(unsigned int count);
ack FRAM_Write_PWR_STS(unsigned int address, unsigned char pwr_sts);
ack FRAM_Write_serial_mux(unsigned char port);
ack FRAM_Write_stx_on_off_duration(unsigned int on_dur, unsigned int off_dur);
ack FRAM_Write_strx_otc_ena(unsigned char ena);
ack FRAM_Write_strx_otc_thr(unsigned char thr);
ack FRAM_Write_strx_power(unsigned char pwr);
ack FRAM_Write_strx_lock_reply_time(unsigned int c_lock, unsigned int s_lock, unsigned char lock_rssi);

ack FRAM_Write_tlmd_anomaly_ena(unsigned char ena);

//add 20201207
ack FRAM_Write_restart_count(unsigned short times);
ack FRAM_Write_auto_ctrl_info(unsigned char auto_ctrl_id, unsigned char bc_id, unsigned char activate, unsigned short exec_times);

ack FRAM_Write_dr_obc_timetag_ena(unsigned char ena);
ack FRAM_Write_dr_msn_timetag_ena(unsigned char ena);


ack FRAM_All_Initialize(void);

ack FRAM_Read_verify(void);
ack FRAM_Read_rtc_sync_ena(ENABLE_STS *ena);
ack FRAM_Read_uvc_ena(ENABLE_STS *ena);
ack FRAM_Read_uvc_threshold(unsigned char *v1, unsigned char *v2, unsigned char *v3);
ack FRAM_Read_ops_mode(MD_ModeID *current_mode, MD_ModeID *prev_mode);
ack FRAM_Read_master_cycle(unsigned int *time);
ack FRAM_Read_reboot_count(unsigned int *count);
ack FRAM_Read_PWR_STS(unsigned int address, unsigned char *pwr_sts);
ack FRAM_Read_serial_mux(unsigned char *port);
ack FRAM_Read_stx_on_off_duration(unsigned int *on_dur, unsigned int *off_dur);
ack FRAM_Read_strx_otc_ena(unsigned char *ena);
ack FRAM_Read_strx_otc_thr(unsigned char *thr);
ack FRAM_Read_strx_power(unsigned char *pwr);
ack FRAM_Read_strx_lock_reply_time(unsigned int* c_lock, unsigned int* s_lock, unsigned char* lock_rssi);

ack FRAM_Read_tlmd_anomaly_ena(unsigned char* ena);


//add 20201207
ack FRAM_Read_restart_count(unsigned short *times);
ack FRAM_Read_auto_ctrl_info(unsigned char auto_ctrl_id, unsigned char *bc_id, unsigned char *activate, unsigned short *exec_times);

ack FRAM_Read_dr_obc_timetag_ena(unsigned char *ena);
ack FRAM_Read_dr_msn_timetag_ena(unsigned char* ena);

#endif