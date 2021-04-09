// 
// 
// 

#include "IMU_update.h"

//地磁気変数(GAS)
float GAS_GeoMag_body[3];			//ボディ座標系での地磁気ベクトル
float GAS_aveGeoMag_body[3];		//MTQ OFFの時の地磁気ベクトルの平均値
float GAS_old_aveGeoMag_body[3];	//上記の一つ前
float GAS_body_ScaleFactor[3];
float GAS_body_Bias[3];

//角速度変数
float GYRO_Rate_body[3];	//ボディ座標系での角速度ベクトル
float GYRO_body_ScaleFactor[3];
float GYRO_body_Bias[3];

//加速度変数
float ACCEL_body[3];	//ボディ座標系での角速度ベクトル
float ACCEL_body_ScaleFactor[3];
float ACCEL_body_Bias[3];

void IMU_update_init_(void);
void IMU_update_(void);

void IMU_Process_GYRO_Update(void);
void IMU_Process_GAS_Update(void);
void IMU_Process_ACCEL_Update(void);

AppInfo IMU_update(void)
{
	return create_app_info("IMU_UD", 10, IMU_update_init_, IMU_update_);
}

void IMU_update_init_(void)
{
	IMU_init();

	GAS_body_ScaleFactor[0] = Initial_GAS_body_ScaleFactor_X;
	GAS_body_ScaleFactor[1] = Initial_GAS_body_ScaleFactor_Y;
	GAS_body_ScaleFactor[2] = Initial_GAS_body_ScaleFactor_Z;
	GAS_body_Bias[0] = Initial_GAS_body_Bias_X;
	GAS_body_Bias[1] = Initial_GAS_body_Bias_Y;
	GAS_body_Bias[2] = Initial_GAS_body_Bias_Z;

	GYRO_body_ScaleFactor[0] = Initial_GYRO_body_ScaleFactor_X;
	GYRO_body_ScaleFactor[1] = Initial_GYRO_body_ScaleFactor_Y;
	GYRO_body_ScaleFactor[2] = Initial_GYRO_body_ScaleFactor_Z;
	GYRO_body_Bias[0] = Initial_GYRO_body_Bias_X;
	GYRO_body_Bias[1] = Initial_GYRO_body_Bias_Y;
	GYRO_body_Bias[2] = Initial_GYRO_body_Bias_Z;

	ACCEL_body_ScaleFactor[0] = Initial_ACCEL_body_ScaleFactor_X;
	ACCEL_body_ScaleFactor[1] = Initial_ACCEL_body_ScaleFactor_Y;
	ACCEL_body_ScaleFactor[2] = Initial_ACCEL_body_ScaleFactor_Z;
	ACCEL_body_Bias[0] = Initial_ACCEL_body_Bias_X;
	ACCEL_body_Bias[1] = Initial_ACCEL_body_Bias_Y;
	ACCEL_body_Bias[2] = Initial_ACCEL_body_Bias_Z;

}

void IMU_update_(void)
{
	static unsigned int interval;
	static unsigned int cycle;

	if (TMGR_get_master_cycle() < cycle)
		interval = 0;

	cycle = TMGR_get_master_cycle();

	if (cycle > ((0.5 * OBCT_CYCLES_PER_SEC) * interval))
	{
		IMU_Task();
		IMU_Process_GYRO_Update();
		IMU_Process_GAS_Update();
		IMU_Process_ACCEL_Update();
		interval++;
	}
}
void IMU_Process_GYRO_Update(void)
{
	float GYRO_Rate_comp[3];
	float GYRO_Rate_b_tmp[3];

	GYRO_Rate_comp[0] = imu.gyroscope.val[0];
	GYRO_Rate_comp[1] = imu.gyroscope.val[1];
	GYRO_Rate_comp[2] = imu.gyroscope.val[2];

	float GYRO_c2b_DCM[3][3];
	Set_Alignment_DCM(GYRO_c2b_Z, GYRO_c2b_Y, GYRO_c2b_X, GYRO_c2b_DCM);
	MatrixVector(GYRO_c2b_DCM, GYRO_Rate_comp, GYRO_Rate_b_tmp);
	int i;
	for (i = 0; i < 3; i++)
	{
		GYRO_Rate_body[i] = GYRO_Rate_b_tmp[i] * GYRO_body_ScaleFactor[i] + GYRO_body_Bias[i];
	}
}

void IMU_Process_ACCEL_Update(void)
{
	float ACCEL_comp[3];
	float ACCEL_b_tmp[3];
	ACCEL_comp[0] = imu.accelerometer.val[0];
	ACCEL_comp[1] = imu.accelerometer.val[1];
	ACCEL_comp[2] = imu.accelerometer.val[2];

	float ACCEL_c2b_DCM[3][3];
	Set_Alignment_DCM(ACCEL_c2b_Z, ACCEL_c2b_Y, ACCEL_c2b_X, ACCEL_c2b_DCM);
	MatrixVector(ACCEL_c2b_DCM, ACCEL_comp, ACCEL_b_tmp);
	int i;
	for (i = 0; i < 3; i++)
	{
		ACCEL_body[i] = ACCEL_b_tmp[i] * ACCEL_body_ScaleFactor[i] + ACCEL_body_Bias[i];
	}
}

void IMU_Process_GAS_Update(void)
{
	float GAS_GeoMag_comp[3];
	float GAS_GeoMag_b_tmp[3];

	GAS_GeoMag_comp[0] = imu.magnetometer.val[0];
	GAS_GeoMag_comp[1] = imu.magnetometer.val[1];
	GAS_GeoMag_comp[2] = imu.magnetometer.val[2];

	float GAS_c2b_DCM[3][3];
	Set_Alignment_DCM(GAS_c2b_Z, GAS_c2b_Y, GAS_c2b_X, GAS_c2b_DCM);

	MatrixVector(GAS_c2b_DCM, GAS_GeoMag_comp, GAS_GeoMag_b_tmp);

	int i;
	for (i = 0; i < 3; i++)
	{
		GAS_GeoMag_body[i] = GAS_GeoMag_b_tmp[i] * GAS_body_ScaleFactor[i] + GAS_body_Bias[i];
	}
}


//DebugMessageList
void IMU_Process_Debug_Message(void)
{
	DEBUG_Serial.print("GAS_body[nT]: ");
	DEBUG_Serial.print(GAS_GeoMag_body[0], 2); DEBUG_Serial.print(" ");
	DEBUG_Serial.print(GAS_GeoMag_body[1], 2); DEBUG_Serial.print(" ");
	DEBUG_Serial.print(GAS_GeoMag_body[2], 2); DEBUG_Serial.println(" ");

	DEBUG_Serial.print("GYRO_body[dps]: ");
	DEBUG_Serial.print(GYRO_Rate_body[0], 2); DEBUG_Serial.print(" ");
	DEBUG_Serial.print(GYRO_Rate_body[1], 2); DEBUG_Serial.print(" ");
	DEBUG_Serial.print(GYRO_Rate_body[2], 2); DEBUG_Serial.println(" ");

	DEBUG_Serial.print("ACCEL[G]: ");
	DEBUG_Serial.print(ACCEL_body[0], 2); DEBUG_Serial.print(" ");
	DEBUG_Serial.print(ACCEL_body[1], 2); DEBUG_Serial.print(" ");
	DEBUG_Serial.print(ACCEL_body[2], 2); DEBUG_Serial.println(" ");

	DEBUG_Serial.print("TEMP[degC]: ");
	DEBUG_Serial.print(imu.temperature.val[0], 2); DEBUG_Serial.println(" ");

	return;
}