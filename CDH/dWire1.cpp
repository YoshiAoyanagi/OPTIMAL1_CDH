// 
// 
// 

#include "dWire1.h"

DTWI1 dtwi1;

void Wire1_Init(void)
{
	dtwi1.beginMaster();
	delay(100);

}
unsigned char Wire1_available(void)
{
	return dtwi1.available();
}
bool Wire1_beginTransmission(unsigned char address)
{
	unsigned int timeout;
	timeout = 0;
	while (!dtwi1.startMasterWrite(address)) { timeout++; if (timeout > DTWI_TIMEOUT) { return (false); } };
	return (true);
}

bool Wire1_endTransmission(void)
{
	unsigned int timeout;
	timeout = 0;
	while (!dtwi1.stopMaster()) { timeout++; if (timeout > DTWI_TIMEOUT) { return (false); } };
	return (true);
}
int Wire1_write(unsigned char data)
{
	dtwi1.write((const byte *)&data, 1);
}
unsigned char Wire1_read(void)
{
	byte    data;

	if (dtwi1.read(&data, 1) == 1)
	{
		return((unsigned char)data);
	}

	return('\0');
}

unsigned char Wire1_requestFrom(unsigned char address, unsigned char quantity)
{
	DTWI::I2C_STATUS status;
	unsigned int timeout;
	timeout = 0;
	while (!dtwi1.startMasterRead(address, quantity)) { timeout++; if (timeout > DTWI_TIMEOUT) { break; } };

	do
	{
		status = dtwi1.getStatus();
	} while (status.fMyBus && !status.fNacking);

	timeout = 0;
	while (!dtwi1.stopMaster()) { timeout++; if (timeout > DTWI_TIMEOUT) { break; } };

	return(dtwi1.available());
}

