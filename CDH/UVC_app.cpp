// 
// 
// 

#include "UVC_app.h"

#include <stddef.h> // for NULL
#include <string.h> // for memcpy

#include "cmd_analyze_obc.h"
#include "BlockCommandDefinisions.h"
#include "utility.h"
#include "EPS.h"

UVCInfo uvci;

static void UVC_init_(void);
static void UVC_execute_(void);

AppInfo UVC_create_app(void)
{
	return create_app_info("uvc", 5, UVC_init_, UVC_execute_);
}

static void UVC_init_(void)
{
	ENABLE_STS ena = DISABLE;
	unsigned char v1, v2, v3;
	uvci.level = UVC_SUCCESS;
	uvci.det_count = UVC_DEFAULT_DET_COUNT;

	FRAM_Read_uvc_ena(&ena);
	FRAM_Read_uvc_threshold(&v1, &v2, &v3);

	uvci.is_enabled = (unsigned char)ena;
	if (UVC_set(v1, v2, v3) != 0)
	{
		uvci.threshold[0] = UVC_DEFAULT_THRESHOLD_RTN;
		uvci.threshold[1] = UVC_DEFAULT_THRESHOLD_LV1;
		uvci.threshold[2] = UVC_DEFAULT_THRESHOLD_LV2;
	}
}

int	UVC_set(unsigned char rtn_v, unsigned char lv1_v, unsigned char lv2_v)
{
	int i_ret = 0;

	if (rtn_v == 0 || lv1_v == 0 || lv2_v == 0)
		return (-1);

	if (rtn_v < lv1_v || lv1_v < lv2_v)
		return (-1);


	uvci.threshold[0] = rtn_v;
	uvci.threshold[1] = lv1_v;
	uvci.threshold[2] = lv2_v;

	FRAM_Write_uvc_threshold(rtn_v, lv1_v, lv2_v);
	return i_ret;
}

void UVC_enable(unsigned char en)
{
	uvci.is_enabled = en;
	uvci.level = UVC_SUCCESS;
	FRAM_Write_uvc_ena((ENABLE_STS)en);
}

static void UVC_execute_(void)
{
	static unsigned char count_uvc_1;
	static unsigned char count_uvc_2;
	static unsigned char count_uvc_rtn;

	if (uvci.is_enabled)
	{
		if (eps_bat_v < ((float)uvci.threshold[2] / 10) && (uvci.level == UVC_LEVEL1))
		{
			if (count_uvc_1 > uvci.det_count)
			{
				uvci.level = UVC_LEVEL2;
				AL_add_anomaly(AL_UVC, UVC_LEVEL2);
			}
			count_uvc_1++;
		}
		else if (eps_bat_v < ((float)uvci.threshold[1] / 10) && ((uvci.level != UVC_LEVEL2) && (uvci.level != UVC_LEVEL1)))
		{
			if (count_uvc_2 > uvci.det_count)
			{
				uvci.level = UVC_LEVEL1;
				AL_add_anomaly(AL_UVC, UVC_LEVEL1);
			}
			count_uvc_2++;
		}
		else if (eps_bat_v > ((float)uvci.threshold[0] / 10) && ((uvci.level == UVC_LEVEL1) || (uvci.level == UVC_LEVEL2)))
		{
			if (count_uvc_rtn > uvci.det_count)
			{
				uvci.level = UVC_RETURN;
				AL_add_anomaly(AL_UVC, UVC_RETURN);
			}
			count_uvc_rtn++;
		}
		else
		{
			count_uvc_1 = 0;
			count_uvc_2 = 0;
			count_uvc_rtn = 0;
		}
	}
}
