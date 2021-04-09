#include "InternalDevice.h"

DSPI1  spi1;
DSPI0  spi0;

void InternalDevice_init(void)
{
	Wire0_Init();
	Wire1_Init();
	
	SPI0_Init();
	//SPI1_Init();
}

void SPI1_Init(void)
{
  spi1.begin();
  spi1.setSpeed(2000000);  //8MHz
}

unsigned char  SPI1_Transfer(unsigned char data)
{
  return (spi1.transfer(data));
}
void  SPI_Select(char pin, bool sts)
{
  digitalWrite(pin, sts);
}
void  SPI_Wait()
{
  delayMicroseconds(10);
}

void SPI0_Init(void)
{
	spi0.begin();
	spi0.setSpeed(2000000);  //8MHz
}
unsigned char  SPI0_Transfer(unsigned char data)
{
	return (spi0.transfer(data));
}