// 
// 
// 

#include "PCU.h"

MCP3421 pcu_adc;
//MCP4716 pcu_dac_mppt;
//MCP4716 pcu_dac_batt;
MCP23017 pcu_ioe;

PCU_AnalogTLM pcu_analog_tlm;

ack PCU_init(void)
{
	PCU_getAnalogTlm_init();
	
	if ((pcu_adc.init(PCU_ADC_ADDRESS, PCU_ADC_SAMPLING_12BIT, PCU_ADC_PGA_GAIN_x1)) != 1)
	{
		Serial.println("pcu_adc.init: error");
		return Error;
	}

	if ((pcu_ioe.init(PCU_IOE_ADDRESS)) != 1)
	{
		Serial.println("pcu_ioe.init: error");
		return Error;
	}
		
	//if ((pcu_dac_mppt.init(PCU_DAC_MPPT_ADDRESS, MCP47X6_VREF_VDD, MCP47X6_GAIN_1X)) != 1)
		//return Error;

	//if ((pcu_dac_batt.init(PCU_DAC_BATT_ADDRESS, MCP47X6_VREF_VDD, MCP47X6_GAIN_1X)) != 1)
	//	return Error;
	//

	pcu_ioe.pinMode(PCU_IOE_MUX_SEL0, OUTPUT);
	pcu_ioe.digitalWrite(PCU_IOE_MUX_SEL0, LOW);
	
	pcu_ioe.pinMode(PCU_IOE_MUX_SEL1, OUTPUT);
	pcu_ioe.digitalWrite(PCU_IOE_MUX_SEL1, LOW);

	pcu_ioe.pinMode(PCU_IOE_MUX_SEL2, OUTPUT);
	pcu_ioe.digitalWrite(PCU_IOE_MUX_SEL2, LOW);

	pcu_ioe.pinMode(PCU_IOE_MUX_SEL3, OUTPUT);
	pcu_ioe.digitalWrite(PCU_IOE_MUX_SEL3, LOW);

	pcu_ioe.pinMode(PCU_IOE_EXIO_ov90, OUTPUT);
	pcu_ioe.digitalWrite(PCU_IOE_EXIO_ov90, LOW);

	pcu_ioe.pinMode(PCU_IOE_MPPT1_START, INPUT);
	
	return Success;
}

void PCU_getAnalogTlm_init(void)
{
	//coeff
	pcu_analog_tlm.sap1_current.conversion_coeff	= PCU_FL_COEFF_SAP_I_1;
	pcu_analog_tlm.sap2_current.conversion_coeff	= PCU_FL_COEFF_SAP_I_2;
	pcu_analog_tlm.sap3_current.conversion_coeff	= PCU_FL_COEFF_SAP_I_3;
	pcu_analog_tlm.sap4_current.conversion_coeff	= PCU_FL_COEFF_SAP_I_4;

	pcu_analog_tlm.sap_v1.conversion_coeff			= PCU_FL_COEFF_SAP_V_1;
	pcu_analog_tlm.sap_v2.conversion_coeff			= PCU_FL_COEFF_SAP_V_2;

	pcu_analog_tlm.batt1_v.conversion_coeff			= PCU_FL_COEFF_BATT_V;
	pcu_analog_tlm.batt1_current.conversion_coeff	= PCU_FL_COEFF_BATT_I;
	pcu_analog_tlm.batt1_temp.conversion_coeff		= PCU_FL_COEFF_BATT_TEMP;
	pcu_analog_tlm.dcdc1_current.conversion_coeff	= PCU_FL_COEFF_DCDC_I;

	pcu_analog_tlm.batt2_v.conversion_coeff			= PCU_FL_COEFF_BATT_V;
	pcu_analog_tlm.batt2_current.conversion_coeff	= PCU_FL_COEFF_BATT_I;
	pcu_analog_tlm.batt2_temp.conversion_coeff		= PCU_FL_COEFF_BATT_TEMP;
	pcu_analog_tlm.dcdc2_current.conversion_coeff	= PCU_FL_COEFF_DCDC_I;

	pcu_analog_tlm.com_current.conversion_coeff		= PCU_FL_COEFF_COM_I;
	pcu_analog_tlm.mppt1_start.conversion_coeff		= PCU_FL_COEFF_MPPT;

	//offset
	pcu_analog_tlm.sap1_current.conversion_offset	= PCU_FL_OFFSET_SAP_I;
	pcu_analog_tlm.sap2_current.conversion_offset	= PCU_FL_OFFSET_SAP_I;
	pcu_analog_tlm.sap3_current.conversion_offset	= PCU_FL_OFFSET_SAP_I;
	pcu_analog_tlm.sap4_current.conversion_offset	= PCU_FL_OFFSET_SAP_I;

	pcu_analog_tlm.sap_v1.conversion_offset			= PCU_FL_OFFSET_SAP_V;
	pcu_analog_tlm.sap_v2.conversion_offset			= PCU_FL_OFFSET_SAP_V;

	pcu_analog_tlm.batt1_v.conversion_offset		= PCU_FL_OFFSET_BATT_V;
	pcu_analog_tlm.batt1_current.conversion_offset	= PCU_FL_OFFSET_BATT_I;
	pcu_analog_tlm.batt1_temp.conversion_offset		= PCU_FL_OFFSET_BATT_TEMP;
	pcu_analog_tlm.dcdc1_current.conversion_offset	= PCU_FL_OFFSET_DCDC_I;

	pcu_analog_tlm.batt2_v.conversion_offset		= PCU_FL_OFFSET_BATT_V;
	pcu_analog_tlm.batt2_current.conversion_offset	= PCU_FL_OFFSET_BATT_I;
	pcu_analog_tlm.batt2_temp.conversion_offset		= PCU_FL_OFFSET_BATT_TEMP;
	pcu_analog_tlm.dcdc2_current.conversion_offset	= PCU_FL_OFFSET_DCDC_I;

	pcu_analog_tlm.com_current.conversion_offset		= PCU_FL_OFFSET_COM_I;
	pcu_analog_tlm.mppt1_start.conversion_offset		= PCU_FL_OFFSET_MPPT;

	//ToByte(coeff)
	pcu_analog_tlm.sap1_current.conversion_coeff_to_byte = PCU_FL_COEFF_I_TO_BYTE;
	pcu_analog_tlm.sap2_current.conversion_coeff_to_byte = PCU_FL_COEFF_I_TO_BYTE;
	pcu_analog_tlm.sap3_current.conversion_coeff_to_byte = PCU_FL_COEFF_I_TO_BYTE;
	pcu_analog_tlm.sap4_current.conversion_coeff_to_byte = PCU_FL_COEFF_I_TO_BYTE;

	pcu_analog_tlm.sap_v1.conversion_coeff_to_byte		 = PCU_FL_COEFF_V_TO_BYTE;
	pcu_analog_tlm.sap_v2.conversion_coeff_to_byte		 = PCU_FL_COEFF_V_TO_BYTE;

	pcu_analog_tlm.batt1_v.conversion_coeff_to_byte		 = PCU_FL_COEFF_V_TO_BYTE;
	pcu_analog_tlm.batt1_current.conversion_coeff_to_byte= PCU_FL_COEFF_BATT_I_TO_BYTE;
	pcu_analog_tlm.batt1_temp.conversion_coeff_to_byte	 = PCU_FL_COEFF_TEMP_TO_BYTE;;
	pcu_analog_tlm.dcdc1_current.conversion_coeff_to_byte= PCU_FL_COEFF_I_TO_BYTE;

	pcu_analog_tlm.batt2_v.conversion_coeff_to_byte		 = PCU_FL_COEFF_V_TO_BYTE;
	pcu_analog_tlm.batt2_current.conversion_coeff_to_byte= PCU_FL_COEFF_BATT_I_TO_BYTE;
	pcu_analog_tlm.batt2_temp.conversion_coeff_to_byte	 = PCU_FL_COEFF_TEMP_TO_BYTE;;
	pcu_analog_tlm.dcdc2_current.conversion_coeff_to_byte= PCU_FL_COEFF_I_TO_BYTE;

	pcu_analog_tlm.com_current.conversion_coeff_to_byte	 = PCU_FL_COEFF_I_TO_BYTE;
	pcu_analog_tlm.mppt1_start.conversion_coeff_to_byte  = 1;

	//ToByte offset
	pcu_analog_tlm.sap1_current.conversion_offset_to_byte = PCU_FL_OFFSET_I_TO_BYTE;
	pcu_analog_tlm.sap2_current.conversion_offset_to_byte = PCU_FL_OFFSET_I_TO_BYTE;
	pcu_analog_tlm.sap3_current.conversion_offset_to_byte = PCU_FL_OFFSET_I_TO_BYTE;
	pcu_analog_tlm.sap4_current.conversion_offset_to_byte = PCU_FL_OFFSET_I_TO_BYTE;

	pcu_analog_tlm.sap_v1.conversion_offset_to_byte		= PCU_FL_OFFSET_V_TO_BYTE;
	pcu_analog_tlm.sap_v2.conversion_offset_to_byte		= PCU_FL_OFFSET_V_TO_BYTE;

	pcu_analog_tlm.batt1_v.conversion_offset_to_byte		= PCU_FL_OFFSET_V_TO_BYTE;
	pcu_analog_tlm.batt1_current.conversion_offset_to_byte	= PCU_FL_OFFSET_BATT_I_TO_BYTE;
	pcu_analog_tlm.batt1_temp.conversion_offset_to_byte		= PCU_FL_OFFSET_TEMP_TO_BYTE;;
	pcu_analog_tlm.dcdc1_current.conversion_offset_to_byte	= PCU_FL_OFFSET_I_TO_BYTE;

	pcu_analog_tlm.batt2_v.conversion_offset_to_byte		= PCU_FL_OFFSET_V_TO_BYTE;
	pcu_analog_tlm.batt2_current.conversion_offset_to_byte	= PCU_FL_OFFSET_BATT_I_TO_BYTE;
	pcu_analog_tlm.batt2_temp.conversion_offset_to_byte		= PCU_FL_OFFSET_TEMP_TO_BYTE;;
	pcu_analog_tlm.dcdc2_current.conversion_offset_to_byte	= PCU_FL_OFFSET_I_TO_BYTE;

	pcu_analog_tlm.com_current.conversion_offset_to_byte	= PCU_FL_OFFSET_I_TO_BYTE;
	pcu_analog_tlm.mppt1_start.conversion_offset_to_byte	= 1;
	return;
}
void PCU_Task(void)
{
	PCU_getAnalogTlm();
	
	return;
}

ack PCU_IO_Write(unsigned char port, unsigned char sts)
{
	pcu_ioe.digitalWrite(port, sts);
	if (sts == pcu_ioe.digitalRead(port))
		return Success;

	return Error;
}
int PCU_IO_Read(unsigned char port)
{
	return (pcu_ioe.digitalRead(port));
}

ack PCU_ChangeMUXport(char ch)
{
	ch = ch & 0x0F;

	pcu_ioe.digitalWrite(PCU_IOE_MUX_SEL0, ch & 0x01);
	ch = ch >> 1;
	pcu_ioe.digitalWrite(PCU_IOE_MUX_SEL1, ch & 0x01);
	ch = ch >> 1;
	pcu_ioe.digitalWrite(PCU_IOE_MUX_SEL2, ch & 0x01);
	ch = ch >> 1;
	pcu_ioe.digitalWrite(PCU_IOE_MUX_SEL3, ch & 0x01);

	return Success;
}

void PCU_ConvertAnalogValue(char mux_port, AN_TLM *analog_tlm)
{
	float millivoltage;
	float float_value;
	unsigned char byte_value;

	millivoltage = PCU_getMilliVoltage(mux_port);

	millivoltage = millivoltage;
	float_value = analog_tlm->conversion_coeff * millivoltage + analog_tlm->conversion_offset;


	byte_value = (analog_tlm->conversion_coeff_to_byte * float_value) + analog_tlm->conversion_offset_to_byte;

	analog_tlm->float_value = float_value;
	analog_tlm->millivoltage = millivoltage;
	analog_tlm->byte_value = byte_value;

	return;
}

//add
float PCU_getMilliVoltage(char mux_port)
{
	float val;
	val = pcu_adc.getMilliVoltage();
	return val;
}

ack PCU_getAnalogTlm(void)
{
	static unsigned int cycle;

	if (cycle == 0)
	{
		PCU_ChangeMUXport(PCU_MUX_PORT_SAP4_I);

	}
	else if (cycle == 1)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_SAP4_I, &pcu_analog_tlm.sap4_current);
		PCU_ChangeMUXport(PCU_MUX_PORT_SAP_V1);
	}
	else if (cycle == 2)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_SAP_V1, &pcu_analog_tlm.sap_v1);
		PCU_ChangeMUXport(PCU_MUX_PORT_SAP_V2);
	}
	else if (cycle == 3)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_SAP_V2, &pcu_analog_tlm.sap_v2);
		PCU_ChangeMUXport(PCU_MUX_PORT_BATT1_V);
	}
	else if (cycle == 4)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_BATT1_V, &pcu_analog_tlm.batt1_v);
		PCU_ChangeMUXport(PCU_MUX_PORT_BATT1_I);
	}
	else if (cycle == 5)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_BATT1_I, &pcu_analog_tlm.batt1_current);
		PCU_ChangeMUXport(PCU_MUX_PORT_DCDC1_I);
	}
	else if (cycle == 6)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_DCDC1_I, &pcu_analog_tlm.dcdc1_current);
		PCU_ChangeMUXport(PCU_MUX_PORT_SAP3_I);
	}
	else if (cycle == 7)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_SAP3_I, &pcu_analog_tlm.sap3_current);
		PCU_ChangeMUXport(PCU_MUX_PORT_SAP1_I);
	}
	else if (cycle == 8)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_SAP1_I, &pcu_analog_tlm.sap1_current);
		PCU_ChangeMUXport(PCU_MUX_PORT_SAP2_I);
	}
	else if (cycle == 9)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_SAP2_I, &pcu_analog_tlm.sap2_current);
		PCU_ChangeMUXport(PCU_MUX_PORT_BATT2_V);
	}
	else if (cycle == 10)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_BATT2_V, &pcu_analog_tlm.batt2_v);
		PCU_ChangeMUXport(PCU_MUX_PORT_BATT2_I);
	}
	else if (cycle == 11)
	{
		PCU_ConvertAnalogValue(PCU_MUX_PORT_BATT2_I, &pcu_analog_tlm.batt2_current);
		PCU_ChangeMUXport(PCU_MUX_PORT_SAP4_I);
		cycle = 0;
	}

	cycle++;

	return Success;
}



void PCU_DebugMessage(void)
{
	Serial.print("SAP_I [mA]: ");
	Serial.print(pcu_analog_tlm.sap1_current.float_value, 3); Serial.print(", ");
	Serial.print(pcu_analog_tlm.sap2_current.float_value, 3); Serial.print(", ");
	Serial.print(pcu_analog_tlm.sap3_current.float_value, 3); Serial.print(", ");
	Serial.print(pcu_analog_tlm.sap4_current.float_value, 3); Serial.println("");

	Serial.print("SAP_V [V]: ");
	Serial.print(pcu_analog_tlm.sap_v1.float_value, 2); Serial.print(", ");
	Serial.print(pcu_analog_tlm.sap_v2.float_value, 2); Serial.println("");

	Serial.print("BATT_V [V]: ");
	Serial.print(pcu_analog_tlm.batt1_v.float_value, 2); Serial.print(", ");
	Serial.print(pcu_analog_tlm.batt2_v.float_value, 2); Serial.print(", ");

	Serial.print("BATT_I [mA]: ");
	Serial.print(pcu_analog_tlm.batt1_current.float_value, 2); Serial.print(", ");
	Serial.print(pcu_analog_tlm.batt2_current.float_value, 2); Serial.println("");
	/*
	Serial.print("DCDC_I [mA]: ");
	Serial.print(pcu_analog_tlm.dcdc1_current.float_value, 2); Serial.print(", ");
	Serial.print(pcu_analog_tlm.dcdc2_current.float_value, 2); Serial.print(", ");

	Serial.print("COM_I [mA]: ");
	Serial.print(pcu_analog_tlm.com_current.float_value, 2); Serial.println("");
	*/

	return;
}