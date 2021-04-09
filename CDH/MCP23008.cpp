// 
// 
// 

#include "MCP23008.h"

int MCP23008::begin(uint8_t addr) {
	bool i_ret = 0;

	if (addr > 7) {
		addr = 7;
	}
	i2caddr = addr;

	// set defaults!
	i_ret = Wire0_beginTransmission(MCP23008_ADDRESS | i2caddr);

	Wire0_write((byte)MCP23008_IODIR);
	Wire0_write((byte)0xFF);  // all inputs
	Wire0_write((byte)0x00);
	Wire0_write((byte)0x00);
	Wire0_write((byte)0x00);
	Wire0_write((byte)0x00);
	Wire0_write((byte)0x00);
	Wire0_write((byte)0x00);
	Wire0_write((byte)0x00);
	Wire0_write((byte)0x00);
	Wire0_write((byte)0x00);

	Wire0_endTransmission();
	return (int)i_ret;
}

int MCP23008::begin(void) {
	return begin(0);
}

void MCP23008::pinMode(uint8_t p, uint8_t d) {
	uint8_t iodir;


	// only 8 bits!
	if (p > 7)
		return;

	iodir = read8(MCP23008_IODIR);

	// set the pin and direction
	if (d == INPUT) {
		iodir |= 1 << p;
	}
	else {
		iodir &= ~(1 << p);
	}

	// write the new IODIR
	write8(MCP23008_IODIR, iodir);
}

uint8_t MCP23008::readGPIO(void) {
	// read the current GPIO input 
	return read8(MCP23008_GPIO);
}

void MCP23008::writeGPIO(uint8_t gpio) {
	write8(MCP23008_GPIO, gpio);
}


void MCP23008::digitalWrite(uint8_t p, uint8_t d) {
	uint8_t gpio;

	// only 8 bits!
	if (p > 7)
		return;

	// read the current GPIO output latches
	gpio = readGPIO();

	// set the pin and direction
	if (d == HIGH) {
		gpio |= 1 << p;
	}
	else {
		gpio &= ~(1 << p);
	}

	// write the new GPIO
	writeGPIO(gpio);
}

void MCP23008::pullUp(uint8_t p, uint8_t d) {
	uint8_t gppu;

	// only 8 bits!
	if (p > 7)
		return;

	gppu = read8(MCP23008_GPPU);
	// set the pin and direction
	if (d == HIGH) {
		gppu |= 1 << p;
	}
	else {
		gppu &= ~(1 << p);
	}
	// write the new GPIO
	write8(MCP23008_GPPU, gppu);
}

uint8_t MCP23008::digitalRead(uint8_t p) {
	// only 8 bits!
	if (p > 7)
		return 0;

	// read the current GPIO
	return (readGPIO() >> p) & 0x1;
}

uint8_t MCP23008::read8(uint8_t addr) {
	Wire0_beginTransmission(MCP23008_ADDRESS | i2caddr);

	Wire0_write((byte)addr);

	Wire0_endTransmission();
	Wire0_requestFrom(MCP23008_ADDRESS | i2caddr, 1);

	return Wire0_read();
}


void MCP23008::write8(uint8_t addr, uint8_t data) {
	Wire0_beginTransmission(MCP23008_ADDRESS | i2caddr);

	Wire0_write((byte)addr);
	Wire0_write((byte)data);

	Wire0_endTransmission();
}
