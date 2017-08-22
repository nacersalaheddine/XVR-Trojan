#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"

int net_cmd_Echo(char* msg, int msgLen)
{
	if(net_SendData(NULL, msg, msgLen) < 1)
	{
		return -1;
	}
	
	return 1;
}