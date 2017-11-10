#include <stdlib.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"

int serv_TERMINATE(void)
{
	int rv = net_SendEmptyCmd(SERV_CMDS_TERMINATE);

	if(rv < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	uint8* rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	free(rmsg);

	if(rv < 1)
	{
		LOG(LOG_WAR, "The slave may not be terminated...\n");
	}else{
		LOG(LOG_SUCC, "Terminated!\n");
	}

	return SERV_CMDS_BREAK;
}
