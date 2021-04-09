// 
// 
// 

#include "TaskDispatcher.h"
#include <stdlib.h>
#include <string.h> // for memcpy
#include "utility.h"

#include "TimeManager.h"
#include "AnomalyLogger.h"
#include "BlockCommandDefinisions.h"
#include "cmd_analyze_obc.h"

static TDSP_Info TDSP_info_;
const TDSP_Info* TDSP_info;

static PL_Info task_list_;

static void TDSP_deploy_block_as_task_list_(void);

void TDSP_initialize(void)
{
	static PL_Node task_stock_[TDSP_TASK_MAX];

	task_list_ = PL_initialize(task_stock_, TDSP_TASK_MAX);

	TDSP_info_.tskd = CDIS_init(&task_list_);
	TDSP_info_.task_list_id = BC_TL_NORMAL;
	TDSP_deploy_block_as_task_list_();
	TDSP_info_.activated_at = 0;
	TDSP_info = &TDSP_info_;
}

TDSP_ACK TDSP_set_task_list_id(size_t id)
{
	TDSP_info_.task_list_id = id;
	return TDSP_SUCCESS;
}

static void TDSP_deploy_block_as_task_list_(void)
{
	PL_ACK ack;
	// 本関数内の処理中にMaster Cycleが変化した場合を検出できるよう、
	// まず次Master Cycleの情報を更新する。
	TDSP_info_.activated_at = master_clock->master + 1;
	ack = PL_deploy_block_cmd(&task_list_, BCT_get_bc(TDSP_info_.task_list_id), 0);
	if (ack != PL_SUCCESS) { AL_add_anomaly(AL_TASK_DISPATCHER, TDSP_DEPLOY_FAILED); }
}

void TDSP_resync_internal_counter(void)
{
	TDSP_info_.activated_at = master_clock->master;
}

void TDSP_execute_pl_as_task_list(void)
{
	if (TDSP_info_.activated_at == master_clock->master)
	{
		PL_ACK ack = PL_check_tl_cmd(&task_list_,
			(size_t)master_clock->step);
		switch (ack)
		{
		case PL_TLC_PAST_TIME:
			CDIS_dispatch_command(&(TDSP_info_.tskd));
			//AL_add_anomaly(AL_TASK_DISPATCHER, TDSP_STEP_OVERRUN);**別スレッド処理が入るためPAST_TIMEの確率が高いため，コメントアウト
			break;
		case PL_TLC_ON_TIME:
			CDIS_dispatch_command(&(TDSP_info_.tskd));
			if (TDSP_info_.tskd.prev.sts != PH_SUCCESS)
			{
				AL_add_anomaly(AL_TASK_DISPATCHER, TDSP_TASK_EXEC_FAILED);
			}
			break;
		case PL_TLC_NOT_YET:
			// task_listが空なら再度タスクリストを展開
			if (task_list_.pl_public.active_nodes == 0)
			{
				TDSP_deploy_block_as_task_list_();
			}
			break;
		default:
			AL_add_anomaly(AL_TASK_DISPATCHER, TDSP_UNKNOWN);
		}
	}
	else if (TDSP_info_.activated_at > master_clock->master)
	{
		return; // 次サイクルの実行待ち状態
	}
	else if (TDSP_info_.activated_at < master_clock->master)
	{
		if ((TDSP_info_.activated_at == 0) && (master_clock->master == OBCT_MAX_CYCLE - 1))
		{
			return; // 次サイクルの実行待ち状態(サイクルオーバーフロー直前)
		}
		else
		{
			//Printf("TDSP_CYCLE_OVERRUN \n");
			AL_add_anomaly(AL_TASK_DISPATCHER, TDSP_CYCLE_OVERRUN);
			// リストをクリア->再展開し次サイクルから再実行
			PL_clear_list(&task_list_);
			TDSP_deploy_block_as_task_list_();
		}
	}
}

int Cmd_TDSP_SET_TASK_LIST_ID(const CTCP* packet)
{
	TDSP_ACK ack = TDSP_SUCCESS;
	// コマンドデータ長確認
	if (CCP_get_length(packet) != 11 + 1)
	{
		// データ長はヘッダ+1Byte(TASK LIST ID)
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	ack = TDSP_set_task_list_id((size_t)(CCP_get_cmd_param(packet)[0]));

	if (ack != TDSP_SUCCESS)
	{
		return CMD_ERROR_ILLEGAL_PARAMETER;
	}

	return CMD_ERROR_NONE;
}