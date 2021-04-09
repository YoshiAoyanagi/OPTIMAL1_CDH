// Reed_Solomon.h

#ifndef _REED_SOLOMON_h
#define _REED_SOLOMON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/* リードソロモン(255,223)の定義 */
#define RS_MM  8
#define RS_NN  255
#define RS_TT  16//32
#define RS_KK  223//191

class Reed_SolomonClass
{
 protected:


 public:
	void init();
	void encode(unsigned char *data, unsigned int size, unsigned char *rs_code, unsigned int *rs_size);
private:
	void generate_gf(void);
	void gen_poly(void);

};

extern Reed_SolomonClass Reed_Solomon;

#endif

