#ifndef PORT_CONFIG_H
#define PORT_CONFIG_H

#define H  1
#define L  0

//PCU_GPIO
#define PORT_PCU_SELECT				98
#define PORT_PCU_RESET1				99
#define PORT_PCU_RESET2				100
#define PORT_PCU_SELECT_BAT1		33
#define PORT_PCU_SELECT_BAT2		32

//Power
#define PORT_HEATER_PWR				76
#define	PORT_MSN_UNREG_PWR			77
#define PORT_RW_UNREG_PWR			78
#define	PORT_IMU_PWR				44

#define PORT_UNREG_ON				89
#define PORT_UNREG_OFF				90

//OPTIMAL-1
#define PORT_AQU_5V_PWR				38
#define PORT_OPT_5V_PWR				34
// add OVCO
#define PORT_OVCO_RESET				82


//AnalogPort for Battery
#define PORT_AN_BAT1_MON			3

//Serial_Mux
#define PORT_SERIAL_MUX_A0			41
#define PORT_SERIAL_MUX_A1			42
#define PORT_SERIAL_MUX_EN			43

//Temperature sensor
#define	PORT_AN_EN					96
#define PORT_TEMP_RTD				2
#define PORT_TEMP_AN0				92
#define PORT_TEMP_AN1				93
#define PORT_TEMP_AN2				94
#define PORT_TEMP_AN3				95


//For SPI
#define SERIAL_CS					69
#define DR_CS						5

#define FRAM1_CS					69
#define FRAM2_CS					71
#define FLASH1_CS					73
#define FLASH2_CS					74

//ADCS
#define	ADCS_I2C_ENABLE				68

#define	SEND_ON	 1
#define SEND_OFF 0

//ADCS_I2C I/O-Expandar
#define		MTQ_IOE_PORT_PWR		0
#define		MTQ_IOE_PORT_PY			1
#define		MTQ_IOE_PORT_MY			2

#define		MTQ_IOE_PORT_PX			3
#define		MTQ_IOE_PORT_MX			4

#define		MTQ_IOE_PORT_PZ			5
#define		MTQ_IOE_PORT_MZ			6
#define		MTQ_IOE_PORT_FAULT		7


// add STx pin
#define	PORT_AD_STX_RTS		18
#define	PORT_AD_STX_EMPTY	83

//add_STRX 電源操作
#define PORT_SRXIF_PWR		60
#define PORT_AD_STX_PWR		61
#define PORT_AD_SRX_PWR		35
#define PORT_AD_SRX_RX_ON	7

//add Atmega interface
#define PORT_SRXIF_STS1		5
#define PORT_SRXIF_STS2		6

//add AD-SRXのキャリア，サブキャリアステータス
#define PORT_AD_SRX_C_LOCK	3
#define PORT_AD_SRX_SC_LOCK	4


// analog
#define PORT_AN_AD_STX_TEMP			4	//add g2
#define PORT_AN_AD_SRX_AGC_INC		5	//add g2
#define PORT_AN_AD_SRX_AGC_COH		6	//add g2
#define PORT_AN_AD_SRX_FREQ_ERR		7	//add g2

#endif
