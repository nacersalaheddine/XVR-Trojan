#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"
#include "hdd/hdd.h"

#define SERV_LS_LEN_GB 1
#define SERV_LS_LEN_MB 2
#define SERV_LS_LEN_KB 3
#define SERV_LS_LEN_B 0
#define SERV_LS_FILE 0x2
#define SERV_LS_FOLDER 0x3
#define SERV_LS_UNKNOWN 0x4

#define SERV_LS_END 0x1
#define SERV_LS_DATA 0x2
#define SERV_LS_FAILED 0x3

typedef struct __serv_ls_info
{
	char type;
	char fname[HDD_MAX_PATH];
	char sizeType;
	double size;
	time_t cTime; //created time
	time_t mTime; //mod time
}SERV_LS_INFO;

int cmds_LS(uint8* msg)
{
	net_SetBuffer(NET_DATA_BUFFSIZE);
		
	DIR *dir = opendir((char*)msg);

	if(!dir)
	{
		if(net_SendEmptyCmd(SERV_LS_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	if(net_SendEmptyCmd(CMDS_HDD_LS) < 1)
	{
		closedir(dir);

		return 0;
	}

	struct _stati64 fbuf;
	struct dirent *_dir;
	int pathLen = strlen((char*)msg);
	char path[net_MaxContLen];
	SERV_LS_INFO lsInfo;
	uint8* rmsg;

	while((_dir = readdir(dir)) != NULL)
	{
		memset(&lsInfo, 0, sizeof(SERV_LS_INFO));
		memset(path, 0, net_MaxContLen);
		strcpy(path, (char*)msg);

		if(path[pathLen - 1] == '\\')
		{
			strcpy(path + pathLen, _dir->d_name);
		}else{
			path[pathLen] = '\\';
			strcpy(path + pathLen + 1, _dir->d_name);
		}

		if(strlen(_dir->d_name) < HDD_MAX_PATH)
		{
			strcpy(lsInfo.fname, _dir->d_name);
		}else{
			strcpy(lsInfo.fname, "000");
		}

		if(_stati64(path, &fbuf) == 0)
		{
			if(S_ISDIR(fbuf.st_mode))
			{
				lsInfo.type = SERV_LS_FOLDER;
			}else if(S_ISREG(fbuf.st_mode)){
				lsInfo.type = SERV_LS_FILE;
			}

			lsInfo.cTime = fbuf.st_ctime;
			lsInfo.mTime = fbuf.st_mtime;

			if(fbuf.st_size / (1024 * 1024 * 1024))
			{
				lsInfo.size = ((double)fbuf.st_size / (1024.0 * 1024.0 * 1024.0));
				lsInfo.sizeType = SERV_LS_LEN_GB;
			}else if(fbuf.st_size / (1024 * 1024)){
				lsInfo.size = ((double)fbuf.st_size / (1024.0 * 1024.0));
				lsInfo.sizeType = SERV_LS_LEN_MB;
			}else if(fbuf.st_size / 1024){
				lsInfo.size = ((double)fbuf.st_size / 1024.0);
				lsInfo.sizeType = SERV_LS_LEN_KB;
			}else{
				lsInfo.size = (double)fbuf.st_size;
				lsInfo.sizeType = SERV_LS_LEN_B;
			}
		}

		if(net_SendCmd((uint8*)&lsInfo, sizeof(lsInfo), SERV_LS_DATA) < 1)
		{
			closedir(dir);

			return 0;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			closedir(dir);

			return 0;
		}

		free(rmsg);
	}

	closedir(dir);

	if(net_SendEmptyCmd(SERV_LS_END) < 1)
	{
		return 0;
	}

	return 1;
}