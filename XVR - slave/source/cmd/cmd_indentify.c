#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"

int net_cmd_Indentify(char* msg, int msgLen)
{
	int buffLen = 50;
	char rval[50];
	memset(rval, 0, 50);

	if(!GetComputerName(rval + 2, &buffLen))
	{
		if(net_SendData("-C\x0", 3) < 1)
		{
			return -1;
		}
	}else{
		rval[0] = '+';
		rval[1] = 'C';

		if(net_SendData(rval, buffLen + 2) < 1)
		{
			return -1;
		}
	}

	buffLen = 50;
	memset(rval, 0, 50);

	if(!GetUserName(rval + 2, &buffLen))
	{
		if(net_SendData("-U\x0", 3) < 1)
		{
			return -1;
		}
	}else{
		rval[0] = '+';
		rval[1] = 'U';

		if(net_SendData(rval, buffLen + 2) < 1)
		{
			return -1;
		}
	}

	int tval = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	buffLen = 50;
	memset(rval, 0, 50);
	rval[0] = '+';
	rval[1] = 'X';
	rval[2] = (tval >> 24) & 0xFF;
	rval[3] = (tval >> 16) & 0xFF;
	rval[4] = (tval >> 8) & 0xFF;
	rval[5] = tval & 0xFF;

	if(net_SendData(rval, 6) < 1)
	{
		return -1;
	}

	tval = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	rval[0] = '+';
	rval[1] = 'Y';
	rval[2] = (tval >> 24) & 0xFF;
	rval[3] = (tval >> 16) & 0xFF;
	rval[4] = (tval >> 8) & 0xFF;
	rval[5] = tval & 0xFF;

	if(net_SendData(rval, 6) < 1)
	{
		return -1;
	}

	return 1;
}