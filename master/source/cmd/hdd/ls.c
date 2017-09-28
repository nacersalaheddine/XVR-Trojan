#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "cmd/hdd/path.h"

#define COMMANDS_HDD_LS_END 0xDC
#define COMMANDS_HDD_LS_DATA_F 0xCD //file
#define COMMANDS_HDD_LS_DATA_D 0xCE //folder
#define COMMANDS_HDD_LS_DATA_U 0xCF //unknown
#define COMMANDS_HDD_LS_DATA_TO_BIG 0xCA
#define COMMANDS_HDD_LS_CANT_GET_INFO 0xC0

int command_hdd_Ls_Arg(char* msg, int len)
{
	uint8* path = malloc(HDD_PATH_MAX + sizeof(unsigned char));
	memset(path, 0, HDD_PATH_MAX + sizeof(unsigned char));
	memcpy(path, msg, strlen(msg));
	int pathLen = strlen(msg);
	free(msg);

	LOG(LOG_INFO, "Listing files and folder in %s\n", path);
	
	int rv = net_SendCmd(path, pathLen, COMMANDS_HDD_LS);

	free(path);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");

		return NET_LOST_CONNECTION;
	}

	uint8 *rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Timed out!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] == COMMANDS_DISAPPROVE)
	{
		LOG(LOG_ERR, "Invalid path\n");

		return COMMANDS_SUCC;
	}

	LOG(LOG_INFO, "  Type  Name\n");

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == COMMANDS_HDD_LS_END)
		{
			break;
		}else if(rmsg[0] == COMMANDS_HDD_LS_DATA_TO_BIG){
			LOG(LOG_ERR, "Path to big!\n");
		}else if(rmsg[0] == COMMANDS_HDD_LS_CANT_GET_INFO){
			LOG(LOG_ERR, "Failed to get info from file/dir\n");
		}else if(rmsg[0] == COMMANDS_HDD_LS_DATA_F){
			LOG(LOG_INFO, " ");
			
			if(log_Color)
			{
				LOG_plus_SetColor(log_colorPalette[LOG_COLOR_LS_FILE]);
				printf(" FILE  %s\n", rmsg + 1);
				LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
			}else{
				printf(" FILE  %s\n", rmsg + 1);
			}
		}else if(rmsg[0] == COMMANDS_HDD_LS_DATA_D){

			if(strcmp((char*)rmsg + 1, "..") != 0)
			{
				if(strcmp((char*)rmsg + 1, ".") != 0)
				{
					LOG(LOG_INFO, " ");
					
					if(log_Color)
					{
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_LS_FOLDER]);
						printf("  DIR  %s\n", rmsg + 1);
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
					}else{
						printf("  DIR  %s\n", rmsg + 1);
					}
				}
			}
		}else if(rmsg[0] == COMMANDS_HDD_LS_DATA_U){
			LOG(LOG_INFO, " ");

			if(log_Color)
			{
				LOG_plus_SetColor(log_colorPalette[LOG_COLOR_LS_UNWN]);
				printf(" UNWN  %s\n", rmsg + 1);
				LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
			}else{
				printf(" UNWN  %s\n", rmsg + 1);
			}
		}

		free(rmsg);

		if(net_SendCmd((uint8*)" ", 1, 0) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}
	}

	free(rmsg);
	
	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Timed out!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}

	return COMMANDS_SUCC;
}

int command_hdd_Ls(void)
{
	uint8* path = malloc(HDD_PATH_MAX + sizeof(unsigned char));
	memset(path, 0, HDD_PATH_MAX + sizeof(unsigned char));
	memcpy(path, hdd_Path, strlen(hdd_Path));
	int pathLen = strlen(hdd_Path);

	LOG(LOG_INFO, "Listing files and folder in %s\n", path);
	
	int rv = net_SendCmd(path, pathLen, COMMANDS_HDD_LS);

	free(path);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");

		return NET_LOST_CONNECTION;
	}

	uint8 *rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Timed out!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] == COMMANDS_DISAPPROVE)
	{
		LOG(LOG_ERR, "Invalid path\n");
		free(rmsg);

		return COMMANDS_SUCC;
	}

	free(rmsg);

	LOG(LOG_INFO, "  Type  Name\n");

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == COMMANDS_HDD_LS_END)
		{
			break;
		}else if(rmsg[0] == COMMANDS_HDD_LS_DATA_TO_BIG){
			LOG(LOG_ERR, "Path to big!\n");
		}else if(rmsg[0] == COMMANDS_HDD_LS_CANT_GET_INFO){
			LOG(LOG_ERR, "Failed to get info from file/dir\n");
		}else if(rmsg[0] == COMMANDS_HDD_LS_DATA_F){
			LOG(LOG_INFO, " ");
			
			if(log_Color)
			{
				LOG_plus_SetColor(log_colorPalette[LOG_COLOR_LS_FILE]);
				printf(" FILE  %s\n", rmsg + 1);
				LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
			}else{
				printf(" FILE  %s\n", rmsg + 1);
			}
		}else if(rmsg[0] == COMMANDS_HDD_LS_DATA_D){
			if(strcmp((char*)rmsg + 1, "..") != 0)
			{
				if(strcmp((char*)rmsg + 1, ".") != 0)
				{
					LOG(LOG_INFO, " ");

					if(log_Color)
					{
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_LS_FOLDER]);
						printf("  DIR  %s\n", rmsg + 1);
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
					}else{
						printf("  DIR  %s\n", rmsg + 1);
					}
				}
			}
		}else if(rmsg[0] == COMMANDS_HDD_LS_DATA_U){
			LOG(LOG_INFO, " ");

			if(log_Color)
			{
				LOG_plus_SetColor(log_colorPalette[LOG_COLOR_LS_UNWN]);
				printf(" UNWN  %s\n", rmsg + 1);
				LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
			}else{
				printf(" UNWN  %s\n", rmsg + 1);
			}
		}

		free(rmsg);

		if(net_SendCmd((uint8*)" ", 1, 0) == NET_LOST_CONNECTION)
		{
			return NET_LOST_CONNECTION;
		}
	}

	free(rmsg);
	
	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");

		return NET_LOST_CONNECTION;
	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Timed out!\n");

		return NET_LOST_CONNECTION;
	}

	return COMMANDS_SUCC;
}