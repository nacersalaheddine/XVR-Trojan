#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include "server.h"
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"
#include "logger.h"

int serv_Activated = 0;
SOCKET serverSock = INVALID_SOCKET;
SOCKADDR_IN saddr;
SOCKADDR_IN sockAddr;
SOCKET clientSocket;

void serv_CloseConnection(void)
{
     shutdown(clientSocket, SD_BOTH);
     closesocket(clientSocket);
     clientSocket = INVALID_SOCKET;
}

int serv_CreateServer(void)
{
     WSADATA wsaData;

     if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
     {
          return SERV_WSASTARTUP;
     }

     serverSock = socket(AF_INET, SOCK_STREAM, 0);

     char _val[1] = { 1 };
     DWORD timeout = 1000;
     setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, _val, sizeof(int));
     setsockopt(serverSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

     saddr.sin_addr.s_addr = INADDR_ANY;
     saddr.sin_family = AF_INET;
     saddr.sin_port = htons(SERV_PORT);

     if(bind(serverSock, (SOCKADDR*)&saddr, sizeof(saddr)) != 0)
     {
          WSACleanup();
          return SERV_BIND;
     }

     clientSocket = INVALID_SOCKET;

     return SERV_NO_ERROR;
}

int serv_StartServer(void)
{
     int i;

     if(listen(serverSock, SOMAXCONN) != 0)
     {
          WSACleanup();

          return SERV_LISTEN;
     }

     serv_Activated = 1;
     Sleep(500);
     LOG(LOG_INFO, "Waiting for connections...\n");

     while(serv_Activated)
     {
          SOCKADDR_IN clientAddr;
          int clientAddrSize = sizeof(clientAddr);
          clientSocket = accept(serverSock, &clientAddr, &clientAddrSize);

          if(clientSocket == INVALID_SOCKET)
          {
               LOG(LOG_ERR, "[%s]Couldn't accept connection!\n", inet_ntoa(clientAddr.sin_addr));
               Sleep(10);
               continue;
          }

          if(log_PS)
          {
               LOG(LOG_INFO, "\x1b[1;30m[\x1b[1;34m%s\x1b[1;30m]\x1b[0;37mHas connected!\n", inet_ntoa(clientAddr.sin_addr));
          }else{
               LOG(LOG_INFO, "[%s]Has connected!\n", inet_ntoa(clientAddr.sin_addr));
          }

          while(1)
          {
               if(net_Execute(clientSocket) != 1)
               {
                    serv_CloseConnection();
                    
                    return SERV_LOST_CONNECTION;
               }
          }
     }

     return SERV_NO_ERROR;
}