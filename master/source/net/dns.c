#include <windows.h>
#include "types.h"

uint64 net_Dns_GetIp(char* hostname)
{
	struct hostent *rhost = gethostbyname(hostname);

	if(!rhost)
	{
		return 0;
	}

	return *(uint64*)rhost->h_addr_list[0];
}