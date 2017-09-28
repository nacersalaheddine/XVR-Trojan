#include <stdio.h>
#include <string.h>
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

int command_Remove(uint8* msg)
{
	int rv = remove((char*)msg);

	if(rv != 0)
	{
		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}
	}else{
		if(net_SendCmd((uint8*)" ", 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}
	}

	return COMMANDS_SUCC;
}