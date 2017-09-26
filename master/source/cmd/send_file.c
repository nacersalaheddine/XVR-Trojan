#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "progressbar.h"

#define COMMAND_SEND_FILE_END 0x6
#define COMMAND_SEND_FILE_DATA 0x66

int command_Send_File(char* msg, int len)
{
	int argsIndex = 0;
	char* dest;

	argsIndex = commands_ExportArg(&msg, &dest, 0);

	if(!argsIndex)
	{
		free(dest);
		free(msg);

		return COMMANDS_UNKNOW_COMMAND;
	}

	char* sourc = malloc(strlen(msg + argsIndex) + sizeof(char));
	memset(sourc, 0, strlen(msg + argsIndex) + sizeof(char));
	strcpy(sourc, msg + argsIndex);
	free(msg);

	FILE *f = fopen(sourc, "rb");

	if(!f)
	{
		LOG(LOG_ERR, "Failed to open %s\n", sourc);
		free(dest);
		free(sourc);

		return COMMANDS_SUCC;
	}

	free(sourc);

	fseek(f, 0, SEEK_END);
	uint32 size = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8* fileData = malloc(size + sizeof(uint8));
	memset(fileData, 0, size + sizeof(uint8));

	if(!fread(fileData, 1, size, f))
	{
		LOG(LOG_ERR, "Failed to read\n");
		free(dest);
		fclose(f);
	}

	fclose(f);

	if(net_SendCmd((uint8*)dest, strlen(dest), COMMANDS_SEND_FILE) == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		free(dest);

		return NET_LOST_CONNECTION;
	}

	free(dest);
	uint8* rmsg;

	int rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);
		free(fileData);

		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Time out!\n");
		free(rmsg);
		free(fileData);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] != COMMANDS_APPROVE)
	{
		LOG(LOG_ERR, "Failed to create file!\n");
		free(rmsg);
		free(fileData);

		return COMMANDS_SUCC;
	}

	free(rmsg);

	LOG(LOG_INFO, "File size: %d %s\n", (size / 1024) < 1 ? size : size / 1024, (size / 1024) < 1 ? "B" : "KB");

	uint32 i;
	uint32 bufferIndex = 0;
	uint8 buffer[NET_BUFFSIE_MAX_CONTENT];
	memset(buffer, 0, NET_BUFFSIE_MAX_CONTENT);

	progressbar_Max = size;
	progressbar_Start();
	progressbar_WaitToStart();

	for(i = 0; i != size; i++)
	{
		if(bufferIndex >= NET_BUFFSIE_MAX_CONTENT)
		{
			if(net_SendCmd(buffer, NET_BUFFSIE_MAX_CONTENT, COMMAND_SEND_FILE_DATA) == NET_LOST_CONNECTION)
			{
				LOG(LOG_ERR, "Failed to send!\n");
				free(fileData);

				return NET_LOST_CONNECTION;
			}

			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				LOG(LOG_ERR, "Failed to receive!\n");
				free(fileData);
				free(rmsg);

				return NET_LOST_CONNECTION;
			}

			progressbar_Index += bufferIndex;

			free(rmsg);
			memset(buffer, 0, NET_BUFFSIE_MAX_CONTENT);
			bufferIndex = 0;
		}

		buffer[bufferIndex++] = fileData[i];
	}

	free(fileData);

	if(bufferIndex != 0 && bufferIndex <= NET_BUFFSIE_MAX_CONTENT)
	{
		if(net_SendCmd(buffer, bufferIndex, COMMAND_SEND_FILE_DATA) == NET_LOST_CONNECTION)
		{
			LOG(LOG_ERR, "Failed to send!\n");

			return NET_LOST_CONNECTION;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			LOG(LOG_ERR, "Failed to receive!\n");
			free(rmsg);

			return NET_LOST_CONNECTION;
		}

		progressbar_Index += bufferIndex;

		free(rmsg);
		memset(buffer, 0, NET_BUFFSIE_MAX_CONTENT);
		bufferIndex = 0;
	}

	progressbar_Stop();

	if(net_SendCmd((uint8*)" ", 1, COMMAND_SEND_FILE_END) == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");

		return NET_LOST_CONNECTION;
	}

	LOG(LOG_SUCC, "Done!\n");

	return COMMANDS_SUCC;
}