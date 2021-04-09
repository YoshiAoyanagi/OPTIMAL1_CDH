// AOCS_Quaterninon.h

#ifndef _AOCS_QUATERNINON_h
#define _AOCS_QUATERNINON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <math.h>
#include "Matrix.h"

void Quat2DCM(float *q, float DCM[][3]);
void DCM2Quat(float DCM[][3], float *q);
void QuatMul(float *a, float *b, float *c);
void Angle2Quat(float Z, float Y, float X, float *q);
void Angle2Quat_1axis(unsigned char i, float theta, float *q);
void NormalizeQuat(float *a);
void Quat0Positive(float *a);
void QuatInverse(float *a, float *b);
void QuatVector(float *q, float *r, float *rd);

#endif

