#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "whitelist.h"
#include "logger.h"

#define WHITELIST_CONFIG_FILE "white.list"

int whitelist_IsInUse = 0;
char whitelist_ipList[WHITELIST_MAX][WHITELIST_IP_MAX];

int whitelist_Count(void)
{
	int i;
	int count = 0;

	for(i = 0; i != WHITELIST_MAX; i++)
	{
		if(whitelist_ipList[i][0] != 0)
		{
			count++;
		}
	}

	return count;
}

void whitelist_StoreAll(void)
{
	FILE *f = fopen(WHITELIST_CONFIG_FILE, "w");
	
	int i;

	for(i = 0; i != WHITELIST_MAX; i++)
	{
		if(strlen(whitelist_ipList[i]))
		{
			fprintf(f, "%s\n", whitelist_ipList[i]);
		}
	}

	fclose(f);
}

int whitelist_Add(char* ip)
{
	if(strlen(ip) > WHITELIST_IP_MAX || strlen(ip) < WHITELIST_IP_MINIMAL)
	{
		return WHITELIST_ERROR_INVALID_IP;
	}

	int i;
	int y;
	int dotCount = 0;

	for(i = 0; i != strlen(ip); i++)
	{
		if(ip[i] < '0' || ip[i] > '9')
		{
			if(ip[i] == '.')
			{
				dotCount++;
			}else{
				return WHITELIST_ERROR_INVALID_IP;
			}
		}
	}

	if(dotCount != 3)
	{
		return WHITELIST_ERROR_INVALID_IP;
	}

	for(i = 0; i != WHITELIST_MAX; i++)
	{
		if(whitelist_ipList[i][0] == 0)
		{
			for(y = 0; y != WHITELIST_IP_MAX; y++)
			{
				whitelist_ipList[i][y] = 0;
			}

			for(y = 0; y != strlen(ip); y++)
			{
				whitelist_ipList[i][y] = ip[y];
			}

			return WHITELIST_NO_ERROR;
		}
	}

	return WHITELIST_ERROR_FULL;
}

void whitelist_Clear(void)
{
	int i;
	int y;

	for(i = 0; i != WHITELIST_MAX; i++)
	{
		for(y = 0; y != WHITELIST_IP_MAX; y++)
		{
			whitelist_ipList[i][y] = 0;
		}
	}

	FILE *f = fopen(WHITELIST_CONFIG_FILE, "w");
	fclose(f);
}

int whitelist_RemoveAt(int idx)
{
	if(idx >= WHITELIST_MAX || idx < 0)
	{
		return WHITELIST_ERROR_OVER_MAX;
	}

	int i;

	for(i = 0; i != WHITELIST_IP_MAX; i++)
	{
		whitelist_ipList[idx][i] = 0;
	}

	whitelist_StoreAll();

	return WHITELIST_NO_ERROR;
}

void whitelist_PrintList(int page)
{
	int total = whitelist_Count();

	if(total < 1)
	{
		LOG(LOG_WAR, "Whitelist is empty!\n");

		return;
	}

	if(page < 1)
	{
		page = 1024;
	}

	int currentPage = 0;
	char ip[WHITELIST_IP_MAX + sizeof(char)];

	int i;
	int y;

	LOG(LOG_INFO, "Total IPs in list: %d\n", total);
	LOG(LOG_NONE, "  Num  IP\n");

	for(i = 0; i != WHITELIST_MAX; i++)
	{
		if(whitelist_ipList[i][0] != 0)
		{
			memset(ip, 0, WHITELIST_IP_MAX + sizeof(char));

			if(currentPage >= page)
			{
				currentPage = 0;

				LOG(LOG_INFO, "Press any key to continue or \"ESC\" to stop... ");
				
				int rkey = getch();

				LOG_NEWLINE();

				if(rkey == WHITELIST_ESCAPE_KEY)
				{
					return;
				}
			}

			for(y = 0; y != WHITELIST_IP_MAX; y++)
			{
				if(whitelist_ipList[i][y] == 0)
				{
					break;
				}

				ip[y] = whitelist_ipList[i][y];
			}

			currentPage++;

			if(y > WHITELIST_IP_MINIMAL)
			{
				LOG(LOG_NONE, "  ");
				LOG_TablePrint(3, "%0d", ip);
				printf("  %s\n", ip);
			}
		}
	}

}

int whitelist_IsKnown(char* cip)
{
	int i;
	int y;
	int cipLen = strlen(cip);

	for(i = 0; i != WHITELIST_MAX; i++)
	{
		if(whitelist_ipList[i][0] != 0)
		{
			for(y = 0; y != WHITELIST_IP_MAX; y++)
			{
				if(whitelist_ipList[i][y] == 0)
				{
					break;
				}

				if(y >= cipLen)
				{
					y++;

					break;
				}
				if(whitelist_ipList[i][y] != cip[y])
				{
					break;
				} 
			}

			if(y == cipLen)
			{
				return WHITELIST_ERROR_HAS;
			}
		}
	}

	return WHITELIST_NO_ERROR;
}

void whitelist_Init(void)
{
	int i;
	int y;

	for(i = 0; i != WHITELIST_MAX; i++)
	{
		for(y = 0; y != WHITELIST_IP_MAX; y++)
		{
			whitelist_ipList[i][y] = 0;
		}
	}

	LOG(LOG_INFO, "Searching for \"%s\"\n", WHITELIST_CONFIG_FILE);

	FILE *f = fopen(WHITELIST_CONFIG_FILE, "r");

	if(!f)
	{
		LOG(LOG_TABLE, "Not found!\n");

		return;
	}

	int addedCount = 0;
	int errorCount = 0;
	char str[256];

	while(fgets(str, 256, f) != NULL)
	{
		if(str[0] == '#')
		{
			continue;
		}

		for(i = 0; i != strlen(str); i++)
		{
			if(str[i] == '\n')
			{
				str[i] = 0;
				
				break;
			}
		}

		if(whitelist_Add(str) == WHITELIST_NO_ERROR)
		{
			addedCount++;
		}else{
			errorCount++;
		}
	}

	fclose(f);

	LOG(LOG_TABLE, "IPs added: %d\n", addedCount);
	LOG(LOG_TABLE, "IPs that have error: %d\n", errorCount);
}