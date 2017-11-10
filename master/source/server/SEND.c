#include <stdlib.h>
#include <string.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"

int serv_SEND(char* str)
{
	int rv = net_SendCmd((uint8*)str, strlen(str), SERV_CMDS_SEND);

	LOG(LOG_WAR, "Sended: %s\n", str);

	if(rv < 1)
	{
		net_PrintLastError();
		free(str);

		return SERV_CMDS_BREAK;
	}

	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		net_PrintLastError();
		free(str);

		return SERV_CMDS_BREAK;
	}

	if(strcmp(str, (char*)rmsg) == 0)
	{
		LOG(LOG_SUCC, "Received: %s\n", rmsg);
	}else{
		LOG(LOG_ERR, "Received: %s\n", rmsg);
	}

	free(rmsg);
	free(str);

	return SERV_CMDS_GOOD;
}