#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server/serv_cmds.h"
#include "net/file.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"
#include "server/hdd/hdd.h"

int serv_FILEGET_fileget(char* dest, char* src)
{
	int rv = net_SendCmd((uint8*)dest, strlen(dest), SERV_CMDS_FILE_GET);
	
	if(rv < 1)
	{
		net_PrintLastError();

		return 0;
	}

	if(!net_File_Recv(src, 1))
	{
		net_PrintLastError();
		
		return 0;
	}

	net_SetBuffer(NET_BUFFSIZE);

	return 1;
}

int serv_FILEGET(char* msg)
{
	int stIdx = 0;
	char* argSrc;
	char* argDest;

	if(!(stIdx = serv_cmds_ExportArg(&msg, &argSrc, 0)))
	{
		LOG(LOG_ERR, "Invalid arguments!\n");
		free(msg);

		return SERV_CMDS_DONT_SEEDUP;
	}

	int len = strlen(msg + stIdx);
	argDest = malloc(len + sizeof(char));
	memset(argDest, 0, len + sizeof(char));
	strcpy(argDest, msg + stIdx);
	
	free(msg);

	if(!hdd_IsValidPath(argDest))
	{
		LOG(LOG_ERR, "Invalid path!\n");
		free(argDest);
		free(argSrc);

		return SERV_CMDS_DONT_SEEDUP;
	}

	int hdd_pathLen = strlen(hdd_Path);
	char* last_hddpath = malloc(hdd_pathLen + sizeof(char));
	memset(last_hddpath, 0, hdd_pathLen + sizeof(char));
	strcpy(last_hddpath, hdd_Path);

	if(!hdd_AppendPath(argDest))
	{
		LOG(LOG_ERR, "The path is too long!\n");
		free(last_hddpath);
		free(argDest);
		free(argSrc);
		
		return SERV_CMDS_DONT_SEEDUP;
	}

	free(argDest);

	int rv = serv_FILEGET_fileget(hdd_Path, argSrc);
	
	memset(hdd_Path, 0, HDD_BUFFSIZE);
	strcpy(hdd_Path, last_hddpath);
	free(last_hddpath);
	free(argSrc);

	return rv;
}
