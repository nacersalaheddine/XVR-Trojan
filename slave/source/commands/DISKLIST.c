#include <stdlib.h>
#include <winioctl.h>
#include <windows.h>
#include "net/interface.h"
#include "net/error.h"
#include "types.h"

#define SERV_DISKLIST_TYPE_GB 1
#define SERV_DISKLIST_TYPE_MB 2
#define SERV_DISKLIST_TYPE_KB 3
#define SERV_DISKLIST_TYPE_B 0
#define SERV_DISKLIST_DATA 0x1
#define SERV_DISKLIST_END 0x2

typedef struct __serv_disklist_info
{
	char disk;
	char info;
	char hasSize;
	char totalType;
	char leftType;
	char usedType;
	uint32 total;
	uint32 left;
	uint32 used;
}SERV_DISKLIST_INFO;

int cmds_DISKLIST(void)
{
	int i;
	char disk[3] = { 0, ':', 0 };
	char disk2[4] = { 0, ':', '\\', 0 };
	ULARGE_INTEGER freeB;
	ULARGE_INTEGER totalB;
	ULARGE_INTEGER totalFreeB;
	uint8* rmsg;
	uint8 buff[14];
	uint32 drives = GetLogicalDrives();

	SERV_DISKLIST_INFO sdInfo;

	for(i = 0; i != 'Z' - 'A'; i++)
	{
		if(drives & ( 1 << i ))
		{
			memset(&sdInfo, 0, sizeof(SERV_DISKLIST_INFO));
			memset(buff, 0, 30);
			disk[0] = 'A' + i;
			disk2[0] = 'A' + i;
			sdInfo.disk = 'A' + i;

			sdInfo.info = GetDriveType(disk2);

			if(GetDiskFreeSpaceEx(disk, &freeB, &totalB, &totalFreeB))
			{
				sdInfo.hasSize = 1;
				
				if(totalB.QuadPart / (1024 * 1024 * 1024))
				{
					sdInfo.total = totalB.QuadPart / (1024 * 1024 * 1024);
					sdInfo.totalType = SERV_DISKLIST_TYPE_GB;
				}else if(totalB.QuadPart / (1024 * 1024)){
					sdInfo.total = totalB.QuadPart / (1024 * 1024);
					sdInfo.totalType = SERV_DISKLIST_TYPE_MB;
				}else if(totalB.QuadPart / 1024){
					sdInfo.total = totalB.QuadPart / 1024;
					sdInfo.totalType = SERV_DISKLIST_TYPE_KB;
				}else{
					sdInfo.total = totalB.QuadPart;
					sdInfo.totalType = SERV_DISKLIST_TYPE_B;
				}

				if(freeB.QuadPart / (1024 * 1024 * 1024))
				{
					sdInfo.left = freeB.QuadPart / (1024 * 1024 * 1024);
					sdInfo.leftType = SERV_DISKLIST_TYPE_GB;
				}else if(freeB.QuadPart / (1024 * 1024)){
					sdInfo.left = freeB.QuadPart / (1024 * 1024);
					sdInfo.leftType = SERV_DISKLIST_TYPE_MB;
				}else if(freeB.QuadPart / 1024){
					sdInfo.left = freeB.QuadPart / 1024;
					sdInfo.leftType = SERV_DISKLIST_TYPE_KB;
				}else{
					sdInfo.left = freeB.QuadPart;
					sdInfo.leftType = SERV_DISKLIST_TYPE_B;
				}

				uint64 used = totalB.QuadPart - freeB.QuadPart;

				if(used / (1024 * 1024 * 1024))
				{
					sdInfo.used = used / (1024 * 1024 * 1024);
					sdInfo.usedType = SERV_DISKLIST_TYPE_GB;
				}else if(used / (1024 * 1024)){
					sdInfo.used = used / (1024 * 1024);
					sdInfo.usedType = SERV_DISKLIST_TYPE_MB;
				}else if(used / 1024){
					sdInfo.used = used / 1024;
					sdInfo.usedType = SERV_DISKLIST_TYPE_KB;
				}else{
					sdInfo.used = used;
					sdInfo.usedType = SERV_DISKLIST_TYPE_B;
				}
			}

			memcpy(buff, &sdInfo, sizeof(SERV_DISKLIST_INFO));

			if(net_SendCmd(buff, 30, SERV_DISKLIST_DATA) < 1)
			{
				return 0;
			}

			if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
			{
				return 0;
			}
		
			free(rmsg);
		}
	}

	if(net_SendEmptyCmd(SERV_DISKLIST_END) < 1)
	{
		return 0;
	}

	return 1;
}

/*
ULARGE_INTEGER freeB;
	ULARGE_INTEGER totalB;
	ULARGE_INTEGER totalFreeB;

	if(!GetDiskFreeSpaceEx("C:", &freeB, &totalB, &totalFreeB))
	{
		return 100;
	}

	int size = totalB.QuadPart / (1024 * 1024 * 1024);
	int freeSize = freeB.QuadPart / (1024 * 1024 * 1024);
	int used = size - freeSize;

	printf("Total: %d/%d Used: %d\n", size, freeSize, used);

	return 0;
*/