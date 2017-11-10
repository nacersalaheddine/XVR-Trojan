#include <stdio.h>
#include <stdlib.h>
#include "commands/cmds.h"
#include "net/file.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"

int cmds_FILESEND(uint8* msg)
{
	if(!net_File_Recv((char*)msg))
	{
		return 0;
	}

	net_SetBuffer(NET_BUFFSIZE);

	return 1;
}