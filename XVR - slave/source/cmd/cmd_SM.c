#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"

int net_cmd_SM(char* msg, int msgLen)
{
	char ermsg[4] = { NET_CMD_SEND_FILE_MASTER_SLAVE, 0, 0, 0 };
	
	FILE *f = fopen(msg, "rb");

	if(!f)
	{
		ermsg[1] = '-';

		if(net_SendData(ermsg, 4) < 1)
		{
			return -1;
		}

		return 1;
	}else{
		ermsg[1] = '+';
		
		if(net_SendData(ermsg, 4) < 1)
		{
			return -1;
		}
	}

	uint8* rmsg;
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	uint8* fcont = malloc(fsize + sizeof(uint8));
	memset(fcont, 0, fsize + sizeof(uint8));
	fread(fcont, 1, fsize, f);

	ermsg[0] = (fsize >> 24) & 0xFF;
	ermsg[1] = (fsize >> 16) & 0xFF;
	ermsg[2] = (fsize >> 8) & 0xFF;
	ermsg[3] = fsize & 0xFF;

	Sleep(10);

	if(net_SendData(ermsg, 4) < 1)
	{
		free(rmsg);
		free(fcont);
		fclose(f);

		return -1;
	}

	ermsg[0] = NET_CMD_SEND_FILE_MASTER_SLAVE;
	ermsg[1] = '+';
	ermsg[2] = 0;
	ermsg[3] = 0;

	int tries = 0;
	long i;
	long dataI = 2;
	uint8* data = malloc(NET_FILE_TRANSFER_PAGE + 2);
	memset(data, 0, NET_FILE_TRANSFER_PAGE + 2);

	for(i = 0; i != fsize; i++)
	{
		if(dataI == NET_FILE_TRANSFER_PAGE)
		{
			data[0] = NET_FILE_TRANSFER_DATA;
			data[1] = dataI - 2;
	
			if(net_SendData(data, dataI) < 1)
			{
				free(rmsg);
				free(fcont);
				free(data);
				fclose(f);
	
				return -1;
			}
	
			if(net_ReciveDataTimeout(&rmsg) < 1)
			{
				free(rmsg);
				free(fcont);
				free(data);
				fclose(f);
		
				return 1;
			}
		
			if(rmsg[1] != '+')
			{
				free(rmsg);
				free(fcont);
				free(data);
				fclose(f);
		
				return 1;
			}
	
			dataI = 2;
			memset(data, 0, NET_FILE_TRANSFER_PAGE + 2);
		}

		data[dataI++] = fcont[i];
	}

	if(dataI > 2)
	{
		data[0] = NET_FILE_TRANSFER_DATA;
		data[1] = dataI - 2;

		if(net_SendData(data, dataI) < 1)
		{
			free(rmsg);
			free(fcont);
			free(data);
			fclose(f);

			return -1;
		}

		if(net_ReciveDataTimeout(&rmsg) < 1)
		{
			free(rmsg);
			free(fcont);
			free(data);
			fclose(f);
	
			return 1;
		}
	
		if(rmsg[1] != '+')
		{
			free(rmsg);
			free(fcont);
			free(data);
			fclose(f);
	
			return 1;
		}

		dataI = 2;
		memset(data, 0, NET_FILE_TRANSFER_PAGE + 2);
	}

	data[0] = NET_FILE_TRANSFER_END;

	if(net_SendData(data, dataI) < 1)
	{
		free(rmsg);
		free(fcont);
		free(data);
		fclose(f);

		return -1;
	}

	free(rmsg);
	free(fcont);
	free(data);
	fclose(f);

	return 1;
}
