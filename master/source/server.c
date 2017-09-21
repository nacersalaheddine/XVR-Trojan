#include <stdio.h>
#include <windows.h>
#include "logger.h"
#include "server.h"
#include "SCL.h"
#include "input.h"
#include "types.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"

int server_isSendingCmd = 0;
int server_keepAliveRunning = 0;
int server_isSendingKeepAlive = 0;
int server_timeout = 1000;
int server_Running = 0;
int server_UsingPort = SERVER_PORT;
int server_HasWhitelist = 0;
char* server_WhitelistIp;
SOCKET server_socket = INVALID_SOCKET;
SOCKADDR_IN server_socketAddr;

SOCKET server_Client = INVALID_SOCKET;
SOCKADDR_IN server_ClientAddr;

void server_CloseConnection(void)
{
	if(server_Running != 0 && server_Client != INVALID_SOCKET)
	{
		shutdown(server_Client, SD_BOTH);
		closesocket(server_Client);
		server_Client = INVALID_SOCKET;
	}

	shutdown(server_socket, SD_BOTH);
	closesocket(server_socket);
	server_socket = INVALID_SOCKET;
	server_Running = 0;
}

int server_WsaInit(void)
{
	WSADATA wsaData;
	
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return SERVER_ERROR_WSASTARTUP;
	}

	return SERVER_NO_ERROR;
}

void server_Cleanup(void)
{
	server_CloseConnection();

	WSACleanup();
}

int server_Create(void)
{
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	char _val1[1] = { 1 };

	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, _val1, sizeof(int));
	setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&server_timeout, sizeof(server_timeout));
	setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, _val1, sizeof(int));

	server_socketAddr.sin_addr.s_addr = INADDR_ANY;
	server_socketAddr.sin_family = AF_INET;
	server_socketAddr.sin_port = htons(server_UsingPort);

	if(bind(server_socket, (SOCKADDR*)&server_socketAddr, sizeof(server_socketAddr)) != 0)
	{
		WSACleanup();

		return SERVER_ERROR_BIND;
	}

	server_Client = INVALID_SOCKET;

	return SERVER_NO_ERROR;
}

int server_WaitForSlave(void)
{
	if(listen(server_socket, SOMAXCONN) != 0)
	{
		WSACleanup();

		return SERVER_ERROR_LISTEN;
	}

	LOG(LOG_INFO, "Waiting for connection...\n");

	while(1)
	{
		int _addrsize = sizeof(server_ClientAddr);
		server_Client = accept(server_socket, (SOCKADDR*)&server_ClientAddr, &_addrsize);

		if(server_Client == INVALID_SOCKET)
		{
			LOG(LOG_ERR, "Couldn't accept connection!\n");
			Sleep(100);

			continue;
		}

		if(server_HasWhitelist)
		{
			if(strcmp(inet_ntoa(server_ClientAddr.sin_addr), server_WhitelistIp) != 0)
			{
				shutdown(server_Client, SD_BOTH);
				closesocket(server_Client);
				server_Client = INVALID_SOCKET;
				
				continue;
			}
		}

		char _val1[1] = { 1 };

		setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, _val1, sizeof(int));		
		setsockopt(server_Client, SOL_SOCKET, SO_RCVTIMEO, (char*)&server_timeout, sizeof(server_timeout));

		LOG(LOG_SUCC, "%s has connected!\n", inet_ntoa(server_ClientAddr.sin_addr));
		SCL_ResetSeed();

		return SERVER_NO_ERROR;
	}

	return -1;
}

//keep-alive option is just 2 hours... not enough! i think this will keep alive the connection forever. i don't wanna test if it will work
uint64 __stdcall server_thread_KeepAlive(void* args)
{
	int i;
	server_keepAliveRunning = 1;
	server_isSendingKeepAlive = 0;

	Sleep(SERVER_KEEPALIVE_INTERVAL);

	while(server_Running)
	{
		server_isSendingKeepAlive = 1;

		while(server_isSendingCmd){};
		
		if(net_CheckConnection() == NET_LOST_CONNECTION)
		{
			server_keepAliveRunning = 0;
			server_isSendingKeepAlive = 0;
			server_CloseConnection();

			return 0;
		}

		server_isSendingKeepAlive = 0;

		//check every 500 ms to se if it the server is stopped; and after 1 minute check if the connection closed
		for(i = 0; i != SERVER_KEEPALIVE_MAX; i++)
		{
			if(!server_Running || !server_keepAliveRunning)
			{
				server_keepAliveRunning = 0;

				return 0;
			}

			Sleep(SERVER_KEEPALIVE_INTERVAL);
		}
	}

	server_keepAliveRunning = 0;
	server_isSendingKeepAlive = 0;

	return 0;
}

void server_ConnectionHandle(void)
{	
	while(server_keepAliveRunning){}; //wait for keep alive

	int rv = 0;
	char* in_msg;
	server_Running = 1;
	server_isSendingCmd = 0;

	if(!CreateThread(NULL, 0, server_thread_KeepAlive, NULL, 0, NULL))
	{
		LOG(LOG_ERR, "Failed to create reviver!\n");
		LOG(LOG_INFO, "The connection my close if you don't send data\n");
	}

	while(server_Running)
	{
		in_msg = input_Get(INPUT_MASTER);

		server_isSendingCmd = 1;
		while(server_isSendingKeepAlive){}; //wait is keepalive is sending
		
		if(net_CheckConnection() == NET_LOST_CONNECTION)
		{
			putchar('\n');
			LOG(LOG_ERR, "%s lost connection!\n", inet_ntoa(server_ClientAddr.sin_addr));
			server_CloseConnection();
			free(in_msg);

			break;
		}

		rv = commands_Translate(in_msg);

		free(in_msg);

		if(rv == NET_LOST_CONNECTION)
		{
			server_CloseConnection();

			break;
		}else if(rv == COMMANDS_SUCC){ //ako e izpulnena komandata
			SCL_SeedUp();
		}

		server_isSendingCmd = 0;
	}
}