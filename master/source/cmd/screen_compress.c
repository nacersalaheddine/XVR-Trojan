#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "cmd/screen.h"

#define COMMANDS_SCREEN_IS_USING_COMPRESSOR_TRUE 1
#define COMMANDS_SCREEN_IS_USING_COMPRESSOR_FALSE 0

int screen_isUsingCompressor = 0;

int command_Screen_IsUsingCompressor(char* msg, int len)
{
	int val = COMMANDS_SCREEN_IS_USING_COMPRESSOR_FALSE;

	if(msg[0] == 't' || msg[0] == 'T' || msg[0] == '1')
	{
		val = COMMANDS_SCREEN_IS_USING_COMPRESSOR_TRUE;
	}

	free(msg);

	uint8 smsg[2];
	smsg[0] = val;
	smsg[1] = 0;

	int rv = net_SendCmd(smsg, 2, COMMANDS_SCREEN_IS_USING_COMPRESSOR);

	if(rv < 1)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		free(msg);

		return NET_LOST_CONNECTION;
	}

	uint8* rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Timed out!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}

	if(val == COMMANDS_SCREEN_IS_USING_COMPRESSOR_TRUE)
	{
		screen_isUsingCompressor = 1;

		LOG(LOG_SUCC, "Compressor is set!\n");
		LOG(LOG_INFO, "The image will be grayish!\n");
	}else{
		screen_isUsingCompressor = 0;

		LOG(LOG_SUCC, "Compressor is removed!\n");
	}
	
	free(rmsg);

	return COMMANDS_SUCC;
}