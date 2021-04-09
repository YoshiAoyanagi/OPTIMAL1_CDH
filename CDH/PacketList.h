#ifndef PACKET_LIST_H_
#define PACKET_LIST_H_

#include <stddef.h>

#include "CommonTlmCmdPacket.h"
#include "BlockCommandTable.h"

struct PL_NodeTag
{
	CTCP packet;
	struct PL_NodeTag* next;
};

typedef struct PL_NodeTag PL_Node;

typedef struct
{
	size_t total_nodes;
	size_t executed_nodes;
	size_t active_nodes;
} PL_Public;

typedef struct
{
	PL_Node* stock;
	PL_Node* inactive_list_head;
	PL_Node* active_list_head;
	PL_Node* active_list_tail;
} PL_Private;

typedef struct
{
	PL_Public pl_public;
	PL_Private pl_private;
} PL_Info;

typedef enum
{
	PL_SUCCESS,
	PL_LIST_FULL,
	PL_TLC_PAST_TIME,
	PL_TLC_ALREADY_EXISTS,
	PL_TLC_ON_TIME,
	PL_TLC_NOT_YET,
	PL_BC_INACTIVE_BLOCK,
	PL_BC_LIST_CLEARED,
	PL_BC_TIME_ADJUSTED,
	PL_NO_SUCH_NODE
} PL_ACK;

PL_Info PL_initialize(PL_Node* stock,
	size_t size);

void PL_clear_list(PL_Info* pli);

size_t PL_count_active_nodes(const PL_Info* pli);

size_t PL_count_inactive_nodes(const PL_Info* pli);

int PL_is_empty(const PL_Info* pli);

int PL_is_full(const PL_Info* pli);

PL_Node* PL_get_head(const PL_Info* pli);

PL_Node* PL_get_tail(const PL_Info* pli);

PL_Node* PL_get_next(const PL_Node* node);

PL_ACK PL_check_st_cmd(PL_Info* pli, size_t time);

PL_ACK PL_push_front(PL_Info* pli,
	const CTCP* packet);

PL_ACK PL_push_back(PL_Info* pli,
	const CTCP* packet);

PL_ACK PL_insert_after(PL_Info* pli,
	PL_Node* pos,
	const CTCP* packet);

PL_ACK PL_insert_tl_cmd(PL_Info* pli,
	const CTCP* packet,
	size_t now);

PL_ACK PL_deploy_block_cmd(PL_Info* pli,
	const BC_Info* bci,
	size_t start_at);

PL_ACK PL_check_tl_cmd(PL_Info* pli,
	size_t time);

void PL_drop_executed(PL_Info* pli);

void PL_drop_head(PL_Info* pli);

void PL_drop_node(PL_Info *pli,
	PL_Node *pos);


#endif // PACKET_LIST_H_
