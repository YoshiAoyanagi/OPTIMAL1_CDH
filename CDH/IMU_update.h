// IMU_Process.h

#ifndef _IMU_PROCESS_h
#define _IMU_PROCESS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "IMU.h"
#include "Alignment.h"
#include "AppInfo.h"
#include "AppHeaders.h"

#define Initial_GAS_body_ScaleFactor_X			1
#define Initial_GAS_body_ScaleFactor_Y			1
#define Initial_GAS_body_ScaleFactor_Z			1
#define Initial_GAS_body_Bias_X					64533.05292	//20171016update
#define Initial_GAS_body_Bias_Y					-11868.27268//20171016update
#define Initial_GAS_body_Bias_Z					+20994.41228//20171016update

#define Initial_GYRO_body_ScaleFactor_X			1.0 
#define Initial_GYRO_body_ScaleFactor_Y			1.0
#define Initial_GYRO_body_ScaleFactor_Z			1.0 
#define Initial_GYRO_body_Bias_X				-0.113075021
#define Initial_GYRO_body_Bias_Y				-0.180997192
#define Initial_GYRO_body_Bias_Z				+0.661083596


#define Initial_ACCEL_body_ScaleFactor_X		1.0
#define Initial_ACCEL_body_ScaleFactor_Y		1.0
#define Initial_ACCEL_body_ScaleFactor_Z		1.0
#define Initial_ACCEL_body_Bias_X				0.0
#define Initial_ACCEL_body_Bias_Y				0.0
#define Initial_ACCEL_body_Bias_Z				0.0


#define	IMU_PROCESS_TASK_INTERVAL				2	//2sec

//IMU0-GAS
extern float GAS_GeoMag_body[3];
extern float GAS_aveGeoMag_body[3];
extern float GAS_old_aveGeoMag_body[3];
extern float GAS_body_ScaleFactor[3];
extern float GAS_body_Bias[3];

//GYRO (IMU0)
extern float GYRO_Rate_body[3];
extern float GYRO_body_ScaleFactor[3];
extern float GYRO_body_Bias[3];

//ACCEL (IMU0)
extern float ACCEL_body[3];
extern float ACCEL_body_ScaleFactor[3];
extern float ACCEL_body_Bias[3];

AppInfo IMU_update(void);
void IMU_Process_Debug_Message(void);

#endif

