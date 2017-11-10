#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include "types.h"
#include "SCL2.h"
#include "server.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"

int net_lastError = 0;
int net_BuffLen;
int net_MaxContLen;
int net_Ping = 0;

void net_PrintLastError(void)
{
	if(net_lastError == NET_ERROR_SEND)
	{
		LOG(LOG_ERR, "Failed to send!\n");
	}else if(net_lastError == NET_ERROR_RECV){
		LOG(LOG_ERR, "Lost connection!\n");
	}else if(net_lastError == NET_ERROR_TIMEOUT){
		LOG(LOG_ERR, "Timed out!\n");
	}else if(net_lastError == NET_ERROR_CLOSED){
		LOG(LOG_ERR, "Slave closed the connection\n");
	}
}

void net_SetBuffer(int buff)
{
	net_BuffLen = buff;
	net_MaxContLen = buff - NET_HEADER_SIZE;
}

int net_CheckConnection(void)
{
	uint8* _msg = malloc(net_BuffLen);
	memset(_msg, 0, net_BuffLen);

	SCL2_Encrypt(&_msg, net_BuffLen);

	clock_t t1; 
	t1 = clock();   
 
	int rv = net_SendData((uint8*)"\x0", 1);

	free(_msg);

	if(rv < 1)
	{
		return NET_ERROR_RECV;
	}

	if(net_ReceiveDataTimeout(&_msg, NET_RECV_TRIES) < 1)
	{
		return NET_ERROR_RECV;
	}

	clock_t t2;
	t2 = clock(); 

	net_Ping = t2 - t1;
	LOG_TitleMaster();

	free(_msg);
	
	return 1;
}

int net_SendData(uint8* msg, int len)
{
	net_lastError = 0;
	uint8* _msg = malloc(net_BuffLen);
	memset(_msg, 0, net_BuffLen);
	memmove(_msg + 2, msg, len);
	_msg[0] = len & 0xFF;
	_msg[1] = (len >> 8) & 0xFF;

	SCL2_Encrypt(&_msg, net_BuffLen);

	int sendedCount = send(server_Client, (char*)_msg, net_BuffLen, 0);

	free(_msg);

	if(sendedCount == INVALID_SOCKET)
	{
		net_lastError = NET_ERROR_SEND;

		return NET_ERROR_SEND;
	}

	return sendedCount;
}

int net_SendEmptyCmd(int cmd)
{
	net_lastError = 0;

	uint8* _msg = malloc(net_BuffLen);
	memset(_msg, 0, net_BuffLen);
	_msg[0] = 2 & 0xFF;
	_msg[1] = (2 >> 8) & 0xFF;
	_msg[2] = cmd;
	_msg[3] = ' ';

	SCL2_Encrypt(&_msg, net_BuffLen);
	
	int sendedCount = send(server_Client,(char*)_msg, net_BuffLen, 0);

	free(_msg);

	if(sendedCount == INVALID_SOCKET)
	{
		net_lastError = NET_ERROR_SEND;

		return NET_ERROR_SEND;
	}

	return sendedCount;
}

int net_SendCmd(uint8* msg, int len, int cmd)
{
	net_lastError = 0;
	uint8* _msg = malloc(net_BuffLen);
	memset(_msg, 0, net_BuffLen);
	memmove(_msg + 3, msg, len);
	len += 1;
	_msg[0] = len & 0xFF;
	_msg[1] = (len >> 8) & 0xFF;
	_msg[2] = cmd;

	SCL2_Encrypt(&_msg, net_BuffLen);

	int sendedCount = send(server_Client,(char*)_msg, net_BuffLen, 0);

	free(_msg);

	if(sendedCount == INVALID_SOCKET)
	{
		net_lastError = NET_ERROR_SEND;

		return NET_ERROR_SEND;
	}

	return sendedCount;
}

int net_ReceiveData(OUT_USTRP msg)
{
	net_lastError = 0;
	uint8* rbuff = malloc(net_BuffLen);
	memset(rbuff, 0, net_BuffLen);
	
	int receivedCount = recv(server_Client, (char*)rbuff, net_BuffLen, 0);

	if(receivedCount == INVALID_SOCKET || receivedCount == 0)
	{
		free(rbuff);
		*msg = NULL;

		if(receivedCount == 0)
		{
			net_lastError = NET_ERROR_CLOSED;
			
			return NET_ERROR_CLOSED;
		}

		net_lastError = NET_ERROR_RECV;

		return NET_ERROR_RECV;
	}

	SCL2_Decrypt(&rbuff, net_BuffLen);

	int len = (rbuff[0] & 0xFF) | (rbuff[1] & 0xFF) << 8;

	if(len > net_MaxContLen)
	{
		free(rbuff);
		*msg = NULL;
		net_lastError = NET_ERROR_RECV;

		return NET_ERROR_RECV;
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

	while((receivedCount = recv(server_Client, (char*)rbuff, net_BuffLen, 0)) < 1)
	{
		if(triesCount == tries)
		{
			free(rbuff);
			*msg = NULL;

			if(receivedCount == 0)
			{
				net_lastError = NET_ERROR_CLOSED;
				
				return NET_ERROR_CLOSED;
			}

			net_lastError = NET_ERROR_TIMEOUT;

			return NET_ERROR_TIMEOUT;
		}

		triesCount++;
	}

	if(receivedCount == INVALID_SOCKET || receivedCount == 0)
	{
		free(rbuff);
		net_lastError = NET_ERROR_RECV;

		return NET_ERROR_RECV;
	}

	SCL2_Decrypt(&rbuff, net_BuffLen);

	int len = (rbuff[0] & 0xFF) | (rbuff[1] & 0xFF) << 8;

	if(len > net_MaxContLen)
	{
		free(rbuff);
		*msg = NULL;
		net_lastError = NET_ERROR_RECV;

		return NET_ERROR_RECV;
	}

	uint8* buff = malloc(len + sizeof(uint8));
	memset(buff, 0, len + sizeof(uint8));
	memmove(buff, rbuff + 2, len);
	free(rbuff);

	*msg = buff;

	return len;
}