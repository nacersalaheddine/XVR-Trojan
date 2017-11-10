#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"
#include "server/hdd/hdd.h"

#define SERV_LS_INFO_MAX_COUNT 3072

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

int serv_ls_Info_Count = 0;
SERV_LS_INFO *serv_ls_Info[SERV_LS_INFO_MAX_COUNT];

void serv_LS_printData(int idx, int nmLen)
{
	if(strlen(serv_ls_Info[idx]->fname) < 1)
	{
		return;
	}

	LOG(LOG_NONE, "  ");

	if(serv_ls_Info[idx]->type == SERV_LS_FILE)
	{
		LOG_SetColor(LOG_COLOR_LS_FILE);
	}else if(serv_ls_Info[idx]->type == SERV_LS_FOLDER){
		LOG_SetColor(LOG_COLOR_LS_FOLDER);
	}else{
		LOG_SetColor(LOG_COLOR_LS_UNKNOWN);
	}

	LOG_TablePrint(nmLen, "%s", serv_ls_Info[idx]->fname);
	LOG_SetColor(LOG_COLOR_TEXT);

	printf("  ");

	LOG_TablePrint(6, "%.2lf", serv_ls_Info[idx]->size);

	putchar(' ');

	if(serv_ls_Info[idx]->sizeType == (uint32)SERV_LS_LEN_GB)
	{
		printf("GB");
	}else if(serv_ls_Info[idx]->sizeType == SERV_LS_LEN_MB){
		printf("MB");
	}else if(serv_ls_Info[idx]->sizeType == SERV_LS_LEN_KB){
		printf("KB");
	}else{
		printf("B ");
	}

	struct tm* tm1 = localtime(&serv_ls_Info[idx]->cTime);
	struct tm* tm2 = localtime(&serv_ls_Info[idx]->mTime);
	
	printf("  ");
	LOG_TablePrint(22, "%d\\%d\\%d %d:%d:%d", tm1->tm_mday, tm1->tm_mon + 1, tm1->tm_year + 1900, tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
	printf("  ");
	LOG_TablePrint(22, "%d\\%d\\%d %d:%d:%d", tm2->tm_mday, tm2->tm_mon + 1, tm2->tm_year + 1900, tm2->tm_hour, tm2->tm_min, tm2->tm_sec);
	LOG_NEWLINE();
}

int serv_LS_ls(char* msg)
{
	int rv = net_SendCmd((uint8*)msg, strlen(msg), SERV_CMDS_HDD_LS);

	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	net_SetBuffer(NET_DATA_BUFFSIZE);
	uint8* rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	if(rmsg[0] == SERV_LS_FAILED)
	{
		LOG(LOG_ERR, "Failed list content!\n");
		free(rmsg);

		return SERV_CMDS_GOOD;
	}

	free(rmsg);
	serv_ls_Info_Count = 0;
	int largestName = 0;
	int rcc = 0;

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == SERV_LS_END)
		{
			free(rmsg);

			break;
		}else if(rmsg[0] == SERV_LS_DATA){
			serv_ls_Info[serv_ls_Info_Count] = malloc(sizeof(SERV_LS_INFO));
			memcpy(serv_ls_Info[serv_ls_Info_Count], rmsg + 1, sizeof(SERV_LS_INFO));

			if(strlen(serv_ls_Info[serv_ls_Info_Count]->fname) > largestName)
			{
				largestName = strlen(serv_ls_Info[serv_ls_Info_Count]->fname);
			}

			serv_ls_Info_Count++;

			if(!rcc)
			{
				rcc++;
			}

			if(serv_ls_Info_Count >= SERV_LS_INFO_MAX_COUNT)
			{
				LOG(LOG_WAR, "Buffer overflow!\n");
				rcc = 0;
			}
		}

		free(rmsg);

		if(net_SendEmptyCmd(SERV_CMDS_HDD_LS) < 1)
		{
			net_PrintLastError();

			return SERV_CMDS_BREAK;
		}
	}

	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	if(rcc < 1)
	{
		LOG(LOG_WAR, "Folder is empty!\n");

		return SERV_CMDS_GOOD;
	}

	int i;
	LOG(LOG_NONE, "  ");
	LOG_TablePrint(largestName + 2, "Name");
	LOG_TablePrint(8, "Size");
	LOG_TablePrint(24, "Created");
	LOG_TablePrint(22, "Modfied");
	LOG_NEWLINE();

	for(i = 0; i != serv_ls_Info_Count; i++)
	{
		if(!serv_ls_Info[i])
		{
			continue;
		}

		serv_LS_printData(i, largestName);
		free(serv_ls_Info[i]);
	}

	return SERV_CMDS_GOOD;
}

int serv_LS_Path(char* path)
{
	if(!hdd_IsValidPath(path))
	{
		LOG(LOG_ERR, "Invalid path!\n");
		free(path);

		return SERV_CMDS_DONT_SEEDUP;
	}

	int hdd_pathLen = strlen(hdd_Path);
	char* last_hddpath = malloc(hdd_pathLen + sizeof(char));
	memset(last_hddpath, 0, hdd_pathLen + sizeof(char));
	strcpy(last_hddpath, hdd_Path);

	if(!hdd_AppendPath(path))
	{
		LOG(LOG_ERR, "The path is too long!\n");
		free(last_hddpath);
		free(path);
		
		return SERV_CMDS_DONT_SEEDUP;
	}

	free(path);

	int rv = serv_LS_ls(hdd_Path);

	memset(hdd_Path, 0, HDD_BUFFSIZE);
	strcpy(hdd_Path, last_hddpath);
	free(last_hddpath);

	return rv;
}

int serv_LS(void)
{
	return serv_LS_ls(hdd_Path);
}