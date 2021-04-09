// 
// 
// 

#include "MCP3421.h"
#include "Wire.h"

MCP3421::MCP3421(void) {};

bool MCP3421::init(int i2c_address, int sample_rate, int gain)
{
	unsigned char data;
	bool i_ver;

	_address = i2c_address;			// set device address
	_sampling_rate = sample_rate & 3;      // Sample Rate
	_pga_gain = gain & 3;			// PGA ampification Factor

	i_ver = Wire1_beginTransmission(_address);

	data = 0;
	data = data | (_sampling_rate << 2);
	data = data | _pga_gain;
	bitWrite(data, 7, 1);   // RDY    
	bitWrite(data, 4, 1);   // O/C 1    

	Wire1_write(data);   // config register %1000 1000
						 // /RDY = 1, One Conversion, 15 samples per, PGA = X1
	Wire1_endTransmission();

	return(i_ver);
}

long MCP3421::getRawValue(unsigned char *sts) {

	long raw_val = 0;
	unsigned char bit_data[4];
	unsigned char read_data;

	if (_sampling_rate < 3) {
		Wire1_requestFrom(_address, 3);
		bit_data[0] = Wire1_read();
		bit_data[1] = Wire1_read();
		read_data = Wire1_read();
		Wire1_endTransmission();
		raw_val = ((bit_data[0] << 8) & 0xFF00) + (bit_data[1] & 0xFF);

	}
	else {
		Wire1_requestFrom(_address, 4);
		bit_data[0] = Wire1_read();
		bit_data[1] = Wire1_read();
		bit_data[2] = Wire1_read();
		read_data = Wire1_read();
		Wire1_endTransmission();

		raw_val = (bit_data[0] << 16) & 0xFF0000
			+ (bit_data[1] << 8) & 0x00FF00
			+ (bit_data[2]);
	}
	*sts = read_data;
	return(raw_val);
}

float MCP3421::getMilliVoltage() {
	unsigned char sts;
	long raw_val = 0;
	float val = 0;

	raw_val = getRawValue(&sts);
	val = raw_val *  (0.001) / (1 << _sampling_rate * 2); // = 1mv * ADC * / sample rate factor
	val = val / (1 << _pga_gain);            // divide by pga amplificati
	
	return(val);
}

void MCP3421::getDigitalNumber(unsigned char *byte_val, float *fl_val) {
	
	float val = getMilliVoltage();
	
	*byte_val = (unsigned char)(val / MCP3421_ADC_DELTA_V_8BIT);
	*fl_val = val;

	return;
}

int MCP3421::ready() {
	unsigned char read_sts;
	getRawValue(&read_sts);
	return(!bitRead(read_sts, 7));
}