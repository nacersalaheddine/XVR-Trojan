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
DWORD timeout = 1000;

int net_SendData(uint8* msg, int len)
{
    uint8 _msg[NET_BUFFSIZE];
    memset(_msg, 0, NET_BUFFSIZE);
    memmove(_msg + 1, msg, len);
    _msg[0] = len;
    
    int rv = send(client, _msg, NET_BUFFSIZE, 0);

    if(rv < 1)
    {
        return 0;
    }

    return rv;
}

int net_ReciveData(OUT_USTRP msg)
{
    uint8 *rbuff = malloc(NET_BUFFSIZE);
    int rv = recv(client, rbuff, NET_BUFFSIZE, 0);

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

int net_ReciveDataTimeout(OUT_USTRP msg)
{
    int rv = 0;
    int tries = 0;
    uint8* rbuff = malloc(NET_BUFFSIZE);
    
    while((rv = recv(client, rbuff, NET_BUFFSIZE, 0)) < 1)
    {
        if(tries == NET_RECV_TRIES)
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

    serverInfo.sin_addr.s_addr = dns_GetIp(NET_IP);
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
                ioctlsocket(client, FIONBIO, &ioBlockMode);
                setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

                if(net_ExecuteCmd(rmsg, strlen(rmsg)) < 1)
                {
                    net_shutdown();

                    free(rmsg);
                    return;
                }

                ioctlsocket(client, FIONBIO, &ioNonBlockMode);
            }

            free(rmsg);
        }
    }
}
