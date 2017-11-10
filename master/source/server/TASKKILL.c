#include <stdlib.h>
#include <string.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"

#define SERV_TASKKILL_FAILED 0xF

int serv_TASKKILL(char* str)
{
	int pid = atoi(str);

	free(str);

	uint8 sval[4];
	sval[0] = ((pid >> 24) & 0xFF);
	sval[1] = ((pid >> 16) & 0xFF);
	sval[2] = ((pid >> 8) & 0xFF);
	sval[3] = (pid & 0xFF);

	int rv = net_SendCmd(sval, 4, SERV_CMDS_TASK_KILL);

	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	uint8 *rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);
	
	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}
	
	if(rmsg[0] == SERV_TASKKILL_FAILED)
	{
		LOG(LOG_ERR, "Failed to kill process!\n");
	}else{
		LOG(LOG_SUCC, "Process terminated!\n");
	}

	free(rmsg);

	return SERV_CMDS_GOOD;
}