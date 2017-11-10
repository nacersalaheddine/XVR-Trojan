#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"
#include "server/hdd/hdd.h"

#define CMDS_CD_ERR_FAILED 0x1
#define CMDS_CD_ERR_FILE 0x2

int serv_CD(char* msg)
{
	if(!hdd_IsValidPath(msg))
	{
		LOG(LOG_ERR, "Invalid path!\n");
		free(msg);

		return SERV_CMDS_DONT_SEEDUP;
	}

	int hdd_pathLen = strlen(hdd_Path);
	char* last_hddpath = malloc(hdd_pathLen + sizeof(char));
	memset(last_hddpath, 0, hdd_pathLen + sizeof(char));
	strcpy(last_hddpath, hdd_Path);

	if(!hdd_AppendPath(msg))
	{
		LOG(LOG_ERR, "The path is too long!\n");
		free(last_hddpath);
		free(msg);
		
		return SERV_CMDS_DONT_SEEDUP;
	}

	int rv = net_SendCmd((uint8*)hdd_Path, strlen(hdd_Path), SERV_CMDS_HDD_CD);

	if(rv < 1)
	{
		net_PrintLastError();
		free(last_hddpath);
		free(msg);

		return SERV_CMDS_BREAK;
	}

	net_SetBuffer(NET_DATA_BUFFSIZE);	
	int smsg = 0;
	uint8 *rmsg;

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == CMDS_CD_ERR_FILE)
		{
			LOG(LOG_ERR, "You must choise a directory\n");
			free(last_hddpath);
			free(rmsg);
			free(msg);

			return SERV_CMDS_GOOD;
		}else if(rmsg[0] == CMDS_CD_ERR_FAILED){
			if(smsg == 0)
			{
				if(net_SendData((uint8*)msg, strlen(msg)) < 1)
				{
					net_PrintLastError();
					free(last_hddpath);
					free(msg);
					
					return SERV_CMDS_BREAK;
				}

				smsg++;
			}else{
				LOG(LOG_ERR, "Failed\n");
				memset(hdd_Path, 0, HDD_BUFFSIZE);
				strcpy(hdd_Path, last_hddpath);

				free(rmsg);
				free(msg);

				break;
			}
		}else if(rmsg[0] == SERV_CMDS_HDD_CD){
			if(smsg == 0)
			{
				free(rmsg);
				free(msg);
			}else{
				hdd_SetPath(msg);
				free(rmsg);
				free(msg);
			}

			putchar('\b');
			
			break;
		}

		free(rmsg);
	}

	free(last_hddpath);

	if(rv < 1)
	{
		net_PrintLastError();
		
		return SERV_CMDS_BREAK;
	}

	hdd_Format();

	return SERV_CMDS_GOOD;
}