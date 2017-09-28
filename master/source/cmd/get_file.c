#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "progressbar.h"
#include "cmd/hdd/path.h"

#define COMMAND_GET_FILE_END 0x2
#define COMMAND_GET_FILE_DATA 0x5

int command_Get_File(char* msg, int len)
{
	int argsIndex = 0;
	char* _sourc;

	argsIndex = commands_ExportArg(&msg, &_sourc, 0);

	if(!argsIndex)
	{
		free(_sourc);
		free(msg);

		return COMMANDS_UNKNOW_COMMAND;
	}

	int sourcLen = strlen((char*)hdd_Path) + strlen(_sourc);
	char* sourc = malloc(sourcLen + sizeof(char));
	memset(sourc, 0, sourcLen + sizeof(char));
	memcpy(sourc, hdd_Path, strlen(hdd_Path));
	memcpy(sourc + strlen(hdd_Path), _sourc, strlen(_sourc));
	free(_sourc);

	char* dest = malloc(strlen(msg + argsIndex) + sizeof(char));
	memset(dest, 0, strlen(msg + argsIndex) + sizeof(char));
	strcpy(dest, msg + argsIndex);
	free(msg);

	if(net_SendCmd((uint8*)sourc, strlen(sourc), COMMANDS_GET_FILE) == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		free(sourc);
		free(dest);

		return NET_LOST_CONNECTION;
	}

	free(sourc);
	uint8* rmsg;

	int rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);
		free(dest);

		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Time out!\n");
		free(rmsg);
		free(dest);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] != COMMANDS_APPROVE)
	{
		LOG(LOG_ERR, "Failed to get file!\n");
		free(rmsg);
		free(dest);

		return COMMANDS_SUCC;
	}

	uint32 size = (rmsg[1] & 0xFF) | (rmsg[2] & 0xFF) << 8 | (rmsg[3] & 0xFF) << 16 | (rmsg[4] & 0xFF) << 24;
	free(rmsg);

	LOG(LOG_INFO, "File size: %d %s\n", (size / 1024) < 1 ? size : size / 1024, (size / 1024) < 1 ? "B" : "KB");

	FILE* f = fopen(dest, "wb");

	free(dest);

	if(!f)
	{
		LOG(LOG_ERR, "Failed to create file!\n");

		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}

		return COMMANDS_SUCC;
	}

	if(net_SendCmd((uint8*)" ", 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		fclose(f);

		return NET_LOST_CONNECTION;
	}

	progressbar_Max = size;
	progressbar_Start();
	progressbar_WaitToStart();

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == COMMAND_GET_FILE_END)
		{
			break;
		}else if(rmsg[0] == COMMAND_GET_FILE_DATA){
			fwrite(rmsg + 1, 1, rv - 1, f);
			progressbar_Index += rv - 1;
		}

		free(rmsg);

		if(net_SendCmd((uint8*)" ", 1, 0) == NET_LOST_CONNECTION)
		{
			progressbar_CriticalStop();
			fclose(f);
		
			return NET_LOST_CONNECTION;
		}
	}

	fclose(f);
	free(rmsg);

	if(rv < 1)
	{
		progressbar_CriticalStop();
	}else{
		progressbar_Stop();
	}

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");

		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Time out!\n");

		return NET_LOST_CONNECTION;
	}

	LOG(LOG_SUCC, "Done!\n");

	return COMMANDS_SUCC;
}