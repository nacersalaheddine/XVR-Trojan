#include <string.h>
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

#define COMMANDS_SCREEN_IS_USING_COMPRESSOR_TRUE 1
#define COMMANDS_SCREEN_IS_USING_COMPRESSOR_FALSE 0

int screen_isUsingCompressor = 0;

int command_Screen_IsUsingCompressor(uint8* msg)
{
	if(net_SendData((uint8*)" ", 1) == NET_LOST_CONNECTION)
	{
		return NET_LOST_CONNECTION;
	}

	if(msg[0] == COMMANDS_SCREEN_IS_USING_COMPRESSOR_TRUE)
	{
		screen_isUsingCompressor = 1;
	}else{
		screen_isUsingCompressor = 0;
	}

	return COMMANDS_SUCC;
}