#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "net/commands.h"
#include "net/interface.h"
#include "logger.h"
#include "server.h"
#include "types.h"

int ms_totalSize = 0;
int ms_seconds = 0;
int ms_sendCount = 0;
int ms_sendCountLoop = 0;

DWORD ms_sendCountFunc()
{
	ms_seconds = 0;
	ms_totalSize = 0;
	int last_ms_sendCount = 0;

	while(ms_sendCountLoop)
	{
		Sleep(3000);
		last_ms_sendCount = ms_sendCount;
		Sleep(1000);
		ms_seconds += 4;
		
		if(!ms_sendCountLoop)
		{
			return;
		}

		LOG(LOG_INFO, "Пратихме %d/%d КБ (%d КБ/сек)\n", ms_sendCount / 1024, ms_totalSize / 1024, (ms_sendCount - last_ms_sendCount) / 1024);
	}
}

int net_cmd_MS(char* msg, int msgLen, SOCKET sock)
{
	ms_sendCountLoop = 0;
	ms_sendCount = 0;
	net_formatCmd(&msg, &msgLen, NET_CMD_SEND_FILE_MASTER_SLAVE);

	int spliter;
	char* arg;
	char* filePath = net_exportArgs(msg, &arg);
	
	if(!arg)
	{
		LOG(LOG_ERR, "Недостатачно аргументи!\n");

		return 1;
	}

	CreateThread(NULL, 0, ms_sendCountFunc, NULL, 0, NULL);
	ms_sendCountLoop = 1;

	FILE *f = fopen(filePath, "rb");

	if(!f)
	{
		LOG(LOG_ERR, "Неуспяхме да отворим/създадем файла от нашата страна!\n");
		free(arg);
		ms_sendCountLoop = 0;

		return 1;
	}
	
	spliter = strlen(arg);

	net_makeToRawCmd(NET_CMD_SEND_FILE_MASTER_SLAVE, &arg, &spliter);

	if(net_SendData(sock, arg, spliter) < 1)
	{
		fclose(f);
		free(arg);
		free(filePath);
		ms_sendCountLoop = 0;

		return -2;
	}

	int tries = 0;
	char* rmsg;
	
	while(1)
	{
		if(net_ReciveData(sock, &rmsg) < 1)
		{
			if(tries == NET_PROTECT_CPU_TRIES * 2)
			{
				LOG(LOG_ERR, "Неполучихме обратна връзка!\n");
				free(arg);
				free(filePath);
				free(rmsg);
				fclose(f);
				ms_sendCountLoop = 0;
		
				return 1;
			}

			tries++;
			NET_TRYING_SLEEP();
		}else{
			break;
		}
	}

	if(rmsg[1] == '-')
	{
		LOG(LOG_ERR, "Неуспяхме да отворим/създадем файла!\n");
		free(arg);
		free(filePath);
		free(rmsg);
		fclose(f);
		ms_sendCountLoop = 0;

		return 1;
	}

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	uint8* fcont = malloc(fsize + sizeof(uint8));
	memset(fcont, 0, fsize + sizeof(uint8));
	fread(fcont, 1, fsize, f);
	ms_totalSize = fsize;

	int i;
	int dataI = 2;
	uint8* data = malloc(NET_FILE_TRANSFER_PAGE + 2);
	memset(data, 0, NET_FILE_TRANSFER_PAGE + 2);

	for(i = 0; i != fsize; i++)
	{
		if(dataI == NET_FILE_TRANSFER_PAGE)
		{
			data[0] = NET_FILE_TRANSFER_DATA;
			data[1] = dataI - 2;

			if(net_SendData(sock, data, dataI) < 1)
			{
				LOG(LOG_ERR, "Прекъсна трансфера на файла!\n");
				free(rmsg);
				free(fcont);
				free(data);
				fclose(f);
				ms_sendCountLoop = 0;

				return -1;
			}

			tries = 0;

			while(1)
			{
				if(net_ReciveData(sock, &rmsg) < 1)
				{
					if(tries == NET_PROTECT_CPU_TRIES)
					{
						LOG(LOG_ERR, "Прекъсна трансфера на файла!\n");
						free(rmsg);
						free(fcont);
						free(data);
						fclose(f);
						ms_sendCountLoop = 0;

						return -1;
					}

					tries++;
					NET_TRYING_SLEEP();
				}else{
					break;
				}
			}

			if(rmsg[1] != '+')
			{
				LOG(LOG_ERR, "Прекъсна трансфера на файла!\n");
				free(rmsg);
				free(fcont);
				free(data);
				fclose(f);
				ms_sendCountLoop = 0;

				return -1;
			}

			ms_sendCount += data[1];

			dataI = 2;
			memset(data, 0, NET_FILE_TRANSFER_PAGE + 3);
		}

		data[dataI++] = fcont[i];
	}

	if(dataI != 2)
	{
		data[0] = NET_FILE_TRANSFER_DATA;
		data[1] = dataI - 2;

		if(net_SendData(sock, data, dataI) < 1)
		{
			LOG(LOG_ERR, "Прекъсна трансфера на файла!\n");
			free(rmsg);
			free(fcont);
			free(data);
			fclose(f);
			ms_sendCountLoop = 0;

			return -1;
		}

		tries = 0;

		while(1)
		{
			if(net_ReciveData(sock, &rmsg) < 1)
			{
				if(tries == NET_PROTECT_CPU_TRIES)
				{
					LOG(LOG_ERR, "Прекъсна трансфера на файла!\n");
					free(rmsg);
					free(fcont);
					free(data);
					fclose(f);
					ms_sendCountLoop = 0;

					return -1;
				}

				tries++;
				NET_TRYING_SLEEP();
			}else{
				break;
			}
		}

		if(rmsg[1] != '+')
		{
			LOG(LOG_ERR, "Прекъсна трансфера на файла!\n");
			free(rmsg);
			free(fcont);
			free(data);
			fclose(f);
			ms_sendCountLoop = 0;

			return -1;
		}

		dataI = 2;
		memset(data, 0, NET_FILE_TRANSFER_PAGE + 3);
	}
	
	data[0] = NET_FILE_TRANSFER_END;

	if(net_SendData(sock, data, dataI) < 1)
	{
		LOG(LOG_ERR, "Прекъсна трансфера на файла!\n");
		free(rmsg);
		free(fcont);
		free(data);
		fclose(f);
		ms_sendCountLoop = 0;

		return -1;
	}

	free(data);
	free(fcont);
	free(rmsg);
	fclose(f);
	ms_sendCountLoop = 0;

	LOG(LOG_SUCC, "Файла е пренесен успешно за %d сек. (%d KB) \n", ms_seconds, ms_sendCount / 1024);

	return 1;
}