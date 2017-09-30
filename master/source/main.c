#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "version.h"
#include "logger.h"
#include "server.h"
#include "input.h"
#include "SCL.h"
#include "sc/sc.h"

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
		LOG(LOG_INFO, "  help               #this\n");
		LOG(LOG_INFO, "  stop               #stops the program\n");
		LOG(LOG_INFO, "  port set {PORT}    #sets the server port\n");
		LOG(LOG_INFO, "  port get           #retrieves server port\n");
		LOG(LOG_INFO, "  whitelist set {IP} #allow only this ip to connect\n");
		LOG(LOG_INFO, "  whitelist get      #retrieves ip form the whitelist\n");
		LOG(LOG_INFO, "  whitelist remove   #removes the whitelist\n");
		LOG(LOG_INFO, "  start              #starts the master\n");

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
	}else if(strncmp(msg, "whitelist set ", 14) == 0){
		if(msgLen - 14 < 1)
		{
			return MAIN_CMD_UNKNOWN;
		}

		if(!server_WhitelistIp)
		{
			server_WhitelistIp = malloc((msgLen - 14) + sizeof(char));
		}else{
			server_WhitelistIp = realloc(server_WhitelistIp, msgLen - 14);
		}

		strcpy(server_WhitelistIp, msg + 14);
		LOG(LOG_SUCC, "Whitelist ip is set to %s\n", server_WhitelistIp);
		server_HasWhitelist = 1;

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "whitelist get") == 0){

		if(!server_WhitelistIp || !server_HasWhitelist)
		{
			LOG(LOG_ERR, "Whitelist is empy!\n");

			return MAIN_CMD_GOOD;
		}

		LOG(LOG_INFO, "Whitelist ip: %s\n", server_WhitelistIp);

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "whitelist remove") == 0){
		server_HasWhitelist = 0;

		LOG(LOG_SUCC, "Whitelist removed!\n");

		return MAIN_CMD_GOOD;
	}else if(strcmp(msg, "scl info") == 0){
		SCL_PrintInfo();

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

		rv = server_WaitForSlave();
		
		if(rv != SERVER_NO_ERROR)
		{
			LOG(LOG_ERR, "Error when creating the server: %d\n", rv);
			LOG(LOG_ERR, "WSA Error: %d\n", WSAGetLastError());
			LOG(LOG_ERR, "Error: %d\n", GetLastError());
	
			return MAIN_CMD_BREAK;
		}

		server_ConnectionHandle();

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
			}
		}
	}

	LOG_Init();
	main_printStuff();

	LOG_LoadConfing();
	LOG(LOG_INFO, "Version: %s\n", VERSION);

	SCL_PrintInfo();

	SC_LoadLibrary();

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