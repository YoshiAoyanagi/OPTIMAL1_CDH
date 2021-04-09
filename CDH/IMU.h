// IMU.h

#ifndef _IMU_h
#define _IMU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Config.h"
#include "EPS.h"
#include "MPU9250_I2C.h"
#include "port_config.h"
#include "Utility.h"

extern MPU9250_I2C imu;
extern ack imu_access_sts;

void IMU_init(void);
void IMU_startup(void);
void IMU_Task(void);
void IMU_DebugMessage(void);

#endif