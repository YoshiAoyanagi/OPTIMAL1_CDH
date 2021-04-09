// 
// 
// 

#include "dWire0.h"

DTWI0 dtwi0;

void Wire0_Init(void)
{
	dtwi0.beginMaster();
	delay(100);

}
unsigned char Wire0_available(void)
{
	return dtwi0.available();
}
bool Wire0_beginTransmission(unsigned char address)
{
	unsigned int timeout;
	timeout = 0;
	while (!dtwi0.startMasterWrite(address)) { timeout++; if (timeout > DTWI_TIMEOUT) { return (false); } };
	return (true);
}

bool Wire0_endTransmission(void)
{
	unsigned int timeout;
	timeout = 0;
	while (!dtwi0.stopMaster()) { timeout++; if (timeout > DTWI_TIMEOUT) { return (false); } };
	return (true);
}
int Wire0_write(unsigned char data)
{
	dtwi0.write((const byte *)&data, 1);
}
unsigned char Wire0_read(void)
{
	byte    data;

	if (dtwi0.read(&data, 1) == 1)
	{
		return((unsigned char)data);
	}

	return('\0');
}

unsigned char Wire0_requestFrom(unsigned char address, unsigned char quantity)
{
	DTWI::I2C_STATUS status;
	unsigned int timeout;
	timeout = 0;
	while (!dtwi0.startMasterRead(address, quantity)) { timeout++; if (timeout > DTWI_TIMEOUT) { break; } };

	do
	{
		status = dtwi0.getStatus();
	} while (status.fMyBus && !status.fNacking);

	timeout = 0;
	while (!dtwi0.stopMaster()) { timeout++; if (timeout > DTWI_TIMEOUT) { break; } };

	return(dtwi0.available());
}
