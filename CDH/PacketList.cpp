#include "PacketList.h"
static PL_Node* PL_get_free_node_(PL_Info* pli);

PL_Info PL_initialize(PL_Node* stock,
	size_t nodes)
{
	PL_Info pli;

	pli.pl_private.stock = stock;
	pli.pl_public.total_nodes = nodes;
	pli.pl_public.executed_nodes = 0;
	PL_clear_list(&pli);

	return pli;
}

void PL_clear_list(PL_Info* pli)
{
	int i;

	for (i = 0; i<pli->pl_public.total_nodes - 1; ++i)
	{
		pli->pl_private.stock[i].next = &(pli->pl_private.stock[i + 1]);
	}
	pli->pl_private.stock[pli->pl_public.total_nodes - 1].next = NULL;

	pli->pl_public.active_nodes = 0;
	pli->pl_private.inactive_list_head = pli->pl_private.stock;
	pli->pl_private.active_list_head = NULL;
	pli->pl_private.active_list_tail = NULL;
}

size_t PL_count_active_nodes(const PL_Info* pli)
{
	return (size_t)(pli->pl_public.active_nodes);
}

size_t PL_count_inactive_nodes(const PL_Info* pli)
{
	return pli->pl_public.total_nodes - pli->pl_public.active_nodes;
}

int PL_is_empty(const PL_Info* pli)
{
	return (pli->pl_private.active_list_head == NULL);
}

int PL_is_full(const PL_Info* pli)
{
	return (pli->pl_private.inactive_list_head == NULL);
}

PL_Node* PL_get_head(const PL_Info* pli)
{
	return pli->pl_private.active_list_head;
}

PL_Node* PL_get_tail(const PL_Info* pli)
{
	return pli->pl_private.active_list_tail;
}

PL_Node* PL_get_next(const PL_Node* node)
{
	if (node != NULL) { return node->next; }
	return NULL;
}

PL_ACK PL_push_front(PL_Info* pli,
	const CTCP* packet)
{
	PL_Node *pl_new = PL_get_free_node_(pli);
	if (pl_new == NULL) { return PL_LIST_FULL; }

	pl_new->packet = *packet;

	pl_new->next = pli->pl_private.active_list_head;
	pli->pl_private.active_list_head = pl_new;
	if (pli->pl_private.active_list_tail == NULL)
	{
		pli->pl_private.active_list_tail = pl_new;
	}

	++(pli->pl_public.active_nodes);

	return PL_SUCCESS;
}

PL_ACK PL_push_back(PL_Info* pli,
	const CTCP* packet)
{
	PL_Node *pl_new = PL_get_free_node_(pli);
	if (pl_new == NULL) { return PL_LIST_FULL; }

	pl_new->packet = *packet;

	pl_new->next = NULL;
	if (pli->pl_private.active_list_tail == NULL)
	{
		pli->pl_private.active_list_head = pl_new;
	}
	else
	{
		pli->pl_private.active_list_tail->next = pl_new;
	}
	pli->pl_private.active_list_tail = pl_new;
	++(pli->pl_public.active_nodes);

	return PL_SUCCESS;
}

PL_ACK PL_insert_after(PL_Info* pli,
	PL_Node* pos,
	const CTCP* packet)
{
	PL_Node* pl_new = NULL;

	if (pos == NULL) { return PL_NO_SUCH_NODE; }

	pl_new = PL_get_free_node_(pli);
	if (pl_new == NULL) { return PL_LIST_FULL; }

	pl_new->packet = *packet;

	pl_new->next = pos->next;
	pos->next = pl_new;

	++(pli->pl_public.active_nodes);

	return PL_SUCCESS;
}

PL_ACK PL_insert_tl_cmd(PL_Info* pli,
	const CTCP* packet,
	size_t now)
{
	size_t planed = CCP_get_cmd_time(packet);

	if (now > planed) // 指定実行時間が既に過ぎている
	{
		DEBUG_Serial.print("now:"); DEBUG_Serial.println(now);
		DEBUG_Serial.print("planed:"); DEBUG_Serial.println(planed);
		return PL_TLC_PAST_TIME;
	}
	else if (PL_is_full(pli)) // 登録余裕がない
	{
		return PL_LIST_FULL;
	}

	if (PL_is_empty(pli)) // 何も登録されていない
	{
		PL_push_front(pli, packet);
	}
	else // 他コマンドが登録されている
	{
		size_t head
			= CCP_get_cmd_time(&(PL_get_head(pli)->packet));
		size_t tail
			= CCP_get_cmd_time(&(PL_get_tail(pli)->packet));

		if (tail < planed) // 他のどれより遅い
		{
			PL_push_back(pli, packet);
		}
		else if (head > planed) // 他のどれより早い
		{
			PL_push_front(pli, packet);
		}
		else if (head == planed) // 登録の先頭と時刻指定が等しい
		{
			return PL_TLC_ALREADY_EXISTS;
		}
		else // 他コマンドの間に挿入操作が必要
		{
			PL_Node* prev = PL_get_head(pli);
			PL_Node* curr = PL_get_next(prev);

			do // 挿入場所探索
			{
				cycle_t test = CCP_get_cmd_time(&(curr->packet));
				if (test < planed) // 挿入場所はまだ後ろ
				{
					prev = curr;
					curr = PL_get_next(prev);
				}
				else if (test > planed) // 挿入場所発見
				{
					PL_insert_after(pli, prev, packet);
					break;
				}
				else { return PL_TLC_ALREADY_EXISTS; } // 既登録コマンドと時刻指定が等しい
			} while (curr != NULL);
		}
	}
	return PL_SUCCESS;
}

PL_ACK PL_deploy_block_cmd(PL_Info* pli,
	const BC_Info* bci,
	size_t start_at)
{
	// リスト強制クリアの記録用変数
	int is_cleared = 0;
	// 時刻調整の累積量保存用変数
	size_t adj = 0;

	// ブロックが無効化されている場合
	if (!(bci->is_active)) { return PL_BC_INACTIVE_BLOCK; }
	// ブロックが有効な場合
	else
	{
		int i;

		// リストにブロック全体を登録する余裕がない場合
		if (PL_count_inactive_nodes(pli) < bci->length)
		{
			// リストをクリアし強制的に空き領域を確保する。
			PL_clear_list(pli);
			is_cleared = 1;
		}

		for (i = 0; i<bci->length; ++i)
		{
			CTCP temp = bci->cmds[i];
			PL_ACK ack;

			cycle_t time = CCP_get_cmd_time(&temp);
			cycle_t _time = 0;
			_time = (time & 0xff) << 24;
			_time += (time >> 8 & 0xff) << 16;
			_time += (time >> 16 & 0xff) << 8;
			_time += (time >> 24 & 0xff);

			cycle_t tmp_offset = (cycle_t)(start_at + adj + _time);	//Endianひっくり返して足し算
			cycle_t offset = 0;
			offset = (tmp_offset & 0xff) << 24;
			offset += (tmp_offset >> 8 & 0xff) << 16;
			offset += (tmp_offset >> 16 & 0xff) << 8;
			offset += (tmp_offset >> 24 & 0xff);

			CCP_set_cmd_time(&temp, offset);
			CCP_set_cmd_exe_type(&temp, CCP_TIMELINE); // RealTime -> Timeline
			ack = PL_insert_tl_cmd(pli, &temp, start_at);
			//ack = PL_insert_tl_cmd(pli, &temp, _start_at);
			// 同一時刻で既に登録されていた場合は時刻をずらして再登録
			
			if(ack != PL_SUCCESS)
			{
				DEBUG_Serial.println("PL: Deploy failed!"); DEBUG_Serial.println(ack);
				DEBUG_Serial.print("offset:"); DEBUG_Serial.println(offset);
				DEBUG_Serial.print("time:"); DEBUG_Serial.println(time);
				DEBUG_Serial.print("start_at:"); DEBUG_Serial.println(start_at);
			}

			while (ack == PL_TLC_ALREADY_EXISTS)
			{
				time = CCP_get_cmd_time(&temp);
				_time = 0;
				_time = (time & 0xff) << 24;
				_time += (time >> 8 & 0xff) << 16;
				_time += (time >> 16 & 0xff) << 8;
				_time += (time >> 24 & 0xff);

				tmp_offset = (cycle_t)(_time + 1);	//Endianひっくり返して足し算
				offset = 0;
				offset = (tmp_offset & 0xff) << 24;
				offset += (tmp_offset >> 8 & 0xff) << 16;
				offset += (tmp_offset >> 16 & 0xff) << 8;
				offset += (tmp_offset >> 24 & 0xff);

				CCP_set_cmd_time(&temp, offset);
				
				ack = PL_insert_tl_cmd(pli, &temp, start_at);
				++adj; // 累積調整時間を更新する
				if (adj > 100)
					break;
			}
			if (ack != PL_SUCCESS) { return ack; }
		}
	}

	// リストの強制クリアを実施した場合。
	if (is_cleared == 1) { return PL_BC_LIST_CLEARED; }
	// 時刻調整を行った場合。
	else if (adj != 0) { return PL_BC_TIME_ADJUSTED; }
	// 問題なく登録できた場合。
	else { return PL_SUCCESS; }
}

PL_ACK PL_check_tl_cmd(PL_Info* pli,
	size_t time)
{
	if (!PL_is_empty(pli)) // コマンドリストが空でない
	{
		CTCP *packet = &(PL_get_head(pli)->packet);
		cycle_t planed = CCP_get_cmd_time(packet);

		if (time == planed) { return PL_TLC_ON_TIME; }
		else if (time > planed) { return PL_TLC_PAST_TIME; }
	}
	return PL_TLC_NOT_YET;
}

void PL_drop_executed(PL_Info* pli)
{
	PL_drop_head(pli);
	++(pli->pl_public.executed_nodes);
}

void PL_drop_head(PL_Info* pli)
{
	PL_Node *temp = NULL;

	if (pli->pl_private.active_list_head == NULL) { return; }

	temp = pli->pl_private.active_list_head;
	pli->pl_private.active_list_head = pli->pl_private.active_list_head->next;
	if (pli->pl_private.active_list_head == NULL)
	{
		pli->pl_private.active_list_tail = NULL;
	}

	temp->next = pli->pl_private.inactive_list_head;
	pli->pl_private.inactive_list_head = temp;

	--(pli->pl_public.active_nodes);
}

void PL_drop_node(PL_Info *pli,
	PL_Node *pos)
{
	PL_Node *temp = NULL;

	if (pos == NULL) { return; }
	else if (pli->pl_private.active_list_head == NULL) { return; }
	else if (pli->pl_private.active_list_head == pos) { PL_drop_head(pli); }

	temp = pli->pl_private.active_list_head;
	while (temp->next != pos)
	{
		if (temp->next == NULL) { return; }
		temp = temp->next;
	}

	temp->next = pos->next;
	if (pli->pl_private.active_list_tail == pos)
	{
		pli->pl_private.active_list_tail = temp;
	}

	pos->next = pli->pl_private.inactive_list_head;
	pli->pl_private.inactive_list_head = pos;
	--(pli->pl_public.active_nodes);
}

static PL_Node* PL_get_free_node_(PL_Info* pli)
{
	PL_Node *temp = pli->pl_private.inactive_list_head;
	if (temp == NULL) { return NULL; }

	pli->pl_private.inactive_list_head
		= pli->pl_private.inactive_list_head->next;

	return temp;
}
