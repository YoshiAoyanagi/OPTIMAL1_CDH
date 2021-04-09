#ifndef IMU_CONFIG_h
#define IMU_CONFIG_h

typedef struct SensData
{
	float val[3];
	unsigned int raw[3];
	float range;
	float val_factor[3];
	float val_offset[3];
}SnsData;

//Calibration value
#define	FACTOR_ACCELEROMETER_0	1.0
#define	FACTOR_ACCELEROMETER_1	1.0
#define	FACTOR_ACCELEROMETER_2	1.0
#define	OFFSET_ACCELEROMETER_0	0.0
#define	OFFSET_ACCELEROMETER_1	0.0
#define	OFFSET_ACCELEROMETER_2	0.0

#define	FACTOR_GYROSCOPE_0		1.0
#define	FACTOR_GYROSCOPE_1		1.0
#define	FACTOR_GYROSCOPE_2		1.0
#define	OFFSET_GYROSCOPE_0		0
#define	OFFSET_GYROSCOPE_1		0
#define	OFFSET_GYROSCOPE_2		0

#define	FACTOR_MAGNETOMETER_0	1.0 //X
#define	FACTOR_MAGNETOMETER_1	1.0 //Y
#define	FACTOR_MAGNETOMETER_2	1.0 //Z
#define	OFFSET_MAGNETOMETER_0	0
#define	OFFSET_MAGNETOMETER_1	0
#define	OFFSET_MAGNETOMETER_2	0


#define	FACTOR_TEMPERATURE		1.063	//1.0
#define	OFFSET_TEMPERATURE		-13.3	//0.0


#endif