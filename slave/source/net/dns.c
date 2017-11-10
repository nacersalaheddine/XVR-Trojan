#include <windows.h>
#include "net/interface.h"
#include "types.h"

uint32 net_Dns_GetIp(char* hostname)
{
	struct hostent *rhost = net_lib_gethostbyname(hostname);

	if(!rhost)
	{
		exit(0);
	}

	return *(uint32*)rhost->h_addr_list[0];
}