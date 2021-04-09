// AOCS_Alignment.h

#ifndef _AOCS_ALIGNMENT_h
#define _AOCS_ALIGNMENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Utility.h"

#include "Quaterninon.h"
#include "Matrix.h"

// GAS
extern float GAS_c2b_Z;
extern float GAS_c2b_Y;
extern float GAS_c2b_X;

// HGAS
extern float HGAS_c2b_Z;
extern float HGAS_c2b_Y;
extern float HGAS_c2b_X;

// GYRO
extern float GYRO_c2b_Z;
extern float GYRO_c2b_Y;
extern float GYRO_c2b_X;

// ACCL
extern float ACCEL_c2b_Z;
extern float ACCEL_c2b_Y;
extern float ACCEL_c2b_X;

// MTQ
extern signed char MTQ_c2b_X;
extern signed char MTQ_c2b_Y;
extern signed char MTQ_c2b_Z;


void Set_Alignment_DCM(float c2b_Z, float c2b_Y, float c2b_X, float c2b_DCM[3][3]);


#endif