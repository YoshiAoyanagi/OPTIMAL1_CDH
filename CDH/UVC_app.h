// UVC_app.h

#ifndef _UVC_APP_h
#define _UVC_APP_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "AppInfo.h"
#include "CommonTlmCmdPacket.h"

#define UVC_DEFAULT_THRESHOLD_RTN	70
#define UVC_DEFAULT_THRESHOLD_LV1	65
#define UVC_DEFAULT_THRESHOLD_LV2	63
#define UVC_DEFAULT_DET_COUNT		10

typedef enum
{
	UVC_SUCCESS,
	UVC_LEVEL1,
	UVC_LEVEL2,
	UVC_RETURN,
	UVC_UNKNOWN
} UVC_ACK;

typedef struct
{
	unsigned char is_enabled;
	unsigned char threshold[3];
	unsigned char det_count;
	unsigned char level;
} UVCInfo;

extern UVCInfo uvci;

AppInfo UVC_create_app(void);
int	UVC_set(unsigned char rtn_v, unsigned char lv1_v, unsigned char lv2_v);
void UVC_enable(unsigned char en);

#endif

