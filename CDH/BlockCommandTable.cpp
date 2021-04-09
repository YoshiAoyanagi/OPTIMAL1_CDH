// 
// 
// 

#include "BlockCommandTable.h"

#include "utility.h"
#include "BlockCommandDefinisions.h"
#include "PacketHandler.h"
#include "cmd_analyze_obc.h"
#include "TimeManager.h"

static BCT_Info bcti_;

const BCT_Info *bcti;

static BCT_ACK BCT_set_position_(const BCT_Position* pos);
static PH_ACK BCT_rotate_block_cmd_(size_t block_id);
static PH_ACK BCT_combine_block_cmd_(size_t block_id);
static BCT_ACK add_cmd_(BC_Info* bci, const CTCP* packet);
static BCT_ACK check_position_(const BCT_Position* pos);

void BCT_initialize(void)
{
	int i;
	for (i = 0; i<BCT_MAX_BLOCKS; ++i)
	{
		bcti_.blocks[i].is_active = 0;
		bcti_.blocks[i].length = 0;
	}

	bcti = &bcti_;

	BC_load_defaults();
}

int Cmd_CLEAR_BLOCK(const CTCP* packet)
{
	size_t block = packet->data[CMD_POSITION_PARAM];
	BCT_ACK ack = BCT_clear_block(block);

	if (ack != BCT_SUCCESS) 
	{ 
		DEBUG_Serial.println("BCT: Clear failed!"); 
	}
	return CMD_ERROR_NONE;
}

int Cmd_SET_BLOCK_POSITION(const CTCP* packet)
{
	BCT_Position pos;
	BCT_ACK ack;

	pos.block = packet->data[CMD_POSITION_PARAM];
	pos.cmd = packet->data[CMD_POSITION_PARAM + 1];
	ack = BCT_set_position_(&pos);

	if (ack != BCT_SUCCESS)
	{ 
		DEBUG_Serial.println("BCT: Invalid positon!"); 
	}
	return CMD_ERROR_NONE;
}

static BCT_ACK BCT_set_position_(const BCT_Position* pos)
{
	BCT_ACK ack = check_position_(pos);
	if (ack != BCT_SUCCESS) { return ack; }

	bcti_.pos = *pos;
	return BCT_SUCCESS;
}

int Cmd_ACTIVATE_BLOCK(const CTCP* packet)
{
	BCT_ACK ack = BCT_activate_block();
	if (ack != BCT_SUCCESS)
	{
		DEBUG_Serial.println("BCT: Activation failed!");
		return CMD_ERROR_ILLEGAL_CONTEXT;
	}

	(void)packet;
	return CMD_ERROR_NONE;
}

int Cmd_ROTATE_BLOCK(const CTCP* packet)
{
	PH_ACK ack;
	if (CCP_get_length(packet) != 11 + 1) // パラメータは1Byte
	{
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	ack = BCT_rotate_block_cmd_((size_t)CCP_get_cmd_param(packet)[0]);
	if (ack == PH_SUCCESS) { return CMD_ERROR_NONE; }
	else { return CMD_ERROR_ILLEGAL_CONTEXT; }
}

static PH_ACK BCT_rotate_block_cmd_(size_t block_id)
{
	unsigned char tes;
	const BC_Info *bci = BCT_get_bc(block_id);
	cycle_t now = master_clock->master;

	if (!(bci->is_active)) { return PH_BC_INACTIVE_BLOCK; }

	tes = (unsigned char)(now%bci->length);

	return PH_despatch_command(&(bci->cmds[now%bci->length]));
}

int Cmd_COMBINE_BLOCK(const CTCP* packet)
{
	PH_ACK ack;
	if (CCP_get_length(packet) != 11 + 1) // パラメータは1Byte
	{
		return CMD_ERROR_ILLEGAL_LENGTH;
	}

	ack = BCT_combine_block_cmd_((size_t)CCP_get_cmd_param(packet)[0]);
	if (ack == PH_SUCCESS) { return CMD_ERROR_NONE; }
	else { return CMD_ERROR_ILLEGAL_CONTEXT; }
}

static PH_ACK BCT_combine_block_cmd_(size_t block_id)
{
	int i;
	PH_ACK ack;
	const BC_Info *bci = BCT_get_bc(block_id);

	if (!(bci->is_active)) { return PH_BC_INACTIVE_BLOCK; }

	for (i = 0; i<bci->length; ++i)
	{
		ack = PH_despatch_command(&(bci->cmds[i]));
		// 異常発生時は実行中断
		if (ack != PH_SUCCESS) { return PH_BC_COMBINE_FAILED; }
	}

	return PH_SUCCESS;
}

BCT_ACK BCT_clear_block(size_t block)
{
	if (block >= BCT_MAX_BLOCKS) { return BCT_INVALID_BLOCK_NO; }

	bcti_.pos.block = block;
	bcti_.pos.cmd = 0;
	bcti_.blocks[bcti_.pos.block].is_active = 0;
	bcti_.blocks[bcti_.pos.block].length = 0;

	return BCT_SUCCESS;
}

const BC_Info* BCT_get_bc(size_t id)
{
	return &(bcti_.blocks[id]);
}

BCT_ACK BCT_register_cmd(const CTCP* packet)
{
	BCT_ACK ack = check_position_(&bcti_.pos);
	if (ack != BCT_SUCCESS) { return ack; }

	if (bcti_.pos.cmd > bcti_.blocks[bcti_.pos.block].length)
	{
		return BCT_ISORATED_CMD;
	}

	bcti_.blocks[bcti_.pos.block].cmds[bcti_.pos.cmd] = *packet;
	// CMD_TYPE_IDをブロックコマンドからシングルコマンドに変更する。
	CCP_set_cmd_type_id(&(bcti_.blocks[bcti_.pos.block].cmds[bcti_.pos.cmd]), CCP_SINGLE);
	// ブロック登録時点でコマンド発行元をOBCに変更する。
	CCP_set_cmd_from_id(&(bcti_.blocks[bcti_.pos.block].cmds[bcti_.pos.cmd]), FROM_TO_COMM);
	// 連続登録に備え、ポインタ位置を進める。上限超過は次回登録時リジェクトされる。
	++(bcti_.pos.cmd);
	// ブロック末尾へのコマンド追加の場合はブロック長を更新する
	if (bcti_.blocks[bcti_.pos.block].length < bcti_.pos.cmd)
	{
		bcti_.blocks[bcti_.pos.block].length = bcti_.pos.cmd;
	}

	return BCT_SUCCESS;
}

BCT_ACK BCT_activate_block(void)
{
	if (bcti_.blocks[bcti_.pos.block].length != bcti_.pos.cmd)
	{
		return BCT_DEFECTIVE_BLOCK;
	}

	bcti_.blocks[bcti_.pos.block].is_active = 1;
	return BCT_SUCCESS;
}

static BCT_ACK add_cmd_(BC_Info* bci, const CTCP* packet)
{
	if (bci->length == BCT_MAX_BLOCK_LENGTH) { return BCT_BC_FULL; }

	bci->cmds[bci->length] = *packet;
	++(bci->length);

	return BCT_SUCCESS;
}

static BCT_ACK check_position_(const BCT_Position* pos)
{
	if (pos->block >= BCT_MAX_BLOCKS)
	{
		return BCT_INVALID_BLOCK_NO;
	}
	else if (pos->cmd >= BCT_MAX_BLOCK_LENGTH)
	{
		return BCT_INVALID_CMD_NO;
	}

	return BCT_SUCCESS;
}

