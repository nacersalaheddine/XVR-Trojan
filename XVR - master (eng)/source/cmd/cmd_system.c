#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "net/commands.h"
#include "net/interface.h"
#include "logger.h"
#include "server.h"
#include "types.h"

int net_cmd_System(char* msg, int msgLen, SOCKET sock)
{
	net_formatCmd(&msg, &msgLen, NET_CMD_SYSTEM);
	net_makeToRawCmd(NET_CMD_SYSTEM, &msg, &msgLen);

	if(net_SendData(sock, msg, msgLen) < 1)
	{
		return -2;
	}

	char* rmsg;
	char nmsg[3] = { NET_CMD_SYSTEM, '+', 0 };

	while(1)
	{		
		if(net_ReciveDataTimeout(sock, &rmsg) < 1)
		{
			LOG(LOG_ERR, "We didn't receive back answer!\n");
			free(rmsg);

			return 1;
		}

		if(rmsg[0] == NET_CMD_SYSTEM)
		{
			break;
		}

		if(net_SendData(sock, nmsg, 3) < 1)
		{
			LOG(LOG_ERR, "Command was interrupted!\n");
			free(rmsg);

			return -1;
		}

		printf("%s", rmsg);
	}

	free(rmsg);
	
	return 1;
}