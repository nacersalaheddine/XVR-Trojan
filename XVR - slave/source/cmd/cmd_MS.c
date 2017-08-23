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

		if(net_SendData(ermsg, 3) < 1)
		{
			return -1;
		}

		return 1;
	}else{
		ermsg[1] = '+';
		
		if(net_SendData(ermsg, 3) < 1)
		{
			return -1;
		}
	}

	int rmsgSize = 0;
	uint8* rmsg;

	while(1)
	{
		if(net_ReciveDataTimeout(&rmsg) < 1)
		{
			free(rmsg);
			fclose(f);

			return 1;
		}

		if(rmsg[0] != NET_FILE_TRANSFER_DATA || rmsg[0] == NET_FILE_TRANSFER_END)
		{
			free(rmsg);
			fclose(f);

			break;
		}

		rmsgSize = rmsg[1];
		fwrite(rmsg + 2, 1, rmsgSize, f);

		if(net_SendData(ermsg, 3) < 1)
		{
			free(rmsg);
			fclose(f);

			return -1;
		}	
	}

	free(rmsg);
	fclose(f);

	return 1;
}
