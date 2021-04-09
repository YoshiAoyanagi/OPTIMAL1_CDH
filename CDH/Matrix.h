// AOCS_Matrix.h

#ifndef _AOCS_MATRIX_h
#define _AOCS_MATRIX_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <math.h>

#define DIM 3

// 行列とベクトルの掛算
void MatrixVectorTest(float *b, float *c);
void MatrixVector(float A[][DIM], float *b, float *c);
void MatrixMatrix(float A[][DIM], float B[][DIM], float C[][DIM]);
void RotationMatrix(unsigned char i, float p, float R[][DIM]);
void MatrixInverse(float A[][DIM], float B[][DIM]);

#endif

