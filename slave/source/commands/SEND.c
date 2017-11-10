#include <string.h>
#include "net/interface.h"
#include "net/error.h"
#include "types.h"

int cmds_SEND(uint8* msg)
{
	if(net_SendData(msg, strlen((char*)msg)) < 1)
	{
		return 0;
	}

	return 1;
}