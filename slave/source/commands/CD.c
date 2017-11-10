#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"

#define CMDS_CD_ERR_FAILED 0x1
#define CMDS_CD_ERR_FILE 0x2

int cmds_CD(uint8* msg)
{
	net_SetBuffer(NET_DATA_BUFFSIZE);	
	struct stat fbuf;

	if(stat((char*)msg, &fbuf) == 0)
	{
		if(!S_ISDIR(fbuf.st_mode))
		{
			if(net_SendEmptyCmd(CMDS_CD_ERR_FILE) < 1)
			{
				return 0;
			}

			return 1;
		}

		if(net_SendEmptyCmd(CMDS_HDD_CD) < 1)
		{
			return 0;
		}

		return 1;
	}

	if(net_SendEmptyCmd(CMDS_CD_ERR_FAILED) < 1)
	{
		return 0;
	}

	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		return 0;
	}
	
	if(stat((char*)rmsg, &fbuf) == 0)
	{
		free(rmsg);

		if(!S_ISDIR(fbuf.st_mode))
		{
			if(net_SendEmptyCmd(CMDS_CD_ERR_FILE) < 1)
			{
				return 0;
			}

			return 1;
		}

		if(net_SendEmptyCmd(CMDS_HDD_CD) < 1)
		{
			return 0;
		}

		return 1;
	}

	free(rmsg);

	if(net_SendEmptyCmd(CMDS_CD_ERR_FAILED) < 1)
	{
		return 0;
	}

	return 1;
}