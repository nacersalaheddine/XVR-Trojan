#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

int command_Terminate(void)
{
	if(net_SendCmd((uint8*)" ", 1, COMMANDS_TERMINATE) == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		LOG(LOG_INFO, "Is the slave connected ?\n");
		
		return NET_LOST_CONNECTION;
	}

	LOG(LOG_SUCC, "The slave should be terminated\n");

	return NET_LOST_CONNECTION;
}