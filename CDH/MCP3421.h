// MCP3421.h

#ifndef _MCP3421_h
#define _MCP3421_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DTWI.h"
//#include "dWire0.h"
#include "dWire1.h"

#define	MCP3421_ADC_DELTA_V_8BIT	0.02

class MCP3421
{
 protected:
 public:
	 MCP3421(void);
	 bool init(int address, int sample_rate, int gain);
	 long getRawValue(unsigned char *sts);
	 float getMilliVoltage(void);
	 void  getDigitalNumber(unsigned char *byte_val, float *fl_val);
	 int ready();
 private:
	 int  _address;  // i2c address
	 int _sampling_rate;  // sample rate 0..7 / 12,14,16,18 bit
	 int _pga_gain; // PGA gain 1=1, 2;=2, 3=4 , 4=8

};


#endif

