// SerialFlash.h

#ifndef _SERIALFLASH_h
#define _SERIALFLASH_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DSPI.h"


//#define		FLASH_ID	0x20BA21
//#define		FLASH_ID	0x202014
#define		FLASH_ID	0x012018	//S25FL127

#define		FLASH_CMD_READ_ID			0x9F
#define		FLASH_CMD_READ_DATA			0x03
#define		FLASH_CMD_READ_STATUS_REG	0x05
#define		FLASH_CMD_READ_FLAG_STATUS_REG	0x70
#define		FLASH_CMD_WRITE_ENABLE		0x06
#define		FLASH_CMD_WRITE_DISABLE		0x04
#define		FLASH_CMD_PAGE_PROGRAM		0x02
#define		FLASH_CMD_ERASE_SUBSECTOR	0x20
#define		FLASH_CMD_ERASE_SECTOR		0xD8

#define		FLASH_CMD_RESET_ENABLE		0x66
#define		FLASH_CMD_RESET_MEMORY		0x99
#define		FLASH_CMD_CLEAR_REGISTER	0x50

#define		FLASH_CMD_SUSPEND			0x7A

#define		FLASH_STS_BUSY_MASK			0x01
#define		FLASH_STS_READY				0x80

#define		FLASH_STS_IDLE				0x00
#define		FLASH_STS_BUSY				0x01
#define		FLASH_STS_FAILED			0xFE
#define		FLASH_STS_STOP				0xFF


class SerialFlash
{
public:
	SerialFlash(char cs_pin);
	char flash_cs;  //Chip select for SPI
	void dSPI_init(char cs_pin);
	unsigned char  dSPI_transfer(unsigned char data);
	void  dSPI_select(void);
	void  dSPI_deselect(void);

	int Begin(char cs_pin);
	int readID(void);

	int WriteEnable(void);
	int WriteDisable(void);
	int IsBusy(void);
	int ReadData(unsigned char *read_buf, unsigned int sector, unsigned int inner_address, unsigned int size);
	int PageWrite(unsigned char *write_buf, unsigned int sector, unsigned int inner_address, unsigned int size);
	int EraseSector(unsigned int sector);

	int Reset(void);
	int ClearRegister(void);

	int Suspend(void);
	int Ready(void);
};



#endif

