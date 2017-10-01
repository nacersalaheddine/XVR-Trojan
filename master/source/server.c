#include <stdio.h>
#include <windows.h>
#include "logger.h"
#include "server.h"
#include "SCL.h"
#include "input.h"
#include "types.h"
#include "cmd/commands.h"
#include "cmd/screen.h"
#include "net/interface.h"
#include "net/error.h"
#include "whitelist.h"
#include "blocklist.h"

int server_ClientListSize = 0;
int server_isSendingCmd = 0;
int server_keepAliveRunning = 0;
int server_isSendingKeepAlive = 0;
int server_timeout = 1000;
int server_Running = 0;
int server_UsingPort = SERVER_PORT;
uint64 server_io_block = 0;
uint64 server_io_nonBlock = 1;
SOCKET server_socket = INVALID_SOCKET;
SOCKADDR_IN server_socketAddr;

SOCKET server_Client = INVALID_SOCKET;
SOCKET server_ClientList[SERVER_MAX_CLIENTS];
SOCKADDR_IN server_ClientAddr;
SOCKADDR_IN server_ClientAddrList[SERVER_MAX_CLIENTS];

void server_closeAllUnused(void)
{
	int id;

	for(id = 0; id != SERVER_MAX_CLIENTS; id++)
	{
		if(server_Client == server_ClientList[id])
		{
			continue;
		}
		
		if(server_ClientList[id] != INVALID_SOCKET)
		{
			shutdown(server_ClientList[id], SD_BOTH);
			closesocket(server_ClientList[id]);
			server_ClientList[id] = INVALID_SOCKET;
		}
	}
	
}

void server_CloseConnection(void)
{
	int id;
	
	for(id = 0; id != SERVER_MAX_CLIENTS; id++)
	{
		if(server_ClientList[id] != INVALID_SOCKET)
		{
			shutdown(server_ClientList[id], SD_BOTH);
			closesocket(server_ClientList[id]);
			server_ClientList[id] = INVALID_SOCKET;
		}
	}

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

	int i;
	
	for(i = 0; i != SERVER_MAX_CLIENTS; i++)
	{
		server_ClientList[i] = INVALID_SOCKET;
	}

	server_Client = INVALID_SOCKET;
	server_ClientListSize = 0;

	return SERVER_NO_ERROR;
}

int server_WaitForSlavesAndChoise(void)
{
	if(listen(server_socket, SOMAXCONN) != 0)
	{
		WSACleanup();

		return SERVER_ERROR_LISTEN;
	}

	ioctlsocket(server_socket, FIONBIO, &server_io_nonBlock);

	if(whitelist_IsInUse)
	{
		int _count = whitelist_Count();

		LOG(LOG_INFO, "IPs to allow: %d\n", _count);
	}

	if(blocklist_IsInUse)
	{
		int _count = blocklist_Count();
		
		LOG(LOG_INFO, "IPs to block: %d\n", _count);
	}

	LOG(LOG_INFO, "Waiting for connection on port %d\n", server_UsingPort);
	LOG(LOG_INFO, "Hold \"ESC\" to stop listening!\n");	
	LOG(LOG_INFO, " Num  IP\n");

	int useSlaveID = -1;

	while(1)
	{
		SOCKET server_TempClient;
		SOCKADDR_IN server_TempClientAddr;

		int _addrsize = sizeof(server_TempClientAddr);

		while((server_TempClient = accept(server_socket, (SOCKADDR*)&server_TempClientAddr, &_addrsize)) == INVALID_SOCKET)
		{
			if((GetKeyState(VK_ESCAPE) < 0))
			{
				while(1)
				{
					if(server_ClientListSize < 1)
					{
						ioctlsocket(server_socket, FIONBIO, &server_io_block);
						server_CloseConnection();

						return SERVER_ERROR_NO_SLAVE;
					}

					LOG(LOG_INFO, "Chose slave ID from 1 to %d (0 to stop):", server_ClientListSize);

					char* cmdTodo = input_Get(INPUT_NONE);

					useSlaveID = atoi(cmdTodo);
					
					free(cmdTodo);

					if(useSlaveID - 1 < 0)
					{
						ioctlsocket(server_socket, FIONBIO, &server_io_block);
						server_CloseConnection();

						return SERVER_ERROR_NO_SLAVE;
					}

					useSlaveID--;

					if(useSlaveID >= 0 && useSlaveID <= server_ClientListSize)
					{
						if(server_ClientList[useSlaveID] == INVALID_SOCKET)
						{
							LOG(LOG_ERR, "There is empty connection...\n");
						}else{
							char _val1[1] = { 1 };
							server_Client = server_ClientList[useSlaveID];
							server_ClientAddr = server_ClientAddrList[useSlaveID];
							
							ioctlsocket(server_socket, FIONBIO, &server_io_block);
							ioctlsocket(server_Client, FIONBIO, &server_io_block);
							setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, _val1, sizeof(int));
							setsockopt(server_Client, SOL_SOCKET, SO_RCVTIMEO, (char*)&server_timeout, sizeof(server_timeout));
							screen_isUsingCompressor = 0;
							
							LOG(LOG_SUCC, "Your slave is now %s\n", inet_ntoa(server_ClientAddr.sin_addr));							
							SCL_ResetSeed();

							server_closeAllUnused();

							return SERVER_NO_ERROR;
						}
					}else{
						LOG(LOG_ERR, "Invalid id number!\n");
					}
				}
			}

			Sleep(500);
		}

		if(server_TempClient != INVALID_SOCKET)
		{
			if(whitelist_IsInUse)
			{
				char* cip = inet_ntoa(server_TempClientAddr.sin_addr);
	
				if(whitelist_IsKnown(cip) != WHITELIST_ERROR_HAS)
				{
					free(cip);
					shutdown(server_TempClient, SD_BOTH);
					closesocket(server_TempClient);
	
					continue;
				}
	
				free(cip);
			}
	
			if(blocklist_IsInUse)
			{
				char* cip = inet_ntoa(server_TempClientAddr.sin_addr);
	
				if(blocklist_IsBlocked(cip) == BLOCKLIST_ERROR_HAS)
				{
					free(cip);
					shutdown(server_TempClient, SD_BOTH);
					closesocket(server_TempClient);
	
					continue;
				}
	
				free(cip);
			}

			LOG(LOG_INFO, " ");

			if(server_ClientListSize + 1 > 99)
			{
				printf("%d  %s\n", server_ClientListSize + 1, inet_ntoa(server_TempClientAddr.sin_addr));			
			}else if(server_ClientListSize + 1 > 9){
				printf(" %d  %s\n", server_ClientListSize + 1, inet_ntoa(server_TempClientAddr.sin_addr));
			}else{
				printf("  %d  %s\n", server_ClientListSize + 1, inet_ntoa(server_TempClientAddr.sin_addr));
			}

			server_ClientList[server_ClientListSize] = server_TempClient;
			server_ClientAddrList[server_ClientListSize++] = server_TempClientAddr;
		}
	}

	ioctlsocket(server_socket, FIONBIO, &server_io_block);

	return -1;
}

int server_WaitForSlave(void)
{
	if(listen(server_socket, SOMAXCONN) != 0)
	{
		WSACleanup();

		return SERVER_ERROR_LISTEN;
	}

	ioctlsocket(server_socket, FIONBIO, &server_io_nonBlock);	

	if(whitelist_IsInUse)
	{
		int _count = whitelist_Count();

		LOG(LOG_INFO, "IPs to allow: %d\n", _count);
	}

	if(blocklist_IsInUse)
	{
		int _count = blocklist_Count();
		
		LOG(LOG_INFO, "IPs to block: %d\n", _count);
	}

	LOG(LOG_INFO, "Waiting for connection on port %d\n", server_UsingPort);
	LOG(LOG_INFO, "Hold \"ESC\" to stop listening!\n");

	while(1)
	{
		int _addrsize = sizeof(server_ClientAddr);
		
		while((server_Client = accept(server_socket, (SOCKADDR*)&server_ClientAddr, &_addrsize)) == INVALID_SOCKET)
		{
			if((GetKeyState(VK_ESCAPE) < 0))
			{
				ioctlsocket(server_socket, FIONBIO, &server_io_block);
				server_CloseConnection();

				return SERVER_ERROR_NO_SLAVE;
			}

			Sleep(500);
		}

		if(whitelist_IsInUse)
		{
			char* cip = inet_ntoa(server_ClientAddr.sin_addr);

			if(whitelist_IsKnown(cip) != WHITELIST_ERROR_HAS)
			{
				free(cip);
				shutdown(server_Client, SD_BOTH);
				closesocket(server_Client);
				server_Client = INVALID_SOCKET;

				continue;
			}

			free(cip);
		}

		if(blocklist_IsInUse)
		{
			char* cip = inet_ntoa(server_ClientAddr.sin_addr);

			if(blocklist_IsBlocked(cip) == BLOCKLIST_ERROR_HAS)
			{
				free(cip);
				shutdown(server_Client, SD_BOTH);
				closesocket(server_Client);
				server_Client = INVALID_SOCKET;

				continue;
			}

			free(cip);
		}

		char _val1[1] = { 1 };

		ioctlsocket(server_socket, FIONBIO, &server_io_block);		
		ioctlsocket(server_Client, FIONBIO, &server_io_block);
		setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, _val1, sizeof(int));		
		setsockopt(server_Client, SOL_SOCKET, SO_RCVTIMEO, (char*)&server_timeout, sizeof(server_timeout));
		screen_isUsingCompressor = 0;

		LOG(LOG_SUCC, "%s has connected!\n", inet_ntoa(server_ClientAddr.sin_addr));
		SCL_ResetSeed();

		return SERVER_NO_ERROR;
	}

	ioctlsocket(server_socket, FIONBIO, &server_io_block);

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