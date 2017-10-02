#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "version.h"
#include "logger.h"
#include "whitelist.h"
#include "blocklist.h"
#include "server.h"
#include "input.h"
#include "SCL.h"
#include "sc/sc.h"
#include "geoIP.h"

#define MAIN_CMD_BREAK -1
#define MAIN_CMD_UNKNOWN 0
#define MAIN_CMD_GOOD 1

int main_cmd(char* msg, int msgLen)
{
	msgLen--; 
	msg[msgLen] = '\0'; //removes \n from the input
	
	int rv;

	if(strcmp(msg, "help") == 0)
	{
		LOG(LOG_INFO, "Commands:\n");
		LOG(LOG_INFO, "  help                  #this\n");
		LOG(LOG_INFO, "  stop                  #stops the program\n");
		LOG(LOG_INFO, "  port set {PORT}       #sets the server port\n");
		LOG(LOG_INFO, "  port get              #retrieves server port\n");
		LOG(LOG_INFO, "  blocklist show {P}    #display all IPs in blocklist\n");		
		LOG(LOG_INFO, "  blocklist add {IP}    #block this IP from connecting\n");		
		LOG(LOG_INFO, "  blocklist remove {I}  #removes IP\n");
		LOG(LOG_INFO, "  blocklist clear       #clears the blocklist data\n");
		LOG(LOG_INFO, "  blocklist count       #displays how many IPs are loaded\n");
		LOG(LOG_INFO, "  blocklist turn on/off #turn blocklist on of off\n");
		LOG(LOG_INFO, "  whitelist show {P}    #display all IPs in whitelist\n");
		LOG(LOG_INFO, "  whitelist add {IP}    #allow this IP to connect\n");
		LOG(LOG_INFO, "  whitelist remove {I}  #removes IP\n");
		LOG(LOG_INFO, "  whitelist clear       #clears the whitelist data\n");
		LOG(LOG_INFO, "  whitelist count       #displays how many IPs are loaded\n");
		LOG(LOG_INFO, "  whitelist turn on/off #turn whitelist on of off\n");
		LOG(LOG_INFO, "  geoip turn on/off     #turn geoip on of off\n");
		LOG(LOG_INFO, "  reload all            #tries to reload the program\n");
		LOG(LOG_INFO, "  reload sc             #tries to reload SC\n");
		LOG(LOG_INFO, "  reload whitelist      #tries to reload whitelist data\n");
		LOG(LOG_INFO, "  reload blocklist      #tries to reload blocklist data\n");
		LOG(LOG_INFO, "  start                 #starts the master\n");

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "stop") == 0){
		return MAIN_CMD_BREAK;
	}else if(strncmp(msg, "port set ", 9) == 0){
			if(msgLen - 9 < 1)
			{
				return MAIN_CMD_UNKNOWN;
			}
	
			server_UsingPort = atoi(msg + 9);
	
			LOG(LOG_SUCC, "Port set to %d\n", server_UsingPort);
	
			return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "port get") == 0){
		LOG(LOG_INFO, "Port: %d\n", server_UsingPort);

		return MAIN_CMD_GOOD;
	}else if(strncmp(msg, "blocklist show ", 15) == 0){
		if(msgLen - 15 < 1)
		{
			return MAIN_CMD_UNKNOWN;
		}

		int page = atoi(msg + 15);

		blocklist_PrintList(page);

		return MAIN_CMD_GOOD;
	}else if(strncmp(msg, "blocklist add ", 14) == 0){
		if(msgLen - 14 < 1)
		{
			return MAIN_CMD_UNKNOWN;
		}

		if(blocklist_Add(msg + 14) != WHITELIST_NO_ERROR)
		{
			LOG(LOG_ERR, "Invalid IP!\n");
		}else{
			LOG(LOG_SUCC, "IP blocked!\n");
		}

		return MAIN_CMD_GOOD;
	}else if(strncmp(msg, "blocklist remove ", 17) == 0){
		if(msgLen - 17 < 1)
		{
			return MAIN_CMD_UNKNOWN;
		}

		int ipId = atoi(msg + 17);

		if(blocklist_RemoveAt(ipId) != WHITELIST_NO_ERROR)
		{
			LOG(LOG_ERR, "Invalid Index...");
			LOG(LOG_INFO, "Use \"blocklist show {P}\" to get all IPs indexes!\n");
		}else{
			LOG(LOG_SUCC, "IP removed from list\n");
		}

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "blocklist clear") == 0){
		blocklist_Clear();

		LOG(LOG_INFO, "Blocklist cleared!\n");

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "blocklist count") == 0){

		int total = blocklist_Count();

		LOG(LOG_INFO, "Total IPs: %d\n", total);

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "blocklist turn on") == 0){
		blocklist_IsInUse = 1;

		LOG(LOG_INFO, "blocklist is turned on!\n");

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "blocklist turn off") == 0){
		blocklist_IsInUse = 0;

		LOG(LOG_INFO, "Blocklist is turned off!\n");

		return MAIN_CMD_GOOD;
	}else if(strncmp(msg, "whitelist show ", 15) == 0){
		if(msgLen - 15 < 1)
		{
			return MAIN_CMD_UNKNOWN;
		}

		int page = atoi(msg + 15);

		whitelist_PrintList(page);

		return MAIN_CMD_GOOD;
	}else if(strncmp(msg, "whitelist add ", 14) == 0){
		if(msgLen - 14 < 1)
		{
			return MAIN_CMD_UNKNOWN;
		}

		if(whitelist_Add(msg + 14) != WHITELIST_NO_ERROR)
		{
			LOG(LOG_ERR, "Invalid IP!\n");
		}else{
			LOG(LOG_SUCC, "IP added!\n");
		}

		return MAIN_CMD_GOOD;
	}else if(strncmp(msg, "whitelist remove ", 17) == 0){
		if(msgLen - 17 < 1)
		{
			return MAIN_CMD_UNKNOWN;
		}

		int ipId = atoi(msg + 17);

		if(whitelist_RemoveAt(ipId) != WHITELIST_NO_ERROR)
		{
			LOG(LOG_ERR, "Invalid Index...");
			LOG(LOG_INFO, "Use \"whitelist show {P}\" to get all IPs indexes!\n");
		}else{
			LOG(LOG_SUCC, "IP removed from list\n");
		}

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "whitelist clear") == 0){
		whitelist_Clear();

		LOG(LOG_INFO, "Whitelist cleared!\n");

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "whitelist count") == 0){

		int total = whitelist_Count();

		LOG(LOG_INFO, "Total IPs: %d\n", total);

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "whitelist turn on") == 0){
		whitelist_IsInUse = 1;

		LOG(LOG_INFO, "Whitelist is turned on!\n");

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "whitelist turn off") == 0){
		whitelist_IsInUse = 0;

		LOG(LOG_INFO, "Whitelist is turned off!\n");

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "geoip turn on") == 0){
		
		if(geoIP_Init() == GEOIP_NO_ERROR)
		{
			LOG(LOG_INFO, "Geoip is turned on!\n");
		}else{
			LOG(LOG_ERR, "Failed to turn on GeoIP!\n");
		}

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "geoip turn off") == 0){
		geoIP_IsInUse = 0;

		LOG(LOG_INFO, "GeoIP is turned off!\n");

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "scl info") == 0){
		SCL_PrintInfo();

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "reload all") == 0){
		SC_LoadLibrary();
		whitelist_Init();
		blocklist_Init();

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "reload sc") == 0){
		SC_LoadLibrary();

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "reload whitelist") == 0){
		whitelist_Init();

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "reload blocklist") == 0){
		blocklist_Init();

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "start") == 0){
		LOG(LOG_INFO, "Creating the server...\n");

		rv = server_Create();

		if(rv != SERVER_NO_ERROR)
		{
			LOG(LOG_ERR, "Error when creating the server: %d\n", rv);
			LOG(LOG_ERR, "WSA Error: %d\n", WSAGetLastError());
			LOG(LOG_ERR, "Error: %d\n", GetLastError());
	
			return MAIN_CMD_BREAK;
		}

		LOG(LOG_SUCC, "Server created!\n");

		rv = server_WaitForSlavesAndChoise();

		if(rv != SERVER_NO_ERROR && rv != SERVER_ERROR_NO_SLAVE)
		{
			LOG(LOG_ERR, "Error when searching for slaves: %d\n", rv);
			LOG(LOG_ERR, "WSA Error: %d\n", WSAGetLastError());
			LOG(LOG_ERR, "Error: %d\n", GetLastError());
	
			return MAIN_CMD_BREAK;
		}

		if(rv == SERVER_NO_ERROR)
		{
			server_ConnectionHandle();
		}

		LOG(LOG_INFO, "Server stoped!\n");

		return MAIN_CMD_GOOD;
	}

	return MAIN_CMD_UNKNOWN;
}

void main_printStuff(void)
{
	LOG_plus_SetColor(0x07);

	printf("\n                      /#\\\n                     /##=\\\n                    /##===\\\n                  /###==#===\\\n                /####===##====\\\n              /#####====###=====\\\n             |#####=====####=====|\n             |#####=====####=====|\n              \\\\####=   ###=====//\n                     |##=|\n                   |####===|\n\n");
	
	LOG_plus_SetColor(0x0F);

	printf("  The quieter you become the more you are able to hear.\n");
	printf("                                       -Ram Dass\n\n");
}

int main(int argc, char* args[])
{
	if(argc > 1)
	{
		int i;

		for(i = 0; i != argc; i++)
		{
			if(strcmp(args[i], "-Color") == 0)
			{
				log_Color = 1;
			}else if(strcmp(args[i], "-Time") == 0){
				log_Time = 1;
			}else if(strcmp(args[i], "-GeoIP") == 0){
				geoIP_IsInUse = 1;
			}
		}
	}

	LOG_Init();
	main_printStuff();

	LOG_LoadConfing();
	LOG(LOG_INFO, "Version: %s\n", VERSION);

	SCL_PrintInfo();
	SC_LoadLibrary();
	whitelist_Init();
	blocklist_Init();

	LOG(LOG_INFO, "Server port: %d\n", server_UsingPort);
	LOG(LOG_INFO, "Starting WSA...\n");

	int rv = server_WsaInit();

	if(rv != SERVER_NO_ERROR)
	{
		LOG(LOG_ERR, "Error when starting WSA: %d\n", rv);
		LOG(LOG_ERR, "WSA Error: %d\n", WSAGetLastError());
		LOG(LOG_ERR, "Error: %d\n", GetLastError());

		return 0;
	}

	LOG(LOG_SUCC, "WSA Started!\n");

	if(geoIP_IsInUse)
	{
		LOG(LOG_INFO, "Preparing GeoIP\n");

		rv = geoIP_Init();

		if(rv != GEOIP_NO_ERROR)
		{
			LOG(LOG_ERR, "Failed to prepare GeoIP!\n");
		}else{
			LOG(LOG_SUCC, "Prepared!\n");
		}
	}

	rv = MAIN_CMD_BREAK;
	char* m_in;

	while(1)
	{
		m_in = input_Get(INPUT_MAIN);

		putchar('\n');

		rv = main_cmd(m_in, strlen(m_in));

		free(m_in);

		if(rv == MAIN_CMD_BREAK)
		{
			break;
		}else if(rv != MAIN_CMD_GOOD){
			LOG(LOG_ERR, "Unknow command!\n");
			LOG(LOG_INFO, "Type \"help\" for help!\n");
		}
	}

	LOG(LOG_INFO, "Freeing libraries...\n");
	SC_FreeLibrary();

	LOG(LOG_INFO, "Freeing server...\n");
	server_Cleanup();

	LOG(LOG_SUCC, "Stoped!");
	exit(0);

	return 0;
}