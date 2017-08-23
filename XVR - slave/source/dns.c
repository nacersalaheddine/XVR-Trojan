#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "net/interface.h"
#include "types.h"

uint64* dns_GetIp(char* hostname)
{
	struct in_addr addr;
	struct hostent *rhost = gethostbyname(hostname);

	return *(uint64*)rhost->h_addr_list[0];
}