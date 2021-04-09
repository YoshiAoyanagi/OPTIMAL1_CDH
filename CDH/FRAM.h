// FRAM.h

#ifndef _FRAM_h
#define _FRAM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DSPI.h"

static char fram_cs;  //Chip select for SPI

#define		FRAM_WREN_ID	0x06
#define		FRAM_WRDI_ID	0x04
#define		FRAM_RDSR_ID	0x05
#define		FRAM_WRSR_ID	0x01
#define		FRAM_READ_ID	0x03
#define		FRAM_WRITE_ID	0x02

#define		FRAM_CAPACITY_MAX	125000//B

class FRAM
{
public:
	FRAM();

	void dSPI_init(char cs_pin);
	unsigned char  dSPI_transfer(unsigned char data);
	void  dSPI_select(void);
	void  dSPI_deselect(void);

	void Begin(char cs_pin);
	void WriteEnable(void);
	void WriteData(unsigned char *data, unsigned int address, unsigned int size);
	void ReadData(unsigned char *data, unsigned int address, unsigned int size);
};

#endif

