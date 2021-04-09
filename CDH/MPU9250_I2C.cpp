//#include "arduino.h" 
#include "MPU9250_I2C.h"

MPU9250_I2C::MPU9250_I2C(void){}

float MPU9250_I2C::Uint2Float(unsigned int val)
{
	int i_val;
	float f_val;
	if(val > 0x8000)
          i_val =  -1 * (0x7FFF - (val & 0x7FFF)) - 1;
    else
        i_val =  1 * (val & ~0x8000);

	f_val = (float)i_val;
	return f_val;
}

void MPU9250_I2C::WriteReg(unsigned char address, unsigned char reg, unsigned char data)
{
	Wire0_beginTransmission(address);
	Wire0_write(reg);
	Wire0_write(data);
	Wire0_endTransmission();
	return;
}

void MPU9250_I2C::ReadRegs(unsigned char address, unsigned char reg, unsigned char *data, unsigned int len)
{
	// Set register address
	Wire0_beginTransmission(address);
	Wire0_write(reg);
	Wire0_endTransmission();
	
	// Read bytes
	Wire0_requestFrom(address, len);
	unsigned char j = 0;
	while(Wire0_available())
	{
		data[j] = Wire0_read();
		j++;
	}

	return;

}


/*-----------------------------------------------------------------------------------------------
WHO AM I?
-----------------------------------------------------------------------------------------------*/
unsigned int MPU9250_I2C::WhoAmI() {

	unsigned char response[1];
	
	int i_ret = 0;
	ReadRegs(MPU9250_I2C_ADDRESS, MPUREG_WHOAMI, response, 1);
	//Serial.println(response[0], DEC);
	if (response[0] == 0x71)
		i_ret = 1;

	return i_ret;
}

int MPU9250_I2C::Init(unsigned char low_pass_filter)
{
	int i_ret = 0;
	int i;

	//Initialize for calibration value
	accelerometer.val_factor[0]	= FACTOR_ACCELEROMETER_0;
	accelerometer.val_factor[1]	= FACTOR_ACCELEROMETER_1;
	accelerometer.val_factor[2]	= FACTOR_ACCELEROMETER_2;
	accelerometer.val_offset[0]	= OFFSET_ACCELEROMETER_0;
	accelerometer.val_offset[1]	= OFFSET_ACCELEROMETER_1;
	accelerometer.val_offset[2]	= OFFSET_ACCELEROMETER_2;

	gyroscope.val_factor[0]		= FACTOR_GYROSCOPE_0;
	gyroscope.val_factor[1]		= FACTOR_GYROSCOPE_1;
	gyroscope.val_factor[2]		= FACTOR_GYROSCOPE_2;
	gyroscope.val_offset[0]		= OFFSET_GYROSCOPE_0;
	gyroscope.val_offset[1]		= OFFSET_GYROSCOPE_1;
	gyroscope.val_offset[2]		= OFFSET_GYROSCOPE_2;

	magnetometer.val_factor[0]		= FACTOR_MAGNETOMETER_0;
	magnetometer.val_factor[1]		= FACTOR_MAGNETOMETER_1;
	magnetometer.val_factor[2]		= FACTOR_MAGNETOMETER_2;
	magnetometer.val_offset[0]		= OFFSET_MAGNETOMETER_0;
	magnetometer.val_offset[1]		= OFFSET_MAGNETOMETER_1;
	magnetometer.val_offset[2]		= OFFSET_MAGNETOMETER_2;

	temperature.val_factor[0]		= FACTOR_TEMPERATURE;
	temperature.val_offset[0]		= OFFSET_TEMPERATURE;

	//MPU_InitRegNum

	unsigned char MPU_Init_Data[6][2] = {
		{ 0x00, MPUREG_PWR_MGMT_1 },     // Enable Acc & Gyro
		{ low_pass_filter, MPUREG_CONFIG },         // Use DLPF set Gyroscope bandwidth 184Hz, temperature bandwidth 188Hz
		{ 0x18, MPUREG_GYRO_CONFIG },    // +-2000dps
		{ 0x08, MPUREG_ACCEL_CONFIG },   // +-4G
		{ 0x09, MPUREG_ACCEL_CONFIG_2 }, // Set Acc Data Rates, Enable Acc LPF , Bandwidth 184Hz
		{ 0x02, MPUREG_INT_PIN_CFG },    //
	};

	for (i = 0; i < 6; i++) {
		WriteReg(MPU9250_I2C_ADDRESS, MPU_Init_Data[i][1], MPU_Init_Data[i][0]);
		delay(10);
	}

	unsigned char MAG_Init_Data[1][2] = {
		{ 0x16, AK8963_CNTL1 }, //100Hz˜A‘±ƒ‚[ƒh
	};

	for (i = 0; i < 1; i++) {
		WriteReg(MAG_I2C_ADDRESS, MAG_Init_Data[i][1], MAG_Init_Data[i][0]);
		delay(10);
	}

    Set_Accel_Range(BITS_FS_16G);
    Set_Gyro_Range(BITS_FS_250DPS);
    
	return i_ret;
}

int MPU9250_I2C::Set_LPF(int LPF){

	if(LPF > 0x08)
		return (-1);

    WriteReg(MPU9250_I2C_ADDRESS, MPUREG_CONFIG, LPF);

	return (1);
}

/*-----------------------------------------------------------------------------------------------
                                ACCELEROMETER SCALE
-----------------------------------------------------------------------------------------------*/
unsigned int MPU9250_I2C::Set_Accel_Range(int range){
	int temp_range = 0;
    WriteReg(MPU9250_I2C_ADDRESS, MPUREG_ACCEL_CONFIG, range);
    
    switch (range){
        case BITS_FS_2G:
            acc_divider=16384;
			temp_range = 2;
        break;
        case BITS_FS_4G:
            acc_divider=8192;
			temp_range = 4;
        break;
        case BITS_FS_8G:
            acc_divider=4096;
			temp_range = 8;
        break;
        case BITS_FS_16G:
            acc_divider=2048;
			temp_range = 16;
        break;
    }
	accelerometer.range = temp_range;
    return temp_range;
}

/*-----------------------------------------------------------------------------------------------
                                GYROSCOPE SCALE
-----------------------------------------------------------------------------------------------*/
unsigned int MPU9250_I2C::Set_Gyro_Range(int range){
	int temp_range = 0;
    WriteReg(MPU9250_I2C_ADDRESS, MPUREG_GYRO_CONFIG, range);
    switch (range){
        case BITS_FS_250DPS:
            gyro_divider=131;
			temp_range = 250;
        break;
        case BITS_FS_500DPS:
            gyro_divider=65.5;
			temp_range = 500;
        break;
        case BITS_FS_1000DPS:
            gyro_divider=32.8;
			temp_range = 1000;
        break;
        case BITS_FS_2000DPS:
            gyro_divider=16.4;
			temp_range = 2000;
        break;   
    }
	gyroscope.range = temp_range;
    return temp_range;
}

/*-----------------------------------------------------------------------------------------------
                                READ DATA
usage: call this function to read accelerometer data. Axis represents selected axis:
0 -> X axis
1 -> Y axis
2 -> Z axis
-----------------------------------------------------------------------------------------------*/
void MPU9250_I2C::Read_Accel()
{
    unsigned char response[6];
    int i;
	
    ReadRegs(MPU9250_I2C_ADDRESS, MPUREG_ACCEL_XOUT_H, response, 6);
    
	for(i=0; i < 3; i++) {

		accelerometer.raw[i]=((unsigned int)response[i*2]<<8)|response[i*2+1];
		accelerometer.val[i] = (Uint2Float(accelerometer.raw[i]) / acc_divider);

    }
}

void MPU9250_I2C::Read_Gyro()
{
    unsigned char response[6];
    int i;
	
    ReadRegs(MPU9250_I2C_ADDRESS, MPUREG_GYRO_XOUT_H, response, 6);
    
	for(i=0; i < 3; i++) {

		gyroscope.raw[i]=((unsigned int)response[i*2]<<8)|response[i*2+1];
		gyroscope.val[i] = (Uint2Float(gyroscope.raw[i]) / gyro_divider);

    }
}

void MPU9250_I2C::Read_Temperature(){

	unsigned char response[6];
    int i;
	
    ReadRegs(MPU9250_I2C_ADDRESS, MPUREG_TEMP_OUT_H, response, 6);
    
	temperature.raw[0]=((unsigned int)response[i*2]<<8)|response[i*2+1];
	temperature.val[0] = (Uint2Float(temperature.raw[0]) / 340.0) + 36.53;

}
unsigned int MPU9250_I2C::MAG_WhoAmI() {

	unsigned char response[1];

	int i_ret = 0;
	ReadRegs(MAG_I2C_ADDRESS, AK8963_WIA, response, 1);

	if (response[0] == 72)
		i_ret = 1;

	return i_ret;
}
unsigned int MPU9250_I2C::Calib_Magnetometer(){

    unsigned char response[3];
    float data;
    int i;

	ReadRegs(MAG_I2C_ADDRESS, MPUREG_EXT_SENS_DATA_00,response,3);
    
    for(i=0; i<3; i++) {
        data = response[i];
        magnetometer_asa[i] = ((data - 128) / 256 + 1) * Magnetometer_Sensitivity_Scale_Factor * 1000;

    }
	return 0;
}

void MPU9250_I2C::Read_Magnetometer(){
    
	unsigned char response[7];
    int i;

	int timeout = 0;
	unsigned int tmp_raw[3];
	float tmp_fl[3];

	unsigned char ST1;
	do
	{
		ReadRegs(MAG_I2C_ADDRESS, 0x02, response, 1);
		timeout++; if (timeout > 0xFF) { break; }
	} while (!(response[0] & 0x01));

    ReadRegs(MAG_I2C_ADDRESS,0x03, response,7);
    
	for(i=0; i < 3; i++) {

		tmp_raw[i]=((unsigned int)response[i*2+1]<<8)|response[i*2];
		tmp_fl[i] = (Uint2Float(magnetometer.raw[i]) * magnetometer_asa[i]);

    }
	magnetometer.raw[0] = tmp_raw[1];
	magnetometer.raw[1] = tmp_raw[0];
	magnetometer.raw[2] = tmp_raw[2];

	magnetometer.val[0] = tmp_fl[1];
	magnetometer.val[1] = tmp_fl[0];
	magnetometer.val[2] = -1 * tmp_fl[2];
}

void MPU9250_I2C::Read_All(){

    unsigned char response[14];
    int bit_data;
    float data;
    int i;
	
    ReadRegs(MPU9250_I2C_ADDRESS, MPUREG_ACCEL_XOUT_H, response, 14);
	
	//Get accelerometer value
    for(i=0; i<3; i++) {
		accelerometer.raw[i]=((unsigned int)response[i*2]<<8)|response[i*2+1];
		accelerometer.val[i] = (Uint2Float(accelerometer.raw[i]) / acc_divider);
    }
	accelerometer = GenCalibrateVal(accelerometer);

    //Get temperature
	temperature.raw[0]=((unsigned int)response[i*2]<<8)|response[i*2+1];
	temperature.val[0] = ((Uint2Float(temperature.raw[0]) - 21) / 333.87) + 21;
	temperature = GenCalibrateVal(temperature);

    //Get gyroscop value
    for(i=4; i<7; i++) {
		gyroscope.raw[i-4]=((unsigned int)response[i*2]<<8)|response[i*2+1];
		gyroscope.val[i-4] = (Uint2Float(gyroscope.raw[i-4]) / gyro_divider);
    }
	gyroscope = GenCalibrateVal(gyroscope);

    //Get Magnetometer value
	Read_Magnetometer();
	magnetometer = GenCalibrateVal(magnetometer);

	return;
}

void MPU9250_I2C::Set_GyroCalibParameters(float scale[3], float offset[3])
{
	int i;
	for(i = 0; i < 3; i++)
	{
		gyroscope.val_factor[i] = scale[i];
	}
	for(i = 0; i < 3; i++)
	{
		gyroscope.val_offset[i] = offset[i];
	}

	return;
}

void MPU9250_I2C::Set_AccelCalibParameters(float scale[3], float offset[3])
{
	int i;
	for(i = 0; i < 3; i++)
	{
		accelerometer.val_factor[i] = scale[i];
	}
	for(i = 0; i < 3; i++)
	{
		accelerometer.val_offset[i] = offset[i];
	}

	return;
}

void MPU9250_I2C::Set_MagCalibParameters(float scale[3], float offset[3])
{
	int i;
	for(i = 0; i < 3; i++)
	{
		magnetometer.val_factor[i] = scale[i];
	}
	for(i = 0; i < 3; i++)
	{
		magnetometer.val_offset[i] = offset[i];
	}

	return;
}

SensData MPU9250_I2C::GenCalibrateVal(SensData sens)
{
	SnsData cal_sens = sens;
	int i;
	for(i = 0; i < 3; i++)
	{
		cal_sens.val[i] = sens.val_factor[i] * sens.val[i] + sens.val_offset[i];
	}

	return cal_sens;
}
