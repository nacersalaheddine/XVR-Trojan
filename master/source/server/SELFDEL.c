#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"
#include "input.h"

char* serv_SELFDEL_guard[21] = { "apple", "fog", "testy", "eggs", "mass", "hate", "beg", "whole", "tough", "man", "late", "drag", "plan", "dome", "word", "art", "dark", "worm", "spit", "wire", "peel" };

int serv_SELFDEL(void)
{
	srand(time(NULL));

	int stuk = rand() % 21;

	LOG(LOG_WAR, "Type \"%s\" to continue: ", serv_SELFDEL_guard[stuk]);
	
	char* inp = input_Gets();

	if(strcmp(inp, serv_SELFDEL_guard[stuk]) != 0)
	{
		LOG(LOG_ERR, "No match\n");
		free(inp);

		return SERV_CMDS_DONT_SEEDUP;
	}

	if(net_SendEmptyCmd(SERV_CMDS_SELF_DEL) < 1)
	{
		net_PrintLastError();

		return 0;
	}

	Sleep(1000);

	return SERV_CMDS_GOOD;
}