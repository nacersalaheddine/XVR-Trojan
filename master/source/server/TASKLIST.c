#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"

#define SERV_TASKLIST_ESCAPE_KEY 0x1B
#define SERV_TASKLIST_TEMP_SP 0x1
#define SERV_TASKLIST_TEMP_BUFFSIZE 256
#define SERV_TASKLIST_TEMP_FILE "tasklist.temp"
#define SERV_TASKLIST_DATA 0x1
#define SERV_TASKLIST_END 0x2

#define SERV_TASKLIST_FAILED 0x0F

int serv_TASKLIST(char* str)
{
	int pageMax = atoi(str);

	free(str);

	if(pageMax < 1)
	{
		pageMax = 1024;
	}

	FILE *f = fopen(SERV_TASKLIST_TEMP_FILE, "w");

	if(!f)
	{
		LOG(LOG_ERR, "Failed to create \"%s\"\n", SERV_TASKLIST_TEMP_FILE);

		return SERV_CMDS_DONT_SEEDUP;
	}

	int rv = net_SendEmptyCmd(SERV_CMDS_TASK_LIST);

	if(rv < 1)
	{
		net_PrintLastError();
		fclose(f);
		remove(SERV_TASKLIST_TEMP_FILE);
		
		return SERV_CMDS_BREAK;
	}

	uint8 *rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv < 1)
	{
		fclose(f);
		remove(SERV_TASKLIST_TEMP_FILE);

		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	if(rmsg[0] == SERV_TASKLIST_FAILED)
	{
		LOG(LOG_ERR, "Failed to get all processors!\n");
		free(rmsg);
		fclose(f);
		remove(SERV_TASKLIST_TEMP_FILE);

		return SERV_CMDS_GOOD;
	}

	free(rmsg);

	LOG(LOG_WAR, "Receiving data...\n");

	while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
	{
		if(rmsg[0] == SERV_TASKLIST_END)
		{
			free(rmsg);

			break;
		}else if(rmsg[0] == SERV_TASKLIST_DATA){
			fprintf(f, "%d%c%s\n", ((rmsg[1] & 0xFF) << 24) | ((rmsg[2] & 0xFF) << 16) | ((rmsg[3] & 0xFF) << 8) | (rmsg[4] & 0xFF), SERV_TASKLIST_TEMP_SP, rmsg + 5);
		}

		free(rmsg);

		if(net_SendEmptyCmd(SERV_CMDS_SYSTEM) < 1)
		{
			net_PrintLastError();
			fclose(f);
			remove(SERV_TASKLIST_TEMP_FILE);
			
			return SERV_CMDS_BREAK;
		}
	}

	if(rv < 1)
	{
		fclose(f);
		remove(SERV_TASKLIST_TEMP_FILE);

		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	fclose(f);
	f = fopen(SERV_TASKLIST_TEMP_FILE, "r");

	if(!f)
	{
		LOG(LOG_ERR, "Couldn't find \"%s\" did you deleted it ?\n", SERV_TASKLIST_TEMP_FILE);

		return SERV_CMDS_GOOD;
	}

	LOG(LOG_SUCC, "Done!\n");
	LOG(LOG_NONE, "  Pid    Name\n");

	int i;
	int curPage = 0;
	int wh = 0;
	int len = 0;
	char line[SERV_TASKLIST_TEMP_BUFFSIZE];

	while(fgets(line, SERV_TASKLIST_TEMP_BUFFSIZE - 1, f) != NULL)
	{
		wh = 0;
		len = strlen(line);
		
		if(len < 2)
		{
			continue;
		}

		for(i = 0; i != len; i++)
		{
			if(line[i] == SERV_TASKLIST_TEMP_SP)
			{
				line[i] = 0;
				wh = i + 1;
			}else if(line[i] == '\n'){
				line[i] = 0;
			}
		}

		if(wh < 1)
		{
			continue;
		}

		if(curPage >= pageMax)
		{
			curPage = 0;

			LOG(LOG_INFO, "Press any key to continue or \"ESC\" to stop... ");
			
			int rkey = getch();

			LOG_NEWLINE();

			if(rkey == SERV_TASKLIST_ESCAPE_KEY)
			{
				break;
			}
		}

		curPage++;
		LOG(LOG_NONE, "  ");

		LOG_TablePrint(6, "%d", atoi(line));
		printf("%s\n", line + wh);
	}

	fclose(f);
	remove(SERV_TASKLIST_TEMP_FILE);

	return SERV_CMDS_GOOD;
}
/*

*/