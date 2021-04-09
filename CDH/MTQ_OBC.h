// MTQ_OBC.h

#ifndef _MTQ_OBC_h
#define _MTQ_OBC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MCP23008.h"
#include "port_config.h"

#define	MTQ_P		1
#define MTQ_M		2
#define MTQ_NONE	0
#define MTQ_STOP	0

typedef struct {
	char x;
	char y;
	char z;
}MTQ_STS;

extern MTQ_STS	mtq_sts;

// MTQ
extern signed char MTQ_c2b_X;
extern signed char MTQ_c2b_Y;
extern signed char MTQ_c2b_Z;

int MTQ_IOE_init(void);
void MTQ_IOE_PWR(int pwr_onoff);
void MTQ_OutputUpdate(void);
signed char MTQ_PORT_READ(int port_p, int port_m, signed char Sign);
void MTQ_PORT_Set(int port_p, int port_m, signed char Sign, char out);

#endif

