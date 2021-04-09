// 
// 
// 

#include "AutonomousControl.h"
#include "FRAM_manager.h"
#include "cmd_item.h"

AUTO_CTRL_STRUCT auto_ctrl;
void AutoControl_debug_message(void);

void AutoControl_init(void)
{
	//Restart counter設定
	unsigned int   i_tmp;
	unsigned short s_tmp;
	
	FRAM_Read_reboot_count(&i_tmp);
	auto_ctrl.restart_counter = i_tmp + 1;

	FRAM_Write_reboot_count(auto_ctrl.restart_counter);

	FRAM_Read_restart_count(&s_tmp);
	auto_ctrl.restart_non_cmd_counter = s_tmp + 1;

	FRAM_Write_restart_count(auto_ctrl.restart_non_cmd_counter);

	//Read info
	int i;
	for (i = 0; i < RESTART_ACTION_MAX; i++)
	{
		FRAM_Read_auto_ctrl_info(i, &auto_ctrl.restart_action[i].bc_id, &auto_ctrl.restart_action[i].activate,
			&auto_ctrl.restart_action[i].execute_time);
	}
	
	AutoControl_debug_message();
}

void AutoControl_execute(void)
{
	int i;

	CTCP packet;
	unsigned char param;

	static int disactivate;

	if (disactivate != 0)	//auto_controlはOBC起動時に1回しか実行させない
		return;

	disactivate = 1;

	if (auto_ctrl.restart_non_cmd_counter == 0)
		return;

	for (i = 0; i < RESTART_ACTION_MAX; i++)
	{
		if (auto_ctrl.restart_non_cmd_counter == auto_ctrl.restart_action[i].execute_time)
		{
			//BC execute
			if (auto_ctrl.restart_action[i].activate == 1)
			{
				param = auto_ctrl.restart_action[i].bc_id;
				packet = CCP_form_rt_cmd(CODE_Cmd_FRAM_DEPLOY_BLOCK, &param, 1u);
				RT_Cmd_Register(packet);
			}
			
			if (i == (RESTART_ACTION_MAX - 1))
			{
				Clear_NonCmd_counter();	//最後まで到達したらゼロクリする
			}

			return;
		}
	}

}

void AutoControl_set_restart_non_cmd_counter(unsigned short val)
{
	unsigned short count;
	
	FRAM_Write_restart_count(val);
	FRAM_Read_restart_count(&count);
	auto_ctrl.restart_non_cmd_counter = count;

}

int AutoControl_set_action(unsigned char auto_ctrl_id, unsigned char bc_id, unsigned char activate, unsigned short exec_times)
{
	if (auto_ctrl_id > RESTART_ACTION_MAX)
		return (-1);

	FRAM_Write_auto_ctrl_info(auto_ctrl_id, bc_id, activate, exec_times);

	auto_ctrl.restart_action[auto_ctrl_id].activate = activate;
	auto_ctrl.restart_action[auto_ctrl_id].bc_id = bc_id;
	auto_ctrl.restart_action[auto_ctrl_id].execute_time = exec_times;

	return 0;
}

int AutoControl_remove_action(unsigned char auto_ctrl_id)
{
	if (auto_ctrl_id > RESTART_ACTION_MAX)
		return (-1);

	FRAM_Write_auto_ctrl_info(auto_ctrl_id, 0, 0, 0);

	auto_ctrl.restart_action[auto_ctrl_id].activate = 0;
	auto_ctrl.restart_action[auto_ctrl_id].bc_id = 0;
	auto_ctrl.restart_action[auto_ctrl_id].execute_time = 0;
	return 0;
}

void AutoControl_remove_all_action(void)
{
	int i;

	for (i = 0; i < RESTART_ACTION_MAX; i++)
	{
		AutoControl_remove_action(i);
	}
}


void AutoControl_debug_message(void)
{
	int i;

	Serial.println("[AUTO_CTRL]");
	for (i = 0; i < RESTART_ACTION_MAX; i++)
	{
		Serial.print("ID= "); Serial.print(i);
		Serial.print(", ACTIVATE= "); Serial.print(auto_ctrl.restart_action[i].activate);
		Serial.print(", BC_ID= "); Serial.print(auto_ctrl.restart_action[i].bc_id);
		Serial.print(", EXECUTE_TIME= "); Serial.print(auto_ctrl.restart_action[i].execute_time);
		Serial.println("");
	}
}

void Clear_NonCmd_counter(void)
{
	static unsigned char r;

	if (r == 0)
	{
		auto_ctrl.restart_non_cmd_counter = 0;
		FRAM_Write_restart_count(auto_ctrl.restart_non_cmd_counter);
		r = 1;
	}
}