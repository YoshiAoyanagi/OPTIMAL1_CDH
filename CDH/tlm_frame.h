#ifndef	_TLM_FRAME_INCLUDE
#define	_TLM_FRAME_INCLUDE

#include "arduino.h"
#include "TimeManager.h"
#include "Config.h"
#include "cmd_analyze_obc.h"
#include "Utility.h"
#include "CommonTlmCmdPacket.h"
#include "Reed_Solomon.h"
#include "PacketHandler.h"
#include "GS_DataHandler.h"
#include "EPS.h"
#include "Temperature.h"
#include "MTQ_OBC.h"
//#include "HighRateForwardHandler.h"
#include "RTC.h"
#include "UVC_app.h"
//#include "ObcCam.h"
#include "DataRecorderManager.h"
#include "Stored_TlmGenerator.h"
#include "STRX_update.h"
#include "AutonomousControl.h"

extern unsigned int  now_time;

typedef enum {
	TLM_REAL,
	TLM_DR
}TLM_MODE_DR;

#ifdef  _TLM_FRAME_MAIN
int   TLM_OBC_01(void);  //for regular
int   TLM_OBC_02(void);  //
int   TLM_OBC_03(void);  //
int   TLM_OBC_04(void);  //
int   TLM_OBC_05(void);  //
//int   TLM_OBC_06(void);  //
int   TLM_OBC_07(void);  //
int   TLM_OBC_08(void);  //
int   TLM_OBC_09(void);  //
int   TLM_OBC_60(void);  //
int   TLM_OBC_61(void);  //
int   TLM_OBC_62(void);  //
int   TLM_OBC_63(void);  //
int   TLM_OBC_64(void);  //

#else
extern  int   TLM_OBC_01(void);
extern  int   TLM_OBC_02(void);
extern  int   TLM_OBC_03(void);
extern  int   TLM_OBC_04(void);
extern  int   TLM_OBC_05(void);
//extern  int   TLM_OBC_06(void);
extern  int   TLM_OBC_07(void);
extern  int   TLM_OBC_08(void);
extern  int   TLM_OBC_09(void);
extern  int   TLM_OBC_60(void);
extern  int   TLM_OBC_61(void);
extern  int   TLM_OBC_62(void);
extern  int   TLM_OBC_63(void);
extern  int   TLM_OBC_64(void);
#endif

#define	  TLM_MAX_NUMBER			65

#define   TLM_ID_01                 0x01
#define   TLM_ID_02                 0x02
#define   TLM_ID_03                 0x03
#define   TLM_ID_04                 0x04
#define   TLM_ID_05                 0x05
//#define   TLM_ID_06                 0x06
#define   TLM_ID_07                 0x07
#define   TLM_ID_08                 0x08
#define   TLM_ID_09                 0x09
#define   TLM_ID_60                 60
#define   TLM_ID_61                 61
#define   TLM_ID_62                 62
#define   TLM_ID_63                 63
#define   TLM_ID_64                 64

#define		TLM_HK					TLM_ID_01
#define		TLM_SBC					TLM_ID_04
#define		TLM_SBC_PARAM			TLM_ID_05

#define	  TLM_PACKET_MAX			256
#define	  TLM_STX_1                 0xEB
#define   TLM_STX_2                 0x90
#define	  TLM_ETX_1                 0xC5
#define   TLM_ETX_2                 0x79
//#define	  TLMCMD_TLM		    0xFF

#define	  TLM_POSITION_STX1			0
#define	  TLM_POSITION_STX2			1
#define	  TLM_POSITION_LENGTH	    2
#define	  TLM_POSITION_DISCRIMINATE 3
#define	  TLM_POSITION_TLM_MODE	    4
#define	  TLM_POSITION_TLM_COUNT    5
#define	  TLM_POSITION_TLM_TIME	    6
#define	  TLM_POSITION_CMD_CODE	    10
#define	  TLM_POSITION_CMD_STAT	    11
#define	  TLM_POSITION_CMD_ERR_STAT 12
#define	  TLM_POSITION_CMD_COUNT    13
#define	  TLM_POSITION_MAIN			14

#define	  TLM_POSITION_CRC_H	    14
#define	  TLM_POSITION_CRC_L        15
#define	  TLM_POSITION_ETX1			16
#define	  TLM_POSITION_ETX2			17
#define	  TLM_BYTE_COUNT			 (TLM_POSITION_ETX2+1)

//Transfer Frame define for MOBC and COM
#define   TLM_POSITION_RS           18
#define   REED_SOLOMON_SIZE         32

#define	  SYNC_MARKER				0x1ACFFC1D

#define	  TF_SIZE					251
#define	  DATA_PACKET_SIZE			219//251-32


#ifndef  _TLM_FRAME_INCLUDE2
#define  _TLM_FRAME_INCLUDE2
typedef union{
  struct {
	  unsigned char packet_data[219];
    unsigned char rs_code[32];
  } Packet;
  struct{
	  unsigned char data[251];
  }  TransferFrame;
} TRANSFER_FRAME;
#endif

#ifdef  _TLM_FRAME_MAIN
int   (*tlm_packet_ptr[TLM_MAX_NUMBER])(void);
int   TlmPacket_combining(unsigned char tlm_id, int (*TlmDataSet_p)(void), TLM_MODE_DR tlm_mode);
int   MakeTlmPacket(unsigned char tlm_id, TLM_MODE_DR tlm_mode);
int  TlmSend(int ofs);
int   TlmCmd_init(void);
void  Tlm_PN_rand(unsigned char *data, unsigned char *rand_data, unsigned int len);
unsigned int	obc_tlm_millis;
unsigned int	obc_tlm_master_cycle;
unsigned char   TLM_ID;
unsigned char   TlmSts;
unsigned char   TlmCount[256];
unsigned char   DrTlmCount[256];
TRANSFER_FRAME  TlmTransferFrame;
#else
extern  int   (*tlm_packet_ptr[TLM_MAX_NUMBER])(void);
extern  int   TlmPacket_combining(unsigned char tlm_id, int (*TlmDataSet_p)(void), TLM_MODE_DR tlm_mode);
extern  int   MakeTlmPacket(unsigned char tlm_id, TLM_MODE_DR tlm_mode);
extern  int  TlmSend(int ofs);
extern  int   TlmCmd_init(void);
extern  void  Tlm_PN_rand(unsigned char *data, unsigned char *rand_data, unsigned int len);
extern  unsigned int	obc_tlm_millis;
extern  unsigned int	obc_tlm_master_cycle;
extern  unsigned char   TLM_ID;
extern  unsigned char   TlmSts;
extern  unsigned char   TlmCount[256];
extern  unsigned char   DrTlmCount[256];
extern  TRANSFER_FRAME  TlmTransferFrame;
#endif

#endif