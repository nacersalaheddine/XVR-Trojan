#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "server/hdd/hdd.h"
#include "types.h"

int hdd_VisableSize = 20;
char hdd_Path[HDD_BUFFSIZE];

char* hdd_illegalChars = "<>\"|?*";

void hdd_Reset(void)
{
	memset(hdd_Path, 0, HDD_BUFFSIZE);
	strcpy(hdd_Path, HDD_DEF_PATH);
}

char* hdd_GetVisable(OUT_INT w)
{
	*w = 0;

	if(!hdd_VisableSize)
	{
		return hdd_Path;
	}

	int len = strlen(hdd_Path);

	if(strlen(hdd_Path) > hdd_VisableSize)
	{
		*w = 1;

		return hdd_Path + (len - hdd_VisableSize);
	}

	return hdd_Path;
}

int hdd_AppendPath(char* str)
{
	int plen = strlen(hdd_Path);
	int slen = strlen(str);

	if(plen + slen + 1 > HDD_MAX_PATH)
	{
		return 0;
	}

	if(hdd_IsFolder())
	{
		strcpy(hdd_Path + plen, str);
	}else{
		hdd_Path[plen] = '\\';
		strcpy(hdd_Path + plen + 1, str);
	}
	
	return 1;
}

int hdd_SetPath(char* str)
{
	if(strlen(str) > HDD_MAX_PATH)
	{
		return 0;
	}

	memset(hdd_Path, 0, HDD_BUFFSIZE);
	strcpy(hdd_Path, str);

	return 1;
}

int hdd_IsFolder(void)
{
	if(hdd_Path[strlen(hdd_Path) - 1] == '\\')
	{
		return 1;
	}

	return 0;
}

void hdd_Format(void)
{
	int i;
	int inDrive = 1;
	int len = strlen(hdd_Path);

	for(i = 0; i != len; i++)
	{
		if(inDrive)
		{
			if(hdd_Path[i] >= 'a' && hdd_Path[i] <= 'z')
			{
				hdd_Path[i] -= 0x20;
			}

			if(hdd_Path[i] == ':')
			{
				inDrive = 0;
			}

			continue;
		}

		if(hdd_Path[i] == '/')
		{
			hdd_Path[i] = '\\';
		}
	}

	char* buff = malloc(HDD_BUFFSIZE);
	memset(buff, 0, HDD_BUFFSIZE);

	if(GetFullPathName(hdd_Path, HDD_BUFFSIZE, buff, NULL))
	{
		memset(hdd_Path, 0, HDD_BUFFSIZE);
		strcpy(hdd_Path, buff);
	}

	free(buff);
}

int hdd_IsValidPath(char* str)
{
	int i;
	int y;
	int slen = strlen(str);
	int ilen = strlen(hdd_illegalChars);

	for(i = 0; i != slen; i++)
	{
		for(y = 0; y != ilen; y++)
		{
			if(str[i] == hdd_illegalChars[y])
			{
				return 0;
			}
		}
	}

	return 1;
}