#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "input.h"
#include "blocklist.h"
#include "whitelist.h"
#include "SCL2.h"
#include "sc/sc.h"
#include "server.h"
#include "geoIP.h"

#define CMDS_RV_BREAK 0
#define CMDS_RV_GOOD 1
#define CMDS_RV_ERR_UNK 2
#define CMDS_RV_ERR_ARG 3

int cmds_translate(char* str, int len)
{
	LOG_NEWLINE();

	if(strcmp(str, "help") == 0)
	{
		LOG(LOG_TABLE, "help                     This\n");
		LOG(LOG_TABLE, "stop                     Exit\n");
		LOG(LOG_TABLE, "clear                    Clears console\n");
		LOG(LOG_TABLE, "blocklist add {IP}       Add ip to block list\n");
		LOG(LOG_TABLE, "blocklist show {PAGE}    Display block list\n");
		LOG(LOG_TABLE, "blocklist remove {ID}    Remove IP by id from list\n");
		LOG(LOG_TABLE, "blocklist clear          Clears block list\n");
		LOG(LOG_TABLE, "whitelist add {IP}       Add ip to white list\n");
		LOG(LOG_TABLE, "whitelist show {PAGE}    Display white list\n");
		LOG(LOG_TABLE, "whitelist remove {ID}    Remove IP by id from list\n");
		LOG(LOG_TABLE, "whitelist clear          Clears white list\n");
		LOG(LOG_TABLE, "scl info                 Show SCL info\n");
		LOG(LOG_TABLE, "reload sc                Reload SC\n");
		LOG(LOG_TABLE, "geoip turn on/off        Turn on and of geoIP\n");
		LOG(LOG_TABLE, "port get                 Show server port\n");
		LOG(LOG_TABLE, "port set {PORT}          Set server port\n");
		LOG(LOG_TABLE, "start                    Start the server\n");

		return CMDS_RV_GOOD;
	}else if(strcmp(str, "stop") == 0){
		return CMDS_RV_BREAK;
	}else if(strcmp(str, "clear") == 0){
		system("CLS");
		return CMDS_RV_GOOD;
	}else if(strncmp(str, "blocklist add ", 14) == 0){
		if(!(len - 14))
		{
			return CMDS_RV_ERR_ARG;
		}

		if(blocklist_Add(str + 14) != BLOCKLIST_NO_ERROR)
		{
			LOG(LOG_WAR, "Invalid IP!\n");
		}else{
			blocklist_StoreAll();
			LOG(LOG_SUCC, "IP Blocked!\n");
		}

		return CMDS_RV_GOOD;
	}else if(strncmp(str, "blocklist show ", 15) == 0){
		if(!(len - 15))
		{
			return CMDS_RV_ERR_ARG;
		}

		int page = atoi(str + 15);
		blocklist_PrintList(page);

		return CMDS_RV_GOOD;
	}else if(strncmp(str, "blocklist remove ", 17) == 0){
		if(!(len - 17))
		{
			return CMDS_RV_ERR_ARG;
		}

		int ipId = atoi(str + 17);

		if(blocklist_RemoveAt(ipId) != BLOCKLIST_NO_ERROR)
		{
			LOG(LOG_WAR, "Failed to remove the ip!\n");
		}else{
			LOG(LOG_SUCC, "IP removed!\n");
			blocklist_StoreAll();
		}

		return CMDS_RV_GOOD;
	}else if(strcmp(str, "blocklist clear") == 0){
		blocklist_Clear();
		LOG(LOG_SUCC, "Blocklist cleared!\n");

		return CMDS_RV_GOOD;
	}else if(strncmp(str, "whitelist add ", 14) == 0){
		if(!(len - 14))
		{
			return CMDS_RV_ERR_ARG;
		}

		if(whitelist_Add(str + 14) != WHITELIST_NO_ERROR)
		{
			LOG(LOG_WAR, "Invalid IP!\n");
		}else{
			whitelist_StoreAll();
			LOG(LOG_SUCC, "IP is added in friends!\n");
		}

		return CMDS_RV_GOOD;
	}else if(strncmp(str, "whitelist show ", 15) == 0){
		if(!(len - 15))
		{
			return CMDS_RV_ERR_ARG;
		}

		int page = atoi(str + 15);
		whitelist_PrintList(page);

		return CMDS_RV_GOOD;
	}else if(strncmp(str, "whitelist remove ", 17) == 0){
		if(!(len - 17))
		{
			return CMDS_RV_ERR_ARG;
		}

		int ipId = atoi(str + 17);

		if(whitelist_RemoveAt(ipId) != WHITELIST_NO_ERROR)
		{
			LOG(LOG_WAR, "Failed to remove the ip!\n");
		}else{
			LOG(LOG_SUCC, "IP removed!\n");
			whitelist_StoreAll();
		}

		return CMDS_RV_GOOD;
	}else if(strcmp(str, "whitelist clear") == 0){
		whitelist_Clear();
		LOG(LOG_SUCC, "Whitelist cleared!\n");

		return CMDS_RV_GOOD;
	}else if(strcmp(str, "scl info") == 0){
		SCL2_PrintInfo();

		return CMDS_RV_GOOD;
	}else if(strcmp(str, "reload sc") == 0){
		SC_LoadLibrary();

		return CMDS_RV_GOOD;
	}else if(strcmp(str, "geoip turn on") == 0){
		if(geoIP_Init() == GEOIP_NO_ERROR)
		{
			LOG(LOG_SUCC, "Geoip is turned on!\n");
		}else{
			LOG(LOG_ERR, "Failed to turn on GeoIP!\n");
		}

		return CMDS_RV_GOOD;
	}else if(strcmp(str, "geoip turn off") == 0){
		geoIP_IsInUse = 0;

		LOG(LOG_SUCC, "GeoIP is turned off!\n");

		return CMDS_RV_GOOD;
	}else if(strcmp(str, "port get") == 0){
		LOG(LOG_SUCC, "Port: %d\n", server_UsingPort);
		
		return CMDS_RV_GOOD;
	}else if(strncmp(str, "port set ", 9) == 0){
		if(!(len - 9))
		{
			return CMDS_RV_ERR_ARG;
		}

		server_UsingPort = atoi(str + 9);

		LOG(LOG_SUCC, "Port set to %d\n", server_UsingPort);

		return CMDS_RV_GOOD;
	}else if(strcmp(str, "start") == 0){
		LOG(LOG_INFO, "Creating the server...\n");

		int rv = server_Create();

		if(rv != SERVER_NO_ERROR)
		{
			LOG(LOG_CRITICAL, "Error when creating the server: 0x%02X/0x%X (WSA)\n", rv, WSAGetLastError());

			return CMDS_RV_BREAK;
		}

		rv = server_WaitForSlaves();

		if(rv != SERVER_NO_ERROR && rv != SERVER_ERROR_NO_SLAVE)
		{
			LOG(LOG_CRITICAL, "Error when searching for slaves: 0x%02X/0x%X (WSA)\n", rv, WSAGetLastError());
			
			return CMDS_RV_BREAK;
		}

		if(rv == SERVER_NO_ERROR)
		{
			server_ConnectionHandle();
		}

		LOG_TitleDefault();
		LOG(LOG_WAR, "Server stopped!\n");

		return CMDS_RV_GOOD;
	}

	return CMDS_RV_ERR_UNK;
}

void cmds_Loop(void)
{
	int rv = 0;
	char* cmd;
	
	while(1)
	{
		LOG_NEWLINE();

		if(!input_Get(INPUT_TYPE_USER, &cmd))
		{
			continue;
		}

		rv = cmds_translate(cmd, strlen(cmd));

		free(cmd);

		if(rv == CMDS_RV_ERR_UNK)
		{
			LOG(LOG_WAR, "Unknow command! Type \"help\" for help.\n");
		}else if(rv == CMDS_RV_ERR_ARG){
			LOG(LOG_WAR, "Invalid arguments!\n");
		}else if(rv == CMDS_RV_BREAK){
			putchar('\b');
			fflush(stdout);
			
			return;
		}
	}
}