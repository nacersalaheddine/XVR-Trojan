#include <windows.h>
#include "types.h"

uint32 net_Dns_GetIp(char* hostname)
{
	struct hostent *rhost = gethostbyname(hostname);

	if(!rhost)
	{
		return 0;
	}

	return *(uint32*)rhost->h_addr_list[0];
}