// 
// 
// 

#include "Matrix.h"

// s—ñ‚ÆƒxƒNƒgƒ‹‚ÌŠ|Z
void MatrixVectorTest(float *b, float *c) {

	int i, j;

	for (i = 0; i < DIM; i++) {
		c[i] = 0.0;
		for (j = 0; j < DIM; j++) {
			c[i] += 0 * b[j];
		}
	}

}

void MatrixVector(float A[][DIM], float *b, float *c) {

	int i, j;

	for (i = 0; i < DIM; i++) {
		c[i] = 0.0;
		for (j = 0; j < DIM; j++) {
			c[i] += A[i][j] * b[j];
		}
	}

}

// s—ñ‚Æs—ñ‚ÌŠ|Z
void MatrixMatrix(float A[][DIM], float B[][DIM], float C[][DIM]) {

	int i, j, k;

	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			C[i][j] = 0.0;
			for (k = 0; k < DIM; k++) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}

}

// ‰ñ“]s—ñ‚ÌŒvZ
void RotationMatrix(unsigned char i, float p, float R[][DIM]) {

	float cp, sp;
	
	cp = cos(p);
	sp = sin(p);

	if (i == 1) {
		R[0][0] = 1.0;
		R[0][1] = 0.0;
		R[0][2] = 0.0;
		R[1][0] = 0.0;
		R[1][1] = cp;
		R[1][2] = sp;
		R[2][0] = 0.0;
		R[2][1] = -sp;
		R[2][2] = cp;
	}

	if (i == 2) {
		R[0][0] = cp;
		R[0][1] = 0.0;
		R[0][2] = -sp;
		R[1][0] = 0.0;
		R[1][1] = 1.0;
		R[1][2] = 0.0;
		R[2][0] = sp;
		R[2][1] = 0.0;
		R[2][2] = cp;
	}

	if (i == 3) {
		R[0][0] = cp;
		R[0][1] = sp;
		R[0][2] = 0.0;
		R[1][0] = -sp;
		R[1][1] = cp;
		R[1][2] = 0.0;
		R[2][0] = 0.0;
		R[2][1] = 0.0;
		R[2][2] = 1.0;
	}

}

// ‹ts—ñ‚ÌŒvZ
void MatrixInverse(float A[][DIM], float B[][DIM]) {

	float det = A[0][0] * A[1][1] * A[2][2]
		+ A[1][0] * A[2][1] * A[0][2]
		+ A[2][0] * A[0][1] * A[1][2]
		- A[0][0] * A[2][1] * A[1][2]
		- A[2][0] * A[1][1] * A[0][2]
		- A[1][0] * A[0][1] * A[2][2];

	B[0][0] = (A[1][1] * A[2][2] - A[1][2] * A[2][1]) / det;
	B[0][1] = (A[0][2] * A[2][1] - A[0][1] * A[2][2]) / det;
	B[0][2] = (A[0][1] * A[1][2] - A[0][2] * A[1][1]) / det;

	B[1][0] = (A[1][2] * A[2][0] - A[1][0] * A[2][2]) / det;
	B[1][1] = (A[0][0] * A[2][2] - A[0][2] * A[2][0]) / det;
	B[1][2] = (A[0][2] * A[1][0] - A[0][0] * A[1][2]) / det;

	B[2][0] = (A[1][0] * A[2][1] - A[1][1] * A[2][0]) / det;
	B[2][1] = (A[0][1] * A[2][0] - A[0][0] * A[2][1]) / det;
	B[2][2] = (A[0][0] * A[1][1] - A[0][1] * A[1][0]) / det;

}

