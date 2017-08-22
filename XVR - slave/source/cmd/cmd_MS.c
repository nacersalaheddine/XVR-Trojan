#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"

int net_cmd_MS(char* msg, int msgLen)
{
	char ermsg[3] = { NET_CMD_SEND_FILE_MASTER_SLAVE, 0, 0 };
	FILE *f = fopen(msg, "wb");

	if(!f)
	{
		ermsg[1] = '-';

		if(net_SendData(NULL, ermsg, 3) < 1)
		{
			return -1;
		}

		return 1;
	}else{
		ermsg[1] = '+';
		
		if(net_SendData(NULL, ermsg, 3) < 1)
		{
			return -1;
		}
	}

	int tries = 0;
	int rmsgSize = 0;
	uint8* rmsg;
	ermsg[1] = '+';
	
	while(1)
	{
		tries = 0;

		while(1)
		{
			if(net_ReciveData(NULL, &rmsg) < 1)
			{
				if(tries == NET_PROTECT_CPU_TRIES)
				{
					free(rmsg);
					fclose(f);

					return 1;
				}

				tries++;
				NET_TRYING_SLEEP();
			}else{
				break;
			}
		}		

		if(rmsg[0] != NET_FILE_TRANSFER_DATA || rmsg[0] == NET_FILE_TRANSFER_END)
		{
			free(rmsg);
			fclose(f);

			break;
		}
		
		rmsgSize = rmsg[1];
		*rmsg++;
		*rmsg++;

		fwrite(rmsg, 1, rmsgSize, f);

		if(net_SendData(NULL, ermsg, 3) < 1)
		{
			free(rmsg);
			fclose(f);

			return -1;
		}

		*rmsg--;
		*rmsg--;
	}

	free(rmsg);
	fclose(f);

	return 1;
}
