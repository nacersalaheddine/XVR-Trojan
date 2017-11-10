#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "net/file.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"

#define NET_FILE_MAX_DATA 1980

typedef struct __serv_file_info
{
	uint16 len;
	uint8 data[NET_FILE_MAX_DATA];
}SERV_FILE_INFO;

int net_File_Send(char* path)
{
	FILE *f = fopen(path, "rb");
	
	if(!f)
	{
		if(net_SendEmptyCmd(NET_FILE_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	fseeko64(f, 0, SEEK_END);
	uint64 flen = ftello64(f);
	fseeko64(f, 0, SEEK_SET);

	uint8 sbuff[8];
	memcpy(sbuff, &flen, sizeof(flen));

	if(net_SendCmd(sbuff, sizeof(flen), NET_FILE_GOOD) < 1)
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

	if(net_SendEmptyCmd(NET_FILE_END) < 1)
	{
		return 0;
	}

	return 1;
}

int net_File_Recv(char* path)
{
	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		return 0;
	}

	if(rmsg[0] != NET_FILE_GOOD)
	{
		free(rmsg);

		return 1;
	}

	free(rmsg);

	FILE *f = fopen(path, "wb");

	if(!f)
	{
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
			free(rmsg);

			if(net_SendEmptyCmd(NET_FILE_GOOD) < 1)
			{
				fclose(f);

				return 0;
			}

			continue;
		}

		free(rmsg);
	}

	fclose(f);

	if(rv < 1)
	{
		return 0;
	}
	
	return 1;
}