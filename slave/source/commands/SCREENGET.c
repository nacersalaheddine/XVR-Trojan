#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"
#include "screenshot.h"

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

int cmds_SCREENGET(uint8* msg)
{
	SERV_SCREENGET_INFO* sinfo = (SERV_SCREENGET_INFO*)msg;
	SERV_SCREENGET_INFO rsinfo;
	memset(&rsinfo, 0, sizeof(SERV_SCREENGET_INFO));

	if(!screenshot_Calculate(&rsinfo.width, &rsinfo.height, sinfo->width, sinfo->height))
	{
		if(net_SendEmptyCmd(SERV_SCREENGET_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	uint8* scData;
	uint32 scSize;

	if(!(scSize = screenshot_Take(sinfo->width, sinfo->height, &scData)))
	{
		if(net_SendEmptyCmd(SERV_SCREENGET_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	if(!scData)
	{
		if(net_SendEmptyCmd(SERV_SCREENGET_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	if(net_SendCmd((uint8*)&rsinfo, sizeof(SERV_SCREENGET_INFO), CMDS_SCREEN_GET) < 1)
	{
		free(scData);

		return 0;
	}

	net_SetBuffer(NET_DATA_BUFFSIZE);

	int dataBuffLen = SERV_SCREENGET_MAX_DATA;
	uint32 dataI = 0;
	uint8* rmsg;

	if(dataBuffLen > scSize)
	{
		dataBuffLen = scSize;
	}

	SERV_SCREENGET_DATA snSData;
	
	while(1)
	{
		memset(&snSData, 0, sizeof(SERV_SCREENGET_DATA));
		memcpy(&snSData.data, scData + dataI, dataBuffLen);
		snSData.len = dataBuffLen;

		if(net_SendCmd((uint8*)&snSData, sizeof(SERV_SCREENGET_DATA), SERV_SCREENGET_CMD_DATA) < 1)
		{
			free(scData);

			return 0;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			free(scData);

			return 0;
		}

		free(rmsg);

		dataI += dataBuffLen;

		if(dataI >= scSize)
		{
			break;
		}

		if(scSize - dataI < dataBuffLen)
		{
			dataBuffLen = scSize - dataI;
		}
	}

	free(scData);

	if(net_SendEmptyCmd(SERV_SCREENGET_END) < 1)
	{
		return 0;
	}

	return 1;
}