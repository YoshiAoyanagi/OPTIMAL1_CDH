// PCU.h

#ifndef _PCU_h
#define _PCU_h

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
#include "port_config.h"
#include "EPS_config.h"

typedef struct
{
	AN_TLM sap1_current;
	AN_TLM sap2_current;
	AN_TLM sap3_current;
	AN_TLM sap4_current;
	AN_TLM sap_v1;
	AN_TLM sap_v2;
	AN_TLM batt1_v;
	AN_TLM batt1_current;
	AN_TLM batt1_temp;
	AN_TLM dcdc1_current;
	AN_TLM batt2_v;
	AN_TLM batt2_current;
	AN_TLM batt2_temp;
	AN_TLM dcdc2_current;
	AN_TLM com_current;
	AN_TLM mppt1_start;
}PCU_AnalogTLM;

#define PCU_MUX_PORT_SAP1_I		0x00
#define PCU_MUX_PORT_SAP2_I		0x01
#define PCU_MUX_PORT_SAP3_I		0x02
#define PCU_MUX_PORT_SAP4_I		0x03
#define PCU_MUX_PORT_SAP_V1		0x04
#define PCU_MUX_PORT_SAP_V2		0x05
#define PCU_MUX_PORT_BATT1_V	0x06
#define PCU_MUX_PORT_BATT1_I	0x07
#define PCU_MUX_PORT_BATT1_TEMP	0x08
#define PCU_MUX_PORT_DCDC1_I	0x09
#define PCU_MUX_PORT_BATT2_V	0x0A
#define PCU_MUX_PORT_BATT2_I	0x0B
#define PCU_MUX_PORT_BATT2_TEMP	0x0C
#define PCU_MUX_PORT_DCDC2_I	0x0D
#define PCU_MUX_PORT_COM_I		0x0E
#define PCU_MUX_PORT_MPPT1		0x0F

//PCU
#define PCU_IOE_MUX_SEL0	0
#define PCU_IOE_MUX_SEL1	1
#define PCU_IOE_MUX_SEL2	2
#define PCU_IOE_MUX_SEL3	3
#define PCU_IOE_EXIO_ov90	4
#define PCU_IOE_MPPT1_START	5







//OPT1 param(2021/01/26 update)

#define PCU_FL_COEFF_SAP_I_1		2.0202 * 1000
#define PCU_FL_COEFF_SAP_I_2		1.9802 * 1000
#define PCU_FL_COEFF_SAP_I_3		1.9802 * 1000
#define PCU_FL_COEFF_SAP_I_4		1.9802 * 1000

#define PCU_FL_COEFF_SAP_V_1		10.2715
#define PCU_FL_COEFF_SAP_V_2		10.2489

#define PCU_FL_COEFF_BATT_I		5.00 * 1000
#define PCU_FL_COEFF_BATT_V		4.3431 
#define PCU_FL_COEFF_BATT_TEMP	1
#define PCU_FL_COEFF_DCDC_I		1.7921 * 1000
#define PCU_FL_COEFF_COM_I		0.1979 * 1000
#define PCU_FL_COEFF_MPPT		1


#define PCU_FL_OFFSET_SAP_I		0
#define PCU_FL_OFFSET_SAP_V		0
#define PCU_FL_OFFSET_BATT_I	-6200
#define PCU_FL_OFFSET_BATT_V	0
#define PCU_FL_OFFSET_BATT_TEMP	0
#define PCU_FL_OFFSET_DCDC_I	0
#define PCU_FL_OFFSET_COM_I		0
#define PCU_FL_OFFSET_MPPT		0

#define PCU_FL_COEFF_V_TO_BYTE			(256 / 16)	//max.16V
#define PCU_FL_COEFF_I_TO_BYTE			0.085		//max. 3A
#define PCU_FL_COEFF_BATT_I_TO_BYTE		0.0425		//max. -3A ~ +3A
#define PCU_FL_COEFF_TEMP_TO_BYTE		(1)

#define PCU_FL_OFFSET_V_TO_BYTE				0
#define PCU_FL_OFFSET_I_TO_BYTE				0
#define PCU_FL_OFFSET_BATT_I_TO_BYTE		127
#define PCU_FL_OFFSET_TEMP_TO_BYTE			0

extern PCU_AnalogTLM pcu_analog_tlm;

ack PCU_init(void);
void PCU_getAnalogTlm_init(void);
void PCU_Task(void);

ack		PCU_ChangeMUXport(char port_name);
ack		PCU_getAnalogTlm(void);
float	PCU_getMilliVoltage(char mux_port);
void	PCU_ConvertAnalogValue(char mux_port, AN_TLM *analog_tlm);

ack PCU_IO_Write(unsigned char port, unsigned char sts);
int PCU_IO_Read(unsigned char port);

void PCU_DebugMessage(void);

void PCU_change_I2C_port(void);

#endif

