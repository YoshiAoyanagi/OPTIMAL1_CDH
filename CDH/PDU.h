// PDU.h

#ifndef _PDU_h
#define _PDU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "port_config.h"
#include "Config.h"
#include "MCP3421.h"
#include "MCP4716.h"
#include "MCP23017.h"
#include "Utility.h"
#include "EPS_config.h"

#define PDU_MUX_PORT_CURRENT_MON1		0x00
#define PDU_MUX_PORT_CURRENT_MON2		0x01
#define PDU_MUX_PORT_CURRENT_MON3		0x02
#define PDU_MUX_PORT_CURRENT_MON4		0x03
#define PDU_MUX_PORT_CURRENT_MON5		0x04
#define PDU_MUX_PORT_CURRENT_MON6		0x05
#define PDU_MUX_PORT_CURRENT_MON7		0x06
#define PDU_MUX_PORT_CURRENT_MON8		0x07
#define PDU_MUX_PORT_VOLTAGE_MON1		0x08
#define PDU_MUX_PORT_VOLTAGE_MON2		0x09
#define PDU_MUX_PORT_VOLTAGE_MON3		0x0A
#define PDU_MUX_PORT_VOLTAGE_MON4		0x0B
#define PDU_MUX_PORT_VOLTAGE_MON5		0x0C
#define PDU_MUX_PORT_VOLTAGE_MON6		0x0D
#define PDU_MUX_PORT_VOLTAGE_MON7		0x0E
#define PDU_MUX_PORT_VOLTAGE_MON8		0x0F

#define PDU_IOE_MUX_SEL0	0
#define PDU_IOE_MUX_SEL1	1
#define PDU_IOE_MUX_SEL2	2
#define PDU_IOE_MUX_SEL3	3
#define PDU_IOE_CHA_ON_5	4
#define PDU_IOE_CHA_OFF_5	5
#define PDU_IOE_CHA_ON_6	6
#define PDU_IOE_CHA_OFF_6	7
#define PDU_IOE_CHA_ON_1	8
#define PDU_IOE_CHA_OFF_1	9
#define PDU_IOE_CHA_ON_2	10
#define PDU_IOE_CHA_OFF_2	11
#define PDU_IOE_CHA_ON_3	12
#define PDU_IOE_CHA_OFF_3	13
#define PDU_IOE_CHA_ON_4	14
#define PDU_IOE_CHA_OFF_4	15

//OPT1 param(2021/02/12 update)
#define PDU_FL_COEFF_RW_I		0.7825 * 1000	//mA
#define PDU_FL_COEFF_RW_V		5.0980			//V
#define PDU_FL_COEFF_ADCS_I		0.4950 * 1000	//mA
#define PDU_FL_COEFF_ADCS_V		4.0276			//V
#define PDU_FL_COEFF_MSN5V_I	1.2285 * 1000	//mA
#define PDU_FL_COEFF_MSN5V_V	4.0261			//V
#define PDU_FL_COEFF_SUBCAM_I	1.2225* 1000	//mA
#define PDU_FL_COEFF_SUBCAM_V	4.0352			//V
#define PDU_FL_COEFF_SF_I		0.7764 * 1000	//mA
#define PDU_FL_COEFF_SF_V		4.0352			//V
#define PDU_FL_COEFF_UNREG_I	1.0 * 1000//0.2041 * 1000	//mA
#define PDU_FL_COEFF_UNREG_V	5.1020			//V
#define PDU_FL_COEFF_STRX_I		1.6340 * 1000	//mA
#define PDU_FL_COEFF_STRX_V		5.0665			//V
#define PDU_FL_COEFF_HEATER_I	0.7776 * 1000	//mA
#define PDU_FL_COEFF_HEATER_V	4.0305			//V

#define PDU_FL_COEFF_V_TO_BYTE	(16)//(256 / 16)
#define PDU_FL_COEFF_I_TO_BYTE	(0.085)//(256/3000)

typedef struct
{
	AN_TLM rw_current;	//1
	AN_TLM adcs_current;	//2
	AN_TLM msn5v_current;	//3
	AN_TLM subcam_current;	//4
	AN_TLM sf_current;		//5
	AN_TLM msn_unreg_current;	//6
	AN_TLM strx_current;	//7
	AN_TLM heater_current;	//8
	AN_TLM rw_voltage;		//9
	AN_TLM adcs_voltage;	//10
	AN_TLM msn5v_voltage;	//11
	AN_TLM subcam_voltage;	//12
	AN_TLM sf_voltage;		//13
	AN_TLM msn_unreg_voltage;	//14
	AN_TLM strx_voltage;	//15
	AN_TLM heater_voltage;	//16
}PDU_AnalogTLM;

extern PDU_AnalogTLM pdu_analog_tlm;

ack		PDU_init(void);
void	PDU_getAnalogTlm_init(void);
void	PDU_Task(void);

ack		PDU_ChangeMUXport(unsigned char port_name);
float	PDU_getMilliVoltage(char mux_port);
void	PDU_ConvertAnalogValue(char mux_port, AN_TLM *analog_tlm);
ack		PDU_getAnalogTlm(void);

ack		PDU_IO_Write(unsigned char port, unsigned char sts);
int		PDU_IO_Read(unsigned char port);

void PDU_DebugMessage(void);

#endif