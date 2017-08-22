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

	while(1)
	{		
		if(net_ReciveData(sock, &rmsg) < 1)
		{
			LOG(LOG_ERR, "Неполучихме обратна връзка!\n");
			free(rmsg);

			return 1;
		}

		if(rmsg[0] == NET_CMD_SYSTEM)
		{
			break;
		}
	
		LOG(LOG_INFO, "%s", rmsg);
	}

	free(rmsg);
	
	return 1;
}