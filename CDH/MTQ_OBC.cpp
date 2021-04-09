// 
// 
// 

#include "MTQ_OBC.h"
#include "Utility.h"

MTQ_STS	mtq_sts;
MCP23008 mtq_ioe;

// MTQ
signed char MTQ_c2b_X = +1;
signed char MTQ_c2b_Y = +1;
signed char MTQ_c2b_Z = +1;

int MTQ_IOE_init(void)
{
	int i_ret = mtq_ioe.begin();

	int i;
	for (i = 0; i < 8; i++)
	{
		mtq_ioe.pinMode(i, OUTPUT);
	}

	mtq_sts.x = 0;
	mtq_sts.y = 0;
	mtq_sts.z = 0;
	MTQ_OutputUpdate();

	return i_ret;
}

void MTQ_IOE_PWR(int pwr_onoff)
{
	if(pwr_onoff == PWR_ON)
		mtq_ioe.digitalWrite(MTQ_IOE_PORT_PWR, HIGH);
	else if (pwr_onoff == PWR_OFF)
		mtq_ioe.digitalWrite(MTQ_IOE_PORT_PWR, LOW);

	return;
}

void MTQ_OutputUpdate(void)
{
	MTQ_PORT_Set(MTQ_IOE_PORT_PY, MTQ_IOE_PORT_MY, MTQ_c2b_X, mtq_sts.x);
	MTQ_PORT_Set(MTQ_IOE_PORT_PX, MTQ_IOE_PORT_MX, MTQ_c2b_Y, mtq_sts.y);
	MTQ_PORT_Set(MTQ_IOE_PORT_PZ, MTQ_IOE_PORT_MZ, MTQ_c2b_Z, mtq_sts.z);

	mtq_sts.x = MTQ_PORT_READ(MTQ_IOE_PORT_PY, MTQ_IOE_PORT_MY, MTQ_c2b_X);
	mtq_sts.y = MTQ_PORT_READ(MTQ_IOE_PORT_PX, MTQ_IOE_PORT_MX, MTQ_c2b_Y);
	mtq_sts.z = MTQ_PORT_READ(MTQ_IOE_PORT_PZ, MTQ_IOE_PORT_MZ, MTQ_c2b_Z);

	return;
}


signed char MTQ_PORT_READ(int port_p, int port_m, signed char Sign)
{
	signed char sts = 0;
	int p = 0;
	int m = 0;

	p = mtq_ioe.digitalRead(port_p);
	m = mtq_ioe.digitalRead(port_m);
	if (p == HIGH && m == LOW)
	{
		if (Sign == 1)
			sts = MTQ_P;
		else
			sts = MTQ_M;
	}
	else if (p == LOW && m == HIGH)
	{
		if (Sign == -1)
			sts = MTQ_P;
		else
			sts = MTQ_M;
	}
	else
		sts = MTQ_NONE;

	return sts;
}

void MTQ_PORT_Set(int port_p, int port_m, signed char Sign, char out)
{
	signed char tmp = out;
	char mtq_out;

	if (out == 2)
		tmp = -1;

	if (Sign * tmp == 1)
		mtq_out = MTQ_P;
	else if (Sign * tmp == -1)
		mtq_out = MTQ_M;
	else
		mtq_out = MTQ_STOP;

	if (mtq_out == MTQ_P)
	{
		mtq_ioe.digitalWrite(port_m, LOW);
		mtq_ioe.digitalWrite(port_p, HIGH);
	}
	else if (mtq_out == MTQ_M)
	{
		mtq_ioe.digitalWrite(port_p, LOW);
		mtq_ioe.digitalWrite(port_m, HIGH);
	}
	else
	{
		mtq_ioe.digitalWrite(port_m, LOW);
		mtq_ioe.digitalWrite(port_p, LOW);
	}
	return;
}

