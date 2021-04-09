// BlockCommandTable.h

#ifndef _BLOCKCOMMANDTABLE_h
#define _BLOCKCOMMANDTABLE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CommonTlmCmdPacket.h"

#define BCT_MAX_BLOCK_LENGTH (10)
#define BCT_MAX_BLOCKS (10)

typedef struct
{
	size_t block;
	size_t cmd;
} BCT_Position;

typedef struct
{
	int is_active;
	size_t length;
	CTCP cmds[BCT_MAX_BLOCK_LENGTH];
} BC_Info;

typedef struct
{
	BCT_Position pos;
	BC_Info blocks[BCT_MAX_BLOCKS];
}BCT_Info;

typedef enum
{
	BCT_SUCCESS,
	BCT_INVALID_BLOCK_NO,
	BCT_INVALID_CMD_NO,
	BCT_ISORATED_CMD,
	BCT_DEFECTIVE_BLOCK,
	BCT_BC_FULL
}BCT_ACK;

extern const BCT_Info *bcti;

void BCT_initialize(void);

int Cmd_CLEAR_BLOCK(const CTCP* packet);

int Cmd_SET_BLOCK_POSITION(const CTCP* packet);

int Cmd_ACTIVATE_BLOCK(const CTCP* packet);

int Cmd_ROTATE_BLOCK(const CTCP* packet);

int Cmd_COMBINE_BLOCK(const CTCP* packet);

BCT_ACK BCT_clear_block(size_t block);

const BC_Info* BCT_get_bc(size_t id);

BCT_ACK BCT_clear_block(size_t block);

BCT_ACK BCT_register_cmd(const CTCP* packet);

BCT_ACK BCT_activate_block(void);

#endif // BLOCK_COMMAND_TABLE_H_