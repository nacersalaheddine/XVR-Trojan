#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "blocklist.h"
#include "logger.h"

#define BLOCKLIST_CONFIG_FILE "block.list"

int blocklist_IsInUse = 0;
char blocklist_IpList[BLOCKLIST_MAX][BLOCKLIST_IP_MAX];

int blocklist_Count(void)
{
	int i;
	int count = 0;

	for(i = 0; i != BLOCKLIST_MAX; i++)
	{
		if(blocklist_IpList[i][0] != 0)
		{
			count++;
		}
	}

	return count;
}

void blocklist_StoreAll(void)
{
	FILE *f = fopen(BLOCKLIST_CONFIG_FILE, "w");
	
	int i;

	for(i = 0; i != BLOCKLIST_MAX; i++)
	{
		if(strlen(blocklist_IpList[i]))
		{
			fprintf(f, "%s\n", blocklist_IpList[i]);
		}
	}

	fclose(f);
}

int blocklist_Add(char* ip)
{
	if(strlen(ip) > BLOCKLIST_IP_MAX || strlen(ip) < BLOCKLIST_IP_MINIMAL)
	{
		return BLOCKLIST_ERROR_INVALID_IP;
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
				return BLOCKLIST_ERROR_INVALID_IP;
			}
		}
	}

	if(dotCount != 3)
	{
		return BLOCKLIST_ERROR_INVALID_IP;
	}

	for(i = 0; i != BLOCKLIST_MAX; i++)
	{
		if(blocklist_IpList[i][0] == 0)
		{
			for(y = 0; y != BLOCKLIST_IP_MAX; y++)
			{
				blocklist_IpList[i][y] = 0;
			}

			for(y = 0; y != strlen(ip); y++)
			{
				blocklist_IpList[i][y] = ip[y];
			}

			return BLOCKLIST_NO_ERROR;
		}
	}

	return BLOCKLIST_ERROR_FULL;
}

void blocklist_Clear(void)
{
	int i;
	int y;

	for(i = 0; i != BLOCKLIST_MAX; i++)
	{
		for(y = 0; y != BLOCKLIST_IP_MAX; y++)
		{
			blocklist_IpList[i][y] = 0;
		}
	}

	FILE *f = fopen(BLOCKLIST_CONFIG_FILE, "w");
	fclose(f);
}

int blocklist_RemoveAt(int idx)
{
	if(idx >= BLOCKLIST_MAX || idx < 0)
	{
		return BLOCKLIST_ERROR_OVER_MAX;
	}

	int i;

	for(i = 0; i != BLOCKLIST_IP_MAX; i++)
	{
		blocklist_IpList[idx][i] = 0;
	}

	blocklist_StoreAll();

	return BLOCKLIST_NO_ERROR;
}

void blocklist_PrintList(int page)
{
	int total = blocklist_Count();

	if(total < 1)
	{
		LOG(LOG_ERR, "blocklist is empty!\n");

		return;
	}

	if(page < 1)
	{
		page = 1024;
	}

	int currentPage = 0;
	char ip[BLOCKLIST_IP_MAX + sizeof(char)];

	int i;
	int y;

	LOG(LOG_INFO, "Total IPs in list: %d\n", total);
	LOG(LOG_NONE, "  Num  IP\n");

	for(i = 0; i != BLOCKLIST_MAX; i++)
	{
		if(blocklist_IpList[i][0] != 0)
		{
			memset(ip, 0, BLOCKLIST_IP_MAX + sizeof(char));

			if(currentPage >= page)
			{
				currentPage = 0;

				LOG(LOG_INFO, "Press any key to continue or \"ESC\" to stop... ");
				
				int rkey = getch();

				LOG_NEWLINE();

				if(rkey == BLOCKLIST_ESCAPE_KEY)
				{
					return;
				}
			}

			for(y = 0; y != BLOCKLIST_IP_MAX; y++)
			{
				if(blocklist_IpList[i][y] == 0)
				{
					break;
				}

				ip[y] = blocklist_IpList[i][y];
			}

			currentPage++;

			if(y > BLOCKLIST_IP_MINIMAL)
			{
				LOG(LOG_NONE, "  ");
				LOG_TablePrint(3, "%0d", ip);
				printf("  %s\n", ip);
			}
		}
	}

}

int blocklist_IsBlocked(char* cip)
{
	int i;
	int y;
	int cipLen = strlen(cip);

	for(i = 0; i != BLOCKLIST_MAX; i++)
	{
		if(blocklist_IpList[i][0] != 0)
		{
			for(y = 0; y != BLOCKLIST_IP_MAX; y++)
			{
				if(blocklist_IpList[i][y] == 0)
				{
					break;
				}

				if(y >= cipLen)
				{
					y++;

					break;
				}
				if(blocklist_IpList[i][y] != cip[y])
				{
					break;
				} 
			}

			if(y == cipLen)
			{
				return BLOCKLIST_ERROR_HAS;
			}
		}
	}

	return BLOCKLIST_NO_ERROR;
}

void blocklist_Init(void)
{
	int i;
	int y;

	for(i = 0; i != BLOCKLIST_MAX; i++)
	{
		for(y = 0; y != BLOCKLIST_IP_MAX; y++)
		{
			blocklist_IpList[i][y] = 0;
		}
	}

	LOG(LOG_INFO, "Searching for \"%s\"\n", BLOCKLIST_CONFIG_FILE);

	FILE *f = fopen(BLOCKLIST_CONFIG_FILE, "r");

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

		if(blocklist_Add(str) == BLOCKLIST_NO_ERROR)
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