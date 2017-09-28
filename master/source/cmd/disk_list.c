#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

#define COMMANDS_DISK_LIST_DATA 0xDF
#define COMMANDS_DISK_LIST_END 0xFB

int command_Disk_list(void)
{
	int rv = net_SendCmd((uint8*)" ", 1, COMMANDS_DISK_LIST);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		
		return NET_LOST_CONNECTION;
	}

	uint8 *rmsg;

	LOG(LOG_INFO, "       Type  Letter\n");

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == COMMANDS_DISK_LIST_END)
		{
			break;
		}else if(rmsg[0] == COMMANDS_DISK_LIST_DATA){
			LOG(LOG_INFO, " ");

			if(rmsg[1] == DRIVE_UNKNOWN)
			{
				printf("   Unknown  %c:\\\\\n", rmsg[2]);
			}else if(rmsg[1] == DRIVE_NO_ROOT_DIR){
				printf(" No Volume  %c:\\\\\n", rmsg[2]);
			}else if(rmsg[1] == DRIVE_REMOVABLE){
				if(log_Color)
				{
					LOG_plus_SetColor(log_colorPalette[LOG_COLOR_DISK_LIST_REMOVABLE]);
					printf(" Removable  %c:\\\\\n", rmsg[2]);
					LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
				}else{
					printf(" Removable  %c:\\\\\n", rmsg[2]);
				}
			}else if(rmsg[1] == DRIVE_FIXED){
				if(log_Color)
				{
					LOG_plus_SetColor(log_colorPalette[LOG_COLOR_DISK_LIST_FIXED]);
					printf("     Fixed  %c:\\\\\n", rmsg[2]);
					LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
				}else{
					printf("     Fixed  %c:\\\\\n", rmsg[2]);
				}
			}else if(rmsg[1] == DRIVE_REMOTE){
				printf("   Network  %c:\\\\\n", rmsg[2]);
			}else if(rmsg[1] == DRIVE_CDROM){
				printf("    CD-Rom  %c:\\\\\n", rmsg[2]);
			}else if(rmsg[1] == DRIVE_RAMDISK){
				printf("       RAM  %c:\\\\\n", rmsg[2]);
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
		LOG(LOG_ERR, "Time out!\n");

		return NET_LOST_CONNECTION;
	}

	return COMMANDS_SUCC;
}