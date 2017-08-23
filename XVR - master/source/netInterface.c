#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "net/commands.h"
#include "net/interface.h"
#include "logger.h"
#include "server.h"
#include "types.h"

int net_reciveTries = NET_RECV_TRIES;

int net_SendData(SOCKET sock, uint8* msg, int len)
{
     uint8 _msg[NET_BUFFSIZE];
     memset(_msg, 0, NET_BUFFSIZE);
     memmove(_msg + 1, msg, len);
     _msg[0] = len;

     int rv = send(sock, _msg, NET_BUFFSIZE, 0);

     if(rv < 1)
     {
          return 0;
     }

     return rv;
}

int net_ReciveData(SOCKET sock, OUT_USTRP msg)
{
     uint8 *rbuff = malloc(NET_BUFFSIZE);
     int rv = recv(sock, rbuff, NET_BUFFSIZE, 0);

     if(rv < 1)
     {
          free(rbuff);

          return 0;
     }

     int len = rbuff[0];
     uint8* buff = malloc(len + sizeof(uint8));
     memset(buff, 0, len + 1);
     memmove(buff, rbuff + 1, len);
     free(rbuff);
     *msg = buff;

     return rv;
}

int net_ReciveDataTimeout(SOCKET sock, OUT_USTRP msg)
{
     int rv = 0;
     int tries = 0;
     uint8* rbuff = malloc(NET_BUFFSIZE);
     
     while((rv = recv(sock, rbuff, NET_BUFFSIZE, 0)) < 1)
     {
          if(tries == net_reciveTries)
          {
               free(rbuff);
               
               return 0;
          }

          tries++;
     }

     int len = rbuff[0];
     uint8* buff = malloc(len + sizeof(uint8));
     memset(buff, 0, len + 1);
     memmove(buff, rbuff + 1, len);
     free(rbuff);
     *msg = buff;

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
          LOG(LOG_ERR, "Closed or lost connection with us!\n");
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