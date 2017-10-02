#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "geoIP.h"
#include "net/dns.h"

int geoIP_timeout = 1000;
int geoIP_IsInUse = 0;
SOCKADDR_IN geoIP_sockAddr;

int geoIP_getCountry(OUT_STRP msg, int msgLen)
{
	int i;
	int startI = 0;
	int endI = 0;
	char* pmsg = *msg;

	for(i = 0; i != msgLen - GEOIP_COUNTRYNAME_LEN; i++)
	{
		if(startI == 0)
		{
			if(strncmp(pmsg + i, GEOIP_COUNTRYNAME, GEOIP_COUNTRYNAME_LEN) == 0)
			{
				i += GEOIP_COUNTRYNAME_LEN;

				if(pmsg[i] == '<')
				{
					free(pmsg);
					*msg = NULL;
				
					return 0;
				}

				startI = i;

				continue;
			}
		}else{
			if(pmsg[i] == '<')
			{
				endI = i;

				break;
			}
		}
	}

	if(endI - startI > 1)
	{
		char* rval = malloc(endI - startI + sizeof(char));
		memset(rval, 0, endI - startI + sizeof(char));

		strncpy(rval, pmsg + startI, endI - startI);

		free(pmsg);
		*msg = rval;

		return 1;
	}

	free(pmsg);
	*msg = NULL;

	return 0;
}

int geoIP_Init(void)
{
	uint64 gip = net_Dns_GetIp(GEOIP_IP);

	if(!gip)
	{
		geoIP_IsInUse = 0;

		return GEOIP_ERROR_NO_WEB;
	}

	geoIP_sockAddr.sin_addr.s_addr = gip;
	geoIP_sockAddr.sin_family = AF_INET;
	geoIP_sockAddr.sin_port = htons(GEOIP_PORT);
	geoIP_IsInUse = 1;

	return GEOIP_NO_ERROR;
}

int geoIP_CheckIP(char* ip, OUT_STRP country)
{
	if(!ip)
	{
		*country = NULL;
		
		return GEOIP_ERROR_EMPTY_IP;
	}

	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);

	if(client == INVALID_SOCKET)
	{
		*country = NULL;

		return GEOIP_ERROR_SOCKET;
	}

	if(connect(client, (SOCKADDR*)&geoIP_sockAddr, sizeof(geoIP_sockAddr)) < 0)
	{
		*country = NULL;
		
		return GEOIP_ERROR_CONNECT;
	}

	int _val1 = 1;

	setsockopt(client, SOL_SOCKET, SO_REUSEADDR, (char*)&_val1, sizeof(_val1));
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (char*)&geoIP_timeout, sizeof(geoIP_timeout));

	int smsgLen = 156;
	char* smsg = malloc(smsgLen + sizeof(char));
	memset(smsg, 0, smsgLen + sizeof(char));
	snprintf(smsg, smsgLen, "%s%s%s%s%s", GEOIP_CMD_1, ip, GEOIP_CMD_2, GEOIP_HOST, GEOIP_CMD_3);

	if(send(client, smsg, smsgLen, 0) < 1)
	{
		shutdown(client, SD_BOTH);
		closesocket(client);
		*country = NULL;
		free(smsg);

		return GEOIP_ERROR_SEND;
	}

	free(smsg);

	int rlen = 0;
	char* rmsg = malloc(GEOIP_BUFFER);
	memset(rmsg, 0, GEOIP_BUFFER);

	if((rlen = recv(client, rmsg, GEOIP_BUFFER, 0)) < 1)
	{
		shutdown(client, SD_BOTH);
		closesocket(client);
		*country = NULL;
		free(rmsg);

		return GEOIP_ERROR_RECV;
	}

	shutdown(client, SD_BOTH);
	closesocket(client);

	if(geoIP_getCountry(&rmsg, strlen(rmsg)))
	{
		*country = rmsg;
		
		return GEOIP_NO_ERROR;
	}

	*country = NULL;

	return GEOIP_ERROR_FAILED;
}
