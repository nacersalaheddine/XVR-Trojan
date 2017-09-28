#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "cmd/hdd/path.h"

int command_Remove(char* msg, int len)
{
	int pathLen = strlen((char*)hdd_Path) + strlen(msg);
	char* path = malloc(pathLen + sizeof(char));
	memset(path, 0, pathLen + sizeof(char));
	memcpy(path, hdd_Path, strlen(hdd_Path));
	memcpy(path + strlen(hdd_Path), msg, strlen(msg));
	free(msg);

	int rv = net_SendCmd((uint8*)path, pathLen, COMMANDS_REMOVE);
	
	if(rv < 1)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		free(path);

		return NET_LOST_CONNECTION;
	}

	uint8* rmsg;
	
	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(path);
		free(rmsg);

		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Timed out!\n");
		free(path);
		free(rmsg);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] != COMMANDS_APPROVE)
	{
		LOG(LOG_ERR, "Failed to delete file\n");
		free(rmsg);
		free(path);

		return COMMANDS_SUCC;
	}

	LOG(LOG_SUCC, "File deleted!\n");

	free(rmsg);
	free(path);

	return COMMANDS_SUCC;
}