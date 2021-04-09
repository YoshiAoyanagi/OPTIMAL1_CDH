// MCP4716.h

#ifndef _MCP4716_h
#define _MCP4716_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DTWI.h"
#include "dWire1.h"

// I2C Address of device
// MCP4706, MCP4716 & MCP4726 are factory programed for any of 0x60 thru 0x67
//  commonly 0x60
#define MCP47X6_DEFAULT_ADDRESS	0x60

/** Config Data Defn
* Config = 0bCCCVVPPG
*/

// Programmable Gain definitions
#define MCP47X6_GAIN_MASK	0xFE
#define MCP47X6_GAIN_1X	0x00
#define MCP47X6_GAIN_2X	0x01

// Power Down Mode definitions
#define MCP47X6_PWRDN_MASK     0xF9
#define MCP47X6_AWAKE          0x00
#define MCP47X6_PWRDN_1K       0x02
#define MCP47X6_PWRDN_100K     0x04
#define MCP47X6_PWRDN_500K     0x06

// Reference Voltage definitions
#define MCP47X6_VREF_MASK             0xE7
#define MCP47X6_VREF_VDD              0x00
#define MCP47X6_VREF_VREFPIN	      0x10
#define MCP47X6_VREF_VREFPIN_BUFFERED 0x18

// Command definitioins
#define MCP47X6_CMD_MASK       0x1F
#define MCP47X6_CMD_VOLDAC     0x00
#define MCP47X6_CMD_VOLALL     0x40
#define MCP47X6_CMD_VOLCONFIG  0x80
#define MCP47X6_CMD_ALL        0x60


class MCP47X6 {
public:
	MCP47X6();
	MCP47X6(uint8_t address);

	bool testConnection(void);

	bool begin(void);
	bool begin(uint8_t config);

	// Set the configuration bits for the DAC
	void setGain(uint8_t gain);
	void setVReference(uint8_t ref);
	bool saveSettings(void);

	// Set the DAC
	bool setOutputLevel(uint8_t level);
	bool setOutputLevel(uint16_t level);

	// Power Down
	// NOTE: writing any settings or DAC value
	// will awaken device
	bool powerDown(uint8_t pdOutR);
	bool powerDown(void);

private:
	bool writeConfigReg(uint8_t theConfig);
	uint8_t devAddr;
	uint8_t config;
};











//
//// Programmable Gain definitions
//#define MCP47X6_GAIN_MASK	0xFE
//#define MCP47X6_GAIN_1X	0x00
//#define MCP47X6_GAIN_2X	0x01
//
//// Power Down Mode definitions
//#define MCP47X6_PWRDN_MASK     0xF9
//#define MCP47X6_AWAKE          0x00
//#define MCP47X6_PWRDN_1K       0x02
//#define MCP47X6_PWRDN_100K     0x04
//#define MCP47X6_PWRDN_500K     0x06
//
//// Reference Voltage definitions
//#define MCP47X6_VREF_MASK             0xE7
//#define MCP47X6_VREF_VDD              0x00
//#define MCP47X6_VREF_VREFPIN	      0x10
//#define MCP47X6_VREF_VREFPIN_BUFFERED 0x18
//
//// Command definitioins
//#define MCP47X6_CMD_MASK       0x1F
//#define MCP47X6_CMD_VOLDAC     0x00
//#define MCP47X6_CMD_VOLALL     0x40
//#define MCP47X6_CMD_VOLCONFIG  0x80
//#define MCP47X6_CMD_ALL        0x60
//
//
//class MCP4716
//{
// protected:
//
//
// public:
//	MCP4716(void);
//	bool init(int address, int vref, int gain);
//
//	void setGain(unsigned char gain);
//	void setVReference(unsigned char vref);
//
//	bool setOutputLevel(unsigned int level);
//
// private:
//	bool writeConfigReg(unsigned char theConfig);
//	unsigned char _address;
//	unsigned char _vref;
//	unsigned char _gain;
//	unsigned char _config;
//
//};

#endif

