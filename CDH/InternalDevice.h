#ifndef INTERNAL_DEVICE_H
#define INTERNAL_DEVICE_H

#include "DSPI.h"
#include "dWire0.h"
#include "dWire1.h"

void InternalDevice_init(void);

void SPI1_Init();
void SPI0_Init();

unsigned char	SPI1_Transfer(unsigned char data);
unsigned char	SPI0_Transfer(unsigned char data);
void			SPI_Wait(void);
void			SPI_Select(char pin, bool select);

#endif
