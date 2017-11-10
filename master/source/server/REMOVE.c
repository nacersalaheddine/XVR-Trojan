#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"
#include "server/hdd/hdd.h"
#include "cmp.h"

#define SERV_REMOVE_TYPE_STD 0
#define SERV_REMOVE_TYPE_ALL 1

typedef struct __serv_remove_info
{
	uint32 fileRemoved;
	uint32 dirRemoved;
	uint32 fileError;
	uint32 dirError;
}SERV_REMOVE_INFO;

int serv_REMOVE_remove(char* path, char op)
{
	int dataLen = strlen(path) + 1;
	uint8* data = malloc(dataLen + sizeof(char));
	memset(data, 0, dataLen + sizeof(char));
	strcpy((char*)data + 1, path);
	data[0] = op;

	LOG(LOG_NONE, "Deleting \"%s\"\n", path);

	int rv = net_SendCmd(data, dataLen, SERV_CMDS_HDD_REMOVE);
	free(data);

	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	uint8* rmsg;
	int wtime = NET_RECV_TRIES;

	if(op == SERV_REMOVE_TYPE_ALL)
	{
		wtime = NET_RECV_TRIES * 2;
	}

	rv = net_ReceiveDataTimeout(&rmsg, wtime);

	if(rv < 1)
	{
		net_PrintLastError();
		
		return SERV_CMDS_BREAK;
	}

	if(rmsg[0] != SERV_CMDS_HDD_REMOVE)
	{
		LOG(LOG_ERR, "Failed to delete!\n");
		free(rmsg);
		
		return SERV_CMDS_GOOD;
	}

	rmsg++;
	SERV_REMOVE_INFO* remInfo = (SERV_REMOVE_INFO*)rmsg;
	rmsg--;

	LOG(LOG_TABLE, "Files: %d\n", remInfo->fileRemoved);
	LOG(LOG_TABLE, "Folders: %d\n", remInfo->dirRemoved);
	LOG(LOG_TABLE, "Errors: %d\n", remInfo->dirError + remInfo->fileError);
	free(rmsg);

	return SERV_CMDS_GOOD;
}

int serv_REMOVE(char* msg)
{
	int stIdx = SERV_REMOVE_TYPE_STD;
	char argType = 0;
	char* argPath;

	if(!(stIdx = serv_cmds_ExportArg(&msg, &argPath, 0)))
	{
		int msgLen = strlen(msg);
		argPath = malloc(msgLen + sizeof(char));
		memset(argPath, 0, msgLen + sizeof(char));
		strcpy(argPath, msg);
	}else{
		if(cmp_cmp(msg + stIdx, "-all"))
		{
			argType = SERV_REMOVE_TYPE_ALL;
		}else{
			free(msg);
			free(argPath);
			LOG(LOG_ERR, "Invalid argument \"%s\"\n", msg + stIdx);

			return SERV_CMDS_DONT_SEEDUP;
		}
	}

	free(msg);

	if(!hdd_IsValidPath(argPath))
	{
		LOG(LOG_ERR, "Invalid path!\n");
		free(argPath);

		return SERV_CMDS_DONT_SEEDUP;
	}

	int hdd_pathLen = strlen(hdd_Path);
	char* last_hddpath = malloc(hdd_pathLen + sizeof(char));
	memset(last_hddpath, 0, hdd_pathLen + sizeof(char));
	strcpy(last_hddpath, hdd_Path);

	if(!hdd_AppendPath(argPath))
	{
		LOG(LOG_ERR, "The path is too long!\n");
		free(last_hddpath);
		free(argPath);
		
		return SERV_CMDS_DONT_SEEDUP;
	}

	free(argPath);

	int rv = serv_REMOVE_remove(hdd_Path, argType);

	memset(hdd_Path, 0, HDD_BUFFSIZE);
	strcpy(hdd_Path, last_hddpath);
	free(last_hddpath);

	return rv;
}