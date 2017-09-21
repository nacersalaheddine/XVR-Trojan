#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

int command_Send(char* msg, int len)
{
	if(strlen(msg) >= NET_BUFFSIE_FOR_FILE)
	{
		LOG(LOG_ERR, "To big message!\n");
		free(msg);

		return COMMANDS_SUCC;
	}

	int rv = net_SendCmd((uint8*)msg, len, COMMANDS_SEND);

	if(rv < 1)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		free(msg);

		return NET_LOST_CONNECTION;
	}

	LOG(LOG_INFO, "Sended \"%s\"\n", msg);
	free(msg);

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
	}else{
		LOG(LOG_SUCC, "Received \"%s\"\n", rmsg);
	}

	free(rmsg);

	return COMMANDS_SUCC;
}