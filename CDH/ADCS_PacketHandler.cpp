// 
// 
// 

#include "ADCS_PacketHandler.h"
#include "LoRaCmd_handler.h"

static PE_Info adcs_pei_;

static void ADCS_init_(void);
static void ADCS_packet_handler_(void);

static void ADCS_cmd_forward_init_(void);
static void ADCS_cmd_forward_(void);

AppInfo ADCS_packet_handler(void)
{
	return create_app_info("ADCS_PH", 10, ADCS_init_, ADCS_packet_handler_);
}

static void ADCS_init_(void)
{
	adcs_pei_ = PE_initialize();
}

static void ADCS_packet_handler_(void)
{
	unsigned char rec_data[256];
	int  rec_len = 0;
	int i;
	int i_ret;
	if ((Serial_DataReceive(PORT_ADCS, rec_data, 0xFFFF, &rec_len)) == Success)
	{
		i_ret = PH_extract_packet(rec_data, rec_len, &adcs_pei_);

		if (i_ret == PH_FORWARDED_VIA_SF)
		{
			LoRa_TlmForward(adcs_pei_.packet);
		}
	}
}

AppInfo ADCS_cmd_forward(void)
{
	return create_app_info("ADCS_PT", 1, ADCS_cmd_forward_init_, ADCS_cmd_forward_);
}

static void ADCS_cmd_forward_init_(void)
{
	PL_clear_list(&to_adcs_cmd_list);
}

static void ADCS_cmd_forward_(void)
{
	CTCP *packet = NULL;
	if (PL_is_empty(&to_adcs_cmd_list)) { return; }
	packet = &(PL_get_head(&to_adcs_cmd_list)->packet);

	int i;
	for (i = 0; i < packet->length; i++)
	{
		ADCS_Serial.write(packet->data[i]);
	}

#ifdef MSG_DEBUG
	DEBUG_Serial.println("ADCS_CMD");
#endif

	PL_drop_head(&to_adcs_cmd_list);
}
