#include <stdlib.h>
#include <windows.h>
#include "types.h"
#include "SCL2.h"
#include "client.h"
#include "net/interface.h"
#include "net/error.h"
#include "loadLibrary.h"

int net_BuffLen;
int net_MaxContLen;

HMODULE net_lib;
__WSAStartup net_lib_WSAStartup;
__WSACleanup net_lib_WSACleanup;
__recv net_lib_recv;
__send net_lib_send;
__gethostbyname net_lib_gethostbyname;
__closesocket net_lib_closesocket;
__shutdown net_lib_shutdown;
__setsockopt net_lib_setsockopt;
__ioctlsocket net_lib_ioctlsocket;
__socket net_lib_socket;
__select net_lib_select;
__htons net_lib_htons;
__connect net_lib_connect;
_WSAFDIsSet net_lib_WSAFDIsSet;

int net_Init(void)
{
	net_lib = loadLibrary_Load(lib_Ws2_32);

	if(!net_lib)
	{
		return 0;
	}

	net_lib_WSAStartup = (__WSAStartup)loadLibrary_LoadFunc(net_lib, lib_WSAStartup);
	net_lib_WSACleanup = (__WSACleanup)loadLibrary_LoadFunc(net_lib, lib_WSACleanup);
	net_lib_recv = (__recv)loadLibrary_LoadFunc(net_lib, lib_recv);
	net_lib_send = (__send)loadLibrary_LoadFunc(net_lib, lib_send);
	net_lib_gethostbyname = (__gethostbyname)loadLibrary_LoadFunc(net_lib, lib_gethostbyname);
	net_lib_closesocket = (__closesocket)loadLibrary_LoadFunc(net_lib, lib_closesocket);
	net_lib_shutdown = (__shutdown)loadLibrary_LoadFunc(net_lib, lib_shutdown);
	net_lib_setsockopt = (__setsockopt)loadLibrary_LoadFunc(net_lib, lib_setsockopt);
	net_lib_ioctlsocket = (__ioctlsocket)loadLibrary_LoadFunc(net_lib, lib_ioctlsocket);
	net_lib_socket = (__socket)loadLibrary_LoadFunc(net_lib, lib_socket);
	net_lib_select = (__select)loadLibrary_LoadFunc(net_lib, lib_select);
	net_lib_htons = (__htons)loadLibrary_LoadFunc(net_lib, lib_htons);
	net_lib_connect = (__connect)loadLibrary_LoadFunc(net_lib, lib_connect);
	net_lib_WSAFDIsSet = (_WSAFDIsSet)loadLibrary_LoadFunc(net_lib, lib_WSAFDIsSet);

	return 1;
}

void net_SetBuffer(int buff)
{
	net_BuffLen = buff;
	net_MaxContLen = buff - NET_HEADER_SIZE;
}

int net_SendData(uint8* msg, int len)
{
	uint8* _msg = malloc(net_BuffLen);
	memset(_msg, 0, net_BuffLen);
	memmove(_msg + 2, msg, len);
	_msg[0] = len & 0xFF;
	_msg[1] = (len >> 8) & 0xFF;
	
	SCL2_Encrypt(&_msg, net_BuffLen);

	int sendedCount = net_lib_send(client_Socket, (char*)_msg, net_BuffLen, 0);

	free(_msg);

	if(sendedCount == INVALID_SOCKET)
	{
		return NET_LOST_CONNECTION;
	}

	return sendedCount;
}

int net_SendEmptyCmd(int cmd)
{
	uint8* _msg = malloc(net_BuffLen);
	memset(_msg, 0, net_BuffLen);
	_msg[0] = 2 & 0xFF;
	_msg[1] = (2 >> 8) & 0xFF;
	_msg[2] = cmd;
	_msg[3] = ' ';

	SCL2_Encrypt(&_msg, net_BuffLen);
	
	int sendedCount = net_lib_send(client_Socket,(char*)_msg, net_BuffLen, 0);

	free(_msg);

	if(sendedCount == INVALID_SOCKET)
	{
		return NET_LOST_CONNECTION;
	}

	return sendedCount;
}

int net_SendCmd(uint8* msg, int len, int cmd)
{
	uint8* _msg = malloc(net_BuffLen);
	memset(_msg, 0, net_BuffLen);
	memmove(_msg + 3, msg, len);

	len += 1;

	_msg[0] = len & 0xFF;
	_msg[1] = (len >> 8) & 0xFF;
	_msg[2] = cmd;

	SCL2_Encrypt(&_msg, net_BuffLen);

	int sendedCount = net_lib_send(client_Socket, (char*)_msg, net_BuffLen, 0);

	free(_msg);

	if(sendedCount == INVALID_SOCKET)
	{
		return NET_LOST_CONNECTION;
	}

	return sendedCount;
}

int net_ReceiveData(OUT_USTRP msg)
{
	uint8* rbuff = malloc(net_BuffLen);
	memset(rbuff, 0, net_BuffLen);
	
	int receivedCount = net_lib_recv(client_Socket, (char*)rbuff, net_BuffLen, 0);

	if(receivedCount == INVALID_SOCKET || receivedCount == 0)
	{
		free(rbuff);
		*msg = NULL;

		return NET_LOST_CONNECTION;
	}

	SCL2_Decrypt(&rbuff, net_BuffLen);

	int len = (rbuff[0] & 0xFF) | (rbuff[1] & 0xFF) << 8;

	if(len > net_MaxContLen)
	{
		free(rbuff);
		*msg = NULL;

		return NET_LOST_CONNECTION;
	}

	uint8* buff = malloc(len + sizeof(uint8));
	memset(buff, 0, len + sizeof(uint8));
	memmove(buff, rbuff + 2, len);
	free(rbuff);

	*msg = buff;

	return len;
}

int net_ReceiveDataTimeout(OUT_USTRP msg, int tries)
{
	int triesCount = 0;
	int receivedCount = 0;
	uint8* rbuff = malloc(net_BuffLen);
	memset(rbuff, 0, net_BuffLen);

	while((receivedCount = net_lib_recv(client_Socket, (char*)rbuff, net_BuffLen, 0)) < 1)
	{
		if(triesCount == tries)
		{
			free(rbuff);
			*msg = NULL;

			return NET_TIMED_OUT;
		}

		triesCount++;
	}

	if(receivedCount == INVALID_SOCKET || receivedCount == 0)
	{
		free(rbuff);
		*msg = NULL;

		return NET_LOST_CONNECTION;
	}

	SCL2_Decrypt(&rbuff, net_BuffLen);

	int len = (rbuff[0] & 0xFF) | (rbuff[1] & 0xFF) << 8;

	if(len > net_MaxContLen)
	{
		free(rbuff);
		*msg = NULL;

		return NET_LOST_CONNECTION;
	}

	uint8* buff = malloc(len + sizeof(uint8));
	memset(buff, 0, len + sizeof(uint8));
	memmove(buff, rbuff + 2, len);
	free(rbuff);

	*msg = buff;

	return len;
}