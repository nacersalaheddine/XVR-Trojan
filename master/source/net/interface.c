#include <stdlib.h>
#include <windows.h>
#include "types.h"
#include "SCL.h"
#include "server.h"
#include "net/interface.h"
#include "net/error.h"

int net_CheckConnection(void)
{
	uint8* _msg = malloc(NET_BUFFSIZE);
	memset(_msg, 0, NET_BUFFSIZE);

	SCL_Encrypt(&_msg, NET_BUFFSIZE);

	int rv = send(server_Client, (char*)_msg, NET_BUFFSIZE, 0);

	free(_msg);

	if(rv == INVALID_SOCKET)
	{
		return NET_LOST_CONNECTION;
	}

	return 1;
}

int net_SendData(uint8* msg, int len)
{
	uint8* _msg = malloc(NET_BUFFSIZE);
	memset(_msg, 0, NET_BUFFSIZE);
	memmove(_msg + 2, msg, len);
	_msg[0] = len & 0xFF;
	_msg[1] = (len >> 8) & 0xFF;

	SCL_Encrypt(&_msg, NET_BUFFSIZE);

	int sendedCount = send(server_Client, (char*)_msg, NET_BUFFSIZE, 0);

	free(_msg);

	if(sendedCount == INVALID_SOCKET)
	{
		return NET_LOST_CONNECTION;
	}

	return sendedCount;
}

int net_SendCmd(uint8* msg, int len, int cmd)
{
	uint8* _msg = malloc(NET_BUFFSIZE);
	memset(_msg, 0, NET_BUFFSIZE);
	memmove(_msg + 3, msg, len);
	len += 1;
	_msg[0] = len & 0xFF;
	_msg[1] = (len >> 8) & 0xFF;
	_msg[2] = cmd;

	SCL_Encrypt(&_msg, NET_BUFFSIZE);

	int sendedCount = send(server_Client,(char*)_msg, NET_BUFFSIZE, 0);

	free(_msg);

	if(sendedCount == INVALID_SOCKET)
	{
		return NET_LOST_CONNECTION;
	}

	return sendedCount;
}

int net_ReceiveData(OUT_USTRP msg)
{
	uint8* rbuff = malloc(NET_BUFFSIZE);
	memset(rbuff, 0, NET_BUFFSIZE);
	
	int receivedCount = recv(server_Client, (char*)rbuff, NET_BUFFSIZE, 0);

	if(receivedCount == INVALID_SOCKET || receivedCount == 0)
	{
		free(rbuff);
		*msg = NULL;

		return NET_LOST_CONNECTION;
	}

	SCL_Decrypt(&rbuff, NET_BUFFSIZE);

	int len = (rbuff[0] & 0xFF) | (rbuff[1] & 0xFF) << 8;

	if(len > NET_BUFFSIE_MAX_CONTENT)
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
	uint8* rbuff = malloc(NET_BUFFSIZE);
	memset(rbuff, 0, NET_BUFFSIZE);

	while((receivedCount = recv(server_Client, (char*)rbuff, NET_BUFFSIZE, 0)) < 1)
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

		return NET_LOST_CONNECTION;
	}

	SCL_Decrypt(&rbuff, NET_BUFFSIZE);

	int len =  (rbuff[0] & 0xFF) | (rbuff[1] & 0xFF) << 8;

	if(len > NET_BUFFSIE_MAX_CONTENT)
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