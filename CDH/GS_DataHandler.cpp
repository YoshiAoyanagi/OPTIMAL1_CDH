// 
// 
// 

#include "GS_DataHandler.h"
#include "AutonomousControl.h"

static void gs_data_handler_init_(void);
static void gs_data_handler_(void);
static void TlmCmd_Ack(CTCP* packet);
void GsCmdCountUp(void);
void GsCmdReset(void);

AppInfo gs_data_handler(void)
{
	return create_app_info("gsdata", 5, gs_data_handler_init_, gs_data_handler_);
}

static void gs_data_handler_init_(void)
{

}

static void gs_data_handler_(void)
{
	unsigned char rec_data[1024];
	int  rec_len = 0;
	int i;
	static PE_Info cmd_pei;

#ifdef UART_DEBUG
	if ((Serial_DataReceive(PORT_DBG, rec_data, 0xFFFF, &rec_len)) == Success)
	{
		GsCmdErrorSts = PH_extract_packet(rec_data, rec_len, &cmd_pei);

		GsCmdCountUp();
		GsCmdToId = CCP_get_cmd_to_id(&cmd_pei.packet);
		GsCmdCode = CCP_get_cmd_id(&cmd_pei.packet);
		if ((GsCmdErrorSts == PH_REGISTERED)
			|| (GsCmdErrorSts == PH_FORWARDED)
			|| (GsCmdErrorSts == PH_TLC_REGISTERD)
			|| (GsCmdErrorSts == PH_BC_REGISTERED))
		{
			GsCmdSts = CMD_STATUS_ACC;
		}
		else if (GsCmdErrorSts > CMD_ERROR_TLMCMD_ERROR)
		{
			GsCmdSts = CMD_STATUS_ERROR_CMD;
		}
		else
		{
			GsCmdSts = CMD_STATUS_END;
		}

		if (GsCmdToId == FROM_TO_COMM)
		{
			CdhCmdCount++;
			CdhCmdCount &= 0xFF;
			CdhCmdCode = CCP_get_cmd_id(&cmd_pei.packet);
		}

		TlmCmd_Ack(&cmd_pei.packet);
		GsCmdReset();
	}

#endif

	if ((Serial_DataReceive(PORT_DATA, rec_data, 0xFFFF, &rec_len)) == Success)
	{
		GsCmdErrorSts = PH_extract_packet(rec_data, rec_len, &cmd_pei);
		
		GsCmdCountUp();

		GsCmdToId = CCP_get_cmd_to_id(&cmd_pei.packet);
		GsCmdCode = CCP_get_cmd_id(&cmd_pei.packet);
		if ((GsCmdErrorSts == PH_REGISTERED)
			|| (GsCmdErrorSts == PH_FORWARDED)
			|| (GsCmdErrorSts == PH_TLC_REGISTERD)
			|| (GsCmdErrorSts == PH_BC_REGISTERED))
		{
			GsCmdSts = CMD_STATUS_ACC;
		}
		else if (GsCmdErrorSts > CMD_ERROR_TLMCMD_ERROR)
		{
			GsCmdSts = CMD_STATUS_ERROR_CMD;
		}
		else
		{
			GsCmdSts = CMD_STATUS_END;
		}


		if (GsCmdToId == FROM_TO_COMM)
		{
			CdhCmdCount++;
			CdhCmdCount &= 0xFF;
			CdhCmdCode = CCP_get_cmd_id(&cmd_pei.packet);
		}

		TlmCmd_Ack(&cmd_pei.packet);
		GsCmdReset();

		//地上からのコマンドかつエラーでなければNonCmdカウンタをクリアする
		if (CCP_get_cmd_from_id(&cmd_pei.packet) == FROM_TO_GS)
		{
			if (GsCmdErrorSts != PH_INVALID_LENGTH
				&& GsCmdErrorSts != PH_INVALID_CRC)
			{
				Clear_NonCmd_counter();
			}
		}
	}
}

static void TlmCmd_Ack(CTCP* packet)
{
	GsTlmDsc = TLM_DSC_ACK;
	OBC_TlmRegister(TLM_ID_01);
	return;
}

void GsCmdCountUp(void)
{
	GsCmdCount++;
	if (GsCmdCount > 0xFF)
		GsCmdCount = 0;

	return;
}

void GsCmdReset(void)
{
	GsTlmDsc = TLM_DSC_HK;
	GsCmdSts = CMD_STATUS_NONE;
	return;
}