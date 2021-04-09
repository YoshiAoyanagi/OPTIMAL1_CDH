// 
// 
// 

#include "CommandDispatcher.h"


#include "cmd_analyze_obc.h"
#include "TimeManager.h"
#include "PacketHandler.h"

static CDIS_EXEC_INFO CEI_init_(void);

CDIS CDIS_init(PL_Info* pli)
{
	CDIS cdis;

	// コマンド実行情報を初期化。
	cdis.prev = CEI_init_();
	cdis.prev_err = CEI_init_();

	// 実行エラーカウンタを0に初期化。
	cdis.error_counter = 0;

	// 実行中断フラグを無効に設定
	cdis.lockout = 0;

	// 異常時実行中断フラグを無効に設定。
	cdis.stop_on_error = 0;

	// 処理対象とするPacketListをクリアして登録。
	PL_clear_list(pli);
	cdis.pli = pli;

	return cdis;
}

static CDIS_EXEC_INFO CEI_init_(void)
{
	CDIS_EXEC_INFO cei;

	OBCT_clear(&cei.time);
	cei.time.step = 0;
	cei.code = 0;
	cei.sts = PH_SUCCESS;

	return cei;
}

void CDIS_dispatch_command(CDIS* cdis)
{
	// パケットコピー用。サイズが大きいため静的変数として宣言。
	static CTCP packet_;

	if (cdis->lockout)
	{
		// 実行有効フラグが無効化されている場合は処理打ち切り。
		return;
	}

	if (PL_is_empty(cdis->pli))
	{
		// 実行すべきコマンドが無い場合は処理終了
		Serial.println("Invalid; CDIS");
		return;
	}

	if (cdis->prev.sts != PH_SUCCESS)
	{
		cdis->prev_err = cdis->prev;
	}

	// 実行すべきコマンドパケットを取得。
	packet_ = PL_get_head(cdis->pli)->packet;
	CCP_set_cmd_exe_type(&packet_, CCP_REALTIME); // TimeLine -> RealTime

												  // 実行時情報を記録しつつコマンドを実行。
	cdis->prev.time = *master_clock;
	cdis->prev.code = CCP_get_cmd_id(&packet_);
	cdis->prev.sts = PH_despatch_command(&packet_);

	// 実行したコマンドをリストから破棄
	PL_drop_executed(cdis->pli);

	if (cdis->prev.sts != PH_SUCCESS)
	{
		// 実行したコマンドが実行異常ステータスを返した場合。
		// エラー発生カウンタをカウントアップ。
		++(cdis->error_counter);

		if (cdis->stop_on_error == 1)
		{
			// 異常時実行中断フラグが有効な場合。
			// 実行許可フラグを無効化し以降の実行を中断。
			cdis->lockout = 1;
		}
	}
}

void CDIS_clear_command_list(CDIS* cdis)
{
	// 保持しているリストの内容をクリア
	PL_clear_list(cdis->pli);
}

void CDIS_clear_error_status(CDIS* cdis)
{
	// 実行エラー状態を初期状態に復元
	cdis->prev_err = CEI_init_();

	// 積算エラー回数を0クリア
	cdis->error_counter = 0;
}
