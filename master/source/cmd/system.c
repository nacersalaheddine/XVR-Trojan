#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

#define COMMAND_SYSTEM_DATA 0x2
#define COMMAND_SYSTEM_END 0x3

int command_System(char* msg, int len)
{
	int rv = net_SendCmd((uint8*)msg, len, COMMANDS_SYSTEM);

	free(msg);

	if(rv < 1)
	{
		LOG(LOG_ERR, "Failed to send!\n");

		return NET_LOST_CONNECTION;
	}

	uint8* rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);
		
		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Timed out!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] == COMMANDS_DISAPPROVE)
	{
		LOG(LOG_ERR, "Failed to start CMD!\n");
		free(rmsg);

		return COMMANDS_SUCC;
	}

	free(rmsg);
	int rcvMsg = 0;

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES + NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == COMMAND_SYSTEM_END)
		{
			break;
		}else if(rmsg[0] == COMMAND_SYSTEM_DATA){
			printf("%s", rmsg + 1);
			rcvMsg++;
		}

		if(net_SendCmd((uint8*)" ", 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
		{
			LOG(LOG_ERR, "Failed to send!\n");
			free(rmsg);

			return NET_LOST_CONNECTION;
		}

		free(rmsg);
	}

	free(rmsg);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		
		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Timed out!\n");

		return NET_LOST_CONNECTION;
	}

	if(rcvMsg < 1)
	{
		LOG(LOG_INFO, "Your command maybe has error ?\n");
	}

	return COMMANDS_SUCC;
}