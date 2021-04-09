// 
// 
// 

#include "IMU.h"

ack imu_access_sts;
MPU9250_I2C imu;

void IMU_init(void)
{
	PWR_IMU(PWR_ON);
	delay(10);	//óßÇøè„Ç™ÇËë“Çøéûä‘10ms

	IMU_startup();

	return;
}

void IMU_startup(void)
{
	int i_ret;

	imu.Init(BITS_DLPF_CFG_98HZ);

	if (imu.WhoAmI() && imu.MAG_WhoAmI())
	{
		Serial.println("Accelerometer, Gyroscope initialize OK");
		delay(100);
	}
	else
	{
		Serial.println("--Sensor Initialize Error!!--");
		imu_access_sts = Error;
		return;
	}

	i_ret = imu.Set_Accel_Range(BITS_FS_16G);
	Serial.print("range of Accelerometer: +/-"); Serial.print(i_ret); Serial.println("G");
	delay(100);

	i_ret = imu.Set_Gyro_Range(BITS_FS_1000DPS);
	Serial.print("range of Gyroscope: +/-"); Serial.print(i_ret); Serial.println("deg/s");

	delay(100);

	i_ret = imu.Calib_Magnetometer();
	delay(100);

	Serial.println("--Sensor Initialize OK!!--");

	imu_access_sts = Success;

	return;
}
void IMU_Task(void)
{
	imu.Read_All();
	return;
}

void IMU_DebugMessage(void)
{
	Serial.print("IMU: ");
	Serial.print("Accel: ");
		Serial.print(imu.accelerometer.val[0], 2); Serial.print(", ");
		Serial.print(imu.accelerometer.val[1], 2); Serial.print(", ");
		Serial.print(imu.accelerometer.val[2], 2); Serial.println("");

	Serial.print("Gyro: ");
		Serial.print(imu.gyroscope.val[0], 2); Serial.print(", ");
		Serial.print(imu.gyroscope.val[1], 2); Serial.print(", ");
		Serial.print(imu.gyroscope.val[2], 2); Serial.println("");

	Serial.print("Mag: ");
		Serial.print(imu.magnetometer.val[0], 2); Serial.print(", ");
		Serial.print(imu.magnetometer.val[1], 2); Serial.print(", ");
		Serial.print(imu.magnetometer.val[2], 2); Serial.println("");
	
	Serial.print("Temp: ");
		Serial.print(imu.temperature.val[0], 2); Serial.println("");

	return;
}