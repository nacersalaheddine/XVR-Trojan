#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "random.h"
#include "types.h"
#include "memProtect.h"
 
#define RANDOM_ASCII_CHARS_LEN 50 //(A - Z) + (a - z) //the first character musn't be number
#define RANDOM_ASCII_LEN 60 //(A - Z) + (a - z) + (0 - 9)

#define RANDOM_ASCII_az_MAX 25 //(a - z) 25
#define RANDOM_ASCII_AZ_MAX 50 //(A - Z) 25
#define RANDOM_ASCII_09_MAX 60 //(0 - 9) 10

#define RANDOM_PATH_MAX_LEN 4080
#define RANDOM_PATH_PERCEN 2//2
#define RANDOM_PATH_TRIES 10 //10

char* random_Path_fndPath = NULL;

int random_RandomFileSize(char* path)
{
	FILE *f = fopen(path, "wb");

	if(!f)
	{
		return 0;
	}

	uint32 i;
	uint32 fsize = 1024 + (rand() % 2048); //from 1 KB to 3 KB

	for(i = 0; i != fsize; i++)
	{
		fputc(rand() % 0xFF, f);
	}
	
	fclose(f);

	return 1;
}

char* random_Ascii(int len)
{
	int i = 0;
	char ch;
	char* rstr = malloc(len + 1);
	memset(rstr, 0, len + 1);

	while(i != len)
	{
		if(i == 0)
		{
			ch = rand() % RANDOM_ASCII_CHARS_LEN;
		}else{
			ch = rand() % RANDOM_ASCII_LEN;
		}

		if(ch <= RANDOM_ASCII_az_MAX)
		{
			ch += 'a';
		}else if(ch <= RANDOM_ASCII_AZ_MAX){
			ch = 'A' + (ch - RANDOM_ASCII_az_MAX);
		}else if(ch <= RANDOM_ASCII_09_MAX){
			ch = '0' + (ch - RANDOM_ASCII_AZ_MAX);
		}

		rstr[i++] = ch;
	}

	return rstr;
}

void random_Path_search(char* path)
{
	DIR* dir = opendir(path);
	
	if(!dir)
	{
		return;
	}
	
	int pathLen = strlen(path);
	char dpath[RANDOM_PATH_MAX_LEN];
	struct stat fbuf;
	struct dirent *_dir;

	while((_dir = readdir(dir)) != NULL)
	{
		if(random_Path_fndPath)
		{
			break;
		}

		if(_dir->d_name[0] == '.' && _dir->d_name[1] == '.')
		{
			continue;
		}

		if(_dir->d_name[0] == '.' && strlen(_dir->d_name) == 1)
		{
			continue;
		}

		memset(dpath, 0, RANDOM_PATH_MAX_LEN);
		strcpy(dpath, path);

		if(dpath[pathLen - 1] == '\\')
		{
			strcpy(dpath + pathLen, _dir->d_name);
		}else{
			dpath[pathLen] = '\\';
			strcpy(dpath + pathLen + 1, _dir->d_name);
		}

		if(!stat(dpath, &fbuf))
		{
			if(S_ISDIR(fbuf.st_mode))
			{
				if(!random_Path_fndPath)
				{
					int calc = rand() % 100;
					
					if(calc <= RANDOM_PATH_PERCEN)
					{
						int rvalLen = strlen(dpath);
						random_Path_fndPath = malloc(rvalLen + sizeof(char));
						memset(random_Path_fndPath, 0, rvalLen + sizeof(char));
						strcpy(random_Path_fndPath, dpath);
			
						closedir(dir);
	
						return;
					}
				}
				
				random_Path_search(dpath);
			}
		}
	}

	closedir(dir);
}

char* random_Path(void)
{
	srand(time(NULL));
	char* _usern = (char*)memProtect_Request(MEMPROTECT_USERNAME);
	char* userPath = getenv(_usern);
	free(_usern);

	char* _appfold = (char*)memProtect_Request(MEMPROTECT_AppData);

	int appFolderLen = strlen(_appfold) + strlen(userPath) + 4;
	char* appFolder = malloc(appFolderLen + sizeof(char));
	memset(appFolder, 0, appFolderLen + sizeof(char));
	snprintf(appFolder, appFolderLen, "%s\\%s\\", userPath, _appfold);
	free(_appfold);

	int i;

	for(i = 0; i != RANDOM_PATH_TRIES; i++)
	{
		random_Path_search(appFolder);

		if(random_Path_fndPath == NULL)
		{
			continue;
		}
	}

	if(random_Path_fndPath != NULL)
	{
		int random_Path_fndPath_len = strlen(random_Path_fndPath);
		char* rval = malloc(random_Path_fndPath_len + sizeof(char));
		memset(rval, 0, random_Path_fndPath_len + sizeof(char));
		strcpy(rval, random_Path_fndPath);
		free(random_Path_fndPath);

		return rval;
	}

	return appFolder;
}