// 
// 
// 

#include "AnomalyHandler.h"
#include "utility.h"
#include "TimeManager.h"
#include "UVC_app.h"
#include "BlockCommandDefinisions.h"
#include "PacketHandler.h"
#include "cmd_analyze_obc.h"

static AHInfo ahi_;
const AHInfo *ahi;

static size_t prev_pos_;
static size_t prev_act_;

static void AH_init_(void);
static void load_default_rules_(void);
static void AH_exec_(void);
static size_t check_rules_(size_t record_id);
static int is_equal_code_(const AnomalyCode* a,
	const AnomalyCode* b);
static void respond_to_anomaly_(size_t id);
static void add_rule_(size_t id,
	const AHRule *ahr);
static void AH_clear_log_(void);

AppInfo AH_create_app(void)
{
	return create_app_info("ah", 1, AH_init_, AH_exec_);
}

static void AH_init_(void)
{
	int i;

	ahi = &ahi_;
	// 初期設定はすべてのルールを無効化する
	for (i = 0; i<AH_MAX_RULES; ++i) { ahi_.elements[i].is_active = 0; }
	// デフォルトのルール構成を読み込み
	load_default_rules_();
	// 対応時刻をゼロクリア
	OBCT_clear(&(ahi_.respond_at));
	// 初期は範囲外に設定し該当なしを表現
	ahi_.latest_id = AH_MAX_RULES;
	// 対応実行数をクリア
	ahi_.action_counter = 0;
	// アノマリ対応検知用の変数をクリア
	prev_act_ = 0;
	// アノマリ処理状況を初期化
	AH_clear_log_();
}

static void load_default_rules_(void)
{
	
	AHRule ahr;

	// Under Voltage Control Lv.1
	ahr.code.group = AL_UVC;
	ahr.code.local = UVC_LEVEL1;
	ahr.cond = AH_SINGLE;
	ahr.threshold = 0;
	ahr.bc_id = BC_UVC_LV1_ACTION;

	add_rule_(0, &ahr);
	ahi_.elements[0].is_active = 1;

	// Under Voltage Control Lv.2
	ahr.code.group = AL_UVC;
	ahr.code.local = UVC_LEVEL2;
	ahr.cond = AH_SINGLE;
	ahr.threshold = 0;
	ahr.bc_id = BC_UVC_LV2_ACTION;

	add_rule_(1, &ahr);
	ahi_.elements[1].is_active = 1;

	// Under Voltage Control return normal
	ahr.code.group = AL_UVC;
	ahr.code.local = UVC_RETURN;
	ahr.cond = AH_SINGLE;
	ahr.threshold = 0;
	ahr.bc_id = BC_UVC_RTN_ACTION;

	add_rule_(2, &ahr);
	ahi_.elements[2].is_active = 1;
	
}

static void AH_exec_(void)
{
	// アノマリ個数が変化していない場合
	if (ahi_.al_pointer.count == al->counter) { return; }
	// 新たなアノマリが発生した場合
	else if (ahi_.al_pointer.count < al->counter)
	{
		size_t record_id;
		for (record_id = ahi_.al_pointer.pos; record_id<al->header; ++record_id)
		{
			size_t id = check_rules_(record_id);
			if (id != AH_MAX_RULES) { respond_to_anomaly_(id); }
		}
		// 処理終了後、最新位置・回数をAnomalyLogger側と同期
		ahi_.al_pointer.pos = al->header - 1;
		ahi_.al_pointer.count = al->counter;
	}
	// 異常: AnomalyHandlerとAnomalyLoggerの関係が崩れている。
	else
	{
		// 仕方がないので、記録をリセットし強制同期する。
		AH_clear_log_();
	}
}

static size_t check_rules_(size_t record_id)
{
	const AnomalyRecord* ar = AL_get_record(record_id);
	int is_latest = (record_id == al->header);
	size_t id;
	// 全ルールを順次走査
	for (id = 0; id<AH_MAX_RULES; ++id)
	{
		AHElement ahe = ahi_.elements[id];
		// ルールが無効の場合はスキップ
		if (!(ahe.is_active)) { continue; }
		// コード不一致の場合はスキップ
		else if (!is_equal_code_(&(ahe.rule.code), &(ar->code))) { continue; }

		// コードに対応するルールが存在。判定条件を評価。
		switch (ahe.rule.cond)
		{
		case AH_SINGLE:
			// コードが存在した時点で無条件合致。
			return id;
		case AH_CONTINUOUS:
			// 連続回数を超過した場合に条件合致。
			if (ahe.rule.threshold < ar->run_length) { return id; }
			break;
		case AH_CUMULATE:
			// 積算回数を超過した場合に条件合致。
			if (ahe.counter <= ar->run_length) { return id; }
			// 最新レコードに対する比較でない場合は積算数を更新する。
			else if (!is_latest) { ahe.counter -= ar->run_length; }
			break;
		}
	}
	return AH_MAX_RULES; // 該当なしの場合は登録上限を返す。
}

static int is_equal_code_(const AnomalyCode* a,
	const AnomalyCode* b)
{
	return ((a->group == b->group) && (a->local == b->local));
}

static void respond_to_anomaly_(size_t id)
{
	// 対応ブロックコマンドをリアルタイムコマンドで展開
	unsigned char param = (unsigned char)ahi_.elements[id].rule.bc_id;
	CTCP packet = CCP_form_rt_cmd(CODE_Cmd_DEPLOY_BLOCK, &param, 1u);
	PH_despatch_command(&packet);

	// 実行したルールを記録し回数を更新
	ahi_.respond_at = *master_clock;
	ahi_.latest_id = id;
	++ahi_.action_counter;

	// 合致したルールは無効化する。
	ahi_.elements[id].is_active = 0;
}

AppInfo print_ah_status(void)
{
	return create_app_info("ahs", 5, NULL, print_ah_status_);
}

void print_ah_status_(void)
{
	/*
	Printf("AH: ID %d, ACT %d, AT <%d, %d, %d>, POS %d, CTR %d\n",
		ahi_.latest_id, ahi_.action_counter,
		ahi_.respond_at.master, ahi_.respond_at.mode, ahi_.respond_at.step,
		ahi_.al_pointer.pos, ahi_.al_pointer.count);
	*/
	// 新種のアノマリが発生した場合警告音1回
	if (prev_pos_ != ahi_.al_pointer.pos)
	{
		DEBUG_Serial.println("\a");
		prev_pos_ = ahi_.al_pointer.pos;
	}

	// アノマリ対応が発生した場合警告音2回
	if (prev_act_ != ahi_.action_counter)
	{
		DEBUG_Serial.println("\a\a");
		prev_act_ = ahi_.action_counter;
	}
}

int Cmd_AH_register_rule(const CTCP* packet)
{
	enum { ID, GROUP, LOCAL, COND, THRESHOLD, BC };
	const unsigned char* param = CCP_get_cmd_param(packet);
	AHRule ahr;

	if (CCP_get_length(packet) != 11 + 6)
	{
		// パラメータは6Bytes
		return CMD_ERROR_ILLEGAL_LENGTH;
	}
	else if (param[ID] >= AH_MAX_RULES)
	{
		// 登録指定位置が許容範囲外
		return CMD_ERROR_ILLEGAL_PARAMETER;
	}
	else if (param[COND] > AH_CUMULATE)
	{
		// 判定条件が定義されたものと一致しない
		return CMD_ERROR_ILLEGAL_PARAMETER;
	}

	ahr.code.group = param[GROUP];
	ahr.code.local = param[LOCAL];
	ahr.cond = (AHCondition)param[COND];
	ahr.threshold = param[THRESHOLD];
	ahr.bc_id = param[BC];

	add_rule_((size_t)param[ID], &ahr);

	return CMD_ERROR_NONE;
}

static void add_rule_(size_t id,
	const AHRule *ahr)
{
	ahi_.elements[id].is_active = 0; // 登録時点では無効とする
	ahi_.elements[id].rule = *ahr;
	ahi_.elements[id].counter = ahr->threshold;
}

int Cmd_AH_activate_rule(const CTCP* packet)
{
	const unsigned char* param = CCP_get_cmd_param(packet);

	if (CCP_get_length(packet) != 11 + 1)
	{
		// パラメータは1Byte
		return CMD_ERROR_ILLEGAL_LENGTH;
	}
	else if (param[0] >= AH_MAX_RULES)
	{
		// 指定位置が範囲外
		return CMD_ERROR_ILLEGAL_PARAMETER;
	}

	ahi_.elements[param[0]].is_active = 1;
	ahi_.elements[param[0]].counter = ahi_.elements[param[0]].rule.threshold;
	return CMD_ERROR_NONE;
}

int Cmd_AH_inactivate_rule(const CTCP* packet)
{
	const unsigned char* param = CCP_get_cmd_param(packet);

	if (CCP_get_length(packet) != 11 + 1)
	{
		// パラメータは1Byte
		return CMD_ERROR_ILLEGAL_LENGTH;
	}
	else if (param[0] >= AH_MAX_RULES)
	{
		// 指定位置が範囲外
		return CMD_ERROR_ILLEGAL_PARAMETER;
	}

	ahi_.elements[param[0]].is_active = 0;
	return CMD_ERROR_NONE;
}

int Cmd_AH_clear_log(const CTCP* packet)
{
	if (CCP_get_length(packet) != 11)
	{
		// パラメータなし
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	AH_clear_log_();
	return CMD_ERROR_NONE;
}

static void AH_clear_log_(void)
{
	// AnomalyHnadler内の保持情報を初期化
	ahi_.al_pointer.count = 0;
	ahi_.al_pointer.pos = 0;
	// アノマリ発生検知用変数を初期化
	prev_pos_ = 0;
	// AnomalyLoggerのリストをクリア
	AL_clear();
}

