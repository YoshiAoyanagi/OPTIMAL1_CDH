#define _TLM_FRAME_MAIN

#include <stddef.h> 
#include "tlm_frame.h"
#include "CommonSerialReceiver.h"
#include "SerialTransmitter.h"
#include "TlmDispatcher.h"

Reed_SolomonClass reed_solomon;

void TlmCmd_init_code_den(void){ 
	tlm_packet_ptr[1] = TLM_OBC_01;
	tlm_packet_ptr[2] = TLM_OBC_02;
	tlm_packet_ptr[3] = TLM_OBC_03;
	tlm_packet_ptr[4] = TLM_OBC_04;
	tlm_packet_ptr[5] = TLM_OBC_05;
	tlm_packet_ptr[7] = TLM_OBC_07;
	tlm_packet_ptr[8] = TLM_OBC_08;
	tlm_packet_ptr[9] = TLM_OBC_09;
	tlm_packet_ptr[60] = TLM_OBC_60;
	tlm_packet_ptr[61] = TLM_OBC_61;
	tlm_packet_ptr[62] = TLM_OBC_62;
	tlm_packet_ptr[63] = TLM_OBC_63;
	tlm_packet_ptr[64] = TLM_OBC_64;

  return;
}

int  TlmCmd_init(void)
{
  int i;
  
  for(i = 0; i < (TLM_MAX_NUMBER + 1); i++){
    tlm_packet_ptr[i] = 0;
    }
    TlmCmd_init_code_den();
	SerialTransmit_init();
	reed_solomon.init();

    return(0);
}

void TlmHeader(unsigned char id, TLM_MODE_DR tlm_mode)
{
	unsigned int now_time = TMGR_get_utime();
	unsigned char tlm_count;
	obc_tlm_millis = millis();
	obc_tlm_master_cycle = TMGR_get_master_cycle();

  // TLM ID
  TLM_ID = id;
  // Telemetry Count
  if (tlm_mode == TLM_DR)
  {
	  DrTlmCount[id]++;
	  DrTlmCount[id] &= 0xFF;
	  tlm_count = DrTlmCount[id];
  }
  else
  {
	  TlmCount[id]++;
	  TlmCount[id] &= 0xFF;
	  tlm_count = TlmCount[id];
  }
  // Make Telemetry Packet
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_STX1]			= TLM_STX_1;
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_STX2]			= TLM_STX_2;
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_DISCRIMINATE]	= TLMCMD_TLM;
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_TLM_MODE]		= TLM_ID;
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_TLM_COUNT]		= tlm_count;
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_TLM_TIME + 0]	= (unsigned char)((now_time >> 24) & 0xff);
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_TLM_TIME + 1]	= (unsigned char)((now_time >> 16) & 0xff);
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_TLM_TIME + 2]	= (unsigned char)((now_time >> 8) & 0xff);
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_TLM_TIME + 3]	= (unsigned char)(now_time & 0xff);
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_CMD_CODE]		= (unsigned char)(GsCmdCode & 0xff);
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_CMD_STAT]		= (unsigned char)(GsCmdSts & 0xFF);
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_CMD_ERR_STAT]	= (unsigned char)(GsCmdErrorSts & 0xFF);
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_CMD_COUNT]		= (unsigned char)(GsCmdCount & 0xff);
}

void	TlmFooter(int	ofs)
{
  unsigned int	ret_crc;
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_LENGTH]  = (unsigned char)((TLM_BYTE_COUNT + ofs - 7) & 0xff);
  ret_crc = crc(&(TlmTransferFrame.Packet.packet_data[TLM_POSITION_DISCRIMINATE]), TLM_BYTE_COUNT + ofs - 7);
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_CRC_H + ofs] = (unsigned char)((ret_crc >> 8) & 0xFF);	// CRC H
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_CRC_L + ofs] = (unsigned char)((ret_crc >> 0) & 0xFF);	// CRC L
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_ETX1 + ofs] = TLM_ETX_1;
  TlmTransferFrame.Packet.packet_data[TLM_POSITION_ETX2 + ofs] = TLM_ETX_2;
}

void  AttachRScode(int ofs)
{
  int i;
  int c = 0;
  unsigned char data[DATA_PACKET_SIZE + 4];
  unsigned char code[REED_SOLOMON_SIZE];
  unsigned int	rs_size;

  for (i = 0; i < 4; i++)
  {
	  data[c] = (SYNC_MARKER >> (24 - i * 8)) & 0xFF;
	  c++;
  }
  for(i = 0; i < (DATA_PACKET_SIZE); i++)
  {
	  data[c] = TlmTransferFrame.Packet.packet_data[i];
	  c++;
  }
  reed_solomon.encode(data, ofs, code, &rs_size);

  for (i = 0; i < REED_SOLOMON_SIZE; i++)
	  TlmTransferFrame.Packet.rs_code[i] = code[i];
  
  return;
}

int TlmSend(int ofs)
{
  int	i;
  int	i_ret;
  int c = 0;

  unsigned int tf_size = TF_SIZE;

  AttachRScode(DATA_PACKET_SIZE + 4);

  unsigned char RandData[TF_SIZE];
  unsigned char Async;
  unsigned char ByteStream[TF_SIZE + 4 + 1];	//255byte + dummy1byte
  Tlm_PN_rand(TlmTransferFrame.TransferFrame.data, RandData, tf_size);



  //SyncMarker
  for (i = 0; i < 4; i++)
  {
	  Async = (SYNC_MARKER >> (24 - i * 8)) & 0xFF;
#ifdef UART_DEBUG
	  DEBUG_Serial.write(Async);
#endif
  }

  ByteStream[c] = 0x1A; c++;
  ByteStream[c] = 0xCF; c++;
  ByteStream[c] = 0xFC; c++;
  ByteStream[c] = 0x1D; c++;

  //Transmit data
  for (i = 0; i < tf_size; i++)
  {
	  ByteStream[c] = RandData[i];
	  c++;
#ifdef UART_DEBUG
	  DEBUG_Serial.write(RandData[i]);
#endif
  }

#ifndef UART_DEBUG
  //STX ON‚Å‚ ‚ê‚Î‘—M
  if (pwr_sts.stx == PWR_ON)
  {
	  i_ret = SerialTransmit(ByteStream, TF_SIZE + 4 + 1);	//add G-SAT2
	  return i_ret;
  }
#endif

  return 0;
}

int  TlmPacket_combining(unsigned char tlm_id, int (*TlmDataSet_p)(void), TLM_MODE_DR tlm_mode){
  int	ofs;
  
  if(TlmDataSet_p == 0){
    Serial.print("Tlm Data Set Function Pointer Error\n");
    return(-2);
  }
  
  TlmHeader(tlm_id, tlm_mode);
  
  if((ofs = (*TlmDataSet_p)()) < 0)
  {
    Serial.print("Tlm Data Set Error\n");
    return(-1);
  }
  TlmFooter(ofs);
  return (ofs);
}

void Tlm_PN_rand(unsigned char *data, unsigned char *rand_data, unsigned int len)
{
  int i;
  int pn_count = 0;
  
  unsigned char pn_code[28] =
  {0xFF,0x48,0x0E,0xC0,0x9A,0x0D,0x70,0xBC,0x8E,0x2C,0x93,0xAD,0xA7,0xB7,  //1
   0x46,0xCE,0x5A,0x97,0x7D,0xCC,0x32,0xA2,0xBF,0x3E,0x0A,0x10,0xF1,0x88,  //2
  };

  for(i = 0; i < len; i++)
  {
    if(pn_count > 27)
      pn_count = 0;
      
    rand_data[i] = data[i]^pn_code[pn_count];
    pn_count++;
  }
  return;
}

int  MakeTlmPacket(unsigned char tlm_id, TLM_MODE_DR tlm_mode)
{
  int	(*TlmDataSet_p)(void);
  
  TlmDataSet_p = NULL;

  if (tlm_id == TLM_ID_01)
  {
	  TlmDataSet_p = TLM_OBC_01;
  }
  else if (tlm_id == TLM_ID_02)
  {
	  TlmDataSet_p = TLM_OBC_02;
  }
  else if (tlm_id == TLM_ID_03)
  {
	  TlmDataSet_p = TLM_OBC_03;
  }
  else if (tlm_id == TLM_ID_04)
  {
	  TlmDataSet_p = TLM_OBC_04;
  }
  else if (tlm_id == TLM_ID_05)
  {
	  TlmDataSet_p = TLM_OBC_05;
  }
  else if (tlm_id == TLM_ID_07)
  {
	  TlmDataSet_p = TLM_OBC_07;
  }
  else if (tlm_id == TLM_ID_08)
  {
	  TlmDataSet_p = TLM_OBC_08;
  }
  else if (tlm_id == TLM_ID_09)
  {
	  TlmDataSet_p = TLM_OBC_09;
  }
  else if (tlm_id == TLM_ID_60)
  {
	  TlmDataSet_p = TLM_OBC_60;
  }
  else if (tlm_id == TLM_ID_61)
  {
	  TlmDataSet_p = TLM_OBC_61;
  }
  else if (tlm_id == TLM_ID_62)
  {
	  TlmDataSet_p = TLM_OBC_62;
  }
  else if (tlm_id == TLM_ID_63)
  {
	  TlmDataSet_p = TLM_OBC_63;
  }
  else if (tlm_id == TLM_ID_64)
  {
	  TlmDataSet_p = TLM_OBC_64;
  }
  return (TlmPacket_combining(tlm_id, TlmDataSet_p, tlm_mode));
}


int TLM_OBC_01(void) {
	unsigned char* p;

	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0] = (unsigned char)(GsCmdToId & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 1] = (unsigned char)(GsTlmDsc & 0xFF);  //	
	p = (unsigned char*)&(obc_tlm_master_cycle)+3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(rtc_time.unix) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 6 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 6 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 6 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 6 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10] = (unsigned char)(ops_mode->stat & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 11] = (unsigned char)(ops_mode->current_id & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12] = (unsigned char)(ops_mode->previous_id & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 13] = (unsigned char)(serial_mux_enable_port & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 14] = (unsigned char)((*((unsigned char*)&(tl_cmd_list.pl_public.active_nodes))) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 15] = (unsigned char)((unsigned char)fram_tlc_queue & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16] = (unsigned char)((*((unsigned char*)&(bcti->pos.block))) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 17] = (unsigned char)((*((unsigned char*)&(bcti->pos.cmd))) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 18] = (unsigned char)((unsigned char)Stored_BC_Number & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 19] = (unsigned char)((unsigned char)Stored_BC_Position & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20] = (unsigned char)(pwr_sts.msn_unreg & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 21] = (unsigned char)(pwr_sts.heater & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 22] = (unsigned char)(pwr_sts.rw & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 23] = (unsigned char)(pwr_sts.adcs & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24] = (unsigned char)(pwr_sts.msn5v & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 25] = (unsigned char)(pwr_sts.subcam & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 26] = (unsigned char)(pwr_sts.sf & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 27] = (unsigned char)(pwr_sts.msn_unreg_en & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28] = (unsigned char)(pwr_sts.rw_en & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 29] = (unsigned char)(pwr_sts.aqu_5v & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 30] = (unsigned char)(pwr_sts.ovco & 0xFF);  //	
	p = (unsigned char*)&(eps_bat_v)+3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 35] = (unsigned char)(pdu_analog_tlm.rw_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36] = (unsigned char)(pdu_analog_tlm.rw_voltage.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 37] = (unsigned char)(pdu_analog_tlm.adcs_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 38] = (unsigned char)(pdu_analog_tlm.adcs_voltage.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 39] = (unsigned char)(pdu_analog_tlm.msn5v_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40] = (unsigned char)(pdu_analog_tlm.msn5v_voltage.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 41] = (unsigned char)(pdu_analog_tlm.subcam_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 42] = (unsigned char)(pdu_analog_tlm.subcam_voltage.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 43] = (unsigned char)(pdu_analog_tlm.sf_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44] = (unsigned char)(pdu_analog_tlm.sf_voltage.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 45] = (unsigned char)(pdu_analog_tlm.msn_unreg_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 46] = (unsigned char)(pdu_analog_tlm.msn_unreg_voltage.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 47] = (unsigned char)(pdu_analog_tlm.strx_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48] = (unsigned char)(pdu_analog_tlm.strx_voltage.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 49] = (unsigned char)(pdu_analog_tlm.heater_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 50] = (unsigned char)(pdu_analog_tlm.heater_voltage.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 51] = (unsigned char)(pcu_analog_tlm.sap1_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52] = (unsigned char)(pcu_analog_tlm.sap2_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 53] = (unsigned char)(pcu_analog_tlm.sap3_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 54] = (unsigned char)(pcu_analog_tlm.sap4_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 55] = (unsigned char)(pcu_analog_tlm.sap_v1.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56] = (unsigned char)(pcu_analog_tlm.sap_v2.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 57] = (unsigned char)(pcu_analog_tlm.batt1_v.byte_value & 0xFF);  //	
	p = (unsigned char*)&(pcu_analog_tlm.batt1_current.float_value) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(pcu_analog_tlm.batt2_current.float_value) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 62 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 62 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 62 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 62 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 66] = (unsigned char)(pcu_analog_tlm.dcdc1_current.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 67] = (unsigned char)(uvci.is_enabled & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68] = (unsigned char)(uvci.level & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 69] = (unsigned char)(uvci.threshold[0] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 70] = (unsigned char)(uvci.threshold[1] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 71] = (unsigned char)(uvci.threshold[2] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72] = (unsigned char)(pcu_select & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 73] = (unsigned char)(temp_sens.an_tlm[0].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 74] = (unsigned char)(temp_sens.an_tlm[1].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 75] = (unsigned char)(temp_sens.an_tlm[2].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76] = (unsigned char)(temp_sens.an_tlm[3].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 77] = (unsigned char)(temp_sens.an_tlm[4].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 78] = (unsigned char)(temp_sens.an_tlm[5].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 79] = (unsigned char)(temp_sens.an_tlm[6].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80] = (unsigned char)(temp_sens.an_tlm[7].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 81] = (unsigned char)(temp_sens.an_tlm[8].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 82] = (unsigned char)(temp_sens.an_tlm[12].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 83] = (unsigned char)(temp_sens.an_tlm[13].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84] = (unsigned char)(temp_sens.an_tlm[14].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 85] = (unsigned char)(hk_dr.enabled & 0xFF);  //	
	p = (unsigned char*)&(hk_dr.written_sector_size) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 86 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 86 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(hk_dr.written_packet_size) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 90] = (unsigned char)(hk_dr.replay_enabled & 0xFF);  //	
	p = (unsigned char*)&(hk_dr.replay_pointer.sector) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 91 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 91 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(hk_dr.replay_pointer.packet_address) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 93 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 93 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(hk_dr.replay_interval_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 95 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 95 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(tlm_send_interval)+1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 97 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 97 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(st_tlm_send_interval)+1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 99 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 99 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 101] = (unsigned char)(msn_dr.enabled & 0xFF);  //	
	p = (unsigned char*)&(msn_dr.written_sector_size) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 102 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 102 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(msn_dr.written_packet_size) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 106] = (unsigned char)(msn_dr.replay_enabled & 0xFF);  //	
	p = (unsigned char*)&(msn_dr.replay_pointer.sector) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 107 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 107 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(msn_dr.replay_pointer.packet_address) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 109 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 109 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(msn_dr.replay_interval_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 111 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 111 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 113] = (unsigned char)(pwr_sts.stx & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 114] = (unsigned char)(pwr_sts.srx & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 115] = (unsigned char)(pwr_sts.srxif & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116] = (unsigned char)(ad_stx_sts.use_port & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 117] = (unsigned char)(ad_stx_sts.bitrate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 118] = (unsigned char)(ad_stx_sts.set_bitrate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 119] = (unsigned char)(ad_stx_sts.set_outport & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120] = (unsigned char)(ad_stx_sts.set_power & 0xFF);  //	
	p = (unsigned char*)&(ad_stx_sts.tmpe) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 121 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 121 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 121 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 121 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(ad_srx_sts.agc_inc) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 125 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 125 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 125 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 125 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(ad_srx_sts.agc_coh) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 129 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 129 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 129 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 129 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(ad_srx_sts.freq_err) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 133 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 133 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 133 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 133 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 137] = (unsigned char)(ad_srx_sts.carrier_lock & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 138] = (unsigned char)(ad_srx_sts.sub_carrier_lock & 0xFF);  //	
	p = (unsigned char*)&(stx_transmit_on_duration)+1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 139 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 139 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(stx_transmit_off_duration)+1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 141 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 141 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(stx_burst_duration)+1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 143 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 143 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(stx_on_remaining)+1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 145 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 145 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 147] = (unsigned char)(strx_otc_enable & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 148] = (unsigned char)(strx_otc_threshold & 0xFF);  //	
	p = (unsigned char*)&(GYRO_Rate_body[0]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 149 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 149 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 149 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 149 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(GYRO_Rate_body[1]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 153 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 153 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 153 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 153 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(GYRO_Rate_body[2]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 157 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 157 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 157 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 157 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(auto_ctrl.restart_non_cmd_counter) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 161 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 161 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(auto_ctrl.restart_counter) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 163 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 163 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 163 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 163 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 167] = (unsigned char)(ahi->respond_at.master & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 168] = (unsigned char)((((unsigned char*)&(ahi->respond_at.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 169] = (unsigned char)((((unsigned char*)&(ahi->latest_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 170] = (unsigned char)((((unsigned char*)&(ahi->elements[0].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 171] = (unsigned char)((((unsigned char*)&(ahi->elements[1].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 172] = (unsigned char)((((unsigned char*)&(ahi->elements[2].is_active))[0]) & 0xFF);  //	
	return(173);
}



int TLM_OBC_02(void) {
	unsigned char* p;

	p = (unsigned char*)&(fram_tlc_time[0]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[1]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[2]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[3]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[4]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[5]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[6]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[7]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[8]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[9]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[10]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[11]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[12]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[13]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[14]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[15]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[16]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[17]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[18]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[19]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[20]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[21]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[22]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[23]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[24]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[25]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[26]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[27]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[28]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[29]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[30]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[31]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[32]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[33]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 132 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 132 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 132 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 132 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[34]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 136 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 136 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 136 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 136 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[35]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 140 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 140 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 140 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 140 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[36]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 144 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 144 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 144 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 144 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[37]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 148 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 148 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 148 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 148 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[38]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 152 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 152 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 152 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 152 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[39]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 156 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 156 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 156 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 156 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[40]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 160 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 160 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 160 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 160 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[41]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 164 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 164 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 164 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 164 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[42]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 168 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 168 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 168 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 168 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[43]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 172 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 172 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 172 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 172 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[44]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 176 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 176 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 176 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 176 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[45]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 180 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 180 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 180 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 180 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[46]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 184 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 184 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 184 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 184 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[47]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 188 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 188 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 188 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 188 + 3] = (unsigned char)(*p);	p--;
	return(192);
}



int TLM_OBC_03(void) {
	unsigned char* p;

	p = (unsigned char*)&(fram_tlc_time[48]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[49]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[50]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[51]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[52]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[53]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[54]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[55]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[56]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[57]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[58]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[59]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[60]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[61]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[62]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(fram_tlc_time[63]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 3] = (unsigned char)(*p);	p--;
	return(64);
}



int TLM_OBC_04(void) {
	unsigned char* p;

	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0] = (unsigned char)(Stored_BC_Param.bc_id & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 1] = (unsigned char)(Stored_BC_Param.is_active & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2] = (unsigned char)(Stored_BC_Param.to_id[0] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 3] = (unsigned char)(Stored_BC_Param.cmd_id[0] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[0]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8] = (unsigned char)(Stored_BC_Param.to_id[1] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 9] = (unsigned char)(Stored_BC_Param.cmd_id[1] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[1]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 14] = (unsigned char)(Stored_BC_Param.to_id[2] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 15] = (unsigned char)(Stored_BC_Param.cmd_id[2] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[2]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20] = (unsigned char)(Stored_BC_Param.to_id[3] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 21] = (unsigned char)(Stored_BC_Param.cmd_id[3] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[3]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 22 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 22 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 22 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 22 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 26] = (unsigned char)(Stored_BC_Param.to_id[4] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 27] = (unsigned char)(Stored_BC_Param.cmd_id[4] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[4]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32] = (unsigned char)(Stored_BC_Param.to_id[5] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 33] = (unsigned char)(Stored_BC_Param.cmd_id[5] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[5]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 34 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 34 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 34 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 34 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 38] = (unsigned char)(Stored_BC_Param.to_id[6] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 39] = (unsigned char)(Stored_BC_Param.cmd_id[6] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[6]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44] = (unsigned char)(Stored_BC_Param.to_id[7] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 45] = (unsigned char)(Stored_BC_Param.cmd_id[7] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[7]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 46 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 46 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 46 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 46 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 50] = (unsigned char)(Stored_BC_Param.to_id[8] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 51] = (unsigned char)(Stored_BC_Param.cmd_id[8] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[8]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56] = (unsigned char)(Stored_BC_Param.to_id[9] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 57] = (unsigned char)(Stored_BC_Param.cmd_id[9] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[9]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 62] = (unsigned char)(Stored_BC_Param.to_id[10] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 63] = (unsigned char)(Stored_BC_Param.cmd_id[10] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[10]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68] = (unsigned char)(Stored_BC_Param.to_id[11] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 69] = (unsigned char)(Stored_BC_Param.cmd_id[11] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[11]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 70 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 70 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 70 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 70 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 74] = (unsigned char)(Stored_BC_Param.to_id[12] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 75] = (unsigned char)(Stored_BC_Param.cmd_id[12] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[12]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80] = (unsigned char)(Stored_BC_Param.to_id[13] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 81] = (unsigned char)(Stored_BC_Param.cmd_id[13] & 0xFF);  //	
	p = (unsigned char*)&(Stored_BC_Param.cycle[13]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 82 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 82 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 82 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 82 + 3] = (unsigned char)(*p);	p--;
	return(86);
}



int TLM_OBC_05(void) {
	unsigned char* p;

	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0] = (unsigned char)(Stored_BC_Number & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 1] = (unsigned char)(Stored_BC_Position & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2] = (unsigned char)(Stored_BC_Data[0] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 3] = (unsigned char)(Stored_BC_Data[1] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4] = (unsigned char)(Stored_BC_Data[2] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 5] = (unsigned char)(Stored_BC_Data[3] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 6] = (unsigned char)(Stored_BC_Data[4] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 7] = (unsigned char)(Stored_BC_Data[5] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8] = (unsigned char)(Stored_BC_Data[6] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 9] = (unsigned char)(Stored_BC_Data[7] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10] = (unsigned char)(Stored_BC_Data[8] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 11] = (unsigned char)(Stored_BC_Data[9] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12] = (unsigned char)(Stored_BC_Data[10] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 13] = (unsigned char)(Stored_BC_Data[11] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 14] = (unsigned char)(Stored_BC_Data[12] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 15] = (unsigned char)(Stored_BC_Data[13] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16] = (unsigned char)(Stored_BC_Data[14] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 17] = (unsigned char)(Stored_BC_Data[15] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 18] = (unsigned char)(Stored_BC_Data[16] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 19] = (unsigned char)(Stored_BC_Data[17] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20] = (unsigned char)(Stored_BC_Data[18] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 21] = (unsigned char)(Stored_BC_Data[19] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 22] = (unsigned char)(Stored_BC_Data[20] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 23] = (unsigned char)(Stored_BC_Data[21] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24] = (unsigned char)(Stored_BC_Data[22] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 25] = (unsigned char)(Stored_BC_Data[23] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 26] = (unsigned char)(Stored_BC_Data[24] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 27] = (unsigned char)(Stored_BC_Data[25] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28] = (unsigned char)(Stored_BC_Data[26] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 29] = (unsigned char)(Stored_BC_Data[27] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 30] = (unsigned char)(Stored_BC_Data[28] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31] = (unsigned char)(Stored_BC_Data[29] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32] = (unsigned char)(Stored_BC_Data[30] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 33] = (unsigned char)(Stored_BC_Data[31] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 34] = (unsigned char)(Stored_BC_Data[32] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 35] = (unsigned char)(Stored_BC_Data[33] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36] = (unsigned char)(Stored_BC_Data[34] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 37] = (unsigned char)(Stored_BC_Data[35] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 38] = (unsigned char)(Stored_BC_Data[36] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 39] = (unsigned char)(Stored_BC_Data[37] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40] = (unsigned char)(Stored_BC_Data[38] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 41] = (unsigned char)(Stored_BC_Data[39] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 42] = (unsigned char)(Stored_BC_Data[40] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 43] = (unsigned char)(Stored_BC_Data[41] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44] = (unsigned char)(Stored_BC_Data[42] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 45] = (unsigned char)(Stored_BC_Data[43] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 46] = (unsigned char)(Stored_BC_Data[44] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 47] = (unsigned char)(Stored_BC_Data[45] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48] = (unsigned char)(Stored_BC_Data[46] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 49] = (unsigned char)(Stored_BC_Data[47] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 50] = (unsigned char)(Stored_BC_Data[48] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 51] = (unsigned char)(Stored_BC_Data[49] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52] = (unsigned char)(Stored_BC_Data[50] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 53] = (unsigned char)(Stored_BC_Data[51] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 54] = (unsigned char)(Stored_BC_Data[52] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 55] = (unsigned char)(Stored_BC_Data[53] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56] = (unsigned char)(Stored_BC_Data[54] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 57] = (unsigned char)(Stored_BC_Data[55] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58] = (unsigned char)(Stored_BC_Data[56] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 59] = (unsigned char)(Stored_BC_Data[57] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60] = (unsigned char)(Stored_BC_Data[58] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 61] = (unsigned char)(Stored_BC_Data[59] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 62] = (unsigned char)(Stored_BC_Data[60] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 63] = (unsigned char)(Stored_BC_Data[61] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64] = (unsigned char)(Stored_BC_Data[62] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 65] = (unsigned char)(Stored_BC_Data[63] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 66] = (unsigned char)(Stored_BC_Data[64] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 67] = (unsigned char)(Stored_BC_Data[65] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68] = (unsigned char)(Stored_BC_Data[66] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 69] = (unsigned char)(Stored_BC_Data[67] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 70] = (unsigned char)(Stored_BC_Data[68] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 71] = (unsigned char)(Stored_BC_Data[69] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72] = (unsigned char)(Stored_BC_Data[70] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 73] = (unsigned char)(Stored_BC_Data[71] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 74] = (unsigned char)(Stored_BC_Data[72] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 75] = (unsigned char)(Stored_BC_Data[73] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76] = (unsigned char)(Stored_BC_Data[74] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 77] = (unsigned char)(Stored_BC_Data[75] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 78] = (unsigned char)(Stored_BC_Data[76] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 79] = (unsigned char)(Stored_BC_Data[77] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80] = (unsigned char)(Stored_BC_Data[78] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 81] = (unsigned char)(Stored_BC_Data[79] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 82] = (unsigned char)(Stored_BC_Data[80] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 83] = (unsigned char)(Stored_BC_Data[81] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84] = (unsigned char)(Stored_BC_Data[82] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 85] = (unsigned char)(Stored_BC_Data[83] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 86] = (unsigned char)(Stored_BC_Data[84] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 87] = (unsigned char)(Stored_BC_Data[85] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88] = (unsigned char)(Stored_BC_Data[86] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 89] = (unsigned char)(Stored_BC_Data[87] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 90] = (unsigned char)(Stored_BC_Data[88] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 91] = (unsigned char)(Stored_BC_Data[89] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92] = (unsigned char)(Stored_BC_Data[90] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 93] = (unsigned char)(Stored_BC_Data[91] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 94] = (unsigned char)(Stored_BC_Data[92] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 95] = (unsigned char)(Stored_BC_Data[93] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96] = (unsigned char)(Stored_BC_Data[94] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 97] = (unsigned char)(Stored_BC_Data[95] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 98] = (unsigned char)(Stored_BC_Data[96] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 99] = (unsigned char)(Stored_BC_Data[97] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100] = (unsigned char)(Stored_BC_Data[98] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 101] = (unsigned char)(Stored_BC_Data[99] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 102] = (unsigned char)(Stored_BC_Data[100] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 103] = (unsigned char)(Stored_BC_Data[101] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104] = (unsigned char)(Stored_BC_Data[102] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 105] = (unsigned char)(Stored_BC_Data[103] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 106] = (unsigned char)(Stored_BC_Data[104] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 107] = (unsigned char)(Stored_BC_Data[105] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108] = (unsigned char)(Stored_BC_Data[106] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 109] = (unsigned char)(Stored_BC_Data[107] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 110] = (unsigned char)(Stored_BC_Data[108] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 111] = (unsigned char)(Stored_BC_Data[109] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112] = (unsigned char)(Stored_BC_Data[110] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 113] = (unsigned char)(Stored_BC_Data[111] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 114] = (unsigned char)(Stored_BC_Data[112] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 115] = (unsigned char)(Stored_BC_Data[113] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116] = (unsigned char)(Stored_BC_Data[114] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 117] = (unsigned char)(Stored_BC_Data[115] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 118] = (unsigned char)(Stored_BC_Data[116] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 119] = (unsigned char)(Stored_BC_Data[117] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120] = (unsigned char)(Stored_BC_Data[118] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 121] = (unsigned char)(Stored_BC_Data[119] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 122] = (unsigned char)(Stored_BC_Data[120] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 123] = (unsigned char)(Stored_BC_Data[121] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124] = (unsigned char)(Stored_BC_Data[122] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 125] = (unsigned char)(Stored_BC_Data[123] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 126] = (unsigned char)(Stored_BC_Data[124] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 127] = (unsigned char)(Stored_BC_Data[125] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128] = (unsigned char)(Stored_BC_Data[126] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 129] = (unsigned char)(Stored_BC_Data[127] & 0xFF);  //	
	return(130);
}



int TLM_OBC_07(void) {
	unsigned char* p;

	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0] = (unsigned char)(GsCmdToId & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 1] = (unsigned char)(serial_mux_enable_port & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2] = (unsigned char)((unsigned char)fram_tlc_queue & 0xFF);  //	
	p = (unsigned char*)&(eps_bat_v)+3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 3 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 3 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 3 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 3 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 7] = (unsigned char)(pcu_analog_tlm.sap_v1.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8] = (unsigned char)(pcu_analog_tlm.sap_v2.byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 9] = (unsigned char)(temp_sens.an_tlm[3].byte_value & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10] = (unsigned char)(temp_sens.an_tlm[4].byte_value & 0xFF);  //	
	p = (unsigned char*)&(ad_srx_sts.agc_inc) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 11 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 11 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 11 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 11 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 15] = (unsigned char)(ad_srx_sts.carrier_lock & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16] = (unsigned char)(ad_srx_sts.sub_carrier_lock & 0xFF);  //	
	return(17);
}



int TLM_OBC_08(void) {
	unsigned char* p;

	p = (unsigned char*)&(strx_carrierlock_reply_time)+1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(strx_subcarrierlock_reply_time)+1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2 + 1] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(strx_rssi_threshold)+3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8] = (unsigned char)(mtq_sts.x & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 9] = (unsigned char)(mtq_sts.y & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10] = (unsigned char)(mtq_sts.z & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 11] = (unsigned char)(tlmd_anomaly_detect_enable & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12] = (unsigned char)(hk_dr.ena_timetag & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 13] = (unsigned char)(msn_dr.ena_timetag & 0xFF);  //	
	return(14);
}



int TLM_OBC_09(void) {
	unsigned char* p;

	p = (unsigned char*)&(GYRO_Rate_body[0]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(GYRO_Rate_body[1]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(GYRO_Rate_body[2]) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 3] = (unsigned char)(*p);	p--;
	return(12);
}



int TLM_OBC_60(void) {
	unsigned char* p;

	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0] = (unsigned char)((((unsigned char*)&(ahi->elements[0].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 1] = (unsigned char)((((unsigned char*)&(ahi->elements[0].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2] = (unsigned char)((((unsigned char*)&(ahi->elements[0].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 3] = (unsigned char)((((unsigned char*)&(ahi->elements[0].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4] = (unsigned char)((((unsigned char*)&(ahi->elements[0].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 5] = (unsigned char)((((unsigned char*)&(ahi->elements[0].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 6] = (unsigned char)((((unsigned char*)&(ahi->elements[0].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 7] = (unsigned char)((((unsigned char*)&(ahi->elements[1].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8] = (unsigned char)((((unsigned char*)&(ahi->elements[1].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 9] = (unsigned char)((((unsigned char*)&(ahi->elements[1].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10] = (unsigned char)((((unsigned char*)&(ahi->elements[1].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 11] = (unsigned char)((((unsigned char*)&(ahi->elements[1].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12] = (unsigned char)((((unsigned char*)&(ahi->elements[1].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 13] = (unsigned char)((((unsigned char*)&(ahi->elements[1].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 14] = (unsigned char)((((unsigned char*)&(ahi->elements[2].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 15] = (unsigned char)((((unsigned char*)&(ahi->elements[2].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16] = (unsigned char)((((unsigned char*)&(ahi->elements[2].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 17] = (unsigned char)((((unsigned char*)&(ahi->elements[2].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 18] = (unsigned char)((((unsigned char*)&(ahi->elements[2].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 19] = (unsigned char)((((unsigned char*)&(ahi->elements[2].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20] = (unsigned char)((((unsigned char*)&(ahi->elements[2].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 21] = (unsigned char)((((unsigned char*)&(ahi->elements[3].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 22] = (unsigned char)((((unsigned char*)&(ahi->elements[3].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 23] = (unsigned char)((((unsigned char*)&(ahi->elements[3].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24] = (unsigned char)((((unsigned char*)&(ahi->elements[3].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 25] = (unsigned char)((((unsigned char*)&(ahi->elements[3].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 26] = (unsigned char)((((unsigned char*)&(ahi->elements[3].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 27] = (unsigned char)((((unsigned char*)&(ahi->elements[3].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28] = (unsigned char)((((unsigned char*)&(ahi->elements[4].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 29] = (unsigned char)((((unsigned char*)&(ahi->elements[4].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 30] = (unsigned char)((((unsigned char*)&(ahi->elements[4].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31] = (unsigned char)((((unsigned char*)&(ahi->elements[4].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32] = (unsigned char)((((unsigned char*)&(ahi->elements[4].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 33] = (unsigned char)((((unsigned char*)&(ahi->elements[4].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 34] = (unsigned char)((((unsigned char*)&(ahi->elements[4].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 35] = (unsigned char)((((unsigned char*)&(ahi->elements[5].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36] = (unsigned char)((((unsigned char*)&(ahi->elements[5].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 37] = (unsigned char)((((unsigned char*)&(ahi->elements[5].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 38] = (unsigned char)((((unsigned char*)&(ahi->elements[5].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 39] = (unsigned char)((((unsigned char*)&(ahi->elements[5].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40] = (unsigned char)((((unsigned char*)&(ahi->elements[5].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 41] = (unsigned char)((((unsigned char*)&(ahi->elements[5].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 42] = (unsigned char)((((unsigned char*)&(ahi->elements[6].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 43] = (unsigned char)((((unsigned char*)&(ahi->elements[6].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44] = (unsigned char)((((unsigned char*)&(ahi->elements[6].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 45] = (unsigned char)((((unsigned char*)&(ahi->elements[6].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 46] = (unsigned char)((((unsigned char*)&(ahi->elements[6].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 47] = (unsigned char)((((unsigned char*)&(ahi->elements[6].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48] = (unsigned char)((((unsigned char*)&(ahi->elements[6].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 49] = (unsigned char)((((unsigned char*)&(ahi->elements[7].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 50] = (unsigned char)((((unsigned char*)&(ahi->elements[7].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 51] = (unsigned char)((((unsigned char*)&(ahi->elements[7].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52] = (unsigned char)((((unsigned char*)&(ahi->elements[7].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 53] = (unsigned char)((((unsigned char*)&(ahi->elements[7].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 54] = (unsigned char)((((unsigned char*)&(ahi->elements[7].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 55] = (unsigned char)((((unsigned char*)&(ahi->elements[7].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56] = (unsigned char)((((unsigned char*)&(ahi->elements[8].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 57] = (unsigned char)((((unsigned char*)&(ahi->elements[8].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58] = (unsigned char)((((unsigned char*)&(ahi->elements[8].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 59] = (unsigned char)((((unsigned char*)&(ahi->elements[8].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60] = (unsigned char)((((unsigned char*)&(ahi->elements[8].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 61] = (unsigned char)((((unsigned char*)&(ahi->elements[8].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 62] = (unsigned char)((((unsigned char*)&(ahi->elements[8].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 63] = (unsigned char)((((unsigned char*)&(ahi->elements[9].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64] = (unsigned char)((((unsigned char*)&(ahi->elements[9].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 65] = (unsigned char)((((unsigned char*)&(ahi->elements[9].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 66] = (unsigned char)((((unsigned char*)&(ahi->elements[9].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 67] = (unsigned char)((((unsigned char*)&(ahi->elements[9].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68] = (unsigned char)((((unsigned char*)&(ahi->elements[9].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 69] = (unsigned char)((((unsigned char*)&(ahi->elements[9].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 70] = (unsigned char)((((unsigned char*)&(ahi->elements[10].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 71] = (unsigned char)((((unsigned char*)&(ahi->elements[10].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72] = (unsigned char)((((unsigned char*)&(ahi->elements[10].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 73] = (unsigned char)((((unsigned char*)&(ahi->elements[10].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 74] = (unsigned char)((((unsigned char*)&(ahi->elements[10].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 75] = (unsigned char)((((unsigned char*)&(ahi->elements[10].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76] = (unsigned char)((((unsigned char*)&(ahi->elements[10].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 77] = (unsigned char)((((unsigned char*)&(ahi->elements[11].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 78] = (unsigned char)((((unsigned char*)&(ahi->elements[11].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 79] = (unsigned char)((((unsigned char*)&(ahi->elements[11].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80] = (unsigned char)((((unsigned char*)&(ahi->elements[11].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 81] = (unsigned char)((((unsigned char*)&(ahi->elements[11].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 82] = (unsigned char)((((unsigned char*)&(ahi->elements[11].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 83] = (unsigned char)((((unsigned char*)&(ahi->elements[11].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84] = (unsigned char)((((unsigned char*)&(ahi->elements[12].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 85] = (unsigned char)((((unsigned char*)&(ahi->elements[12].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 86] = (unsigned char)((((unsigned char*)&(ahi->elements[12].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 87] = (unsigned char)((((unsigned char*)&(ahi->elements[12].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88] = (unsigned char)((((unsigned char*)&(ahi->elements[12].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 89] = (unsigned char)((((unsigned char*)&(ahi->elements[12].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 90] = (unsigned char)((((unsigned char*)&(ahi->elements[12].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 91] = (unsigned char)((((unsigned char*)&(ahi->elements[13].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92] = (unsigned char)((((unsigned char*)&(ahi->elements[13].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 93] = (unsigned char)((((unsigned char*)&(ahi->elements[13].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 94] = (unsigned char)((((unsigned char*)&(ahi->elements[13].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 95] = (unsigned char)((((unsigned char*)&(ahi->elements[13].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96] = (unsigned char)((((unsigned char*)&(ahi->elements[13].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 97] = (unsigned char)((((unsigned char*)&(ahi->elements[13].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 98] = (unsigned char)((((unsigned char*)&(ahi->elements[14].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 99] = (unsigned char)((((unsigned char*)&(ahi->elements[14].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100] = (unsigned char)((((unsigned char*)&(ahi->elements[14].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 101] = (unsigned char)((((unsigned char*)&(ahi->elements[14].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 102] = (unsigned char)((((unsigned char*)&(ahi->elements[14].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 103] = (unsigned char)((((unsigned char*)&(ahi->elements[14].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104] = (unsigned char)((((unsigned char*)&(ahi->elements[14].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 105] = (unsigned char)((((unsigned char*)&(ahi->elements[15].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 106] = (unsigned char)((((unsigned char*)&(ahi->elements[15].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 107] = (unsigned char)((((unsigned char*)&(ahi->elements[15].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108] = (unsigned char)((((unsigned char*)&(ahi->elements[15].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 109] = (unsigned char)((((unsigned char*)&(ahi->elements[15].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 110] = (unsigned char)((((unsigned char*)&(ahi->elements[15].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 111] = (unsigned char)((((unsigned char*)&(ahi->elements[15].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112] = (unsigned char)((((unsigned char*)&(ahi->elements[16].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 113] = (unsigned char)((((unsigned char*)&(ahi->elements[16].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 114] = (unsigned char)((((unsigned char*)&(ahi->elements[16].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 115] = (unsigned char)((((unsigned char*)&(ahi->elements[16].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116] = (unsigned char)((((unsigned char*)&(ahi->elements[16].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 117] = (unsigned char)((((unsigned char*)&(ahi->elements[16].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 118] = (unsigned char)((((unsigned char*)&(ahi->elements[16].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 119] = (unsigned char)((((unsigned char*)&(ahi->elements[17].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120] = (unsigned char)((((unsigned char*)&(ahi->elements[17].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 121] = (unsigned char)((((unsigned char*)&(ahi->elements[17].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 122] = (unsigned char)((((unsigned char*)&(ahi->elements[17].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 123] = (unsigned char)((((unsigned char*)&(ahi->elements[17].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124] = (unsigned char)((((unsigned char*)&(ahi->elements[17].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 125] = (unsigned char)((((unsigned char*)&(ahi->elements[17].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 126] = (unsigned char)((((unsigned char*)&(ahi->elements[18].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 127] = (unsigned char)((((unsigned char*)&(ahi->elements[18].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128] = (unsigned char)((((unsigned char*)&(ahi->elements[18].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 129] = (unsigned char)((((unsigned char*)&(ahi->elements[18].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 130] = (unsigned char)((((unsigned char*)&(ahi->elements[18].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 131] = (unsigned char)((((unsigned char*)&(ahi->elements[18].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 132] = (unsigned char)((((unsigned char*)&(ahi->elements[18].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 133] = (unsigned char)((((unsigned char*)&(ahi->elements[19].is_active))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 134] = (unsigned char)((((unsigned char*)&(ahi->elements[19].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 135] = (unsigned char)((((unsigned char*)&(ahi->elements[19].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 136] = (unsigned char)((((unsigned char*)&(ahi->elements[19].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 137] = (unsigned char)((((unsigned char*)&(ahi->elements[19].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 138] = (unsigned char)((((unsigned char*)&(ahi->elements[19].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 139] = (unsigned char)((((unsigned char*)&(ahi->elements[19].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 140] = (unsigned char)((((unsigned char*)&(ahi->elements[20].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 141] = (unsigned char)((((unsigned char*)&(ahi->elements[20].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 142] = (unsigned char)((((unsigned char*)&(ahi->elements[20].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 143] = (unsigned char)((((unsigned char*)&(ahi->elements[20].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 144] = (unsigned char)((((unsigned char*)&(ahi->elements[20].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 145] = (unsigned char)((((unsigned char*)&(ahi->elements[20].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 146] = (unsigned char)((((unsigned char*)&(ahi->elements[21].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 147] = (unsigned char)((((unsigned char*)&(ahi->elements[21].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 148] = (unsigned char)((((unsigned char*)&(ahi->elements[21].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 149] = (unsigned char)((((unsigned char*)&(ahi->elements[21].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 150] = (unsigned char)((((unsigned char*)&(ahi->elements[21].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 151] = (unsigned char)((((unsigned char*)&(ahi->elements[21].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 152] = (unsigned char)((((unsigned char*)&(ahi->elements[22].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 153] = (unsigned char)((((unsigned char*)&(ahi->elements[22].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 154] = (unsigned char)((((unsigned char*)&(ahi->elements[22].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 155] = (unsigned char)((((unsigned char*)&(ahi->elements[22].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 156] = (unsigned char)((((unsigned char*)&(ahi->elements[22].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 157] = (unsigned char)((((unsigned char*)&(ahi->elements[22].counter))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 158] = (unsigned char)((((unsigned char*)&(ahi->elements[23].rule.code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 159] = (unsigned char)((((unsigned char*)&(ahi->elements[23].rule.code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 160] = (unsigned char)((((unsigned char*)&(ahi->elements[23].rule.cond))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 161] = (unsigned char)((((unsigned char*)&(ahi->elements[23].rule.bc_id))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 162] = (unsigned char)((((unsigned char*)&(ahi->elements[23].rule.threshold))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 163] = (unsigned char)((((unsigned char*)&(ahi->elements[23].counter))[0]) & 0xFF);  //	
	return(164);
}



int TLM_OBC_61(void) {
	unsigned char* p;

	p = (unsigned char*)&(al->counter) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(al->header) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(al->records[0].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12] = (unsigned char)((((unsigned char*)&(al->records[0].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 13] = (unsigned char)((((unsigned char*)&(al->records[0].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 14] = (unsigned char)((((unsigned char*)&(al->records[0].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 15] = (unsigned char)((((unsigned char*)&(al->records[0].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[1].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20] = (unsigned char)((((unsigned char*)&(al->records[1].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 21] = (unsigned char)((((unsigned char*)&(al->records[1].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 22] = (unsigned char)((((unsigned char*)&(al->records[1].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 23] = (unsigned char)((((unsigned char*)&(al->records[1].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[2].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28] = (unsigned char)((((unsigned char*)&(al->records[2].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 29] = (unsigned char)((((unsigned char*)&(al->records[2].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 30] = (unsigned char)((((unsigned char*)&(al->records[2].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31] = (unsigned char)((((unsigned char*)&(al->records[2].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[3].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36] = (unsigned char)((((unsigned char*)&(al->records[3].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 37] = (unsigned char)((((unsigned char*)&(al->records[3].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 38] = (unsigned char)((((unsigned char*)&(al->records[3].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 39] = (unsigned char)((((unsigned char*)&(al->records[3].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[4].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44] = (unsigned char)((((unsigned char*)&(al->records[4].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 45] = (unsigned char)((((unsigned char*)&(al->records[4].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 46] = (unsigned char)((((unsigned char*)&(al->records[4].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 47] = (unsigned char)((((unsigned char*)&(al->records[4].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[5].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52] = (unsigned char)((((unsigned char*)&(al->records[5].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 53] = (unsigned char)((((unsigned char*)&(al->records[5].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 54] = (unsigned char)((((unsigned char*)&(al->records[5].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 55] = (unsigned char)((((unsigned char*)&(al->records[5].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[6].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60] = (unsigned char)((((unsigned char*)&(al->records[6].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 61] = (unsigned char)((((unsigned char*)&(al->records[6].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 62] = (unsigned char)((((unsigned char*)&(al->records[6].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 63] = (unsigned char)((((unsigned char*)&(al->records[6].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[7].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68] = (unsigned char)((((unsigned char*)&(al->records[7].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 69] = (unsigned char)((((unsigned char*)&(al->records[7].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 70] = (unsigned char)((((unsigned char*)&(al->records[7].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 71] = (unsigned char)((((unsigned char*)&(al->records[7].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[8].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76] = (unsigned char)((((unsigned char*)&(al->records[8].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 77] = (unsigned char)((((unsigned char*)&(al->records[8].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 78] = (unsigned char)((((unsigned char*)&(al->records[8].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 79] = (unsigned char)((((unsigned char*)&(al->records[8].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[9].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84] = (unsigned char)((((unsigned char*)&(al->records[9].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 85] = (unsigned char)((((unsigned char*)&(al->records[9].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 86] = (unsigned char)((((unsigned char*)&(al->records[9].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 87] = (unsigned char)((((unsigned char*)&(al->records[9].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[10].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92] = (unsigned char)((((unsigned char*)&(al->records[10].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 93] = (unsigned char)((((unsigned char*)&(al->records[10].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 94] = (unsigned char)((((unsigned char*)&(al->records[10].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 95] = (unsigned char)((((unsigned char*)&(al->records[10].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[11].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100] = (unsigned char)((((unsigned char*)&(al->records[11].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 101] = (unsigned char)((((unsigned char*)&(al->records[11].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 102] = (unsigned char)((((unsigned char*)&(al->records[11].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 103] = (unsigned char)((((unsigned char*)&(al->records[11].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[12].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108] = (unsigned char)((((unsigned char*)&(al->records[12].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 109] = (unsigned char)((((unsigned char*)&(al->records[12].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 110] = (unsigned char)((((unsigned char*)&(al->records[12].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 111] = (unsigned char)((((unsigned char*)&(al->records[12].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[13].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116] = (unsigned char)((((unsigned char*)&(al->records[13].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 117] = (unsigned char)((((unsigned char*)&(al->records[13].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 118] = (unsigned char)((((unsigned char*)&(al->records[13].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 119] = (unsigned char)((((unsigned char*)&(al->records[13].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[14].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124] = (unsigned char)((((unsigned char*)&(al->records[14].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 125] = (unsigned char)((((unsigned char*)&(al->records[14].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 126] = (unsigned char)((((unsigned char*)&(al->records[14].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 127] = (unsigned char)((((unsigned char*)&(al->records[14].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[15].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 132] = (unsigned char)((((unsigned char*)&(al->records[15].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 133] = (unsigned char)((((unsigned char*)&(al->records[15].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 134] = (unsigned char)((((unsigned char*)&(al->records[15].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 135] = (unsigned char)((((unsigned char*)&(al->records[15].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[16].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 136 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 136 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 136 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 136 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 140] = (unsigned char)((((unsigned char*)&(al->records[16].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 141] = (unsigned char)((((unsigned char*)&(al->records[16].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 142] = (unsigned char)((((unsigned char*)&(al->records[16].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 143] = (unsigned char)((((unsigned char*)&(al->records[16].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[17].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 144 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 144 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 144 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 144 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 148] = (unsigned char)((((unsigned char*)&(al->records[17].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 149] = (unsigned char)((((unsigned char*)&(al->records[17].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 150] = (unsigned char)((((unsigned char*)&(al->records[17].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 151] = (unsigned char)((((unsigned char*)&(al->records[17].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[18].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 152 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 152 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 152 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 152 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 156] = (unsigned char)((((unsigned char*)&(al->records[18].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 157] = (unsigned char)((((unsigned char*)&(al->records[18].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 158] = (unsigned char)((((unsigned char*)&(al->records[18].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 159] = (unsigned char)((((unsigned char*)&(al->records[18].run_length))[0]) & 0xFF);  //	
	p = (unsigned char*)&(al->records[19].time.master) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 160 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 160 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 160 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 160 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 164] = (unsigned char)((((unsigned char*)&(al->records[19].time.step))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 165] = (unsigned char)((((unsigned char*)&(al->records[19].code.group))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 166] = (unsigned char)((((unsigned char*)&(al->records[19].code.local))[0]) & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 167] = (unsigned char)((((unsigned char*)&(al->records[19].run_length))[0]) & 0xFF);  //	
	return(168);
}



int TLM_OBC_62(void) {
	unsigned char* p;

	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[0]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[1]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[2]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[3]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[4]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[5]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[6]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[7]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[8]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[9]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[10]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[11]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[12]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[13]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[14]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[15]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[16]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 64 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[17]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 68 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[18]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 72 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[19]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 76 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[20]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 80 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[21]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 84 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[22]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 88 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[23]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 92 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[24]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 96 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[25]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 100 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[26]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 104 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[27]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 108 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[28]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 112 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[29]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 116 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[30]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 120 + 3] = (unsigned char)(*p);	p--;
	p = (unsigned char*)&(*((unsigned int*)&(tl_list_for_tlm[31]->data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 124 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 128] = (unsigned char)(tl_list_for_tlm[0]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 129] = (unsigned char)(tl_list_for_tlm[1]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 130] = (unsigned char)(tl_list_for_tlm[2]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 131] = (unsigned char)(tl_list_for_tlm[3]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 132] = (unsigned char)(tl_list_for_tlm[4]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 133] = (unsigned char)(tl_list_for_tlm[5]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 134] = (unsigned char)(tl_list_for_tlm[6]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 135] = (unsigned char)(tl_list_for_tlm[7]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 136] = (unsigned char)(tl_list_for_tlm[8]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 137] = (unsigned char)(tl_list_for_tlm[9]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 138] = (unsigned char)(tl_list_for_tlm[10]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 139] = (unsigned char)(tl_list_for_tlm[11]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 140] = (unsigned char)(tl_list_for_tlm[12]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 141] = (unsigned char)(tl_list_for_tlm[13]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 142] = (unsigned char)(tl_list_for_tlm[14]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 143] = (unsigned char)(tl_list_for_tlm[15]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 144] = (unsigned char)(tl_list_for_tlm[16]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 145] = (unsigned char)(tl_list_for_tlm[17]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 146] = (unsigned char)(tl_list_for_tlm[18]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 147] = (unsigned char)(tl_list_for_tlm[19]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 148] = (unsigned char)(tl_list_for_tlm[20]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 149] = (unsigned char)(tl_list_for_tlm[21]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 150] = (unsigned char)(tl_list_for_tlm[22]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 151] = (unsigned char)(tl_list_for_tlm[23]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 152] = (unsigned char)(tl_list_for_tlm[24]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 153] = (unsigned char)(tl_list_for_tlm[25]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 154] = (unsigned char)(tl_list_for_tlm[26]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 155] = (unsigned char)(tl_list_for_tlm[27]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 156] = (unsigned char)(tl_list_for_tlm[28]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 157] = (unsigned char)(tl_list_for_tlm[29]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 158] = (unsigned char)(tl_list_for_tlm[30]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 159] = (unsigned char)(tl_list_for_tlm[31]->data[CMD_POSITION_TO] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 160] = (unsigned char)(tl_list_for_tlm[0]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 161] = (unsigned char)(tl_list_for_tlm[1]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 162] = (unsigned char)(tl_list_for_tlm[2]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 163] = (unsigned char)(tl_list_for_tlm[3]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 164] = (unsigned char)(tl_list_for_tlm[4]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 165] = (unsigned char)(tl_list_for_tlm[5]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 166] = (unsigned char)(tl_list_for_tlm[6]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 167] = (unsigned char)(tl_list_for_tlm[7]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 168] = (unsigned char)(tl_list_for_tlm[8]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 169] = (unsigned char)(tl_list_for_tlm[9]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 170] = (unsigned char)(tl_list_for_tlm[10]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 171] = (unsigned char)(tl_list_for_tlm[11]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 172] = (unsigned char)(tl_list_for_tlm[12]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 173] = (unsigned char)(tl_list_for_tlm[13]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 174] = (unsigned char)(tl_list_for_tlm[14]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 175] = (unsigned char)(tl_list_for_tlm[15]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 176] = (unsigned char)(tl_list_for_tlm[16]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 177] = (unsigned char)(tl_list_for_tlm[17]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 178] = (unsigned char)(tl_list_for_tlm[18]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 179] = (unsigned char)(tl_list_for_tlm[19]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 180] = (unsigned char)(tl_list_for_tlm[20]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 181] = (unsigned char)(tl_list_for_tlm[21]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 182] = (unsigned char)(tl_list_for_tlm[22]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 183] = (unsigned char)(tl_list_for_tlm[23]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 184] = (unsigned char)(tl_list_for_tlm[24]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 185] = (unsigned char)(tl_list_for_tlm[25]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 186] = (unsigned char)(tl_list_for_tlm[26]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 187] = (unsigned char)(tl_list_for_tlm[27]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 188] = (unsigned char)(tl_list_for_tlm[28]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 189] = (unsigned char)(tl_list_for_tlm[29]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 190] = (unsigned char)(tl_list_for_tlm[30]->data[CMD_POSITION_CODE] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 191] = (unsigned char)(tl_list_for_tlm[31]->data[CMD_POSITION_CODE] & 0xFF);  //	
	return(192);
}



int TLM_OBC_63(void) {
	unsigned char* p;

	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[0].data[CMD_POSITION_CODE] & 0xFF);  //	
	p = (unsigned char*)&(*((unsigned int*)&(bcti->blocks[bcti->pos.block].cmds[0].data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 1 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 1 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 1 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 1 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 5] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[0].data[CMD_POSITION_PARAM] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 6] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[1].data[CMD_POSITION_CODE] & 0xFF);  //	
	p = (unsigned char*)&(*((unsigned int*)&(bcti->blocks[bcti->pos.block].cmds[1].data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 7 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 7 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 7 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 7 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 11] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[1].data[CMD_POSITION_PARAM] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[2].data[CMD_POSITION_CODE] & 0xFF);  //	
	p = (unsigned char*)&(*((unsigned int*)&(bcti->blocks[bcti->pos.block].cmds[2].data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 13 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 13 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 13 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 13 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 17] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[2].data[CMD_POSITION_PARAM] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 18] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[3].data[CMD_POSITION_CODE] & 0xFF);  //	
	p = (unsigned char*)&(*((unsigned int*)&(bcti->blocks[bcti->pos.block].cmds[3].data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 19 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 19 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 19 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 19 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 23] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[3].data[CMD_POSITION_PARAM] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[4].data[CMD_POSITION_CODE] & 0xFF);  //	
	p = (unsigned char*)&(*((unsigned int*)&(bcti->blocks[bcti->pos.block].cmds[4].data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 25 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 25 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 25 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 25 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 29] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[4].data[CMD_POSITION_PARAM] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 30] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[5].data[CMD_POSITION_CODE] & 0xFF);  //	
	p = (unsigned char*)&(*((unsigned int*)&(bcti->blocks[bcti->pos.block].cmds[5].data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 31 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 35] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[5].data[CMD_POSITION_PARAM] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[6].data[CMD_POSITION_CODE] & 0xFF);  //	
	p = (unsigned char*)&(*((unsigned int*)&(bcti->blocks[bcti->pos.block].cmds[6].data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 37 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 37 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 37 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 37 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 41] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[6].data[CMD_POSITION_PARAM] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 42] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[7].data[CMD_POSITION_CODE] & 0xFF);  //	
	p = (unsigned char*)&(*((unsigned int*)&(bcti->blocks[bcti->pos.block].cmds[7].data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 43 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 43 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 43 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 43 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 47] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[7].data[CMD_POSITION_PARAM] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[8].data[CMD_POSITION_CODE] & 0xFF);  //	
	p = (unsigned char*)&(*((unsigned int*)&(bcti->blocks[bcti->pos.block].cmds[8].data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 49 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 49 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 49 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 49 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 53] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[8].data[CMD_POSITION_PARAM] & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 54] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[9].data[CMD_POSITION_CODE] & 0xFF);  //	
	p = (unsigned char*)&(*((unsigned int*)&(bcti->blocks[bcti->pos.block].cmds[9].data[CMD_POSITION_TIME]))) + 3;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 55 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 55 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 55 + 2] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 55 + 3] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 59] = (unsigned char)(bcti->blocks[bcti->pos.block].cmds[9].data[CMD_POSITION_PARAM] & 0xFF);  //	
	return(60);
}



int TLM_OBC_64(void) {
	unsigned char* p;

	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 0] = (unsigned char)(auto_ctrl.restart_action[0].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 1] = (unsigned char)(auto_ctrl.restart_action[0].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[0].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 2 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 4] = (unsigned char)(auto_ctrl.restart_action[1].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 5] = (unsigned char)(auto_ctrl.restart_action[1].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[1].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 6 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 6 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 8] = (unsigned char)(auto_ctrl.restart_action[2].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 9] = (unsigned char)(auto_ctrl.restart_action[2].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[2].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 10 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 12] = (unsigned char)(auto_ctrl.restart_action[3].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 13] = (unsigned char)(auto_ctrl.restart_action[3].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[3].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 14 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 14 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 16] = (unsigned char)(auto_ctrl.restart_action[4].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 17] = (unsigned char)(auto_ctrl.restart_action[4].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[4].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 18 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 18 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 20] = (unsigned char)(auto_ctrl.restart_action[5].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 21] = (unsigned char)(auto_ctrl.restart_action[5].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[5].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 22 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 22 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 24] = (unsigned char)(auto_ctrl.restart_action[6].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 25] = (unsigned char)(auto_ctrl.restart_action[6].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[6].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 26 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 26 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 28] = (unsigned char)(auto_ctrl.restart_action[7].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 29] = (unsigned char)(auto_ctrl.restart_action[7].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[7].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 30 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 30 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 32] = (unsigned char)(auto_ctrl.restart_action[8].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 33] = (unsigned char)(auto_ctrl.restart_action[8].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[8].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 34 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 34 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 36] = (unsigned char)(auto_ctrl.restart_action[9].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 37] = (unsigned char)(auto_ctrl.restart_action[9].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[9].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 38 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 38 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 40] = (unsigned char)(auto_ctrl.restart_action[10].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 41] = (unsigned char)(auto_ctrl.restart_action[10].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[10].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 42 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 42 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 44] = (unsigned char)(auto_ctrl.restart_action[11].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 45] = (unsigned char)(auto_ctrl.restart_action[11].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[11].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 46 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 46 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 48] = (unsigned char)(auto_ctrl.restart_action[12].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 49] = (unsigned char)(auto_ctrl.restart_action[12].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[12].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 50 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 50 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 52] = (unsigned char)(auto_ctrl.restart_action[13].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 53] = (unsigned char)(auto_ctrl.restart_action[13].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[13].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 54 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 54 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 56] = (unsigned char)(auto_ctrl.restart_action[14].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 57] = (unsigned char)(auto_ctrl.restart_action[14].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[14].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 58 + 1] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 60] = (unsigned char)(auto_ctrl.restart_action[15].activate & 0xFF);  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 61] = (unsigned char)(auto_ctrl.restart_action[15].bc_id & 0xFF);  //	
	p = (unsigned char*)&(auto_ctrl.restart_action[15].execute_time) + 1;  //	
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 62 + 0] = (unsigned char)(*p);	p--;
	TlmTransferFrame.Packet.packet_data[TLM_POSITION_MAIN + 62 + 1] = (unsigned char)(*p);	p--;
	return(64);
}

