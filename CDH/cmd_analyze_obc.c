##include "cmd_action.h"
#include "cmd_analyze.h"

int       CmdSts;
int       CmdCode;
int       CmdCount;
int       CmdErrorSts;
int       CmdLength;
int       CmdTo;
int       CmdTlmCmd;


int cmdExec(const CTCP *packet)
{
	CmdCode = CCP_get_cmd_id(packet);
	CmdCount++;
	CmdCount &= 0xff;

/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/
//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##


	if(CmdCode == CODE_Cmd_NOP){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_NOP(packet);	// ダミーコマンド, コマンドカウントのみ上昇するが何も起こらない
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_DEPLOY_BLOCK){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_DEPLOY_BLOCK(packet);	// 引数であたえたブロックのブロックコマンドを展開してタイムラインコマンドに格納する
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_CLEAR_BLOCK){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_CLEAR_BLOCK(packet);	// 引数で与えたブロックのブロックコマンドをクリアする
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_SET_BLOCK_POSITION){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_BLOCK_POSITION(packet);	// ブロックコマンドの登録位置を指定する。
param1: ブロック番号 [0-7]
param2: コマンド位置 [0-7]
最大8個のコマンドを1つのブロックコマンドに登録が可能でそのようなブロックコマンドを最大8個定義できる
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_ACTIVATE_BLOCK){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_ACTIVATE_BLOCK(packet);	// ブロックコマンドを有効化する

		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_FINISH_TRANSITION){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FINISH_TRANSITION(packet);	// モード遷移終了トリガコマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_SET_TRANSITION_TABLE){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_TRANSITION_TABLE(packet);	// TransitionTable書き換えコマンド(from, to , index)
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_EXECUTE_APP){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_EXECUTE_APP(packet);	// アプリケーション実行コマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_SET_TASK_LIST_ID){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_TASK_LIST_ID(packet);	// タスクリストBC設定コマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_ROTATE_BLOCK){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_ROTATE_BLOCK(packet);	// BC回転コマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_COMBINE_BLOCK){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_COMBINE_BLOCK(packet);	// BC融合コマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_AH_register_rule){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AH_register_rule(packet);	// アノマリルール設定コマンド(id, group)
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_AH_activate_rule){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AH_activate_rule(packet);	// アノマリルール有効化コマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_AH_inactivate_rule){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AH_inactivate_rule(packet);	// アノマリルール無効化コマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_AH_clear_log){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AH_clear_log(packet);	// アノマリログクリアコマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_AS_set_id){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AS_set_id(packet);	// アプリID設定コマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_CMD_REGISTER_APP){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = CMD_REGISTER_APP(packet);	// アプリ登録コマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_CMD_INITIALIZE_APP){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = CMD_INITIALIZE_APP(packet);	// アプリ初期化コマンド
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_SET_MODE_LIST){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_MODE_LIST(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_START_TRANSITION){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_START_TRANSITION(packet);	// 運用モードを変更する
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_GENERATE_TLM){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_GENERATE_TLM(packet);	// Param1[1byte]:TLM_ID
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_TLM_SET_MODE){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_TLM_SET_MODE(packet);	// OBCテレメトリモード設定：開始
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_SET_TLM_INTERVAL){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_TLM_INTERVAL(packet);	// interval[s]
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_SET_DR_INTERVAL){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_DR_INTERVAL(packet);	// interval[s]
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_CLEAR_ALL_TIMELINE){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_CLEAR_ALL_TIMELINE(packet);	// MOBCコマンドクリア(タイムライン)
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_CLEAR_TIMELINE_AT){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_CLEAR_TIMELINE_AT(packet);	// MOBCコマンドクリア(タイムライン)
MOBCに登録されているタイムラインコマンドから1つ選んで削除する
登録されている時刻(MOBC_TLM_TIME)をパラメータで指定する
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_SET_MOBC_TIME){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_MOBC_TIME(packet);	// MOBC時刻タグを入力値に設定する
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_MSN_DATA_FORWARD){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MSN_DATA_FORWARD(packet);	// param1: target id
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_DR_RECORD_MTLM){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_DR_RECORD_MTLM(packet);	// param1: start/stop, param2: interval[s]
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_DR_REPLAY_SET){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_DR_REPLAY_SET(packet);	// param1: start sector, param2: start pk-address
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_DR_REPLAY){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_DR_REPLAY(packet);	// DRリプレイ終了
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_DR_ERASE){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_DR_ERASE(packet);	// DR消去: S&F, param2: num sector
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_COM_MUX_UART_SET){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = COM_MUX_UART_SET(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_SOFTWARE_RESET){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = SOFTWARE_RESET(packet);	// ソフト強制リセット
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_FRAM_INITIALIZE){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = FRAM_INITIALIZE(packet);	// FRAMの初期化, 打ち上げ前モードにする
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_SET_STX_POWER_MODE){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_STX_POWER_MODE(packet);	// Power Modeの切り替え
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_STX_ON){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_STX_ON(packet);	// 送信機の送信機能をONにする
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_STX_OFF){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_STX_OFF(packet);	// 送信機の送信機能をOFFにする
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_Cmd_SET_DOWNLINK_BITRATE){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_DOWNLINK_BITRATE(packet);	// ダウンリンク速度を一括で設定する
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_EPS_RESET){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = EPS_RESET(packet);	// 0: MAIN, 1:SUB
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_EPS_UVC_ENABLE){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = EPS_UVC_ENABLE(packet);	// 0: DIS, 1:EN
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_EPS_UVC_SET_BAT_V){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = EPS_UVC_SET_BAT_V(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_ADCS_POWER){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = ADCS_POWER(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_RW_unreg_POWER){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = RW_unreg_POWER(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_RESERVE_5V_POWER){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = RESERVE_5V_POWER(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_GNSSR_POWER){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = GNSSR_POWER(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_MSN_unreg_POWER){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = MSN_unreg_POWER(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_MSN_5V_POWER){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = MSN_5V_POWER(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_SUBCAM_POWER){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = SUBCAM_POWER(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_SF_POWER){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = SF_POWER(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_SAS_POWER_TO_ADCS){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = SAS_POWER_TO_ADCS(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_MTQ_POWER_TO_ADCS){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = MTQ_POWER_TO_ADCS(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}else if(CmdCode == CODE_RW_5V_POWER_TO_ADCS){
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = RW_5V_POWER_TO_ADCS(packet);	// 
		if(CmdErrorSts == CMD_ERROR_NONE){
			CmdSts = CMD_STATUS_END;
		}else{
			CmdSts = CMD_STATUS_ERROR_EXE;
		}


//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
/*
This pattern is a "separator".
This should not be changed.
This should not be used in other places.
*/

	}else{
		Printf("*** UNKONWN CMD ***");
		Printf("%4X\n", CmdCode);
		return(CMD_ERROR_NOT_DEFINED);
	}
	
	return(CmdErrorSts);
}
