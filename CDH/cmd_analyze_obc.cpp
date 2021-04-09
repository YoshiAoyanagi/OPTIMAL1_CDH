#include "cmd_action.h"
#include "cmd_analyze_obc.h"
#include "TL_CmdDispatcher.h"

int       CmdSts;
int       CmdCode;
int       CmdCount;
int       CmdErrorSts;
int       CmdLength;
int       CmdTo;
int       CmdTlmCmd;


int cmdExec(const CTCP* packet)
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


	if (CmdCode == CODE_Cmd_NOP) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_NOP(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_DEPLOY_BLOCK) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_DEPLOY_BLOCK(packet);	// 引数であたえたブロックのブロックコマンドを展開してタイムラインコマンドに格納する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_CLEAR_BLOCK) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_CLEAR_BLOCK(packet);	// 引数で与えたブロックのブロックコマンドをクリアする
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SET_BLOCK_POSITION) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_BLOCK_POSITION(packet);	// ブロックコマンドの登録位置を指定する。
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_ACTIVATE_BLOCK) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_ACTIVATE_BLOCK(packet);	// ブロックコマンドを有効化する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FINISH_TRANSITION) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FINISH_TRANSITION(packet);	// モード遷移終了トリガコマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SET_TRANSITION_TABLE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_TRANSITION_TABLE(packet);	// TransitionTable書き換えコマンド(from, to , index)
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_EXECUTE_APP) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_EXECUTE_APP(packet);	// アプリケーション実行コマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_TDSP_SET_TASK_LIST_ID) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_TDSP_SET_TASK_LIST_ID(packet);	// タスクリストBC設定コマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_ROTATE_BLOCK) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_ROTATE_BLOCK(packet);	// BC回転コマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_COMBINE_BLOCK) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_COMBINE_BLOCK(packet);	// BC融合コマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AH_register_rule) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AH_register_rule(packet);	// アノマリルール設定コマンド(id, group)
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AH_activate_rule) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AH_activate_rule(packet);	// アノマリルール有効化コマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AH_inactivate_rule) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AH_inactivate_rule(packet);	// アノマリルール無効化コマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AH_clear_log) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AH_clear_log(packet);	// アノマリログクリアコマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AS_set_id) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AS_set_id(packet);	// アプリID設定コマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_REGISTER_APP) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_REGISTER_APP(packet);	// アプリ登録コマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_INITIALIZE_APP) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_INITIALIZE_APP(packet);	// アプリ初期化コマンド
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SET_MODE_LIST) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_MODE_LIST(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_START_TRANSITION) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_START_TRANSITION(packet);	// 運用モードを変更する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_TlmRequest) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_TlmRequest(packet);	// 指定TLM_IDのテレメトリをダウンリンクする
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_TLM_SET_MODE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_TLM_SET_MODE(packet);	// OBCテレメトリモード設定：開始
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SET_TLM_INTERVAL) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_TLM_INTERVAL(packet);	// interval[s]
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_CLEAR_ALL_TIMELINE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_CLEAR_ALL_TIMELINE(packet);	// MOBCコマンドクリア(タイムライン)
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_CLEAR_TIMELINE_AT) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_CLEAR_TIMELINE_AT(packet);	// MOBCに登録されているタイムラインコマンドから1つ選んで削除する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SET_OBC_TIME) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_OBC_TIME(packet);	// MOBC時刻タグを入力値に設定する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_STRX_SET_BR) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_STRX_SET_BR(packet);	// param0: 0=4kbps, 1:32kbps, 2:64kbps (IF基板とSTRX本体同時に変更)
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_STRX_TX_ON) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_STRX_TX_ON(packet);	// param0: 0=OFF, 1=ON
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MUX_UART_SET) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MUX_UART_SET(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SYNC_TIME) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SYNC_TIME(packet);	// 地上局時刻と衛星(OBC, RTC)時刻を同期させる
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SYNC_OBC_TIME) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SYNC_OBC_TIME(packet);	// 地上局時刻とOBC時刻を同期させる
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SYNC_RTC_TIME) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SYNC_RTC_TIME(packet);	// 地上局時刻とRTC時刻を同期させる
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SYNC_GPS_TIME) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SYNC_GPS_TIME(packet);	// GPS時刻と衛星時刻を同期させる
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_SYNC_RTC_ENABLE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_SYNC_RTC_ENABLE(packet);	// OBCリセット時にRTCと時刻同期させる(ENA:1, DIS:0)
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_TLC_CLEAR_ALL) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_TLC_CLEAR_ALL(packet);	// OBC-FRAMのコマンドクリア(タイムライン)
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_TLC_CLEAR_AT_TIME) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_TLC_CLEAR_AT_TIME(packet);	// OBC-FRAMに登録されているタイムラインコマンドから時刻指定で1つ選んで削除する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_TLC_CLEAR_AT_NUMBER) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_TLC_CLEAR_AT_NUMBER(packet);	// OBC-FRAMに登録されているタイムラインコマンドから番号指定で1つ選んで削除する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SOFTWARE_RESET) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SOFTWARE_RESET(packet);	// ソフト強制リセット
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_INITIALIZE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_INITIALIZE(packet);	// FRAMの初期化, 打ち上げ前モードにする
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_CMD_INITIALIZE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_CMD_INITIALIZE(packet);	// FRAM(TLC, BC)を初期化する(全消去)
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_DEPLOY_BLOCK) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_DEPLOY_BLOCK(packet);	// 引数であたえたブロックのブロックコマンドを展開してタイムラインコマンドに格納する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_REMOVE_BLOCK) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_REMOVE_BLOCK(packet);	// 引数で与えたブロックのブロックコマンドをクリアする
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_SET_BLOCK_POSITION) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_SET_BLOCK_POSITION(packet);	// ブロックコマンドの登録位置を指定する。
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_ACTIVATE_BLOCK) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_ACTIVATE_BLOCK(packet);	// ブロックコマンドの有効にする
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_REMOVE_BLOCK_AT_POSITION) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_REMOVE_BLOCK_AT_POSITION(packet);	// 指定IDの位置のブロックコマンドを消去する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_REMOVE_ALL_BLOCK) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_REMOVE_ALL_BLOCK(packet);	// 登録済みのブロックコマンドの情報をすべて消去
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_REQUEST_BC_INFO) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_REQUEST_BC_INFO(packet);	// 指定IDのブロックコマンド情報をダウンリンクする
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_FRAM_REQUEST_BC_PARAM) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_FRAM_REQUEST_BC_PARAM(packet);	// 指定ID, 位置のブロックコマンドの実行パケットをダウンリンクする
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_STRX_SET_TX_PWR) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_STRX_SET_TX_PWR(packet);	// Power Modeの切り替え
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_STRX_OTC_ENABLE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_STRX_OTC_ENABLE(packet);	// 0: DIS, 1:EN
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_STRX_OTC_SET_THRESHOLD) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_STRX_OTC_SET_THRESHOLD(packet);	// Temperature[degC]
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SET_STX_ONOFF_DURATION) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_STX_ONOFF_DURATION(packet);	// param1: on時間[s]，param2: off時間[s]
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SET_STX_BURST_DURATION) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_STX_BURST_DURATION(packet);	// 連続送信時間を設定[s]
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_EPS_RESET) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_EPS_RESET(packet);	// 0: MAIN, 1:SUB
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_EPS_UVC_ENABLE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_EPS_UVC_ENABLE(packet);	// 0: DIS, 1:EN
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_EPS_UVC_SET_BAT_V) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_EPS_UVC_SET_BAT_V(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_EPS_SELECT) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_EPS_SELECT(packet);	// 0:ChA, 1:ChB
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SRX_RESET) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SRX_RESET(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_ADCS_POWER) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_ADCS_POWER(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_RW_ENABLE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_RW_ENABLE(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_HEATER_POWER) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_HEATER_POWER(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_RW_POWER) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_RW_POWER(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MSN_unreg_ENABLE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MSN_unreg_ENABLE(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MSN_5V_POWER) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MSN_5V_POWER(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SUBCAM_POWER) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SUBCAM_POWER(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SF_POWER) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SF_POWER(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MTQ_POWER_OBC) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MTQ_POWER_OBC(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_POWER_SAFE_OFF) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_POWER_SAFE_OFF(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_UNREG_POWER) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_UNREG_POWER(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_POWER_REBOOT) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_POWER_REBOOT(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_OBC_MTQ_X) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_OBC_MTQ_X(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_OBC_MTQ_Y) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_OBC_MTQ_Y(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_OBC_MTQ_Z) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_OBC_MTQ_Z(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AQU_5V_POWER) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AQU_5V_POWER(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_OVCO_5V_POWER) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_OVCO_5V_POWER(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_OBC_DR_RECORD) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_OBC_DR_RECORD(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_OBC_DR_REPLAY_PARAM_SET) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_OBC_DR_REPLAY_PARAM_SET(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_OBC_DR_REPLAY) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_OBC_DR_REPLAY(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_OBC_DR_ERASE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_OBC_DR_ERASE(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MSN_DR_RECORD_ENABLE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MSN_DR_RECORD_ENABLE(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MSN_DR_REPLAY_PARAM_SET) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MSN_DR_REPLAY_PARAM_SET(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MSN_DR_REPLAY) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MSN_DR_REPLAY(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MSN_DR_ERASE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MSN_DR_ERASE(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_OBC_DR_INITIALIZE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_OBC_DR_INITIALIZE(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MSN_DR_INITIALIZE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MSN_DR_INITIALIZE(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_OBC_DR_DIRECT_REPLAY) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_OBC_DR_DIRECT_REPLAY(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MSN_DR_DIRECT_REPLAY) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MSN_DR_DIRECT_REPLAY(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_STX_SET_OUTPORT) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_STX_SET_OUTPORT(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_STX_SET_MOD) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_STX_SET_MOD(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_STX_SET_PN_TEST) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_STX_SET_PN_TEST(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SET_STRX_LOCK_REPLY_TIME) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_STRX_LOCK_REPLY_TIME(packet);	// 
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_TlmRequest_via_SF) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_TlmRequest_via_SF(packet);	// SF経由で指定IDのテレメトリをダウンリンクする
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_Set_Fwd_TlmID_via_SF) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_Set_Fwd_TlmID_via_SF(packet);	// SF経由で指定IDのテレメトリをダウンリンクする
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AutoControl_set_restart_counter) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AutoControl_set_restart_counter(packet);	// 再起動自動処理のカウンターを設定する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AutoControl_set_action) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AutoControl_set_action(packet);	// 再起動自動処理の内容を設定する
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AutoControl_remove_action) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AutoControl_remove_action(packet);	// 再起動自動処理の消去（ID指定)
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AutoControl_remove_all_action) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AutoControl_remove_all_action(packet);	// 再起動自動処理の全消去
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_AutoControl_execute) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_AutoControl_execute(packet);	// 再起動自動処理の実行
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_ClearRebootCount) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_ClearRebootCount(packet);	// OBC再起動回数のクリア
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_SET_TLMD_ANOMALY_ENABLE) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_SET_TLMD_ANOMALY_ENABLE(packet);	// TLMDアノマリの有効/無効
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_RETERN_CMD_OBC_UTIME) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_RETURN_CMD_OBC_UTIME(packet);	// OBC UTIMEをコマンドで返答する（返してほしいCMD_IDを指定する）
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}
	}
	else if (CmdCode == CODE_Cmd_MSN_DR_ENABLE_OBC_TIMETAG) {
		CmdSts = CMD_STATUS_START;
		CmdErrorSts = Cmd_MSN_DR_ENABLE_OBC_TIMETAG(packet);	// ミッションテレメをMSN_DRに保存するときに～TLM_TIMEをOBC_UTIMEにセットする
		if (CmdErrorSts == CMD_ERROR_NONE) {
			CmdSts = CMD_STATUS_END;
		}
		else {
			CmdSts = CMD_STATUS_ERROR_EXE;
		}


		//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##//##
		/*
		This pattern is a "separator".
		This should not be changed.
		This should not be used in other places.
		*/

	}
	else {
		Serial.println("*** UNKONWN CMD ***");
		return(CMD_ERROR_NOT_DEFINED);
	}

	return(CmdErrorSts);
}
