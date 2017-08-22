#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "net/commands.h"
#include "net/interface.h"
#include "logger.h"
#include "server.h"
#include "types.h"

int net_cmd_Echo(char* msg, int msgLen, SOCKET sock)
{
	net_formatCmd(&msg, &msgLen, NET_CMD_ECHO);
	net_makeToRawCmd(NET_CMD_ECHO, &msg, &msgLen);

	if(net_SendData(sock, msg, msgLen) < 1)
	{
		return -2;
	}

	char* rmsg;

	if(net_ReciveData(sock, &rmsg) < 1)
	{
		LOG(LOG_ERR, "We didn't receive back answer!\n");
		free(rmsg);

		return 1;
	}

	LOG(LOG_INFO, "%s\n", rmsg);
	free(rmsg);
	
	return 1;
}