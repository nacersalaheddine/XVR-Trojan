#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "cmd/hdd/path.h"

#define COMMANDS_HDD_CD_NOT_FOUND 0x64
#define COMMANDS_HDD_CD_IS_FILE 0x53
#define COMMANDS_HDD_CD_PATH_APPEND 0x36
#define COMMANDS_HDD_CD_PATH_REPLACE 0x35
#define COMMANDS_HDD_CD_NEW_PATH 0xAD

int command_hdd_Cd(char* msg, int len)
{
	uint8* path = malloc(2048 + sizeof(unsigned char));
	memset(path, 0, 2048 + sizeof(unsigned char));
	memcpy(path, hdd_Path, strlen(hdd_Path));
	int pathLen = strlen(hdd_Path);

	int i;

	for(i = 0; i != strlen(msg); i++)
	{
		if(msg[i] == '/')
		{
			msg[i] = '\\';
		}
	}

	if(strcmp(msg, "..") == 0)
	{
		int countC = 0;

		for(i = 0; i != pathLen; i++)
		{
			if(path[i] == '\\')
			{
				countC++;
			}
		}

		if(countC > 1)
		{
			for(i = pathLen - 2; i != 0; i--)
			{
				if(path[i] == '\\')
				{
					if(countC == 2)
					{
						i += 1;
					}

					break;
				}
			}
		}

		free(msg);
		msg = malloc(i + sizeof(char));
		memset(msg, 0, i + sizeof(char));
		memcpy(msg, path, i);

		free(path);
		path = malloc(HDD_PATH_MAX);
		memset(path, 0, HDD_PATH_MAX);
		memcpy(path, msg, i);
	}else{
		memcpy(path + pathLen, msg, strlen(msg));
	}

	if(strlen((char*)path) >= HDD_PATH_MAX)
	{
		LOG(LOG_ERR, "Path to big!\n");
		free(msg);
		free(path);

		return COMMANDS_DONT_SEEDUP;
	}

	int rv = net_SendCmd(path, strlen((char*)path), COMMANDS_HDD_CD);
	
	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		free(msg);
		free(path);

		return NET_LOST_CONNECTION;
	}

	uint8 *rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);
		free(msg);
		free(path);

		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Timed out!\n");
		free(rmsg);
		free(msg);
		free(path);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] == COMMANDS_HDD_CD_IS_FILE)
	{
		LOG(LOG_ERR, "It's file or can't find the folder!\n");
	}else if(rmsg[0] == COMMANDS_HDD_CD_PATH_APPEND){
		memset(hdd_Path, 0, HDD_PATH_MAX);
		memcpy(hdd_Path, path, strlen((char*)path));

		if(path[strlen((char*)path) - 1] != '\\')
		{
			if(path[strlen((char*)path)] != '\\')
			{
				hdd_Path[strlen((char*)path)] = '\\';
			}
		}

		LOG(LOG_SUCC, "Path set to %s\n", hdd_Path);
	}else if(rmsg[0] == COMMANDS_HDD_CD_NEW_PATH){
		free(rmsg);
		
		if(net_SendData((uint8*)msg, strlen(msg)) == NET_LOST_CONNECTION)
		{
			LOG(LOG_ERR, "Failed to send!\n");
			free(msg);
			free(path);

			return NET_LOST_CONNECTION;
		}

		rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

		if(rv == NET_LOST_CONNECTION)
		{
			LOG(LOG_ERR, "Failed to receive!\n");
			free(rmsg);
			free(msg);
			free(path);
	
			return NET_LOST_CONNECTION;
		}else if(rv == NET_TIMED_OUT){
			LOG(LOG_ERR, "Timed out!\n");
			free(rmsg);
			free(msg);
			free(path);
	
			return NET_LOST_CONNECTION;
		}
	
		if(rmsg[0] == COMMANDS_HDD_CD_IS_FILE)
		{
			LOG(LOG_ERR, "It's file or can't find the folder!\n");			
		}else if(rmsg[0] == COMMANDS_HDD_CD_PATH_REPLACE){
			memset(hdd_Path, 0, HDD_PATH_MAX);
			memcpy(hdd_Path, msg, strlen(msg));

			if(msg[strlen(msg) - 1] != '\\')
			{
				if(msg[strlen(msg)] != '\\')
				{
					hdd_Path[strlen(msg)] = '\\';
				}
			}

			LOG(LOG_SUCC, "Path set to %s\n", hdd_Path);
		}
	}

	free(rmsg);
	free(msg);
	free(path);

	return COMMANDS_SUCC;
}