// 
// 
// 

#include "PDU.h"

MCP3421		pdu_adc;
MCP23017	pdu_ioe;

PDU_AnalogTLM pdu_analog_tlm;

ack PDU_init(void)
{
	PDU_getAnalogTlm_init();

	if ((pdu_adc.init(PDU_ADC_ADDRESS, PDU_ADC_SAMPLING_12BIT, PDU_ADC_PGA_GAIN_x1)) != 1)
	{
		return Error;
	}

	if ((pdu_ioe.init(PDU_IOE_ADDRESS)) != 1)
	{
		return Error;
	}

	int i;
	for (i = 0; i < 16; i++)
	{
		pdu_ioe.pinMode(i, OUTPUT);
		pdu_ioe.digitalWrite(i, LOW);
	}

	return Success;
}

void PDU_getAnalogTlm_init(void)
{
	pdu_analog_tlm.rw_current.conversion_coeff	= PDU_FL_COEFF_RW_I;
	pdu_analog_tlm.rw_voltage.conversion_coeff	= PDU_FL_COEFF_RW_V;

	pdu_analog_tlm.adcs_current.conversion_coeff	= PDU_FL_COEFF_ADCS_I;
	pdu_analog_tlm.adcs_voltage.conversion_coeff	= PDU_FL_COEFF_ADCS_V;

	pdu_analog_tlm.msn5v_current.conversion_coeff	= PDU_FL_COEFF_MSN5V_I;
	pdu_analog_tlm.msn5v_voltage.conversion_coeff	= PDU_FL_COEFF_MSN5V_V;
	
	pdu_analog_tlm.subcam_current.conversion_coeff	= PDU_FL_COEFF_SUBCAM_I;
	pdu_analog_tlm.subcam_voltage.conversion_coeff	= PDU_FL_COEFF_SUBCAM_V;

	pdu_analog_tlm.sf_current.conversion_coeff		= PDU_FL_COEFF_SF_I;
	pdu_analog_tlm.sf_voltage.conversion_coeff		= PDU_FL_COEFF_SF_V;

	pdu_analog_tlm.msn_unreg_current.conversion_coeff	= PDU_FL_COEFF_UNREG_I;
	pdu_analog_tlm.msn_unreg_voltage.conversion_coeff	= PDU_FL_COEFF_UNREG_V;

	pdu_analog_tlm.strx_current.conversion_coeff	= PDU_FL_COEFF_STRX_I;
	pdu_analog_tlm.strx_voltage.conversion_coeff	= PDU_FL_COEFF_STRX_V;

	pdu_analog_tlm.heater_current.conversion_coeff	= PDU_FL_COEFF_HEATER_I;
	pdu_analog_tlm.heater_voltage.conversion_coeff	= PDU_FL_COEFF_HEATER_V;

	pdu_analog_tlm.rw_current.conversion_coeff_to_byte = PDU_FL_COEFF_I_TO_BYTE;
	pdu_analog_tlm.rw_voltage.conversion_coeff_to_byte = PDU_FL_COEFF_V_TO_BYTE;

	pdu_analog_tlm.adcs_current.conversion_coeff_to_byte = PDU_FL_COEFF_I_TO_BYTE;;
	pdu_analog_tlm.adcs_voltage.conversion_coeff_to_byte = PDU_FL_COEFF_V_TO_BYTE;

	pdu_analog_tlm.msn5v_current.conversion_coeff_to_byte = PDU_FL_COEFF_I_TO_BYTE;
	pdu_analog_tlm.msn5v_voltage.conversion_coeff_to_byte = PDU_FL_COEFF_V_TO_BYTE;

	pdu_analog_tlm.subcam_current.conversion_coeff_to_byte = PDU_FL_COEFF_I_TO_BYTE;
	pdu_analog_tlm.subcam_voltage.conversion_coeff_to_byte = PDU_FL_COEFF_V_TO_BYTE;

	pdu_analog_tlm.sf_current.conversion_coeff_to_byte = PDU_FL_COEFF_I_TO_BYTE;
	pdu_analog_tlm.sf_voltage.conversion_coeff_to_byte = PDU_FL_COEFF_V_TO_BYTE;

	pdu_analog_tlm.msn_unreg_current.conversion_coeff_to_byte = PDU_FL_COEFF_I_TO_BYTE;
	pdu_analog_tlm.msn_unreg_voltage.conversion_coeff_to_byte = PDU_FL_COEFF_V_TO_BYTE;

	pdu_analog_tlm.strx_current.conversion_coeff_to_byte = PDU_FL_COEFF_I_TO_BYTE;
	pdu_analog_tlm.strx_voltage.conversion_coeff_to_byte = PDU_FL_COEFF_V_TO_BYTE;

	pdu_analog_tlm.heater_current.conversion_coeff_to_byte = PDU_FL_COEFF_I_TO_BYTE;
	pdu_analog_tlm.heater_voltage.conversion_coeff_to_byte = PDU_FL_COEFF_V_TO_BYTE;


	return;
}

void PDU_Task(void)
{
	PDU_getAnalogTlm();
	return;
}

ack PDU_IO_Write(unsigned char port, unsigned char sts)
{
	pdu_ioe.digitalWrite(port, sts);
	if (sts == pdu_ioe.digitalRead(port))
		return Success;

	return Error;
}
int PDU_IO_Read(unsigned char port)
{
	return (pdu_ioe.digitalRead(port));
}

ack PDU_ChangeMUXport(unsigned char ch)
{
	ch = ch & 0x0F;

	pdu_ioe.digitalWrite(PDU_IOE_MUX_SEL0, ch & 0x01);
	ch = ch >> 1;
	pdu_ioe.digitalWrite(PDU_IOE_MUX_SEL1, ch & 0x01);
	ch = ch >> 1;
	pdu_ioe.digitalWrite(PDU_IOE_MUX_SEL2, ch & 0x01);
	ch = ch >> 1;
	pdu_ioe.digitalWrite(PDU_IOE_MUX_SEL3, ch & 0x01);

	return Success;
}

void PDU_ConvertAnalogValue(char mux_port, AN_TLM *analog_tlm)
{
	float millivoltage;
	float float_value;
	unsigned char byte_value;
	float	tmp;

	millivoltage = PDU_getMilliVoltage(mux_port);

	millivoltage = millivoltage;
	float_value = analog_tlm->conversion_coeff * millivoltage;
	tmp = (float)(analog_tlm->conversion_coeff_to_byte * float_value / 1000);
	byte_value = (unsigned char)(tmp * 1000);

	analog_tlm->float_value		= float_value;
	analog_tlm->millivoltage	= millivoltage;
	analog_tlm->byte_value		= byte_value;

	return;
}

float PDU_getMilliVoltage(char mux_port)
{
	float val;
	val = pdu_adc.getMilliVoltage();
	return val;
}


ack PDU_getAnalogTlm(void)
{
	static unsigned int cycle;

	if (cycle == 0)
	{
		PDU_ChangeMUXport(PDU_MUX_PORT_CURRENT_MON2);

	}
	else if (cycle == 1)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_CURRENT_MON2, &pdu_analog_tlm.adcs_current);
		PDU_ChangeMUXport(PDU_MUX_PORT_VOLTAGE_MON2);
	}
	else if (cycle == 2)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_VOLTAGE_MON2, &pdu_analog_tlm.adcs_voltage);
		PDU_ChangeMUXport(PDU_MUX_PORT_CURRENT_MON3);
	}
	else if (cycle == 3)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_CURRENT_MON3, &pdu_analog_tlm.msn5v_current);
		PDU_ChangeMUXport(PDU_MUX_PORT_VOLTAGE_MON3);
	}
	else if (cycle == 4)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_VOLTAGE_MON3, &pdu_analog_tlm.msn5v_voltage);
		PDU_ChangeMUXport(PDU_MUX_PORT_CURRENT_MON4);
	}
	else if (cycle == 5)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_CURRENT_MON4, &pdu_analog_tlm.subcam_current);
		PDU_ChangeMUXport(PDU_MUX_PORT_VOLTAGE_MON4);
	}
	else if (cycle == 6)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_VOLTAGE_MON4, &pdu_analog_tlm.subcam_voltage);
		PDU_ChangeMUXport(PDU_MUX_PORT_CURRENT_MON5);
	}
	else if (cycle == 7)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_CURRENT_MON5, &pdu_analog_tlm.sf_current);
		PDU_ChangeMUXport(PDU_MUX_PORT_VOLTAGE_MON5);
	}
	else if (cycle == 8)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_VOLTAGE_MON5, &pdu_analog_tlm.sf_voltage);
		PDU_ChangeMUXport(PDU_MUX_PORT_CURRENT_MON6);
	}
	else if (cycle == 9)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_CURRENT_MON6, &pdu_analog_tlm.msn_unreg_current);
		PDU_ChangeMUXport(PDU_MUX_PORT_VOLTAGE_MON6);
	}
	else if (cycle == 10)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_VOLTAGE_MON6, &pdu_analog_tlm.msn_unreg_voltage);
		PDU_ChangeMUXport(PDU_MUX_PORT_CURRENT_MON7);
	}
	else if (cycle == 11)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_CURRENT_MON7, &pdu_analog_tlm.rw_current);
		PDU_ChangeMUXport(PDU_MUX_PORT_VOLTAGE_MON7);
	}
	else if (cycle == 12)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_VOLTAGE_MON7, &pdu_analog_tlm.rw_voltage);
		PDU_ChangeMUXport(PDU_MUX_PORT_CURRENT_MON8);
	}
	else if (cycle == 13)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_CURRENT_MON8, &pdu_analog_tlm.strx_current);
		PDU_ChangeMUXport(PDU_MUX_PORT_VOLTAGE_MON8);
	}
	else if (cycle == 14)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_VOLTAGE_MON8, &pdu_analog_tlm.strx_voltage);
		PDU_ChangeMUXport(PDU_MUX_PORT_CURRENT_MON1);
	}
	else if (cycle == 15)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_CURRENT_MON1, &pdu_analog_tlm.heater_current);
		PDU_ChangeMUXport(PDU_MUX_PORT_VOLTAGE_MON1);
	}
	else if (cycle == 16)
	{
		PDU_ConvertAnalogValue(PDU_MUX_PORT_VOLTAGE_MON1, &pdu_analog_tlm.heater_voltage);
		PDU_ChangeMUXport(PDU_MUX_PORT_CURRENT_MON2);
		cycle = 0;
	}
	cycle++;

	return Success;
}


void PDU_DebugMessage(void)
{

	Serial.print("STRx_I,V: ");
	Serial.print(pdu_analog_tlm.strx_current.float_value, 0); Serial.print(", ");
	Serial.print(pdu_analog_tlm.strx_voltage.float_value, 1); Serial.print(", ");

	Serial.print("HEATER_I,V: ");
	Serial.print(pdu_analog_tlm.heater_current.float_value); Serial.print(", ");
	Serial.print(pdu_analog_tlm.heater_voltage.float_value, 1); Serial.print(", ");

	Serial.print("RW_I,V: ");
	Serial.print(pdu_analog_tlm.rw_current.float_value, 0); Serial.print(", ");
	Serial.print(pdu_analog_tlm.rw_voltage.float_value, 1); Serial.print(", ");

	Serial.print("ADCS_I,V: ");
	Serial.print(pdu_analog_tlm.adcs_current.float_value, 0); Serial.print(", ");
	Serial.print(pdu_analog_tlm.adcs_voltage.float_value, 1); Serial.print(", ");

	Serial.println("");

	Serial.print("MSN5V_I,V: ");
	Serial.print(pdu_analog_tlm.msn5v_current.float_value, 0); Serial.print(", ");
	Serial.print(pdu_analog_tlm.msn5v_voltage.float_value, 1); Serial.print(", ");
	Serial.println("");

	Serial.print("SubCAM_I,V: ");
	Serial.print(pdu_analog_tlm.subcam_current.float_value, 0); Serial.print(", ");
	Serial.print(pdu_analog_tlm.subcam_voltage.float_value, 1); Serial.print(", ");

	Serial.print("S&F_I,V: ");
	Serial.print(pdu_analog_tlm.sf_current.float_value, 0); Serial.print(", ");
	Serial.print(pdu_analog_tlm.sf_voltage.float_value, 1); Serial.print(", ");

	Serial.print("MSN_UNREG_I,V: ");
	Serial.print(pdu_analog_tlm.msn_unreg_current.float_value, 0); Serial.print(", ");
	Serial.print(pdu_analog_tlm.msn_unreg_voltage.float_value, 1); Serial.print(", ");

	Serial.println("");

	return;
}