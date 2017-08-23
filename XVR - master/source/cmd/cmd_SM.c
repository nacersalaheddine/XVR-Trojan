#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "net/commands.h"
#include "net/interface.h"
#include "logger.h"
#include "server.h"
#include "types.h"

int sm_canOuput = 0;
int sm_totalSize = 0;
int sm_seconds = 0;
int sm_recvCount = 0;
int sm_recvCountLoop = 0;

DWORD sm_recvCountFunc()
{
	sm_canOuput = 0;
	sm_totalSize = 0;
	sm_seconds = 0;
	int last_sm_recvCount = 0;

	while(sm_recvCountLoop)
	{
		Sleep(3000);
		last_sm_recvCount = sm_recvCount;
		Sleep(1000);
		sm_seconds += 4;
		
		if(!sm_recvCountLoop)
		{
			return;
		}

		if(!sm_canOuput)
		{
			continue;
		}

		LOG(LOG_INFO, "Recived %d/%d KB (%d KB/sec)\n", sm_recvCount / 1024, sm_totalSize / 1024, (sm_recvCount - last_sm_recvCount) / 1024);
	}
}

int net_cmd_SM(char* msg, int msgLen, SOCKET sock)
{
	sm_recvCountLoop = 0;
	sm_recvCount = 0;
	net_formatCmd(&msg, &msgLen, NET_CMD_SEND_FILE_SLAVE_MASTER);

	int spliter;
	char* arg;
	char* filePath = net_exportArgs(msg, &arg);

	if(!arg)
	{
		free(filePath);
		LOG(LOG_ERR, "Not enough arguments!\n");

		return 1;
	}

	CreateThread(NULL, 0, sm_recvCountFunc, NULL, 0, NULL);
	sm_recvCountLoop = 1;

	FILE *f = fopen(filePath, "wb");

	if(!f)
	{
		LOG(LOG_ERR, "Couldn't create/open the file on the ower side!\n");
		sm_recvCountLoop = 0;

		return 1;
	}
	
	net_reciveTries = NET_RECV_TRIES * 2;
	spliter = strlen(arg);
	net_makeToRawCmd(NET_CMD_SEND_FILE_SLAVE_MASTER, &arg, &spliter);

	if(net_SendData(sock, arg, spliter) < 1)
	{
		free(arg);
		free(filePath);
		fclose(f);
		sm_recvCountLoop = 0;

		return -2;
	}

	uint8* rmsg;

	if(net_ReciveDataTimeout(sock, &rmsg) < 1)
	{
		LOG(LOG_ERR, "We didn't receive back answer!\n");
		free(arg);
		free(filePath);
		fclose(f);
		sm_recvCountLoop = 0;

		return 1;
	}

	if(rmsg[1] != '+')
	{
		LOG(LOG_ERR, "Couldn't create/open the file!\n");
		free(arg);
		free(filePath);
		fclose(f);
		sm_recvCountLoop = 0;

		return 1;
	}

	if(net_ReciveDataTimeout(sock, &rmsg) < 1)
	{
		LOG(LOG_ERR, "We didn't receive file size!\n");
		free(arg);
		free(filePath);
		fclose(f);
		sm_recvCountLoop = 0;

		return 1;
	}

	sm_totalSize = ((rmsg[0] & 0xFF) << 24) | ((rmsg[1] & 0xFF) << 16) | ((rmsg[2] & 0xFF) << 8) | (rmsg[3] & 0xFF);	
	sm_canOuput = 1;

	int rmsgSize = 0;
	char ermsg[3] = { NET_CMD_SEND_FILE_MASTER_SLAVE, '+', 0 };
	net_reciveTries = NET_RECV_TRIES;

	while(1)
	{
		if(net_ReciveDataTimeout(sock, &rmsg) < 1)
		{
			free(rmsg);
			fclose(f);
			LOG(LOG_ERR, "File receive was interrupted!\n");
			sm_recvCountLoop = 0;

			return -1;
		}

		if(rmsg[0] != NET_FILE_TRANSFER_DATA || rmsg[0] == NET_FILE_TRANSFER_END)
		{
			LOG(LOG_SUCC, "The file was received successfully in %d sec. (%d KB)\n", sm_seconds, sm_recvCount / 1024);			

			break;
		}

		rmsgSize = rmsg[1];
		sm_recvCount += rmsgSize;
		fwrite(rmsg + 2, 1, rmsgSize, f);

		if(net_SendData(sock, ermsg, 3) < 1)
		{
			free(rmsg);
			fclose(f);
			LOG(LOG_ERR, "File receive was interrupted!\n");
			sm_recvCountLoop = 0;

			return -1;
		}
	}

	free(rmsg);
	fclose(f);
	sm_recvCountLoop = 0;

	return 1;
}