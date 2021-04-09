// 
// 
// 

#include "Alignment.h"

// GAS
float GAS_c2b_Z = 90.0;
float GAS_c2b_Y = 0.0;
float GAS_c2b_X = 180.0;

// GYRO
float GYRO_c2b_Z = 90.0;//0.0;
float GYRO_c2b_Y = -180.0;//0.0;
float GYRO_c2b_X = 90.0;//0.0;

// ACCL
float ACCEL_c2b_Z = 0.0;
float ACCEL_c2b_Y = 0.0;
float ACCEL_c2b_X = 0.0;


void Set_Alignment_DCM(float c2b_Z, float c2b_Y, float c2b_X, float c2b_DCM[3][3])
{
	float c2b_quat[4];

	Angle2Quat(c2b_Z * DEG2RAD, c2b_Y * DEG2RAD, c2b_X * DEG2RAD, c2b_quat);
	Quat0Positive(c2b_quat);
	Quat2DCM(c2b_quat, c2b_DCM);

	return;
}