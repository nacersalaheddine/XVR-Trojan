#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "net/file.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "progressbar.h"

#define NET_FILE_MAX_DATA 1980

typedef struct __serv_file_info
{
	uint16 len;
	uint8 data[NET_FILE_MAX_DATA];
}SERV_FILE_INFO;

int net_File_Send(char* path, int sout)
{
	FILE *f = fopen(path, "rb");
	
	if(!f)
	{
		if(sout)
		{
			LOG(LOG_ERR, "Failed to open \"%s\"\n", path);
		}

		if(net_SendEmptyCmd(NET_FILE_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	fseeko64(f, 0, SEEK_END);
	uint64 flen = ftello64(f);
	fseeko64(f, 0, SEEK_SET);

	if(sout)
	{
		progressbar_Create(flen, PROGRESSBAR_TYPE_FILE);
	}

	if(net_SendEmptyCmd(NET_FILE_GOOD) < 1)
	{
		fclose(f);

		return 0;
	}

	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		fclose(f);

		return 0;
	}

	if(rmsg[0] != NET_FILE_GOOD)
	{
		if(sout)
		{
			LOG(LOG_ERR, "Failed to create the file!\n");
		}

		free(rmsg);
		fclose(f);

		return 1;
	}

	free(rmsg);
	net_SetBuffer(NET_DATA_BUFFSIZE);

	uint64 dataI = 0;
	uint32 dataBuffLen = NET_FILE_MAX_DATA;

	if(dataBuffLen > flen)
	{
		dataBuffLen = flen;
	}

	SERV_FILE_INFO fileInfo;

	while(1)
	{
		memset(&fileInfo, 0, sizeof(SERV_FILE_INFO));
		fread(fileInfo.data, 1, dataBuffLen, f);
		fileInfo.len = dataBuffLen;

		if(net_SendCmd((uint8*)&fileInfo, sizeof(SERV_FILE_INFO), NET_FILE_DATA) < 1)
		{
			fclose(f);

			return 0;
		}

		if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
		{
			fclose(f);

			return 0;
		}

		free(rmsg);

		if(sout)
		{
			progressbar_Update(fileInfo.len);
		}

		dataI += dataBuffLen;

		if(dataI >= flen)
		{
			break;
		}

		if(flen - dataI < dataBuffLen)
		{
			dataBuffLen = flen - dataI;
		}
	}

	fclose(f);

	if(sout)
	{
		progressbar_End();
	}

	if(net_SendEmptyCmd(NET_FILE_END) < 1)
	{
		if(sout)
		{
			LOG(LOG_WAR, "Lost connection but the file is sended!\n");
		}

		return 0;
	}

	return 1;
}

int net_File_Recv(char* path, int sout)
{
	uint8* rmsg;
	
	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		return 0;
	}

	if(rmsg[0] != NET_FILE_GOOD)
	{
		if(sout)
		{
			LOG(LOG_ERR, "Failed to open file!\n");
		}

		free(rmsg);

		return 1;
	}

	if(sout)
	{
		uint64 fsvSize = 0x0;
		memcpy(&fsvSize, rmsg + 1, sizeof(fsvSize));

		progressbar_Create(fsvSize, PROGRESSBAR_TYPE_FILE);
	}

	free(rmsg);

	FILE *f = fopen(path, "wb");

	if(!f)
	{
		if(sout)
		{
			LOG(LOG_ERR, "Failed to create \"%s\"\n", path);
		}

		if(net_SendEmptyCmd(NET_FILE_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	if(net_SendEmptyCmd(NET_FILE_GOOD) < 1)
	{
		fclose(f);

		return 0;
	}

	net_SetBuffer(NET_DATA_BUFFSIZE);

	int rv;
	SERV_FILE_INFO *fileInfo;

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == NET_FILE_END)
		{
			free(rmsg);

			break;
		}else if(rmsg[0] == NET_FILE_DATA){
			rmsg++;
			fileInfo = (SERV_FILE_INFO*)rmsg;
			rmsg--;

			fwrite(fileInfo->data, 1, fileInfo->len, f);

			if(net_SendEmptyCmd(NET_FILE_GOOD) < 1)
			{
				fclose(f);
				free(rmsg);

				return 0;
			}

			if(sout)
			{
				progressbar_Update(fileInfo->len);
			}

			free(rmsg);

			continue;
		}

		free(rmsg);
	}

	fclose(f);

	if(rv < 1)
	{
		return 0;
	}

	if(sout)
	{
		progressbar_End();
	}
	
	return 1;
}