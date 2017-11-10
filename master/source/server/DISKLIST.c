#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"

#define SERV_DISKLIST_TYPE_GB 1
#define SERV_DISKLIST_TYPE_MB 2
#define SERV_DISKLIST_TYPE_KB 3
#define SERV_DISKLIST_TYPE_B 0
#define SERV_DISKLIST_DATA 0x1
#define SERV_DISKLIST_END 0x2

typedef struct __serv_disklist_info
{
	char disk;
	char info;
	char hasSize;
	char totalType;
	char leftType;
	char usedType;
	uint32 total;
	uint32 left;
	uint32 used;
}SERV_DISKLIST_INFO;

void serv_DISKLIST_printType(int type)
{
	if(type == SERV_DISKLIST_TYPE_GB)
	{
		printf("GB\n");
	}else if(type == SERV_DISKLIST_TYPE_MB){
		printf("MB\n");
	}else if(type == SERV_DISKLIST_TYPE_KB){
		printf("KB\n");
	}else{
		printf("B\n");
	}
}

void serv_DISKLIST_printInfo(uint8* msg)
{
	SERV_DISKLIST_INFO* sdInfo = (SERV_DISKLIST_INFO*)msg;

	LOG(LOG_NONE, "Drive %c:\\\\\n", sdInfo->disk);
	
	LOG(LOG_TABLE, "Type: ");

	if(sdInfo->info == DRIVE_NO_ROOT_DIR){
		LOG_SetColor(LOG_COLOR_DISKLIST_NO_ROOT_DIR);
		printf("No Volume");
	}else if(sdInfo->info == DRIVE_REMOVABLE){
		LOG_SetColor(LOG_COLOR_DISKLIST_REMOVABLE);
		printf("Removable");
	}else if(sdInfo->info == DRIVE_FIXED){
		LOG_SetColor(LOG_COLOR_DISKLIST_FIXED);
		printf("Fixed");
	}else if(sdInfo->info == DRIVE_REMOTE){
		LOG_SetColor(LOG_COLOR_DISKLIST_REMOTE);
		printf("Network");
	}else if(sdInfo->info == DRIVE_CDROM){
		LOG_SetColor(LOG_COLOR_DISKLIST_CDROM);
		printf("CD-Rom");
	}else if(sdInfo->info == DRIVE_RAMDISK){
		LOG_SetColor(LOG_COLOR_DISKLIST_RAMDISK);
		printf("RAM");
	}else{
		LOG_SetColor(LOG_COLOR_DISKLIST_UNKNOWN);
		printf("Unknown");
	}

	LOG_SetColor(LOG_COLOR_TEXT);
	LOG_NEWLINE();

	if(sdInfo->hasSize != 1)
	{
		LOG(LOG_TABLE, "No information\n");

		return;
	}

	LOG(LOG_TABLE, "Total: %d ", sdInfo->total);
	serv_DISKLIST_printType(sdInfo->totalType);

	LOG(LOG_TABLE, "Free: %d ", sdInfo->left);
	serv_DISKLIST_printType(sdInfo->leftType);

	LOG(LOG_TABLE, "Used: %d ", sdInfo->used);
	serv_DISKLIST_printType(sdInfo->usedType);
}

int serv_DISKLIST(void)
{
	int rv = net_SendEmptyCmd(SERV_CMDS_DISK_LIST);

	if(rv < 1)
	{
		net_PrintLastError();
		
		return SERV_CMDS_BREAK;
	}

	uint8 *rmsg;

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == SERV_DISKLIST_END)
		{
			free(rmsg);

			break;
		}else if(rmsg[0] == SERV_DISKLIST_DATA){
			serv_DISKLIST_printInfo(rmsg + 1);
			LOG_NEWLINE();
		}

		free(rmsg);

		if(net_SendEmptyCmd(SERV_CMDS_DISK_LIST) < 1)
		{
			net_PrintLastError();
			
			return SERV_CMDS_BREAK;
		}
	}

	putchar('\b');

	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	return SERV_CMDS_GOOD;
}
