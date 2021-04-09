// AutonomousControl.h

#ifndef _AUTONOMOUSCONTROL_h
#define _AUTONOMOUSCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#define RESTART_ACTION_MAX	16

typedef struct
{
	unsigned char activate;
	unsigned char bc_id;
	unsigned short  execute_time;
}RESTART_ACTION_STRUCT;

typedef struct
{
	unsigned int	restart_counter;
	unsigned short	restart_non_cmd_counter;
	RESTART_ACTION_STRUCT restart_action[16];
	
}AUTO_CTRL_STRUCT;

extern AUTO_CTRL_STRUCT auto_ctrl;

void AutoControl_init(void);
void AutoControl_execute(void);


void AutoControl_set_restart_non_cmd_counter(unsigned short val);
int AutoControl_set_action(unsigned char auto_ctrl_id, unsigned char bc_id, unsigned char activate, unsigned short exec_times);
int AutoControl_remove_action(unsigned char auto_ctrl_id);
void AutoControl_remove_all_action(void);

void Clear_NonCmd_counter(void);

#endif

