#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"
#include "progressbar.h"
#include "bmp.h"

#define SERV_SCREENGET_MAX_DATA 1980
#define SERV_SCREENGET_CMD_DATA 0x1
#define SERV_SCREENGET_END 0x2
#define SERV_SCREENGET_GOOD 0x3
#define SERV_SCREENGET_FAILED 0xF

typedef struct _serv_screenget_info
{
	uint32 width;
	uint32 height;
}SERV_SCREENGET_INFO;

typedef struct _serv_screenget_data
{
	uint16 len;
	uint8 data[SERV_SCREENGET_MAX_DATA];
}SERV_SCREENGET_DATA;

int serv_SCREENGET_screenget(char* path, SERV_SCREENGET_INFO sinfo)
{
	FILE *f = fopen(path, "wb");

	if(!f)
	{
		LOG(LOG_ERR, "Failed to open \"%s\"", path);

		return SERV_CMDS_DONT_SEEDUP;
	}

	if(net_SendCmd((uint8*)&sinfo, sizeof(SERV_SCREENGET_INFO), SERV_CMDS_SCREEN_GET) < 1)
	{
		net_PrintLastError();
		fclose(f);
	
		return SERV_CMDS_BREAK;
	}

	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		net_PrintLastError();
		fclose(f);

		return SERV_CMDS_BREAK;
	}

	if(rmsg[0] == SERV_SCREENGET_FAILED)
	{
		LOG(LOG_ERR, "Failed to capture screen!\n");
		free(rmsg);
		fclose(f);

		return SERV_CMDS_GOOD;
	}

	rmsg++;
	SERV_SCREENGET_INFO *rsinfo = (SERV_SCREENGET_INFO*)rmsg;
	rmsg--;

	uint32 scSize = rsinfo->width * rsinfo->height * 3;
	uint32 scWidth = rsinfo->width;
	uint32 scHeight = rsinfo->height;
	bmp_WriteHeader(f, rsinfo->width, rsinfo->height);

	LOG(LOG_TABLE, "Width: %d\n", scWidth);
	LOG(LOG_TABLE, "Height: %d\n", scHeight);
	LOG(LOG_TABLE, "Color depth: 24\n");
	LOG(LOG_TABLE, "Size: ");

	if(scSize / (1024 * 1024 * 1024))
	{
		printf("%.2f GB\n", (double)scSize / (1024.0 * 1024.0 * 1024.0));
	}else if(scSize / (1024 * 1024)){
		printf("%.2f MB\n", (double)scSize / (1024.0 * 1024.0));
	}else if(scSize / 1024){
		printf("%.2f KB\n", (double)scSize / 1024.0);
	}else{
		printf("%.2f B\n", (double)scSize);
	}

	free(rmsg);

	net_SetBuffer(NET_DATA_BUFFSIZE);
	progressbar_Create(scSize, PROGRESSBAR_TYPE_SIMPLE);

	if(prgs_op_Use)
	{
		LOG_NEWLINE();
	}
	
	int rv;
	SERV_SCREENGET_DATA* snData;

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == SERV_SCREENGET_END)
		{
			free(rmsg);

			break;
		}else if(rmsg[0] == SERV_SCREENGET_CMD_DATA){
			rmsg++;
			snData = (SERV_SCREENGET_DATA*)rmsg;
			rmsg--;

			fwrite(snData->data, 1, snData->len, f);
			free(rmsg);

			if(net_SendEmptyCmd(SERV_SCREENGET_GOOD) < 1)
			{
				net_PrintLastError();
				fclose(f);

				return SERV_CMDS_BREAK;
			}

			progressbar_Update(rv);
			continue;
		}

		free(rmsg);
	}

	fclose(f);

	if(rv < 1)
	{
		net_PrintLastError();
		
		return SERV_CMDS_BREAK;
	}

	progressbar_End();

	return SERV_CMDS_GOOD;
}

int serv_SCREENGET(char* msg)
{
	int stIdx = 0;
	char* argW;
	char* argH;

	if(!(stIdx = serv_cmds_ExportArg(&msg, &argW, 0)))
	{
		LOG(LOG_ERR, "Invalid arguments!\n");
		free(msg);

		return SERV_CMDS_DONT_SEEDUP;
	}
	
	if(!(stIdx = serv_cmds_ExportArg(&msg, &argH, stIdx)))
	{
		LOG(LOG_ERR, "Invalid arguments!\n");
		free(argW);
		free(msg);

		return SERV_CMDS_DONT_SEEDUP;
	}

	int _argW = atoi(argW);
	int _argH = atoi(argH);

	free(argW);
	free(argH);

	if(_argW > 100 || _argW < 0)
	{
		LOG(LOG_ERR, "The width value must be between 0 and 100!\n");

		free(msg);

		return SERV_CMDS_DONT_SEEDUP;
	}

	if(_argH > 100 || _argH < 0)
	{
		LOG(LOG_ERR, "The height value must be between 0 and 100!\n");
		free(msg);

		return SERV_CMDS_DONT_SEEDUP;
	}

	int len = strlen(msg + stIdx);
	char* argPath = malloc(len + sizeof(char));
	memset(argPath, 0, len + sizeof(char));
	strcpy(argPath, msg + stIdx);
	free(msg);

	SERV_SCREENGET_INFO sncCap;
	memset(&sncCap, 0, sizeof(SERV_SCREENGET_INFO));
	sncCap.width = _argW;
	sncCap.height = _argH;

	int rv = serv_SCREENGET_screenget(argPath, sncCap);

	net_SetBuffer(NET_BUFFSIZE);

	free(argPath);

	return rv;
}