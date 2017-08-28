#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "net/commands.h"
#include "net/interface.h"
#include "logger.h"
#include "server.h"
#include "types.h"

#define MBOX_MB_PLAIN 0x0
#define MBOX_MB_ERROR 0x1
#define MBOX_MB_QUESTION 0x2
#define MBOX_MB_WARNING 0x3
#define MBOX_MB_INFO 0x4

int net_cmd_Mbox(char* msg, int msgLen, SOCKET sock)
{
	net_formatCmd(&msg, &msgLen, NET_CMD_MESSAGEBOX);

	char* argTitle;
	char* argMsg;
	msg = net_exportArgs(msg, &argTitle);
	msg = net_exportArgs(msg, &argMsg);

	int type = MBOX_MB_PLAIN;

	if(!strcmp(msg, "ERROR"))
	{
		type = MBOX_MB_ERROR;
	}else if(!strcmp(msg, "QUESTION")){
		type = MBOX_MB_QUESTION;
	}else if(!strcmp(msg, "WARNING")){
		type = MBOX_MB_WARNING;
	}else if(!strcmp(msg, "INFO")){
		type = MBOX_MB_INFO;
	}

	int nmsgLen = strlen(argTitle) + strlen(argMsg) + 3 + sizeof(char);
	char* nmsg = malloc(nmsgLen);
	memset(nmsg, 0, nmsgLen);
	snprintf(nmsg, nmsgLen, "   %s%s", argTitle, argMsg);
	nmsg[0] = NET_CMD_MESSAGEBOX;
	nmsg[1] = type;
	nmsg[2] = strlen(argTitle);

	if(net_SendData(sock, nmsg, nmsgLen) < 1)
	{
		free(nmsg);

		return -2;
	}
	
	free(argTitle);
	free(argMsg);
	free(nmsg);
	
	return 1;
}